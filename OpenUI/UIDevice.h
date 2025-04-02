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
#include "UIContext.h"

class UIDevicePrivate {};
using UIDevicePrivateRaw = UIRaw<UIDevicePrivate>;

/// @brief Interface for UI device
class OPENUI_API UIDevice
{
public:
    virtual ~UIDevice() = default;
    virtual UIContextRaw getContext() const = 0;
    virtual bool update() = 0;
};