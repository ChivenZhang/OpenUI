/*=================================================
* Copyright © 2020-2026 ChivenZhang.
* All Rights Reserved.
* =====================Note=========================
*
*
* ====================History=======================
* Created by chivenzhang@gmail.com.
*
* =================================================*/
#ifdef OPENUI_ENABLE_OPENRT
#include "OpenRTPainter.h"
#include "OpenRTDevice.h"
#include <OpenUI/UICanvas.h>
#include <nanovg.h>
#include <nanovg_rt.h>
#include <cmath>
#include <cstring>
#include <algorithm>

// UIPainter implemented on top of NanoVG (OpenRT backend).
//
// The NVGcontext is owned by OpenRTDevice; the painter only borrows it.
// A paint session runs between setTarget(target) [nvgBeginFrame] and the first
// getTarget() afterwards [nvgEndFrame]. When the target is a GPU image whose
// Handle points at an rt_texture_t, NanoVG's offscreen result is copied into it.

struct OpenRTPainterPrivate : UIPrivate
{
	NVGcontext* Context = nullptr; // owned by OpenRTDevice
	UIPen Pen;
	UIFont Font;
	UIBrush Brush;
	UIRect Scissor, Viewport;
	bool EnableClip = true;
	bool InFrame = false;
	UIImageRaw Target = nullptr;
	UIList<int> FrameImages; // NanoVG images created during the current frame
};
#define PRIVATE() ((OpenRTPainterPrivate*) m_Private)
#define CONTEXT() (PRIVATE()->Context)

// ====================================================================
// Helpers

static constexpr float NVG_KAPPA = 0.5522847493f; // 4/3 * (sqrt(2) - 1)

static NVGcolor nvg_color(UIColor const& color)
{
	return nvgRGBAf(color.R, color.G, color.B, color.A);
}

// Applies the clip rectangle. Returns false when the clip is empty (nothing to draw).
static bool nvg_applyClip(NVGcontext* vg, UIRect const& rect, bool enable)
{
	if (!enable || std::isnan(rect.X) || std::isnan(rect.Y) || std::isnan(rect.W) || std::isnan(rect.H))
	{
		nvgResetScissor(vg);
		return true;
	}
	if (rect.W <= 0 || rect.H <= 0) return false;
	nvgScissor(vg, rect.X, rect.Y, rect.W, rect.H);
	return true;
}

static void nvg_applyPen(NVGcontext* vg, UIPen const& pen)
{
	nvgStrokeColor(vg, nvg_color(pen.Color));
	nvgStrokeWidth(vg, pen.Width);
	nvgMiterLimit(vg, pen.MiterLimit);
	switch (pen.CapStyle)
	{
	case UIPen::FlatCap: nvgLineCap(vg, NVG_BUTT); break;
	case UIPen::SquareCap: nvgLineCap(vg, NVG_SQUARE); break;
	case UIPen::RoundCap: nvgLineCap(vg, NVG_ROUND); break;
	}
	switch (pen.JoinStyle)
	{
	case UIPen::MiterJoin: nvgLineJoin(vg, NVG_MITER); break;
	case UIPen::BevelJoin: nvgLineJoin(vg, NVG_BEVEL); break;
	case UIPen::RoundJoin: nvgLineJoin(vg, NVG_ROUND); break;
	}
	// Dash patterns are not supported by NanoVG; all pens render as solid lines.
}

static void nvg_applyBrush(NVGcontext* vg, UIBrush const& brush)
{
	// Only solid colour brushes are supported; other patterns fall back to the brush colour.
	nvgFillColor(vg, nvg_color(brush.Color));
}

