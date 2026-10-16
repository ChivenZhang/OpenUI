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
#include <optional>
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
    std::optional<UIPropDisplay> Display;
    std::optional<UIPropDirection> Direction;
    std::optional<UIPropBoxSizing> BoxSizing;
    std::optional<UIPropPosition> Position;
    std::optional<UIPropLeft> Left;
    std::optional<UIPropTop> Top;
    std::optional<UIPropRight> Right;
    std::optional<UIPropBottom> Bottom;
    std::optional<UIPropWidth> Width;
    std::optional<UIPropHeight> Height;
    std::optional<UIPropMinWidth> MinWidth;
    std::optional<UIPropMinHeight> MinHeight;
    std::optional<UIPropMaxWidth> MaxWidth;
    std::optional<UIPropMaxHeight> MaxHeight;
    std::optional<UIPropBorder> Border;
    std::optional<UIPropMargin> Margin;
    std::optional<UIPropPadding> Padding;
    std::optional<UIPropFlexDirection> FlexDirection;
    std::optional<UIPropFlexWrap> FlexWrap;
    std::optional<UIPropJustifyContent> JustifyContent;
    std::optional<UIPropAlignItems> AlignItems;
    std::optional<UIPropAlignContent> AlignContent;
    std::optional<UIPropAlignSelf> AlignSelf;
    std::optional<UIPropJustifyItems> JustifyItems;
    std::optional<UIPropJustifySelf> JustifySelf;
    std::optional<UIPropFlex> Flex;
    std::optional<UIPropAspectRatio> AspectRatio;
    std::optional<UIPropOverflow> Overflow;
    std::optional<UIPropGridTemplateColumns> GridTemplateColumns;
    std::optional<UIPropGridTemplateRows> GridTemplateRows;
    std::optional<UIPropGridAutoColumns> GridAutoColumns;
    std::optional<UIPropGridAutoRows> GridAutoRows;
    std::optional<UIPropGridColumn> GridColumn;
    std::optional<UIPropGridRow> GridRow;
    std::optional<UIPropGap> Gap;

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
    if (PRIVATE()->Display) return *PRIVATE()->Display;
    return {};
}

void UIWidget::setDisplayType(UIPropDisplay value)
{
    PRIVATE()->Display = value;
}

UIPropPosition UIWidget::getPositionType() const
{
    if (PRIVATE()->Position) return *PRIVATE()->Position;
    return {};
}

void UIWidget::setPositionType(UIPropPosition value)
{
    PRIVATE()->Position = value;
}

UIPropLeft UIWidget::getFixedPosX() const
{
    if (PRIVATE()->Left) return *PRIVATE()->Left;
    return {};
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
    if (PRIVATE()->Top) return *PRIVATE()->Top;
    return {};
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
    if (PRIVATE()->MinWidth) return *PRIVATE()->MinWidth;
    return {};
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
    if (PRIVATE()->MaxWidth) return *PRIVATE()->MaxWidth;
    return {};
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
    if (PRIVATE()->Width) return *PRIVATE()->Width;
    return {};
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
    if (PRIVATE()->MinHeight) return *PRIVATE()->MinHeight;
    return {};
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
    if (PRIVATE()->MaxHeight) return *PRIVATE()->MaxHeight;
    return {};
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
    if (PRIVATE()->Height) return *PRIVATE()->Height;
    return {};
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
    if (PRIVATE()->Border) return PRIVATE()->Border->Top;
    return {};
}

void UIWidget::setBorderTop(UIPropBorderTop value)
{
    if (!PRIVATE()->Border) PRIVATE()->Border.emplace();
    PRIVATE()->Border->Top = value;
}

UIPropBorderBottom UIWidget::getBorderBottom() const
{
    if (PRIVATE()->Border) return PRIVATE()->Border->Bottom;
    return {};
}

void UIWidget::setBorderBottom(UIPropBorderBottom value)
{
    if (!PRIVATE()->Border) PRIVATE()->Border.emplace();
    PRIVATE()->Border->Bottom = value;
}

UIPropBorderLeft UIWidget::getBorderLeft() const
{
    if (PRIVATE()->Border) return PRIVATE()->Border->Left;
    return {};
}

