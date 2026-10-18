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
#include <OpenUI/UICanvas.h>
#include <nanovg.h>
#include <nanovg_rt.h>
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <pango/pangoft2.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <functional>

// UIPainter implemented on top of nanovg (OpenRT backend).
//
// The painter owns its NVGcontext.
// A paint session runs between setTarget(target) [nvgBeginFrame] and
// setTarget(nullptr) [nvgEndFrame]. Each GPU target keeps one NVGtarget:
// the image's colour texture plus a matching stencil owned here. That group is
// passed to nvgBeginFrame.

struct OpenRTNVGTarget
{
	rt_texture_t stencil = {};
	NVGtarget target = {};
};

// One uploaded glyph texture. Image < 0 means the glyph has no bitmap (for example a space).
struct OpenRTGlyphImage
{
	int Image = -1;
	int Left = 0;
	int Top = 0;
	int Width = 0;
	int Height = 0;
};

// Face pointer is the PangoFcFont's FreeType face. PixelSize keeps resized faces apart.
// Rgb is the baked text colour; colour emoji still keys on it so lookup does not reload the glyph.
struct OpenRTGlyphKey
{
	FT_Face Face = nullptr;
	FT_UInt Index = 0;
	unsigned PixelSize = 0;
	uint32_t Rgb = 0;

	bool operator<(OpenRTGlyphKey const& other) const
	{
		if (Face != other.Face) return std::less<FT_Face>{}(Face, other.Face);
		if (Index != other.Index) return Index < other.Index;
		if (PixelSize != other.PixelSize) return PixelSize < other.PixelSize;
		return Rgb < other.Rgb;
	}
};

struct OpenRTPainterPrivate : UIPrivate
{
	NVGcontext* Context = nullptr;
	UIPen Pen;
	UIFont Font;
	UIBrush Brush;
	UIRect Scissor, Viewport;
	bool EnableClip = true;
	bool InFrame = false;
	UIImageRaw Target = nullptr;
	UIList<int> Images; // nanovg images created during the current frame
	UIMap<rt_texture_t*, OpenRTNVGTarget> Targets; // one NVGtarget group per colour attachment
	PangoFontMap* FontMap = nullptr;
	PangoLayout* Layout = nullptr;
	UIMap<OpenRTGlyphKey, OpenRTGlyphImage> Glyphs;
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
	// Dash patterns are not supported by nanovg; all pens render as solid lines.
}

static void nvg_applyBrush(NVGcontext* vg, UIBrush const& brush)
{
	// Only solid colour brushes are supported; other patterns fall back to the brush colour.
	nvgFillColor(vg, nvg_color(brush.Color));
}

// Rounded rectangle with independent x / y radii (nanovg only offers a single radius).
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
// Text layout (Pango) and glyph drawing (nanovg)

// Positive finite sizes become Pango units. Non-positive and huge values
// (callers pass FLT_MAX to measure unwrapped text) mean "unconstrained".
static int pango_units(float pixels)
{
	if (!(pixels > 0.0f) || pixels >= 1.0e6f) return -1;
	auto units = std::lround((double)pixels * PANGO_SCALE);
	if (units <= 0 || units > INT32_MAX) return -1;
	return (int)units;
}

static float pango_pixels(int units)
{
	return (float)units / (float)PANGO_SCALE;
}

struct OpenRTTextFrame
{
	UIRect Rect;     // aligned content rectangle in target coordinates
	float OriginX = 0; // Pango layout origin (box left, aligned top)
	float OriginY = 0;
};