static void nvg_applyFont(NVGcontext* vg, UIFont const& font)
{
	const char* face = "sans";
	if (font.Weight >= UIFont::WeightDemiBold) face = "sans-bold";
	else if (font.Weight <= UIFont::WeightLight) face = "sans-light";
	if (!font.Family.empty() && nvgFindFont(vg, font.Family.c_str()) != -1) face = font.Family.c_str();
	if (nvgFindFont(vg, face) == -1) face = "sans";
	nvgFontFace(vg, face);
	nvgFontSize(vg, (float)font.Size);
	nvgTextLetterSpacing(vg, (float)font.Spacing);
	nvgTextLineHeight(vg, font.LineSpacing > 0 ? font.LineSpacing : 1.0f);
	nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
}

// Rounded rectangle with independent x / y radii (NanoVG only offers a single radius).
static void nvg_roundedRectPath(NVGcontext* vg, float x, float y, float w, float h, float rx, float ry)
{
	rx = std::clamp(rx, 0.0f, w * 0.5f);
	ry = std::clamp(ry, 0.0f, h * 0.5f);
	if (rx < 0.1f || ry < 0.1f)
	{
		nvgRect(vg, x, y, w, h);
		return;
	}
	nvgMoveTo(vg, x, y + ry);
	nvgLineTo(vg, x, y + h - ry);
	nvgBezierTo(vg, x, y + h - ry * (1 - NVG_KAPPA), x + rx * (1 - NVG_KAPPA), y + h, x + rx, y + h);
	nvgLineTo(vg, x + w - rx, y + h);
	nvgBezierTo(vg, x + w - rx * (1 - NVG_KAPPA), y + h, x + w, y + h - ry * (1 - NVG_KAPPA), x + w, y + h - ry);
	nvgLineTo(vg, x + w, y + ry);
	nvgBezierTo(vg, x + w, y + ry * (1 - NVG_KAPPA), x + w - rx * (1 - NVG_KAPPA), y, x + w - rx, y);
	nvgLineTo(vg, x + rx, y);
	nvgBezierTo(vg, x + rx * (1 - NVG_KAPPA), y, x, y + ry * (1 - NVG_KAPPA), x, y + ry);
	nvgClosePath(vg);
}

// ====================================================================
// Text layout

struct OpenRTTextRow
{
	const char* Start;
	const char* End;
	float Width;
};

struct OpenRTTextLayout
{
	UIRect Rect; // aligned text rectangle in target coordinates
	float LineHeight = 0;
	float AlignFactor = 0; // 0 = left, 0.5 = centre, 1 = right (per-row offset inside Rect)
	UIList<OpenRTTextRow> Rows;

	float rowX(size_t row) const { return Rect.X + (Rect.W - Rows[row].Width) * AlignFactor; }
	float rowY(size_t row) const { return Rect.Y + LineHeight * row; }
};

static OpenRTTextLayout nvg_layoutText(NVGcontext* vg, UIFont const& font, float x, float y, float width, float height, UIString const& text)
{
	OpenRTTextLayout layout;
	nvg_applyFont(vg, font);

	float ascender, descender, lineh;
	nvgTextMetrics(vg, &ascender, &descender, &lineh);
	layout.LineHeight = lineh;

	const char* start = text.c_str();
	const char* end = start + text.size();
	float textWidth = 0;
	if (font.LineWrap && width > 0)
	{
		NVGtextRow rows[16];
		const char* cursor = start;
		int nrows;
		while ((nrows = nvgTextBreakLines(vg, cursor, end, width, rows, 16)) > 0)
		{
			for (int i = 0; i < nrows; ++i)
			{
				layout.Rows.push_back({rows[i].start, rows[i].end, rows[i].width});
				textWidth = std::max(textWidth, rows[i].width);
			}
			cursor = rows[nrows - 1].next;
		}
	}
	else
	{
		textWidth = nvgTextBounds(vg, 0, 0, start, end, nullptr);
		layout.Rows.push_back({start, end, textWidth});
	}
	if (layout.Rows.empty()) layout.Rows.push_back({start, end, 0.0f});
	float textHeight = lineh * (float)layout.Rows.size();

	UIRect rect{x, y, textWidth, textHeight};
	if (width > 0)
	{
		if (font.Align & UIFont::AlignRight) { rect.X = x + width - textWidth; layout.AlignFactor = 1.0f; }
		else if (font.Align & UIFont::AlignCenter) { rect.X = x + std::round((width - textWidth) * 0.5f); layout.AlignFactor = 0.5f; }
	}
	if (height > 0)
	{
		if (font.Align & UIFont::AlignBottom) rect.Y = y + height - textHeight;
		else if (font.Align & UIFont::AlignVCenter) rect.Y = y + std::round((height - textHeight) * 0.5f);
	}
	layout.Rect = rect;
	return layout;
}

