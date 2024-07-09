#pragma once
#include "UIEvent.h"
#include "UIPainter.h"

class UIElement;
using UIElementRef = UIRef<UIElement>;
using UIElementRaw = UIRaw<UIElement>;

class UIElementPrivate {};
using UIElementPrivateRaw = UIRaw<UIElementPrivate>;

class UIContext;
using UIContextRaw = UIRaw<UIContext>;

/// @brief Base interface of element
class OPENUI_API UIElement : public UIReactor, public UIHandler, public UIFilter
{
public:
	UIElement();
	virtual ~UIElement();
	UIElementRaw getParent() const;
	UIArrayView<const UIElementRef> getChildren() const;
	virtual UIPainterRaw getPainter() const;
	virtual void setPainter(UIPainterRef value);
	virtual UIFilterRaw getEventFilter() const;
	virtual void setEventFilter(UIFilterRaw value);
	virtual UIString getStyle() const;
	virtual void setStyle(UIString value);
	virtual UIString getStyle(UIString name) const;
	virtual void setStyle(UIString name, UIString value);
	virtual UIArrayView<const UIPointUV3> getPrimitive() const;
	virtual bool addElement(UIElementRef value);
	virtual bool removeElement(UIElementRef value);
	virtual void removeElement();
	virtual void layout(UIRect client);
	virtual void paint(UIRect client, UIPainterRaw painter);
	virtual bool filter(UIReactorRaw source, UIEventRaw event) override;
	virtual void handle(UIReactorRaw source, UIEventRaw event) final override;

	// =================================

	virtual bool getEnable() const;
	virtual void setEnable(bool value);
	virtual bool getVisible() const;
	virtual void setVisible(bool value);
	virtual UIRect getBounds() const;
	virtual void setBounds(UIRect value);
	virtual UIRect getViewport() const;
	virtual void setViewport(UIRect value);

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

protected:
	virtual UIContextRaw getContext() const;
	virtual void setContext(UIContextRaw value);
	virtual void setParent(UIElementRaw value);

private:
	friend class UIContext;
	UIElementPrivateRaw m_Private;
};