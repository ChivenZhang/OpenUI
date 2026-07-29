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

/// @brief 
class CairoWGLDevice : public UIDevice
{
    // TODO: Implement WebGL device
public:
    CairoWGLDevice(){}
    ~CairoWGLDevice() override {}
    UICanvasRaw getCanvas() const override { return nullptr;}
    bool update() override { return false;}
};