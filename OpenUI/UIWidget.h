#pragma once
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
#include "UIAttrib.h"
#include "UIEvent.h"
#include "UIPainter.h"
#include "UISignal.h"
#include "UIStyle.h"

class UICanvas;
using UICanvasRef = UIRef<UICanvas>;
using UICanvasRaw = UIRaw<UICanvas>;

class UIWidget;
using UIWidgetRef = UIRef<UIWidget>;
using UIWidgetRaw = UIRaw<UIWidget>;

class UIWidgetPrivate {};
using UIWidgetPrivateRaw = UIRaw<UIWidgetPrivate>;

/// @brief Base interface of widget
class OPENUI_API UIWidget : public UIReactor, public UIHandler, public UIFilter, public std::enable_shared_from_this<UIWidget>
{
public:
	explicit UIWidget(UICanvasRaw canvas);
	~UIWidget() override;
	UIString getID() const;
	void setID(UIString value);
	UIWidgetRaw getParent() const;
	UIListView<const UIWidgetRef> getWidgets() const;

	virtual UIFilterRaw getEventFilter() const;
	virtual void setEventFilter(UIFilterRaw value);
	virtual bool addWidget(UIWidgetRef value);
	virtual bool removeWidget(UIWidgetRef value);
	virtual void removeWidget();
	virtual UIWidgetRef findWidget(UIString identity) const;
	virtual UIList<UIWidgetRef> findWidgets(UIString identity) const;
	virtual UIWidgetRef findWidget(UILambda<bool(UIWidgetRef)> selector) const;
	virtual UIList<UIWidgetRef> findWidgets(UILambda<bool(UIWidgetRef)> selector) const;

	// ==================================== Arrange ===================================

	virtual void arrange(UIRect client);
	virtual void layout(UIRect client);
	virtual void paint(UIRect client, UIPainterRaw painter);
	virtual void repaint(UIRect client, UIPainterRaw painter);
	virtual bool filter(UIReactorRaw source, UIEventRaw event) override;
	virtual void handle(UIReactorRaw source, UIEventRaw event) final override;

	// ================================== Attrib Query =================================

	bool getEnable() const;
	void setEnable(bool value);
	bool getVisible() const;
	void setVisible(bool value);
	bool getAnimate() const;
	void setAnimate(bool value);
	UIRect getBounds() const;
	void setBounds(UIRect value);
	UIRect getViewport() const;
	void setViewport(UIRect value);
	UIRect getLocalBounds() const;
	void setLocalBounds(UIRect value);
	float getScale() const;
	void setScale(float value);
	float getRotate() const;
	void setRotate(float value);
	UIFloat2 getTranslate() const;
	void setTranslate(UIFloat2 value);
	float getPosX() const;
	float getPosY() const;
	UIFloat2 getPos() const;
	float getLocalX() const;
	float getLocalY() const;
	UIFloat2 getLocalPos() const;
	float getWidth() const;
	float getHeight() const;
	UIFloat2 getSize() const;
	bool inBounds(UIFloat2 pos);
	bool inBounds(float x, float y);

	// ================================= Flex Query ======================================

	UI::DisplayType getDisplayType() const;
	void setDisplayType(UI::DisplayType value);
	UI::PositionType getPositionType() const;
	void setPositionType(UI::PositionType value);
	UIValueF getFixedPosX() const;
	void setFixedPosX(UIValueF value);
	UIValueF getFixedPosY() const;
	void setFixedPosY(UIValueF value);
	UIValue2F getFixedPos() const;
	void setFixedPos(UIValueF left, UIValueF top);
	UIValueF getMinWidth() const;
	void setMinWidth(UIValueF value);
	UIValueF getMaxWidth() const;
	void setMaxWidth(UIValueF value);
	UIValueF getFixedWidth() const;
	void setFixedWidth(UIValueF value);
	UIValueF getMinHeight() const;
	void setMinHeight(UIValueF value);
	UIValueF getMaxHeight() const;
	void setMaxHeight(UIValueF value);
	UIValueF getFixedHeight() const;
	void setFixedHeight(UIValueF value);
	UIValue2F getMinSize() const;
	void setMinSize(UIValueF width, UIValueF height);
	UIValue2F getMaxSize() const;
	void setMaxSize(UIValueF width, UIValueF height);
	UIValue2F getFixedSize() const;
	void setFixedSize(UIValueF width, UIValueF height);

	UIValue4F getBorder() const;
	void setBorder(UIValue4F value);
	UIValue4F getMargin() const;
	void setMargin(UIValue4F value);
	UIValue4F getPadding() const;
	void setPadding(UIValue4F value);
	UIValue2F getSpacing() const;
	void setSpacing(UIValue2F value);

