/*=================================================
* Copyright © 2020-2024 ChivenZhang.
* All Rights Reserved.
* =====================Note=========================
*
*
*=====================History========================
* Created by ChivenZhang@gmail.com.
*
* =================================================*/
#include "CairoUIPainter.h"
#include <cairo/cairo.h>
#include <pango/pangocairo.h>

class CairoUIPainterData : public UIPainterPrivate
{
public:
	cairo_t* NativeContext;
	PangoLayout* NativeLayout;
	cairo_surface_t* NativeSurface;
	UIPen Pen;
	UIFont Font;
	UIBrush Brush;
	UIRect CilpRect;
	bool EnableCilp = false;
};
#define PRIVATE() ((CairoUIPainterData*) m_Private)

CairoUIPainter::CairoUIPainter(uint32_t width, uint32_t height)
{
	m_Private = new CairoUIPainterData;

	PRIVATE()->NativeSurface = cairo_image_surface_create(cairo_format_t::CAIRO_FORMAT_ARGB32, width, height);
	PRIVATE()->NativeContext = cairo_create(PRIVATE()->NativeSurface);
	auto cr = PRIVATE()->NativeContext;
	cairo_set_source_rgba(cr, 0, 0, 0, 1);
	cairo_set_antialias(cr, CAIRO_ANTIALIAS_BEST);
	cairo_paint(cr);

	PRIVATE()->NativeLayout = pango_cairo_create_layout(cr);
	auto& font = PRIVATE()->Font;
	auto layout = PRIVATE()->NativeLayout;
	auto font_desc = pango_font_description_new();
	pango_font_description_set_family(font_desc, font.Family.c_str());
	pango_font_description_set_style(font_desc, (PangoStyle)font.Style);
	pango_font_description_set_size(font_desc, font.Size * PANGO_SCALE);
	pango_font_description_set_weight(font_desc, (PangoWeight)font.Weight);
	pango_layout_set_font_description(layout, font_desc);
	pango_font_description_free(font_desc);
}

CairoUIPainter::~CairoUIPainter()
{
	g_object_unref(PRIVATE()->NativeLayout); PRIVATE()->NativeLayout = nullptr;
	cairo_destroy(PRIVATE()->NativeContext); PRIVATE()->NativeContext = nullptr;
	cairo_surface_destroy(PRIVATE()->NativeSurface); PRIVATE()->NativeSurface = nullptr;
	delete  m_Private; m_Private = nullptr;
}

UIRect CairoUIPainter::boundingRect(float x, float y, float width, float height, UIString const& text, float cursor, UIRectRaw cursorRect)
{
	auto layout = PRIVATE()->NativeLayout;
	auto& font = PRIVATE()->Font;

	pango_layout_set_text(layout, text.c_str(), -1);
	pango_layout_set_width(layout, width * PANGO_SCALE);
	pango_layout_set_height(layout, height * PANGO_SCALE);

	UIRect text_rect;
	int32_t text_width, text_height;
	int32_t baseline = pango_layout_get_baseline(layout);
	pango_layout_get_pixel_size(layout, &text_width, &text_height);
	text_rect.W = text_width; text_rect.H = text_height;

	if (font.Align & UIFont::AlignTop) { text_rect.X = x; text_rect.Y = y; }
	else if (font.Align & UIFont::AlignBottom) { text_rect.X = x; text_rect.Y = y + height - text_height; }
	else if (font.Align & UIFont::AlignVCenter) { text_rect.X = x; text_rect.Y = y + std::round((height - text_height) * 0.5f); }
	else if (font.Align & UIFont::AlignBaseline) { text_rect.X = x; text_rect.Y = y + baseline; }
	else { text_rect.X = x; text_rect.Y = y; }

	if (cursorRect)
	{
		PangoRectangle strong_pos, weak_pos;
		pango_layout_get_cursor_pos(layout, cursor, &strong_pos, &weak_pos);
		(*cursorRect) = UIRect{ text_rect.X + (float)strong_pos.x / PANGO_SCALE, text_rect.Y + (float)strong_pos.y / PANGO_SCALE, 0, (float)strong_pos.height / PANGO_SCALE };
	}

	return text_rect;
}

