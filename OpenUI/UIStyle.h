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

struct UIStylePrivate {};

/// @brief
class OPENUI_API UIStyle
{
public:
    UIStyle();
    ~UIStyle();
    UIAny const& getStyle(UIString const& key) const;
    void setStyle(UIString const& key, UIAny const& value);

    template<class T>
    T const& getStyle(UIString const& key) const
    {
        static T s_EmptyStyle;
        auto& result = getStyle(key);
        if (result.type() == typeid(T)) return std::any_cast<T>(result);
        return s_EmptyStyle;
    }

    template<class T>
    void setStyle(UIString const& key, T const& value)
    {
        setStyle(key, UIAny(value));
    }

private:
    UIRaw<UIStylePrivate> m_Private;
};
using UIStyleRef = UIRef<UIStyle>;
using UIStyleRaw = UIRaw<UIStyle>;