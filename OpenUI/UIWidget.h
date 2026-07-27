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
	virtual UIString getStyleText() const;
	virtual void setStyleText(UIString value);
	virtual UIString getStyleText(UIString name) const;
	virtual void setStyleText(UIString name, UIString value);
	virtual UIString getAttribute(UIString name) const;
	virtual void setAttribute(UIString name, UIString value);
	virtual UIListView<const UIPointUV3> getPrimitive() const;
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

	// ====================================Attribute===================================

	virtual bool getEnable() const;
	virtual void setEnable(bool value);
	virtual bool getVisible() const;
	virtual void setVisible(bool value);
	virtual bool getAnimate() const;
	virtual void setAnimate(bool value);
	virtual UIRect getBounds() const;
	virtual void setBounds(UIRect value);
	virtual UIRect getViewport() const;
	virtual void setViewport(UIRect value);
	virtual UIRect getLocalBounds() const;
	virtual void setLocalBounds(UIRect value);
	virtual float getScale() const;
	virtual void setScale(float value);
	virtual float getRotate() const;
	virtual void setRotate(float value);
	virtual UIFloat2 getTranslate() const;
	virtual void setTranslate(UIFloat2 value);
	virtual float getPosX() const;
	virtual float getPosY() const;
	virtual UIFloat2 getPos() const;
	virtual float getLocalX() const;
	virtual float getLocalY() const;
	virtual UIFloat2 getLocalPos() const;
	virtual float getWidth() const;
	virtual float getHeight() const;
	virtual UIFloat2 getSize() const;
	virtual bool inBounds(UIFloat2 pos);
	virtual bool inBounds(float x, float y);

	// =================================Flex Layout======================================

	virtual UI::DisplayType getDisplayType() const;
	virtual void setDisplayType(UI::DisplayType value);
	virtual UI::PositionType getPositionType() const;
	virtual void setPositionType(UI::PositionType value);
	virtual UIValueF getFixedPosX() const;
	virtual void setFixedPosX(UIValueF value);
	virtual UIValueF getFixedPosY() const;
	virtual void setFixedPosY(UIValueF value);
	virtual UIValue2F getFixedPos() const;
	virtual void setFixedPos(UIValueF left, UIValueF top);
	virtual UIValueF getMinWidth() const;
	virtual void setMinWidth(UIValueF value);
	virtual UIValueF getMaxWidth() const;
	virtual void setMaxWidth(UIValueF value);
	virtual UIValueF getFixedWidth() const;
	virtual void setFixedWidth(UIValueF value);
	virtual UIValueF getMinHeight() const;
	virtual void setMinHeight(UIValueF value);
	virtual UIValueF getMaxHeight() const;
	virtual void setMaxHeight(UIValueF value);
	virtual UIValueF getFixedHeight() const;
	virtual void setFixedHeight(UIValueF value);
	virtual UIValue2F getMinSize() const;
	virtual void setMinSize(UIValueF width, UIValueF height);
	virtual UIValue2F getMaxSize() const;
	virtual void setMaxSize(UIValueF width, UIValueF height);
	virtual UIValue2F getFixedSize() const;
	virtual void setFixedSize(UIValueF width, UIValueF height);

	virtual UIValue4F getBorder() const;
	virtual void setBorder(UIValue4F value);
	virtual UIValue4F getMargin() const;
	virtual void setMargin(UIValue4F value);
	virtual UIValue4F getPadding() const;
	virtual void setPadding(UIValue4F value);
	virtual UIValue2F getSpacing() const;
	virtual void setSpacing(UIValue2F value);

	virtual UI::FlexDirection getFlexDirection() const;
	virtual void setFlexDirection(UI::FlexDirection value);
	virtual UI::FlexWrap getFlexWrap() const;
	virtual void setFlexWrap(UI::FlexWrap value);
	virtual UI::JustifyContent getJustifyContent() const;
	virtual void setJustifyContent(UI::JustifyContent value);
	virtual UI::AlignItems getAlignItems() const;
	virtual void setAlignItems(UI::AlignItems value);
	virtual UI::AlignContent getAlignContent() const;
	virtual void setAlignContent(UI::AlignContent value);
	virtual void setFlexFlow(UI::FlexDirection direction, UI::FlexWrap wrap);

	virtual UI::FlexGrow getFlexGrow() const;
	virtual void setFlexGrow(UI::FlexGrow value);
	virtual UI::FlexShrink getFlexShrink() const;
	virtual void setFlexShrink(UI::FlexShrink value);
	virtual UI::FlexBasis getFlexBasis() const;
	virtual void setFlexBasis(UI::FlexBasis value);
	virtual UI::AlignSelf getAlignSelf() const;
	virtual void setAlignSelf(UI::AlignSelf value);
	virtual void setFlex(UI::FlexGrow grow, UI::FlexShrink shrink, UI::FlexBasis basis);

	// =================================CSS Style======================================

	virtual UIStyleRaw getStyleSheet() const;
	virtual void setStyleSheet(UIStyleRef value);

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
	virtual void setContext(UICanvasRaw value);
	virtual void setParent(UIWidgetRaw value);

private:
	friend class UICanvas;
	UIRaw<UIWidgetPrivate> m_Private;
};