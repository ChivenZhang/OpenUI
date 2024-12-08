#pragma once
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
#include <OpenUI/UIPainter.h>

class CairoUIPainter : public UIPainter
{
public:
	CairoUIPainter(uint32_t width, uint32_t height);
	~CairoUIPainter();
	virtual UIRect boundingRect(float x, float y, float width, float height, UIString const& text, float cursor, UIRectRaw cursorRect) override;
	virtual UIRect boundingRect(float x, float y, float width, float height, UIString const& text, float posX, float posY, int* cursor, UIRectRaw cursorRect) override;
	virtual void drawArc(float x, float y, float width, float height, float startAngle, float spanAngle) override;
	virtual void drawChord(float x, float y, float width, float height, float startAngle, float spanAngle) override;
	virtual void drawEllipse(float x, float y, float width, float height) override;
	virtual void drawImage(float x, float y, UIImage image, float sx, float sy, float sw, float sh) override;
	virtual void drawLine(float x1, float y1, float x2, float y2) override;
	virtual void drawLines(UIArrayView<UILine> lines) override;
	virtual void drawPie(float x, float y, float width, float height, float startAngle, float spanAngle) override;
	virtual void drawPoint(float x, float y) override;
	virtual void drawPoints(UIArrayView<UIPoint> points) override;
	virtual void drawPolygon(UIArrayView<UIPoint> points) override;
	virtual void drawPolyline(UIArrayView<UIPoint> points) override;
	virtual void drawRect(float x, float y, float width, float height) override;
	virtual void drawRects(UIArrayView<UIRect> rects) override;
	virtual void drawRoundedRect(float x, float y, float width, float height, float xRadius, float yRadius) override;
	virtual void drawText(float x, float y, float width, float height, const UIString& text, UIRectRaw boundingRect, float cursor, UIRectRaw cursorRect) override;
	virtual UIPen const& getPen() const override;
	virtual void setPen(const UIPen& pen) override;
	virtual UIBrush const& getBrush() const override;
	virtual void setBrush(const UIBrush& brush) override;
	virtual UIFont const& getFont() const override;
	virtual void setFont(const UIFont& font) override;
	virtual void setClipping(bool enable) override;
	virtual void setClipRect(float x, float y, float width, float height) override;
	virtual void setViewport(float x, float y, float width, float height) override;
	virtual void shear(float sh, float sv) override;
	virtual void rotate(float angle) override;
	virtual void scale(float dx, float dy) override;
	virtual void translate(float dx, float dy) override;
	virtual uint32_t getWidth() const override;
	virtual uint32_t getHeight() const override;
	virtual uint32_t getStride() const override;
	virtual UIArrayView<const uint8_t> getPixelData() const override;
	virtual void resize(uint32_t width, uint32_t height) override;

private:
	UIPainterPrivateRaw m_Private;
};
