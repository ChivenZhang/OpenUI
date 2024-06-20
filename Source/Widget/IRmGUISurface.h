#pragma once
#include "IRmGUIPainter.h"
#include "IRmGUIContext.h"

/// @brief Base interface of native window.
class RMGUI_API IRmGUISurface
{
public:
	virtual ~IRmGUISurface() = default;

	virtual RmRaw<IRmGUIPainter> getPainter() const = 0;

	virtual RmRaw<IRmGUIContext> getContext() const = 0;

	virtual void setContext(RmRef<IRmGUIContext> value) = 0;
};