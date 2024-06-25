#include "SDL2Painter.h"

SDL2Painter::SDL2Painter(cairo_t* native)
	:
	m_NativeContext(native)
{
	auto cr = native;
	cairo_set_source_rgba(cr, 0, 0, 0, 1);
	cairo_set_antialias(cr, CAIRO_ANTIALIAS_BEST);
	cairo_paint(cr);

	m_NativeLayout = pango_cairo_create_layout(cr);
	auto& font = m_Font;
	auto layout = m_NativeLayout;
	auto font_desc = pango_font_description_new();
	pango_font_description_set_family(font_desc, font.Family.c_str());
	pango_font_description_set_style(font_desc, (PangoStyle)font.Style);
	pango_font_description_set_size(font_desc, font.Size * PANGO_SCALE);
	pango_font_description_set_weight(font_desc, (PangoWeight)font.Weight);
	pango_layout_set_font_description(layout, font_desc);
	pango_font_description_free(font_desc);
}

SDL2Painter::~SDL2Painter()
{
	g_object_unref(m_NativeLayout); m_NativeLayout = nullptr;
}

RmRect SDL2Painter::boundingRect(int x, int y, int width, int height, RmString const& text)
{
	RmRect result;
	auto layout = m_NativeLayout;
	auto& font = m_Font;

	pango_layout_set_text(layout, text.c_str(), -1);
	pango_layout_set_width(layout, width * PANGO_SCALE);
	pango_layout_set_height(layout, height * PANGO_SCALE);

	int text_width, text_height;
	int baseline = pango_layout_get_baseline(layout);
	pango_layout_get_pixel_size(layout, &text_width, &text_height);
	result.W = text_width; result.H = text_height;

	if (font.Align & RmFont::AlignTop) { result.X = x; result.Y = y; }
	else if (font.Align & RmFont::AlignBottom) { result.X = x; result.Y = y + height - text_height; }
	else if (font.Align & RmFont::AlignVCenter) { result.X = x; result.Y = y + (height - text_height) * 0.5; }
	else if (font.Align & RmFont::AlignBaseline) { result.X = x; result.Y = y + baseline; }
	else { result.X = x; result.Y = y; }

	return result;
}

void SDL2Painter::drawArc(int x, int y, int width, int height, int startAngle, int spanAngle)
{
	if (width <= 0 || height <= 0) return;
	auto cr = m_NativeContext;
	if (m_Brush.Style != RmBrush::NoBrush)
	{
		cairo_save(cr); setClipping(true);
		auto scale_factor = 1.0 * height / width;
		cairo_scale(cr, 1.0, scale_factor);
		cairo_translate(cr, x, y / scale_factor);
		cairo_arc(cr, 0, 0, width * 0.5, startAngle * (M_PI / 180.0), spanAngle * (M_PI / 180.0));
		cairo_set_source_rgba(cr, m_Brush.Color.R, m_Brush.Color.G, m_Brush.Color.B, m_Brush.Color.A);
		cairo_fill(cr);
		cairo_restore(cr);
	}
	if (m_Pen.Style != RmPen::NoPen)
	{
		cairo_save(cr); setClipping(true);
		auto scale_factor = 1.0 * height / width;
		cairo_scale(cr, 1.0, scale_factor);
		cairo_translate(cr, x, y / scale_factor);
		cairo_arc(cr, 0, 0, width * 0.5, startAngle * (M_PI / 180.0), spanAngle * (M_PI / 180.0));
		cairo_set_line_width(cr, m_Pen.Width);
		cairo_set_source_rgba(cr, m_Pen.Color.R, m_Pen.Color.G, m_Pen.Color.B, m_Pen.Color.A);
		cairo_stroke(cr);
		cairo_restore(cr);
	}
}

