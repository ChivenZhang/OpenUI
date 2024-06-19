#pragma once
#include "IRmGUIEvent.h"

/// @brief Base interface of widget context.
class RMGUI_API IRmGUIContext
{
public:
	static RmRef<IRmGUIContext> GetInstance();

public:
	virtual ~IRmGUIContext() = default;

	virtual void sendEvent(rmreactor source, rmevent event) = 0;

	virtual void postEvent(rmreactor source, rmevent event) = 0;
};
using rmcontext = RmRaw<IRmGUIContext>;