UIRect CairoUIPainter::boundingRect(float x, float y, float width, float height, UIString const& text, float posX, float posY, int* cursor, UIRectRaw cursorRect)
{
	auto layout = PRIVATE()->NativeLayout;
	auto& font = PRIVATE()->Font;

	pango_layout_set_text(layout, text.c_str(), -1);
	pango_layout_set_width(layout, width * PANGO_SCALE);
	pango_layout_set_height(layout, height * PANGO_SCALE);

	UIRect text_rect;
	int32_t text_width, text_height;
	int32_t baseline = pango_layout_get_baseline(layout);
	pango_layout_get_pixel_size(layout, &text_width, &text_height);
	text_rect.W = text_width; text_rect.H = text_height;

	if (font.Align & UIFont::AlignTop) { text_rect.X = x; text_rect.Y = y; }
	else if (font.Align & UIFont::AlignBottom) { text_rect.X = x; text_rect.Y = y + height - text_height; }
	else if (font.Align & UIFont::AlignVCenter) { text_rect.X = x; text_rect.Y = y + std::round((height - text_height) * 0.5f); }
	else if (font.Align & UIFont::AlignBaseline) { text_rect.X = x; text_rect.Y = y + baseline; }
	else { text_rect.X = x; text_rect.Y = y; }

	int32_t text_index, text_trailing;
	pango_layout_xy_to_index(layout, (posX - text_rect.X) * PANGO_SCALE, (posY - text_rect.Y) * PANGO_SCALE, &text_index, &text_trailing);
	auto _cursor = text_index + ((text_trailing) ? UIUTF8Num(text[text_index]) : 0);

	if (cursor) (*cursor) = _cursor;
	if (cursorRect)
	{
		PangoRectangle strong_pos, weak_pos;
		pango_layout_get_cursor_pos(layout, _cursor, &strong_pos, &weak_pos);
		(*cursorRect) = UIRect{ text_rect.X + (float)strong_pos.x / PANGO_SCALE, text_rect.Y + (float)strong_pos.y / PANGO_SCALE, 0, (float)strong_pos.height / PANGO_SCALE };
	}

	return text_rect;
}

void CairoUIPainter::drawArc(float x, float y, float width, float height, float startAngle, float spanAngle)
{
	if (width <= 0 || height <= 0) return;
	auto cr = PRIVATE()->NativeContext;
	if (PRIVATE()->Brush.Style != UIBrush::NoBrush)
	{
		cairo_save(cr); setClipping(true);
		auto scale_factor = 1.0 * height / width;
		cairo_scale(cr, 1.0, scale_factor);
		cairo_translate(cr, x, y / scale_factor);
		cairo_arc(cr, width * 0.5, height * 0.5 / scale_factor, width * 0.5, startAngle * (M_PI / 180.0), spanAngle * (M_PI / 180.0));
		cairo_set_source_rgba(cr, PRIVATE()->Brush.Color.R, PRIVATE()->Brush.Color.G, PRIVATE()->Brush.Color.B, PRIVATE()->Brush.Color.A);
		cairo_fill(cr);
		cairo_restore(cr);
	}
	if (PRIVATE()->Pen.Style != UIPen::NoPen)
	{
		cairo_save(cr); setClipping(true);
		auto scale_factor = 1.0 * height / width;
		cairo_scale(cr, 1.0, scale_factor);
		cairo_translate(cr, x, y / scale_factor);
		cairo_arc(cr, width * 0.5, height * 0.5 / scale_factor, width * 0.5, startAngle * (M_PI / 180.0), spanAngle * (M_PI / 180.0));
		cairo_set_line_width(cr, PRIVATE()->Pen.Width);
		cairo_set_source_rgba(cr, PRIVATE()->Pen.Color.R, PRIVATE()->Pen.Color.G, PRIVATE()->Pen.Color.B, PRIVATE()->Pen.Color.A);
		cairo_stroke(cr);
		cairo_restore(cr);
	}
}

void CairoUIPainter::drawChord(float x, float y, float width, float height, float startAngle, float spanAngle)
{
	if (width <= 0 || height <= 0) return;
	auto cr = PRIVATE()->NativeContext;
	if (PRIVATE()->Brush.Style != UIBrush::NoBrush)
	{
		cairo_save(cr); setClipping(true);
		auto scale_factor = 1.0 * height / width;
		cairo_scale(cr, 1.0, scale_factor);
		cairo_translate(cr, x, y / scale_factor);
		cairo_arc(cr, 0, 0, width * 0.5, startAngle * (M_PI / 180.0), spanAngle * (M_PI / 180.0));
		cairo_set_source_rgba(cr, PRIVATE()->Brush.Color.R, PRIVATE()->Brush.Color.G, PRIVATE()->Brush.Color.B, PRIVATE()->Brush.Color.A);
		cairo_fill(cr);
		cairo_restore(cr);
	}
	if (PRIVATE()->Pen.Style != UIPen::NoPen)
	{
		cairo_save(cr); setClipping(true);
		auto scale_factor = 1.0 * height / width;
		cairo_scale(cr, 1.0, scale_factor);
		cairo_translate(cr, x, y / scale_factor);
		cairo_arc(cr, 0, 0, width * 0.5, startAngle * (M_PI / 180.0), spanAngle * (M_PI / 180.0));
		cairo_close_path(cr);
		cairo_set_line_width(cr, PRIVATE()->Pen.Width);
		cairo_set_source_rgba(cr, PRIVATE()->Pen.Color.R, PRIVATE()->Pen.Color.G, PRIVATE()->Pen.Color.B, PRIVATE()->Pen.Color.A);
		cairo_stroke(cr);
		cairo_restore(cr);
	}
}

