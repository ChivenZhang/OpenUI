#pragma once
#include "Widget/IRmGUIWidget.h"
class RmGUIContext;

class RmGUIWidgetPrivate;
class RMGUI_API RmGUIWidget : public IRmGUIWidget
{
public:
	RmGUIWidget(IRmGUIWidgetRaw parent = nullptr);
	~RmGUIWidget();
	virtual RmRect getRect() const override;
	virtual void setRect(RmRect client) override;
	virtual RmRect getChildrenRect() const override;
	virtual IRmGUIWidgetRaw getParent() const override;
	virtual void setParent(IRmGUIWidgetRaw parent) override;
	virtual RmArrayView<IRmGUIWidgetRef> getChildren() override;
	virtual RmArrayView<const IRmGUIWidgetRef> getChildren() const override;
	virtual bool addWidget(IRmGUIWidgetRef widget) override;
	virtual bool removeWidget(IRmGUIWidgetRef widget) override;
	virtual bool filter(IRmGUIReactorRaw source, IRmGUIEventRaw event) override;
	virtual void handle(IRmGUIReactorRaw source, IRmGUIEventRaw event) override;
	virtual void layout(RmRectRaw client) override;
	virtual void paint(IRmGUIPainterRaw painter, RmRectRaw client) override;
	virtual RmString getAttribute(uint32_t name) const override;
	virtual void setAttribute(uint32_t name, RmString const& value) override;

	virtual float getPositionX() const;
	virtual void setPositionX(float value);
	virtual float getPositionY() const;
	virtual void setPositionY(float value);
	virtual float getWidth() const;
	virtual void setWidth(float value);
	virtual float getHeight() const;
	virtual void setHeight(float value);
	virtual float getMinWidth() const;
	virtual void setMinWidth(float value);
	virtual float getMinHeight() const;
	virtual void setMinHeight(float value);
	virtual float getMaxWidth() const;
	virtual void setMaxWidth(float value);
	virtual float getMaxHeight() const;
	virtual void setMaxHeight(float value);
	virtual float getFixedWidth() const;
	virtual void setFixedWidth(float value);
	virtual float getFixedHeight() const;
	virtual void setFixedHeight(float value);
	virtual void setPosition(float x, float y);
	virtual void setSize(float width, float height);
	virtual void setMinSize(float width, float height);
	virtual void setMaxSize(float width, float height);
	virtual void setFixedSize(float width, float height);
	virtual RmFloat4 getMargin() const override;
	virtual void setMargin(RmFloat4 value) override;
	virtual RmFloat4 getPadding() const override;
	virtual void setPadding(RmFloat4 value) override;
	virtual RmFloat4 getBorder() const override;
	virtual void setBorder(RmFloat4 value) override;

protected:
	virtual void closeEvent(IRmGUICloseEventRaw event) override;
	virtual void dragEnterEvent(IRmGUIDragEnterEventRaw event) override;
	virtual void dragLeaveEvent(IRmGUIDragLeaveEventRaw event) override;
	virtual void dragMoveEvent(IRmGUIDragMoveEventRaw event) override;
	virtual void dropEvent(IRmGUIDropEventRaw event) override;
	virtual void enterEvent(IRmGUIMouseEnterEventRaw event) override;
	virtual void focusInEvent(IRmGUIFocusEventRaw event) override;
	virtual void focusOutEvent(IRmGUIFocusEventRaw event) override;
	virtual void hideEvent(IRmGUIHideEventRaw event) override;
	virtual void inputEvent(IRmGUIKeyInputEventRaw event) override;
	virtual void keyPressEvent(IRmGUIKeyDownEventRaw event) override;
	virtual void keyReleaseEvent(IRmGUIKeyUpEventRaw event) override;
	virtual void leaveEvent(IRmGUIMouseLeaveEventRaw event) override;
	virtual void mouseDoubleClickEvent(IRmGUIMouseEventRaw event) override;
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
	RmGUIWidgetPrivate* m_PrivateData;
	friend class RmGUIContext;
};