static void pango_applyFont(PangoLayout* layout, UIFont const& font)
{
	auto* desc = pango_font_description_new();
	pango_font_description_set_family(desc, font.Family.empty() ? OPENUI_FONT_FAMILY : font.Family.c_str());
	pango_font_description_set_style(desc, (PangoStyle)font.Style);
	pango_font_description_set_weight(desc, (PangoWeight)font.Weight);
	int size = font.Size > 0 ? (int)font.Size : 1;
	pango_font_description_set_absolute_size(desc, (double)size * PANGO_SCALE);
	pango_layout_set_font_description(layout, desc);
	pango_font_description_free(desc);

	pango_layout_set_justify(layout, (font.Align & UIFont::AlignJustify) ? TRUE : FALSE);
	if (font.Align & UIFont::AlignRight) pango_layout_set_alignment(layout, PANGO_ALIGN_RIGHT);
	else if (font.Align & UIFont::AlignCenter) pango_layout_set_alignment(layout, PANGO_ALIGN_CENTER);
	else pango_layout_set_alignment(layout, PANGO_ALIGN_LEFT);

	if (font.Direction == UIFont::DirectionAutoLayout)
	{
		pango_layout_set_auto_dir(layout, TRUE);
	}
	else
	{
		pango_layout_set_auto_dir(layout, FALSE);
		auto dir = font.Direction == UIFont::DirectionRightToLeft ? PANGO_DIRECTION_RTL : PANGO_DIRECTION_LTR;
		pango_context_set_base_dir(pango_layout_get_context(layout), dir);
	}

	switch (font.Ellipsize)
	{
	case UIFont::EllipsizeStart: pango_layout_set_ellipsize(layout, PANGO_ELLIPSIZE_START); break;
	case UIFont::EllipsizeMiddle: pango_layout_set_ellipsize(layout, PANGO_ELLIPSIZE_MIDDLE); break;
	case UIFont::EllipsizeEnd: pango_layout_set_ellipsize(layout, PANGO_ELLIPSIZE_END); break;
	default: pango_layout_set_ellipsize(layout, PANGO_ELLIPSIZE_NONE); break;
	}

	pango_layout_set_wrap(layout, PANGO_WRAP_WORD_CHAR);
	pango_layout_set_single_paragraph_mode(layout, font.LineWrap ? FALSE : TRUE);
	pango_layout_set_line_spacing(layout, font.LineSpacing > 0 ? font.LineSpacing : 1.0f);

	auto* attrs = pango_attr_list_new();
	if (font.Spacing != 0)
	{
		auto* spacing = pango_attr_letter_spacing_new((int)font.Spacing * PANGO_SCALE);
		spacing->start_index = 0;
		spacing->end_index = G_MAXUINT;
		pango_attr_list_insert(attrs, spacing);
	}
	pango_layout_set_attributes(layout, attrs);
	pango_attr_list_unref(attrs);
}

static OpenRTTextFrame pango_layoutText(PangoLayout* layout, UIFont const& font, float x, float y, float width, float height, UIString const& text)
{
	OpenRTTextFrame frame;
	pango_applyFont(layout, font);
	pango_layout_set_width(layout, pango_units(width));
	pango_layout_set_height(layout, pango_units(height));
	pango_layout_set_text(layout, text.c_str(), (int)text.size());

	int contentUnits = 0;
	auto* iter = pango_layout_get_iter(layout);
	if (iter)
	{
		do
		{
			auto* line = pango_layout_iter_get_line_readonly(iter);
			int lineUnits = 0;
			if (line)
			{
				for (auto* run = line->runs; run; run = run->next)
				{
					auto* item = (PangoGlyphItem*)run->data;
					lineUnits += item->start_x_offset + pango_glyph_string_get_width(item->glyphs) + item->end_x_offset;
				}
			}
			contentUnits = std::max(contentUnits, lineUnits);
		} while (pango_layout_iter_next_line(iter));
		pango_layout_iter_free(iter);
	}

	int pixelW = 0, pixelH = 0;
	pango_layout_get_pixel_size(layout, &pixelW, &pixelH);
	float textW = pango_pixels(contentUnits);
	float textH = (float)pixelH;
	if (textW <= 0.0f) textW = (float)pixelW;

	frame.OriginX = x;
	frame.OriginY = y;
	if (height > 0.0f && height < 1.0e6f)
	{
		if (font.Align & UIFont::AlignBottom) frame.OriginY = y + height - textH;
		else if (font.Align & UIFont::AlignVCenter) frame.OriginY = y + std::round((height - textH) * 0.5f);
	}

	frame.Rect = {x, frame.OriginY, textW, textH};
	if (width > 0.0f && width < 1.0e6f)
	{
		if (font.Align & UIFont::AlignRight) frame.Rect.X = x + width - textW;
		else if (font.Align & UIFont::AlignCenter) frame.Rect.X = x + std::round((width - textW) * 0.5f);
	}
	return frame;
}

