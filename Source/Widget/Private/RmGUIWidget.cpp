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
	RmVector<IRmGUIWidgetRef> ChildrenList;

	float FixedWidth = VALUE_UNDEFINED, FixedHeight = VALUE_UNDEFINED;
	float MinWidth = VALUE_UNDEFINED, MinHeight = VALUE_UNDEFINED;
	float MaxWidth = VALUE_UNDEFINED, MaxHeight = VALUE_UNDEFINED;
	RmRect ClientRect, ChildrenRect;
	RmFloat4 Margin = {}, Padding = {}, Border = {};
	bool Visible = true;
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

RmArrayView<IRmGUIWidgetRef> RmGUIWidget::getChildren()
{
	return PRIVATE()->ChildrenList;
}

RmArrayView<const IRmGUIWidgetRef> RmGUIWidget::getChildren() const
{
	return PRIVATE()->ChildrenList;
}

bool RmGUIWidget::addWidget(IRmGUIWidgetRef value)
{
	auto widget = RmCast<RmGUIWidget>(value);
	if (widget == nullptr) return false;
	auto result = std::find(PRIVATE()->ChildrenList.begin(), PRIVATE()->ChildrenList.end(), widget);
	if (result == PRIVATE()->ChildrenList.end()) PRIVATE()->ChildrenList.push_back(widget);
	widget->setContext(PRIVATE()->Context);
	widget->setParent(this);
	return true;
}

bool RmGUIWidget::removeWidget(IRmGUIWidgetRef value)
{
	auto widget = RmCast<RmGUIWidget>(value);
	auto result = std::remove(PRIVATE()->ChildrenList.begin(), PRIVATE()->ChildrenList.end(), widget);
	if (result == PRIVATE()->ChildrenList.end()) return false;
	PRIVATE()->ChildrenList.erase(result, PRIVATE()->ChildrenList.end());
	widget->setContext(nullptr);
	widget->setParent(nullptr);
	return true;
}

bool RmGUIWidget::filter(IRmGUIReactorRaw source, IRmGUIEventRaw event)
{
	return false;
}

void RmGUIWidget::handle(IRmGUIReactorRaw source, IRmGUIEventRaw event)
{
	switch (event->Type)
	{
	case RmHash("Show"):
	{
		showEvent(RmCast<IRmGUIShowEvent>(event));
	} break;
	case RmHash("Hide"):
	{
		hideEvent(RmCast<IRmGUIHideEvent>(event));
	} break;
	case RmHash("Close"):
	{
		closeEvent(RmCast<IRmGUICloseEvent>(event));
	} break;
	case RmHash("Move"):
	{
		moveEvent(RmCast<IRmGUIMoveEvent>(event));
	} break;
	case RmHash("Resize"):
	{
		resizeEvent(RmCast<IRmGUIResizeEvent>(event));
	} break;
	case RmHash("MouseDown"):
	{
		mousePressEvent(RmCast<IRmGUIMouseDownEvent>(event));
	} break;
	case RmHash("MouseUp"):
	{
		mouseReleaseEvent(RmCast<IRmGUIMouseUpEvent>(event));
	} break;
	case RmHash("DoubleClick"):
	{
		mouseDoubleClickEvent(RmCast<IRmGUIMouseDblClickEvent>(event));
	} break;
	case RmHash("MouseMove"):
	{
		mouseMoveEvent(RmCast<IRmGUIMouseMoveEvent>(event));
	} break;
	case RmHash("MouseWheel"):
	{
		wheelEvent(RmCast<IRmGUIMouseWheelEvent>(event));
	} break;
	case RmHash("MouseEnter"):
	{
		enterEvent(RmCast<IRmGUIMouseEnterEvent>(event));
	} break;
	case RmHash("MouseLeave"):
	{
		leaveEvent(RmCast<IRmGUIMouseLeaveEvent>(event));
	} break;
	case RmHash("KeyDown"):
	{
		keyPressEvent(RmCast<IRmGUIKeyDownEvent>(event));
	} break;
	case RmHash("KeyUp"):
	{
		keyReleaseEvent(RmCast<IRmGUIKeyUpEvent>(event));
	} break;
	case RmHash("KeyInput"):
	{
		inputEvent(RmCast<IRmGUIKeyInputEvent>(event));
	} break;
	case RmHash("Drop"):
	{
		dropEvent(RmCast<IRmGUIDropEvent>(event));
	} break;
	case RmHash("DragMove"):
	{
		dragMoveEvent(RmCast<IRmGUIDragMoveEvent>(event));
	} break;
	case RmHash("DragEnter"):
	{
		dragEnterEvent(RmCast<IRmGUIDragEnterEvent>(event));
	} break;
	case RmHash("DragLeave"):
	{
		dragLeaveEvent(RmCast<IRmGUIDragLeaveEvent>(event));
	} break;
	case RmHash("FocusIn"):
	{
		focusInEvent(RmCast<IRmGUIFocusEvent>(event));
	} break;
	case RmHash("FocusOut"):
	{
		focusOutEvent(RmCast<IRmGUIFocusEvent>(event));
	} break;
	}
}