void CairoUIPainter::drawEllipse(float x, float y, float width, float height)
{
	drawArc(x, y, width, height, 0, 360);
}

void CairoUIPainter::drawImage(float x, float y, UIImage image, float sx, float sy, float sw, float sh)
{
	auto cr = PRIVATE()->NativeContext;
	if (image.Pixel
		&& image.Type == UIImage::Byte
		&& image.Width * 4 == image.Stride)
	{
		auto surface = cairo_image_surface_create_for_data((uint8_t*)image.Pixel, CAIRO_FORMAT_ARGB32, image.Width, image.Height, image.Stride);
		cairo_save(cr); setClipping(true);
		cairo_set_source_surface(cr, surface, x - sx, y - sy);
		cairo_rectangle(cr, x, y, std::min<float>(uint32_t(sw), image.Width), std::min<float>(uint32_t(sh), image.Height));
		cairo_fill(cr);
		cairo_restore(cr);
		cairo_surface_destroy(surface);
	}
}

void CairoUIPainter::drawLine(float x1, float y1, float x2, float y2)
{
	auto cr = PRIVATE()->NativeContext;
	if (PRIVATE()->Pen.Style != UIPen::NoPen)
	{
		cairo_save(cr); setClipping(true);
		cairo_set_line_width(cr, PRIVATE()->Pen.Width);
		cairo_set_source_rgba(cr, PRIVATE()->Pen.Color.R, PRIVATE()->Pen.Color.G, PRIVATE()->Pen.Color.B, PRIVATE()->Pen.Color.A);
		cairo_move_to(cr, x1, y1);
		cairo_line_to(cr, x2, y2);
		cairo_stroke(cr);
		cairo_restore(cr);
	}
}

void CairoUIPainter::drawLines(UIArrayView<UILine> lines)
{
	auto cr = PRIVATE()->NativeContext;
	if (PRIVATE()->Pen.Style != UIPen::NoPen && 2 <= lines.size())
	{
		cairo_save(cr); setClipping(true);
		cairo_set_line_width(cr, PRIVATE()->Pen.Width);
		cairo_set_source_rgba(cr, PRIVATE()->Pen.Color.R, PRIVATE()->Pen.Color.G, PRIVATE()->Pen.Color.B, PRIVATE()->Pen.Color.A);
		for (size_t i = 0; i < lines.size(); ++i)
		{
			if (i == 0) cairo_move_to(cr, lines[i].P0.X, lines[i].P0.Y);
			else cairo_line_to(cr, lines[i].P1.X, lines[i].P1.Y);
		}
		cairo_stroke(cr);
		cairo_restore(cr);
	}
}

void CairoUIPainter::drawPie(float x, float y, float width, float height, float startAngle, float spanAngle)
{
	if (width <= 0 || height <= 0) return;
	auto cr = PRIVATE()->NativeContext;
	if (PRIVATE()->Brush.Style != UIBrush::NoBrush)
	{
		cairo_save(cr); setClipping(true);
		auto scale_factor = 1.0 * height / width;
		cairo_scale(cr, 1.0, scale_factor);
		cairo_translate(cr, x, y / scale_factor);
		cairo_move_to(cr, 0, 0);
		cairo_arc(cr, 0, 0, width * 0.5, startAngle * (M_PI / 180.0), spanAngle * (M_PI / 180.0));
		cairo_set_source_rgba(cr, PRIVATE()->Brush.Color.R, PRIVATE()->Brush.Color.G, PRIVATE()->Brush.Color.B, PRIVATE()->Brush.Color.A);
		cairo_fill(cr);
		cairo_restore(cr);
	}
	if (PRIVATE()->Pen.Style != UIPen::NoPen)
	{
		cairo_save(cr); setClipping(true);
		auto scale_factor = 1.0 * height / width;
		cairo_scale(cr, 1.0, scale_factor);
		cairo_translate(cr, x, y / scale_factor);
		cairo_move_to(cr, 0, 0);
		cairo_arc(cr, 0, 0, width * 0.5, startAngle * (M_PI / 180.0), spanAngle * (M_PI / 180.0));
		cairo_close_path(cr);
		cairo_set_line_width(cr, PRIVATE()->Pen.Width);
		cairo_set_source_rgba(cr, PRIVATE()->Pen.Color.R, PRIVATE()->Pen.Color.G, PRIVATE()->Pen.Color.B, PRIVATE()->Pen.Color.A);
		cairo_stroke(cr);
		cairo_restore(cr);
	}
}

