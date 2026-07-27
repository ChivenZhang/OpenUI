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
#include "../UIWidget.h"
#include "../UIContext.h"
#include "../UIPainter.h"
#include <yoga/Yoga.h>

class UIWidgetPrivateData : public UIWidgetPrivate
{
public:
	UIString Identity;
	UIWidgetRaw Parent = nullptr;
	UIList<UIWidgetRef> Children;
	UIFilterRaw Filter = nullptr;
	UIContextRaw Context = nullptr;
	UIPointUV3 Primitive[2];

	UI::DisplayType DisplayType = UI::DisplayFlex;
	UI::PositionType PositionType = UI::PositionRelative;
	UIValue2F Position{ UIValueF{UINAN, 0}, UIValueF{UINAN, 0} };
	float Scale = 1.0f;
	float Rotate = 0.0f;
	UIFloat2 Translate;
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
#define PRIVATE() ((UIWidgetPrivateData*) m_Private)

UIWidget::UIWidget(UIContextRaw context)
{
	m_Private = new UIWidgetPrivateData;

	PRIVATE()->Context = context;
}

UIWidget::~UIWidget()
{
	if (getContext()) getContext()->setAnimate(this, false);
	delete m_Private; m_Private = nullptr;
}

UIString UIWidget::getID() const
{
	return PRIVATE()->Identity;
}

void UIWidget::setID(UIString value)
{
	PRIVATE()->Identity = value;
}

UIWidgetRaw UIWidget::getParent() const
{
	return PRIVATE()->Parent;
}

UIListView<const UIWidgetRef> UIWidget::getWidgets() const
{
	return PRIVATE()->Children;
}

UIFilterRaw UIWidget::getEventFilter() const
{
	return PRIVATE()->Filter;
}

void UIWidget::setEventFilter(UIFilterRaw value)
{
	PRIVATE()->Filter = value;
}

UIString UIWidget::getStyleText() const
{
	return UIString();
}

void UIWidget::setStyleText(UIString value)
{
}

UIString UIWidget::getStyleText(UIString name) const
{
	return UIString();
}

void UIWidget::setStyleText(UIString name, UIString value)
{
}

UIString UIWidget::getAttribute(UIString name) const
{
	return UIString();
}

void UIWidget::setAttribute(UIString name, UIString value)
{
}

UIListView<const UIPointUV3> UIWidget::getPrimitive() const
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

bool UIWidget::addWidget(UIWidgetRef value)
{
	if (value == nullptr || value.get() == this) return false;
	auto result = std::find(PRIVATE()->Children.begin(), PRIVATE()->Children.end(), value);
	if (result == PRIVATE()->Children.end()) PRIVATE()->Children.push_back(value);
	value->setContext(getContext());
	value->setParent(this);
	if (getContext()) getContext()->layoutWidget();
	return true;
}

bool UIWidget::removeWidget(UIWidgetRef value)
{
	auto result = std::remove(PRIVATE()->Children.begin(), PRIVATE()->Children.end(), value);
	if (result == PRIVATE()->Children.end()) return false;
	PRIVATE()->Children.erase(result, PRIVATE()->Children.end());
	if (getContext()) getContext()->layoutWidget();
	value->setContext(nullptr);
	value->setParent(nullptr);
	return true;
}

void UIWidget::removeWidget()
{
	if (getContext()) getContext()->layoutWidget();
	for (size_t i = 0; i < PRIVATE()->Children.size(); ++i)
	{
		PRIVATE()->Children[i]->setContext(nullptr);
		PRIVATE()->Children[i]->setParent(nullptr);
	}
	PRIVATE()->Children.clear();
}

UIWidgetRef UIWidget::findWidget(UIString identity) const
{
	return findWidget([=](UIWidgetRef element)->bool { return element->getID() == identity; });
}

UIList<UIWidgetRef> UIWidget::findWidgets(UIString identity) const
{
	return findWidgets([=](UIWidgetRef element)->bool { return element->getID() == identity; });
}

UIWidgetRef UIWidget::findWidget(UILambda<bool(UIWidgetRef)> selector) const
{
	UIWidgetRef result;
	UILambda<bool(UIWidgetRef)> foreach_func;
	foreach_func = [&](UIWidgetRef element)->bool {
		if (selector && selector(element))
		{
			result = element;
			return false;
		}
		for (size_t i = 0; i < element->getWidgets().size(); ++i)
		{
			if (foreach_func(element->getWidgets()[i]) == false) return false;
		}
		return true;
		};
	foreach_func(std::const_pointer_cast<UIWidget>(this->shared_from_this()));
	return result;
}

UIList<UIWidgetRef> UIWidget::findWidgets(UILambda<bool(UIWidgetRef)> selector) const
{
	UIList<UIWidgetRef> result;
	UILambda<void(UIWidgetRef)> foreach_func;
	foreach_func = [&](UIWidgetRef element) {
		if (selector && selector(element))
		{
			result.push_back(element);
		}
		for (size_t i = 0; i < element->getWidgets().size(); ++i)
		{
			foreach_func(element->getWidgets()[i]);
		}
		};
	foreach_func(std::const_pointer_cast<UIWidget>(this->shared_from_this()));
	return result;
}

void UIWidget::arrange(UIRect client)
{
}

void UIWidget::layout(UIRect client)
{
}

void UIWidget::paint(UIRect client, UIPainterRaw painter)
{
	auto viewport = getViewport();
	painter->setClipRect(viewport.X, viewport.Y, viewport.W, viewport.H);
}

void UIWidget::repaint(UIRect client, UIPainterRaw painter)
{
	auto viewport = getViewport();
	painter->setClipRect(viewport.X, viewport.Y, viewport.W, viewport.H);
}

bool UIWidget::filter(UIReactorRaw source, UIEventRaw event)
{
	return false;
}

void UIWidget::handle(UIReactorRaw source, UIEventRaw event)
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

bool UIWidget::getEnable() const
{
	return PRIVATE()->Enable;
}

void UIWidget::setEnable(bool value)
{
	PRIVATE()->Enable = value;
}

bool UIWidget::getVisible() const
{
	return PRIVATE()->Visible;
}

void UIWidget::setVisible(bool value)
{
	PRIVATE()->Visible = value;
}

bool UIWidget::getAnimate() const
{
	return PRIVATE()->Animate;
}

void UIWidget::setAnimate(bool value)
{
	PRIVATE()->Animate = value;
	if (getContext()) getContext()->setAnimate(this, value);
}

UIRect UIWidget::getBounds() const
{
	return PRIVATE()->ClientRect;
}

void UIWidget::setBounds(UIRect value)
{
	PRIVATE()->ClientRect = value;
}

UIRect UIWidget::getViewport() const
{
	return PRIVATE()->ViewRect;
}

void UIWidget::setViewport(UIRect value)
{
	PRIVATE()->ViewRect = value;
}

UIRect UIWidget::getLocalBounds() const
{
	return PRIVATE()->LocalRect;
}

void UIWidget::setLocalBounds(UIRect value)
{
	PRIVATE()->LocalRect = value;
}

float UIWidget::getScale() const
{
	return PRIVATE()->Scale;
}

void UIWidget::setScale(float value)
{
	PRIVATE()->Scale = value;
}

float UIWidget::getRotate() const
{
	return PRIVATE()->Rotate;
}

void UIWidget::setRotate(float value)
{
	PRIVATE()->Rotate = value;
}

UIFloat2 UIWidget::getTranslate() const
{
	return PRIVATE()->Translate;
}

void UIWidget::setTranslate(UIFloat2 value)
{
	PRIVATE()->Translate = value;
}

float UIWidget::getPosX() const
{
	return getBounds().X;
}

float UIWidget::getPosY() const
{
	return getBounds().Y;
}

UIFloat2 UIWidget::getPos() const
{
	return UIFloat2{ getPosX(), getPosY() };
}

float UIWidget::getLocalX() const
{
	return 0.0f;
}

float UIWidget::getLocalY() const
{
	return 0.0f;
}

UIFloat2 UIWidget::getLocalPos() const
{
	return UIFloat2();
}

float UIWidget::getWidth() const
{
	return getBounds().W;
}

float UIWidget::getHeight() const
{
	return getBounds().H;
}

UIFloat2 UIWidget::getSize() const
{
	return UIFloat2{ getWidth(), getHeight() };
}

bool UIWidget::inBounds(UIFloat2 pos)
{
	return inBounds(pos.X, pos.Y);
}

bool UIWidget::inBounds(float x, float y)
{
	auto viewport = UIOverlap(getViewport(), getBounds());
	return (viewport.X <= x && x <= viewport.X + viewport.W
		&& viewport.Y <= y && y <= viewport.Y + viewport.H);
}

UI::DisplayType UIWidget::getDisplayType() const
{
	return PRIVATE()->DisplayType;
}

void UIWidget::setDisplayType(UI::DisplayType value)
{
	PRIVATE()->DisplayType = value;
}

UI::PositionType UIWidget::getPositionType() const
{
	return PRIVATE()->PositionType;
}

void UIWidget::setPositionType(UI::PositionType value)
{
	PRIVATE()->PositionType = value;
}

UIValueF UIWidget::getFixedPosX() const
{
	return PRIVATE()->Position[0];
}

void UIWidget::setFixedPosX(UIValueF value)
{
	PRIVATE()->Position[0] = value;
}

UIValueF UIWidget::getFixedPosY() const
{
	return PRIVATE()->Position[1];
}

void UIWidget::setFixedPosY(UIValueF value)
{
	PRIVATE()->Position[1] = value;
}

UIValue2F UIWidget::getFixedPos() const
{
	return PRIVATE()->Position;
}

void UIWidget::setFixedPos(UIValueF left, UIValueF top)
{
	setFixedPosX(left);
	setFixedPosY(top);
}

UIValueF UIWidget::getMinWidth() const
{
	return PRIVATE()->MinWidth;
}

void UIWidget::setMinWidth(UIValueF value)
{
	PRIVATE()->MinWidth = value;
}

UIValueF UIWidget::getMaxWidth() const
{
	return PRIVATE()->MaxWidth;
}

void UIWidget::setMaxWidth(UIValueF value)
{
	PRIVATE()->MaxWidth = value;
}

UIValueF UIWidget::getFixedWidth() const
{
	return PRIVATE()->FixedWidth;
}

void UIWidget::setFixedWidth(UIValueF value)
{
	PRIVATE()->FixedWidth = value;
}

UIValueF UIWidget::getMinHeight() const
{
	return PRIVATE()->MinHeight;
}

void UIWidget::setMinHeight(UIValueF value)
{
	PRIVATE()->MinHeight = value;
}

UIValueF UIWidget::getMaxHeight() const
{
	return PRIVATE()->MaxHeight;
}

void UIWidget::setMaxHeight(UIValueF value)
{
	PRIVATE()->MaxHeight = value;
}

UIValueF UIWidget::getFixedHeight() const
{
	return PRIVATE()->FixedHeight;
}

void UIWidget::setFixedHeight(UIValueF value)
{
	PRIVATE()->FixedHeight = value;
}

UIValue2F UIWidget::getMinSize() const
{
	return UIValue2F{ PRIVATE()->MinWidth, PRIVATE()->MinHeight };
}

void UIWidget::setMinSize(UIValueF width, UIValueF height)
{
	setMinWidth(width);
	setMinHeight(height);
}

UIValue2F UIWidget::getMaxSize() const
{
	return UIValue2F{ PRIVATE()->MaxWidth, PRIVATE()->MaxHeight };
}

void UIWidget::setMaxSize(UIValueF width, UIValueF height)
{
	setMaxWidth(width);
	setMaxHeight(height);
}

UIValue2F UIWidget::getFixedSize() const
{
	return UIValue2F{ PRIVATE()->FixedWidth, PRIVATE()->FixedHeight };
}

void UIWidget::setFixedSize(UIValueF width, UIValueF height)
{
	setFixedWidth(width);
	setFixedHeight(height);
}

UIValue4F UIWidget::getBorder() const
{
	return PRIVATE()->Border;
}

void UIWidget::setBorder(UIValue4F value)
{
	PRIVATE()->Border = value;
}

UIValue4F UIWidget::getMargin() const
{
	return PRIVATE()->Margin;
}

void UIWidget::setMargin(UIValue4F value)
{
	PRIVATE()->Margin = value;
}

UIValue4F UIWidget::getPadding() const
{
	return PRIVATE()->Padding;
}

void UIWidget::setPadding(UIValue4F value)
{
	PRIVATE()->Padding = value;
}

UIValue2F UIWidget::getSpacing() const
{
	return PRIVATE()->Spacing;
}

void UIWidget::setSpacing(UIValue2F value)
{
	PRIVATE()->Spacing = value;
}

UI::FlexDirection UIWidget::getFlexDirection() const
{
	return PRIVATE()->FlexFlow.FlexDirection;
}

void UIWidget::setFlexDirection(UI::FlexDirection value)
{
	PRIVATE()->FlexFlow.FlexDirection = value;
}

UI::FlexWrap UIWidget::getFlexWrap() const
{
	return PRIVATE()->FlexFlow.FlexWrap;
}

void UIWidget::setFlexWrap(UI::FlexWrap value)
{
	PRIVATE()->FlexFlow.FlexWrap = value;
}

UI::JustifyContent UIWidget::getJustifyContent() const
{
	return PRIVATE()->JustifyContent;
}

void UIWidget::setJustifyContent(UI::JustifyContent value)
{
	PRIVATE()->JustifyContent = value;
}

UI::AlignItems UIWidget::getAlignItems() const
{
	return PRIVATE()->AlignItems;
}

void UIWidget::setAlignItems(UI::AlignItems value)
{
	PRIVATE()->AlignItems = value;
}

UI::AlignContent UIWidget::getAlignContent() const
{
	return PRIVATE()->AlignContent;
}

void UIWidget::setAlignContent(UI::AlignContent value)
{
	PRIVATE()->AlignContent = value;
}

void UIWidget::setFlexFlow(UI::FlexDirection direction, UI::FlexWrap wrap)
{
	setFlexDirection(direction);
	setFlexWrap(wrap);
}

UI::FlexGrow UIWidget::getFlexGrow() const
{
	return PRIVATE()->Flex.FlexGrow;
}

void UIWidget::setFlexGrow(UI::FlexGrow value)
{
	PRIVATE()->Flex.FlexGrow = value;
}

UI::FlexShrink UIWidget::getFlexShrink() const
{
	return PRIVATE()->Flex.FlexShrink;
}

void UIWidget::setFlexShrink(UI::FlexShrink value)
{
	PRIVATE()->Flex.FlexShrink = value;
}

UI::FlexBasis UIWidget::getFlexBasis() const
{
	return PRIVATE()->Flex.FlexBasis;
}

void UIWidget::setFlexBasis(UI::FlexBasis value)
{
	PRIVATE()->Flex.FlexBasis = value;
}

UI::AlignSelf UIWidget::getAlignSelf() const
{
	return PRIVATE()->AlignSelf;
}

void UIWidget::setAlignSelf(UI::AlignSelf value)
{
	PRIVATE()->AlignSelf = value;
}

void UIWidget::setFlex(UI::FlexGrow grow, UI::FlexShrink shrink, UI::FlexBasis basis)
{
	setFlexGrow(grow);
	setFlexShrink(shrink);
	setFlexBasis(basis);
}

void UIWidget::closeEvent(UICloseEventRaw event)
{
}

void UIWidget::dragEnterEvent(UIDragEnterEventRaw event)
{
}

void UIWidget::dragLeaveEvent(UIDragLeaveEventRaw event)
{
}

void UIWidget::dragMoveEvent(UIDragMoveEventRaw event)
{
}

void UIWidget::dropEvent(UIDropEventRaw event)
{
}

void UIWidget::enterEvent(UIMouseEventRaw event)
{
}

void UIWidget::focusInEvent(UIFocusEventRaw event)
{
}

void UIWidget::focusOutEvent(UIFocusEventRaw event)
{
}

void UIWidget::hideEvent(UIHideEventRaw event)
{
}

void UIWidget::inputEvent(UITextInputEventRaw event)
{
}

void UIWidget::keyPressEvent(UIKeyEventRaw event)
{
}

void UIWidget::keyReleaseEvent(UIKeyEventRaw event)
{
}

void UIWidget::leaveEvent(UIMouseEventRaw event)
{
}

void UIWidget::mouseDoubleEvent(UIMouseEventRaw event)
{
}

void UIWidget::mouseMoveEvent(UIMouseEventRaw event)
{
}

void UIWidget::mousePressEvent(UIMouseEventRaw event)
{
}

void UIWidget::mouseReleaseEvent(UIMouseEventRaw event)
{
}

void UIWidget::moveEvent(UIMoveEventRaw event)
{
}

void UIWidget::resizeEvent(UIResizeEventRaw event)
{
}

void UIWidget::showEvent(UIShowEventRaw event)
{
}

void UIWidget::tabletEvent(UIMouseTabletEventRaw event)
{
}

void UIWidget::wheelEvent(UIMouseWheelEventRaw event)
{
}

void UIWidget::timerEvent(UITimerEventRaw event)
{
}

UIContextRaw UIWidget::getContext() const
{
	return PRIVATE()->Context;
}

void UIWidget::setContext(UIContextRaw value)
{
	if (getContext()) getContext()->setAnimate(this, false);
	PRIVATE()->Context = value;
	if (getContext()) getContext()->setAnimate(this, getAnimate());
	for (size_t i = 0; i < getWidgets().size(); ++i) getWidgets()[i]->setContext(value);
}

void UIWidget::setParent(UIWidgetRaw value)
{
	if (value == this) return;
	PRIVATE()->Parent = value;
}