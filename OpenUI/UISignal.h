#pragma once
#include "UI.h"
class UIElement;
using UIElementRaw = UIRaw<UIElement>;

/// @brief Base interface of signals.
class OPENUI_API UISignal
{
public:
	virtual ~UISignal() = default;

};

template <class... T>
class UISignalSlot
{
public:
	bool Dirty = false;
	uint32_t Handle = 0;
	UISignalSlotRaw Owner = nullptr;
	UILambda<void(T...)> Slot;
};
template <class... T>
using UISignalSlotRef = UIRef<UISignalSlot<T...>>;

/// @brief Base interface of signals with arguments.
/// @tparam ...T 
template <class... T>
class OPENUI_API UISignalAs : public UISignal
{
public:
	virtual uint32_t connect(UIElementRaw owner, UILambda<void(T...)> slot);

	virtual void disconnect(UIElementRaw owner, uint32_t handle);

	virtual void disconnect(UIElementRaw owner);

	virtual void signal(T... args);

private:
	uint32_t ConnectID = 0;
	UIVector<UISignalSlotRef<T...>> ConnectList;
};

template <>
class UISignalSlot<void>
{
public:
	bool Dirty = false;
	uint32_t Handle = 0;
	UIGUIWidgetRaw Owner = nullptr;
	UILambda<void()> Slot;
};

/// @brief Base interface of signals without argument.
template <>
class OPENUI_API UISignalAs<void> : public UISignal
{
public:
	virtual uint32_t connect(UIElementRaw owner, UILambda<void()> slot);

	virtual void disconnect(UIElementRaw owner, uint32_t handle);

	virtual void disconnect(UIElementRaw owner);

	virtual void signal();
};

template <class... T>
using UISignalAsRaw = UIRaw<UISignalAs<T...>>;