#pragma once
#include "RmGUI.h"
class IRmGUIWidget;
using IRmGUIWidgetRaw = RmRaw<IRmGUIWidget>;

/// @brief Base interface of signals.
class RMGUI_API IRmGUISignal
{
public:
	virtual ~IRmGUISignal() = default;

};

/// @brief Base interface of signals with argments.
/// @tparam ...T 
template <class... T>
class RMGUI_API IRmGUISignalAs : public IRmGUISignal
{
public:
	virtual uint32_t connect(IRmGUIWidgetRaw owner, RmLambda<void(T...)> slot) = 0;

	virtual void disconnect(IRmGUIWidgetRaw owner, uint32_t handle) = 0;

	virtual void disconnect(IRmGUIWidgetRaw owner) = 0;

	virtual void emit(T... args) = 0;
};
template <class... T>
using IRmGUISignalAsRaw = RmRaw<IRmGUISignalAs<T...>>;

template <>
class RMGUI_API IRmGUISignalAs<void> : public IRmGUISignal
{
public:
	virtual uint32_t connect(IRmGUIWidgetRaw owner, RmLambda<void()> slot) = 0;

	virtual void disconnect(IRmGUIWidgetRaw owner, uint32_t handle) = 0;

	virtual void disconnect(IRmGUIWidgetRaw owner) = 0;

	virtual void emit() = 0;
};