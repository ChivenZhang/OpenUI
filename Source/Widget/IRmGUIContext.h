#pragma once
#include "IRmGUIEvent.h"
#include "IRmGUIWidget.h"
#include "IRmGUISurface.h"

/// @brief Base interface of widget context.
class RMGUI_API IRmGUIContext
{
public:
	static RmRef<IRmGUIContext> GetInstance();

public:
	virtual ~IRmGUIContext() = default;

	virtual RmRaw<IRmGUISurface> getSurface() const = 0;

	virtual void setSurface(RmRef<IRmGUISurface> value) = 0;

	virtual bool addWidget(RmRef<IRmGUIWidget> widget) = 0;

	virtual bool removeWidget(RmRef<IRmGUIWidget> widget) = 0;

	virtual void layoutWidget(RmRect client) = 0;

	virtual bool paintWidget(RmRect client) = 0;

	virtual void sendEvent(IRmGUIReactorRaw source, IRmGUIEventRaw event) = 0;

	virtual void postEvent(IRmGUIReactorRaw source, IRmGUIEventRaw event) = 0;
};
using rmcontext = RmRaw<IRmGUIContext>;