void CairoUIPainter::drawPoint(float x, float y)
{
	auto cr = PRIVATE()->NativeContext;
	if (PRIVATE()->Brush.Style != UIBrush::NoBrush)
	{
		cairo_save(cr); setClipping(true);
		cairo_rectangle(cr, x - 1, y - 1, 2, 2);
		cairo_set_source_rgba(cr, PRIVATE()->Brush.Color.R, PRIVATE()->Brush.Color.G, PRIVATE()->Brush.Color.B, PRIVATE()->Brush.Color.A);
		cairo_fill(cr);
		cairo_restore(cr);
	}
	if (PRIVATE()->Pen.Style != UIPen::NoPen)
	{
		cairo_save(cr); setClipping(true);
		cairo_rectangle(cr, x - 1, y - 1, 2, 2);
		cairo_set_line_width(cr, PRIVATE()->Pen.Width);
		cairo_set_source_rgba(cr, PRIVATE()->Pen.Color.R, PRIVATE()->Pen.Color.G, PRIVATE()->Pen.Color.B, PRIVATE()->Pen.Color.A);
		cairo_stroke(cr);
		cairo_restore(cr);
	}
}

void CairoUIPainter::drawPoints(UIArrayView<UIPoint> points)
{
	auto cr = PRIVATE()->NativeContext;
	if (PRIVATE()->Brush.Style != UIBrush::NoBrush)
	{
		cairo_save(cr); setClipping(true);
		for (size_t i = 0; i < points.size(); ++i) cairo_rectangle(cr, points[i].X - 1, points[i].Y - 1, 2, 2);
		cairo_set_source_rgba(cr, PRIVATE()->Brush.Color.R, PRIVATE()->Brush.Color.G, PRIVATE()->Brush.Color.B, PRIVATE()->Brush.Color.A);
		cairo_fill(cr);
		cairo_restore(cr);
	}
	if (PRIVATE()->Pen.Style != UIPen::NoPen)
	{
		cairo_save(cr); setClipping(true);
		for (size_t i = 0; i < points.size(); ++i) cairo_rectangle(cr, points[i].X - 1, points[i].Y - 1, 2, 2);
		cairo_set_line_width(cr, PRIVATE()->Pen.Width);
		cairo_set_source_rgba(cr, PRIVATE()->Pen.Color.R, PRIVATE()->Pen.Color.G, PRIVATE()->Pen.Color.B, PRIVATE()->Pen.Color.A);
		cairo_stroke(cr);
		cairo_restore(cr);
	}
}

void CairoUIPainter::drawPolygon(UIArrayView<UIPoint> points)
{
	auto cr = PRIVATE()->NativeContext;
	if (PRIVATE()->Brush.Style != UIBrush::NoBrush && 2 <= points.size())
	{
		cairo_save(cr); setClipping(true);
		for (size_t i = 0; i < points.size(); ++i)
		{
			if (i == 0) cairo_move_to(cr, points[i].X, points[i].Y);
			else cairo_line_to(cr, points[i].X, points[i].Y);
		}
		cairo_set_source_rgba(cr, PRIVATE()->Brush.Color.R, PRIVATE()->Brush.Color.G, PRIVATE()->Brush.Color.B, PRIVATE()->Brush.Color.A);
		cairo_fill(cr);
		cairo_restore(cr);
	}
	if (PRIVATE()->Pen.Style != UIPen::NoPen && 2 <= points.size())
	{
		cairo_save(cr); setClipping(true);
		for (size_t i = 0; i < points.size(); ++i)
		{
			if (i == 0) cairo_move_to(cr, points[i].X, points[i].Y);
			else cairo_line_to(cr, points[i].X, points[i].Y);
		}
		cairo_close_path(cr);
		cairo_set_line_width(cr, PRIVATE()->Pen.Width);
		cairo_set_source_rgba(cr, PRIVATE()->Pen.Color.R, PRIVATE()->Pen.Color.G, PRIVATE()->Pen.Color.B, PRIVATE()->Pen.Color.A);
		cairo_stroke(cr);
		cairo_restore(cr);
	}
}

