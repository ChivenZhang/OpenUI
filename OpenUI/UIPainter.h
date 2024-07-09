#pragma once
#include "UI.h"

/// @brief 
class OPENUI_API UIPainter
{
public:
	virtual ~UIPainter() = default;

	virtual UIRect boundingRect(int x, int y, int width, int height, UIString const& text, int cursor, int* row, int* column, UIRectRaw cursorRect = nullptr) = 0;

	virtual UIRect boundingRect(int x, int y, int width, int height, UIString const& text, int row, int column, int* cursor, UIRectRaw cursorRect = nullptr) = 0;

	virtual UIRect boundingRect(int x, int y, int width, int height, UIString const& text, int posX, int posY, int* row, int* column, int* cursor, UIRectRaw cursorRect = nullptr) = 0;

	virtual void drawArc(int x, int y, int width, int height, int startAngle, int spanAngle) = 0;

	virtual void drawChord(int x, int y, int width, int height, int startAngle, int spanAngle) = 0;

	virtual void drawEllipse(int x, int y, int width, int height) = 0;

	virtual void drawImage(int x, int y, UIImage image, int sx = 0, int sy = 0, int sw = -1, int sh = -1) = 0;

	virtual void drawLine(int x1, int y1, int x2, int y2) = 0;

	virtual void drawLines(UIArrayView<UILine> lines) = 0;

	virtual void drawPie(int x, int y, int width, int height, int startAngle, int spanAngle) = 0;

	virtual void drawPoint(int x, int y) = 0;

	virtual void drawPoints(UIArrayView<UIPoint> points) = 0;

	virtual void drawPolygon(UIArrayView<UIPoint> points) = 0;

	virtual void drawPolyline(UIArrayView<UIPoint> points) = 0;

	virtual void drawRect(int x, int y, int width, int height) = 0;

	virtual void drawRects(UIArrayView<UIRect> rects) = 0;

	virtual void drawRoundedRect(int x, int y, int w, int h, float xRadius, float yRadius) = 0;

	virtual void drawText(int x, int y, int width, int height, const UIString& text, UIRectRaw boundingRect = nullptr, int cursor = 0, UIRectRaw cursorRect = nullptr) = 0;

	virtual void setPen(const UIPen& pen) = 0;

	virtual void setBrush(const UIBrush& brush) = 0;

	virtual void setFont(const UIFont& font) = 0;

	virtual void setClipping(bool enable) = 0;

	// virtual void setClipPath(UIArrayView<UIPoint> points) = 0;

	virtual void setClipRect(int x, int y, int width, int height) = 0;

	virtual void setViewport(int x, int y, int width, int height) = 0;

	virtual void shear(float sh, float sv) = 0;

	virtual void rotate(float angle) = 0;

	virtual void scale(float dx, float dy) = 0;

	virtual void translate(float dx, float dy) = 0;

	virtual uint32_t getWidth() const = 0;

	virtual uint32_t getHeight() const = 0;

	virtual uint32_t getStride() const = 0;

	virtual UIArrayView<const uint8_t> getPixelData() const = 0;

	virtual void resize(uint32_t width, uint32_t height) = 0;
};
using UIPainterRef = UIRef<UIPainter>;
using UIPainterRaw = UIRaw<UIPainter>;