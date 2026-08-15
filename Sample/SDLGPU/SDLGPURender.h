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
    ~SDLGPURender() override;
    UICanvasRaw getCanvas() const override;
    UIString getName() const override;
    UIImage newImage(uint32_t width, uint32_t height) override;
    void delImage(UIImage value) override;

protected:
    UICanvasRaw m_Canvas;
    UIList<UIImage> m_Targets;
};

#endif