	UI::FlexDirection getFlexDirection() const;
	void setFlexDirection(UI::FlexDirection value);
	UI::FlexWrap getFlexWrap() const;
	void setFlexWrap(UI::FlexWrap value);
	UI::JustifyContent getJustifyContent() const;
	void setJustifyContent(UI::JustifyContent value);
	UI::AlignItems getAlignItems() const;
	void setAlignItems(UI::AlignItems value);
	UI::AlignContent getAlignContent() const;
	void setAlignContent(UI::AlignContent value);
	void setFlexFlow(UI::FlexDirection direction, UI::FlexWrap wrap);

	UI::FlexGrow getFlexGrow() const;
	void setFlexGrow(UI::FlexGrow value);
	UI::FlexShrink getFlexShrink() const;
	void setFlexShrink(UI::FlexShrink value);
	UI::FlexBasis getFlexBasis() const;
	void setFlexBasis(UI::FlexBasis value);
	UI::AlignSelf getAlignSelf() const;
	void setAlignSelf(UI::AlignSelf value);
	void setFlex(UI::FlexGrow grow, UI::FlexShrink shrink, UI::FlexBasis basis);

	// =================================CSS Style======================================

	UIStyleRaw getStyles() const;
	void setStyles(UIStyleRef value);
	UIComputedStyleRaw getStyleComputed() const;
	virtual UIString getStyleText() const;	// Style Sheet
	virtual void setStyleText(UIString value);	// Style Sheet
	virtual UIString getStyleText(UIString name) const;
	virtual bool setStyleText(UIString name, UIString value);

	template<class T>
	T const& getStyle(UIString const& key, T const& value = T()) const
	{
		return getStyles()->getStyle<T>(key, value);
	}

	template<class T>
	void setStyle(UIString const& key, T const& value) const
	{
		getStyles()->setStyle<T>(key, value);
	}

	template<class T>
	bool hasStyle(UIString const& key) const
	{
		if (auto result = getStyles()->getStyle(key)) return result->getData(typeid(std::remove_cvref_t<T>));
		return false;
	}

	// ================================DOM Attrib======================================

	UIAttribRaw getAttribs() const;
	void setAttribs(UIAttribRef value);
	virtual UIString getAttribText(UIString name) const;
	virtual void setAttribText(UIString name, UIString value);

	template<class T>
	T const& getAttrib(UIString const& key, T const& value = T()) const
	{
		return getAttribs()->getAttrib<T>(key, value);
	}

	template<class T>
	void setAttrib(UIString const& key, T const& value) const
	{
		getAttribs()->setAttrib<T>(key, value);
	}

	template<class T>
	bool hasAttrib(UIString const& key) const
	{
		if (auto result = getAttribs()->getAttrib(key)) return result->getData(typeid(std::remove_cvref_t<T>));
		return false;
	}

protected:
	template<class T>
	void setEmbedStyle(UIString const& key, T& value) const
	{
		getStyles()->setEmbedStyle<T>(key, value);
	}

	template<class T>
	void setEmbedAttrib(UIString const& key, T& value) const
	{
		getAttribs()->setEmbedAttrib<T>(key, value);
	}

protected:
	virtual void closeEvent(UICloseEventRaw event);
	virtual void dragEnterEvent(UIDragEnterEventRaw event);
	virtual void dragLeaveEvent(UIDragLeaveEventRaw event);
	virtual void dragMoveEvent(UIDragMoveEventRaw event);
	virtual void dropEvent(UIDropEventRaw event);
	virtual void enterEvent(UIMouseEventRaw event);
	virtual void focusInEvent(UIFocusEventRaw event);
	virtual void focusOutEvent(UIFocusEventRaw event);
	virtual void hideEvent(UIHideEventRaw event);
	virtual void inputEvent(UITextInputEventRaw event);
	virtual void keyPressEvent(UIKeyEventRaw event);
	virtual void keyReleaseEvent(UIKeyEventRaw event);
	virtual void leaveEvent(UIMouseEventRaw event);
	virtual void mouseDoubleEvent(UIMouseEventRaw event);
	virtual void mouseMoveEvent(UIMouseEventRaw event);
	virtual void mousePressEvent(UIMouseEventRaw event);
	virtual void mouseReleaseEvent(UIMouseEventRaw event);
	virtual void moveEvent(UIMoveEventRaw event);
	virtual void resizeEvent(UIResizeEventRaw event);
	virtual void showEvent(UIShowEventRaw event);
	virtual void tabletEvent(UIMouseTabletEventRaw event);
	virtual void wheelEvent(UIMouseWheelEventRaw event);
	virtual void timerEvent(UITimerEventRaw event);

protected:
	UICanvasRaw getCanvas() const;

private:
	void setContext(UICanvasRaw value);
	void setParent(UIWidgetRaw value);

private:
	friend class UICanvas;
	UIRaw<UIWidgetPrivate> m_Private;
};

