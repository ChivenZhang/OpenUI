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
#include "UIWidget.h"
#include "UIPainter.h"
#include "UIRender.h"
#include "UIBuilder.h"

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
	explicit UIContext(UIConfig config = {});
	~UIContext();
	UIConfig const& getConfig() const;
	UIPainterRaw getPainter() const;
	void setPainter(UIPainterRef value);
	UIRenderRaw getRender() const;
	void setRender(UIRenderRef value);
	UIBuilderRaw getBuilder() const;
	UIWidgetRaw getFocus() const;
	void setFocus(UIWidgetRaw value);
	void setAnimate(UIWidgetRaw value, bool animate);
	void sendEvent(UIReactorRaw sender, UIEventRaw event);
	void postEvent(UIReactorRef sender, UIEventRef event);
	bool addElement(UIWidgetRef value, int32_t zorder = 0);
	bool removeElement(UIWidgetRef value);
	void removeElement();
	bool existElement(UIWidgetRef value) const;
	UIListView<const UIWidgetRef> getElement() const;
	void layoutElement();
	bool layoutElement(UIRect client);
	void paintElement();
	bool paintElement(UIRect client);
	void renderElement(UIRect client);
	void animateElement(float time);

	void updateElement(float time, UIRect client)
	{
		animateElement(time);
		layoutElement(client);
		paintElement(client);
    }

private:
	UIContextPrivateRaw m_Private;
};
using UIContextRef = UIRef<UIContext>;
using UIContextRaw = UIRaw<UIContext>;