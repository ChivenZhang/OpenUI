#pragma once
/*=================================================
* Copyright @ 2020-2025 ChivenZhang.
* All Rights Reserved.
* =====================Note=========================
*
*
* ====================History=======================
* Created by ChivenZhang at 2025/03/30 15:07:17.
*
* =================================================*/
#include <SDL3/SDL.h>
#include <OpenUI/UIDevice.h>
#include "CairoDXRender.h"

/// @brief 
class SDL3DXDevice : public UIDevice
{
    // TODO: Implement SDL3 DirectX 12 device
public:
    SDL3DXDevice(){}
    ~SDL3DXDevice() override {}
    UIContextRaw getContext() const override { return nullptr;}
    bool update() override { return false;}
};