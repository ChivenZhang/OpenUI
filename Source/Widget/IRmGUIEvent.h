#pragma once
#include "RmGUI.h"

/// @brief Base interface of all events.
class RMGUI_API IRmGUIEvent
{
public:
	virtual ~IRmGUIEvent() = default;
};
using rmevent = RmRaw<IRmGUIEvent>;

/// @brief Base interface of all event reactors.
class RMGUI_API IRmGUIEventSource
{
public:
	virtual ~IRmGUIEventSource() = default;
};
using rmevent_source = RmRaw<IRmGUIEventSource>;

/// @brief Base interface of all event handlers.
class RMGUI_API IRmGUIEventHandler
{
public:
	virtual ~IRmGUIEventHandler() = default;

	virtual void handle(rmevent_source source, rmevent event) = 0;
};
using rmevent_handler = RmRaw<IRmGUIEventHandler>;