void SDL2Painter::drawChord(int x, int y, int width, int height, int startAngle, int spanAngle)
{
	if (width <= 0 || height <= 0) return;
	auto cr = m_NativeContext;
	if (m_Brush.Style != RmBrush::NoBrush)
	{
		cairo_save(cr); setClipping(true);
		auto scale_factor = 1.0 * height / width;
		cairo_scale(cr, 1.0, scale_factor);
		cairo_translate(cr, x, y / scale_factor);
		cairo_arc(cr, 0, 0, width * 0.5, startAngle * (M_PI / 180.0), spanAngle * (M_PI / 180.0));
		cairo_set_source_rgba(cr, m_Brush.Color.R, m_Brush.Color.G, m_Brush.Color.B, m_Brush.Color.A);
		cairo_fill(cr);
		cairo_restore(cr);
	}
	if (m_Pen.Style != RmPen::NoPen)
	{
		cairo_save(cr); setClipping(true);
		auto scale_factor = 1.0 * height / width;
		cairo_scale(cr, 1.0, scale_factor);
		cairo_translate(cr, x, y / scale_factor);
		cairo_arc(cr, 0, 0, width * 0.5, startAngle * (M_PI / 180.0), spanAngle * (M_PI / 180.0));
		cairo_close_path(cr);
		cairo_set_line_width(cr, m_Pen.Width);
		cairo_set_source_rgba(cr, m_Pen.Color.R, m_Pen.Color.G, m_Pen.Color.B, m_Pen.Color.A);
		cairo_stroke(cr);
		cairo_restore(cr);
	}
}

void SDL2Painter::drawEllipse(int x, int y, int width, int height)
{
	drawArc(x, y, width, height, 0, 360);
}

void SDL2Painter::drawImage(int x, int y, RmImageRaw image, int sx, int sy, int sw, int sh)
{
}

void SDL2Painter::drawLine(int x1, int y1, int x2, int y2)
{
	auto cr = m_NativeContext;
	if (m_Pen.Style != RmPen::NoPen)
	{
		cairo_save(cr); setClipping(true);
		cairo_set_line_width(cr, m_Pen.Width);
		cairo_set_source_rgba(cr, m_Pen.Color.R, m_Pen.Color.G, m_Pen.Color.B, m_Pen.Color.A);
		cairo_move_to(cr, x1, y1);
		cairo_line_to(cr, x2, y2);
		cairo_stroke(cr);
		cairo_restore(cr);
	}
}

void SDL2Painter::drawLines(RmArrayView<RmLine> lines)
{
	auto cr = m_NativeContext;
	if (m_Pen.Style != RmPen::NoPen && 2 <= lines.size())
	{
		cairo_save(cr); setClipping(true);
		cairo_set_line_width(cr, m_Pen.Width);
		cairo_set_source_rgba(cr, m_Pen.Color.R, m_Pen.Color.G, m_Pen.Color.B, m_Pen.Color.A);
		for (size_t i = 0; i < lines.size(); ++i)
		{
			if (i == 0) cairo_move_to(cr, lines[i].P0.X, lines[i].P0.Y);
			else cairo_line_to(cr, lines[i].P1.X, lines[i].P1.Y);
		}
		cairo_stroke(cr);
		cairo_restore(cr);
	}
}

void SDL2Painter::drawPie(int x, int y, int width, int height, int startAngle, int spanAngle)
{
	if (width <= 0 || height <= 0) return;
	auto cr = m_NativeContext;
	if (m_Brush.Style != RmBrush::NoBrush)
	{
		cairo_save(cr); setClipping(true);
		auto scale_factor = 1.0 * height / width;
		cairo_scale(cr, 1.0, scale_factor);
		cairo_translate(cr, x, y / scale_factor);
		cairo_move_to(cr, 0, 0);
		cairo_arc(cr, 0, 0, width * 0.5, startAngle * (M_PI / 180.0), spanAngle * (M_PI / 180.0));
		cairo_set_source_rgba(cr, m_Brush.Color.R, m_Brush.Color.G, m_Brush.Color.B, m_Brush.Color.A);
		cairo_fill(cr);
		cairo_restore(cr);
	}
	if (m_Pen.Style != RmPen::NoPen)
	{
		cairo_save(cr); setClipping(true);
		auto scale_factor = 1.0 * height / width;
		cairo_scale(cr, 1.0, scale_factor);
		cairo_translate(cr, x, y / scale_factor);
		cairo_move_to(cr, 0, 0);
		cairo_arc(cr, 0, 0, width * 0.5, startAngle * (M_PI / 180.0), spanAngle * (M_PI / 180.0));
		cairo_close_path(cr);
		cairo_set_line_width(cr, m_Pen.Width);
		cairo_set_source_rgba(cr, m_Pen.Color.R, m_Pen.Color.G, m_Pen.Color.B, m_Pen.Color.A);
		cairo_stroke(cr);
		cairo_restore(cr);
	}
}

