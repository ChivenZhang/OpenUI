#pragma once
#include "IRmGUIPainter.h"
#include "IRmGUIEvent.h"

class IRmGUIWidget;
using rmwidget = RmRaw<IRmGUIWidget>;

/// @brief Base interface of all widgets.
class RMGUI_API IRmGUIWidget : public IRmGUIEventHandler
{
public:
	virtual rmwidget getParent() const = 0;

	virtual RmArrayView<RmRef<IRmGUIWidget>> getChildren() = 0;

	virtual RmArrayView<const RmRef<IRmGUIWidget>> getChildren() const = 0;

	virtual void paint(rmpainter painter, rmrect client) = 0;
};