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

struct UIWidgetPrivate : UIPrivate
{
public:
	UIString Identity;
	UIWidgetRaw Parent = nullptr;
	UIList<UIWidgetRef> Children;
	UIFilterRaw Filter = nullptr;
	UICanvasRaw Canvas = nullptr;
	UIImage Target = {};

	// =============================Flex Layout==========================

	float Scale = 1.0f;
	float Rotate = 0.0f;
	UIFloat2 Translate;
	UIRect LocalRect, ClientRect, ViewRect;
	bool Enable = true, Visible = true, Animate = false;
	UIPropBorder Border;
	UIPropMargin Margin;
	UIPropPadding Padding;

	// =============================DOM Attrib===========================

	UIAttribRef Attribs;

	// ==============================CSS Style===========================

	UIStyleRef Styles;
	UIComputedStyleRef ComputedStyles;
};
#define PRIVATE() ((UIWidgetPrivate*) m_Private)

UIWidget::UIWidget(UICanvasRaw canvas)
{
	m_Private = new UIWidgetPrivate;
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

UIPropDisplay UIWidget::getDisplayType() const
{
	return getStyle<UIPropDisplay>("display");
}

void UIWidget::setDisplayType(UIPropDisplay value)
{
	setStyle("display", value);
}

UIPropPosition UIWidget::getPositionType() const
{
	return getStyle<UIPropPosition>("position");
}

void UIWidget::setPositionType(UIPropPosition value)
{
	setStyle("position", value);
}

UIPropLeft UIWidget::getFixedPosX() const
{
	return getStyle<UIPropLeft>("left");
}

void UIWidget::setFixedPosX(UIPropLeft value)
{
	setStyle("left", value);
}

void UIWidget::setFixedPosX(float value)
{
	setFixedPosX({value, UI_CSS_LEFT_LENGTH});
}

UIPropTop UIWidget::getFixedPosY() const
{
	return getStyle<UIPropTop>("top");
}

void UIWidget::setFixedPosY(UIPropTop value)
{
	setStyle("top", value);
}

void UIWidget::setFixedPosY(float value)
{
	setFixedPosY({value, UI_CSS_TOP_LENGTH});
}

void UIWidget::setFixedPos(UIPropLeft left, UIPropTop top)
{
	setFixedPosX(left);
	setFixedPosY(top);
}

void UIWidget::setFixedPos(float left, float top)
{
	setFixedPos({left, UI_CSS_LEFT_LENGTH}, {top, UI_CSS_TOP_LENGTH});
}

UIPropMinWidth UIWidget::getMinWidth() const
{
	return getStyle<UIPropMinWidth>("min-width");
}

void UIWidget::setMinWidth(UIPropMinWidth value)
{
	setStyle("min-width", value);
}

void UIWidget::setMinWidth(float value)
{
	setMinWidth({value, UI_CSS_MIN_WIDTH_LENGTH});
}

UIPropMaxWidth UIWidget::getMaxWidth() const
{
	return getStyle<UIPropMaxWidth>("max-width");
}

void UIWidget::setMaxWidth(UIPropMaxWidth value)
{
	setStyle("max-width", value);
}

void UIWidget::setMaxWidth(float value)
{
	setMaxWidth({value, UI_CSS_MAX_WIDTH_LENGTH});
}

UIPropWidth UIWidget::getFixedWidth() const
{
	return getStyle<UIPropWidth>("width");
}

void UIWidget::setFixedWidth(UIPropWidth value)
{
	setStyle("width", value);
}

void UIWidget::setFixedWidth(float value)
{
	setFixedWidth({value, UI_CSS_WIDTH_LENGTH});
}

UIPropMinHeight UIWidget::getMinHeight() const
{
	return getStyle<UIPropMinHeight>("min-height");
}

void UIWidget::setMinHeight(UIPropMinHeight value)
{
	setStyle("min-height", value);
}

void UIWidget::setMinHeight(float value)
{
	setMinHeight({value, UI_CSS_MIN_HEIGHT_LENGTH});
}

UIPropMaxHeight UIWidget::getMaxHeight() const
{
	return getStyle<UIPropMaxHeight>("max-height");
}

void UIWidget::setMaxHeight(UIPropMaxHeight value)
{
	setStyle("max-height", value);
}

void UIWidget::setMaxHeight(float value)
{
	setMaxHeight({value, UI_CSS_MAX_HEIGHT_LENGTH});
}

UIPropHeight UIWidget::getFixedHeight() const
{
	return getStyle<UIPropHeight>("height");
}

void UIWidget::setFixedHeight(UIPropHeight value)
{
	setStyle("height", value);
}

void UIWidget::setFixedHeight(float value)
{
	setFixedHeight({value, UI_CSS_HEIGHT_LENGTH});
}

void UIWidget::setMinSize(UIPropMinWidth width, UIPropMinHeight height)
{
	setMinWidth(width);
	setMinHeight(height);
}

void UIWidget::setMinSize(float width, float height)
{
	setMinSize({width, UI_CSS_MIN_WIDTH_LENGTH}, {height, UI_CSS_MIN_HEIGHT_LENGTH});
}

void UIWidget::setMaxSize(UIPropMaxWidth width, UIPropMaxHeight height)
{
	setMaxWidth(width);
	setMaxHeight(height);
}

void UIWidget::setMaxSize(float width, float height)
{
	setMaxSize({width, UI_CSS_MAX_WIDTH_LENGTH}, {height, UI_CSS_MAX_HEIGHT_LENGTH});
}

void UIWidget::setFixedSize(UIPropWidth width, UIPropHeight height)
{
	setFixedWidth(width);
	setFixedHeight(height);
}

void UIWidget::setFixedSize(float width, float height)
{
	setFixedSize({width, UI_CSS_WIDTH_LENGTH}, {height, UI_CSS_HEIGHT_LENGTH});
}

void UIWidget::setBorder(UIPropBorderLeft left, UIPropBorderTop top, UIPropBorderRight right, UIPropBorderBottom bottom)
{
	setBorderLeft(left);
	setBorderTop(top);
	setBorderRight(right);
	setBorderBottom(bottom);
}

UIPropBorderTop UIWidget::getBorderTop() const
{
	return getStyle<UIPropBorderTop>("border-top");
}

void UIWidget::setBorderTop(UIPropBorderTop value)
{
	setStyle("border-top", value);
}

UIPropBorderBottom UIWidget::getBorderBottom() const
{
	return getStyle<UIPropBorderBottom>("border-bottom");
}

void UIWidget::setBorderBottom(UIPropBorderBottom value)
{
	setStyle("border-bottom", value);
}

UIPropBorderLeft UIWidget::getBorderLeft() const
{
	return getStyle<UIPropBorderLeft>("border-left");
}

void UIWidget::setBorderLeft(UIPropBorderLeft value)
{
	setStyle("border-left", value);
}

UIPropBorderRight UIWidget::getBorderRight() const
{
	return getStyle<UIPropBorderRight>("border-right");
}

void UIWidget::setBorderRight(UIPropBorderRight value)
{
	setStyle("border-right", value);
}

void UIWidget::setMargin(UIPropMarginLeft left, UIPropMarginTop top, UIPropMarginRight right, UIPropMarginBottom bottom)
{
	setMarginLeft(left);
	setMarginTop(top);
	setMarginRight(right);
	setMarginBottom(bottom);
}

UIPropMarginTop UIWidget::getMarginTop() const
{
	return getStyle<UIPropMarginTop>("margin-top");
}

void UIWidget::setMarginTop(UIPropMarginTop value)
{
	setStyle("margin-top", value);
}

UIPropMarginBottom UIWidget::getMarginBottom() const
{
	return getStyle<UIPropMarginBottom>("margin-bottom");
}

void UIWidget::setMarginBottom(UIPropMarginBottom value)
{
	setStyle("margin-bottom", value);
}

UIPropMarginLeft UIWidget::getMarginLeft() const
{
	return getStyle<UIPropMarginLeft>("margin-left");
}

void UIWidget::setMarginLeft(UIPropMarginLeft value)
{
	setStyle("margin-left", value);
}

UIPropMarginRight UIWidget::getMarginRight() const
{
	return getStyle<UIPropMarginRight>("margin-right");
}

void UIWidget::setMarginRight(UIPropMarginRight value)
{
	setStyle("margin-right", value);
}

void UIWidget::setPadding(UIPropPaddingLeft left, UIPropPaddingTop top, UIPropPaddingRight right, UIPropPaddingBottom bottom)
{
	setPaddingLeft(left);
	setPaddingTop(top);
	setPaddingRight(right);
	setPaddingBottom(bottom);
}

UIPropPaddingTop UIWidget::getPaddingTop() const
{
	return getStyle<UIPropPaddingTop>("padding-top");
}

void UIWidget::setPaddingTop(UIPropPaddingTop value)
{
	setStyle("padding-top", value);
}

UIPropPaddingBottom UIWidget::getPaddingBottom() const
{
	return getStyle<UIPropPaddingBottom>("padding-bottom");
}

void UIWidget::setPaddingBottom(UIPropPaddingBottom value)
{
	setStyle("padding-bottom", value);
}

UIPropPaddingLeft UIWidget::getPaddingLeft() const
{
	return getStyle<UIPropPaddingLeft>("padding-left");
}

void UIWidget::setPaddingLeft(UIPropPaddingLeft value)
{
	setStyle("padding-left", value);
}

UIPropPaddingRight UIWidget::getPaddingRight() const
{
	return getStyle<UIPropPaddingRight>("padding-right");
}

void UIWidget::setPaddingRight(UIPropPaddingRight value)
{
	setStyle("padding-right", value);
}

UIPropFlexDirection UIWidget::getFlexDirection() const
{
	return getStyle<UIPropFlexDirection>("flex-direction");
}

void UIWidget::setFlexDirection(UIPropFlexDirection value)
{
	setStyle("flex-direction", value);
}

UIPropFlexWrap UIWidget::getFlexWrap() const
{
	return getStyle<UIPropFlexWrap>("flex-wrap");
}

void UIWidget::setFlexWrap(UIPropFlexWrap value)
{
	setStyle("flex-wrap", value);
}

UIPropJustifyContent UIWidget::getJustifyContent() const
{
	return getStyle<UIPropJustifyContent>("justify-content");
}

void UIWidget::setJustifyContent(UIPropJustifyContent value)
{
	setStyle("justify-content", value);
}

UIPropAlignItems UIWidget::getAlignItems() const
{
	return getStyle<UIPropAlignItems>("align-items");
}

void UIWidget::setAlignItems(UIPropAlignItems value)
{
	setStyle("align-items", value);
}

UIPropAlignContent UIWidget::getAlignContent() const
{
	return getStyle<UIPropAlignContent>("align-content");
}

void UIWidget::setAlignContent(UIPropAlignContent value)
{
	setStyle("align-content", value);
}

void UIWidget::setFlexFlow(UIPropFlexDirection direction, UIPropFlexWrap wrap)
{
	setFlexDirection(direction);
	setFlexWrap(wrap);
}

UIPropFlexGrow UIWidget::getFlexGrow() const
{
	return getStyle<UIPropFlexGrow>("flex-grow");
}

void UIWidget::setFlexGrow(UIPropFlexGrow value)
{
	setStyle("flex-grow", value);
}

UIPropFlexShrink UIWidget::getFlexShrink() const
{
	return getStyle<UIPropFlexShrink>("flex-shrink");
}

void UIWidget::setFlexShrink(UIPropFlexShrink value)
{
	setStyle("flex-shrink", value);
}

UIPropFlexBasis UIWidget::getFlexBasis() const
{
	return getStyle<UIPropFlexBasis>("flex-basis");
}

void UIWidget::setFlexBasis(UIPropFlexBasis value)
{
	setStyle("flex-basis", value);
}

UIPropAlignSelf UIWidget::getAlignSelf() const
{
	return getStyle<UIPropAlignSelf>("align-self");
}

void UIWidget::setAlignSelf(UIPropAlignSelf value)
{
	setStyle("align-self", value);
}

void UIWidget::setFlex(UIPropFlexGrow grow, UIPropFlexShrink shrink, UIPropFlexBasis basis)
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
		default: setStyle(name, value); break;
		case UIHash("align-content"): setStyle<UIPropAlignContent>(name); break;
		case UIHash("align-items"): setStyle<UIPropAlignItems>(name); break;
		case UIHash("align-self"): setStyle<UIPropAlignSelf>(name); break;
		case UIHash("alignment-baseline"): setStyle<UIPropAlignmentBaseline>(name); break;
		case UIHash("baseline-shift"): setStyle<UIPropBaselineShift>(name); break;
		case UIHash("baseline-source"): setStyle<UIPropBaselineSource>(name); break;
		case UIHash("border"): setEmbedStyle(name, PRIVATE()->Border); break;
		case UIHash("border-bottom"): setEmbedStyle("border", PRIVATE()->Border); setEmbedStyle(name, PRIVATE()->Border.Bottom); break;
		case UIHash("border-left"): setEmbedStyle("border", PRIVATE()->Border); setEmbedStyle(name, PRIVATE()->Border.Left); break;
		case UIHash("border-right"): setEmbedStyle("border", PRIVATE()->Border); setEmbedStyle(name, PRIVATE()->Border.Right); break;
		case UIHash("border-top"): setEmbedStyle("border", PRIVATE()->Border); setEmbedStyle(name, PRIVATE()->Border.Top); break;
		case UIHash("bottom"): setStyle<UIPropBottom>(name); break;
		case UIHash("box-sizing"): setStyle<UIPropBoxSizing>(name); break;
		case UIHash("clear"): setStyle<UIPropClear>(name); break;
		//case UIHash("color"): setStyle<UIPropColor>(name); break;
		case UIHash("direction"): setStyle<UIPropDirection>(name); break;
		case UIHash("display"): setStyle<UIPropDisplay>(name); break;
		//case UIHash("dominant-baseline"): setStyle<UIPropDominantBaseline>(name); break;
		//case UIHash("flex"): setStyle<UIPropFlex>(name); break;
		case UIHash("flex-basis"): setStyle<UIPropFlexBasis>(name); break;
		//case UIHash("flex-direction"): setStyle<UIPropFlexDirection>(name); break;
		case UIHash("flex-grow"): setStyle<UIPropFlexGrow>(name); break;
		case UIHash("flex-shrink"): setStyle<UIPropFlexShrink>(name); break;
		//case UIHash("flex-wrap"): setStyle<UIPropFlexWrap>(name); break;
		//case UIHash("float"): setStyle<UIPropFloat>(name); break;
		//case UIHash("float-defer"): setStyle<UIPropFloatDefer>(name); break;
		case UIHash("float-offset"): setStyle<UIPropFloatOffset>(name); break;
		//case UIHash("float-reference"): setStyle<UIPropFloatReference>(name); break;
		//case UIHash("font-family"): setStyle<UIPropFontFamily>(name); break;
		case UIHash("font-size"): setStyle<UIPropFontSize>(name); break;
		case UIHash("font-stretch"): setStyle<UIPropFontStretch>(name); break;
		//case UIHash("font-style"): setStyle<UIPropFontStyle>(name); break;
		case UIHash("font-weight"): setStyle<UIPropFontWeight>(name); break;
		//case UIHash("hanging-punctuation"): setStyle<UIPropHangingPunctuation>(name); break;
		case UIHash("height"): setStyle<UIPropHeight>(name); break;
		//case UIHash("hyphens"): setStyle<UIPropHyphens>(name); break;
		case UIHash("inset-block-end"): setStyle<UIPropInsetBlockEnd>(name); break;
		case UIHash("inset-block-start"): setStyle<UIPropInsetBlockStart>(name); break;
		case UIHash("inset-inline-end"): setStyle<UIPropInsetInlineEnd>(name); break;
		case UIHash("inset-inline-start"): setStyle<UIPropInsetInlineStart>(name); break;
		case UIHash("justify-content"): setStyle<UIPropJustifyContent>(name); break;
		case UIHash("left"): setStyle<UIPropLeft>(name); break;
		case UIHash("letter-spacing"): setStyle<UIPropLetterSpacing>(name); break;
		//case UIHash("line-break"): setStyle<UIPropLineBreak>(name); break;
		case UIHash("line-height"): setStyle<UIPropLineHeight>(name); break;
		case UIHash("margin"): setEmbedStyle(name, PRIVATE()->Margin); break;
		case UIHash("margin-bottom"): setEmbedStyle("margin", PRIVATE()->Margin); setEmbedStyle(name, PRIVATE()->Margin.Bottom); break;
		case UIHash("margin-left"): setEmbedStyle("margin", PRIVATE()->Margin); setEmbedStyle(name, PRIVATE()->Margin.Left); break;
		case UIHash("margin-right"): setEmbedStyle("margin", PRIVATE()->Margin); setEmbedStyle(name, PRIVATE()->Margin.Right); break;
		case UIHash("margin-top"): setEmbedStyle("margin", PRIVATE()->Margin); setEmbedStyle(name, PRIVATE()->Margin.Top); break;
		case UIHash("max-height"): setStyle<UIPropMaxHeight>(name); break;
		case UIHash("max-width"): setStyle<UIPropMaxWidth>(name); break;
		case UIHash("min-height"): setStyle<UIPropMinHeight>(name); break;
		case UIHash("min-width"): setStyle<UIPropMinWidth>(name); break;
		//case UIHash("opacity"): setStyle<UIPropOpacity>(name); break;
		//case UIHash("order"): setStyle<UIPropOrder>(name); break;
		//case UIHash("overflow-block"): setStyle<UIPropOverflowBlock>(name); break;
		//case UIHash("overflow-inline"): setStyle<UIPropOverflowInline>(name); break;
		//case UIHash("overflow-wrap"): setStyle<UIPropOverflowWrap>(name); break;
		case UIHash("overflow-x"): setStyle<UIPropOverflowX>(name); break;
		//case UIHash("overflow-y"): setStyle<UIPropOverflowY>(name); break;
		case UIHash("padding"): setEmbedStyle(name, PRIVATE()->Padding); break;
		case UIHash("padding-bottom"): setEmbedStyle("padding", PRIVATE()->Padding); setEmbedStyle(name, PRIVATE()->Padding.Bottom); break;
		case UIHash("padding-left"): setEmbedStyle("padding", PRIVATE()->Padding); setEmbedStyle(name, PRIVATE()->Padding.Left); break;
		case UIHash("padding-right"): setEmbedStyle("padding", PRIVATE()->Padding); setEmbedStyle(name, PRIVATE()->Padding.Right); break;
		case UIHash("padding-top"): setEmbedStyle("padding", PRIVATE()->Padding); setEmbedStyle(name, PRIVATE()->Padding.Top); break;
		case UIHash("position"): setStyle<UIPropPosition>(name); break;
		case UIHash("right"): setStyle<UIPropRight>(name); break;
		case UIHash("tab-size"): setStyle<UIPropTabSize>(name); break;
		//case UIHash("text-align"): setStyle<UIPropTextAlign>(name); break;
		//case UIHash("text-align-all"): setStyle<UIPropTextAlignAll>(name); break;
		//case UIHash("text-align-last"): setStyle<UIPropTextAlignLast>(name); break;
		//case UIHash("text-combine-upright"): setStyle<UIPropTextCombineUpright>(name); break;
		//case UIHash("text-decoration-line"): setStyle<UIPropTextDecorationLine>(name); break;
		//case UIHash("text-decoration-style"): setStyle<UIPropTextDecorationStyle>(name); break;
		case UIHash("text-indent"): setStyle<UIPropTextIndent>(name); break;
		//case UIHash("text-justify"): setStyle<UIPropTextJustify>(name); break;
		//case UIHash("text-orientation"): setStyle<UIPropTextOrientation>(name); break;
		//case UIHash("text-overflow"): setStyle<UIPropTextOverflow>(name); break;
		//case UIHash("text-transform"): setStyle<UIPropTextTransform>(name); break;
		case UIHash("top"): setStyle<UIPropTop>(name); break;
		//case UIHash("unicode-bidi"): setStyle<UIPropUnicodeBidi>(name); break;
		//case UIHash("vertical-align"): setStyle<UIPropVerticalAlign>(name); break;
		case UIHash("visibility"): setStyle<UIPropVisibility>(name); break;
		//case UIHash("white-space"): setStyle<UIPropWhiteSpace>(name); break;
		case UIHash("width"): setStyle<UIPropWidth>(name); break;
		//case UIHash("word-break"): setStyle<UIPropWordBreak>(name); break;
		//case UIHash("word-spacing"): setStyle<UIPropWordSpacing>(name); break;
		//case UIHash("word-wrap"): setStyle<UIPropWordWrap>(name); break;
		//case UIHash("wrap-flow"): setStyle<UIPropWrapFlow>(name); break;
		//case UIHash("wrap-through"): setStyle<UIPropWrapThrough>(name); break;
		case UIHash("writing-mode"): setStyle<UIPropWritingMode>(name); break;
		case UIHash("z-index"): setStyle<UIPropZIndex>(name); break;
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

UIImageRaw UIWidget::getTarget() const
{
	return &PRIVATE()->Target;
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