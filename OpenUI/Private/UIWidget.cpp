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
    UIPropDisplay Display;
    UIPropDirection Direction;
    UIPropBoxSizing BoxSizing;
    UIPropPosition Position;
    UIPropLeft Left;
    UIPropTop Top;
    UIPropRight Right;
    UIPropBottom Bottom;
    UIPropWidth Width;
    UIPropHeight Height;
    UIPropMinWidth MinWidth;
    UIPropMinHeight MinHeight;
    UIPropMaxWidth MaxWidth;
    UIPropMaxHeight MaxHeight;
    UIPropBorder Border;
    UIPropColor BorderColor;
    UIPropBorderRadius BorderRadius;
    UIPropBackground Background;
    UIPropForeground Foreground;
    UIPropMargin Margin;
    UIPropPadding Padding;
    UIPropFlexDirection FlexDirection;
    UIPropFlexWrap FlexWrap;
    UIPropJustifyContent JustifyContent;
    UIPropAlignItems AlignItems;
    UIPropAlignContent AlignContent;
    UIPropAlignSelf AlignSelf;
    UIPropJustifyItems JustifyItems;
    UIPropJustifySelf JustifySelf;
    UIPropFlex Flex;
    UIPropAspectRatio AspectRatio;
    UIPropOverflow Overflow;
    UIPropGridTemplateColumns GridTemplateColumns;
    UIPropGridTemplateRows GridTemplateRows;
    UIPropGridAutoColumns GridAutoColumns;
    UIPropGridAutoRows GridAutoRows;
    UIPropGridColumn GridColumn;
    UIPropGridRow GridRow;
    UIPropGap Gap;

    // =============================Grid Layout==========================

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
    PRIVATE()->Attribs = UINew<UIAttrib>();
    PRIVATE()->ComputedStyles = UINew<UIComputedStyle>(PRIVATE()->Styles.get());
}

UIWidget::~UIWidget()
{
    if (getCanvas()) getCanvas()->setAnimate(this, false);
    delete m_Private;
    m_Private = nullptr;
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
    if (result == PRIVATE()->Children.end())
        PRIVATE()->Children.push_back(value);
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
    return findWidget([=](UIWidgetRef element)-> bool { return element->getID() == identity; });
}

UIList<UIWidgetRef> UIWidget::findWidgets(UIString identity) const
{
    return findWidgets([=](UIWidgetRef element)-> bool { return element->getID() == identity; });
}

