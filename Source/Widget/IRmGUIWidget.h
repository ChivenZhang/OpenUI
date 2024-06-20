#pragma once
#include "IRmGUIPainter.h"
#include "IRmGUIEvent.h"
class IRmGUIContext;

class IRmGUIWidget;
using rmwidget = RmRaw<IRmGUIWidget>;

/// @brief Base interface of all widgets.
class RMGUI_API IRmGUIWidget : public IRmGUIEventHandler
{
public:
	virtual RmRect getRect() const = 0;

	virtual void setRect(RmRect client) = 0;

	virtual RmRect getChildrenRect() const = 0;

	virtual rmwidget getParent() const = 0;

	virtual void setParent(rmwidget parent) = 0;

	virtual RmArrayView<RmRef<IRmGUIWidget>> getChildren() = 0;

	virtual RmArrayView<const RmRef<IRmGUIWidget>> getChildren() const = 0;

	virtual bool addWidget(RmRef<IRmGUIWidget> widget) = 0;

	virtual bool removeWidget(RmRef<IRmGUIWidget> widget) = 0;

	virtual void layout(rmrect client) = 0;

	virtual void paint(rmpainter painter, rmrect client) = 0;

	virtual RmString getAttribute(uint32_t name) const = 0;

	virtual void setAttribute(uint32_t name, RmString const& value) = 0;

protected:
	virtual void closeEvent(rmevent_close event) = 0;

	virtual void dragEnterEvent(rmevent_drag_enter event) = 0;

	virtual void dragLeaveEvent(rmevent_drag_leave event) = 0;

	virtual void dragMoveEvent(rmevent_drag_move event) = 0;

	virtual void dropEvent(rmevent_drop event) = 0;

	virtual void enterEvent(rmevent_enter event) = 0;

	virtual void focusInEvent(rmevent_focus event) = 0;

	virtual void focusOutEvent(rmevent_focus event) = 0;

	virtual void hideEvent(rmevent_hide event) = 0;

	virtual void inputEvent(rmevent_input event) = 0;

	virtual void keyPressEvent(rmevent_key_down event) = 0;

	virtual void keyReleaseEvent(rmevent_key_up event) = 0;

	virtual void leaveEvent(rmevent_leave event) = 0;

	virtual void mouseDoubleClickEvent(rmevent_mouse event) = 0;

	virtual void mouseMoveEvent(rmevent_mouse event) = 0;

	virtual void mousePressEvent(rmevent_mouse event) = 0;

	virtual void mouseReleaseEvent(rmevent_mouse event) = 0;

	virtual void moveEvent(rmevent_move event) = 0;

	virtual void resizeEvent(rmevent_resize event) = 0;

	virtual void showEvent(rmevent_show event) = 0;

	virtual void tabletEvent(rmevent_tablet event) = 0;

	virtual void wheelEvent(rmevent_wheel event) = 0;
};