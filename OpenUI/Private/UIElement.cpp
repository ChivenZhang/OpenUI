/*=================================================
* Copyright © 2020-2025 ChivenZhang.
* All Rights Reserved.
* =====================Note=========================
*
*
* ====================History=======================
* Created by ChivenZhang@gmail.com.
*
* =================================================*/
#include "../UIElement.h"
#include "../UIContext.h"
#include "../UIPainter.h"
#include <yoga/Yoga.h>

class UIElementPrivateData : public UIElementPrivate
{
public:
	UIString Identity;
	UIElementRaw Parent = nullptr;
	UIList<UIElementRef> Children;
	UIFilterRaw Filter = nullptr;
	UIContextRaw Context = nullptr;
	UIPointUV3 Primitive[2];

	UI::DisplayType DisplayType = UI::DisplayFlex;
	UI::PositionType PositionType = UI::PositionRelative;
	UIValue2F Position{ UIValueF{UINAN, 0}, UIValueF{UINAN, 0} };
	UIValueF MinWidth = { UINAN, 0 }, MinHeight = { UINAN, 0 };
	UIValueF MaxWidth = { UINAN, 0 }, MaxHeight = { UINAN, 0 };
	UIValueF FixedWidth = { UINAN, 0 }, FixedHeight = { UINAN, 0 };
	UIValue4F Border{ UIValueF{UINAN, 0}, UIValueF{UINAN, 0}, UIValueF{UINAN, 0}, UIValueF{UINAN, 0} };
	UIValue4F Margin{ UIValueF{UINAN, 0}, UIValueF{UINAN, 0}, UIValueF{UINAN, 0}, UIValueF{UINAN, 0} };
	UIValue4F Padding{ UIValueF{UINAN, 0}, UIValueF{UINAN, 0}, UIValueF{UINAN, 0}, UIValueF{UINAN, 0} };
	UIValue2F Spacing{ UIValueF{UINAN, 0}, UIValueF{UINAN, 0} };
	UIRect LocalRect, ClientRect, ViewRect;
	bool Enable = true, Visible = true, Animate = false;

	struct
	{
		UI::FlexDirection FlexDirection = UI::FlexDirectionRow;
		UI::FlexWrap FlexWrap = UI::FlexNoWrap;
	} FlexFlow;
	UI::JustifyContent JustifyContent = UI::JustifyFlexStart;
	UI::AlignItems AlignItems = UI::AlignStretch;
	UI::AlignContent AlignContent = UI::AlignStretch;

	struct
	{
		UI::FlexGrow FlexGrow = UIValueF{ 0, 0 };
		UI::FlexShrink FlexShrink = UIValueF{ 1, 0 };
		UI::FlexBasis FlexBasis = UIValueF{ UINAN, 0 };
	} Flex;
	// int32_t Order;
	UI::AlignSelf AlignSelf = UI::AlignAuto;
};
#define PRIVATE() ((UIElementPrivateData*) m_Private)

UIElement::UIElement(UIContextRaw context)
{
	m_Private = new UIElementPrivateData;

	PRIVATE()->Context = context;
}

UIElement::~UIElement()
{
	if (getContext()) getContext()->setAnimate(this, false);
	delete m_Private; m_Private = nullptr;
}

UIString UIElement::getID() const
{
	return PRIVATE()->Identity;
}

void UIElement::setID(UIString value)
{
	PRIVATE()->Identity = value;
}

UIElementRaw UIElement::getParent() const
{
	return PRIVATE()->Parent;
}

UIListView<const UIElementRef> UIElement::getChildren() const
{
	return PRIVATE()->Children;
}

UIFilterRaw UIElement::getEventFilter() const
{
	return PRIVATE()->Filter;
}

void UIElement::setEventFilter(UIFilterRaw value)
{
	PRIVATE()->Filter = value;
}

UIString UIElement::getStyleText() const
{
	return UIString();
}

void UIElement::setStyleText(UIString value)
{
}

UIString UIElement::getStyleText(UIString name) const
{
	return UIString();
}

void UIElement::setStyleText(UIString name, UIString value)
{
}

UIString UIElement::getAttribute(UIString name) const
{
	return UIString();
}

void UIElement::setAttribute(UIString name, UIString value)
{
}

