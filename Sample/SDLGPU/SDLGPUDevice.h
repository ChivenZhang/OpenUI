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
#include <OpenUI/UIDevice.h>
#include <SDL3/SDL_video.h>

class SDLGPUDevice : public UIDevice
{
public:
    SDLGPUDevice();
    ~SDLGPUDevice() override;
    UICanvasRaw getCanvas() const override;
    bool update() override;
    SDL_Window* getWindow() const;

protected:
	SDL_Window* m_Window;
	UICanvasRef m_UICanvas;
};

#endif
