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
    SDLGPURender(int width, int height);
    UIString getName() const override;
    void render(UIImage canvas, UIRect client, UIListView<UIPrimitive> data) override;
};

#endif