UIListView<const UIPointUV3> UIElement::getPrimitive() const
{
	auto viewport = UIOverlap(getViewport(), getBounds());
	PRIVATE()->Primitive[0].P0 = { viewport.X, viewport.Y };
	PRIVATE()->Primitive[0].P1 = { viewport.X + viewport.W, viewport.Y };
	PRIVATE()->Primitive[0].P2 = { viewport.X + viewport.W, viewport.Y + viewport.H };
	PRIVATE()->Primitive[1].P0 = { viewport.X, viewport.Y };
	PRIVATE()->Primitive[1].P1 = { viewport.X + viewport.W, viewport.Y + viewport.H };
	PRIVATE()->Primitive[1].P2 = { viewport.X, viewport.Y + viewport.H };
	return PRIVATE()->Primitive;
}

bool UIElement::addElement(UIElementRef value)
{
	if (value == nullptr || value.get() == this) return false;
	auto result = std::find(PRIVATE()->Children.begin(), PRIVATE()->Children.end(), value);
	if (result == PRIVATE()->Children.end()) PRIVATE()->Children.push_back(value);
	value->setContext(getContext());
	value->setParent(this);
	if (getContext()) getContext()->layoutElement();
	return true;
}

bool UIElement::removeElement(UIElementRef value)
{
	auto result = std::remove(PRIVATE()->Children.begin(), PRIVATE()->Children.end(), value);
	if (result == PRIVATE()->Children.end()) return false;
	PRIVATE()->Children.erase(result, PRIVATE()->Children.end());
	if (getContext()) getContext()->layoutElement();
	value->setContext(nullptr);
	value->setParent(nullptr);
	return true;
}

void UIElement::removeElement()
{
	if (getContext()) getContext()->layoutElement();
	for (size_t i = 0; i < PRIVATE()->Children.size(); ++i)
	{
		PRIVATE()->Children[i]->setContext(nullptr);
		PRIVATE()->Children[i]->setParent(nullptr);
	}
	PRIVATE()->Children.clear();
}

UIElementRef UIElement::getElementByID(UIString identity) const
{
	return getElement([=](UIElementRef element)->bool { return element->getID() == identity; });
}

UIList<UIElementRef> UIElement::getElementsByID(UIString identity) const
{
	return getElements([=](UIElementRef element)->bool { return element->getID() == identity; });
}

UIElementRef UIElement::getElement(UILambda<bool(UIElementRef)> selector) const
{
	UIElementRef result;
	UILambda<bool(UIElementRef)> foreach_func;
	foreach_func = [&](UIElementRef element)->bool {
		if (selector && selector(element))
		{
			result = element;
			return false;
		}
		for (size_t i = 0; i < element->getChildren().size(); ++i)
		{
			if (foreach_func(element->getChildren()[i]) == false) return false;
		}
		return true;
		};
	foreach_func(std::const_pointer_cast<UIElement>(this->shared_from_this()));
	return result;
}

UIList<UIElementRef> UIElement::getElements(UILambda<bool(UIElementRef)> selector) const
{
	UIList<UIElementRef> result;
	UILambda<void(UIElementRef)> foreach_func;
	foreach_func = [&](UIElementRef element) {
		if (selector && selector(element))
		{
			result.push_back(element);
		}
		for (size_t i = 0; i < element->getChildren().size(); ++i)
		{
			foreach_func(element->getChildren()[i]);
		}
		};
	foreach_func(std::const_pointer_cast<UIElement>(this->shared_from_this()));
	return result;
}

void UIElement::arrange(UIRect client)
{
}

void UIElement::layout(UIRect client)
{
}

void UIElement::paint(UIRect client, UIPainterRaw painter)
{
	auto viewport = getViewport();
	painter->setClipRect(viewport.X, viewport.Y, viewport.W, viewport.H);
}

void UIElement::repaint(UIRect client, UIPainterRaw painter)
{
	auto viewport = getViewport();
	painter->setClipRect(viewport.X, viewport.Y, viewport.W, viewport.H);
}

bool UIElement::filter(UIReactorRaw source, UIEventRaw event)
{
	return false;
}

