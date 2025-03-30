#pragma once
/*=================================================
* Copyright © 2020-2025 ChivenZhang.
* All Rights Reserved.
* =====================Note=========================
*
*
* ====================History=======================
* Created by ChivenZhang@gmail.com.
*
* =================================================*/
#include <OpenUI/UIPainter.h>

class CairoUIPainter : public UIPainter
{
public:
	CairoUIPainter(uint32_t width, uint32_t height);
	~CairoUIPainter() override;

	// Public API

	UIRect boundingRect(float x, float y, float width, float height, UIString const& text, float cursor, UIRectRaw cursorRect) override;
	UIRect boundingRect(float x, float y, float width, float height, UIString const& text, float posX, float posY, int* cursor, UIRectRaw cursorRect) override;
	void drawArc(float x, float y, float width, float height, float startAngle, float spanAngle) override;
	void drawChord(float x, float y, float width, float height, float startAngle, float spanAngle) override;
	void drawEllipse(float x, float y, float width, float height) override;
	void drawImage(float x, float y, UIImage image, float sx, float sy, float sw, float sh) override;
	void drawLine(float x1, float y1, float x2, float y2) override;
	void drawLines(UIListView<UILine> lines) override;
	void drawPie(float x, float y, float width, float height, float startAngle, float spanAngle) override;
	void drawPoint(float x, float y) override;
	void drawPoints(UIListView<UIPoint> points) override;
	void drawPolygon(UIListView<UIPoint> points) override;
	void drawPolyline(UIListView<UIPoint> points) override;
	void drawRect(float x, float y, float width, float height) override;
	void drawRects(UIListView<UIRect> rects) override;
	void drawRoundedRect(float x, float y, float width, float height, float xRadius, float yRadius) override;
	void drawText(float x, float y, float width, float height, const UIString& text, UIRectRaw boundingRect, float cursor, UIRectRaw cursorRect) override;
	UIPen const& getPen() const override;
	void setPen(const UIPen& pen) override;
	UIBrush const& getBrush() const override;
	void setBrush(const UIBrush& brush) override;
	UIFont const& getFont() const override;
	void setFont(const UIFont& font) override;
	void setClipping(bool enable) override;
	void setClipRect(float x, float y, float width, float height) override;
	void setViewport(float x, float y, float width, float height) override;
	void shear(float sh, float sv) override;
	void rotate(float angle) override;
	void scale(float dx, float dy) override;
	void translate(float dx, float dy) override;

	// Native API

	int32_t getWidth() const;
	int32_t getHeight() const;
	int32_t getStride() const;
	UIListView<const uint8_t> getPixels() const;
	virtual void resize(uint32_t width, uint32_t height);

private:
	UIPainterPrivateRaw m_Private;
};
