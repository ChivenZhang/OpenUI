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
#include "CairoMTRender.h"

/// @brief 
class SDL3MTDevice : public UIDevice
{
    // TODO: Implement SDL3 Metal device
public:
    SDL3MTDevice(){}
    ~SDL3MTDevice() override {}
    UIContextRaw getContext() const override { return nullptr;}
    bool update() override { return false;}
};