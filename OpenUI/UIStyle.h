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

class OPENUI_API UIStyleData
{
public:
    virtual ~UIStyleData() = default;
    virtual std::type_info const& getType() const = 0;
    virtual void* getData(std::type_info const& type) = 0;
    virtual const void* getData(std::type_info const& type) const = 0;
};
using UIStyleDataRef = UIRef<UIStyleData>;
using UIStyleDataRaw = UIRaw<UIStyleData>;

template<class T>
class UIStyleValue : public UIStyleData
{
public:
    UIStyleValue(T const& value) : m_Data(value) {}
    std::type_info const& getType() const override { return typeid(T); }
    void* getData(const std::type_info& type) override { return (type == typeid(T)) ? &m_Data : nullptr; }
    const void* getData(const std::type_info& type) const override { return (type == typeid(T)) ? &m_Data : nullptr; }

protected:
    T m_Data;
};

template<class T>
class UIStyleRefer : public UIStyleData
{
public:
    UIStyleRefer(T& value) : m_Data(value) {}
    std::type_info const& getType() const override { return typeid(T); }
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
    UIStyleDataRef getStyle(UIString const& key) const;
    void setStyle(UIString const& key, UIStyleDataRef value);

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
        if (result && result->getData(typeid(std::remove_cvref_t<T>))) *(T*)result->getData(typeid(std::remove_cvref_t<T>)) = value;
        else this->setStyle(key, UICast<UIStyleData>(UINew<UIStyleValue<std::remove_cvref_t<T>>>(value)));
    }

    template<class T>
    void setEmbedStyle(UIString const& key, T& value)
    {
        this->setStyle(key, UICast<UIStyleData>(UINew<UIStyleRefer<std::remove_cvref_t<T>>>(value)));
    }

private:
    UIRaw<UIStylePrivate> m_Private;
};
using UIStyleRef = UIRef<UIStyle>;
using UIStyleRaw = UIRaw<UIStyle>;