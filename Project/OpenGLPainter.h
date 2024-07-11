#pragma once
#include <UIPainter.h>
#include <cairo/cairo.h>
#include <pango/pangocairo.h>

class OpenGLPainter : public UIPainter
{
public:
	OpenGLPainter(uint32_t width, uint32_t height);
	~OpenGLPainter();
	virtual UIRect boundingRect(int x, int y, int width, int height, UIString const& text, int cursor, int* row, int* column, UIRectRaw cursorRect) override;
	virtual UIRect boundingRect(int x, int y, int width, int height, UIString const& text, int row, int column, int* cursor, UIRectRaw cursorRect) override;
	virtual UIRect boundingRect(int x, int y, int width, int height, UIString const& text, int posX, int posY, int* row, int* column, int* cursor, UIRectRaw cursorRect) override;
	virtual void drawArc(int x, int y, int width, int height, int startAngle, int spanAngle) override;
	virtual void drawChord(int x, int y, int width, int height, int startAngle, int spanAngle) override;
	virtual void drawEllipse(int x, int y, int width, int height) override;
	virtual void drawImage(int x, int y, UIImage image, int sx, int sy, int sw, int sh) override;
	virtual void drawLine(int x1, int y1, int x2, int y2) override;
	virtual void drawLines(UIArrayView<UILine> lines) override;
	virtual void drawPie(int x, int y, int width, int height, int startAngle, int spanAngle) override;
	virtual void drawPoint(int x, int y) override;
	virtual void drawPoints(UIArrayView<UIPoint> points) override;
	virtual void drawPolygon(UIArrayView<UIPoint> points) override;
	virtual void drawPolyline(UIArrayView<UIPoint> points) override;
	virtual void drawRect(int x, int y, int width, int height) override;
	virtual void drawRects(UIArrayView<UIRect> rects) override;
	virtual void drawRoundedRect(int x, int y, int width, int height, float xRadius, float yRadius) override;
	virtual void drawText(int x, int y, int width, int height, const UIString& text, UIRectRaw boundingRect, int cursor, UIRectRaw cursorRect) override;
	virtual void setPen(const UIPen& pen) override;
	virtual void setBrush(const UIBrush& brush) override;
	virtual void setFont(const UIFont& font) override;
	virtual void setClipping(bool enable) override;
	virtual void setClipRect(int x, int y, int width, int height) override;
	virtual void setViewport(int x, int y, int width, int height) override;
	virtual void shear(float sh, float sv) override;
	virtual void rotate(float angle) override;
	virtual void scale(float dx, float dy) override;
	virtual void translate(float dx, float dy) override;
	virtual uint32_t getWidth() const override;
	virtual uint32_t getHeight() const override;
	virtual uint32_t getStride() const override;
	virtual UIArrayView<const uint8_t> getPixelData() const override;
	virtual void resize(uint32_t width, uint32_t height) override;
	uint32_t getTexture() const;
	uint32_t getTextureUpdated() const;

protected:
	uint32_t m_NativeTexture;
	cairo_t* m_NativeContext;
	PangoLayout* m_NativeLayout;
	cairo_surface_t* m_NativeSurface;
	UIPen m_Pen;
	UIFont m_Font;
	UIBrush m_Brush;
	UIRect m_CilpRect;
	bool m_EnableCilp = false;
};