void CairoUIPainter::drawPolyline(UIArrayView<UIPoint> points)
{
	auto cr = PRIVATE()->NativeContext;
	if (PRIVATE()->Pen.Style != UIPen::NoPen && 2 <= points.size())
	{
		cairo_save(cr); setClipping(true);
		for (size_t i = 0; i < points.size(); ++i)
		{
			if (i == 0) cairo_move_to(cr, points[i].X, points[i].Y);
			else cairo_line_to(cr, points[i].X, points[i].Y);
		}
		cairo_set_line_width(cr, PRIVATE()->Pen.Width);
		cairo_set_source_rgba(cr, PRIVATE()->Pen.Color.R, PRIVATE()->Pen.Color.G, PRIVATE()->Pen.Color.B, PRIVATE()->Pen.Color.A);
		cairo_stroke(cr);
		cairo_restore(cr);
	}
}

void CairoUIPainter::drawRect(float x, float y, float width, float height)
{
	auto cr = PRIVATE()->NativeContext;
	if (PRIVATE()->Brush.Style != UIBrush::NoBrush && 0 <= width && 0 <= height)
	{
		cairo_save(cr); setClipping(true);
		cairo_rectangle(cr, x, y, width, height);
		cairo_set_source_rgba(cr, PRIVATE()->Brush.Color.R, PRIVATE()->Brush.Color.G, PRIVATE()->Brush.Color.B, PRIVATE()->Brush.Color.A);
		cairo_fill(cr);
		cairo_restore(cr);
	}
	if (PRIVATE()->Pen.Style != UIPen::NoPen && 0 <= width && 0 <= height)
	{
		cairo_save(cr); setClipping(true);
		cairo_rectangle(cr, x, y, width, height);
		cairo_set_line_width(cr, PRIVATE()->Pen.Width);
		cairo_set_source_rgba(cr, PRIVATE()->Pen.Color.R, PRIVATE()->Pen.Color.G, PRIVATE()->Pen.Color.B, PRIVATE()->Pen.Color.A);
		cairo_stroke(cr);
		cairo_restore(cr);
	}
}

void CairoUIPainter::drawRects(UIArrayView<UIRect> rects)
{
	auto cr = PRIVATE()->NativeContext;
	if (PRIVATE()->Brush.Style != UIBrush::NoBrush)
	{
		cairo_save(cr); setClipping(true);
		for (size_t i = 0; i < rects.size(); ++i)
		{
			if (0 < rects[i].W && 0 < rects[i].H) cairo_rectangle(cr, rects[i].X, rects[i].Y, rects[i].W, rects[i].H);
		}
		cairo_set_source_rgba(cr, PRIVATE()->Brush.Color.R, PRIVATE()->Brush.Color.G, PRIVATE()->Brush.Color.B, PRIVATE()->Brush.Color.A);
		cairo_fill(cr);
		cairo_restore(cr);
	}
	if (PRIVATE()->Pen.Style != UIPen::NoPen)
	{
		cairo_save(cr); setClipping(true);
		for (size_t i = 0; i < rects.size(); ++i)
		{
			if (0 < rects[i].W && 0 < rects[i].H) cairo_rectangle(cr, rects[i].X, rects[i].Y, rects[i].W, rects[i].H);
		}
		cairo_set_line_width(cr, PRIVATE()->Pen.Width);
		cairo_set_source_rgba(cr, PRIVATE()->Pen.Color.R, PRIVATE()->Pen.Color.G, PRIVATE()->Pen.Color.B, PRIVATE()->Pen.Color.A);
		cairo_stroke(cr);
		cairo_restore(cr);
	}
}

