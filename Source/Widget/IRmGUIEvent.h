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
class RMGUI_API IRmGUIEventReactor
{
public:
	virtual ~IRmGUIEventReactor() = default;
};
using rmreactor = RmRaw<IRmGUIEventReactor>;

/// @brief Base interface of all event handlers.
class RMGUI_API IRmGUIEventHandler
{
public:
	virtual ~IRmGUIEventHandler() = default;

	virtual void handle(rmreactor source, rmevent event) = 0;
};
using rmhandler = RmRaw<IRmGUIEventHandler>;