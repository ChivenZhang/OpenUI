#include "RmGUIWidget.h"
#include "RmGUIWidget.h"
#include "RmGUIWidget.h"
#include "RmGUIWidget.h"
#include "RmGUIWidget.h"
#include "Widget/IRmGUIContext.h"

class RmGUIWidgetPrivate {};
class RmGUIWidgetPrivateData : public RmGUIWidgetPrivate
{
public:
	rmcontext Context = nullptr;
	rmwidget Parent = nullptr;
	RmVector<RmRef<IRmGUIWidget>> ChildrenList;
	RmRect ClientRect, ChildrenRect;
};
#define PRIVATE() ((RmGUIWidgetPrivateData*)m_PrivateData)

RmGUIWidget::RmGUIWidget(rmwidget parent)
	:
	m_PrivateData(nullptr)
{
	m_PrivateData = new RmGUIWidgetPrivateData;
	PRIVATE()->Parent = nullptr;
}

RmGUIWidget::~RmGUIWidget()
{
	if (m_PrivateData) delete m_PrivateData; m_PrivateData = nullptr;
}

RmRect RmGUIWidget::getRect() const
{
	return PRIVATE()->ClientRect;
}

void RmGUIWidget::setRect(RmRect client)
{
	PRIVATE()->ClientRect = client;
}

RmRect RmGUIWidget::getChildrenRect() const
{
	return PRIVATE()->ChildrenRect;
}

RmRaw<IRmGUIWidget> RmGUIWidget::getParent() const
{
	return PRIVATE()->Parent;
}

void RmGUIWidget::setParent(rmwidget parent)
{
	if (parent == this) return;

}

RmArrayView<RmRef<IRmGUIWidget>> RmGUIWidget::getChildren()
{
	return PRIVATE()->ChildrenList;
}

RmArrayView<const RmRef<IRmGUIWidget>> RmGUIWidget::getChildren() const
{
	return PRIVATE()->ChildrenList;
}

bool RmGUIWidget::addWidget(RmRef<IRmGUIWidget> value)
{
	auto widget = RmCast<RmGUIWidget>(value);
	if (widget == nullptr) return false;
	auto result = std::find(PRIVATE()->ChildrenList.begin(), PRIVATE()->ChildrenList.end(), widget);
	if (result == PRIVATE()->ChildrenList.end()) PRIVATE()->ChildrenList.push_back(widget);
	widget->setContext(PRIVATE()->Context);
	return true;
}

bool RmGUIWidget::removeWidget(RmRef<IRmGUIWidget> value)
{
	auto widget = RmCast<RmGUIWidget>(value);
	auto result = std::remove(PRIVATE()->ChildrenList.begin(), PRIVATE()->ChildrenList.end(), widget);
	if (result == PRIVATE()->ChildrenList.end()) return false;
	PRIVATE()->ChildrenList.erase(result, PRIVATE()->ChildrenList.end());
	widget->setContext(nullptr);
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

void RmGUIWidget::layout(rmrect client)
{
	auto childrenList = getChildren();
	for (size_t i = 0; i < childrenList.size(); ++i) childrenList[i]->layout(client);
}

void RmGUIWidget::paint(rmpainter painter, rmrect client)
{
	auto childrenList = getChildren();
	for (size_t i = 0; i < childrenList.size(); ++i) childrenList[i]->paint(painter, client);
}

RmString RmGUIWidget::getAttribute(uint32_t name) const
{
	return RmString();
}

void RmGUIWidget::setAttribute(uint32_t name, RmString const& value)
{
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

RmRaw<IRmGUIContext> RmGUIWidget::getContext() const
{
	return PRIVATE()->Context;
}

void RmGUIWidget::setContext(RmRaw<IRmGUIContext> context)
{
	PRIVATE()->Context = context;
}