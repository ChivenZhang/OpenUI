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
#include "../UIStyle.h"

struct UIStylePrivateData : UIStylePrivate
{
    UIStringMap<UIStyleDataRef> Styles;
};
#define PRIVATE() ((UIStylePrivateData*)m_Private)

UIStyle::UIStyle()
{
    m_Private = new UIStylePrivateData;
}

UIStyle::~UIStyle()
{
	delete m_Private; m_Private = nullptr;
}

UIStyleDataRef UIStyle::getStyle(UIString const& key) const
{
    auto result = PRIVATE()->Styles.find(key);
    if (result == PRIVATE()->Styles.end()) return {};
    return result->second;
}

void UIStyle::setStyle(UIString const& key, UIStyleDataRef value)
{
    PRIVATE()->Styles[key] = value;
}