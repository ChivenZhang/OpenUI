#include "RMGUIWidget.h"

class RmGUIWidgetPrivate {};
class RmGUIWidgetPrivateData : public RmGUIWidgetPrivate
{
public:
	rmwidget m_ParentWidget = nullptr;
	RmVector<RmRef<IRmGUIWidget>> m_ChildWidgetList;
};
#define PRIVATE() ((RmGUIWidgetPrivateData*)m_PrivateData)

RmGUIWidget::RmGUIWidget(rmwidget parent)
	:
	m_PrivateData(nullptr)
{
	m_PrivateData = new RmGUIWidgetPrivateData;
	PRIVATE()->m_ParentWidget = nullptr;
}

RmGUIWidget::~RmGUIWidget()
{
	if (m_PrivateData) delete m_PrivateData; m_PrivateData = nullptr;
}

RmRect RmGUIWidget::getRect() const
{
	return RmRect();
}

RmRect RmGUIWidget::getChildrenRect() const
{
	return RmRect();
}

RmRaw<IRmGUIWidget> RmGUIWidget::getParent() const
{
	return PRIVATE()->m_ParentWidget;
}

void RmGUIWidget::setParent(rmwidget parent)
{
	if (parent == this) return;

}

RmArrayView<RmRef<IRmGUIWidget>> RmGUIWidget::getChildren()
{
	return PRIVATE()->m_ChildWidgetList;
}

RmArrayView<const RmRef<IRmGUIWidget>> RmGUIWidget::getChildren() const
{
	return PRIVATE()->m_ChildWidgetList;
}

bool RmGUIWidget::addWidget(RmRef<IRmGUIWidget> widget)
{
	if (widget == nullptr) return false;
	auto result = std::find(PRIVATE()->m_ChildWidgetList.begin(), PRIVATE()->m_ChildWidgetList.end(), widget);
	if (result == PRIVATE()->m_ChildWidgetList.end()) PRIVATE()->m_ChildWidgetList.push_back(widget);
	return true;
}

bool RmGUIWidget::removeWidget(RmRef<IRmGUIWidget> widget)
{
	auto result = std::remove(PRIVATE()->m_ChildWidgetList.begin(), PRIVATE()->m_ChildWidgetList.end(), widget);
	if (result == PRIVATE()->m_ChildWidgetList.end()) return false;
	PRIVATE()->m_ChildWidgetList.erase(result, PRIVATE()->m_ChildWidgetList.end());
	return true;
}

bool RmGUIWidget::filter(rmreactor source, rmevent event)
{
	return false;
}

void RmGUIWidget::handle(rmreactor source, rmevent event)
{
	switch (event->Type)
	{
	case HashUtility::Encrypt("Show"):
	{
		showEvent(RmCast<IRmGUIShowEvent>(event));
	} break;
	case HashUtility::Encrypt("Hide"):
	{
		hideEvent(RmCast<IRmGUIHideEvent>(event));
	} break;
	case HashUtility::Encrypt("Close"):
	{
		closeEvent(RmCast<IRmGUICloseEvent>(event));
	} break;
	case HashUtility::Encrypt("Move"):
	{
		moveEvent(RmCast<IRmGUIMoveEvent>(event));
	} break;
	case HashUtility::Encrypt("Resize"):
	{
		resizeEvent(RmCast<IRmGUIResizeEvent>(event));
	} break;
	case HashUtility::Encrypt("MouseDown"):
	{
		mousePressEvent(RmCast<IRmGUIMouseDownEvent>(event));
	} break;
	case HashUtility::Encrypt("MouseUp"):
	{
		mouseReleaseEvent(RmCast<IRmGUIMouseUpEvent>(event));
	} break;
	case HashUtility::Encrypt("DoubleClick"):
	{
		mouseDoubleClickEvent(RmCast<IRmGUIMouseDblClickEvent>(event));
	} break;
	case HashUtility::Encrypt("MouseMove"):
	{
		mouseMoveEvent(RmCast<IRmGUIMouseMoveEvent>(event));
	} break;
	case HashUtility::Encrypt("MouseWheel"):
	{
		wheelEvent(RmCast<IRmGUIMouseWheelEvent>(event));
	} break;
	case HashUtility::Encrypt("MouseEnter"):
	{
		enterEvent(RmCast<IRmGUIMouseEnterEvent>(event));
	} break;
	case HashUtility::Encrypt("MouseLeave"):
	{
		leaveEvent(RmCast<IRmGUIMouseLeaveEvent>(event));
	} break;
	case HashUtility::Encrypt("KeyDown"):
	{
		keyPressEvent(RmCast<IRmGUIKeyDownEvent>(event));
	} break;
	case HashUtility::Encrypt("KeyUp"):
	{
		keyReleaseEvent(RmCast<IRmGUIKeyUpEvent>(event));
	} break;
	case HashUtility::Encrypt("KeyInput"):
	{
		inputEvent(RmCast<IRmGUIKeyInputEvent>(event));
	} break;
	case HashUtility::Encrypt("Drop"):
	{
		dropEvent(RmCast<IRmGUIDropEvent>(event));
	} break;
	case HashUtility::Encrypt("DragMove"):
	{
		dragMoveEvent(RmCast<IRmGUIDragMoveEvent>(event));
	} break;
	case HashUtility::Encrypt("DragEnter"):
	{
		dragEnterEvent(RmCast<IRmGUIDragEnterEvent>(event));
	} break;
	case HashUtility::Encrypt("DragLeave"):
	{
		dragLeaveEvent(RmCast<IRmGUIDragLeaveEvent>(event));
	} break;
	case HashUtility::Encrypt("FocusIn"):
	{
		focusInEvent(RmCast<IRmGUIFocusEvent>(event));
	} break;
	case HashUtility::Encrypt("FocusOut"):
	{
		focusOutEvent(RmCast<IRmGUIFocusEvent>(event));
	} break;
	}
}

void RmGUIWidget::closeEvent(rmevent_close event)
{
}

void RmGUIWidget::dragEnterEvent(rmevent_drag_enter event)
{
}

void RmGUIWidget::dragLeaveEvent(rmevent_drag_leave event)
{
}

void RmGUIWidget::dragMoveEvent(rmevent_drag_move event)
{
}

void RmGUIWidget::dropEvent(rmevent_drop event)
{
}

void RmGUIWidget::enterEvent(rmevent_enter event)
{
}

void RmGUIWidget::focusInEvent(rmevent_focus event)
{
}

void RmGUIWidget::focusOutEvent(rmevent_focus event)
{
}

void RmGUIWidget::hideEvent(rmevent_hide event)
{
}

void RmGUIWidget::inputEvent(rmevent_input event)
{
}

void RmGUIWidget::keyPressEvent(rmevent_key_down event)
{
}

void RmGUIWidget::keyReleaseEvent(rmevent_key_up event)
{
}

void RmGUIWidget::leaveEvent(rmevent_leave event)
{
}

void RmGUIWidget::mouseDoubleClickEvent(rmevent_mouse event)
{
}

void RmGUIWidget::mouseMoveEvent(rmevent_mouse event)
{
}

void RmGUIWidget::mousePressEvent(rmevent_mouse event)
{
}

void RmGUIWidget::mouseReleaseEvent(rmevent_mouse event)
{
}

void RmGUIWidget::moveEvent(rmevent_move event)
{
}

void RmGUIWidget::resizeEvent(rmevent_resize event)
{
}

void RmGUIWidget::showEvent(rmevent_show event)
{
}

void RmGUIWidget::tabletEvent(rmevent_tablet event)
{
}

void RmGUIWidget::wheelEvent(rmevent_wheel event)
{
}
