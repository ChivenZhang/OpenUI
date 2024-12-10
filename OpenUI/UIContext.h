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
#include "UIElement.h"
#include "UIPainter.h"
#include "UIRender.h"

class UIConfig
{
public:
	float DisplayScale = 1.0f;
	float PixelDensity = 1.0f;
};

/// @brief 
class UIContextPrivate
{
public:
	virtual ~UIContextPrivate() = default;
};

using UIContextPrivateRaw = UIRaw<UIContextPrivate>;

/// @brief 
class OPENUI_API UIContext
{
public:
	UIContext(UIConfig config = {});
	virtual ~UIContext();
	virtual UIConfig const& getConfig() const;
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
	virtual bool existElement(UIElementRef value) const;
	virtual UIArrayView<const UIElementRef> getElement() const;
	virtual void layoutElement();
	virtual bool layoutElement(UIRect client);
	virtual void paintElement();
	virtual bool paintElement(UIRect client);
	virtual void renderElement(UIRect client);
	virtual void animateElement(float time);

private:
	UIContextPrivateRaw m_Private;
};
using UIContextRef = UIRef<UIContext>;
using UIContextRaw = UIRaw<UIContext>;