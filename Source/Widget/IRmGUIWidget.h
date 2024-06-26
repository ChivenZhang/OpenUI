#pragma once
#include "IRmGUIPainter.h"
#include "IRmGUIEvent.h"
class IRmGUIContext;
#define RmNAN (NAN)

class IRmGUIWidget;
using IRmGUIWidgetRef = RmRef<IRmGUIWidget>;
using IRmGUIWidgetRaw = RmRaw<IRmGUIWidget>;

/// @brief Base interface of all widgets.
class RMGUI_API IRmGUIWidget : public IRmGUIHandler, public IRmGUIFilter
{
public:
	using resize_t = enum
	{
		SizeFixed = 0,
		SizeMinimum = 1,
		SizeMaximum = 2,
		SizePreferred = 3,
		SizeExpanding = 4,
		SizeMinimumExpanding = 5,
		SizeIgnored = 6,
	};

	using policy_t = struct policy_t
	{
		resize_t HorizontalPolicy = SizeFixed;
		uint32_t HorizontalStretch = 1;
		resize_t VerticalPolicy = SizeFixed;
		uint32_t VerticalStretch = 1;
	};

	using focus_t = enum
	{
		FocusNoFocus = 0,
		FocusTabFocus = 0x0001,
		FocusClickFocus = 0x0002,
		FocusStrongFocus = FocusTabFocus | FocusClickFocus | 0x0004,
		FocusWheelFocus = FocusStrongFocus | 0x0008,
	};

public:
	virtual bool getVisible() const = 0;
	virtual void  setVisible(bool value) = 0;

	virtual bool getEnable() const = 0;
	virtual void  setEnable(bool value) = 0;

	virtual float getWidth() const = 0;
	virtual void setWidth(float value) = 0;

	virtual float getHeight() const = 0;
	virtual void setHeight(float value) = 0;

	virtual float getMinWidth() const = 0;
	virtual void setMinWidth(float value) = 0;

	virtual float getMinHeight() const = 0;
	virtual void setMinHeight(float value) = 0;

	virtual float getMaxWidth() const = 0;
	virtual void setMaxWidth(float value) = 0;

	virtual float getMaxHeight() const = 0;
	virtual void setMaxHeight(float value) = 0;

	virtual float getFixedWidth() const = 0;
	virtual void setFixedWidth(float value) = 0;

	virtual float getFixedHeight() const = 0;
	virtual void setFixedHeight(float value) = 0;

	virtual float getPositionX() const = 0;
	virtual void setPositionX(float value) = 0;

	virtual float getPositionY() const = 0;
	virtual void setPositionY(float value) = 0;

	virtual void setPosition(float x, float y) = 0;
	virtual void setSize(float width, float height) = 0;
	virtual void setMinSize(float width, float height) = 0;
	virtual void setMaxSize(float width, float height) = 0;
	virtual void setFixedSize(float width, float height) = 0;

	virtual RmFloat4 getMargin() const = 0;
	virtual void setMargin(RmFloat4 value) = 0;

	virtual RmFloat4 getPadding() const = 0;
	virtual void setPadding(RmFloat4 value) = 0;

	virtual RmFloat4 getBorder() const = 0;
	virtual void setBorder(RmFloat4 value) = 0;

	virtual policy_t getSizePolicy() const = 0;
	virtual void setSizePolicy(policy_t value) = 0;

	virtual bool getFocus() const = 0;
	virtual void setFocus(bool value) = 0;

	virtual focus_t getFocusPolicy() const = 0;
	virtual void setFocusPolicy(focus_t value) = 0;

	virtual IRmGUIWidgetRaw getLastFocus() const = 0;
	virtual IRmGUIWidgetRaw getNextFocus() const = 0;

	virtual RmRect getRect() const = 0;
	virtual void setRect(RmRect client) = 0;
	virtual RmRect getViewport() const = 0;
	virtual void setViewport(RmRect value) = 0;

	virtual IRmGUIWidgetRaw getParent() const = 0;
	virtual RmArrayView<IRmGUIWidgetRef> getChildren() = 0;
	virtual RmArrayView<const IRmGUIWidgetRef> getChildren() const = 0;

	virtual bool addWidget(IRmGUIWidgetRef widget) = 0;
	virtual bool removeWidget(IRmGUIWidgetRef widget) = 0;
	virtual void removeWidget() = 0;

	virtual IRmGUIFilterRaw getEventFilter() const = 0;
	virtual void setEventFilter(IRmGUIFilterRaw value) = 0;

	virtual IRmGUIPainterRaw getPainter() const = 0;
	virtual void setPainter(IRmGUIPainterRef value) = 0;

	virtual RmArrayView<const RmPointUV3> getPrimitive() const = 0;

	virtual RmString getAttribute(uint32_t name) const = 0;
	virtual void setAttribute(uint32_t name, RmString const& value) = 0;

	virtual void layout(RmRectRaw client) = 0;
	virtual void paint(IRmGUIPainterRaw painter, RmRectRaw client) = 0;

protected:
	virtual void closeEvent(IRmGUICloseEventRaw event) = 0;

	virtual void dragEnterEvent(IRmGUIDragEnterEventRaw event) = 0;

	virtual void dragLeaveEvent(IRmGUIDragLeaveEventRaw event) = 0;

	virtual void dragMoveEvent(IRmGUIDragMoveEventRaw event) = 0;

	virtual void dropEvent(IRmGUIDropEventRaw event) = 0;

	virtual void enterEvent(IRmGUIMouseEventRaw event) = 0;

	virtual void focusInEvent(IRmGUIFocusEventRaw event) = 0;

	virtual void focusOutEvent(IRmGUIFocusEventRaw event) = 0;

	virtual void hideEvent(IRmGUIHideEventRaw event) = 0;

	virtual void inputEvent(IRmGUIKeyEventRaw event) = 0;

	virtual void keyPressEvent(IRmGUIKeyEventRaw event) = 0;

	virtual void keyReleaseEvent(IRmGUIKeyEventRaw event) = 0;

	virtual void leaveEvent(IRmGUIMouseEventRaw event) = 0;

	virtual void mouseDoubleEvent(IRmGUIMouseEventRaw event) = 0;

	virtual void mouseMoveEvent(IRmGUIMouseEventRaw event) = 0;

	virtual void mousePressEvent(IRmGUIMouseEventRaw event) = 0;

	virtual void mouseReleaseEvent(IRmGUIMouseEventRaw event) = 0;

	virtual void moveEvent(IRmGUIMoveEventRaw event) = 0;

	virtual void resizeEvent(IRmGUIResizeEventRaw event) = 0;

	virtual void showEvent(IRmGUIShowEventRaw event) = 0;

	virtual void tabletEvent(IRmGUIMouseTabletEventRaw event) = 0;

	virtual void wheelEvent(IRmGUIMouseWheelEventRaw event) = 0;
};