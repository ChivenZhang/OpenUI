#pragma once
#include "Widget/IRmGUIPainter.h"
#include <cairo/cairo.h>

class RmGUIPainter : public IRmGUIPainter
{
public:
	void drawArc(int x, int y, int width, int height, int startAngle, int spanAngle) override;
	void drawChord(int x, int y, int width, int height, int startAngle, int spanAngle) override;
	void drawEllipse(int x, int y, int width, int height) override;
	void drawImage(int x, int y, rmimage image, int sx, int sy, int sw, int sh) override;
	void drawLine(int x1, int y1, int x2, int y2) override;
	void drawLines(RmArrayView<RmLine> lines) override;
	void drawPie(int x, int y, int width, int height, int startAngle, int spanAngle) override;
	void drawPoint(int x, int y) override;
	void drawPoints(RmArrayView<RmPoint> points) override;
	void drawPolygon(RmArrayView<RmPoint> points) override;
	void drawPolyline(RmArrayView<RmPoint> points) override;
	void drawRect(int x, int y, int width, int height) override;
	void drawRects(RmArrayView<RmRect> rects) override;
	void drawText(int x, int y, int width, int height, int flags, const RmString& text, RmRect* boundingRect) override;
	void setPen(const RmPen& pen) override;
	void setBrush(const RmBrush& brush) override;
	void setFont(const RmFont& font) override;
	void setClipping(bool enable) override;
	void setClipRect(int x, int y, int width, int height) override;
	void setViewport(int x, int y, int width, int height) override;
	void shear(float sh, float sv) override;
	void rotate(float angle) override;
	void scale(float dx, float dy) override;
	void translate(float dx, float dy) override;

protected:
	RmRaw<_cairo> m_NativeContext;
};