static UIRect pango_cursorRect(PangoLayout* layout, OpenRTTextFrame const& frame, int cursor)
{
	PangoRectangle strongPos, weakPos;
	pango_layout_get_cursor_pos(layout, cursor, &strongPos, &weakPos);
	return {
		frame.OriginX + pango_pixels(strongPos.x),
		frame.OriginY + pango_pixels(strongPos.y),
		pango_pixels(strongPos.width),
		pango_pixels(strongPos.height),
	};
}

static int pango_hitTest(PangoLayout* layout, OpenRTTextFrame const& frame, UIString const& text, float posX, float posY)
{
	int index = 0, trailing = 0;
	pango_layout_xy_to_index(layout,
		(int)((posX - frame.OriginX) * PANGO_SCALE),
		(int)((posY - frame.OriginY) * PANGO_SCALE),
		&index, &trailing);
	if (trailing && index >= 0 && index < (int)text.size())
		index += UIUTF8Num(text[(size_t)index]);
	return index;
}

static void nvg_blitGlyph(NVGcontext* vg, OpenRTGlyphImage const& glyph, float x, float y, float alpha)
{
	if (glyph.Image < 0 || glyph.Width <= 0 || glyph.Height <= 0) return;
	float ix = x + (float)glyph.Left;
	float iy = y - (float)glyph.Top;
	float w = (float)glyph.Width;
	float h = (float)glyph.Height;
	auto paint = nvgImagePattern(vg, ix, iy, w, h, 0.0f, glyph.Image, alpha);
	nvgBeginPath(vg);
	nvgRect(vg, ix, iy, w, h);
	nvgFillPaint(vg, paint);
	nvgFill(vg);
}

static uint32_t nvg_glyphRgb(UIColor const& color)
{
	uint32_t r = (uint32_t)std::lround(std::clamp(color.R, 0.0f, 1.0f) * 255.0f);
	uint32_t g = (uint32_t)std::lround(std::clamp(color.G, 0.0f, 1.0f) * 255.0f);
	uint32_t b = (uint32_t)std::lround(std::clamp(color.B, 0.0f, 1.0f) * 255.0f);
	return (r << 16) | (g << 8) | b;
}

