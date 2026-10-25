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
    UIWidgetStyle Normal;
    UIWidgetStyle Hover;
    UIWidgetStyle Active;
    UIWidgetStyle Disabled;
    UIWidgetStyle Checked;

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

    auto color = PRIVATE()->Normal.Background.Color;
    if (color.Value.A == 0) return;
    UIBrush brush{ .Color { color.Value.R / 255.0f, color.Value.G / 255.0f, color.Value.B / 255.0f, color.Value.A / 255.0f } };
    painter->setPen(UINoPen);
    painter->setBrush(brush);
    auto radius = PRIVATE()->Normal.BorderRadius.TopLeft;
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
    return PRIVATE()->Normal.Display;
}

void UIWidget::setDisplayType(UIPropDisplay value)
{
    PRIVATE()->Normal.Display = value;
}

UIPropPosition UIWidget::getPositionType() const
{
    return PRIVATE()->Normal.Position;
}

void UIWidget::setPositionType(UIPropPosition value)
{
    PRIVATE()->Normal.Position = value;
}

UIPropLeft UIWidget::getFixedPosX() const
{
    return PRIVATE()->Normal.Left;
}

void UIWidget::setFixedPosX(UIPropLeft value)
{
    PRIVATE()->Normal.Left = value;
}

void UIWidget::setFixedPosX(float value)
{
    setFixedPosX({value, UI_CSS_LEFT_LENGTH});
}

UIPropTop UIWidget::getFixedPosY() const
{
    return PRIVATE()->Normal.Top;
}

void UIWidget::setFixedPosY(UIPropTop value)
{
    PRIVATE()->Normal.Top = value;
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
    return PRIVATE()->Normal.MinWidth;
}

void UIWidget::setMinWidth(UIPropMinWidth value)
{
    PRIVATE()->Normal.MinWidth = value;
}

void UIWidget::setMinWidth(float value)
{
    setMinWidth({value, UI_CSS_MIN_WIDTH_LENGTH});
}

UIPropMaxWidth UIWidget::getMaxWidth() const
{
    return PRIVATE()->Normal.MaxWidth;
}

void UIWidget::setMaxWidth(UIPropMaxWidth value)
{
    PRIVATE()->Normal.MaxWidth = value;
}

void UIWidget::setMaxWidth(float value)
{
    setMaxWidth({value, UI_CSS_MAX_WIDTH_LENGTH});
}

UIPropWidth UIWidget::getFixedWidth() const
{
    return PRIVATE()->Normal.Width;
}

void UIWidget::setFixedWidth(UIPropWidth value)
{
    PRIVATE()->Normal.Width = value;
}

void UIWidget::setFixedWidth(float value)
{
    setFixedWidth({value, UI_CSS_WIDTH_LENGTH});
}

UIPropMinHeight UIWidget::getMinHeight() const
{
    return PRIVATE()->Normal.MinHeight;
}

void UIWidget::setMinHeight(UIPropMinHeight value)
{
    PRIVATE()->Normal.MinHeight = value;
}

void UIWidget::setMinHeight(float value)
{
    setMinHeight({value, UI_CSS_MIN_HEIGHT_LENGTH});
}

UIPropMaxHeight UIWidget::getMaxHeight() const
{
    return PRIVATE()->Normal.MaxHeight;
}

void UIWidget::setMaxHeight(UIPropMaxHeight value)
{
    PRIVATE()->Normal.MaxHeight = value;
}

void UIWidget::setMaxHeight(float value)
{
    setMaxHeight({value, UI_CSS_MAX_HEIGHT_LENGTH});
}

UIPropHeight UIWidget::getFixedHeight() const
{
    return PRIVATE()->Normal.Height;
}

void UIWidget::setFixedHeight(UIPropHeight value)
{
    PRIVATE()->Normal.Height = value;
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
    return PRIVATE()->Normal.Border.Top;
}

void UIWidget::setBorderTop(UIPropBorderTop value)
{
    PRIVATE()->Normal.Border.Top = value;
}

UIPropBorderBottom UIWidget::getBorderBottom() const
{
    return PRIVATE()->Normal.Border.Bottom;
}

void UIWidget::setBorderBottom(UIPropBorderBottom value)
{
    PRIVATE()->Normal.Border.Bottom = value;
}

UIPropBorderLeft UIWidget::getBorderLeft() const
{
    return PRIVATE()->Normal.Border.Left;
}

void UIWidget::setBorderLeft(UIPropBorderLeft value)
{
    PRIVATE()->Normal.Border.Left = value;
}

UIPropBorderRight UIWidget::getBorderRight() const
{
    return PRIVATE()->Normal.Border.Right;
}

void UIWidget::setBorderRight(UIPropBorderRight value)
{
    PRIVATE()->Normal.Border.Right = value;
}

UIPropColor UIWidget::getBorderColor() const
{
    return PRIVATE()->Normal.BorderColor;
}

void UIWidget::setBorderColor(UIPropColor value)
{
    PRIVATE()->Normal.BorderColor = value;
}