void UIWidget::setBorderLeft(UIPropBorderLeft value)
{
    if (!PRIVATE()->Border) PRIVATE()->Border.emplace();
    PRIVATE()->Border->Left = value;
}

UIPropBorderRight UIWidget::getBorderRight() const
{
    if (PRIVATE()->Border) return PRIVATE()->Border->Right;
    return {};
}

void UIWidget::setBorderRight(UIPropBorderRight value)
{
    if (!PRIVATE()->Border) PRIVATE()->Border.emplace();
    PRIVATE()->Border->Right = value;
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
    if (PRIVATE()->Margin) return PRIVATE()->Margin->Top;
    return {};
}

void UIWidget::setMarginTop(UIPropMarginTop value)
{
    if (!PRIVATE()->Margin) PRIVATE()->Margin.emplace();
    PRIVATE()->Margin->Top = value;
}

UIPropMarginBottom UIWidget::getMarginBottom() const
{
    if (PRIVATE()->Margin) return PRIVATE()->Margin->Bottom;
    return {};
}

void UIWidget::setMarginBottom(UIPropMarginBottom value)
{
    if (!PRIVATE()->Margin) PRIVATE()->Margin.emplace();
    PRIVATE()->Margin->Bottom = value;
}

UIPropMarginLeft UIWidget::getMarginLeft() const
{
    if (PRIVATE()->Margin) return PRIVATE()->Margin->Left;
    return {};
}

void UIWidget::setMarginLeft(UIPropMarginLeft value)
{
    if (!PRIVATE()->Margin) PRIVATE()->Margin.emplace();
    PRIVATE()->Margin->Left = value;
}

UIPropMarginRight UIWidget::getMarginRight() const
{
    if (PRIVATE()->Margin) return PRIVATE()->Margin->Right;
    return {};
}

void UIWidget::setMarginRight(UIPropMarginRight value)
{
    if (!PRIVATE()->Margin) PRIVATE()->Margin.emplace();
    PRIVATE()->Margin->Right = value;
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
    if (PRIVATE()->Padding) return PRIVATE()->Padding->Top;
    return {};
}

void UIWidget::setPaddingTop(UIPropPaddingTop value)
{
    if (!PRIVATE()->Padding) PRIVATE()->Padding.emplace();
    PRIVATE()->Padding->Top = value;
}

UIPropPaddingBottom UIWidget::getPaddingBottom() const
{
    if (PRIVATE()->Padding) return PRIVATE()->Padding->Bottom;
    return {};
}

void UIWidget::setPaddingBottom(UIPropPaddingBottom value)
{
    if (!PRIVATE()->Padding) PRIVATE()->Padding.emplace();
    PRIVATE()->Padding->Bottom = value;
}

UIPropPaddingLeft UIWidget::getPaddingLeft() const
{
    if (PRIVATE()->Padding) return PRIVATE()->Padding->Left;
    return {};
}

void UIWidget::setPaddingLeft(UIPropPaddingLeft value)
{
    if (!PRIVATE()->Padding) PRIVATE()->Padding.emplace();
    PRIVATE()->Padding->Left = value;
}

UIPropPaddingRight UIWidget::getPaddingRight() const
{
    if (PRIVATE()->Padding) return PRIVATE()->Padding->Right;
    return {};
}

void UIWidget::setPaddingRight(UIPropPaddingRight value)
{
    if (!PRIVATE()->Padding) PRIVATE()->Padding.emplace();
    PRIVATE()->Padding->Right = value;
}

UIPropFlexDirection UIWidget::getFlexDirection() const
{
    if (PRIVATE()->FlexDirection) return *PRIVATE()->FlexDirection;
    return {};
}

void UIWidget::setFlexDirection(UIPropFlexDirection value)
{
    PRIVATE()->FlexDirection = value;
}

UIPropFlexWrap UIWidget::getFlexWrap() const
{
    if (PRIVATE()->FlexWrap) return *PRIVATE()->FlexWrap;
    return {};
}

void UIWidget::setFlexWrap(UIPropFlexWrap value)
{
    PRIVATE()->FlexWrap = value;
}