struct UIWidgetStyle
{
    UIPropAlignContent AlignContent;
    UIPropAlignItems AlignItems;
    UIPropAlignSelf AlignSelf;
    UIPropAlignmentBaseline AlignmentBaseline;
    UIPropBaselineShift BaselineShift;
    UIPropBaselineSource BaselineSource;
    UIPropBorder Border;
    UIPropBorderBottom BorderBottom;
    UIPropBorderLeft BorderLeft;
    UIPropBorderRight BorderRight;
    UIPropBorderTop BorderTop;
    UIPropBottom Bottom;
    UIPropBoxSizing BoxSizing;
    UIPropClear Clear;
    UIPropColor Color;
    UIPropDirection Direction;
    UIPropDisplay Display;
    UIPropDominantBaseline DominantBaseline;
    UIPropFlex Flex;
    UIPropFlexBasis FlexBasis;
    UIPropFlexDirection FlexDirection;
    UIPropFlexGrow FlexGrow;
    UIPropFlexShrink FlexShrink;
    UIPropFlexWrap FlexWrap;
    UIPropFloat Float;
    UIPropFloatDefer FloatDefer;
    UIPropFloatOffset FloatOffset;
    UIPropFloatReference FloatReference;
    UIPropFontFamily FontFamily;
    UIPropFontSize FontSize;
    UIPropFontStretch FontStretch;
    UIPropFontStyle FontStyle;
    UIPropFontWeight FontWeight;
    UIPropHangingPunctuation HangingPunctuation;
    UIPropHeight Height;
    UIPropHyphens Hyphens;
    UIPropInsetBlockEnd InsetBlockEnd;
    UIPropInsetBlockStart InsetBlockStart;
    UIPropInsetInlineEnd InsetInlineEnd;
    UIPropInsetInlineStart InsetInlineStart;
    UIPropJustifyContent JustifyContent;
    UIPropLeft Left;
    UIPropLetterSpacing LetterSpacing;
    UIPropLineBreak LineBreak;
    UIPropLineHeight LineHeight;
    UIPropMargin Margin;
    UIPropMarginBottom MarginBottom;
    UIPropMarginLeft MarginLeft;
    UIPropMarginRight MarginRight;
    UIPropMarginTop MarginTop;
    UIPropMaxHeight MaxHeight;
    UIPropMaxWidth MaxWidth;
    UIPropMinHeight MinHeight;
    UIPropMinWidth MinWidth;
    UIPropOpacity Opacity;
    UIPropOrder Order;
    UIPropOverflowBlock OverflowBlock;
    UIPropOverflowInline OverflowInline;
    UIPropOverflowWrap OverflowWrap;
    UIPropOverflowX OverflowX;
    UIPropOverflowY OverflowY;
    UIPropPadding Padding;
    UIPropPaddingBottom PaddingBottom;
    UIPropPaddingLeft PaddingLeft;
    UIPropPaddingRight PaddingRight;
    UIPropPaddingTop PaddingTop;
    UIPropPosition Position;
    UIPropRight Right;
    UIPropTabSize TabSize;
    UIPropTextAlign TextAlign;
    UIPropTextAlignAll TextAlignAll;
    UIPropTextAlignLast TextAlignLast;
    UIPropTextCombineUpright TextCombineUpright;
    UIPropTextDecorationLine TextDecorationLine;
    UIPropTextDecorationStyle TextDecorationStyle;
    UIPropTextIndent TextIndent;
    UIPropTextJustify TextJustify;
    UIPropTextOrientation TextOrientation;
    UIPropTextOverflow TextOverflow;
    UIPropTextTransform TextTransform;
    UIPropTop Top;
    UIPropUnicodeBidi UnicodeBidi;
    UIPropVerticalAlign VerticalAlign;
    UIPropVisibility Visibility;
    UIPropWhiteSpace WhiteSpace;
    UIPropWidth Width;
    UIPropWordBreak WordBreak;
    UIPropWordSpacing WordSpacing;
    UIPropWordWrap WordWrap;
    UIPropWrapFlow WrapFlow;
    UIPropWrapThrough WrapThrough;
    UIPropWritingMode WritingMode;
    UIPropZIndex ZIndex;
};