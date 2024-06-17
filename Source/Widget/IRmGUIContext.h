#pragma once
#include "RmGUI.h"

/// @brief Base interface of widget context.
class RMGUI_API IRmGUIContext
{
public:
	virtual ~IRmGUIContext() = default;

};
using rmcontext = RmRaw<IRmGUIContext>;