UIWidgetRef UIWidget::findWidget(UILambda<bool(UIWidgetRef)> selector) const
{
    UIWidgetRef result;
    UILambda<bool(UIWidgetRef)> foreach_func;
    foreach_func = [&](UIWidgetRef element)-> bool
    {
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
    foreach_func = [&](UIWidgetRef element)
    {
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

    auto color = PRIVATE()->Background.Color;
    if (color.Value.A == 0) return;
    UIBrush brush{ .Color { color.Value.R / 255.0f, color.Value.G / 255.0f, color.Value.B / 255.0f, color.Value.A / 255.0f } };
    painter->setPen(UINoPen);
    painter->setBrush(brush);
    auto radius = PRIVATE()->BorderRadius.TopLeft;
    auto rx = radius.HorizontalType == UI_CSS_BORDER_RADIUS_PERCENTAGE ? client.W * radius.Horizontal / 100.0f : radius.Horizontal;
    auto ry = radius.VerticalType == UI_CSS_BORDER_RADIUS_PERCENTAGE ? client.H * radius.Vertical / 100.0f : radius.Vertical;
    if (rx < 0.5f || ry < 0.5f) painter->drawRect(client.X, client.Y, client.W, client.H);
    else painter->drawRoundedRect(client.X, client.Y, client.W, client.H, rx, ry);
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
        }
        break;
    case UIHash("Close"):
        {
            closeEvent(UICast<UICloseEvent>(event));
        }
        break;
    case UIHash("Hide"):
        {
            hideEvent(UICast<UIHideEvent>(event));
        }
        break;
    case UIHash("Move"):
        {
            moveEvent(UICast<UIMoveEvent>(event));
        }
        break;
    case UIHash("Resize"):
        {
            resizeEvent(UICast<UIResizeEvent>(event));
        }
        break;
    case UIHash("MouseDown"):
        {
            mousePressEvent(UICast<UIMouseDownEvent>(event));
        }
        break;
    case UIHash("MouseUp"):
        {
            mouseReleaseEvent(UICast<UIMouseUpEvent>(event));
        }
        break;
    case UIHash("DoubleClick"):
        {
            mouseDoubleEvent(UICast<UIMouseDblClickEvent>(event));
        }
        break;
    case UIHash("MouseMove"):
        {
            mouseMoveEvent(UICast<UIMouseMoveEvent>(event));
        }
        break;
    case UIHash("MouseWheel"):
        {
            wheelEvent(UICast<UIMouseWheelEvent>(event));
        }
        break;
    case UIHash("MouseEnter"):
        {
            enterEvent(UICast<UIMouseEnterEvent>(event));
        }
        break;
    case UIHash("MouseLeave"):
        {
            leaveEvent(UICast<UIMouseLeaveEvent>(event));
        }
        break;
    case UIHash("KeyDown"):
        {
            keyPressEvent(UICast<UIKeyDownEvent>(event));
        }
        break;
    case UIHash("KeyUp"):
        {
            keyReleaseEvent(UICast<UIKeyUpEvent>(event));
        }
        break;
    case UIHash("TextInput"):
        {
            inputEvent(UICast<UITextInputEvent>(event));
        }
        break;
    case UIHash("Drop"):
        {
            dropEvent(UICast<UIDropEvent>(event));
        }
        break;
    case UIHash("DragMove"):
        {
            dragMoveEvent(UICast<UIDragMoveEvent>(event));
        }
        break;
    case UIHash("DragEnter"):
        {
            dragEnterEvent(UICast<UIDragEnterEvent>(event));
        }
        break;
    case UIHash("DragLeave"):
        {
            dragLeaveEvent(UICast<UIDragLeaveEvent>(event));
        }
        break;
    case UIHash("FocusIn"):
        {
            focusInEvent(UICast<UIFocusEvent>(event));
        }
        break;
    case UIHash("FocusOut"):
        {
            focusOutEvent(UICast<UIFocusEvent>(event));
        }
        break;
    case UIHash("Timer"):
        {
            timerEvent(UICast<UITimerEvent>(event));
        }
        break;
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
    return UIFloat2{getPosX(), getPosY()};
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
    return UIFloat2{getWidth(), getHeight()};
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
    return PRIVATE()->Display;
}

void UIWidget::setDisplayType(UIPropDisplay value)
{
    PRIVATE()->Display = value;
}

UIPropPosition UIWidget::getPositionType() const
{
    return PRIVATE()->Position;
}

void UIWidget::setPositionType(UIPropPosition value)
{
    PRIVATE()->Position = value;
}

UIPropLeft UIWidget::getFixedPosX() const
{
    return PRIVATE()->Left;
}

void UIWidget::setFixedPosX(UIPropLeft value)
{
    PRIVATE()->Left = value;
}

void UIWidget::setFixedPosX(float value)
{
    setFixedPosX({value, UI_CSS_LEFT_LENGTH});
}

UIPropTop UIWidget::getFixedPosY() const
{
    return PRIVATE()->Top;
}

void UIWidget::setFixedPosY(UIPropTop value)
{
    PRIVATE()->Top = value;
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
    return PRIVATE()->MinWidth;
}

void UIWidget::setMinWidth(UIPropMinWidth value)
{
    PRIVATE()->MinWidth = value;
}

void UIWidget::setMinWidth(float value)
{
    setMinWidth({value, UI_CSS_MIN_WIDTH_LENGTH});
}

UIPropMaxWidth UIWidget::getMaxWidth() const
{
    return PRIVATE()->MaxWidth;
}

void UIWidget::setMaxWidth(UIPropMaxWidth value)
{
    PRIVATE()->MaxWidth = value;
}

void UIWidget::setMaxWidth(float value)
{
    setMaxWidth({value, UI_CSS_MAX_WIDTH_LENGTH});
}

UIPropWidth UIWidget::getFixedWidth() const
{
    return PRIVATE()->Width;
}

void UIWidget::setFixedWidth(UIPropWidth value)
{
    PRIVATE()->Width = value;
}

void UIWidget::setFixedWidth(float value)
{
    setFixedWidth({value, UI_CSS_WIDTH_LENGTH});
}

UIPropMinHeight UIWidget::getMinHeight() const
{
    return PRIVATE()->MinHeight;
}

void UIWidget::setMinHeight(UIPropMinHeight value)
{
    PRIVATE()->MinHeight = value;
}

void UIWidget::setMinHeight(float value)
{
    setMinHeight({value, UI_CSS_MIN_HEIGHT_LENGTH});
}

UIPropMaxHeight UIWidget::getMaxHeight() const
{
    return PRIVATE()->MaxHeight;
}

void UIWidget::setMaxHeight(UIPropMaxHeight value)
{
    PRIVATE()->MaxHeight = value;
}

void UIWidget::setMaxHeight(float value)
{
    setMaxHeight({value, UI_CSS_MAX_HEIGHT_LENGTH});
}

UIPropHeight UIWidget::getFixedHeight() const
{
    return PRIVATE()->Height;
}

void UIWidget::setFixedHeight(UIPropHeight value)
{
    PRIVATE()->Height = value;
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
    return PRIVATE()->Border.Top;
}

void UIWidget::setBorderTop(UIPropBorderTop value)
{
    PRIVATE()->Border.Top = value;
}

UIPropBorderBottom UIWidget::getBorderBottom() const
{
    return PRIVATE()->Border.Bottom;
}

void UIWidget::setBorderBottom(UIPropBorderBottom value)
{
    PRIVATE()->Border.Bottom = value;
}

UIPropBorderLeft UIWidget::getBorderLeft() const
{
    return PRIVATE()->Border.Left;
}

void UIWidget::setBorderLeft(UIPropBorderLeft value)
{
    PRIVATE()->Border.Left = value;
}

UIPropBorderRight UIWidget::getBorderRight() const
{
    return PRIVATE()->Border.Right;
}

void UIWidget::setBorderRight(UIPropBorderRight value)
{
    PRIVATE()->Border.Right = value;
}

UIPropColor UIWidget::getBorderColor() const
{
    return PRIVATE()->BorderColor;
}

void UIWidget::setBorderColor(UIPropColor value)
{
    PRIVATE()->BorderColor = value;
}

UIPropBorderRadius UIWidget::getBorderRadius() const
{
    return PRIVATE()->BorderRadius;
}

void UIWidget::setBorderRadius(UIPropBorderRadius value)
{
    PRIVATE()->BorderRadius = value;
}

UIPropBorderTopLeftRadius UIWidget::getBorderTopLeftRadius() const
{
    return PRIVATE()->BorderRadius.TopLeft;
}

void UIWidget::setBorderTopLeftRadius(UIPropBorderTopLeftRadius value)
{
    PRIVATE()->BorderRadius.TopLeft = value;
}

UIPropBorderTopRightRadius UIWidget::getBorderTopRightRadius() const
{
    return PRIVATE()->BorderRadius.TopRight;
}

void UIWidget::setBorderTopRightRadius(UIPropBorderTopRightRadius value)
{
    PRIVATE()->BorderRadius.TopRight = value;
}

UIPropBorderBottomRightRadius UIWidget::getBorderBottomRightRadius() const
{
    return PRIVATE()->BorderRadius.BottomRight;
}

void UIWidget::setBorderBottomRightRadius(UIPropBorderBottomRightRadius value)
{
    PRIVATE()->BorderRadius.BottomRight = value;
}

UIPropBorderBottomLeftRadius UIWidget::getBorderBottomLeftRadius() const
{
    return PRIVATE()->BorderRadius.BottomLeft;
}

void UIWidget::setBorderBottomLeftRadius(UIPropBorderBottomLeftRadius value)
{
    PRIVATE()->BorderRadius.BottomLeft = value;
}

UIPropBackground UIWidget::getBackground() const
{
    return PRIVATE()->Background;
}

void UIWidget::setBackground(UIPropBackground value)
{
    PRIVATE()->Background = value;
}

UIPropColor UIWidget::getBackgroundColor() const
{
    return PRIVATE()->Background.Color;
}

void UIWidget::setBackgroundColor(UIPropColor value)
{
    PRIVATE()->Background.Color = value;
}

UIPropForeground UIWidget::getForeground() const
{
    return PRIVATE()->Foreground;
}

void UIWidget::setForeground(UIPropForeground value)
{
    PRIVATE()->Foreground = value;
}

UIPropColor UIWidget::getForegroundColor() const
{
    return PRIVATE()->Foreground.Color;
}

void UIWidget::setForegroundColor(UIPropColor value)
{
    PRIVATE()->Foreground.Color = value;
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
    return PRIVATE()->Margin.Top;
}

void UIWidget::setMarginTop(UIPropMarginTop value)
{
    PRIVATE()->Margin.Top = value;
}

UIPropMarginBottom UIWidget::getMarginBottom() const
{
    return PRIVATE()->Margin.Bottom;
}

void UIWidget::setMarginBottom(UIPropMarginBottom value)
{
    PRIVATE()->Margin.Bottom = value;
}

UIPropMarginLeft UIWidget::getMarginLeft() const
{
    return PRIVATE()->Margin.Left;
}

void UIWidget::setMarginLeft(UIPropMarginLeft value)
{
    PRIVATE()->Margin.Left = value;
}

UIPropMarginRight UIWidget::getMarginRight() const
{
    return PRIVATE()->Margin.Right;
}

void UIWidget::setMarginRight(UIPropMarginRight value)
{
    PRIVATE()->Margin.Right = value;
}

void UIWidget::setPadding(UIPropPaddingLeft left, UIPropPaddingTop top, UIPropPaddingRight right,
                          UIPropPaddingBottom bottom)
{
    setPaddingLeft(left);
    setPaddingTop(top);
    setPaddingRight(right);
    setPaddingBottom(bottom);
}

UIPropPaddingTop UIWidget::getPaddingTop() const
{
    return PRIVATE()->Padding.Top;
}

void UIWidget::setPaddingTop(UIPropPaddingTop value)
{
    PRIVATE()->Padding.Top = value;
}

UIPropPaddingBottom UIWidget::getPaddingBottom() const
{
    return PRIVATE()->Padding.Bottom;
}

void UIWidget::setPaddingBottom(UIPropPaddingBottom value)
{
    PRIVATE()->Padding.Bottom = value;
}

UIPropPaddingLeft UIWidget::getPaddingLeft() const
{
    return PRIVATE()->Padding.Left;
}

void UIWidget::setPaddingLeft(UIPropPaddingLeft value)
{
    PRIVATE()->Padding.Left = value;
}

UIPropPaddingRight UIWidget::getPaddingRight() const
{
    return PRIVATE()->Padding.Right;
}

void UIWidget::setPaddingRight(UIPropPaddingRight value)
{
    PRIVATE()->Padding.Right = value;
}

UIPropFlexDirection UIWidget::getFlexDirection() const
{
    return PRIVATE()->FlexDirection;
}

void UIWidget::setFlexDirection(UIPropFlexDirection value)
{
    PRIVATE()->FlexDirection = value;
}

UIPropFlexWrap UIWidget::getFlexWrap() const
{
    return PRIVATE()->FlexWrap;
}

void UIWidget::setFlexWrap(UIPropFlexWrap value)
{
    PRIVATE()->FlexWrap = value;
}

UIPropJustifyContent UIWidget::getJustifyContent() const
{
    return PRIVATE()->JustifyContent;
}

void UIWidget::setJustifyContent(UIPropJustifyContent value)
{
    PRIVATE()->JustifyContent = value;
}

UIPropAlignItems UIWidget::getAlignItems() const
{
    return PRIVATE()->AlignItems;
}

void UIWidget::setAlignItems(UIPropAlignItems value)
{
    PRIVATE()->AlignItems = value;
}

UIPropAlignContent UIWidget::getAlignContent() const
{
    return PRIVATE()->AlignContent;
}

void UIWidget::setAlignContent(UIPropAlignContent value)
{
    PRIVATE()->AlignContent = value;
}

void UIWidget::setFlexFlow(UIPropFlexDirection direction, UIPropFlexWrap wrap)
{
    setFlexDirection(direction);
    setFlexWrap(wrap);
}

UIPropFlexGrow UIWidget::getFlexGrow() const
{
    return PRIVATE()->Flex.Grow;
}

void UIWidget::setFlexGrow(UIPropFlexGrow value)
{
    PRIVATE()->Flex.Grow = value;
}

UIPropFlexShrink UIWidget::getFlexShrink() const
{
    return PRIVATE()->Flex.Shrink;
}

void UIWidget::setFlexShrink(UIPropFlexShrink value)
{
    PRIVATE()->Flex.Shrink = value;
}

UIPropFlexBasis UIWidget::getFlexBasis() const
{
    return PRIVATE()->Flex.Basis;
}

void UIWidget::setFlexBasis(UIPropFlexBasis value)
{
    PRIVATE()->Flex.Basis = value;
}

UIPropAlignSelf UIWidget::getAlignSelf() const
{
    return PRIVATE()->AlignSelf;
}

void UIWidget::setAlignSelf(UIPropAlignSelf value)
{
    PRIVATE()->AlignSelf = value;
}

void UIWidget::setFlex(UIPropFlexGrow grow, UIPropFlexShrink shrink, UIPropFlexBasis basis)
{
    setFlexGrow(grow);
    setFlexShrink(shrink);
    setFlexBasis(basis);
}

UIPropGridTemplateColumns UIWidget::getGridTemplateColumns() const
{
    return PRIVATE()->GridTemplateColumns;
}

void UIWidget::setGridTemplateColumns(UIPropGridTemplateColumns value)
{
    PRIVATE()->GridTemplateColumns = value;
}

void UIWidget::setGridTemplateColumns(UIList<UIGridTrack> value)
{
    UIPropGridTemplateColumns prop;
    prop.Tracks = std::move(value);
    setGridTemplateColumns(prop);
}

UIPropGridTemplateRows UIWidget::getGridTemplateRows() const
{
    return PRIVATE()->GridTemplateRows;
}

void UIWidget::setGridTemplateRows(UIPropGridTemplateRows value)
{
    PRIVATE()->GridTemplateRows = value;
}

void UIWidget::setGridTemplateRows(UIList<UIGridTrack> value)
{
    UIPropGridTemplateRows prop;
    prop.Tracks = std::move(value);
    setGridTemplateRows(prop);
}

UIPropGridAutoColumns UIWidget::getGridAutoColumns() const
{
    return PRIVATE()->GridAutoColumns;
}

void UIWidget::setGridAutoColumns(UIPropGridAutoColumns value)
{
    PRIVATE()->GridAutoColumns = value;
}

void UIWidget::setGridAutoColumns(UIList<UIGridTrack> value)
{
    UIPropGridAutoColumns prop;
    prop.Tracks = std::move(value);
    setGridAutoColumns(prop);
}

UIPropGridAutoRows UIWidget::getGridAutoRows() const
{
    return PRIVATE()->GridAutoRows;
}

void UIWidget::setGridAutoRows(UIPropGridAutoRows value)
{
    PRIVATE()->GridAutoRows = value;
}

void UIWidget::setGridAutoRows(UIList<UIGridTrack> value)
{
    UIPropGridAutoRows prop;
    prop.Tracks = std::move(value);
    setGridAutoRows(prop);
}

UIPropGridColumnStart UIWidget::getGridColumnStart() const
{
    return PRIVATE()->GridColumn.Start;
}

void UIWidget::setGridColumnStart(UIPropGridColumnStart value)
{
    PRIVATE()->GridColumn.Start = value;
}

void UIWidget::setGridColumnStart(int value)
{
    UIPropGridColumnStart prop;
    prop.Type = UI_CSS_GRID_LINE_INTEGER;
    prop.Value = value;
    setGridColumnStart(prop);
}

void UIWidget::setGridColumnStartAuto()
{
    setGridColumnStart(UIPropGridColumnStart{});
}

void UIWidget::setGridColumnStartSpan(int span)
{
    UIPropGridColumnStart prop;
    prop.Type = UI_CSS_GRID_LINE_SPAN;
    prop.Value = span;
    setGridColumnStart(prop);
}

UIPropGridColumnEnd UIWidget::getGridColumnEnd() const
{
    return PRIVATE()->GridColumn.End;
}

void UIWidget::setGridColumnEnd(UIPropGridColumnEnd value)
{
    PRIVATE()->GridColumn.End = value;
}

void UIWidget::setGridColumnEnd(int value)
{
    UIPropGridColumnEnd prop;
    prop.Type = UI_CSS_GRID_LINE_INTEGER;
    prop.Value = value;
    setGridColumnEnd(prop);
}

void UIWidget::setGridColumnEndAuto()
{
    setGridColumnEnd(UIPropGridColumnEnd{});
}

void UIWidget::setGridColumnEndSpan(int span)
{
    UIPropGridColumnEnd prop;
    prop.Type = UI_CSS_GRID_LINE_SPAN;
    prop.Value = span;
    setGridColumnEnd(prop);
}

UIPropGridRowStart UIWidget::getGridRowStart() const
{
    return PRIVATE()->GridRow.Start;
}

void UIWidget::setGridRowStart(UIPropGridRowStart value)
{
    PRIVATE()->GridRow.Start = value;
}

void UIWidget::setGridRowStart(int value)
{
    UIPropGridRowStart prop;
    prop.Type = UI_CSS_GRID_LINE_INTEGER;
    prop.Value = value;
    setGridRowStart(prop);
}

void UIWidget::setGridRowStartAuto()
{
    setGridRowStart(UIPropGridRowStart{});
}

void UIWidget::setGridRowStartSpan(int span)
{
    UIPropGridRowStart prop;
    prop.Type = UI_CSS_GRID_LINE_SPAN;
    prop.Value = span;
    setGridRowStart(prop);
}

UIPropGridRowEnd UIWidget::getGridRowEnd() const
{
    return PRIVATE()->GridRow.End;
}

void UIWidget::setGridRowEnd(UIPropGridRowEnd value)
{
    PRIVATE()->GridRow.End = value;
}

void UIWidget::setGridRowEnd(int value)
{
    UIPropGridRowEnd prop;
    prop.Type = UI_CSS_GRID_LINE_INTEGER;
    prop.Value = value;
    setGridRowEnd(prop);
}

void UIWidget::setGridRowEndAuto()
{
    setGridRowEnd(UIPropGridRowEnd{});
}

void UIWidget::setGridRowEndSpan(int span)
{
    UIPropGridRowEnd prop;
    prop.Type = UI_CSS_GRID_LINE_SPAN;
    prop.Value = span;
    setGridRowEnd(prop);
}

UIPropGridColumnGap UIWidget::getGridColumnGap() const
{
    return PRIVATE()->Gap.Column;
}

void UIWidget::setGridColumnGap(UIPropGridColumnGap value)
{
    PRIVATE()->Gap.Column = value;
}

void UIWidget::setGridColumnGap(float value)
{
    UIPropGridColumnGap prop;
    prop.Type = UI_CSS_GRID_GAP_LENGTH;
    prop.Value = value;
    setGridColumnGap(prop);
}

void UIWidget::setGridColumnGapPercent(float value)
{
    UIPropGridColumnGap prop;
    prop.Type = UI_CSS_GRID_GAP_PERCENTAGE;
    prop.Value = value;
    setGridColumnGap(prop);
}

UIPropGridRowGap UIWidget::getGridRowGap() const
{
    return PRIVATE()->Gap.Row;
}

void UIWidget::setGridRowGap(UIPropGridRowGap value)
{
    PRIVATE()->Gap.Row = value;
}

void UIWidget::setGridRowGap(float value)
{
    UIPropGridRowGap prop;
    prop.Type = UI_CSS_GRID_GAP_LENGTH;
    prop.Value = value;
    setGridRowGap(prop);
}

void UIWidget::setGridRowGapPercent(float value)
{
    UIPropGridRowGap prop;
    prop.Type = UI_CSS_GRID_GAP_PERCENTAGE;
    prop.Value = value;
    setGridRowGap(prop);
}

void UIWidget::setGridGap(float value)
{
    setGridRowGap(value);
    setGridColumnGap(value);
}

void UIWidget::setGridGap(float row, float column)
{
    setGridRowGap(row);
    setGridColumnGap(column);
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
    switch (UIHash(name))
    {
    case UIHash("align-content"):
        setEmbedStyle(name, PRIVATE()->AlignContent);
        break;
    case UIHash("align-items"):
        setEmbedStyle(name, PRIVATE()->AlignItems);
        break;
    case UIHash("align-self"):
        setEmbedStyle(name, PRIVATE()->AlignSelf);
        break;
    case UIHash("alignment-baseline"): setStyle<UIPropAlignmentBaseline>(name);
        break;
    case UIHash("baseline-shift"): setStyle<UIPropBaselineShift>(name);
        break;
    case UIHash("baseline-source"): setStyle<UIPropBaselineSource>(name);
        break;
    case UIHash("border"):
        setEmbedStyle(name, PRIVATE()->Border);
        break;
    case UIHash("border-bottom"):
        setEmbedStyle("border", PRIVATE()->Border);
        setEmbedStyle(name, PRIVATE()->Border.Bottom);
        break;
    case UIHash("border-left"):
        setEmbedStyle("border", PRIVATE()->Border);
        setEmbedStyle(name, PRIVATE()->Border.Left);
        break;
    case UIHash("border-right"):
        setEmbedStyle("border", PRIVATE()->Border);
        setEmbedStyle(name, PRIVATE()->Border.Right);
        break;
    case UIHash("border-top"):
        setEmbedStyle("border", PRIVATE()->Border);
        setEmbedStyle(name, PRIVATE()->Border.Top);
        break;
    case UIHash("border-color"):
        setEmbedStyle(name, PRIVATE()->BorderColor);
        break;
    case UIHash("border-width"):
        setEmbedStyle("border", PRIVATE()->Border);
        setEmbedStyle(name, PRIVATE()->Border);
        break;
    case UIHash("border-radius"):
        setEmbedStyle(name, PRIVATE()->BorderRadius);
        break;
    case UIHash("border-top-left-radius"):
        setEmbedStyle("border-radius", PRIVATE()->BorderRadius);
        setEmbedStyle(name, PRIVATE()->BorderRadius.TopLeft);
        break;
    case UIHash("border-top-right-radius"):
        setEmbedStyle("border-radius", PRIVATE()->BorderRadius);
        setEmbedStyle(name, PRIVATE()->BorderRadius.TopRight);
        break;
    case UIHash("border-bottom-right-radius"):
        setEmbedStyle("border-radius", PRIVATE()->BorderRadius);
        setEmbedStyle(name, PRIVATE()->BorderRadius.BottomRight);
        break;
    case UIHash("border-bottom-left-radius"):
        setEmbedStyle("border-radius", PRIVATE()->BorderRadius);
        setEmbedStyle(name, PRIVATE()->BorderRadius.BottomLeft);
        break;
    case UIHash("bottom"):
        setEmbedStyle(name, PRIVATE()->Bottom);
        break;
    case UIHash("box-sizing"):
        setEmbedStyle(name, PRIVATE()->BoxSizing);
        break;
    case UIHash("background"):
        setEmbedStyle(name, PRIVATE()->Background);
        setEmbedStyle("background-color", PRIVATE()->Background.Color);
        break;
    case UIHash("background-color"):
        setEmbedStyle("background", PRIVATE()->Background);
        setEmbedStyle(name, PRIVATE()->Background.Color);
        break;
    case UIHash("clear"): setStyle<UIPropClear>(name);
        break;
    case UIHash("color"):
        setEmbedStyle("foreground", PRIVATE()->Foreground, true);
        setEmbedStyle(name, PRIVATE()->Foreground.Color, true);
        break;
    case UIHash("direction"):
        setEmbedStyle(name, PRIVATE()->Direction, true);
        break;
    case UIHash("display"):
        setEmbedStyle(name, PRIVATE()->Display);
        break;
    //case UIHash("dominant-baseline"): setStyle<UIPropDominantBaseline>(name); break;
    case UIHash("filter"): setStyle<UIPropFilter>(name);
        break;
    case UIHash("foreground"):
        setEmbedStyle(name, PRIVATE()->Foreground, true);
        setEmbedStyle("color", PRIVATE()->Foreground.Color, true);
        break;
    case UIHash("flex"):
        setEmbedStyle("flex", PRIVATE()->Flex);
        setEmbedStyle("flex-grow", PRIVATE()->Flex.Grow);
        setEmbedStyle("flex-shrink", PRIVATE()->Flex.Shrink);
        setEmbedStyle("flex-basis", PRIVATE()->Flex.Basis);
        break;
    case UIHash("flex-basis"):
        setEmbedStyle("flex", PRIVATE()->Flex);
        setEmbedStyle(name, PRIVATE()->Flex.Basis);
        break;
    case UIHash("flex-direction"):
        setEmbedStyle(name, PRIVATE()->FlexDirection);
        break;
    case UIHash("flex-grow"):
        setEmbedStyle("flex", PRIVATE()->Flex);
        setEmbedStyle(name, PRIVATE()->Flex.Grow);
        break;
    case UIHash("flex-shrink"):
        setEmbedStyle("flex", PRIVATE()->Flex);
        setEmbedStyle(name, PRIVATE()->Flex.Shrink);
        break;
    case UIHash("flex-wrap"):
        setEmbedStyle(name, PRIVATE()->FlexWrap);
        break;
    case UIHash("gap"):
        setEmbedStyle("gap", PRIVATE()->Gap);
        setEmbedStyle("row-gap", PRIVATE()->Gap.Row);
        setEmbedStyle("column-gap", PRIVATE()->Gap.Column);
        break;
    case UIHash("column-gap"):
    case UIHash("grid-column-gap"):
        setEmbedStyle("gap", PRIVATE()->Gap);
        setEmbedStyle("column-gap", PRIVATE()->Gap.Column);
        name = "column-gap";
        break;
    case UIHash("row-gap"):
    case UIHash("grid-row-gap"):
        setEmbedStyle("gap", PRIVATE()->Gap);
        setEmbedStyle("row-gap", PRIVATE()->Gap.Row);
        name = "row-gap";
        break;
    case UIHash("grid-template-columns"):
        setEmbedStyle(name, PRIVATE()->GridTemplateColumns);
        break;
    case UIHash("grid-template-rows"):
        setEmbedStyle(name, PRIVATE()->GridTemplateRows);
        break;
    case UIHash("grid-auto-columns"):
        setEmbedStyle(name, PRIVATE()->GridAutoColumns);
        break;
    case UIHash("grid-auto-rows"):
        setEmbedStyle(name, PRIVATE()->GridAutoRows);
        break;
    case UIHash("grid-column-start"):
        setEmbedStyle("grid-column", PRIVATE()->GridColumn);
        setEmbedStyle(name, PRIVATE()->GridColumn.Start);
        break;
    case UIHash("grid-column-end"):
        setEmbedStyle("grid-column", PRIVATE()->GridColumn);
        setEmbedStyle(name, PRIVATE()->GridColumn.End);
        break;
    case UIHash("grid-row-start"):
        setEmbedStyle("grid-row", PRIVATE()->GridRow);
        setEmbedStyle(name, PRIVATE()->GridRow.Start);
        break;
    case UIHash("grid-row-end"):
        setEmbedStyle("grid-row", PRIVATE()->GridRow);
        setEmbedStyle(name, PRIVATE()->GridRow.End);
        break;
    case UIHash("grid-column"):
        setEmbedStyle("grid-column", PRIVATE()->GridColumn);
        setEmbedStyle("grid-column-start", PRIVATE()->GridColumn.Start);
        setEmbedStyle("grid-column-end", PRIVATE()->GridColumn.End);
        break;
    case UIHash("grid-row"):
        setEmbedStyle("grid-row", PRIVATE()->GridRow);
        setEmbedStyle("grid-row-start", PRIVATE()->GridRow.Start);
        setEmbedStyle("grid-row-end", PRIVATE()->GridRow.End);
        break;
    case UIHash("justify-items"):
        setEmbedStyle(name, PRIVATE()->JustifyItems);
        break;
    case UIHash("justify-self"):
        setEmbedStyle(name, PRIVATE()->JustifySelf);
        break;
    case UIHash("aspect-ratio"):
        setEmbedStyle(name, PRIVATE()->AspectRatio);
        break;
    case UIHash("overflow"):
        setEmbedStyle("overflow", PRIVATE()->Overflow);
        setEmbedStyle("overflow-x", PRIVATE()->Overflow.X);
        setEmbedStyle("overflow-y", PRIVATE()->Overflow.Y);
        break;
    //case UIHash("float"): setStyle<UIPropFloat>(name); break;
    //case UIHash("float-defer"): setStyle<UIPropFloatDefer>(name); break;
    case UIHash("float-offset"): setStyle<UIPropFloatOffset>(name);
        break;
    //case UIHash("float-reference"): setStyle<UIPropFloatReference>(name); break;
    //case UIHash("font-family"): setStyle<UIPropFontFamily>(name, {}, true); break;
    case UIHash("font-size"): setStyle<UIPropFontSize>(name, {}, true);
        break;
    case UIHash("font-stretch"): setStyle<UIPropFontStretch>(name);
        break;
    //case UIHash("font-style"): setStyle<UIPropFontStyle>(name, {}, true); break;
    case UIHash("font-weight"): setStyle<UIPropFontWeight>(name, {}, true);
        break;
    //case UIHash("hanging-punctuation"): setStyle<UIPropHangingPunctuation>(name); break;
    case UIHash("height"):
        setEmbedStyle(name, PRIVATE()->Height);
        break;
    //case UIHash("hyphens"): setStyle<UIPropHyphens>(name); break;
    case UIHash("inset-block-end"): setStyle<UIPropInsetBlockEnd>(name);
        break;
    case UIHash("inset-block-start"): setStyle<UIPropInsetBlockStart>(name);
        break;
    case UIHash("inset-inline-end"): setStyle<UIPropInsetInlineEnd>(name);
        break;
    case UIHash("inset-inline-start"): setStyle<UIPropInsetInlineStart>(name);
        break;
    case UIHash("justify-content"):
        setEmbedStyle(name, PRIVATE()->JustifyContent);
        break;
    case UIHash("left"):
        setEmbedStyle(name, PRIVATE()->Left);
        break;
    case UIHash("letter-spacing"): setStyle<UIPropLetterSpacing>(name, {}, true);
        break;
    //case UIHash("line-break"): setStyle<UIPropLineBreak>(name); break;
    case UIHash("line-height"): setStyle<UIPropLineHeight>(name, {}, true);
        break;
    case UIHash("margin"):
        setEmbedStyle(name, PRIVATE()->Margin);
        break;
    case UIHash("margin-bottom"):
        setEmbedStyle("margin", PRIVATE()->Margin);
        setEmbedStyle(name, PRIVATE()->Margin.Bottom);
        break;
    case UIHash("margin-left"):
        setEmbedStyle("margin", PRIVATE()->Margin);
        setEmbedStyle(name, PRIVATE()->Margin.Left);
        break;
    case UIHash("margin-right"):
        setEmbedStyle("margin", PRIVATE()->Margin);
        setEmbedStyle(name, PRIVATE()->Margin.Right);
        break;
    case UIHash("margin-top"):
        setEmbedStyle("margin", PRIVATE()->Margin);
        setEmbedStyle(name, PRIVATE()->Margin.Top);
        break;
    case UIHash("max-height"):
        setEmbedStyle(name, PRIVATE()->MaxHeight);
        break;
    case UIHash("max-width"):
        setEmbedStyle(name, PRIVATE()->MaxWidth);
        break;
    case UIHash("min-height"):
        setEmbedStyle(name, PRIVATE()->MinHeight);
        break;
    case UIHash("min-width"):
        setEmbedStyle(name, PRIVATE()->MinWidth);
        break;
    //case UIHash("opacity"): setStyle<UIPropOpacity>(name); break;
    //case UIHash("order"): setStyle<UIPropOrder>(name); break;
    //case UIHash("overflow-block"): setStyle<UIPropOverflowBlock>(name); break;
    //case UIHash("overflow-inline"): setStyle<UIPropOverflowInline>(name); break;
    //case UIHash("overflow-wrap"): setStyle<UIPropOverflowWrap>(name); break;
    case UIHash("overflow-x"):
        setEmbedStyle("overflow", PRIVATE()->Overflow);
        setEmbedStyle(name, PRIVATE()->Overflow.X);
        break;
    case UIHash("overflow-y"):
        setEmbedStyle("overflow", PRIVATE()->Overflow);
        setEmbedStyle(name, PRIVATE()->Overflow.Y);
        break;
    case UIHash("padding"):
        setEmbedStyle(name, PRIVATE()->Padding);
        break;
    case UIHash("padding-bottom"):
        setEmbedStyle("padding", PRIVATE()->Padding);
        setEmbedStyle(name, PRIVATE()->Padding.Bottom);
        break;
    case UIHash("padding-left"):
        setEmbedStyle("padding", PRIVATE()->Padding);
        setEmbedStyle(name, PRIVATE()->Padding.Left);
        break;
    case UIHash("padding-right"):
        setEmbedStyle("padding", PRIVATE()->Padding);
        setEmbedStyle(name, PRIVATE()->Padding.Right);
        break;
    case UIHash("padding-top"):
        setEmbedStyle("padding", PRIVATE()->Padding);
        setEmbedStyle(name, PRIVATE()->Padding.Top);
        break;
    case UIHash("position"):
        setEmbedStyle(name, PRIVATE()->Position);
        break;
    case UIHash("right"):
        setEmbedStyle(name, PRIVATE()->Right);
        break;
    case UIHash("tab-size"): setStyle<UIPropTabSize>(name);
        break;
    //case UIHash("text-align"): setStyle<UIPropTextAlign>(name, {}, true); break;
    //case UIHash("text-align-all"): setStyle<UIPropTextAlignAll>(name); break;
    //case UIHash("text-align-last"): setStyle<UIPropTextAlignLast>(name); break;
    //case UIHash("text-combine-upright"): setStyle<UIPropTextCombineUpright>(name); break;
    //case UIHash("text-decoration-line"): setStyle<UIPropTextDecorationLine>(name); break;
    //case UIHash("text-decoration-style"): setStyle<UIPropTextDecorationStyle>(name); break;
    case UIHash("text-indent"): setStyle<UIPropTextIndent>(name, {}, true);
        break;
    //case UIHash("text-justify"): setStyle<UIPropTextJustify>(name); break;
    //case UIHash("text-orientation"): setStyle<UIPropTextOrientation>(name); break;
    //case UIHash("text-overflow"): setStyle<UIPropTextOverflow>(name); break;
    //case UIHash("text-transform"): setStyle<UIPropTextTransform>(name, {}, true); break;
    case UIHash("top"):
        setEmbedStyle(name, PRIVATE()->Top);
        break;
    //case UIHash("unicode-bidi"): setStyle<UIPropUnicodeBidi>(name); break;
    //case UIHash("vertical-align"): setStyle<UIPropVerticalAlign>(name); break;
    case UIHash("visibility"): setStyle<UIPropVisibility>(name, {}, true);
        break;
    //case UIHash("white-space"): setStyle<UIPropWhiteSpace>(name, {}, true); break;
    case UIHash("width"):
        setEmbedStyle(name, PRIVATE()->Width);
        break;
    //case UIHash("word-break"): setStyle<UIPropWordBreak>(name); break;
    //case UIHash("word-spacing"): setStyle<UIPropWordSpacing>(name, {}, true); break;
    //case UIHash("word-wrap"): setStyle<UIPropWordWrap>(name); break;
    //case UIHash("wrap-flow"): setStyle<UIPropWrapFlow>(name); break;
    //case UIHash("wrap-through"): setStyle<UIPropWrapThrough>(name); break;
    case UIHash("writing-mode"): setStyle<UIPropWritingMode>(name);
        break;
    case UIHash("z-index"): setStyle<UIPropZIndex>(name);
        break;
    default: break;
    }
    return getStyles()->setStyleText(name, value);
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

void UIWidget::setTarget(UIImage value)
{
    PRIVATE()->Target = value;
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
