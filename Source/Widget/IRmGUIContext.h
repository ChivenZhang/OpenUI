#pragma once
#include "IRmGUIEvent.h"
#include "IRmGUIWidget.h"

/// @brief Base interface of widget context.
class RMGUI_API IRmGUIContext
{
public:
	static RmRef<IRmGUIContext> GetInstance();

public:
	virtual ~IRmGUIContext() = default;

	virtual bool addWidget(RmRef<IRmGUIWidget> widget) = 0;

	virtual bool removeWidget(RmRef<IRmGUIWidget> widget) = 0;

	virtual void sendEvent(rmreactor source, rmevent event) = 0;

	virtual void postEvent(rmreactor source, rmevent event) = 0;

	virtual bool renderSurface(RmRect client) = 0;
};
using rmcontext = RmRaw<IRmGUIContext>;