void UIElement::handle(UIReactorRaw source, UIEventRaw event)
{
	switch (event->Type)
	{
	case UIHash("Show"):
	{
		showEvent(UICast<UIShowEvent>(event));
	} break;
	case UIHash("Close"):
	{
		closeEvent(UICast<UICloseEvent>(event));
	} break;
	case UIHash("Hide"):
	{
		hideEvent(UICast<UIHideEvent>(event));
	} break;
	case UIHash("Move"):
	{
		moveEvent(UICast<UIMoveEvent>(event));
	} break;
	case UIHash("Resize"):
	{
		resizeEvent(UICast<UIResizeEvent>(event));
	} break;
	case UIHash("MouseDown"):
	{
		mousePressEvent(UICast<UIMouseDownEvent>(event));
	} break;
	case UIHash("MouseUp"):
	{
		mouseReleaseEvent(UICast<UIMouseUpEvent>(event));
	} break;
	case UIHash("DoubleClick"):
	{
		mouseDoubleEvent(UICast<UIMouseDblClickEvent>(event));
	} break;
	case UIHash("MouseMove"):
	{
		mouseMoveEvent(UICast<UIMouseMoveEvent>(event));
	} break;
	case UIHash("MouseWheel"):
	{
		wheelEvent(UICast<UIMouseWheelEvent>(event));
	} break;
	case UIHash("MouseEnter"):
	{
		enterEvent(UICast<UIMouseEnterEvent>(event));
	} break;
	case UIHash("MouseLeave"):
	{
		leaveEvent(UICast<UIMouseLeaveEvent>(event));
	} break;
	case UIHash("KeyDown"):
	{
		keyPressEvent(UICast<UIKeyDownEvent>(event));
	} break;
	case UIHash("KeyUp"):
	{
		keyReleaseEvent(UICast<UIKeyUpEvent>(event));
	} break;
	case UIHash("TextInput"):
	{
		inputEvent(UICast<UITextInputEvent>(event));
	} break;
	case UIHash("Drop"):
	{
		dropEvent(UICast<UIDropEvent>(event));
	} break;
	case UIHash("DragMove"):
	{
		dragMoveEvent(UICast<UIDragMoveEvent>(event));
	} break;
	case UIHash("DragEnter"):
	{
		dragEnterEvent(UICast<UIDragEnterEvent>(event));
	} break;
	case UIHash("DragLeave"):
	{
		dragLeaveEvent(UICast<UIDragLeaveEvent>(event));
	} break;
	case UIHash("FocusIn"):
	{
		focusInEvent(UICast<UIFocusEvent>(event));
	} break;
	case UIHash("FocusOut"):
	{
		focusOutEvent(UICast<UIFocusEvent>(event));
	} break;
	case UIHash("Timer"):
	{
		timerEvent(UICast<UITimerEvent>(event));
	} break;
	}
}

bool UIElement::getEnable() const
{
	return PRIVATE()->Enable;
}

void UIElement::setEnable(bool value)
{
	PRIVATE()->Enable = value;
}

bool UIElement::getVisible() const
{
	return PRIVATE()->Visible;
}

void UIElement::setVisible(bool value)
{
	PRIVATE()->Visible = value;
}

bool UIElement::getAnimate() const
{
	return PRIVATE()->Animate;
}

void UIElement::setAnimate(bool value)
{
	PRIVATE()->Animate = value;
	if (getContext()) getContext()->setAnimate(this, value);
}

UIRect UIElement::getBounds() const
{
	return PRIVATE()->ClientRect;
}

void UIElement::setBounds(UIRect value)
{
	PRIVATE()->ClientRect = value;
}

UIRect UIElement::getViewport() const
{
	return PRIVATE()->ViewRect;
}

void UIElement::setViewport(UIRect value)
{
	PRIVATE()->ViewRect = value;
}

UIRect UIElement::getLocalBounds() const
{
	return PRIVATE()->LocalRect;
}

void UIElement::setLocalBounds(UIRect value)
{
	PRIVATE()->LocalRect = value;
}

float UIElement::getPosX() const
{
	return getBounds().X;
}

float UIElement::getPosY() const
{
	return getBounds().Y;
}

UIFloat2 UIElement::getPos() const
{
	return UIFloat2{ getPosX(), getPosY() };
}

float UIElement::getLocalX() const
{
	return 0.0f;
}

float UIElement::getLocalY() const
{
	return 0.0f;
}

UIFloat2 UIElement::getLocalPos() const
{
	return UIFloat2();
}

float UIElement::getWidth() const
{
	return getBounds().W;
}

float UIElement::getHeight() const
{
	return getBounds().H;
}

UIFloat2 UIElement::getSize() const
{
	return UIFloat2{ getWidth(), getHeight() };
}

bool UIElement::inBounds(UIFloat2 pos)
{
	return inBounds(pos.X, pos.Y);
}

bool UIElement::inBounds(float x, float y)
{
	auto viewport = UIOverlap(getViewport(), getBounds());
	return (viewport.X <= x && x <= viewport.X + viewport.W
		&& viewport.Y <= y && y <= viewport.Y + viewport.H);
}