// FreeType rasterizes a miss. Later draws reuse the nanovg texture.
static bool nvg_drawGlyphBitmap(NVGcontext* vg, UIMap<OpenRTGlyphKey, OpenRTGlyphImage>& cache, FT_Face face, FT_UInt glyph, float x, float y, UIColor const& color)
{
	OpenRTGlyphKey key;
	key.Face = face;
	key.Index = glyph;
	key.PixelSize = face->size ? (unsigned)face->size->metrics.y_ppem : 0;
	key.Rgb = nvg_glyphRgb(color);
	auto found = cache.find(key);
	if (found != cache.end())
	{
		nvg_blitGlyph(vg, found->second, x, y, color.A);
		return true;
	}

	if (FT_Load_Glyph(face, glyph, FT_LOAD_RENDER | FT_LOAD_COLOR) != 0) return false;
	auto const& bitmap = face->glyph->bitmap;
	OpenRTGlyphImage cached;
	cached.Left = face->glyph->bitmap_left;
	cached.Top = face->glyph->bitmap_top;
	cached.Width = (int)bitmap.width;
	cached.Height = (int)bitmap.rows;
	if (bitmap.width == 0 || bitmap.rows == 0 || bitmap.buffer == nullptr)
	{
		cache.emplace(key, cached);
		return true;
	}

	int pitch = bitmap.pitch;
	const uint8_t* src = bitmap.buffer;
	if (pitch < 0)
	{
		src += (size_t)(bitmap.rows - 1) * (size_t)(-pitch);
		pitch = -pitch;
	}

	UIList<uint8_t> pixels((size_t)bitmap.width * bitmap.rows * 4);
	if (bitmap.pixel_mode == FT_PIXEL_MODE_BGRA)
	{
		for (unsigned row = 0; row < bitmap.rows; ++row)
		{
			auto const* line = src + (size_t)row * pitch;
			for (unsigned col = 0; col < bitmap.width; ++col)
			{
				auto const* p = line + col * 4;
				auto* d = pixels.data() + ((size_t)row * bitmap.width + col) * 4;
				uint8_t b = p[0], g = p[1], r = p[2], a = p[3];
				if (a == 0) { d[0] = d[1] = d[2] = d[3] = 0; }
				else
				{
					d[0] = (uint8_t)std::min(255, r * 255 / a);
					d[1] = (uint8_t)std::min(255, g * 255 / a);
					d[2] = (uint8_t)std::min(255, b * 255 / a);
					d[3] = a;
				}
			}
		}
	}
	else if (bitmap.pixel_mode == FT_PIXEL_MODE_GRAY)
	{
		uint8_t r = (uint8_t)((key.Rgb >> 16) & 0xFF);
		uint8_t g = (uint8_t)((key.Rgb >> 8) & 0xFF);
		uint8_t b = (uint8_t)(key.Rgb & 0xFF);
		for (unsigned row = 0; row < bitmap.rows; ++row)
		{
			auto const* line = src + (size_t)row * pitch;
			for (unsigned col = 0; col < bitmap.width; ++col)
			{
				auto* d = pixels.data() + ((size_t)row * bitmap.width + col) * 4;
				d[0] = r; d[1] = g; d[2] = b; d[3] = line[col];
			}
		}
	}
	else return false;

	cached.Image = nvgCreateImageRGBA(vg, cached.Width, cached.Height, 0, pixels.data());
	if (cached.Image == -1) return false;
	auto stored = cache.emplace(key, cached).first;
	nvg_blitGlyph(vg, stored->second, x, y, color.A);
	return true;
}

// ====================================================================

OpenRTPainter::OpenRTPainter(UICanvasRaw canvas, int width, int height)
    :
    m_Canvas(canvas)
{
	m_Private = new OpenRTPainterPrivate;
	PRIVATE()->Context = rtCreateRT(RTVG_ANTIALIAS | RTVG_STENCIL_STROKES);
	if (PRIVATE()->Context == nullptr) UI_FATAL("NanoVG context could not be created");
	PRIVATE()->FontMap = pango_ft2_font_map_new();
	if (PRIVATE()->FontMap)
	{
		pango_ft2_font_map_set_resolution(PANGO_FT2_FONT_MAP(PRIVATE()->FontMap), 96.0, 96.0);
		auto* context = pango_font_map_create_context(PRIVATE()->FontMap);
		PRIVATE()->Layout = pango_layout_new(context);
		g_object_unref(context);
	}
}

OpenRTPainter::~OpenRTPainter()
{
	if (auto vg = CONTEXT())
	{
		if (PRIVATE()->InFrame) nvgCancelFrame(vg);
		for (auto image : PRIVATE()->Images) nvgDeleteImage(vg, image);
		for (auto& glyph : PRIVATE()->Glyphs)
			if (glyph.second.Image >= 0) nvgDeleteImage(vg, glyph.second.Image);
	}
	for (auto& slot : PRIVATE()->Targets) rt_destroy_texture(slot.second.stencil);
	if (PRIVATE()->Context) { rtDeleteRT(PRIVATE()->Context); PRIVATE()->Context = nullptr; }
	if (PRIVATE()->Layout) g_object_unref(PRIVATE()->Layout);
	if (PRIVATE()->FontMap) g_object_unref(PRIVATE()->FontMap);
	delete m_Private; m_Private = nullptr;
}

