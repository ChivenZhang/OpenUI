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
    virtual UICanvasRaw getContext() const = 0;
    virtual bool update() = 0;
};