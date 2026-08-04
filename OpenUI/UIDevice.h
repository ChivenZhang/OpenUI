/*=================================================
* Copyright © 2020-2025 ChivenZhang.
* All Rights Reserved.
* =====================Note=========================
*
*
* ====================History=======================
* Created by ChivenZhang@gmail.com.
*
* =================================================*/
#include "UICanvas.h"

class UIDevicePrivate {};
using UIDevicePrivateRaw = UIRaw<UIDevicePrivate>;

/// @brief Interface for UI device
class OPENUI_API UIDevice
{
public:
    virtual ~UIDevice() = default;
    virtual UICanvasRaw getCanvas() const = 0;

    virtual bool update() = 0;
    virtual void setCursor(UIString type) = 0;

    virtual UIString getClipText() const = 0;
    virtual void setClipText(UIString text) = 0;

    virtual void setKeyboard(bool value) = 0;
    virtual bool translateText(UIString text, UIString& result) const = 0;

    virtual void logMessage(uint8_t type, UIString text) const = 0;
};