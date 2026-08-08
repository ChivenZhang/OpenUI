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

template<class T, class U>
bool UITypeC(T const& src, U& dst) { return false; }

template<class T>
bool UITypeC(T const& src, T& dst) { dst = src; return true;}

template<class T, class U = UIString>
bool UITypeC(T const& src, UIString& dst) { dst = std::to_string(src); return true; }

struct UIStylePrivate {};

class OPENUI_API UIStyleData
{
public:
    virtual ~UIStyleData() = default;
    virtual std::type_info const& getType() const = 0;
    virtual UIString getText() const = 0;
    virtual void setText(UIString const& text) = 0;
    virtual void* getData(std::type_info const& type) = 0;
    virtual const void* getData(std::type_info const& type) const = 0;
};
using UIStyleDataRef = UIRef<UIStyleData>;
using UIStyleDataRaw = UIRaw<UIStyleData>;

template<class T>
class UIStyleValue : public UIStyleData
{
public:
    explicit UIStyleValue(T const& value) : m_Data(value) {}
    std::type_info const& getType() const override { return typeid(T); }
    UIString getText() const override { UIString result; UITypeC(m_Data, result); return result; }
    void setText(const UIString& text) override { UITypeC(text, m_Data); }
    void* getData(const std::type_info& type) override { return (type == typeid(T)) ? &m_Data : nullptr; }
    const void* getData(const std::type_info& type) const override { return (type == typeid(T)) ? &m_Data : nullptr; }

protected:
    T m_Data;
};

template<class T>
class UIStyleRefer : public UIStyleData
{
public:
    explicit UIStyleRefer(T& value) : m_Data(value) {}
    std::type_info const& getType() const override { return typeid(T); }
    UIString getText() const override { UIString result; UITypeC(m_Data, result); return result; }
    void setText(const UIString& text) override { UITypeC(text, m_Data); }
    void* getData(const std::type_info& type) override { return (type == typeid(T)) ? &m_Data : nullptr; }
    const void* getData(const std::type_info& type) const override { return (type == typeid(T)) ? &m_Data : nullptr; }

protected:
    T& m_Data;
};

/// @brief
class OPENUI_API UIStyle
{
public:
    UIStyle();
    ~UIStyle();

    bool getDirty() const;
    void setDirty(bool value);

    UIStyleDataRef getStyle(UIString const& key) const;
    void setStyle(UIString const& key, UIStyleDataRef value);
    void setStyleByText(UIString const& key, UIString const& value);

    template<class T>
    T const& getStyle(UIString const& key, T const& value = T()) const
    {
        auto result = getStyle(key);
        if (result && result->getData(typeid(std::remove_cvref_t<T>))) return *(T*)result->getData(typeid(std::remove_cvref_t<T>));
        return value;
    }

    template<class T>
    void setStyle(UIString const& key, T const& value)
    {
        auto result = getStyle(key);
        if (result) *(T*)result->getData(typeid(std::remove_cvref_t<T>)) = value;
        else this->setStyle(key, UICast<UIStyleData>(UINew<UIStyleValue<std::remove_cvref_t<T>>>(value)));
    }

    template<class T>
    void setEmbedStyle(UIString const& key, T& value)
    {
        this->setStyle(key, UICast<UIStyleData>(UINew<UIStyleRefer<std::remove_cvref_t<T>>>(value)));
    }

private:
    bool m_IsDirty;
    UIRaw<UIStylePrivate> m_Private;
};
using UIStyleRef = UIRef<UIStyle>;
using UIStyleRaw = UIRaw<UIStyle>;