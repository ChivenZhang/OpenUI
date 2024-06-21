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

	float getWidth() const;
	void setWidth(float value);
	float getHeight() const;
	void setHeight(float value);
	float getMinWidth() const;
	void setMinWidth(float value);
	float getMinHeight() const;
	void setMinHeight(float value);
	float getMaxWidth() const;
	void setMaxWidth(float value);
	float getMaxHeight() const;
	void setMaxHeight(float value);
	RmFloat4 getMargin() const;
	void setMargin(RmFloat4 value);
	RmFloat4 getPadding() const;
	void setPadding(RmFloat4 value);
	RmFloat4 getBorder() const;
	void setBorder(RmFloat4 value);

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