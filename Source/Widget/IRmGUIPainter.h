#pragma once
#include "RmGUI.h"

struct RmRect
{
	float X, Y, W, H;
};
using rmrect = RmRaw<RmRect>;

struct RmPoint
{
	float X, Y;
};

struct RmLine
{
	RmPoint P0, P1;
};

struct RmPen
{
};

struct RmBrush
{
};

struct RmFont
{
};

struct RmFloat2
{
	float X, Y;
};

struct RmFloat3
{
	float X, Y, Z;
};

struct RmFloat4
{
	float X, Y, Z, W;
};

struct RmFloat2x2
{
	RmFloat2 X, Y;
};

struct RmFloat3x3
{
	RmFloat3 X, Y, Z;
};

struct RmFloat4x4
{
	RmFloat4 X, Y, Z, W;
};

class RmImage;
using rmimage = RmRaw<RmImage>;

/// @brief Base interface of all painters.
class RMGUI_API IRmGUIPainter
{
public:
	virtual void drawArc(int x, int y, int width, int height, int startAngle, int spanAngle) = 0;

	virtual void drawChord(int x, int y, int width, int height, int startAngle, int spanAngle) = 0;

	virtual void drawEllipse(int x, int y, int width, int height) = 0;

	virtual void drawImage(int x, int y, rmimage image, int sx = 0, int sy = 0, int sw = -1, int sh = -1) = 0;

	virtual void drawLine(int x1, int y1, int x2, int y2) = 0;

	virtual void drawLines(RmArrayView<RmLine> lines) = 0;

	virtual void drawPie(int x, int y, int width, int height, int startAngle, int spanAngle) = 0;

	virtual void drawPoint(int x, int y) = 0;

	virtual void drawPoints(RmArrayView<RmPoint> points) = 0;

	virtual void drawPolygon(RmArrayView<RmPoint> points) = 0;

	virtual void drawPolyline(RmArrayView<RmPoint> points) = 0;

	virtual void drawRect(int x, int y, int width, int height) = 0;

	virtual void drawRects(RmArrayView<RmRect> rects) = 0;

	virtual void drawText(int x, int y, int width, int height, int flags, const RmString& text, RmRect* boundingRect = nullptr) = 0;

	virtual void setPen(const RmPen& pen) = 0;

	virtual void setBrush(const RmBrush& brush) = 0;

	virtual void setFont(const RmFont& font) = 0;

	virtual void setClipping(bool enable) = 0;

	virtual void setClipRect(int x, int y, int width, int height) = 0;

	virtual void setViewport(int x, int y, int width, int height) = 0;

	virtual void shear(float sh, float sv) = 0;

	virtual void rotate(float angle) = 0;

	virtual void scale(float dx, float dy) = 0;

	virtual void translate(float dx, float dy) = 0;

};
using rmpainter = RmRaw<IRmGUIPainter>;