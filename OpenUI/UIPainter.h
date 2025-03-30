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
#include "UI.h"
class UIPainterPrivate {};
using UIPainterPrivateRaw = UIRaw<UIPainterPrivate>;

/// @brief 
class OPENUI_API UIPainter
{
public:
	virtual ~UIPainter() = default;

	virtual UIRect boundingRect(float x, float y, float width, float height, UIString const& text, float cursor, UIRectRaw cursorRect = nullptr) = 0;

	virtual UIRect boundingRect(float x, float y, float width, float height, UIString const& text, float posX, float posY, int* cursor, UIRectRaw cursorRect = nullptr) = 0;

	virtual void drawArc(float x, float y, float width, float height, float startAngle, float spanAngle) = 0;

	virtual void drawChord(float x, float y, float width, float height, float startAngle, float spanAngle) = 0;

	virtual void drawEllipse(float x, float y, float width, float height) = 0;

	virtual void drawImage(float x, float y, UIImage image, float sx = 0, float sy = 0, float sw = -1, float sh = -1) = 0;

	virtual void drawLine(float x1, float y1, float x2, float y2) = 0;

	virtual void drawLines(UIListView<UILine> lines) = 0;

	virtual void drawPie(float x, float y, float width, float height, float startAngle, float spanAngle) = 0;

	virtual void drawPoint(float x, float y) = 0;

	virtual void drawPoints(UIListView<UIPoint> points) = 0;

	virtual void drawPolygon(UIListView<UIPoint> points) = 0;

	virtual void drawPolyline(UIListView<UIPoint> points) = 0;

	virtual void drawRect(float x, float y, float width, float height) = 0;

	virtual void drawRects(UIListView<UIRect> rects) = 0;

	virtual void drawRoundedRect(float x, float y, float w, float h, float xRadius, float yRadius) = 0;

	virtual void drawText(float x, float y, float width, float height, const UIString& text, UIRectRaw boundingRect = nullptr, float cursor = 0, UIRectRaw cursorRect = nullptr) = 0;

	virtual UIPen const& getPen() const = 0;

	virtual void setPen(const UIPen& pen) = 0;

	virtual UIBrush const& getBrush() const = 0;

	virtual void setBrush(const UIBrush& brush) = 0;

	virtual UIFont const& getFont() const = 0;

	virtual void setFont(const UIFont& font) = 0;

	virtual void setClipping(bool enable) = 0;

	virtual void setClipRect(float x, float y, float width, float height) = 0;

	virtual void setViewport(float x, float y, float width, float height) = 0;

	virtual void shear(float sh, float sv) = 0;

	virtual void rotate(float angle) = 0;

	virtual void scale(float dx, float dy) = 0;

	virtual void translate(float dx, float dy) = 0;
};
using UIPainterRef = UIRef<UIPainter>;
using UIPainterRaw = UIRaw<UIPainter>;