UI::DisplayType UIElement::getDisplayType() const
{
	return PRIVATE()->DisplayType;
}

void UIElement::setDisplayType(UI::DisplayType value)
{
	PRIVATE()->DisplayType = value;
}

UI::PositionType UIElement::getPositionType() const
{
	return PRIVATE()->PositionType;
}

void UIElement::setPositionType(UI::PositionType value)
{
	PRIVATE()->PositionType = value;
}

UIValueF UIElement::getFixedPosX() const
{
	return PRIVATE()->Position[0];
}

void UIElement::setFixedPosX(UIValueF value)
{
	PRIVATE()->Position[0] = value;
}

UIValueF UIElement::getFixedPosY() const
{
	return PRIVATE()->Position[1];
}

void UIElement::setFixedPosY(UIValueF value)
{
	PRIVATE()->Position[1] = value;
}

UIValue2F UIElement::getFixedPos() const
{
	return PRIVATE()->Position;
}

void UIElement::setFixedPos(UIValueF left, UIValueF top)
{
	setFixedPosX(left);
	setFixedPosY(top);
}

UIValueF UIElement::getMinWidth() const
{
	return PRIVATE()->MinWidth;
}

void UIElement::setMinWidth(UIValueF value)
{
	PRIVATE()->MinWidth = value;
}

UIValueF UIElement::getMaxWidth() const
{
	return PRIVATE()->MaxWidth;
}

void UIElement::setMaxWidth(UIValueF value)
{
	PRIVATE()->MaxWidth = value;
}

UIValueF UIElement::getFixedWidth() const
{
	return PRIVATE()->FixedWidth;
}

void UIElement::setFixedWidth(UIValueF value)
{
	PRIVATE()->FixedWidth = value;
}

UIValueF UIElement::getMinHeight() const
{
	return PRIVATE()->MinHeight;
}

void UIElement::setMinHeight(UIValueF value)
{
	PRIVATE()->MinHeight = value;
}

UIValueF UIElement::getMaxHeight() const
{
	return PRIVATE()->MaxHeight;
}

void UIElement::setMaxHeight(UIValueF value)
{
	PRIVATE()->MaxHeight = value;
}

UIValueF UIElement::getFixedHeight() const
{
	return PRIVATE()->FixedHeight;
}

void UIElement::setFixedHeight(UIValueF value)
{
	PRIVATE()->FixedHeight = value;
}

UIValue2F UIElement::getMinSize() const
{
	return UIValue2F{ PRIVATE()->MinWidth, PRIVATE()->MinHeight };
}

void UIElement::setMinSize(UIValueF width, UIValueF height)
{
	setMinWidth(width);
	setMinHeight(height);
}

UIValue2F UIElement::getMaxSize() const
{
	return UIValue2F{ PRIVATE()->MaxWidth, PRIVATE()->MaxHeight };
}

void UIElement::setMaxSize(UIValueF width, UIValueF height)
{
	setMaxWidth(width);
	setMaxHeight(height);
}

UIValue2F UIElement::getFixedSize() const
{
	return UIValue2F{ PRIVATE()->FixedWidth, PRIVATE()->FixedHeight };
}

void UIElement::setFixedSize(UIValueF width, UIValueF height)
{
	setFixedWidth(width);
	setFixedHeight(height);
}

UIValue4F UIElement::getBorder() const
{
	return PRIVATE()->Border;
}

void UIElement::setBorder(UIValue4F value)
{
	PRIVATE()->Border = value;
}

UIValue4F UIElement::getMargin() const
{
	return PRIVATE()->Margin;
}

void UIElement::setMargin(UIValue4F value)
{
	PRIVATE()->Margin = value;
}

UIValue4F UIElement::getPadding() const
{
	return PRIVATE()->Padding;
}

void UIElement::setPadding(UIValue4F value)
{
	PRIVATE()->Padding = value;
}

UIValue2F UIElement::getSpacing() const
{
	return PRIVATE()->Spacing;
}

void UIElement::setSpacing(UIValue2F value)
{
	PRIVATE()->Spacing = value;
}

UI::FlexDirection UIElement::getFlexDirection() const
{
	return PRIVATE()->FlexFlow.FlexDirection;
}

void UIElement::setFlexDirection(UI::FlexDirection value)
{
	PRIVATE()->FlexFlow.FlexDirection = value;
}

UI::FlexWrap UIElement::getFlexWrap() const
{
	return PRIVATE()->FlexFlow.FlexWrap;
}

void UIElement::setFlexWrap(UI::FlexWrap value)
{
	PRIVATE()->FlexFlow.FlexWrap = value;
}