void CairoUIPainter::drawRoundedRect(float x, float y, float width, float height, float xRadius, float yRadius)
{
	auto cr = PRIVATE()->NativeContext;
	if (PRIVATE()->Brush.Style != UIBrush::NoBrush && 0 < width && 0 < height)
	{
		if (xRadius > width * 0.5f) xRadius = width * 0.5f;
		if (yRadius > height * 0.5f) yRadius = height * 0.5f;
		auto to_radius = (M_PI / 180.0);
		auto scale_factor = 1.0 * yRadius / xRadius;
		auto scale_inverse = 1.0 / scale_factor;
		cairo_save(cr); setClipping(true);
		cairo_scale(cr, 1.0, scale_factor);
		cairo_new_sub_path(cr);
		cairo_arc(cr, x + width - xRadius, (y + yRadius) * scale_inverse, xRadius, -90 * to_radius, 0);
		cairo_arc(cr, x + width - xRadius, (y + height - yRadius) * scale_inverse, xRadius, 0, 90 * to_radius);
		cairo_arc(cr, x + xRadius, (y + height - yRadius) * scale_inverse, xRadius, 90 * to_radius, 180 * to_radius);
		cairo_arc(cr, x + xRadius, (y + yRadius) * scale_inverse, xRadius, 180 * to_radius, 270 * to_radius);
		cairo_set_source_rgba(cr, PRIVATE()->Brush.Color.R, PRIVATE()->Brush.Color.G, PRIVATE()->Brush.Color.B, PRIVATE()->Brush.Color.A);
		cairo_fill(cr);
		cairo_restore(cr);
	}
	if (PRIVATE()->Pen.Style != UIPen::NoPen && 0 < width && 0 < height)
	{
		if (xRadius > width * 0.5f) xRadius = width * 0.5f;
		if (yRadius > height * 0.5f) yRadius = height * 0.5f;
		auto to_radius = (M_PI / 180.0);
		auto scale_factor = 1.0 * yRadius / xRadius;
		auto scale_inverse = 1.0 / scale_factor;
		cairo_save(cr); setClipping(true);
		cairo_scale(cr, 1.0, scale_factor);
		cairo_new_sub_path(cr);
		cairo_arc(cr, x + width - xRadius, (y + yRadius) * scale_inverse, xRadius, -90 * to_radius, 0);
		cairo_arc(cr, x + width - xRadius, (y + height - yRadius) * scale_inverse, xRadius, 0, 90 * to_radius);
		cairo_arc(cr, x + xRadius, (y + height - yRadius) * scale_inverse, xRadius, 90 * to_radius, 180 * to_radius);
		cairo_arc(cr, x + xRadius, (y + yRadius) * scale_inverse, xRadius, 180 * to_radius, 270 * to_radius);
		cairo_close_path(cr);
		cairo_set_line_width(cr, PRIVATE()->Pen.Width);
		cairo_set_source_rgba(cr, PRIVATE()->Pen.Color.R, PRIVATE()->Pen.Color.G, PRIVATE()->Pen.Color.B, PRIVATE()->Pen.Color.A);
		cairo_stroke(cr);
		cairo_restore(cr);
	}
}

void CairoUIPainter::drawText(float x, float y, float width, float height, const UIString& text, UIRectRaw boundingRect, float cursor, UIRectRaw cursorRect)
{
	auto cr = PRIVATE()->NativeContext;
	auto layout = PRIVATE()->NativeLayout;
	auto& font = PRIVATE()->Font;
	if (PRIVATE()->Brush.Style != UIBrush::NoBrush)
	{
		cairo_save(cr); setClipping(true);
		pango_layout_set_text(layout, text.c_str(), -1);
		pango_layout_set_width(layout, width * PANGO_SCALE);
		pango_layout_set_height(layout, height * PANGO_SCALE);

		UIRect text_rect;
		int32_t text_width, text_height;
		int32_t baseline = pango_layout_get_baseline(layout);
		pango_layout_get_pixel_size(layout, &text_width, &text_height);
		text_rect.W = text_width; text_rect.H = text_height;

		if (font.Align & UIFont::AlignTop) { text_rect.X = x; text_rect.Y = y; }
		else if (font.Align & UIFont::AlignBottom) { text_rect.X = x; text_rect.Y = y + height - text_height; }
		else if (font.Align & UIFont::AlignVCenter) { text_rect.X = x; text_rect.Y = y + std::round((height - text_height) * 0.5f); }
		else if (font.Align & UIFont::AlignBaseline) { text_rect.X = x; text_rect.Y = y + baseline; }
		else { text_rect.X = x; text_rect.Y = y; }
		if (boundingRect) (*boundingRect) = text_rect;

		if (cursorRect)
		{
			PangoRectangle strong_pos, weak_pos;
			pango_layout_get_cursor_pos(layout, cursor, &strong_pos, &weak_pos);
			(*cursorRect) = UIRect{ text_rect.X + (float)strong_pos.x / PANGO_SCALE, text_rect.Y + (float)strong_pos.y / PANGO_SCALE, 0, (float)strong_pos.height / PANGO_SCALE };
		}

		cairo_move_to(cr, text_rect.X, text_rect.Y);
		cairo_set_source_rgba(cr, PRIVATE()->Brush.Color.R, PRIVATE()->Brush.Color.G, PRIVATE()->Brush.Color.B, PRIVATE()->Brush.Color.A);
		pango_cairo_show_layout(cr, layout);
		cairo_restore(cr);
	}
	if (PRIVATE()->Pen.Style != UIPen::NoPen)
	{
		cairo_save(cr); setClipping(true);
		pango_layout_set_width(layout, width * PANGO_SCALE);
		pango_layout_set_height(layout, height * PANGO_SCALE);

		UIRect text_rect;
		int32_t text_width, text_height;
		int32_t baseline = pango_layout_get_baseline(layout);
		pango_layout_get_pixel_size(layout, &text_width, &text_height);
		text_rect.W = text_width; text_rect.H = text_height;

		if (font.Align & UIFont::AlignTop) { text_rect.X = x; text_rect.Y = y; }
		else if (font.Align & UIFont::AlignBottom) { text_rect.X = x; text_rect.Y = y + height - text_height; }
		else if (font.Align & UIFont::AlignVCenter) { text_rect.X = x; text_rect.Y = y + std::round((height - text_height) * 0.5f); }
		else if (font.Align & UIFont::AlignBaseline) { text_rect.X = x; text_rect.Y = y + baseline; }
		else { text_rect.X = x; text_rect.Y = y; }
		if (boundingRect) (*boundingRect) = text_rect;

		if (cursorRect)
		{
			PangoRectangle strong_pos, weak_pos;
			pango_layout_get_cursor_pos(layout, cursor, &strong_pos, &weak_pos);
			(*cursorRect) = UIRect{ text_rect.X + (float)strong_pos.x / PANGO_SCALE, text_rect.Y + (float)strong_pos.y / PANGO_SCALE, 0, (float)strong_pos.height / PANGO_SCALE };
		}

		cairo_move_to(cr, text_rect.X, text_rect.Y);
		pango_cairo_layout_path(cr, layout);
		cairo_set_line_width(cr, PRIVATE()->Pen.Width);
		cairo_set_source_rgba(cr, PRIVATE()->Pen.Color.R, PRIVATE()->Pen.Color.G, PRIVATE()->Pen.Color.B, PRIVATE()->Pen.Color.A);
		cairo_stroke(cr);
		cairo_restore(cr);
	}
}

