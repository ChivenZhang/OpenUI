#pragma once
#include "RmGUI.h"

class IRmGUIRender;
using rmrender = RmRaw<IRmGUIRender>;

/// @brief Base interface of render backends.
class RMGUI_API IRmGUIRender
{
public:
	virtual ~IRmGUIRender() = default;

};