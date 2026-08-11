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

	UIPropDisplay getDisplayType() const;
	void setDisplayType(UIPropDisplay value);
	UIPropPosition getPositionType() const;
	void setPositionType(UIPropPosition value);
	UIPropLeft getFixedPosX() const;
	void setFixedPosX(UIPropLeft value);
	void setFixedPosX(float value);
	UIPropTop getFixedPosY() const;
	void setFixedPosY(UIPropTop value);
	void setFixedPosY(float value);
	void setFixedPos(UIPropLeft left, UIPropTop top);
	void setFixedPos(float left, float top);
	UIPropMinWidth getMinWidth() const;
	void setMinWidth(UIPropMinWidth value);
	void setMinWidth(float value);
	UIPropMaxWidth getMaxWidth() const;
	void setMaxWidth(UIPropMaxWidth value);
	void setMaxWidth(float value);
	UIPropWidth getFixedWidth() const;
	void setFixedWidth(UIPropWidth value);
	void setFixedWidth(float value);
	UIPropMinHeight getMinHeight() const;
	void setMinHeight(UIPropMinHeight value);
	void setMinHeight(float value);
	UIPropMaxHeight getMaxHeight() const;
	void setMaxHeight(UIPropMaxHeight value);
	void setMaxHeight(float value);
	UIPropHeight getFixedHeight() const;
	void setFixedHeight(UIPropHeight value);
	void setFixedHeight(float value);
	void setMinSize(UIPropMinWidth width, UIPropMinHeight height);
	void setMinSize(float width, float height);
	void setMaxSize(UIPropMaxWidth width, UIPropMaxHeight height);
	void setMaxSize(float width, float height);
	void setFixedSize(UIPropWidth width, UIPropHeight height);
	void setFixedSize(float width, float height);

	void setBorder(UIPropBorderLeft left, UIPropBorderTop top, UIPropBorderRight right, UIPropBorderBottom bottom);
	UIPropBorderTop getBorderTop() const;
	void setBorderTop(UIPropBorderTop value);
	UIPropBorderBottom getBorderBottom() const;
	void setBorderBottom(UIPropBorderBottom value);
	UIPropBorderLeft getBorderLeft() const;
	void setBorderLeft(UIPropBorderLeft value);
	UIPropBorderRight getBorderRight() const;
	void setBorderRight(UIPropBorderRight value);

	void setMargin(UIPropMarginLeft left, UIPropMarginTop top, UIPropMarginRight right, UIPropMarginBottom bottom);
	UIPropMarginTop getMarginTop() const;
	void setMarginTop(UIPropMarginTop value);
	UIPropMarginBottom getMarginBottom() const;
	void setMarginBottom(UIPropMarginBottom value);
	UIPropMarginLeft getMarginLeft() const;
	void setMarginLeft(UIPropMarginLeft value);
	UIPropMarginRight getMarginRight() const;
	void setMarginRight(UIPropMarginRight value);

	void setPadding(UIPropPaddingLeft left, UIPropPaddingTop top, UIPropPaddingRight right, UIPropPaddingBottom bottom);
	UIPropPaddingTop getPaddingTop() const;
	void setPaddingTop(UIPropPaddingTop value);
	UIPropPaddingBottom getPaddingBottom() const;
	void setPaddingBottom(UIPropPaddingBottom value);
	UIPropPaddingLeft getPaddingLeft() const;
	void setPaddingLeft(UIPropPaddingLeft value);
	UIPropPaddingRight getPaddingRight() const;
	void setPaddingRight(UIPropPaddingRight value);

	UIPropFlexDirection getFlexDirection() const;
	void setFlexDirection(UIPropFlexDirection value);
	UIPropFlexWrap getFlexWrap() const;
	void setFlexWrap(UIPropFlexWrap value);
	UIPropJustifyContent getJustifyContent() const;
	void setJustifyContent(UIPropJustifyContent value);
	UIPropAlignItems getAlignItems() const;
	void setAlignItems(UIPropAlignItems value);
	UIPropAlignContent getAlignContent() const;
	void setAlignContent(UIPropAlignContent value);
	void setFlexFlow(UIPropFlexDirection direction, UIPropFlexWrap wrap);

	UIPropFlexGrow getFlexGrow() const;
	void setFlexGrow(UIPropFlexGrow value);
	UIPropFlexShrink getFlexShrink() const;
	void setFlexShrink(UIPropFlexShrink value);
	UIPropFlexBasis getFlexBasis() const;
	void setFlexBasis(UIPropFlexBasis value);
	UIPropAlignSelf getAlignSelf() const;
	void setAlignSelf(UIPropAlignSelf value);
	void setFlex(UIPropFlexGrow grow, UIPropFlexShrink shrink, UIPropFlexBasis basis);

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
	void setStyle(UIString const& key, T const& value = T()) const
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
	UIPrivateRaw m_Private;
};