#pragma once
#include "Widget/IRmGUIPainter.h"
#include <cairo/cairo.h>
#include <pango/pangocairo.h>

class SDL2Painter : public IRmGUIPainter
{
public:
	SDL2Painter(uint32_t width, uint32_t height);
	~SDL2Painter();
	virtual uint32_t getWidth() const override;
	virtual uint32_t getHeight() const override;
	virtual uint32_t getStride() const override;
	virtual RmArrayView<const uint8_t> getPixelData() const override;
	virtual void resize(uint32_t width, uint32_t height) override;
	virtual RmRect boundingRect(int x, int y, int width, int height, RmString const& text, int cursor, RmRectRaw cursorRect) override;
	virtual void drawArc(int x, int y, int width, int height, int startAngle, int spanAngle) override;
	virtual void drawChord(int x, int y, int width, int height, int startAngle, int spanAngle) override;
	virtual void drawEllipse(int x, int y, int width, int height) override;
	virtual void drawImage(int x, int y, RmImageRaw image, int sx, int sy, int sw, int sh) override;
	virtual void drawLine(int x1, int y1, int x2, int y2) override;
	virtual void drawLines(RmArrayView<RmLine> lines) override;
	virtual void drawPie(int x, int y, int width, int height, int startAngle, int spanAngle) override;
	virtual void drawPoint(int x, int y) override;
	virtual void drawPoints(RmArrayView<RmPoint> points) override;
	virtual void drawPolygon(RmArrayView<RmPoint> points) override;
	virtual void drawPolyline(RmArrayView<RmPoint> points) override;
	virtual void drawRect(int x, int y, int width, int height) override;
	virtual void drawRects(RmArrayView<RmRect> rects) override;
	virtual void drawRoundedRect(int x, int y, int width, int height, float xRadius, float yRadius) override;
	virtual void drawText(int x, int y, int width, int height, const RmString& text, RmRectRaw boundingRect, int cursor, RmRectRaw cursorRect) override;
	virtual void setPen(const RmPen& pen) override;
	virtual void setBrush(const RmBrush& brush) override;
	virtual void setFont(const RmFont& font) override;
	virtual void setClipping(bool enable) override;
	virtual void setClipRect(int x, int y, int width, int height) override;
	virtual void setViewport(int x, int y, int width, int height) override;
	virtual void shear(float sh, float sv) override;
	virtual void rotate(float angle) override;
	virtual void scale(float dx, float dy) override;
	virtual void translate(float dx, float dy) override;

protected:
	cairo_t* m_NativeContext;
	PangoLayout* m_NativeLayout;
	cairo_surface_t* m_NativeSurface;
	RmPen m_Pen;
	RmBrush m_Brush;
	RmFont m_Font;
	RmRect m_CilpRect;
	bool m_EnableCilp = false;
};