UIPropBorderRadius UIWidget::getBorderRadius() const
{
    return PRIVATE()->Normal.BorderRadius;
}

void UIWidget::setBorderRadius(UIPropBorderRadius value)
{
    PRIVATE()->Normal.BorderRadius = value;
}

UIPropBorderTopLeftRadius UIWidget::getBorderTopLeftRadius() const
{
    return PRIVATE()->Normal.BorderRadius.TopLeft;
}

void UIWidget::setBorderTopLeftRadius(UIPropBorderTopLeftRadius value)
{
    PRIVATE()->Normal.BorderRadius.TopLeft = value;
}

UIPropBorderTopRightRadius UIWidget::getBorderTopRightRadius() const
{
    return PRIVATE()->Normal.BorderRadius.TopRight;
}

void UIWidget::setBorderTopRightRadius(UIPropBorderTopRightRadius value)
{
    PRIVATE()->Normal.BorderRadius.TopRight = value;
}

UIPropBorderBottomRightRadius UIWidget::getBorderBottomRightRadius() const
{
    return PRIVATE()->Normal.BorderRadius.BottomRight;
}

void UIWidget::setBorderBottomRightRadius(UIPropBorderBottomRightRadius value)
{
    PRIVATE()->Normal.BorderRadius.BottomRight = value;
}

UIPropBorderBottomLeftRadius UIWidget::getBorderBottomLeftRadius() const
{
    return PRIVATE()->Normal.BorderRadius.BottomLeft;
}

void UIWidget::setBorderBottomLeftRadius(UIPropBorderBottomLeftRadius value)
{
    PRIVATE()->Normal.BorderRadius.BottomLeft = value;
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
    return PRIVATE()->Normal.Margin.Top;
}

void UIWidget::setMarginTop(UIPropMarginTop value)
{
    PRIVATE()->Normal.Margin.Top = value;
}

UIPropMarginBottom UIWidget::getMarginBottom() const
{
    return PRIVATE()->Normal.Margin.Bottom;
}

void UIWidget::setMarginBottom(UIPropMarginBottom value)
{
    PRIVATE()->Normal.Margin.Bottom = value;
}

UIPropMarginLeft UIWidget::getMarginLeft() const
{
    return PRIVATE()->Normal.Margin.Left;
}

void UIWidget::setMarginLeft(UIPropMarginLeft value)
{
    PRIVATE()->Normal.Margin.Left = value;
}

UIPropMarginRight UIWidget::getMarginRight() const
{
    return PRIVATE()->Normal.Margin.Right;
}

void UIWidget::setMarginRight(UIPropMarginRight value)
{
    PRIVATE()->Normal.Margin.Right = value;
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
    return PRIVATE()->Normal.Padding.Top;
}

void UIWidget::setPaddingTop(UIPropPaddingTop value)
{
    PRIVATE()->Normal.Padding.Top = value;
}

UIPropPaddingBottom UIWidget::getPaddingBottom() const
{
    return PRIVATE()->Normal.Padding.Bottom;
}

void UIWidget::setPaddingBottom(UIPropPaddingBottom value)
{
    PRIVATE()->Normal.Padding.Bottom = value;
}

UIPropPaddingLeft UIWidget::getPaddingLeft() const
{
    return PRIVATE()->Normal.Padding.Left;
}

void UIWidget::setPaddingLeft(UIPropPaddingLeft value)
{
    PRIVATE()->Normal.Padding.Left = value;
}

UIPropPaddingRight UIWidget::getPaddingRight() const
{
    return PRIVATE()->Normal.Padding.Right;
}

void UIWidget::setPaddingRight(UIPropPaddingRight value)
{
    PRIVATE()->Normal.Padding.Right = value;
}

UIPropFlexDirection UIWidget::getFlexDirection() const
{
    return PRIVATE()->Normal.FlexDirection;
}

void UIWidget::setFlexDirection(UIPropFlexDirection value)
{
    PRIVATE()->Normal.FlexDirection = value;
}

UIPropFlexWrap UIWidget::getFlexWrap() const
{
    return PRIVATE()->Normal.FlexWrap;
}

void UIWidget::setFlexWrap(UIPropFlexWrap value)
{
    PRIVATE()->Normal.FlexWrap = value;
}

UIPropJustifyContent UIWidget::getJustifyContent() const
{
    return PRIVATE()->Normal.JustifyContent;
}

void UIWidget::setJustifyContent(UIPropJustifyContent value)
{
    PRIVATE()->Normal.JustifyContent = value;
}

UIPropAlignItems UIWidget::getAlignItems() const
{
    return PRIVATE()->Normal.AlignItems;
}

void UIWidget::setAlignItems(UIPropAlignItems value)
{
    PRIVATE()->Normal.AlignItems = value;
}

UIPropAlignContent UIWidget::getAlignContent() const
{
    return PRIVATE()->Normal.AlignContent;
}

void UIWidget::setAlignContent(UIPropAlignContent value)
{
    PRIVATE()->Normal.AlignContent = value;
}