// Caret rectangle for a byte offset into the text.
static UIRect nvg_cursorRect(NVGcontext* vg, OpenRTTextLayout const& layout, UIString const& text, int cursor)
{
	const char* base = text.c_str();
	cursor = std::clamp<int>(cursor, 0, (int)text.size());
	const char* pos = base + cursor;

	size_t row = layout.Rows.size() - 1;
	for (size_t i = 0; i < layout.Rows.size(); ++i)
	{
		const char* next = (i + 1 < layout.Rows.size()) ? layout.Rows[i + 1].Start : nullptr;
		if (next == nullptr || pos < next) { row = i; break; }
	}
	auto const& r = layout.Rows[row];
	const char* clamped = std::clamp(pos, r.Start, r.End);
	float advance = nvgTextBounds(vg, 0, 0, r.Start, clamped, nullptr);
	return UIRect{layout.rowX(row) + advance, layout.rowY(row), 0, layout.LineHeight};
}

// Byte offset of the caret closest to a point.
static int nvg_hitTest(NVGcontext* vg, OpenRTTextLayout const& layout, UIString const& text, float posX, float posY)
{
	const char* base = text.c_str();
	size_t row = 0;
	if (layout.LineHeight > 0)
	{
		auto index = (int)std::floor((posY - layout.Rect.Y) / layout.LineHeight);
		row = (size_t)std::clamp<int>(index, 0, (int)layout.Rows.size() - 1);
	}
	auto const& r = layout.Rows[row];
	if (r.End <= r.Start) return (int)(r.Start - base);

	UIList<NVGglyphPosition> glyphs((size_t)(r.End - r.Start) + 1);
	float rowX = layout.rowX(row);
	int count = nvgTextGlyphPositions(vg, rowX, 0, r.Start, r.End, glyphs.data(), (int)glyphs.size());
	for (int i = 0; i < count; ++i)
	{
		float x0 = glyphs[i].x;
		float x1 = (i + 1 < count) ? glyphs[i + 1].x : rowX + r.Width;
		if (posX < (x0 + x1) * 0.5f) return (int)(glyphs[i].str - base);
	}
	return (int)(r.End - base);
}

// ====================================================================

OpenRTPainter::OpenRTPainter(UICanvasRaw canvas, int width, int height)
    :
    m_Canvas(canvas)
{
	m_Private = new OpenRTPainterPrivate;
	if (auto device = UICast<OpenRTDevice>(canvas->getDevice()))
	{
		PRIVATE()->Context = device->getNanoVG();
	}
}

OpenRTPainter::~OpenRTPainter()
{
	auto vg = CONTEXT();
	if (vg)
	{
		if (PRIVATE()->InFrame) nvgCancelFrame(vg);
		for (auto image : PRIVATE()->FrameImages) nvgDeleteImage(vg, image);
	}
	delete m_Private; m_Private = nullptr;
}

UICanvasRaw OpenRTPainter::getCanvas() const
{
    return m_Canvas;
}

