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

#undef PRIVATE
struct UIStylePrivate : UIPrivate
{
    UIStringHashMap<UIStyleDataRef> Styles;
};
#define PRIVATE() ((UIStylePrivate*)m_Private)

UIStyle::UIStyle()
    :
    m_IsDirty(true)
{
    m_Private = new UIStylePrivate;
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

bool UIStyle::setStyleText(UIString const& key, UIString const& value)
{
    auto result = PRIVATE()->Styles.find(key);
    if (result == PRIVATE()->Styles.end()) return false;
    result->second->setText(value);
    return true;
}

//================================================================================================================

#undef PRIVATE
struct UIComputedStylePrivate : UIStylePrivate
{
    UIRaw<UIStyle> Style;
    UIStringHashMap<UIStyleDataRaw> ComputedStyles;
};
#define PRIVATE() ((UIComputedStylePrivate*)m_Private)

UIComputedStyle::UIComputedStyle(UIRaw<UIStyle> style)
{
    m_Private = new UIComputedStylePrivate;
    PRIVATE()->Style = style;
}

UIComputedStyle::~UIComputedStyle()
{
    delete m_Private; m_Private = nullptr;
}

bool UIComputedStyle::compute(UIRaw<UIComputedStyle> parent)
{
    PRIVATE()->ComputedStyles.clear();
    if (parent)
    {
        for (auto& style : ((UIComputedStylePrivate*)parent->m_Private)->ComputedStyles)
        {
            if (style.second->inherited()) PRIVATE()->ComputedStyles[style.first] = style.second;
        }
    }
    for (auto& style : ((UIStylePrivate*)PRIVATE()->Style->m_Private)->Styles)
    {
        PRIVATE()->ComputedStyles[style.first] = style.second.get();
    }
    return true;
}

UIStyleDataRaw UIComputedStyle::getStyle(UIString const& key) const
{
    auto result = PRIVATE()->ComputedStyles.find(key);
    if (result == PRIVATE()->ComputedStyles.end()) return {};
    return result->second;
}