UIPen const& CairoUIPainter::getPen() const
{
	return PRIVATE()->Pen;
}

void CairoUIPainter::setPen(const UIPen& pen)
{
	PRIVATE()->Pen = pen;
}

UIBrush const& CairoUIPainter::getBrush() const
{
	return PRIVATE()->Brush;
}

void CairoUIPainter::setBrush(const UIBrush& brush)
{
	PRIVATE()->Brush = brush;
}

UIFont const& CairoUIPainter::getFont() const
{
	return PRIVATE()->Font;
}

void CairoUIPainter::setFont(const UIFont& font)
{
	auto layout = PRIVATE()->NativeLayout;
	auto font_desc = pango_layout_get_font_description(layout);
	if (pango_font_description_get_family(font_desc) != font.Family
		|| pango_font_description_get_style(font_desc) != (PangoStyle)font.Style
		|| pango_font_description_get_size(font_desc) != font.Size * PANGO_SCALE
		|| pango_font_description_get_weight(font_desc) != (PangoWeight)font.Weight)
	{
		auto font_desc = pango_font_description_new();
		pango_font_description_set_family(font_desc, font.Family.c_str());
		pango_font_description_set_style(font_desc, (PangoStyle)font.Style);
		pango_font_description_set_size(font_desc, font.Size * PANGO_SCALE);
		pango_font_description_set_weight(font_desc, (PangoWeight)font.Weight);
		pango_layout_set_font_description(layout, font_desc);
		pango_font_description_free(font_desc);
	}
	if (font.Align & UIFont::AlignLeft)
	{
		pango_layout_set_justify(layout, false);
		pango_layout_set_alignment(layout, PangoAlignment::PANGO_ALIGN_LEFT);
	}
	else if (font.Align & UIFont::AlignRight)
	{
		pango_layout_set_justify(layout, false);
		pango_layout_set_alignment(layout, PangoAlignment::PANGO_ALIGN_RIGHT);
	}
	else if (font.Align & UIFont::AlignCenter)
	{
		pango_layout_set_justify(layout, false);
		pango_layout_set_alignment(layout, PangoAlignment::PANGO_ALIGN_CENTER);
	}
	else if (font.Align & UIFont::AlignJustify) pango_layout_set_justify(layout, true);

	switch (font.Direction)
	{
	case UIFont::DirectionAutoLayout: pango_layout_set_auto_dir(layout, true); break;
	default: pango_layout_set_auto_dir(layout, false); break;
	}
	switch (font.Ellipsize)
	{
	case UIFont::EllipsizeNone: pango_layout_set_ellipsize(layout, PangoEllipsizeMode::PANGO_ELLIPSIZE_NONE); break;
	case UIFont::EllipsizeStart: pango_layout_set_ellipsize(layout, PangoEllipsizeMode::PANGO_ELLIPSIZE_START); break;
	case UIFont::EllipsizeMiddle: pango_layout_set_ellipsize(layout, PangoEllipsizeMode::PANGO_ELLIPSIZE_MIDDLE); break;
	case UIFont::EllipsizeEnd: pango_layout_set_ellipsize(layout, PangoEllipsizeMode::PANGO_ELLIPSIZE_END); break;
	}
	pango_layout_set_spacing(layout, font.Spacing);
	pango_layout_set_line_spacing(layout, font.LineSpacing);
	pango_layout_set_single_paragraph_mode(layout, font.LineWrap == false);
	PRIVATE()->Font = font;
}