UIPropJustifyContent UIWidget::getJustifyContent() const
{
    if (PRIVATE()->JustifyContent) return *PRIVATE()->JustifyContent;
    return {};
}

void UIWidget::setJustifyContent(UIPropJustifyContent value)
{
    PRIVATE()->JustifyContent = value;
}

UIPropAlignItems UIWidget::getAlignItems() const
{
    if (PRIVATE()->AlignItems) return *PRIVATE()->AlignItems;
    return {};
}

void UIWidget::setAlignItems(UIPropAlignItems value)
{
    PRIVATE()->AlignItems = value;
}

UIPropAlignContent UIWidget::getAlignContent() const
{
    if (PRIVATE()->AlignContent) return *PRIVATE()->AlignContent;
    return {};
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
    if (PRIVATE()->Flex) return PRIVATE()->Flex->Grow;
    return {};
}

void UIWidget::setFlexGrow(UIPropFlexGrow value)
{
    if (!PRIVATE()->Flex) PRIVATE()->Flex.emplace();
    PRIVATE()->Flex->Grow = value;
}

UIPropFlexShrink UIWidget::getFlexShrink() const
{
    if (PRIVATE()->Flex) return PRIVATE()->Flex->Shrink;
    return {};
}

void UIWidget::setFlexShrink(UIPropFlexShrink value)
{
    if (!PRIVATE()->Flex) PRIVATE()->Flex.emplace();
    PRIVATE()->Flex->Shrink = value;
}

UIPropFlexBasis UIWidget::getFlexBasis() const
{
    if (PRIVATE()->Flex) return PRIVATE()->Flex->Basis;
    return {};
}

void UIWidget::setFlexBasis(UIPropFlexBasis value)
{
    if (!PRIVATE()->Flex) PRIVATE()->Flex.emplace();
    PRIVATE()->Flex->Basis = value;
}

UIPropAlignSelf UIWidget::getAlignSelf() const
{
    if (PRIVATE()->AlignSelf) return *PRIVATE()->AlignSelf;
    return {};
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
    if (PRIVATE()->GridTemplateColumns) return *PRIVATE()->GridTemplateColumns;
    return {};
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
    if (PRIVATE()->GridTemplateRows) return *PRIVATE()->GridTemplateRows;
    return {};
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
    if (PRIVATE()->GridAutoColumns) return *PRIVATE()->GridAutoColumns;
    return {};
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
    if (PRIVATE()->GridAutoRows) return *PRIVATE()->GridAutoRows;
    return {};
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
    if (PRIVATE()->GridColumn) return PRIVATE()->GridColumn->Start;
    return {};
}

void UIWidget::setGridColumnStart(UIPropGridColumnStart value)
{
    if (!PRIVATE()->GridColumn) PRIVATE()->GridColumn.emplace();
    PRIVATE()->GridColumn->Start = value;
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
    if (PRIVATE()->GridColumn) return PRIVATE()->GridColumn->End;
    return {};
}

void UIWidget::setGridColumnEnd(UIPropGridColumnEnd value)
{
    if (!PRIVATE()->GridColumn) PRIVATE()->GridColumn.emplace();
    PRIVATE()->GridColumn->End = value;
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
    if (PRIVATE()->GridRow) return PRIVATE()->GridRow->Start;
    return {};
}

void UIWidget::setGridRowStart(UIPropGridRowStart value)
{
    if (!PRIVATE()->GridRow) PRIVATE()->GridRow.emplace();
    PRIVATE()->GridRow->Start = value;
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
    if (PRIVATE()->GridRow) return PRIVATE()->GridRow->End;
    return {};
}

void UIWidget::setGridRowEnd(UIPropGridRowEnd value)
{
    if (!PRIVATE()->GridRow) PRIVATE()->GridRow.emplace();
    PRIVATE()->GridRow->End = value;
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
    if (PRIVATE()->Gap) return PRIVATE()->Gap->Column;
    return {};
}

void UIWidget::setGridColumnGap(UIPropGridColumnGap value)
{
    if (!PRIVATE()->Gap) PRIVATE()->Gap.emplace();
    PRIVATE()->Gap->Column = value;
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
    if (PRIVATE()->Gap) return PRIVATE()->Gap->Row;
    return {};
}