void UIWidget::setFlexFlow(UIPropFlexDirection direction, UIPropFlexWrap wrap)
{
    setFlexDirection(direction);
    setFlexWrap(wrap);
}

UIPropFlexGrow UIWidget::getFlexGrow() const
{
    return PRIVATE()->Normal.Flex.Grow;
}

void UIWidget::setFlexGrow(UIPropFlexGrow value)
{
    PRIVATE()->Normal.Flex.Grow = value;
}

UIPropFlexShrink UIWidget::getFlexShrink() const
{
    return PRIVATE()->Normal.Flex.Shrink;
}

void UIWidget::setFlexShrink(UIPropFlexShrink value)
{
    PRIVATE()->Normal.Flex.Shrink = value;
}

UIPropFlexBasis UIWidget::getFlexBasis() const
{
    return PRIVATE()->Normal.Flex.Basis;
}

void UIWidget::setFlexBasis(UIPropFlexBasis value)
{
    PRIVATE()->Normal.Flex.Basis = value;
}

UIPropAlignSelf UIWidget::getAlignSelf() const
{
    return PRIVATE()->Normal.AlignSelf;
}

void UIWidget::setAlignSelf(UIPropAlignSelf value)
{
    PRIVATE()->Normal.AlignSelf = value;
}

void UIWidget::setFlex(UIPropFlexGrow grow, UIPropFlexShrink shrink, UIPropFlexBasis basis)
{
    setFlexGrow(grow);
    setFlexShrink(shrink);
    setFlexBasis(basis);
}

UIPropGridTemplateColumns UIWidget::getGridTemplateColumns() const
{
    return PRIVATE()->Normal.GridTemplateColumns;
}

void UIWidget::setGridTemplateColumns(UIPropGridTemplateColumns value)
{
    PRIVATE()->Normal.GridTemplateColumns = value;
}

void UIWidget::setGridTemplateColumns(UIList<UIGridTrack> value)
{
    UIPropGridTemplateColumns prop;
    prop.Tracks = std::move(value);
    setGridTemplateColumns(prop);
}

UIPropGridTemplateRows UIWidget::getGridTemplateRows() const
{
    return PRIVATE()->Normal.GridTemplateRows;
}

void UIWidget::setGridTemplateRows(UIPropGridTemplateRows value)
{
    PRIVATE()->Normal.GridTemplateRows = value;
}

void UIWidget::setGridTemplateRows(UIList<UIGridTrack> value)
{
    UIPropGridTemplateRows prop;
    prop.Tracks = std::move(value);
    setGridTemplateRows(prop);
}

UIPropGridAutoColumns UIWidget::getGridAutoColumns() const
{
    return PRIVATE()->Normal.GridAutoColumns;
}

void UIWidget::setGridAutoColumns(UIPropGridAutoColumns value)
{
    PRIVATE()->Normal.GridAutoColumns = value;
}

void UIWidget::setGridAutoColumns(UIList<UIGridTrack> value)
{
    UIPropGridAutoColumns prop;
    prop.Tracks = std::move(value);
    setGridAutoColumns(prop);
}

UIPropGridAutoRows UIWidget::getGridAutoRows() const
{
    return PRIVATE()->Normal.GridAutoRows;
}

void UIWidget::setGridAutoRows(UIPropGridAutoRows value)
{
    PRIVATE()->Normal.GridAutoRows = value;
}

void UIWidget::setGridAutoRows(UIList<UIGridTrack> value)
{
    UIPropGridAutoRows prop;
    prop.Tracks = std::move(value);
    setGridAutoRows(prop);
}

UIPropGridColumnStart UIWidget::getGridColumnStart() const
{
    return PRIVATE()->Normal.GridColumn.Start;
}

void UIWidget::setGridColumnStart(UIPropGridColumnStart value)
{
    PRIVATE()->Normal.GridColumn.Start = value;
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
    return PRIVATE()->Normal.GridColumn.End;
}

void UIWidget::setGridColumnEnd(UIPropGridColumnEnd value)
{
    PRIVATE()->Normal.GridColumn.End = value;
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
    return PRIVATE()->Normal.GridRow.Start;
}

void UIWidget::setGridRowStart(UIPropGridRowStart value)
{
    PRIVATE()->Normal.GridRow.Start = value;
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
    return PRIVATE()->Normal.GridRow.End;
}

void UIWidget::setGridRowEnd(UIPropGridRowEnd value)
{
    PRIVATE()->Normal.GridRow.End = value;
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
    return PRIVATE()->Normal.Gap.Column;
}

void UIWidget::setGridColumnGap(UIPropGridColumnGap value)
{
    PRIVATE()->Normal.Gap.Column = value;
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
    return PRIVATE()->Normal.Gap.Row;
}