void CairoUIPainter::setClipping(bool enable)
{
	auto cr = PRIVATE()->NativeContext;
	if (enable)
	{
		cairo_rectangle(cr, PRIVATE()->CilpRect.X, PRIVATE()->CilpRect.Y, PRIVATE()->CilpRect.W, PRIVATE()->CilpRect.H);
		cairo_clip(cr);
	}
	else
	{
		cairo_reset_clip(cr);
	}
}

void CairoUIPainter::setClipRect(float x, float y, float width, float height)
{
	auto cr = PRIVATE()->NativeContext;
	PRIVATE()->CilpRect = UIRect{ (float)x, (float)y, (float)width, (float)height };
}

void CairoUIPainter::setViewport(float x, float y, float width, float height)
{
	// TODO
}

void CairoUIPainter::shear(float sh, float sv)
{
	// TODO
}

void CairoUIPainter::rotate(float angle)
{
	auto cr = PRIVATE()->NativeContext;
	cairo_rotate(cr, angle * (M_PI / 180.0));
}

void CairoUIPainter::scale(float dx, float dy)
{
	auto cr = PRIVATE()->NativeContext;
	cairo_scale(cr, dx, dy);
}

void CairoUIPainter::translate(float dx, float dy)
{
	auto cr = PRIVATE()->NativeContext;
	cairo_translate(cr, dx, dy);
}

uint32_t CairoUIPainter::getWidth() const
{
	return cairo_image_surface_get_width(PRIVATE()->NativeSurface);
}

uint32_t CairoUIPainter::getHeight() const
{
	return cairo_image_surface_get_height(PRIVATE()->NativeSurface);
}

uint32_t CairoUIPainter::getStride() const
{
	return cairo_image_surface_get_stride(PRIVATE()->NativeSurface);
}

UIArrayView<const uint8_t> CairoUIPainter::getPixels() const
{
	return UIArrayView<const uint8_t>(cairo_image_surface_get_data(PRIVATE()->NativeSurface), getHeight() * getStride());
}

void CairoUIPainter::resize(uint32_t width, uint32_t height)
{
	g_object_unref(PRIVATE()->NativeLayout); PRIVATE()->NativeLayout = nullptr;
	cairo_destroy(PRIVATE()->NativeContext); PRIVATE()->NativeContext = nullptr;
	cairo_surface_destroy(PRIVATE()->NativeSurface); PRIVATE()->NativeSurface = nullptr;

	PRIVATE()->NativeSurface = cairo_image_surface_create(cairo_format_t::CAIRO_FORMAT_ARGB32, width, height);
	PRIVATE()->NativeContext = cairo_create(PRIVATE()->NativeSurface);
	auto cr = PRIVATE()->NativeContext;
	cairo_set_source_rgba(cr, 0, 0, 0, 1);
	cairo_set_antialias(cr, CAIRO_ANTIALIAS_BEST);
	cairo_paint(cr);

	PRIVATE()->NativeLayout = pango_cairo_create_layout(cr);
	auto& font = PRIVATE()->Font;
	auto layout = PRIVATE()->NativeLayout;
	auto font_desc = pango_font_description_new();
	pango_font_description_set_family(font_desc, font.Family.c_str());
	pango_font_description_set_style(font_desc, (PangoStyle)font.Style);
	pango_font_description_set_size(font_desc, font.Size * PANGO_SCALE);
	pango_font_description_set_weight(font_desc, (PangoWeight)font.Weight);
	pango_layout_set_font_description(layout, font_desc);
	pango_font_description_free(font_desc);
}