void UIWidget::setGridRowGap(UIPropGridRowGap value)
{
    if (!PRIVATE()->Gap) PRIVATE()->Gap.emplace();
    PRIVATE()->Gap->Row = value;
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
    default: setStyle(name, value);
        break;
    case UIHash("align-content"):
        if (!PRIVATE()->AlignContent) PRIVATE()->AlignContent.emplace();
        setEmbedStyle(name, *PRIVATE()->AlignContent);
        break;
    case UIHash("align-items"):
        if (!PRIVATE()->AlignItems) PRIVATE()->AlignItems.emplace();
        setEmbedStyle(name, *PRIVATE()->AlignItems);
        break;
    case UIHash("align-self"):
        if (!PRIVATE()->AlignSelf) PRIVATE()->AlignSelf.emplace();
        setEmbedStyle(name, *PRIVATE()->AlignSelf);
        break;
    case UIHash("alignment-baseline"): setStyle<UIPropAlignmentBaseline>(name);
        break;
    case UIHash("baseline-shift"): setStyle<UIPropBaselineShift>(name);
        break;
    case UIHash("baseline-source"): setStyle<UIPropBaselineSource>(name);
        break;
    case UIHash("border"):
        if (!PRIVATE()->Border) PRIVATE()->Border.emplace();
        setEmbedStyle(name, *PRIVATE()->Border);
        break;
    case UIHash("border-bottom"):
        if (!PRIVATE()->Border) PRIVATE()->Border.emplace();
        setEmbedStyle("border", *PRIVATE()->Border);
        setEmbedStyle(name, PRIVATE()->Border->Bottom);
        break;
    case UIHash("border-left"):
        if (!PRIVATE()->Border) PRIVATE()->Border.emplace();
        setEmbedStyle("border", *PRIVATE()->Border);
        setEmbedStyle(name, PRIVATE()->Border->Left);
        break;
    case UIHash("border-right"):
        if (!PRIVATE()->Border) PRIVATE()->Border.emplace();
        setEmbedStyle("border", *PRIVATE()->Border);
        setEmbedStyle(name, PRIVATE()->Border->Right);
        break;
    case UIHash("border-top"):
        if (!PRIVATE()->Border) PRIVATE()->Border.emplace();
        setEmbedStyle("border", *PRIVATE()->Border);
        setEmbedStyle(name, PRIVATE()->Border->Top);
        break;
    case UIHash("bottom"):
        if (!PRIVATE()->Bottom) PRIVATE()->Bottom.emplace();
        setEmbedStyle(name, *PRIVATE()->Bottom);
        break;
    case UIHash("box-sizing"):
        if (!PRIVATE()->BoxSizing) PRIVATE()->BoxSizing.emplace();
        setEmbedStyle(name, *PRIVATE()->BoxSizing);
        break;
    case UIHash("clear"): setStyle<UIPropClear>(name);
        break;
    case UIHash("color"): setStyle<UIPropColor>(name, {}, true);
        break;
    case UIHash("direction"):
        if (!PRIVATE()->Direction) PRIVATE()->Direction.emplace();
        setEmbedStyle(name, *PRIVATE()->Direction, true);
        break;
    case UIHash("display"):
        if (!PRIVATE()->Display) PRIVATE()->Display.emplace();
        setEmbedStyle(name, *PRIVATE()->Display);
        break;
    //case UIHash("dominant-baseline"): setStyle<UIPropDominantBaseline>(name); break;
    case UIHash("filter"): setStyle<UIPropFilter>(name);
        break;
    case UIHash("flex"):
        if (!PRIVATE()->Flex) PRIVATE()->Flex.emplace();
        setEmbedStyle("flex", *PRIVATE()->Flex);
        setEmbedStyle("flex-grow", PRIVATE()->Flex->Grow);
        setEmbedStyle("flex-shrink", PRIVATE()->Flex->Shrink);
        setEmbedStyle("flex-basis", PRIVATE()->Flex->Basis);
        break;
    case UIHash("flex-basis"):
        if (!PRIVATE()->Flex) PRIVATE()->Flex.emplace();
        setEmbedStyle("flex", *PRIVATE()->Flex);
        setEmbedStyle(name, PRIVATE()->Flex->Basis);
        break;
    case UIHash("flex-direction"):
        if (!PRIVATE()->FlexDirection) PRIVATE()->FlexDirection.emplace();
        setEmbedStyle(name, *PRIVATE()->FlexDirection);
        break;
    case UIHash("flex-grow"):
        if (!PRIVATE()->Flex) PRIVATE()->Flex.emplace();
        setEmbedStyle("flex", *PRIVATE()->Flex);
        setEmbedStyle(name, PRIVATE()->Flex->Grow);
        break;
    case UIHash("flex-shrink"):
        if (!PRIVATE()->Flex) PRIVATE()->Flex.emplace();
        setEmbedStyle("flex", *PRIVATE()->Flex);
        setEmbedStyle(name, PRIVATE()->Flex->Shrink);
        break;
    case UIHash("flex-wrap"):
        if (!PRIVATE()->FlexWrap) PRIVATE()->FlexWrap.emplace();
        setEmbedStyle(name, *PRIVATE()->FlexWrap);
        break;
    case UIHash("gap"):
        if (!PRIVATE()->Gap) PRIVATE()->Gap.emplace();
        setEmbedStyle("gap", *PRIVATE()->Gap);
        setEmbedStyle("row-gap", PRIVATE()->Gap->Row);
        setEmbedStyle("column-gap", PRIVATE()->Gap->Column);
        break;
    case UIHash("column-gap"):
    case UIHash("grid-column-gap"):
        if (!PRIVATE()->Gap) PRIVATE()->Gap.emplace();
        setEmbedStyle("gap", *PRIVATE()->Gap);
        setEmbedStyle("column-gap", PRIVATE()->Gap->Column);
        name = "column-gap";
        break;
    case UIHash("row-gap"):
    case UIHash("grid-row-gap"):
        if (!PRIVATE()->Gap) PRIVATE()->Gap.emplace();
        setEmbedStyle("gap", *PRIVATE()->Gap);
        setEmbedStyle("row-gap", PRIVATE()->Gap->Row);
        name = "row-gap";
        break;
    case UIHash("grid-template-columns"):
        if (!PRIVATE()->GridTemplateColumns) PRIVATE()->GridTemplateColumns.emplace();
        setEmbedStyle(name, *PRIVATE()->GridTemplateColumns);
        break;
    case UIHash("grid-template-rows"):
        if (!PRIVATE()->GridTemplateRows) PRIVATE()->GridTemplateRows.emplace();
        setEmbedStyle(name, *PRIVATE()->GridTemplateRows);
        break;
    case UIHash("grid-auto-columns"):
        if (!PRIVATE()->GridAutoColumns) PRIVATE()->GridAutoColumns.emplace();
        setEmbedStyle(name, *PRIVATE()->GridAutoColumns);
        break;
    case UIHash("grid-auto-rows"):
        if (!PRIVATE()->GridAutoRows) PRIVATE()->GridAutoRows.emplace();
        setEmbedStyle(name, *PRIVATE()->GridAutoRows);
        break;
    case UIHash("grid-column-start"):
        if (!PRIVATE()->GridColumn) PRIVATE()->GridColumn.emplace();
        setEmbedStyle("grid-column", *PRIVATE()->GridColumn);
        setEmbedStyle(name, PRIVATE()->GridColumn->Start);
        break;
    case UIHash("grid-column-end"):
        if (!PRIVATE()->GridColumn) PRIVATE()->GridColumn.emplace();
        setEmbedStyle("grid-column", *PRIVATE()->GridColumn);
        setEmbedStyle(name, PRIVATE()->GridColumn->End);
        break;
    case UIHash("grid-row-start"):
        if (!PRIVATE()->GridRow) PRIVATE()->GridRow.emplace();
        setEmbedStyle("grid-row", *PRIVATE()->GridRow);
        setEmbedStyle(name, PRIVATE()->GridRow->Start);
        break;
    case UIHash("grid-row-end"):
        if (!PRIVATE()->GridRow) PRIVATE()->GridRow.emplace();
        setEmbedStyle("grid-row", *PRIVATE()->GridRow);
        setEmbedStyle(name, PRIVATE()->GridRow->End);
        break;
    case UIHash("grid-column"):
        if (!PRIVATE()->GridColumn) PRIVATE()->GridColumn.emplace();
        setEmbedStyle("grid-column", *PRIVATE()->GridColumn);
        setEmbedStyle("grid-column-start", PRIVATE()->GridColumn->Start);
        setEmbedStyle("grid-column-end", PRIVATE()->GridColumn->End);
        break;
    case UIHash("grid-row"):
        if (!PRIVATE()->GridRow) PRIVATE()->GridRow.emplace();
        setEmbedStyle("grid-row", *PRIVATE()->GridRow);
        setEmbedStyle("grid-row-start", PRIVATE()->GridRow->Start);
        setEmbedStyle("grid-row-end", PRIVATE()->GridRow->End);
        break;
    case UIHash("justify-items"):
        if (!PRIVATE()->JustifyItems) PRIVATE()->JustifyItems.emplace();
        setEmbedStyle(name, *PRIVATE()->JustifyItems);
        break;
    case UIHash("justify-self"):
        if (!PRIVATE()->JustifySelf) PRIVATE()->JustifySelf.emplace();
        setEmbedStyle(name, *PRIVATE()->JustifySelf);
        break;
    case UIHash("aspect-ratio"):
        if (!PRIVATE()->AspectRatio) PRIVATE()->AspectRatio.emplace();
        setEmbedStyle(name, *PRIVATE()->AspectRatio);
        break;
    case UIHash("overflow"):
        if (!PRIVATE()->Overflow) PRIVATE()->Overflow.emplace();
        setEmbedStyle("overflow", *PRIVATE()->Overflow);
        setEmbedStyle("overflow-x", PRIVATE()->Overflow->X);
        setEmbedStyle("overflow-y", PRIVATE()->Overflow->Y);
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
        if (!PRIVATE()->Height) PRIVATE()->Height.emplace();
        setEmbedStyle(name, *PRIVATE()->Height);
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
        if (!PRIVATE()->JustifyContent) PRIVATE()->JustifyContent.emplace();
        setEmbedStyle(name, *PRIVATE()->JustifyContent);
        break;
    case UIHash("left"):
        if (!PRIVATE()->Left) PRIVATE()->Left.emplace();
        setEmbedStyle(name, *PRIVATE()->Left);
        break;
    case UIHash("letter-spacing"): setStyle<UIPropLetterSpacing>(name, {}, true);
        break;
    //case UIHash("line-break"): setStyle<UIPropLineBreak>(name); break;
    case UIHash("line-height"): setStyle<UIPropLineHeight>(name, {}, true);
        break;
    case UIHash("margin"):
        if (!PRIVATE()->Margin) PRIVATE()->Margin.emplace();
        setEmbedStyle(name, *PRIVATE()->Margin);
        break;
    case UIHash("margin-bottom"):
        if (!PRIVATE()->Margin) PRIVATE()->Margin.emplace();
        setEmbedStyle("margin", *PRIVATE()->Margin);
        setEmbedStyle(name, PRIVATE()->Margin->Bottom);
        break;
    case UIHash("margin-left"):
        if (!PRIVATE()->Margin) PRIVATE()->Margin.emplace();
        setEmbedStyle("margin", *PRIVATE()->Margin);
        setEmbedStyle(name, PRIVATE()->Margin->Left);
        break;
    case UIHash("margin-right"):
        if (!PRIVATE()->Margin) PRIVATE()->Margin.emplace();
        setEmbedStyle("margin", *PRIVATE()->Margin);
        setEmbedStyle(name, PRIVATE()->Margin->Right);
        break;
    case UIHash("margin-top"):
        if (!PRIVATE()->Margin) PRIVATE()->Margin.emplace();
        setEmbedStyle("margin", *PRIVATE()->Margin);
        setEmbedStyle(name, PRIVATE()->Margin->Top);
        break;
    case UIHash("max-height"):
        if (!PRIVATE()->MaxHeight) PRIVATE()->MaxHeight.emplace();
        setEmbedStyle(name, *PRIVATE()->MaxHeight);
        break;
    case UIHash("max-width"):
        if (!PRIVATE()->MaxWidth) PRIVATE()->MaxWidth.emplace();
        setEmbedStyle(name, *PRIVATE()->MaxWidth);
        break;
    case UIHash("min-height"):
        if (!PRIVATE()->MinHeight) PRIVATE()->MinHeight.emplace();
        setEmbedStyle(name, *PRIVATE()->MinHeight);
        break;
    case UIHash("min-width"):
        if (!PRIVATE()->MinWidth) PRIVATE()->MinWidth.emplace();
        setEmbedStyle(name, *PRIVATE()->MinWidth);
        break;
    //case UIHash("opacity"): setStyle<UIPropOpacity>(name); break;
    //case UIHash("order"): setStyle<UIPropOrder>(name); break;
    //case UIHash("overflow-block"): setStyle<UIPropOverflowBlock>(name); break;
    //case UIHash("overflow-inline"): setStyle<UIPropOverflowInline>(name); break;
    //case UIHash("overflow-wrap"): setStyle<UIPropOverflowWrap>(name); break;
    case UIHash("overflow-x"):
        if (!PRIVATE()->Overflow) PRIVATE()->Overflow.emplace();
        setEmbedStyle("overflow", *PRIVATE()->Overflow);
        setEmbedStyle(name, PRIVATE()->Overflow->X);
        break;
    case UIHash("overflow-y"):
        if (!PRIVATE()->Overflow) PRIVATE()->Overflow.emplace();
        setEmbedStyle("overflow", *PRIVATE()->Overflow);
        setEmbedStyle(name, PRIVATE()->Overflow->Y);
        break;
    case UIHash("padding"):
        if (!PRIVATE()->Padding) PRIVATE()->Padding.emplace();
        setEmbedStyle(name, *PRIVATE()->Padding);
        break;
    case UIHash("padding-bottom"):
        if (!PRIVATE()->Padding) PRIVATE()->Padding.emplace();
        setEmbedStyle("padding", *PRIVATE()->Padding);
        setEmbedStyle(name, PRIVATE()->Padding->Bottom);
        break;
    case UIHash("padding-left"):
        if (!PRIVATE()->Padding) PRIVATE()->Padding.emplace();
        setEmbedStyle("padding", *PRIVATE()->Padding);
        setEmbedStyle(name, PRIVATE()->Padding->Left);
        break;
    case UIHash("padding-right"):
        if (!PRIVATE()->Padding) PRIVATE()->Padding.emplace();
        setEmbedStyle("padding", *PRIVATE()->Padding);
        setEmbedStyle(name, PRIVATE()->Padding->Right);
        break;
    case UIHash("padding-top"):
        if (!PRIVATE()->Padding) PRIVATE()->Padding.emplace();
        setEmbedStyle("padding", *PRIVATE()->Padding);
        setEmbedStyle(name, PRIVATE()->Padding->Top);
        break;
    case UIHash("position"):
        if (!PRIVATE()->Position) PRIVATE()->Position.emplace();
        setEmbedStyle(name, *PRIVATE()->Position);
        break;
    case UIHash("right"):
        if (!PRIVATE()->Right) PRIVATE()->Right.emplace();
        setEmbedStyle(name, *PRIVATE()->Right);
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
        if (!PRIVATE()->Top) PRIVATE()->Top.emplace();
        setEmbedStyle(name, *PRIVATE()->Top);
        break;
    //case UIHash("unicode-bidi"): setStyle<UIPropUnicodeBidi>(name); break;
    //case UIHash("vertical-align"): setStyle<UIPropVerticalAlign>(name); break;
    case UIHash("visibility"): setStyle<UIPropVisibility>(name, {}, true);
        break;
    //case UIHash("white-space"): setStyle<UIPropWhiteSpace>(name, {}, true); break;
    case UIHash("width"):
        if (!PRIVATE()->Width) PRIVATE()->Width.emplace();
        setEmbedStyle(name, *PRIVATE()->Width);
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
    }
    getStyles()->setStyleText(name, value);
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
