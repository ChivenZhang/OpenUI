#pragma once
#include "RmGUI.h"

/// @brief Base interface of render backends.
class RMGUI_API IRmGUIRender
{
public:
	virtual ~IRmGUIRender() = default;

	virtual void render(RmRect client, RmArrayView<RmPrimitive> widgets) = 0;
};
using IRmGUIRenderRef = RmRef<IRmGUIRender>;
using IRmGUIRenderRaw = RmRaw<IRmGUIRender>;