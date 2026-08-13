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
#include <OpenUI/UIRender.h>

class SDLGPURender : public UIRender
{
public:
    SDLGPURender(UICanvasRaw canvas, int width, int height);
    UICanvasRaw getCanvas() const override;
    UIString getName() const override;
    void render(UIRect client, UIImageRaw srcImage, UIImageRaw dstImage, UIComputedStyleRaw style) override;

protected:
    UICanvasRaw m_Canvas;
};

#endif