#pragma once
#include "Widget/IRmGUIWidget.h"
#include "RmGUISignal.h"
class RmGUIContext;

class RmGUIWidgetPrivate {};
using RmGUIWidgetPrivateRaw = RmRaw<RmGUIWidgetPrivate>;
class RMGUI_API RmGUIWidget : public IRmGUIWidget
{
public:
	RmGUIWidget(IRmGUIWidgetRaw parent = nullptr);
	~RmGUIWidget();
	virtual RmRect getRect() const override;
	virtual void setRect(RmRect client) override;
	virtual RmRect getViewport() const override;
	virtual void setViewport(RmRect value) override;
	virtual RmRect getChildrenRect() const override;
	virtual IRmGUIWidgetRaw getParent() const override;
	virtual void setParent(IRmGUIWidgetRaw parent) override;
	virtual RmArrayView<IRmGUIWidgetRef> getChildren() override;
	virtual RmArrayView<const IRmGUIWidgetRef> getChildren() const override;
	virtual bool addWidget(IRmGUIWidgetRef widget) override;
	virtual bool removeWidget(IRmGUIWidgetRef widget) override;
	virtual bool filter(IRmGUIReactorRaw source, IRmGUIEventRaw event) override;
	virtual void handle(IRmGUIReactorRaw source, IRmGUIEventRaw event) final override;
	virtual void layout(RmRectRaw client) override;
	virtual void paint(IRmGUIPainterRaw painter, RmRectRaw client) override;
	virtual RmString getAttribute(uint32_t name) const override;
	virtual void setAttribute(uint32_t name, RmString const& value) override;

	virtual bool getVisible() const override;
	virtual void  setVisible(bool value) override;
	virtual bool getEnable() const override;
	virtual void  setEnable(bool value) override;
	virtual float getWidth() const override;
	virtual void setWidth(float value) override;
	virtual float getHeight() const override;
	virtual void setHeight(float value) override;
	virtual float getMinWidth() const override;
	virtual void setMinWidth(float value) override;
	virtual float getMinHeight() const override;
	virtual void setMinHeight(float value) override;
	virtual float getMaxWidth() const override;
	virtual void setMaxWidth(float value) override;
	virtual float getMaxHeight() const override;
	virtual void setMaxHeight(float value) override;
	virtual float getFixedWidth() const override;
	virtual void setFixedWidth(float value) override;
	virtual float getFixedHeight() const override;
	virtual void setFixedHeight(float value) override;
	virtual float getPositionX() const override;
	virtual void setPositionX(float value) override;
	virtual float getPositionY() const override;
	virtual void setPositionY(float value) override;
	virtual void setPosition(float x, float y) override;
	virtual void setSize(float width, float height) override;
	virtual void setMinSize(float width, float height) override;
	virtual void setMaxSize(float width, float height) override;
	virtual void setFixedSize(float width, float height) override;
	virtual RmFloat4 getMargin() const override;
	virtual void setMargin(RmFloat4 value) override;
	virtual RmFloat4 getPadding() const override;
	virtual void setPadding(RmFloat4 value) override;
	virtual RmFloat4 getBorder() const override;
	virtual void setBorder(RmFloat4 value) override;
	virtual policy_t getSizePolicy() const override;
	virtual void setSizePolicy(policy_t value) override;
	virtual bool getFocus() const override;
	virtual void setFocus(bool value) override;
	virtual focus_t getFocusPolicy() const override;
	virtual void setFocusPolicy(focus_t value) override;
	virtual IRmGUIWidgetRaw getLastFocus() const override;
	virtual IRmGUIWidgetRaw getNextFocus() const override;

protected:
	virtual void closeEvent(IRmGUICloseEventRaw event) override;
	virtual void dragEnterEvent(IRmGUIDragEnterEventRaw event) override;
	virtual void dragLeaveEvent(IRmGUIDragLeaveEventRaw event) override;
	virtual void dragMoveEvent(IRmGUIDragMoveEventRaw event) override;
	virtual void dropEvent(IRmGUIDropEventRaw event) override;
	virtual void enterEvent(IRmGUIMouseEventRaw event) override;
	virtual void focusInEvent(IRmGUIFocusEventRaw event) override;
	virtual void focusOutEvent(IRmGUIFocusEventRaw event) override;
	virtual void hideEvent(IRmGUIHideEventRaw event) override;
	virtual void inputEvent(IRmGUIKeyEventRaw event) override;
	virtual void keyPressEvent(IRmGUIKeyEventRaw event) override;
	virtual void keyReleaseEvent(IRmGUIKeyEventRaw event) override;
	virtual void leaveEvent(IRmGUIMouseEventRaw event) override;
	virtual void mouseDoubleEvent(IRmGUIMouseEventRaw event) override;
	virtual void mouseMoveEvent(IRmGUIMouseEventRaw event) override;
	virtual void mousePressEvent(IRmGUIMouseEventRaw event) override;
	virtual void mouseReleaseEvent(IRmGUIMouseEventRaw event) override;
	virtual void moveEvent(IRmGUIMoveEventRaw event) override;
	virtual void resizeEvent(IRmGUIResizeEventRaw event) override;
	virtual void showEvent(IRmGUIShowEventRaw event) override;
	virtual void tabletEvent(IRmGUIMouseTabletEventRaw event) override;
	virtual void wheelEvent(IRmGUIMouseWheelEventRaw event) override;

protected:
	virtual RmRaw<IRmGUIContext> getContext() const;
	virtual void setContext(RmRaw<IRmGUIContext> context);

private:
	friend class RmGUIContext;
	RmGUIWidgetPrivateRaw m_PrivateData;
};