void SDL2Painter::drawPoint(int x, int y)
{
	auto cr = m_NativeContext;
	if (m_Brush.Style != RmBrush::NoBrush)
	{
		cairo_save(cr); setClipping(true);
		cairo_rectangle(cr, x - 1, y - 1, 2, 2);
		cairo_set_source_rgba(cr, m_Brush.Color.R, m_Brush.Color.G, m_Brush.Color.B, m_Brush.Color.A);
		cairo_fill(cr);
		cairo_restore(cr);
	}
	if (m_Pen.Style != RmPen::NoPen)
	{
		cairo_save(cr); setClipping(true);
		cairo_rectangle(cr, x - 1, y - 1, 2, 2);
		cairo_set_line_width(cr, m_Pen.Width);
		cairo_set_source_rgba(cr, m_Pen.Color.R, m_Pen.Color.G, m_Pen.Color.B, m_Pen.Color.A);
		cairo_stroke(cr);
		cairo_restore(cr);
	}
}

void SDL2Painter::drawPoints(RmArrayView<RmPoint> points)
{
	auto cr = m_NativeContext;
	if (m_Brush.Style != RmBrush::NoBrush)
	{
		cairo_save(cr); setClipping(true);
		for (size_t i = 0; i < points.size(); ++i) cairo_rectangle(cr, points[i].X - 1, points[i].Y - 1, 2, 2);
		cairo_set_source_rgba(cr, m_Brush.Color.R, m_Brush.Color.G, m_Brush.Color.B, m_Brush.Color.A);
		cairo_fill(cr);
		cairo_restore(cr);
	}
	if (m_Pen.Style != RmPen::NoPen)
	{
		cairo_save(cr); setClipping(true);
		for (size_t i = 0; i < points.size(); ++i) cairo_rectangle(cr, points[i].X - 1, points[i].Y - 1, 2, 2);
		cairo_set_line_width(cr, m_Pen.Width);
		cairo_set_source_rgba(cr, m_Pen.Color.R, m_Pen.Color.G, m_Pen.Color.B, m_Pen.Color.A);
		cairo_stroke(cr);
		cairo_restore(cr);
	}
}

void SDL2Painter::drawPolygon(RmArrayView<RmPoint> points)
{
	auto cr = m_NativeContext;
	if (m_Brush.Style != RmBrush::NoBrush && 2 <= points.size())
	{
		cairo_save(cr); setClipping(true);
		for (size_t i = 0; i < points.size(); ++i)
		{
			if (i == 0) cairo_move_to(cr, points[i].X, points[i].Y);
			else cairo_line_to(cr, points[i].X, points[i].Y);
		}
		cairo_set_source_rgba(cr, m_Brush.Color.R, m_Brush.Color.G, m_Brush.Color.B, m_Brush.Color.A);
		cairo_fill(cr);
		cairo_restore(cr);
	}
	if (m_Pen.Style != RmPen::NoPen && 2 <= points.size())
	{
		cairo_save(cr); setClipping(true);
		for (size_t i = 0; i < points.size(); ++i)
		{
			if (i == 0) cairo_move_to(cr, points[i].X, points[i].Y);
			else cairo_line_to(cr, points[i].X, points[i].Y);
		}
		cairo_close_path(cr);
		cairo_set_line_width(cr, m_Pen.Width);
		cairo_set_source_rgba(cr, m_Pen.Color.R, m_Pen.Color.G, m_Pen.Color.B, m_Pen.Color.A);
		cairo_stroke(cr);
		cairo_restore(cr);
	}
}

void SDL2Painter::drawPolyline(RmArrayView<RmPoint> points)
{
	auto cr = m_NativeContext;
	if (m_Pen.Style != RmPen::NoPen && 2 <= points.size())
	{
		cairo_save(cr); setClipping(true);
		for (size_t i = 0; i < points.size(); ++i)
		{
			if (i == 0) cairo_move_to(cr, points[i].X, points[i].Y);
			else cairo_line_to(cr, points[i].X, points[i].Y);
		}
		cairo_set_line_width(cr, m_Pen.Width);
		cairo_set_source_rgba(cr, m_Pen.Color.R, m_Pen.Color.G, m_Pen.Color.B, m_Pen.Color.A);
		cairo_stroke(cr);
		cairo_restore(cr);
	}
}

