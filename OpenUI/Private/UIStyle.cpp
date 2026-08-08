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
    :
    m_IsDirty(false)
{
    m_Private = new UIStylePrivateData;
}

UIStyle::~UIStyle()
{
	delete m_Private; m_Private = nullptr;
}

bool UIStyle::getDirty() const
{
    return m_IsDirty;
}

void UIStyle::setDirty(bool value)
{
    m_IsDirty = value;
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
    m_IsDirty = true;
}

UIString UIStyle::getStyleText(UIString const& key) const
{
    auto result = PRIVATE()->Styles.find(key);
    if (result == PRIVATE()->Styles.end()) return {};
    return result->second->getText();
}

void UIStyle::setStyleText(UIString const& key, UIString const& value)
{
    auto result = PRIVATE()->Styles.find(key);
    if (result == PRIVATE()->Styles.end()) return;
    result->second->setText(value);
}