UIImageRaw OpenRTPainter::getTarget() const
{
	auto vg = CONTEXT();
	auto target = PRIVATE()->Target;
	if (vg && target && PRIVATE()->InFrame)
	{
		// End paint: flush NanoVG into its offscreen target.
		nvgEndFrame(vg);
		PRIVATE()->InFrame = false;
		for (auto image : PRIVATE()->FrameImages) nvgDeleteImage(vg, image);
		PRIVATE()->FrameImages.clear();

		// Copy the result into the GPU target (Handle is an rt_texture_t*).
		if (target->Format == UIImage::GPUByte && target->Handle)
		{
			auto src = rtGetTargetRT(vg);
			auto& dst = *(rt_texture_t*)(uintptr_t)target->Handle;
			if (src.handle && dst.handle)
			{
				rt_size_t size{std::min(src.width, dst.width), std::min(src.height, dst.height), 1};
				rt_pass_transfer_t pass;
				rt_begin_transfer(pass);
				rt_copy_texture({.texture = src}, {.texture = dst}, size);
				rt_end_transfer(pass);
			}
		}
	}
    return target;
}

void OpenRTPainter::setTarget(UIImageRaw value)
{
	auto vg = CONTEXT();
	if (vg && PRIVATE()->InFrame)
	{
		// A previous session was never finished; drop it.
		nvgCancelFrame(vg);
		PRIVATE()->InFrame = false;
		for (auto image : PRIVATE()->FrameImages) nvgDeleteImage(vg, image);
		PRIVATE()->FrameImages.clear();
	}
	PRIVATE()->Target = value;
	if (vg && value && value->Width && value->Height)
	{
		// Begin paint.
		auto density = m_Canvas ? m_Canvas->getConfig().PixelDensity : 1.0f;
		nvgBeginFrame(vg, (float)value->Width, (float)value->Height, density > 0 ? density : 1.0f);
		PRIVATE()->InFrame = true;
	}
}

UIRect OpenRTPainter::boundingRect(float x, float y, float width, float height, UIString const& text, float cursor, UIRectRaw cursorRect)
{
	auto vg = CONTEXT();
	if (vg == nullptr) return {};
	auto layout = nvg_layoutText(vg, PRIVATE()->Font, x, y, width, height, text);
	if (cursorRect) (*cursorRect) = nvg_cursorRect(vg, layout, text, (int)cursor);
	return layout.Rect;
}

UIRect OpenRTPainter::boundingRect(float x, float y, float width, float height, UIString const& text, float posX, float posY, int* cursor, UIRectRaw cursorRect)
{
	auto vg = CONTEXT();
	if (vg == nullptr) return {};
	auto layout = nvg_layoutText(vg, PRIVATE()->Font, x, y, width, height, text);
	auto index = nvg_hitTest(vg, layout, text, posX, posY);
	if (cursor) (*cursor) = index;
	if (cursorRect) (*cursorRect) = nvg_cursorRect(vg, layout, text, index);
	return layout.Rect;
}

void OpenRTPainter::drawImage(float x, float y, UIImage image, float sx, float sy, float sw, float sh)
{
	auto vg = CONTEXT();
	if (vg == nullptr || !PRIVATE()->InFrame) return;
	if (PRIVATE()->Brush.Style == UIBrush::NoBrush) return;
	if (image.Width == 0 || image.Height == 0) return;

	// Only CPU RGBA8 images are supported; GPU handles would need an import path in nanovg_rt.
	int handle = -1;
	if (image.Format == UIImage::Byte && image.Pixels && image.Channel == 4)
	{
		auto tight = image.Width * 4;
		if (image.Stride == 0 || image.Stride == tight)
		{
			handle = nvgCreateImageRGBA(vg, (int)image.Width, (int)image.Height, 0, (const unsigned char*)image.Pixels);
		}
		else
		{
			UIList<uint8_t> packed((size_t)tight * image.Height);
			for (uint32_t row = 0; row < image.Height; ++row)
				memcpy(packed.data() + (size_t)row * tight, (const uint8_t*)image.Pixels + (size_t)row * image.Stride, tight);
			handle = nvgCreateImageRGBA(vg, (int)image.Width, (int)image.Height, 0, packed.data());
		}
	}
	if (handle == -1) return;
	PRIVATE()->FrameImages.push_back(handle); // deleted after nvgEndFrame

	if (sw < 0) sw = (float)image.Width - sx;
	if (sh < 0) sh = (float)image.Height - sy;
	if (sw <= 0 || sh <= 0) return;

	if (!nvg_applyClip(vg, PRIVATE()->Scissor, PRIVATE()->EnableClip)) return;
	auto paint = nvgImagePattern(vg, x - sx, y - sy, (float)image.Width, (float)image.Height, 0.0f, handle, PRIVATE()->Brush.Color.A);
	nvgBeginPath(vg);
	nvgRect(vg, x, y, sw, sh);
	nvgFillPaint(vg, paint);
	nvgFill(vg);
}

