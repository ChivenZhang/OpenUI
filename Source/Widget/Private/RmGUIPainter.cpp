#include "RmGUIPainter.h"

void RmGUIPainter::drawArc(int x, int y, int width, int height, int startAngle, int spanAngle)
{
}

void RmGUIPainter::drawChord(int x, int y, int width, int height, int startAngle, int spanAngle)
{
}

void RmGUIPainter::drawEllipse(int x, int y, int width, int height)
{
}

void RmGUIPainter::drawImage(int x, int y, rmimage image, int sx, int sy, int sw, int sh)
{
}

void RmGUIPainter::drawLine(int x1, int y1, int x2, int y2)
{
}

void RmGUIPainter::drawLines(RmArrayView<RmLine> lines)
{
}

void RmGUIPainter::drawPie(int x, int y, int width, int height, int startAngle, int spanAngle)
{
}

void RmGUIPainter::drawPoint(int x, int y)
{
}

void RmGUIPainter::drawPoints(RmArrayView<RmPoint> points)
{
}

void RmGUIPainter::drawPolygon(RmArrayView<RmPoint> points)
{
}

void RmGUIPainter::drawPolyline(RmArrayView<RmPoint> points)
{
}

void RmGUIPainter::drawRect(int x, int y, int width, int height)
{
}

void RmGUIPainter::drawRects(RmArrayView<RmRect> rects)
{
}

void RmGUIPainter::drawText(int x, int y, int width, int height, int flags, const RmString& text, RmRect* boundingRect)
{
}

void RmGUIPainter::setPen(const RmPen& pen)
{
}

void RmGUIPainter::setBrush(const RmBrush& brush)
{
}

void RmGUIPainter::setFont(const RmFont& font)
{
}

void RmGUIPainter::setClipping(bool enable)
{
}

void RmGUIPainter::setClipRect(int x, int y, int width, int height)
{
}

void RmGUIPainter::setViewport(int x, int y, int width, int height)
{
}

void RmGUIPainter::shear(float sh, float sv)
{
}

void RmGUIPainter::rotate(float angle)
{
}

void RmGUIPainter::scale(float dx, float dy)
{
}

void RmGUIPainter::translate(float dx, float dy)
{
}