void RmGUIWidget::layout(RmRectRaw client)
{
	auto layout_func = [](RmRaw<IRmGUIWidget> widget)->taitank::TaitankNodeRef {
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
		return node;
		};

	auto root = layout_func(this);
	taitank::SetWidth(root, client->W);
	taitank::SetHeight(root, client->H);
	auto childList = getChildren();
	for (size_t i = 0; i < childList.size(); ++i)
	{
		auto node = layout_func(childList[i].get());
		root->AddChild(node);
	}
	taitank::DoLayout(root, VALUE_UNDEFINED, VALUE_UNDEFINED);

	auto left = taitank::GetLeft(root); auto top = taitank::GetTop(root);
	auto width = taitank::GetWidth(root); auto height = taitank::GetHeight(root);
	setRect({ client->X + left, client->Y + top, width, height });
	for (size_t i = 0; i < childList.size(); ++i)
	{
		auto node = root->GetChild(i);
		auto left = taitank::GetLeft(node); auto top = taitank::GetTop(node);
		auto width = taitank::GetWidth(node); auto height = taitank::GetHeight(node);
		childList[i]->setRect({ client->X + left, client->Y + top, width, height });
	}

	taitank::NodeFreeRecursive(root);
}

void RmGUIWidget::paint(IRmGUIPainterRaw painter, RmRectRaw client)
{
}

RmString RmGUIWidget::getAttribute(uint32_t name) const
{
	return RmString();
}

void RmGUIWidget::setAttribute(uint32_t name, RmString const& value)
{
}

bool RmGUIWidget::getVisible() const
{
	return PRIVATE()->Visible;
}

void RmGUIWidget::setVisible(bool value)
{
	PRIVATE()->Visible = value;
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

void RmGUIWidget::closeEvent(IRmGUICloseEventRaw event)
{
}

void RmGUIWidget::dragEnterEvent(IRmGUIDragEnterEventRaw event)
{
}

void RmGUIWidget::dragLeaveEvent(IRmGUIDragLeaveEventRaw event)
{
}

void RmGUIWidget::dragMoveEvent(IRmGUIDragMoveEventRaw event)
{
}

void RmGUIWidget::dropEvent(IRmGUIDropEventRaw event)
{
}

void RmGUIWidget::enterEvent(IRmGUIMouseEnterEventRaw event)
{
}

void RmGUIWidget::focusInEvent(IRmGUIFocusEventRaw event)
{
}

void RmGUIWidget::focusOutEvent(IRmGUIFocusEventRaw event)
{
}

void RmGUIWidget::hideEvent(IRmGUIHideEventRaw event)
{
}

void RmGUIWidget::inputEvent(IRmGUIKeyInputEventRaw event)
{
}

void RmGUIWidget::keyPressEvent(IRmGUIKeyDownEventRaw event)
{
}

void RmGUIWidget::keyReleaseEvent(IRmGUIKeyUpEventRaw event)
{
}

void RmGUIWidget::leaveEvent(IRmGUIMouseLeaveEventRaw event)
{
}

void RmGUIWidget::mouseDoubleClickEvent(IRmGUIMouseEventRaw event)
{
}

void RmGUIWidget::mouseMoveEvent(IRmGUIMouseEventRaw event)
{
}

void RmGUIWidget::mousePressEvent(IRmGUIMouseEventRaw event)
{
}

void RmGUIWidget::mouseReleaseEvent(IRmGUIMouseEventRaw event)
{
}

void RmGUIWidget::moveEvent(IRmGUIMoveEventRaw event)
{
}

void RmGUIWidget::resizeEvent(IRmGUIResizeEventRaw event)
{
}

void RmGUIWidget::showEvent(IRmGUIShowEventRaw event)
{
}

void RmGUIWidget::tabletEvent(IRmGUIMouseTabletEventRaw event)
{
}

void RmGUIWidget::wheelEvent(IRmGUIMouseWheelEventRaw event)
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