UI::JustifyContent UIElement::getJustifyContent() const
{
	return PRIVATE()->JustifyContent;
}

void UIElement::setJustifyContent(UI::JustifyContent value)
{
	PRIVATE()->JustifyContent = value;
}

UI::AlignItems UIElement::getAlignItems() const
{
	return PRIVATE()->AlignItems;
}

void UIElement::setAlignItems(UI::AlignItems value)
{
	PRIVATE()->AlignItems = value;
}

UI::AlignContent UIElement::getAlignContent() const
{
	return PRIVATE()->AlignContent;
}

void UIElement::setAlignContent(UI::AlignContent value)
{
	PRIVATE()->AlignContent = value;
}

void UIElement::setFlexFlow(UI::FlexDirection direction, UI::FlexWrap wrap)
{
	setFlexDirection(direction);
	setFlexWrap(wrap);
}

UI::FlexGrow UIElement::getFlexGrow() const
{
	return PRIVATE()->Flex.FlexGrow;
}

void UIElement::setFlexGrow(UI::FlexGrow value)
{
	PRIVATE()->Flex.FlexGrow = value;
}

UI::FlexShrink UIElement::getFlexShrink() const
{
	return PRIVATE()->Flex.FlexShrink;
}

void UIElement::setFlexShrink(UI::FlexShrink value)
{
	PRIVATE()->Flex.FlexShrink = value;
}

UI::FlexBasis UIElement::getFlexBasis() const
{
	return PRIVATE()->Flex.FlexBasis;
}

void UIElement::setFlexBasis(UI::FlexBasis value)
{
	PRIVATE()->Flex.FlexBasis = value;
}

UI::AlignSelf UIElement::getAlignSelf() const
{
	return PRIVATE()->AlignSelf;
}

void UIElement::setAlignSelf(UI::AlignSelf value)
{
	PRIVATE()->AlignSelf = value;
}

void UIElement::setFlex(UI::FlexGrow grow, UI::FlexShrink shrink, UI::FlexBasis basis)
{
	setFlexGrow(grow);
	setFlexShrink(shrink);
	setFlexBasis(basis);
}

void UIElement::closeEvent(UICloseEventRaw event)
{
}

void UIElement::dragEnterEvent(UIDragEnterEventRaw event)
{
}

void UIElement::dragLeaveEvent(UIDragLeaveEventRaw event)
{
}

void UIElement::dragMoveEvent(UIDragMoveEventRaw event)
{
}

void UIElement::dropEvent(UIDropEventRaw event)
{
}

void UIElement::enterEvent(UIMouseEventRaw event)
{
}

void UIElement::focusInEvent(UIFocusEventRaw event)
{
}

void UIElement::focusOutEvent(UIFocusEventRaw event)
{
}

void UIElement::hideEvent(UIHideEventRaw event)
{
}

void UIElement::inputEvent(UITextInputEventRaw event)
{
}

void UIElement::keyPressEvent(UIKeyEventRaw event)
{
}

void UIElement::keyReleaseEvent(UIKeyEventRaw event)
{
}

void UIElement::leaveEvent(UIMouseEventRaw event)
{
}

void UIElement::mouseDoubleEvent(UIMouseEventRaw event)
{
}

void UIElement::mouseMoveEvent(UIMouseEventRaw event)
{
}

void UIElement::mousePressEvent(UIMouseEventRaw event)
{
}

void UIElement::mouseReleaseEvent(UIMouseEventRaw event)
{
}

void UIElement::moveEvent(UIMoveEventRaw event)
{
}

void UIElement::resizeEvent(UIResizeEventRaw event)
{
}

void UIElement::showEvent(UIShowEventRaw event)
{
}

void UIElement::tabletEvent(UIMouseTabletEventRaw event)
{
}

void UIElement::wheelEvent(UIMouseWheelEventRaw event)
{
}

void UIElement::timerEvent(UITimerEventRaw event)
{
}

UIContextRaw UIElement::getContext() const
{
	return PRIVATE()->Context;
}

void UIElement::setContext(UIContextRaw value)
{
	if (getContext()) getContext()->setAnimate(this, false);
	PRIVATE()->Context = value;
	if (getContext()) getContext()->setAnimate(this, getAnimate());
	for (size_t i = 0; i < getChildren().size(); ++i) getChildren()[i]->setContext(value);
}

void UIElement::setParent(UIElementRaw value)
{
	if (value == this) return;
	PRIVATE()->Parent = value;
}