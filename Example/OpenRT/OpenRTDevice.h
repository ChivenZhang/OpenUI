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
#include <OpenUI/UIDevice.h>
#include <SDL3/SDL.h>

class OpenRTDevice : public UIDevice
{
public:
    OpenRTDevice();
    ~OpenRTDevice() override;
    UICanvasRaw getCanvas() const override;
    bool update() override;
    void setCursor(UIString type) override;
    UIString getClipText() const override;
    void setClipText(UIString text) override;
    void setKeyboard(bool value) override;
    bool translateText(UIString text, UIString& result) const override;
    void logMessage(uint8_t type, UIString text) const override;
    SDL_Window* getWindow() const;

protected:
	SDL_Window* m_Window;
	SDL_GLContext m_Context;
	UICanvasRef m_Canvas;
};

#endif
