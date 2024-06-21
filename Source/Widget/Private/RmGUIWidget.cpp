#include "RmGUIWidget.h"
#include "RmGUIWidget.h"
#include "RmGUIWidget.h"
#include "RmGUIWidget.h"
#include "RmGUIWidget.h"
#include "RmGUIWidget.h"
#include "RmGUIWidget.h"
#include "RmGUIWidget.h"
#include "RmGUIWidget.h"
#include "RmGUIWidget.h"
#include "RmGUIWidget.h"
#include "RmGUIWidget.h"
#include "RmGUIWidget.h"
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
	float FixedWidth = VALUE_UNDEFINED, FixedHeight = VALUE_UNDEFINED;
	float MinWidth = VALUE_UNDEFINED, MinHeight = VALUE_UNDEFINED;
	float MaxWidth = VALUE_UNDEFINED, MaxHeight = VALUE_UNDEFINED;
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
	PRIVATE()->Parent = parent;
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
	widget->setParent(this);
	return true;
}

bool RmGUIWidget::removeWidget(RmRef<IRmGUIWidget> value)
{
	auto widget = RmCast<RmGUIWidget>(value);
	auto result = std::remove(PRIVATE()->ChildrenList.begin(), PRIVATE()->ChildrenList.end(), widget);
	if (result == PRIVATE()->ChildrenList.end()) return false;
	PRIVATE()->ChildrenList.erase(result, PRIVATE()->ChildrenList.end());
	widget->setContext(nullptr);
	widget->setParent(nullptr);
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
		auto node = taitank::NodeCreate();
		taitank::SetWidth(node, widget->getFixedWidth());
		taitank::SetHeight(node, widget->getFixedHeight());
		taitank::SetMinWidth(node, widget->getMinWidth());
		taitank::SetMinHeight(node, widget->getMinHeight());
		taitank::SetMaxWidth(node, widget->getMaxWidth());
		taitank::SetMaxHeight(node, widget->getMaxHeight());
		taitank::SetBorder(node, taitank::CSSDirection::CSS_LEFT, widget->getBorder().X);
		taitank::SetBorder(node, taitank::CSSDirection::CSS_TOP, widget->getBorder().Y);
		taitank::SetBorder(node, taitank::CSSDirection::CSS_RIGHT, widget->getBorder().Z);
		taitank::SetBorder(node, taitank::CSSDirection::CSS_BOTTOM, widget->getBorder().W);
		taitank::SetMargin(node, taitank::CSSDirection::CSS_LEFT, widget->getMargin().X);
		taitank::SetMargin(node, taitank::CSSDirection::CSS_TOP, widget->getMargin().Y);
		taitank::SetMargin(node, taitank::CSSDirection::CSS_RIGHT, widget->getMargin().Z);
		taitank::SetMargin(node, taitank::CSSDirection::CSS_BOTTOM, widget->getMargin().W);
		taitank::SetPadding(node, taitank::CSSDirection::CSS_LEFT, widget->getPadding().X);
		taitank::SetPadding(node, taitank::CSSDirection::CSS_TOP, widget->getPadding().Y);
		taitank::SetPadding(node, taitank::CSSDirection::CSS_RIGHT, widget->getPadding().Z);
		taitank::SetPadding(node, taitank::CSSDirection::CSS_BOTTOM, widget->getPadding().W);
		taitank::SetAlignItems(node, taitank::FlexAlign::FLEX_ALIGN_CENTER);
		taitank::SetJustifyContent(node, taitank::FlexAlign::FLEX_ALIGN_SPACE_EVENLY);
		return node;
		};

	auto root = layout_func(this);
	taitank::SetPosition(root, taitank::CSSDirection::CSS_LEFT, client->X);
	taitank::SetPosition(root, taitank::CSSDirection::CSS_TOP, client->Y);
	taitank::SetWidth(root, client->W);
	taitank::SetHeight(root, client->H);
	auto childList = getChildren();
	for (size_t i = 0; i < childList.size(); ++i) root->AddChild(layout_func(childList[i].get()));
	taitank::DoLayout(root, VALUE_UNDEFINED, VALUE_UNDEFINED);

	for (size_t i = 0; i < childList.size(); ++i)
	{
		auto node = root->GetChild(i);
		auto left = taitank::GetLeft(node); auto top = taitank::GetTop(node);
		auto width = taitank::GetWidth(node); auto height = taitank::GetHeight(node);
		childList[i]->setRect({ left, top, width, height });
	}
	auto left = taitank::GetLeft(root); auto top = taitank::GetTop(root);
	auto width = taitank::GetWidth(root); auto height = taitank::GetHeight(root);
	setRect({ left, top, width, height });

	taitank::NodeFreeRecursive(root);
}

void RmGUIWidget::paint(rmpainter painter, rmrect client)
{
	printf("%f %f %f %f\n", client->X, client->Y, client->W, client->H);
	auto childList = getChildren();
	for (size_t i = 0; i < childList.size(); ++i)
	{
		auto bounds = childList[i]->getRect();
		childList[i]->paint(painter, &bounds);
	}
}

RmString RmGUIWidget::getAttribute(uint32_t name) const
{
	return RmString();
}

void RmGUIWidget::setAttribute(uint32_t name, RmString const& value)
{
}

float RmGUIWidget::getPositionX() const
{
	return PRIVATE()->ClientRect.X;
}

void RmGUIWidget::setPositionX(float value)
{
	PRIVATE()->ClientRect.X = value;
}

float RmGUIWidget::getPositionY() const
{
	return PRIVATE()->ClientRect.Y;
}

void RmGUIWidget::setPositionY(float value)
{
	PRIVATE()->ClientRect.Y = value;
}

float RmGUIWidget::getWidth() const
{
	return PRIVATE()->ClientRect.W;
}

void RmGUIWidget::setWidth(float value)
{
	PRIVATE()->ClientRect.W = value;
}

float RmGUIWidget::getHeight() const
{
	return PRIVATE()->ClientRect.H;
}

void RmGUIWidget::setHeight(float value)
{
	PRIVATE()->ClientRect.H = value;
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

float RmGUIWidget::getFixedWidth() const
{
	return PRIVATE()->FixedWidth;
}

void RmGUIWidget::setFixedWidth(float value)
{
	PRIVATE()->FixedWidth = value;
}

float RmGUIWidget::getFixedHeight() const
{
	return PRIVATE()->FixedHeight;
}

void RmGUIWidget::setFixedHeight(float value)
{
	PRIVATE()->FixedHeight = value;
}

void RmGUIWidget::setPosition(float x, float y)
{
	setPositionX(x);
	setPositionY(y);
}

void RmGUIWidget::setSize(float width, float height)
{
	setFixedWidth(width);
	setFixedHeight(height);
}

void RmGUIWidget::setMinSize(float width, float height)
{
	setMinWidth(width);
	setMinHeight(height);
}

void RmGUIWidget::setMaxSize(float width, float height)
{
	setMaxWidth(width);
	setMaxHeight(height);
}

void RmGUIWidget::setFixedSize(float width, float height)
{
	setFixedWidth(width);
	setFixedHeight(height);
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