void UIWidget::setGridRowGap(UIPropGridRowGap value)
{
    PRIVATE()->Normal.Gap.Row = value;
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

UIPropBackground UIWidget::getBackground() const
{
    return PRIVATE()->Normal.Background;
}

void UIWidget::setBackground(UIPropBackground value)
{
    PRIVATE()->Normal.Background = value;
}

UIPropColor UIWidget::getBackgroundColor() const
{
    return PRIVATE()->Normal.Background.Color;
}

void UIWidget::setBackgroundColor(UIPropColor value)
{
    PRIVATE()->Normal.Background.Color = value;
}

UIPropForeground UIWidget::getForeground() const
{
    return PRIVATE()->Normal.Foreground;
}

void UIWidget::setForeground(UIPropForeground value)
{
    PRIVATE()->Normal.Foreground = value;
}

UIPropColor UIWidget::getForegroundColor() const
{
    return PRIVATE()->Normal.Foreground.Color;
}

void UIWidget::setForegroundColor(UIPropColor value)
{
    PRIVATE()->Normal.Foreground.Color = value;
}

UIWidgetStyle& UIWidget::getStyleNormal()
{
    return PRIVATE()->Normal;
}

UIWidgetStyle const& UIWidget::getStyleNormal() const
{
    return PRIVATE()->Normal;
}

UIWidgetStyle& UIWidget::getStyleHover()
{
    return PRIVATE()->Hover;
}

UIWidgetStyle const& UIWidget::getStyleHover() const
{
    return PRIVATE()->Hover;
}

UIWidgetStyle& UIWidget::getStyleActive()
{
    return PRIVATE()->Active;
}

UIWidgetStyle const& UIWidget::getStyleActive() const
{
    return PRIVATE()->Active;
}

UIWidgetStyle& UIWidget::getStyleDisabled()
{
    return PRIVATE()->Disabled;
}

UIWidgetStyle const& UIWidget::getStyleDisabled() const
{
    return PRIVATE()->Disabled;
}

UIWidgetStyle& UIWidget::getStyleChecked()
{
    return PRIVATE()->Checked;
}

UIWidgetStyle const& UIWidget::getStyleChecked() const
{
    return PRIVATE()->Checked;
}

void UIWidget::writeColor(UIPropColor& dst, UIColor const& color)
{
    dst.Type = UI_CSS_COLOR_RGBA;
    dst.Value.R = (uint8_t)(color.R * 255.0f + 0.5f);
    dst.Value.G = (uint8_t)(color.G * 255.0f + 0.5f);
    dst.Value.B = (uint8_t)(color.B * 255.0f + 0.5f);
    dst.Value.A = (uint8_t)(color.A * 255.0f + 0.5f);
}

UIColor UIWidget::readColor(UIPropColor const& color) const
{
    return { color.Value.R / 255.0f, color.Value.G / 255.0f, color.Value.B / 255.0f, color.Value.A / 255.0f };
}

void UIWidget::assignFontFamily(UIWidgetStyle& style, UIString const& family)
{
    UIPropFontFamily parsed;
    if (family.empty()) style.FontFamily = {};
    else if (UITypeC(family, parsed)) style.FontFamily = parsed;
}

void UIWidget::applyFontFamily(UIWidgetStyle const& style, UIFont& font) const
{
    if (style.FontFamily.Name.empty() == false) font.Family = style.FontFamily.Name;
    switch (style.FontSize.Type)
    {
    case UI_CSS_FONT_SIZE_XX_SMALL: font.Size = 9; break;
    case UI_CSS_FONT_SIZE_X_SMALL: font.Size = 10; break;
    case UI_CSS_FONT_SIZE_SMALL: font.Size = 13; break;
    case UI_CSS_FONT_SIZE_LARGE: font.Size = 18; break;
    case UI_CSS_FONT_SIZE_X_LARGE: font.Size = 24; break;
    case UI_CSS_FONT_SIZE_XX_LARGE: font.Size = 32; break;
    case UI_CSS_FONT_SIZE_XXX_LARGE: font.Size = 48; break;
    case UI_CSS_FONT_SIZE_LARGER: font.Size = (uint32_t)(font.Size * 1.2f + 0.5f); if (font.Size < 1) font.Size = 1; break;
    case UI_CSS_FONT_SIZE_SMALLER: font.Size = (uint32_t)(font.Size / 1.2f + 0.5f); if (font.Size < 1) font.Size = 1; break;
    case UI_CSS_FONT_SIZE_LENGTH:
        if (style.FontSize.Value > 0.0f) font.Size = (uint32_t)(style.FontSize.Value + 0.5f);
        break;
    default: break;
    }
    switch (style.FontStyle.Value)
    {
    default:
    case UI_CSS_FONT_STYLE_NORMAL: font.Style = UIFont::StyleNormal; break;
    case UI_CSS_FONT_STYLE_ITALIC: font.Style = UIFont::StyleItalic; break;
    case UI_CSS_FONT_STYLE_OBLIQUE: font.Style = UIFont::StyleOblique; break;
    }
    auto snapWeight = [](int value) -> UIFont::weight_t
    {
        if (value < 150) return UIFont::WeightThin;
        if (value < 250) return UIFont::WeightExtraLight;
        if (value < 350) return UIFont::WeightLight;
        if (value < 450) return UIFont::WeightNormal;
        if (value < 550) return UIFont::WeightMedium;
        if (value < 650) return UIFont::WeightDemiBold;
        if (value < 750) return UIFont::WeightBold;
        if (value < 850) return UIFont::WeightExtraBold;
        return UIFont::WeightBlack;
    };
    switch (style.FontWeight.Type)
    {
    case UI_CSS_FONT_WEIGHT_BOLD: font.Weight = UIFont::WeightBold; break;
    case UI_CSS_FONT_WEIGHT_BOLDER: font.Weight = snapWeight((int)font.Weight + 200); break;
    case UI_CSS_FONT_WEIGHT_LIGHTER: font.Weight = snapWeight((int)font.Weight - 200); break;
    case UI_CSS_FONT_WEIGHT_NUMBER: font.Weight = snapWeight((int)(style.FontWeight.Value + 0.5f)); break;
    default: break;
    }
    if (style.LetterSpacing.Type == UI_CSS_LETTER_SPACING_LENGTH)
        font.Spacing = style.LetterSpacing.Value > 0.0f ? (uint32_t)(style.LetterSpacing.Value + 0.5f) : 0;
    switch (style.LineHeight.Type)
    {
    case UI_CSS_LINE_HEIGHT_NUMBER: font.LineSpacing = style.LineHeight.Value; break;
    case UI_CSS_LINE_HEIGHT_PERCENTAGE: font.LineSpacing = style.LineHeight.Value / 100.0f; break;
    case UI_CSS_LINE_HEIGHT_LENGTH:
        if (font.Size > 0) font.LineSpacing = style.LineHeight.Value / (float)font.Size;
        break;
    default: break;
    }
    uint32_t horizontal = 0;
    bool align = true;
    switch (style.TextAlign.Value)
    {
    case UI_CSS_TEXT_ALIGN_LEFT: horizontal = UIFont::AlignLeft; break;
    case UI_CSS_TEXT_ALIGN_END:
    case UI_CSS_TEXT_ALIGN_RIGHT: horizontal = UIFont::AlignRight; break;
    case UI_CSS_TEXT_ALIGN_CENTER: horizontal = UIFont::AlignCenter; break;
    case UI_CSS_TEXT_ALIGN_JUSTIFY:
    case UI_CSS_TEXT_ALIGN_JUSTIFY_ALL: horizontal = UIFont::AlignJustify; break;
    default: align = false; break;
    }
    if (align)
    {
        uint32_t keep = font.Align & ~(uint32_t)(UIFont::AlignLeft | UIFont::AlignRight | UIFont::AlignCenter | UIFont::AlignJustify);
        font.Align = keep | horizontal;
    }
    if (style.TextOverflow.Value == UI_CSS_TEXT_OVERFLOW_ELLIPSIS) font.Ellipsize = UIFont::EllipsizeEnd;
    switch (style.WhiteSpace.Value)
    {
    case UI_CSS_WHITE_SPACE_NOWRAP:
    case UI_CSS_WHITE_SPACE_PRE: font.LineWrap = false; break;
    case UI_CSS_WHITE_SPACE_PRE_WRAP:
    case UI_CSS_WHITE_SPACE_BREAK_SPACES:
    case UI_CSS_WHITE_SPACE_PRE_LINE: font.LineWrap = true; break;
    default: break;
    }
}

void UIWidget::writeFrame(UIWidgetStyle& style, UIPen const& pen, UIBrush const& brush)
{
    if (pen.Style == UIPen::NoPen)
    {
        style.BorderColor = {};
        style.Border.Left = {};
        style.Border.Top = {};
        style.Border.Right = {};
        style.Border.Bottom = {};
    }
    else
    {
        writeColor(style.BorderColor, pen.Color);
        style.Border.Left = { pen.Width, UI_CSS_BORDER_LENGTH };
        style.Border.Top = { pen.Width, UI_CSS_BORDER_LENGTH };
        style.Border.Right = { pen.Width, UI_CSS_BORDER_LENGTH };
        style.Border.Bottom = { pen.Width, UI_CSS_BORDER_LENGTH };
    }
    if (brush.Style == UIBrush::NoBrush) style.Background.Color = {};
    else writeColor(style.Background.Color, brush.Color);
}

void UIWidget::readFrame(UIWidgetStyle const& style, UIPen& pen, UIBrush& brush) const
{
    auto border = readColor(style.BorderColor);
    float width = 0.0f;
    if (style.Border.Top.Type == UI_CSS_BORDER_LENGTH) width = style.Border.Top.Value;
    else if (style.Border.Top.Type == UI_CSS_BORDER_THIN) width = 1.0f;
    else if (style.Border.Top.Type == UI_CSS_BORDER_MEDIUM) width = 3.0f;
    else if (style.Border.Top.Type == UI_CSS_BORDER_THICK) width = 5.0f;
    if (border.A <= 0.0f || width <= 0.0f) pen = UINoPen;
    else pen = { .Color = border, .Width = width };
    auto fill = readColor(style.Background.Color);
    if (fill.A <= 0.0f) brush = UINoBrush;
    else brush = { .Color = fill };
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
    UIString property = name;
    UIString state;
    auto split = name.find(':');
    if (split != UIString::npos)
    {
        property = name.substr(0, split);
        state = name.substr(split + 1);
    }
    UIWidgetStyle* bucket = nullptr;
    switch (UIHash(state))
    {
    case UIHash(""):
    case UIHash("normal"): bucket = &PRIVATE()->Normal; break;
    case UIHash("hover"): bucket = &PRIVATE()->Hover; break;
    case UIHash("active"):
    case UIHash("press"): bucket = &PRIVATE()->Active; break;
    case UIHash("disabled"):
    case UIHash("disable"): bucket = &PRIVATE()->Disabled; break;
    case UIHash("checked"): bucket = &PRIVATE()->Checked; break;
    default: return getStyles()->setStyleText(name, value);
    }
    auto& style = *bucket;
    auto group = [&](char const* prop) -> UIString
    {
        if (state.empty()) return prop;
        return UIString(prop) + ":" + state;
    };
    switch (UIHash(property))
    {
    case UIHash("align-content"):
        setEmbedStyle(name, style.AlignContent);
        break;
    case UIHash("align-items"):
        setEmbedStyle(name, style.AlignItems);
        break;
    case UIHash("align-self"):
        setEmbedStyle(name, style.AlignSelf);
        break;
    case UIHash("alignment-baseline"): setStyle<UIPropAlignmentBaseline>(name);
        break;
    case UIHash("baseline-shift"): setStyle<UIPropBaselineShift>(name);
        break;
    case UIHash("baseline-source"): setStyle<UIPropBaselineSource>(name);
        break;
    case UIHash("border"):
        setEmbedStyle(name, style.Border);
        break;
    case UIHash("border-bottom"):
        setEmbedStyle(group("border"), style.Border);
        setEmbedStyle(name, style.Border.Bottom);
        break;
    case UIHash("border-left"):
        setEmbedStyle(group("border"), style.Border);
        setEmbedStyle(name, style.Border.Left);
        break;
    case UIHash("border-right"):
        setEmbedStyle(group("border"), style.Border);
        setEmbedStyle(name, style.Border.Right);
        break;
    case UIHash("border-top"):
        setEmbedStyle(group("border"), style.Border);
        setEmbedStyle(name, style.Border.Top);
        break;
    case UIHash("border-color"):
        setEmbedStyle(name, style.BorderColor);
        break;
    case UIHash("border-width"):
        setEmbedStyle(group("border"), style.Border);
        setEmbedStyle(name, style.Border);
        break;
    case UIHash("border-radius"):
        setEmbedStyle(name, style.BorderRadius);
        break;
    case UIHash("border-top-left-radius"):
        setEmbedStyle(group("border-radius"), style.BorderRadius);
        setEmbedStyle(name, style.BorderRadius.TopLeft);
        break;
    case UIHash("border-top-right-radius"):
        setEmbedStyle(group("border-radius"), style.BorderRadius);
        setEmbedStyle(name, style.BorderRadius.TopRight);
        break;
    case UIHash("border-bottom-right-radius"):
        setEmbedStyle(group("border-radius"), style.BorderRadius);
        setEmbedStyle(name, style.BorderRadius.BottomRight);
        break;
    case UIHash("border-bottom-left-radius"):
        setEmbedStyle(group("border-radius"), style.BorderRadius);
        setEmbedStyle(name, style.BorderRadius.BottomLeft);
        break;
    case UIHash("bottom"):
        setEmbedStyle(name, style.Bottom);
        break;
    case UIHash("box-sizing"):
        setEmbedStyle(name, style.BoxSizing);
        break;
    case UIHash("background"):
        setEmbedStyle(name, style.Background);
        setEmbedStyle(group("background-color"), style.Background.Color);
        break;
    case UIHash("background-color"):
        setEmbedStyle(group("background"), style.Background);
        setEmbedStyle(name, style.Background.Color);
        break;
    case UIHash("clear"): setStyle<UIPropClear>(name);
        break;
    case UIHash("color"):
        setEmbedStyle(group("foreground"), style.Foreground, true);
        setEmbedStyle(name, style.Foreground.Color, true);
        break;
    case UIHash("direction"):
        setEmbedStyle(name, style.Direction, true);
        break;
    case UIHash("display"):
        setEmbedStyle(name, style.Display);
        break;
    //case UIHash("dominant-baseline"): setStyle<UIPropDominantBaseline>(name); break;
    case UIHash("filter"): setStyle<UIPropFilter>(name);
        break;
    case UIHash("foreground"):
        setEmbedStyle(name, style.Foreground, true);
        setEmbedStyle(group("color"), style.Foreground.Color, true);
        break;
    case UIHash("flex"):
        setEmbedStyle(group("flex"), style.Flex);
        setEmbedStyle(group("flex-grow"), style.Flex.Grow);
        setEmbedStyle(group("flex-shrink"), style.Flex.Shrink);
        setEmbedStyle(group("flex-basis"), style.Flex.Basis);
        break;
    case UIHash("flex-basis"):
        setEmbedStyle(group("flex"), style.Flex);
        setEmbedStyle(name, style.Flex.Basis);
        break;
    case UIHash("flex-direction"):
        setEmbedStyle(name, style.FlexDirection);
        break;
    case UIHash("flex-grow"):
        setEmbedStyle(group("flex"), style.Flex);
        setEmbedStyle(name, style.Flex.Grow);
        break;
    case UIHash("flex-shrink"):
        setEmbedStyle(group("flex"), style.Flex);
        setEmbedStyle(name, style.Flex.Shrink);
        break;
    case UIHash("flex-wrap"):
        setEmbedStyle(name, style.FlexWrap);
        break;
    case UIHash("gap"):
        setEmbedStyle(group("gap"), style.Gap);
        setEmbedStyle(group("row-gap"), style.Gap.Row);
        setEmbedStyle(group("column-gap"), style.Gap.Column);
        break;
    case UIHash("column-gap"):
    case UIHash("grid-column-gap"):
        setEmbedStyle(group("gap"), style.Gap);
        setEmbedStyle(group("column-gap"), style.Gap.Column);
        name = group("column-gap");
        break;
    case UIHash("row-gap"):
    case UIHash("grid-row-gap"):
        setEmbedStyle(group("gap"), style.Gap);
        setEmbedStyle(group("row-gap"), style.Gap.Row);
        name = group("row-gap");
        break;
    case UIHash("grid-template-columns"):
        setEmbedStyle(name, style.GridTemplateColumns);
        break;
    case UIHash("grid-template-rows"):
        setEmbedStyle(name, style.GridTemplateRows);
        break;
    case UIHash("grid-auto-columns"):
        setEmbedStyle(name, style.GridAutoColumns);
        break;
    case UIHash("grid-auto-rows"):
        setEmbedStyle(name, style.GridAutoRows);
        break;
    case UIHash("grid-column-start"):
        setEmbedStyle(group("grid-column"), style.GridColumn);
        setEmbedStyle(name, style.GridColumn.Start);
        break;
    case UIHash("grid-column-end"):
        setEmbedStyle(group("grid-column"), style.GridColumn);
        setEmbedStyle(name, style.GridColumn.End);
        break;
    case UIHash("grid-row-start"):
        setEmbedStyle(group("grid-row"), style.GridRow);
        setEmbedStyle(name, style.GridRow.Start);
        break;
    case UIHash("grid-row-end"):
        setEmbedStyle(group("grid-row"), style.GridRow);
        setEmbedStyle(name, style.GridRow.End);
        break;
    case UIHash("grid-column"):
        setEmbedStyle(group("grid-column"), style.GridColumn);
        setEmbedStyle(group("grid-column-start"), style.GridColumn.Start);
        setEmbedStyle(group("grid-column-end"), style.GridColumn.End);
        break;
    case UIHash("grid-row"):
        setEmbedStyle(group("grid-row"), style.GridRow);
        setEmbedStyle(group("grid-row-start"), style.GridRow.Start);
        setEmbedStyle(group("grid-row-end"), style.GridRow.End);
        break;
    case UIHash("justify-items"):
        setEmbedStyle(name, style.JustifyItems);
        break;
    case UIHash("justify-self"):
        setEmbedStyle(name, style.JustifySelf);
        break;
    case UIHash("aspect-ratio"):
        setEmbedStyle(name, style.AspectRatio);
        break;
    case UIHash("overflow"):
        setEmbedStyle(group("overflow"), style.Overflow);
        setEmbedStyle(group("overflow-x"), style.Overflow.X);
        setEmbedStyle(group("overflow-y"), style.Overflow.Y);
        break;
    //case UIHash("float"): setStyle<UIPropFloat>(name); break;
    //case UIHash("float-defer"): setStyle<UIPropFloatDefer>(name); break;
    case UIHash("float-offset"): setStyle<UIPropFloatOffset>(name);
        break;
    //case UIHash("float-reference"): setStyle<UIPropFloatReference>(name); break;
    case UIHash("font-family"):
        setEmbedStyle(name, style.FontFamily, true);
        break;
    case UIHash("font-size"):
        setEmbedStyle(name, style.FontSize, true);
        break;
    case UIHash("font-stretch"):
        setEmbedStyle(name, style.FontStretch, true);
        break;
    case UIHash("font-style"):
        setEmbedStyle(name, style.FontStyle, true);
        break;
    case UIHash("font-weight"):
        setEmbedStyle(name, style.FontWeight, true);
        break;
    //case UIHash("hanging-punctuation"): setStyle<UIPropHangingPunctuation>(name); break;
    case UIHash("height"):
        setEmbedStyle(name, style.Height);
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
        setEmbedStyle(name, style.JustifyContent);
        break;
    case UIHash("left"):
        setEmbedStyle(name, style.Left);
        break;
    case UIHash("letter-spacing"):
        setEmbedStyle(name, style.LetterSpacing, true);
        break;
    //case UIHash("line-break"): setStyle<UIPropLineBreak>(name); break;
    case UIHash("line-height"):
        setEmbedStyle(name, style.LineHeight, true);
        break;
    case UIHash("margin"):
        setEmbedStyle(name, style.Margin);
        break;
    case UIHash("margin-bottom"):
        setEmbedStyle(group("margin"), style.Margin);
        setEmbedStyle(name, style.Margin.Bottom);
        break;
    case UIHash("margin-left"):
        setEmbedStyle(group("margin"), style.Margin);
        setEmbedStyle(name, style.Margin.Left);
        break;
    case UIHash("margin-right"):
        setEmbedStyle(group("margin"), style.Margin);
        setEmbedStyle(name, style.Margin.Right);
        break;
    case UIHash("margin-top"):
        setEmbedStyle(group("margin"), style.Margin);
        setEmbedStyle(name, style.Margin.Top);
        break;
    case UIHash("max-height"):
        setEmbedStyle(name, style.MaxHeight);
        break;
    case UIHash("max-width"):
        setEmbedStyle(name, style.MaxWidth);
        break;
    case UIHash("min-height"):
        setEmbedStyle(name, style.MinHeight);
        break;
    case UIHash("min-width"):
        setEmbedStyle(name, style.MinWidth);
        break;
    //case UIHash("opacity"): setStyle<UIPropOpacity>(name); break;
    //case UIHash("order"): setStyle<UIPropOrder>(name); break;
    //case UIHash("overflow-block"): setStyle<UIPropOverflowBlock>(name); break;
    //case UIHash("overflow-inline"): setStyle<UIPropOverflowInline>(name); break;
    //case UIHash("overflow-wrap"): setStyle<UIPropOverflowWrap>(name); break;
    case UIHash("overflow-x"):
        setEmbedStyle(group("overflow"), style.Overflow);
        setEmbedStyle(name, style.Overflow.X);
        break;
    case UIHash("overflow-y"):
        setEmbedStyle(group("overflow"), style.Overflow);
        setEmbedStyle(name, style.Overflow.Y);
        break;
    case UIHash("padding"):
        setEmbedStyle(name, style.Padding);
        break;
    case UIHash("padding-bottom"):
        setEmbedStyle(group("padding"), style.Padding);
        setEmbedStyle(name, style.Padding.Bottom);
        break;
    case UIHash("padding-left"):
        setEmbedStyle(group("padding"), style.Padding);
        setEmbedStyle(name, style.Padding.Left);
        break;
    case UIHash("padding-right"):
        setEmbedStyle(group("padding"), style.Padding);
        setEmbedStyle(name, style.Padding.Right);
        break;
    case UIHash("padding-top"):
        setEmbedStyle(group("padding"), style.Padding);
        setEmbedStyle(name, style.Padding.Top);
        break;
    case UIHash("position"):
        setEmbedStyle(name, style.Position);
        break;
    case UIHash("right"):
        setEmbedStyle(name, style.Right);
        break;
    case UIHash("tab-size"): setStyle<UIPropTabSize>(name);
        break;
    case UIHash("text-align"):
        setEmbedStyle(name, style.TextAlign, true);
        break;
    //case UIHash("text-align-all"): setStyle<UIPropTextAlignAll>(name); break;
    //case UIHash("text-align-last"): setStyle<UIPropTextAlignLast>(name); break;
    //case UIHash("text-combine-upright"): setStyle<UIPropTextCombineUpright>(name); break;
    //case UIHash("text-decoration-line"): setStyle<UIPropTextDecorationLine>(name); break;
    //case UIHash("text-decoration-style"): setStyle<UIPropTextDecorationStyle>(name); break;
    case UIHash("text-indent"):
        setEmbedStyle(name, style.TextIndent, true);
        break;
    //case UIHash("text-justify"): setStyle<UIPropTextJustify>(name); break;
    //case UIHash("text-orientation"): setStyle<UIPropTextOrientation>(name); break;
    case UIHash("text-overflow"):
        setEmbedStyle(name, style.TextOverflow);
        break;
    //case UIHash("text-transform"): setStyle<UIPropTextTransform>(name, {}, true); break;
    case UIHash("top"):
        setEmbedStyle(name, style.Top);
        break;
    //case UIHash("unicode-bidi"): setStyle<UIPropUnicodeBidi>(name); break;
    //case UIHash("vertical-align"): setStyle<UIPropVerticalAlign>(name); break;
    case UIHash("visibility"): setStyle<UIPropVisibility>(name, {}, true);
        break;
    case UIHash("white-space"):
        setEmbedStyle(name, style.WhiteSpace, true);
        break;
    case UIHash("width"):
        setEmbedStyle(name, style.Width);
        break;
    //case UIHash("word-break"): setStyle<UIPropWordBreak>(name); break;
    case UIHash("word-spacing"):
        setEmbedStyle(name, style.WordSpacing, true);
        break;
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
