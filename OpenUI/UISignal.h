#pragma once
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
#include "UI.h"
class UIElement;
using UIElementRaw = UIRaw<UIElement>;

/// @brief Base interface of signals.
class UISignal
{
public:
	virtual ~UISignal() = default;
};

/// @brief Base interface of signals with arguments.
/// @tparam ...T 
template <class... T>
class UISignalAs : public UISignal
{
public:
	virtual uint32_t connect(UIElementRaw owner, UILambda<void(T...)> slot);
	virtual void disconnect(UIElementRaw owner, uint32_t handle);
	virtual void disconnect(UIElementRaw owner);
	virtual void disconnect();
	virtual void signal(T... args);

private:
	uint32_t m_SlotID = 0;
	struct UISignalSlot
	{
		bool Dirty = false;
		uint32_t Handle = 0;
		UIElementRaw Owner = nullptr;
		UILambda<void(T...)> Lambda;
	};
	using UISignalSlotRef = UIRef<UISignalSlot>;
	UIList<UISignalSlotRef> m_SlotList;
};

template<class ...T>
inline uint32_t UISignalAs<T...>::connect(UIElementRaw owner, UILambda<void(T...)> slot)
{
	auto handle = ++this->m_SlotID;
	auto element = UINew<UISignalSlot>();
	element->Handle = handle;
	element->Owner = owner;
	element->Lambda = slot;
	this->m_SlotList.emplace_back(element);
	return handle;
}

template<class ...T>
inline void UISignalAs<T...>::disconnect(UIElementRaw owner, uint32_t handle)
{
	auto& connectList = this->m_SlotList;
	for (size_t i = 0; i < connectList.size(); ++i)
	{
		if (connectList[i]->Owner == owner && connectList[i]->Handle == handle)
		{
			connectList[i]->Dirty = true;
			return;
		}
	}
}

template<class ...T>
inline void UISignalAs<T...>::disconnect(UIElementRaw owner)
{
	auto& connectList = this->m_SlotList;
	for (size_t i = 0; i < connectList.size(); ++i)
	{
		if (connectList[i]->Owner == owner)
		{
			connectList[i]->Dirty = true;
		}
	}
}

template<class ...T>
inline void UISignalAs<T...>::disconnect()
{
	auto& connectList = this->m_SlotList;
	for (size_t i = 0; i < connectList.size(); ++i)
	{
		connectList[i]->Dirty = true;
	}
}

template<class ...T>
inline void UISignalAs<T...>::signal(T ...args)
{
	bool isDirty = false;
	auto& connectList = this->m_SlotList;
	for (size_t i = 0; i < connectList.size(); ++i)
	{
		if (connectList[i]->Lambda)
		{
			if (connectList[i]->Dirty == false) connectList[i]->Lambda(std::forward<T>(args)...);
			else isDirty = true;
		}
	}

	if (isDirty)
	{
		UIList<UISignalSlotRef> result;
		for (size_t i = 0; i < connectList.size(); ++i)
		{
			if (connectList[i]->Dirty) result.push_back(connectList[i]);
		}
		for (size_t i = 0; i < result.size(); ++i)
		{
			auto index = std::find(connectList.begin(), connectList.end(), result[i]);
			if (index == connectList.end()) continue;
			connectList.erase(index);
		}
	}
}

// ===================================================

/// @brief Base interface of signals without argument.
template <>
class UISignalAs<void> : public UISignal
{
public:
	virtual uint32_t connect(UIElementRaw owner, UILambda<void()> slot);
	virtual void disconnect(UIElementRaw owner, uint32_t handle);
	virtual void disconnect(UIElementRaw owner);
	virtual void disconnect();
	virtual void signal();

private:
	uint32_t m_SlotID = 0;
	struct UISignalSlot
	{
		bool Dirty = false;
		uint32_t Handle = 0;
		UIElementRaw Owner = nullptr;
		UILambda<void()> Lambda;
	};
	using UISignalSlotVoidRef = UIRef<UISignalSlot>;
	UIList<UISignalSlotVoidRef> m_SlotList;
};

inline uint32_t UISignalAs<void>::connect(UIElementRaw owner, UILambda<void()> slot)
{
	auto handle = ++this->m_SlotID;
	auto element = UINew<UISignalSlot>();
	element->Handle = handle;
	element->Owner = owner;
	element->Lambda = slot;
	this->m_SlotList.emplace_back(element);
	return handle;
}

inline void UISignalAs<void>::disconnect(UIElementRaw owner, uint32_t handle)
{
	auto& connectList = this->m_SlotList;
	for (size_t i = 0; i < connectList.size(); ++i)
	{
		if (connectList[i]->Owner == owner && connectList[i]->Handle == handle)
		{
			connectList[i]->Dirty = true;
			return;
		}
	}
}

inline void UISignalAs<void>::disconnect(UIElementRaw owner)
{
	auto& connectList = this->m_SlotList;
	for (size_t i = 0; i < connectList.size(); ++i)
	{
		if (connectList[i]->Owner == owner)
		{
			connectList[i]->Dirty = true;
		}
	}
}

inline void UISignalAs<void>::disconnect()
{
	auto& connectList = this->m_SlotList;
	for (size_t i = 0; i < connectList.size(); ++i)
	{
		connectList[i]->Dirty = true;
	}
}

inline void UISignalAs<void>::signal()
{
	bool isDirty = false;
	auto& connectList = this->m_SlotList;
	for (size_t i = 0; i < connectList.size(); ++i)
	{
		if (connectList[i]->Lambda)
		{
			if (connectList[i]->Dirty == false) connectList[i]->Lambda();
			else isDirty = true;
		}
	}

	if (isDirty)
	{
		UIList<UISignalSlotVoidRef> result;
		for (size_t i = 0; i < connectList.size(); ++i)
		{
			if (connectList[i]->Dirty) result.push_back(connectList[i]);
		}
		for (size_t i = 0; i < result.size(); ++i)
		{
			auto index = std::find(connectList.begin(), connectList.end(), result[i]);
			if (index == connectList.end()) continue;
			connectList.erase(index);
		}
	}
}

template <class... T>
using UISignalAsRaw = UIRaw<UISignalAs<T...>>;