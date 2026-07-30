#pragma once
/*=================================================
* Copyright © 2020-2026 ChivenZhang.
* All Rights Reserved.
* =====================Note=========================
*
*
* ====================History=======================
* Created by chivenzhang@gmail.com.
*
* =================================================*/
#ifdef OPENUI_ENABLE_SDLGPU
#include <OpenUI/UIPainter.h>

class SDLGPUPainter : public UIPainter
{
public:
    SDLGPUPainter(int width, int height);
    UIRect boundingRect(float x, float y, float width, float height, const UIString& text, float cursor, UIRectRaw cursorRect) override;
    UIRect boundingRect(float x, float y, float width, float height, const UIString& text, float posX, float posY, int* cursor, UIRectRaw cursorRect) override;
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
    void drawRoundedRect(float x, float y, float w, float h, float xRadius, float yRadius) override;
    void drawText(float x, float y, float width, float height, const UIString& text, UIRectRaw boundingRect, float cursor, UIRectRaw cursorRect) override;
    const UIPen& getPen() const override;
    void setPen(const UIPen& pen) override;
    const UIBrush& getBrush() const override;
    void setBrush(const UIBrush& brush) override;
    const UIFont& getFont() const override;
    void setFont(const UIFont& font) override;
    void setClipping(bool enable) override;
    void setClipRect(float x, float y, float width, float height) override;
    void setViewport(float x, float y, float width, float height) override;
    void shear(float sh, float sv) override;
    void rotate(float angle) override;
    void scale(float dx, float dy) override;
    void translate(float dx, float dy) override;
    void resize(int width, int height);
    UIListView<const UIGeometry> getGeometry() const override;
};

#endif