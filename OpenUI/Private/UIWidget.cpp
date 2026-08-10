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
#include "../UICanvas.h"
#include "../UIPainter.h"
#include <yoga/Yoga.h>

class UIWidgetPrivateData : public UIWidgetPrivate
{
public:
	UIString Identity;
	UIWidgetRaw Parent = nullptr;
	UIList<UIWidgetRef> Children;
	UIFilterRaw Filter = nullptr;
	UICanvasRaw Canvas = nullptr;

	// =============================Flex Layout==========================

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

	// ==============================CSS Style===========================

	UIStyleRef Styles;
	UIComputedStyleRef ComputedStyles;

	// =============================DOM Attrib===========================

	UIAttribRef Attribs;

	UIWidgetStyle StyleCSS;
};
#define PRIVATE() ((UIWidgetPrivateData*) m_Private)

UIWidget::UIWidget(UICanvasRaw canvas)
{
	m_Private = new UIWidgetPrivateData;
	PRIVATE()->Canvas = canvas;
	PRIVATE()->Styles = UINew<UIStyle>();
	PRIVATE()->ComputedStyles = UINew<UIComputedStyle>(PRIVATE()->Styles.get());
	PRIVATE()->Attribs = UINew<UIAttrib>();
}

UIWidget::~UIWidget()
{
	if (getCanvas()) getCanvas()->setAnimate(this, false);
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

bool UIWidget::addWidget(UIWidgetRef value)
{
	if (value == nullptr || value.get() == this) return false;
	auto result = std::find(PRIVATE()->Children.begin(), PRIVATE()->Children.end(), value);
	if (result == PRIVATE()->Children.end()) PRIVATE()->Children.push_back(value);
	value->setContext(getCanvas());
	value->setParent(this);
	if (getCanvas()) getCanvas()->layoutWidget();
	return true;
}

bool UIWidget::removeWidget(UIWidgetRef value)
{
	auto result = std::remove(PRIVATE()->Children.begin(), PRIVATE()->Children.end(), value);
	if (result == PRIVATE()->Children.end()) return false;
	PRIVATE()->Children.erase(result, PRIVATE()->Children.end());
	if (getCanvas()) getCanvas()->layoutWidget();
	value->setContext(nullptr);
	value->setParent(nullptr);
	return true;
}

void UIWidget::removeWidget()
{
	if (getCanvas()) getCanvas()->layoutWidget();
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
	if (getCanvas()) getCanvas()->setAnimate(this, value);
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

UIStyleRaw UIWidget::getStyles() const
{
	return PRIVATE()->Styles.get();
}

void UIWidget::setStyles(UIStyleRef value)
{
	PRIVATE()->Styles = value;
}

UIComputedStyleRaw UIWidget::getStyleComputed() const
{
	return PRIVATE()->ComputedStyles.get();
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
	return getStyles()->getStyleText(name);
}

bool UIWidget::setStyleText(UIString name, UIString value)
{
	if (getStyles()->setStyleText(name, value) == false)
	{
		switch(UIHash(name))
		{
		default: return false;
	    case UIHash("align-content"): setEmbedStyle(name, PRIVATE()->StyleCSS.AlignContent); break;
	    case UIHash("align-items"): setEmbedStyle(name, PRIVATE()->StyleCSS.AlignItems); break;
	    case UIHash("align-self"): setEmbedStyle(name, PRIVATE()->StyleCSS.AlignSelf); break;
	    case UIHash("alignment-baseline"): setEmbedStyle(name, PRIVATE()->StyleCSS.AlignmentBaseline); break;
	    case UIHash("baseline-shift"): setEmbedStyle(name, PRIVATE()->StyleCSS.BaselineShift); break;
	    case UIHash("baseline-source"): setEmbedStyle(name, PRIVATE()->StyleCSS.BaselineSource); break;
	    case UIHash("border"): setEmbedStyle(name, PRIVATE()->StyleCSS.Border); break;
	    case UIHash("border-bottom"): setEmbedStyle(name, PRIVATE()->StyleCSS.BorderBottom); break;
	    case UIHash("border-left"): setEmbedStyle(name, PRIVATE()->StyleCSS.BorderLeft); break;
	    case UIHash("border-right"): setEmbedStyle(name, PRIVATE()->StyleCSS.BorderRight); break;
	    case UIHash("border-top"): setEmbedStyle(name, PRIVATE()->StyleCSS.BorderTop); break;
	    case UIHash("bottom"): setEmbedStyle(name, PRIVATE()->StyleCSS.Bottom); break;
	    case UIHash("box-sizing"): setEmbedStyle(name, PRIVATE()->StyleCSS.BoxSizing); break;
	    case UIHash("clear"): setEmbedStyle(name, PRIVATE()->StyleCSS.Clear); break;
	    //case UIHash("color"): setEmbedStyle(name, PRIVATE()->StyleCSS.Color); break;
	    case UIHash("direction"): setEmbedStyle(name, PRIVATE()->StyleCSS.Direction); break;
	    case UIHash("display"): setEmbedStyle(name, PRIVATE()->StyleCSS.Display); break;
	    //case UIHash("dominant-baseline"): setEmbedStyle(name, PRIVATE()->StyleCSS.DominantBaseline); break;
	    //case UIHash("flex"): setEmbedStyle(name, PRIVATE()->StyleCSS.Flex); break;
	    case UIHash("flex-basis"): setEmbedStyle(name, PRIVATE()->StyleCSS.FlexBasis); break;
	    //case UIHash("flex-direction"): setEmbedStyle(name, PRIVATE()->StyleCSS.FlexDirection); break;
	    case UIHash("flex-grow"): setEmbedStyle(name, PRIVATE()->StyleCSS.FlexGrow); break;
	    case UIHash("flex-shrink"): setEmbedStyle(name, PRIVATE()->StyleCSS.FlexShrink); break;
	    //case UIHash("flex-wrap"): setEmbedStyle(name, PRIVATE()->StyleCSS.FlexWrap); break;
	    //case UIHash("float"): setEmbedStyle(name, PRIVATE()->StyleCSS.Float); break;
	    //case UIHash("float-defer"): setEmbedStyle(name, PRIVATE()->StyleCSS.FloatDefer); break;
	    case UIHash("float-offset"): setEmbedStyle(name, PRIVATE()->StyleCSS.FloatOffset); break;
	    //case UIHash("float-reference"): setEmbedStyle(name, PRIVATE()->StyleCSS.FloatReference); break;
	    //case UIHash("font-family"): setEmbedStyle(name, PRIVATE()->StyleCSS.FontFamily); break;
	    case UIHash("font-size"): setEmbedStyle(name, PRIVATE()->StyleCSS.FontSize); break;
	    case UIHash("font-stretch"): setEmbedStyle(name, PRIVATE()->StyleCSS.FontStretch); break;
	    //case UIHash("font-style"): setEmbedStyle(name, PRIVATE()->StyleCSS.FontStyle); break;
	    case UIHash("font-weight"): setEmbedStyle(name, PRIVATE()->StyleCSS.FontWeight); break;
	    //case UIHash("hanging-punctuation"): setEmbedStyle(name, PRIVATE()->StyleCSS.HangingPunctuation); break;
	    case UIHash("height"): setEmbedStyle(name, PRIVATE()->StyleCSS.Height); break;
	    //case UIHash("hyphens"): setEmbedStyle(name, PRIVATE()->StyleCSS.Hyphens); break;
	    case UIHash("inset-block-end"): setEmbedStyle(name, PRIVATE()->StyleCSS.InsetBlockEnd); break;
	    case UIHash("inset-block-start"): setEmbedStyle(name, PRIVATE()->StyleCSS.InsetBlockStart); break;
	    case UIHash("inset-inline-end"): setEmbedStyle(name, PRIVATE()->StyleCSS.InsetInlineEnd); break;
	    case UIHash("inset-inline-start"): setEmbedStyle(name, PRIVATE()->StyleCSS.InsetInlineStart); break;
	    case UIHash("justify-content"): setEmbedStyle(name, PRIVATE()->StyleCSS.JustifyContent); break;
	    case UIHash("left"): setEmbedStyle(name, PRIVATE()->StyleCSS.Left); break;
	    case UIHash("letter-spacing"): setEmbedStyle(name, PRIVATE()->StyleCSS.LetterSpacing); break;
	    //case UIHash("line-break"): setEmbedStyle(name, PRIVATE()->StyleCSS.LineBreak); break;
	    case UIHash("line-height"): setEmbedStyle(name, PRIVATE()->StyleCSS.LineHeight); break;
	    case UIHash("margin"): setEmbedStyle(name, PRIVATE()->StyleCSS.Margin); break;
	    case UIHash("margin-bottom"): setEmbedStyle(name, PRIVATE()->StyleCSS.MarginBottom); break;
	    case UIHash("margin-left"): setEmbedStyle(name, PRIVATE()->StyleCSS.MarginLeft); break;
	    case UIHash("margin-right"): setEmbedStyle(name, PRIVATE()->StyleCSS.MarginRight); break;
	    case UIHash("margin-top"): setEmbedStyle(name, PRIVATE()->StyleCSS.MarginTop); break;
	    case UIHash("max-height"): setEmbedStyle(name, PRIVATE()->StyleCSS.MaxHeight); break;
	    case UIHash("max-width"): setEmbedStyle(name, PRIVATE()->StyleCSS.MaxWidth); break;
	    case UIHash("min-height"): setEmbedStyle(name, PRIVATE()->StyleCSS.MinHeight); break;
	    case UIHash("min-width"): setEmbedStyle(name, PRIVATE()->StyleCSS.MinWidth); break;
	    //case UIHash("opacity"): setEmbedStyle(name, PRIVATE()->StyleCSS.Opacity); break;
	    //case UIHash("order"): setEmbedStyle(name, PRIVATE()->StyleCSS.Order); break;
	    //case UIHash("overflow-block"): setEmbedStyle(name, PRIVATE()->StyleCSS.OverflowBlock); break;
	    //case UIHash("overflow-inline"): setEmbedStyle(name, PRIVATE()->StyleCSS.OverflowInline); break;
	    //case UIHash("overflow-wrap"): setEmbedStyle(name, PRIVATE()->StyleCSS.OverflowWrap); break;
	    case UIHash("overflow-x"): setEmbedStyle(name, PRIVATE()->StyleCSS.OverflowX); break;
	    //case UIHash("overflow-y"): setEmbedStyle(name, PRIVATE()->StyleCSS.OverflowY); break;
	    case UIHash("padding"): setEmbedStyle(name, PRIVATE()->StyleCSS.Padding); break;
	    case UIHash("padding-bottom"): setEmbedStyle(name, PRIVATE()->StyleCSS.PaddingBottom); break;
	    case UIHash("padding-left"): setEmbedStyle(name, PRIVATE()->StyleCSS.PaddingLeft); break;
	    case UIHash("padding-right"): setEmbedStyle(name, PRIVATE()->StyleCSS.PaddingRight); break;
	    case UIHash("padding-top"): setEmbedStyle(name, PRIVATE()->StyleCSS.PaddingTop); break;
	    case UIHash("position"): setEmbedStyle(name, PRIVATE()->StyleCSS.Position); break;
	    case UIHash("right"): setEmbedStyle(name, PRIVATE()->StyleCSS.Right); break;
	    case UIHash("tab-size"): setEmbedStyle(name, PRIVATE()->StyleCSS.TabSize); break;
	    //case UIHash("text-align"): setEmbedStyle(name, PRIVATE()->StyleCSS.TextAlign); break;
	    //case UIHash("text-align-all"): setEmbedStyle(name, PRIVATE()->StyleCSS.TextAlignAll); break;
	    //case UIHash("text-align-last"): setEmbedStyle(name, PRIVATE()->StyleCSS.TextAlignLast); break;
	    //case UIHash("text-combine-upright"): setEmbedStyle(name, PRIVATE()->StyleCSS.TextCombineUpright); break;
	    //case UIHash("text-decoration-line"): setEmbedStyle(name, PRIVATE()->StyleCSS.TextDecorationLine); break;
	    //case UIHash("text-decoration-style"): setEmbedStyle(name, PRIVATE()->StyleCSS.TextDecorationStyle); break;
	    case UIHash("text-indent"): setEmbedStyle(name, PRIVATE()->StyleCSS.TextIndent); break;
	    //case UIHash("text-justify"): setEmbedStyle(name, PRIVATE()->StyleCSS.TextJustify); break;
	    //case UIHash("text-orientation"): setEmbedStyle(name, PRIVATE()->StyleCSS.TextOrientation); break;
	    //case UIHash("text-overflow"): setEmbedStyle(name, PRIVATE()->StyleCSS.TextOverflow); break;
	    //case UIHash("text-transform"): setEmbedStyle(name, PRIVATE()->StyleCSS.TextTransform); break;
	    case UIHash("top"): setEmbedStyle(name, PRIVATE()->StyleCSS.Top); break;
	    //case UIHash("unicode-bidi"): setEmbedStyle(name, PRIVATE()->StyleCSS.UnicodeBidi); break;
	    //case UIHash("vertical-align"): setEmbedStyle(name, PRIVATE()->StyleCSS.VerticalAlign); break;
	    case UIHash("visibility"): setEmbedStyle(name, PRIVATE()->StyleCSS.Visibility); break;
	    //case UIHash("white-space"): setEmbedStyle(name, PRIVATE()->StyleCSS.WhiteSpace); break;
	    case UIHash("width"): setEmbedStyle(name, PRIVATE()->StyleCSS.Width); break;
	    //case UIHash("word-break"): setEmbedStyle(name, PRIVATE()->StyleCSS.WordBreak); break;
	    //case UIHash("word-spacing"): setEmbedStyle(name, PRIVATE()->StyleCSS.WordSpacing); break;
	    //case UIHash("word-wrap"): setEmbedStyle(name, PRIVATE()->StyleCSS.WordWrap); break;
	    //case UIHash("wrap-flow"): setEmbedStyle(name, PRIVATE()->StyleCSS.WrapFlow); break;
	    //case UIHash("wrap-through"): setEmbedStyle(name, PRIVATE()->StyleCSS.WrapThrough); break;
	    case UIHash("writing-mode"): setEmbedStyle(name, PRIVATE()->StyleCSS.WritingMode); break;
	    case UIHash("z-index"): setEmbedStyle(name, PRIVATE()->StyleCSS.ZIndex); break;
		}
		getStyles()->setStyleText(name, value);
	}
	return true;
}

UIAttribRaw UIWidget::getAttribs() const
{
	return PRIVATE()->Attribs.get();
}

void UIWidget::setAttribs(UIAttribRef value)
{
	PRIVATE()->Attribs = value;
}

UIString UIWidget::getAttribText(UIString name) const
{
	return getAttribs()->getAttribText(name);
}

void UIWidget::setAttribText(UIString name, UIString value)
{
	getAttribs()->setAttribText(name, value);
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

UICanvasRaw UIWidget::getCanvas() const
{
	return PRIVATE()->Canvas;
}

void UIWidget::setContext(UICanvasRaw value)
{
	if (getCanvas()) getCanvas()->setAnimate(this, false);
	PRIVATE()->Canvas = value;
	if (getCanvas()) getCanvas()->setAnimate(this, getAnimate());
	for (size_t i = 0; i < getWidgets().size(); ++i) getWidgets()[i]->setContext(value);
}

void UIWidget::setParent(UIWidgetRaw value)
{
	if (value == this) return;
	PRIVATE()->Parent = value;
}