void SDL2Painter::drawRect(int x, int y, int width, int height)
{
	auto cr = m_NativeContext;
	if (m_Brush.Style != RmBrush::NoBrush && 0 <= width && 0 <= height)
	{
		cairo_save(cr); setClipping(true);
		cairo_rectangle(cr, x, y, width, height);
		cairo_set_source_rgba(cr, m_Brush.Color.R, m_Brush.Color.G, m_Brush.Color.B, m_Brush.Color.A);
		cairo_fill(cr);
		cairo_restore(cr);
	}
	if (m_Pen.Style != RmPen::NoPen && 0 <= width && 0 <= height)
	{
		cairo_save(cr); setClipping(true);
		cairo_rectangle(cr, x, y, width, height);
		cairo_set_line_width(cr, m_Pen.Width);
		cairo_set_source_rgba(cr, m_Pen.Color.R, m_Pen.Color.G, m_Pen.Color.B, m_Pen.Color.A);
		cairo_stroke(cr);
		cairo_restore(cr);
	}
}

void SDL2Painter::drawRects(RmArrayView<RmRect> rects)
{
	auto cr = m_NativeContext;
	if (m_Brush.Style != RmBrush::NoBrush)
	{
		cairo_save(cr); setClipping(true);
		for (size_t i = 0; i < rects.size(); ++i)
		{
			if (0 < rects[i].W && 0 < rects[i].H) cairo_rectangle(cr, rects[i].X, rects[i].Y, rects[i].W, rects[i].H);
		}
		cairo_set_source_rgba(cr, m_Brush.Color.R, m_Brush.Color.G, m_Brush.Color.B, m_Brush.Color.A);
		cairo_fill(cr);
		cairo_restore(cr);
	}
	if (m_Pen.Style != RmPen::NoPen)
	{
		cairo_save(cr); setClipping(true);
		for (size_t i = 0; i < rects.size(); ++i)
		{
			if (0 < rects[i].W && 0 < rects[i].H) cairo_rectangle(cr, rects[i].X, rects[i].Y, rects[i].W, rects[i].H);
		}
		cairo_set_line_width(cr, m_Pen.Width);
		cairo_set_source_rgba(cr, m_Pen.Color.R, m_Pen.Color.G, m_Pen.Color.B, m_Pen.Color.A);
		cairo_stroke(cr);
		cairo_restore(cr);
	}
}

void SDL2Painter::drawRoundedRect(int x, int y, int width, int height, float xRadius, float yRadius)
{
	auto cr = m_NativeContext;
	if (m_Brush.Style != RmBrush::NoBrush && 0 < width && 0 < height)
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
		cairo_set_source_rgba(cr, m_Brush.Color.R, m_Brush.Color.G, m_Brush.Color.B, m_Brush.Color.A);
		cairo_fill(cr);
		cairo_restore(cr);
	}
	if (m_Pen.Style != RmPen::NoPen && 0 < width && 0 < height)
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
		cairo_set_line_width(cr, m_Pen.Width);
		cairo_set_source_rgba(cr, m_Pen.Color.R, m_Pen.Color.G, m_Pen.Color.B, m_Pen.Color.A);
		cairo_stroke(cr);
		cairo_restore(cr);
	}
}

void SDL2Painter::drawText(int x, int y, int width, int height, const RmString& text, RmRectRaw boundingRect)
{
	auto cr = m_NativeContext;
	auto layout = m_NativeLayout;
	auto& font = m_Font;
	if (m_Brush.Style != RmBrush::NoBrush)
	{
		cairo_save(cr); setClipping(true);
		pango_layout_set_text(layout, text.c_str(), -1);
		pango_layout_set_width(layout, width * PANGO_SCALE);
		pango_layout_set_height(layout, height * PANGO_SCALE);

		int text_width, text_height;
		auto baseline = pango_layout_get_baseline(layout);
		pango_layout_get_pixel_size(layout, &text_width, &text_height);

		if (font.Align & RmFont::AlignTop) cairo_move_to(cr, x, y);
		else if (font.Align & RmFont::AlignBottom) cairo_move_to(cr, x, y + height - text_height);
		else if (font.Align & RmFont::AlignVCenter) cairo_move_to(cr, x, y + (height - text_height) * 0.5);
		else if (font.Align & RmFont::AlignBaseline) cairo_move_to(cr, x, y + baseline);
		else cairo_move_to(cr, x, y);

		cairo_set_source_rgba(cr, m_Brush.Color.R, m_Brush.Color.G, m_Brush.Color.B, m_Brush.Color.A);
		pango_cairo_show_layout(cr, layout);
		cairo_restore(cr);
	}
	if (m_Pen.Style != RmPen::NoPen)
	{
		cairo_save(cr); setClipping(true);
		pango_layout_set_width(layout, width * PANGO_SCALE);
		pango_layout_set_height(layout, height * PANGO_SCALE);

		int text_width, text_height;
		auto baseline = pango_layout_get_baseline(layout);
		pango_layout_get_pixel_size(layout, &text_width, &text_height);

		if (font.Align & RmFont::AlignTop) cairo_move_to(cr, x, y);
		else if (font.Align & RmFont::AlignBottom) cairo_move_to(cr, x, y + height - text_height);
		else if (font.Align & RmFont::AlignVCenter) cairo_move_to(cr, x, y + (height - text_height) * 0.5);
		else if (font.Align & RmFont::AlignBaseline) cairo_move_to(cr, x, y + baseline);
		else cairo_move_to(cr, x, y);

		pango_cairo_layout_path(cr, layout);
		cairo_set_line_width(cr, m_Pen.Width);
		cairo_set_source_rgba(cr, m_Pen.Color.R, m_Pen.Color.G, m_Pen.Color.B, m_Pen.Color.A);
		cairo_stroke(cr);
		cairo_restore(cr);
	}
}

