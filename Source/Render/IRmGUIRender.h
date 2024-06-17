#pragma once
#include "RmGUI.h"

class IRmGUIRender;
using rmrender = RmRaw<IRmGUIRender>;

/// @brief Base interface of render backends.
class RMGUI_API IRmGUIRender
{
public:
	using data_t = struct { };

public:
	virtual ~IRmGUIRender() = default;

	virtual void render(RmArrayView<data_t> data) = 0;
};