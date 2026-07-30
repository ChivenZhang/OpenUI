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
#include "UI.h"

class OPENUI_API UIShader
{
public:
    virtual ~UIShader() = default;
    virtual void render(UIImage const& canvas, UIListView<const UIPrimitive> data) const = 0;
};