void OpenRTPainter::drawLine(float x1, float y1, float x2, float y2)
{
	auto vg = CONTEXT();
	if (vg == nullptr || !PRIVATE()->InFrame) return;
	if (PRIVATE()->Pen.Style != UIPen::NoPen)
	{
		if (!nvg_applyClip(vg, PRIVATE()->Scissor, PRIVATE()->EnableClip)) return;
		nvg_applyPen(vg, PRIVATE()->Pen);
		nvgBeginPath(vg);
		nvgMoveTo(vg, x1, y1);
		nvgLineTo(vg, x2, y2);
		nvgStroke(vg);
	}
}

void OpenRTPainter::drawLines(UIListView<UILine> lines)
{
	auto vg = CONTEXT();
	if (vg == nullptr || !PRIVATE()->InFrame) return;
	if (PRIVATE()->Pen.Style != UIPen::NoPen && !lines.empty())
	{
		if (!nvg_applyClip(vg, PRIVATE()->Scissor, PRIVATE()->EnableClip)) return;
		nvg_applyPen(vg, PRIVATE()->Pen);
		nvgBeginPath(vg);
		for (size_t i = 0; i < lines.size(); ++i)
		{
			nvgMoveTo(vg, lines[i].P0.X, lines[i].P0.Y);
			nvgLineTo(vg, lines[i].P1.X, lines[i].P1.Y);
		}
		nvgStroke(vg);
	}
}

void OpenRTPainter::drawPoint(float x, float y)
{
	drawRect(x, y, 1, 1);
}

void OpenRTPainter::drawPoints(UIListView<UIPoint> points)
{
	for (size_t i = 0; i < points.size(); ++i)
	{
		drawRect(points[i].X, points[i].Y, 1, 1);
	}
}

void OpenRTPainter::drawRect(float x, float y, float width, float height)
{
	auto vg = CONTEXT();
	if (vg == nullptr || !PRIVATE()->InFrame) return;
	if (width <= 0 || height <= 0) return;
	if (!nvg_applyClip(vg, PRIVATE()->Scissor, PRIVATE()->EnableClip)) return;
	if (PRIVATE()->Brush.Style != UIBrush::NoBrush)
	{
		nvg_applyBrush(vg, PRIVATE()->Brush);
		nvgBeginPath(vg);
		nvgRect(vg, x, y, width, height);
		nvgFill(vg);
	}
	if (PRIVATE()->Pen.Style != UIPen::NoPen)
	{
		nvg_applyPen(vg, PRIVATE()->Pen);
		nvgBeginPath(vg);
		nvgRect(vg, x, y, width, height);
		nvgStroke(vg);
	}
}

void OpenRTPainter::drawRects(UIListView<UIRect> rects)
{
	for (size_t i = 0; i < rects.size(); ++i)
	{
		drawRect(rects[i].X, rects[i].Y, rects[i].W, rects[i].H);
	}
}