void SDL2Painter::setPen(const RmPen& pen)
{
	m_Pen = pen;
}

void SDL2Painter::setBrush(const RmBrush& brush)
{
	m_Brush = brush;
}

void SDL2Painter::setFont(const RmFont& font)
{
	auto layout = m_NativeLayout;
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

	if (font.Align & RmFont::AlignLeft)
	{
		pango_layout_set_justify(layout, false);
		pango_layout_set_alignment(layout, PangoAlignment::PANGO_ALIGN_LEFT);
	}
	else if (font.Align & RmFont::AlignRight)
	{
		pango_layout_set_justify(layout, false);
		pango_layout_set_alignment(layout, PangoAlignment::PANGO_ALIGN_RIGHT);
	}
	else if (font.Align & RmFont::AlignCenter)
	{
		pango_layout_set_justify(layout, false);
		pango_layout_set_alignment(layout, PangoAlignment::PANGO_ALIGN_CENTER);
	}
	else if (font.Align & RmFont::AlignJustify) pango_layout_set_justify(layout, true);

	switch (font.Direction)
	{
	case RmFont::DirectionAutoLayout: pango_layout_set_auto_dir(layout, true); break;
	default: pango_layout_set_auto_dir(layout, false); break;
	}
	switch (font.Ellipsize)
	{
	case RmFont::EllipsizeNone: pango_layout_set_ellipsize(layout, PangoEllipsizeMode::PANGO_ELLIPSIZE_NONE); break;
	case RmFont::EllipsizeStart: pango_layout_set_ellipsize(layout, PangoEllipsizeMode::PANGO_ELLIPSIZE_START); break;
	case RmFont::EllipsizeMiddle: pango_layout_set_ellipsize(layout, PangoEllipsizeMode::PANGO_ELLIPSIZE_MIDDLE); break;
	case RmFont::EllipsizeEnd: pango_layout_set_ellipsize(layout, PangoEllipsizeMode::PANGO_ELLIPSIZE_END); break;
	}
	pango_layout_set_spacing(layout, font.Spacing);
	pango_layout_set_line_spacing(layout, font.LineSpacing);
	m_Font = font;
}

void SDL2Painter::setClipping(bool enable)
{
	auto cr = m_NativeContext;
	if (enable)
	{
		cairo_rectangle(cr, m_CilpRect.X, m_CilpRect.Y, m_CilpRect.W, m_CilpRect.H);
		cairo_clip(cr);
	}
	else
	{
		cairo_reset_clip(cr);
	}
}

void SDL2Painter::setClipRect(int x, int y, int width, int height)
{
	auto cr = m_NativeContext;
	m_CilpRect = RmRect{ (float)x, (float)y, (float)width, (float)height };
}

void SDL2Painter::setViewport(int x, int y, int width, int height)
{
	// TODO
}

void SDL2Painter::shear(float sh, float sv)
{
	// TODO
}

void SDL2Painter::rotate(float angle)
{
	auto cr = m_NativeContext;
	cairo_rotate(cr, angle * (M_PI / 180.0));
}

void SDL2Painter::scale(float dx, float dy)
{
	auto cr = m_NativeContext;
	cairo_scale(cr, dx, dy);
}

void SDL2Painter::translate(float dx, float dy)
{
	auto cr = m_NativeContext;
	cairo_translate(cr, dx, dy);
}