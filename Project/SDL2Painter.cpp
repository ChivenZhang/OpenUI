#include "SDL2Painter.h"

SDL2Painter::SDL2Painter(cairo_t* native)
	:
	m_NativeContext(native)
{
	/*{
		auto cr = native;
		cairo_set_source_rgba(cr, 104 / 255.0f, 33 / 255.0f, 122 / 255.0f, 255 / 255.0f);
		cairo_set_antialias(cr, CAIRO_ANTIALIAS_BEST);
		cairo_paint(cr);
		cairo_set_source_rgba(cr, 0, 0, 0, 1);
		cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
		cairo_set_font_size(cr, 90.0);

		cairo_move_to(cr, 10.0, 135.0);
		cairo_show_text(cr, "Hello");

		cairo_move_to(cr, 100.0, 180.0);
		cairo_text_path(cr, "OpenUI");
		cairo_set_source_rgb(cr, 0.5, 0.5, 1);
		cairo_fill_preserve(cr);
		cairo_set_source_rgb(cr, 0, 0, 0);
		cairo_set_line_width(cr, 2.56);
		cairo_stroke(cr);

		cairo_set_source_rgba(cr, 1, 0.2, 0.2, 0.6);
		cairo_arc(cr, 10.0, 135.0, 5.12, 0, 2 * M_PI);
		cairo_close_path(cr);
		cairo_arc(cr, 70.0, 165.0, 5.12, 0, 2 * M_PI);
		cairo_fill(cr);
	}*/
}

void SDL2Painter::drawArc(int x, int y, int width, int height, int startAngle, int spanAngle)
{
}

void SDL2Painter::drawChord(int x, int y, int width, int height, int startAngle, int spanAngle)
{
}

void SDL2Painter::drawEllipse(int x, int y, int width, int height)
{
}

void SDL2Painter::drawImage(int x, int y, RmImageRaw image, int sx, int sy, int sw, int sh)
{
}

void SDL2Painter::drawLine(int x1, int y1, int x2, int y2)
{
	auto cr = m_NativeContext;

	cairo_line_to(cr, x1, y1);
	cairo_line_to(cr, x2, y2);
	cairo_stroke(cr);
}

void SDL2Painter::drawLines(RmArrayView<RmLine> lines)
{
}

void SDL2Painter::drawPie(int x, int y, int width, int height, int startAngle, int spanAngle)
{
}

void SDL2Painter::drawPoint(int x, int y)
{
}

void SDL2Painter::drawPoints(RmArrayView<RmPoint> points)
{
}

void SDL2Painter::drawPolygon(RmArrayView<RmPoint> points)
{
}

void SDL2Painter::drawPolyline(RmArrayView<RmPoint> points)
{
}

void SDL2Painter::drawRect(int x, int y, int width, int height)
{
	auto cr = m_NativeContext;
	cairo_rectangle(cr, x, y, width, height);
	cairo_stroke(cr);
}

void SDL2Painter::drawRects(RmArrayView<RmRect> rects)
{
}

void SDL2Painter::drawText(int x, int y, int width, int height, int flags, const RmString& text, RmRectRaw boundingRect)
{
}

void SDL2Painter::setPen(const RmPen& pen)
{
	auto cr = m_NativeContext;
	cairo_set_source_rgba(cr, pen.Color.R, pen.Color.G, pen.Color.B, pen.Color.A);
}

void SDL2Painter::setBrush(const RmBrush& brush)
{
}

void SDL2Painter::setFont(const RmFont& font)
{
}

void SDL2Painter::setClipping(bool enable)
{
}

void SDL2Painter::setClipRect(int x, int y, int width, int height)
{
}

void SDL2Painter::setViewport(int x, int y, int width, int height)
{
}

void SDL2Painter::shear(float sh, float sv)
{
}

void SDL2Painter::rotate(float angle)
{
}

void SDL2Painter::scale(float dx, float dy)
{
}

void SDL2Painter::translate(float dx, float dy)
{
}
