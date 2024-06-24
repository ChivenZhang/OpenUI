#include "RmGUIWidget.h"
#include <taitank.h>
namespace flex = taitank;

class RmGUIWidgetPrivateData : public RmGUIWidgetPrivate
{
public:
	RmRaw<IRmGUIContext> Context = nullptr;
	RmRaw<IRmGUIWidget> Parent = nullptr;
	RmVector<IRmGUIWidgetRef> ChildrenList;

	float MinWidth = RmNAN, MinHeight = RmNAN;
	float MaxWidth = RmNAN, MaxHeight = RmNAN;
	float FixedWidth = RmNAN, FixedHeight = RmNAN;
	bool Visible = true, Enable = true;
	RmRect ClientRect, Viewport, ChildrenRect;
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
	delete m_PrivateData; m_PrivateData = nullptr;
}

RmRect RmGUIWidget::getRect() const
{
	return PRIVATE()->ClientRect;
}

void RmGUIWidget::setRect(RmRect client)
{
	PRIVATE()->ClientRect = client;
}

RmRect RmGUIWidget::getViewport() const
{
	return PRIVATE()->Viewport;
}

void RmGUIWidget::setViewport(RmRect value)
{
	PRIVATE()->Viewport = value;
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
	auto childList = getChildren();
	for (size_t i = 0; i < childList.size(); ++i)
	{
		childList[i]->setViewport(childList[i]->getRect());
	}
}

void RmGUIWidget::paint(IRmGUIPainterRaw painter, RmRectRaw client)
{
	auto viewport = getViewport();
	painter->setClipRect(viewport.X, viewport.Y, viewport.W, viewport.H);
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

bool RmGUIWidget::getEnable() const
{
	return PRIVATE()->Enable;
}

void RmGUIWidget::setEnable(bool value)
{
	PRIVATE()->Enable = value;
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