UICanvasRaw OpenRTPainter::getCanvas() const
{
    return m_Canvas;
}

void OpenRTPainter::setTarget(UIImageRaw value)
{
	auto vg = CONTEXT();
	if (value == nullptr)
	{
		if (vg && PRIVATE()->Target && PRIVATE()->InFrame)
		{
			nvgEndFrame(vg);
			PRIVATE()->InFrame = false;
			for (auto image : PRIVATE()->Images) nvgDeleteImage(vg, image);
			PRIVATE()->Images.clear();
		}
		PRIVATE()->Target = nullptr;
		return;
	}
	if (vg && PRIVATE()->InFrame)
	{
		// A previous session was never finished; drop it.
		nvgCancelFrame(vg);
		PRIVATE()->InFrame = false;
		for (auto image : PRIVATE()->Images) nvgDeleteImage(vg, image);
		PRIVATE()->Images.clear();
	}
	PRIVATE()->Target = value;
	if (vg && value && value->Width && value->Height)
	{
		NVGtarget nvgTarget = {};
		if (value->Format == UIImage::GPUByte && value->Texture)
		{
			auto* color = (rt_texture_t*)(uintptr_t)value->Texture;
			auto& slot = PRIVATE()->Targets[color];
			if (slot.stencil.handle == 0 || slot.stencil.width != color->width || slot.stencil.height != color->height)
			{
				if (slot.stencil.handle) rt_destroy_texture(slot.stencil);
				slot.stencil = rt_create_texture({
					.width = color->width, .height = color->height, .target = RT_TEXTURE_2D,
					.format = RT_TEXTURE_DEPTH24PLUS_STENCIL8,
					.usage = RT_TEXTURE_USAGE_RENDER_ATTACHMENT,
					.min_filter = RT_NEAREST, .mag_filter = RT_NEAREST,
					.address_u = RT_CLAMP_TO_EDGE, .address_v = RT_CLAMP_TO_EDGE, .address_w = RT_CLAMP_TO_EDGE,
					.mipmaps = 1,
				});
			}
			slot.target.colorTarget = color;
			slot.target.stencilTarget = &slot.stencil;
			nvgTarget = slot.target;
		}
		auto density = m_Canvas ? m_Canvas->getConfig().PixelDensity : 1.0f;
		nvgBeginFrame(vg, nvgTarget, (float)value->Width, (float)value->Height, density > 0 ? density : 1.0f);
		PRIVATE()->InFrame = true;
	}
}

UIRect OpenRTPainter::boundingRect(float x, float y, float width, float height, UIString const& text, float cursor, UIRectRaw cursorRect)
{
	auto layout = PRIVATE()->Layout;
	if (layout == nullptr) return {};
	auto frame = pango_layoutText(layout, PRIVATE()->Font, x, y, width, height, text);
	if (cursorRect) (*cursorRect) = pango_cursorRect(layout, frame, std::clamp((int)cursor, 0, (int)text.size()));
	return frame.Rect;
}

