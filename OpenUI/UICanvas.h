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
	UIImage RenderTexture;
};

/// @brief 
class UICanvasPrivate
{
public:
	virtual ~UICanvasPrivate() = default;
};
using UIContextPrivateRaw = UIRaw<UICanvasPrivate>;

/// @brief 
class OPENUI_API UICanvas
{
public:
	explicit UICanvas(UIConfig config = {});
	~UICanvas();
	UIConfig const& getConfig() const;
	UIPainterRaw getPainter() const;
	void setPainter(UIPainterRef value);
	UIRenderRaw getRender(UIString name) const;
	void setRender(UIRenderRef value);
	UIBuilderRaw getBuilder() const;
	UIWidgetRaw getFocus() const;
	void setFocus(UIWidgetRaw value);
	void setAnimate(UIWidgetRaw value, bool animate);
	void sendEvent(UIReactorRaw sender, UIEventRaw event);
	void postEvent(UIReactorRef sender, UIEventRef event);
	bool addWidget(UIWidgetRef value, int32_t zorder = 0);
	bool removeWidget(UIWidgetRef value);
	void removeWidget();
	bool existWidget(UIWidgetRef value) const;
	UIListView<const UIWidgetRef> getWidget() const;
	void layoutWidget();
	bool layoutWidget(UIRect client);
	void paintWidget();
	bool paintWidget(UIRect client);
	void renderWidget(UIRect client);
	void animateWidget(float time);
	void updateWidget(float time, UIRect client);
	UIImage getTexture() const;
	void setTexture(UIImage value);

private:
	UIContextPrivateRaw m_Private;
};
using UICanvasRef = UIRef<UICanvas>;
using UICanvasRaw = UIRaw<UICanvas>;