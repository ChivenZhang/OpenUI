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
#ifdef OPENUI_ENABLE_OPENRT
#include <OpenUI/UIRender.h>

class OpenRTRender : public UIRender
{
public:
    OpenRTRender(UICanvasRaw canvas, int width, int height);
    ~OpenRTRender() override;
    UICanvasRaw getCanvas() const override;
    UIString getName() const override;
    UIImage newImage(uint32_t width, uint32_t height) override;
    void delImage(UIImage value) override;
    void render(UIRect client, UIMat4 matrix, UIImageRaw srcImg, UIImageRaw dstImg, UIComputedStyleRaw style) override;

protected:
    UICanvasRaw m_Canvas;
    UIList<UIImage> m_Targets;
};

#endif