void OpenRTPainter::drawRoundedRect(float x, float y, float width, float height, float xRadius, float yRadius)
{
	auto vg = CONTEXT();
	if (vg == nullptr || !PRIVATE()->InFrame) return;
	if (width <= 0 || height <= 0) return;
	if (!nvg_applyClip(vg, PRIVATE()->Scissor, PRIVATE()->EnableClip)) return;
	if (PRIVATE()->Brush.Style != UIBrush::NoBrush)
	{
		nvg_applyBrush(vg, PRIVATE()->Brush);
		nvgBeginPath(vg);
		nvg_roundedRectPath(vg, x, y, width, height, xRadius, yRadius);
		nvgFill(vg);
	}
	if (PRIVATE()->Pen.Style != UIPen::NoPen)
	{
		nvg_applyPen(vg, PRIVATE()->Pen);
		nvgBeginPath(vg);
		nvg_roundedRectPath(vg, x, y, width, height, xRadius, yRadius);
		nvgStroke(vg);
	}
}

void OpenRTPainter::drawText(float x, float y, float width, float height, const UIString& text, UIRectRaw boundingRect, float cursor, UIRectRaw cursorRect)
{
	auto vg = CONTEXT();
	if (vg == nullptr) return;

	auto layout = nvg_layoutText(vg, PRIVATE()->Font, x, y, width, height, text);
	if (boundingRect) (*boundingRect) = layout.Rect;
	if (cursorRect) (*cursorRect) = nvg_cursorRect(vg, layout, text, (int)cursor);

	if (!PRIVATE()->InFrame) return;
	if (PRIVATE()->Brush.Style == UIBrush::NoBrush) return;
	if (!nvg_applyClip(vg, PRIVATE()->Scissor, PRIVATE()->EnableClip)) return;

	nvgFillColor(vg, nvg_color(PRIVATE()->Brush.Color));
	for (size_t i = 0; i < layout.Rows.size(); ++i)
	{
		auto const& row = layout.Rows[i];
		if (row.End <= row.Start) continue;
		nvgText(vg, layout.rowX(i), layout.rowY(i), row.Start, row.End);
	}
}

UIPen const& OpenRTPainter::getPen() const
{
	return PRIVATE()->Pen;
}

void OpenRTPainter::setPen(const UIPen& pen)
{
	PRIVATE()->Pen = pen;
}

UIBrush const& OpenRTPainter::getBrush() const
{
	return PRIVATE()->Brush;
}

void OpenRTPainter::setBrush(const UIBrush& brush)
{
	PRIVATE()->Brush = brush;
}

UIFont const& OpenRTPainter::getFont() const
{
	return PRIVATE()->Font;
}

void OpenRTPainter::setFont(const UIFont& font)
{
	PRIVATE()->Font = font;
}

void OpenRTPainter::setClipping(bool enable)
{
	PRIVATE()->EnableClip = enable;
}

void OpenRTPainter::setClipRect(float x, float y, float width, float height)
{
	PRIVATE()->Scissor = { x, y, width, height };
}

void OpenRTPainter::setViewport(float x, float y, float width, float height)
{
	PRIVATE()->Viewport = { x, y, width, height };
}

void OpenRTPainter::skew(float sh, float sv)
{
	auto vg = CONTEXT();
	if (vg == nullptr || !PRIVATE()->InFrame) return;
	nvgSkewX(vg, nvgDegToRad(sh));
	nvgSkewY(vg, nvgDegToRad(sv));
}

void OpenRTPainter::rotate(float angle)
{
	auto vg = CONTEXT();
	if (vg == nullptr || !PRIVATE()->InFrame) return;
	nvgRotate(vg, nvgDegToRad(angle));
}

void OpenRTPainter::scale(float dx, float dy)
{
	auto vg = CONTEXT();
	if (vg == nullptr || !PRIVATE()->InFrame) return;
	nvgScale(vg, dx, dy);
}

void OpenRTPainter::translate(float dx, float dy)
{
	auto vg = CONTEXT();
	if (vg == nullptr || !PRIVATE()->InFrame) return;
	nvgTranslate(vg, dx, dy);
}

#endif
