#pragma once
#include "UIElement.h"
#include "UIPainter.h"
#include "UIRender.h"

/// @brief 
class UIContextPrivate {};
using UIContextPrivateRaw = UIRaw<UIContextPrivate>;

/// @brief 
class OPENUI_API UIContext
{
public:
	UIContext();
	virtual ~UIContext();
	virtual UIPainterRaw getPainter() const;
	virtual void setPainter(UIPainterRef value);
	virtual UIRenderRaw getRender() const;
	virtual void setRender(UIRenderRef value);
	virtual UIElementRaw getFocus() const;
	virtual void setFocus(UIElementRaw value);
	virtual void setAnimate(UIElementRaw value, bool animate);
	virtual void sendEvent(UIReactorRaw sender, UIEventRaw event);
	virtual void postEvent(UIReactorRef sender, UIEventRef event);
	virtual bool addElement(UIElementRef value, int32_t zorder = 0);
	virtual bool removeElement(UIElementRef value);
	virtual void removeElement();
	virtual void layoutElement(UIRect client);
	virtual void paintElement(UIRect client);
	virtual void renderElement(UIRect client);
	virtual void animateElement(float time);

private:
	UIContextPrivateRaw m_Private;
};
using UIContextRef = UIRef<UIContext>;
using UIContextRaw = UIRaw<UIContext>;