UIRect OpenRTPainter::boundingRect(float x, float y, float width, float height, UIString const& text, float posX, float posY, int* cursor, UIRectRaw cursorRect)
{
	auto layout = PRIVATE()->Layout;
	if (layout == nullptr) return {};
	auto frame = pango_layoutText(layout, PRIVATE()->Font, x, y, width, height, text);
	auto index = pango_hitTest(layout, frame, text, posX, posY);
	if (cursor) (*cursor) = index;
	if (cursorRect) (*cursorRect) = pango_cursorRect(layout, frame, index);
	return frame.Rect;
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
	else if (image.Format == UIImage::GPUByte && image.Pixels && image.Channel == 4)
	{
		auto& texture = *(rt_texture_t*)image.Texture;
		handle = rtCreateTextureNative(vg, 0, texture);
	}

	if (handle == -1) return;
	PRIVATE()->Images.push_back(handle); // deleted after nvgEndFrame

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
	auto layout = PRIVATE()->Layout;
	if (layout == nullptr) return;

	auto frame = pango_layoutText(layout, PRIVATE()->Font, x, y, width, height, text);
	if (boundingRect) (*boundingRect) = frame.Rect;
	if (cursorRect) (*cursorRect) = pango_cursorRect(layout, frame, std::clamp((int)cursor, 0, (int)text.size()));

	auto vg = CONTEXT();
	if (vg == nullptr || !PRIVATE()->InFrame) return;
	if (PRIVATE()->Brush.Style == UIBrush::NoBrush) return;
	if (!nvg_applyClip(vg, PRIVATE()->Scissor, PRIVATE()->EnableClip)) return;

	auto* iter = pango_layout_get_iter(layout);
	if (iter == nullptr) return;
	do
	{
		auto* line = pango_layout_iter_get_line_readonly(iter);
		if (line == nullptr) continue;
		PangoRectangle logical;
		pango_layout_iter_get_line_extents(iter, nullptr, &logical);
		int baseline = pango_layout_iter_get_baseline(iter);
		int xOff = 0;
		for (auto* link = line->runs; link; link = link->next)
		{
			auto* run = (PangoGlyphItem*)link->data;
			xOff += run->start_x_offset;
			int yOff = run->y_offset;
			PangoFont* font = run->item ? run->item->analysis.font : nullptr;
			if (font && (run->item->analysis.flags & PANGO_ANALYSIS_FLAG_CENTERED_BASELINE))
			{
				PangoRectangle runLogical;
				pango_glyph_string_extents(run->glyphs, font, nullptr, &runLogical);
				int adjustment = runLogical.y + runLogical.height / 2;
				if (((runLogical.y | runLogical.height) & (PANGO_SCALE - 1)) == 0)
					adjustment = PANGO_UNITS_ROUND(adjustment);
				yOff += adjustment;
			}

			float baseX = frame.OriginX + pango_pixels(logical.x + xOff);
			float baseY = frame.OriginY + pango_pixels(baseline - yOff);
			// Pango's hb_font is not created with hb-ft, so hb_ft_font_lock_face returns null.
			// The FreeType face lives on the PangoFcFont that PangoFT2 uses.
			PangoFcFont* fcFont = (font && PANGO_IS_FC_FONT(font)) ? PANGO_FC_FONT(font) : nullptr;
			FT_Face face = fcFont ? pango_fc_font_lock_face(fcFont) : nullptr;

			int pen = 0;
			for (int i = 0; i < run->glyphs->num_glyphs; ++i)
			{
				auto const& glyph = run->glyphs->glyphs[i];
				float gx = baseX + pango_pixels(pen + glyph.geometry.x_offset);
				float gy = baseY + pango_pixels(glyph.geometry.y_offset);
				pen += glyph.geometry.width;
				if (glyph.glyph == PANGO_GLYPH_EMPTY || face == nullptr) continue;

				// Unknown input has no font glyph; draw the face's .notdef bitmap.
				auto index = (glyph.glyph & PANGO_GLYPH_UNKNOWN_FLAG) ? (FT_UInt)0 : (FT_UInt)glyph.glyph;
				nvg_drawGlyphBitmap(vg, PRIVATE()->Glyphs, face, index, gx, gy, PRIVATE()->Brush.Color);
			}

			if (fcFont && face) pango_fc_font_unlock_face(fcFont);
			xOff += pango_glyph_string_get_width(run->glyphs) + run->end_x_offset;
		}
	} while (pango_layout_iter_next_line(iter));
	pango_layout_iter_free(iter);
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
