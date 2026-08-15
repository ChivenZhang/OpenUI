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
#include "../UIAttrib.h"

#undef PRIVATE
struct UIAttribPrivate : UIPrivate
{
    UIStringHashMap<UIAttribDataRef> Attribs;
};
#define PRIVATE() ((UIAttribPrivate*)m_Private)

UIAttrib::UIAttrib()
    :
    m_IsDirty(true)
{
    m_Private = new UIAttribPrivate;
}

UIAttrib::~UIAttrib()
{
    delete m_Private; m_Private = nullptr;
}

bool UIAttrib::getDirty() const
{
    return m_IsDirty;
}

void UIAttrib::setDirty(bool value)
{
    m_IsDirty = value;
}

UIAttribDataRef UIAttrib::getAttrib(UIString const& key) const
{
    auto result = PRIVATE()->Attribs.find(key);
    if (result == PRIVATE()->Attribs.end()) return {};
    return result->second;
}

void UIAttrib::setAttrib(UIString const& key, UIAttribDataRef value)
{
    PRIVATE()->Attribs[key] = value;
    m_IsDirty = true;
}

UIString UIAttrib::getAttribText(UIString const& key) const
{
    auto result = PRIVATE()->Attribs.find(key);
    if (result == PRIVATE()->Attribs.end()) return {};
    return result->second->getText();
}

bool UIAttrib::setAttribText(UIString const& key, UIString const& value)
{
    auto result = PRIVATE()->Attribs.find(key);
    if (result == PRIVATE()->Attribs.end()) return false;
    result->second->setText(value);
    return true;
}