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
#include "UIPublic.h"

class OPENUI_API UIAttribData
{
public:
    virtual ~UIAttribData() = default;
    virtual std::type_info const& getType() const = 0;
    virtual UIString getText() const = 0;
    virtual void setText(UIString const& text) = 0;
    virtual void* getData(std::type_info const& type) = 0;
    virtual const void* getData(std::type_info const& type) const = 0;
};
using UIAttribDataRef = UIRef<UIAttribData>;
using UIAttribDataRaw = UIRaw<UIAttribData>;

template<class T>
class UIAttribValue : public UIAttribData
{
public:
    explicit UIAttribValue(T const& value) : m_Data(value) {}
    std::type_info const& getType() const override { return typeid(T); }
    UIString getText() const override { UIString result; UITypeC(m_Data, result); return result; }
    void setText(const UIString& text) override { UITypeC(text, m_Data); }
    void* getData(const std::type_info& type) override { return (type == typeid(T)) ? &m_Data : nullptr; }
    const void* getData(const std::type_info& type) const override { return (type == typeid(T)) ? &m_Data : nullptr; }

protected:
    T m_Data;
};

template<class T>
class UIAttribRefer : public UIAttribData
{
public:
    explicit UIAttribRefer(T& value) : m_Data(value) {}
    std::type_info const& getType() const override { return typeid(T); }
    UIString getText() const override { UIString result; UITypeC(m_Data, result); return result; }
    void setText(const UIString& text) override { UITypeC(text, m_Data); }
    void* getData(const std::type_info& type) override { return (type == typeid(T)) ? &m_Data : nullptr; }
    const void* getData(const std::type_info& type) const override { return (type == typeid(T)) ? &m_Data : nullptr; }

protected:
    T& m_Data;
};

/// @brief
class OPENUI_API UIAttrib
{
public:
    UIAttrib();
    ~UIAttrib();
    bool getDirty() const;
    void setDirty(bool value);
    UIAttribDataRef getAttrib(UIString const& key) const;
    void setAttrib(UIString const& key, UIAttribDataRef value);
    UIString getAttribText(UIString const& key) const;
    bool setAttribText(UIString const& key, UIString const& value);

    template<class T>
    T const& getAttrib(UIString const& key, T const& value = T()) const
    {
        auto result = getAttrib(key);
        if (result && result->getData(typeid(std::remove_cvref_t<T>))) return *(T*)result->getData(typeid(std::remove_cvref_t<T>));
        return value;
    }

    template<class T>
    void setAttrib(UIString const& key, T const& value)
    {
        auto result = getAttrib(key);
        if (result) *(T*)result->getData(typeid(std::remove_cvref_t<T>)) = value;
        else this->setAttrib(key, UICast<UIAttribData>(UINew<UIAttribValue<std::remove_cvref_t<T>>>(value)));
    }

    template<class T>
    void setEmbedAttrib(UIString const& key, T& value)
    {
        this->setAttrib(key, UICast<UIAttribData>(UINew<UIAttribRefer<std::remove_cvref_t<T>>>(value)));
    }

private:
    bool m_IsDirty;
    UIPrivateRaw m_Private;
};
using UIAttribRef = UIRef<UIAttrib>;
using UIAttribRaw = UIRaw<UIAttrib>;