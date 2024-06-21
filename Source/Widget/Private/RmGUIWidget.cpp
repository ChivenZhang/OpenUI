#include "RmGUIWidget.h"
#include <taitank.h>

class RmGUIWidgetPrivate {};
class RmGUIWidgetPrivateData : public RmGUIWidgetPrivate
{
public:
	RmRaw<IRmGUIContext> Context = nullptr;
	RmRaw<IRmGUIWidget> Parent = nullptr;
	RmVector<RmRef<IRmGUIWidget>> ChildrenList;
	RmRect ClientRect, ChildrenRect;
	// Attributes
	float Width = VALUE_UNDEFINED, Height = VALUE_UNDEFINED, MinWidth = VALUE_UNDEFINED, MinHeight = VALUE_UNDEFINED, MaxWidth = VALUE_UNDEFINED, MaxHeight = VALUE_UNDEFINED;
	RmFloat4 Margin = {}, Padding = {}, Border = {};
};
#define PRIVATE() ((RmGUIWidgetPrivateData*)m_PrivateData)

RmGUIWidget::RmGUIWidget(IRmGUIWidgetRaw parent)
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

void RmGUIWidget::setParent(IRmGUIWidgetRaw parent)
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
	auto layout_func = [=](RmRaw<IRmGUIWidget> widget)->taitank::TaitankNodeRef {
		auto flex = taitank::NodeCreate();
		taitank::SetWidth(flex, getWidth());
		taitank::SetHeight(flex, getHeight());
		taitank::SetMinWidth(flex, getMinWidth());
		taitank::SetMinHeight(flex, getMinHeight());
		taitank::SetMaxWidth(flex, getMaxWidth());
		taitank::SetMaxHeight(flex, getMaxHeight());
		taitank::SetBorder(flex, taitank::CSSDirection::CSS_LEFT, getBorder().X);
		taitank::SetBorder(flex, taitank::CSSDirection::CSS_TOP, getBorder().Y);
		taitank::SetBorder(flex, taitank::CSSDirection::CSS_RIGHT, getBorder().Z);
		taitank::SetBorder(flex, taitank::CSSDirection::CSS_BOTTOM, getBorder().W);
		taitank::SetMargin(flex, taitank::CSSDirection::CSS_LEFT, getMargin().X);
		taitank::SetMargin(flex, taitank::CSSDirection::CSS_TOP, getMargin().Y);
		taitank::SetMargin(flex, taitank::CSSDirection::CSS_RIGHT, getMargin().Z);
		taitank::SetMargin(flex, taitank::CSSDirection::CSS_BOTTOM, getMargin().W);
		taitank::SetPadding(flex, taitank::CSSDirection::CSS_LEFT, getPadding().X);
		taitank::SetPadding(flex, taitank::CSSDirection::CSS_TOP, getPadding().Y);
		taitank::SetPadding(flex, taitank::CSSDirection::CSS_RIGHT, getPadding().Z);
		taitank::SetPadding(flex, taitank::CSSDirection::CSS_BOTTOM, getPadding().W);
		taitank::SetFlexDirection(flex, taitank::FlexDirection::FLEX_DIRECTION_ROW);
		taitank::SetAlignItems(flex, taitank::FlexAlign::FLEX_ALIGN_CENTER);
		taitank::SetJustifyContent(flex, taitank::FlexAlign::FLEX_ALIGN_SPACE_BETWEEN);
		return flex;
		};

	auto root = layout_func(this);
	auto childList = getChildren();
	for (size_t i = 0; i < childList.size(); ++i)
	{
		root->AddChild(layout_func(childList[i].get()));
	}
	taitank::DoLayout(root, client->W, client->H);
	for (size_t i = 0; i < childList.size(); ++i)
	{
		auto flex = root->GetChild(i);
		auto left = taitank::GetLeft(flex);
		auto right = taitank::GetRight(flex);
		auto top = taitank::GetTop(flex);
		auto bottom = taitank::GetBottom(flex);
		childList[i]->setRect({ client->X + left, client->Y + top, right - left, bottom - top });
	}
}

void RmGUIWidget::paint(rmpainter painter, rmrect client)
{
	auto bounds = getRect();
	auto childrenList = getChildren();
	for (size_t i = 0; i < childrenList.size(); ++i) childrenList[i]->paint(painter, &bounds);
}

RmString RmGUIWidget::getAttribute(uint32_t name) const
{
	return RmString();
}

void RmGUIWidget::setAttribute(uint32_t name, RmString const& value)
{
}

float RmGUIWidget::getWidth() const
{
	return PRIVATE()->Width;
}

void RmGUIWidget::setWidth(float value)
{
	PRIVATE()->Width = value;
}

float RmGUIWidget::getHeight() const
{
	return PRIVATE()->Height;
}

void RmGUIWidget::setHeight(float value)
{
	PRIVATE()->Height = value;
}

float RmGUIWidget::getMinWidth() const
{
	return PRIVATE()->MinWidth;
}

void RmGUIWidget::setMinWidth(float value)
{
	PRIVATE()->MinWidth = value;
}

float RmGUIWidget::getMinHeight() const
{
	return PRIVATE()->MinHeight;
}

void RmGUIWidget::setMinHeight(float value)
{
	PRIVATE()->MinHeight = value;
}

float RmGUIWidget::getMaxWidth() const
{
	return PRIVATE()->MaxWidth;
}

void RmGUIWidget::setMaxWidth(float value)
{
	PRIVATE()->MaxWidth = value;
}

float RmGUIWidget::getMaxHeight() const
{
	return PRIVATE()->MaxHeight;
}

void RmGUIWidget::setMaxHeight(float value)
{
	PRIVATE()->MaxHeight = value;
}

RmFloat4 RmGUIWidget::getMargin() const
{
	return PRIVATE()->Margin;
}

void RmGUIWidget::setMargin(RmFloat4 value)
{
	PRIVATE()->Margin = value;
}

RmFloat4 RmGUIWidget::getPadding() const
{
	return PRIVATE()->Padding;
}

void RmGUIWidget::setPadding(RmFloat4 value)
{
	PRIVATE()->Padding = value;
}

RmFloat4 RmGUIWidget::getBorder() const
{
	return PRIVATE()->Border;
}

void RmGUIWidget::setBorder(RmFloat4 value)
{
	PRIVATE()->Border = value;
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