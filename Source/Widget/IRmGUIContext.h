#pragma once
#include "IRmGUIEvent.h"
#include "IRmGUIWidget.h"
#include "IRmGUIRender.h"

/// @brief Base interface of widget context.
class RMGUI_API IRmGUIContext
{
public:
	virtual ~IRmGUIContext() = default;

	virtual IRmGUIPainterRaw getPainter() const = 0;

	virtual void setPainter(IRmGUIPainterRef value) = 0;

	virtual IRmGUIRenderRaw getRender() const = 0;

	virtual void setRender(IRmGUIRenderRef value) = 0;

	virtual bool addWidget(IRmGUIWidgetRef widget, int32_t zorder = 0) = 0;

	virtual bool removeWidget(IRmGUIWidgetRef widget) = 0;

	virtual void layoutWidget(RmRect client) = 0;

	virtual void paintWidget(RmRect client) = 0;

	virtual void renderWidget(RmRect client) = 0;

	virtual void sendEvent(IRmGUIReactorRaw source, IRmGUIEventRaw event) = 0;

	virtual void postEvent(IRmGUIReactorRaw source, IRmGUIEventRaw event) = 0;
};
using IRmGUIContextRef = RmRef<IRmGUIContext>;
using IRmGUIContextRaw = RmRaw<IRmGUIContext>;