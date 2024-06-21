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
	virtual RmArrayView<RmRef<IRmGUIWidget>> getChildren() override;
	virtual RmArrayView<const RmRef<IRmGUIWidget>> getChildren() const override;
	virtual bool addWidget(RmRef<IRmGUIWidget> widget) override;
	virtual bool removeWidget(RmRef<IRmGUIWidget> widget) override;
	virtual bool filter(rmreactor source, rmevent event) override;
	virtual void handle(rmreactor source, rmevent event) override;
	virtual void layout(rmrect client) override;
	virtual void paint(rmpainter painter, rmrect client) override;
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
	virtual void closeEvent(rmevent_close event) override;
	virtual void dragEnterEvent(rmevent_drag_enter event) override;
	virtual void dragLeaveEvent(rmevent_drag_leave event) override;
	virtual void dragMoveEvent(rmevent_drag_move event) override;
	virtual void dropEvent(rmevent_drop event) override;
	virtual void enterEvent(rmevent_enter event) override;
	virtual void focusInEvent(rmevent_focus event) override;
	virtual void focusOutEvent(rmevent_focus event) override;
	virtual void hideEvent(rmevent_hide event) override;
	virtual void inputEvent(rmevent_input event) override;
	virtual void keyPressEvent(rmevent_key_down event) override;
	virtual void keyReleaseEvent(rmevent_key_up event) override;
	virtual void leaveEvent(rmevent_leave event) override;
	virtual void mouseDoubleClickEvent(rmevent_mouse event) override;
	virtual void mouseMoveEvent(rmevent_mouse event) override;
	virtual void mousePressEvent(rmevent_mouse event) override;
	virtual void mouseReleaseEvent(rmevent_mouse event) override;
	virtual void moveEvent(rmevent_move event) override;
	virtual void resizeEvent(rmevent_resize event) override;
	virtual void showEvent(rmevent_show event) override;
	virtual void tabletEvent(rmevent_tablet event) override;
	virtual void wheelEvent(rmevent_wheel event) override;

protected:
	virtual RmRaw<IRmGUIContext> getContext() const;
	virtual void setContext(RmRaw<IRmGUIContext> context);

private:
	RmGUIWidgetPrivate* m_PrivateData;
	friend class RmGUIContext;
};