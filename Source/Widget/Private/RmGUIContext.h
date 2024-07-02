#pragma once
#include "Widget/IRmGUIContext.h"

class RmGUIContextPrivate {};
using RmGUIContextPrivateRaw = RmRaw<RmGUIContextPrivate>;

/// @brief 
class RMGUI_API RmGUIContext : public IRmGUIContext
{
public:
	RmGUIContext();
	~RmGUIContext();
	virtual IRmGUIPainterRaw getPainter() const override;
	virtual void setPainter(IRmGUIPainterRef value) override;
	virtual IRmGUIRenderRaw getRender() const override;
	virtual void setRender(IRmGUIRenderRef value) override;
	virtual IRmGUIWidgetRaw getFocus() const override;
	virtual void setFocus(IRmGUIWidgetRaw widget) override;
	virtual bool addWidget(IRmGUIWidgetRef widget, int32_t zorder = 0) override;
	virtual bool removeWidget(IRmGUIWidgetRef widget) override;
	virtual void sendEvent(IRmGUIReactorRaw source, IRmGUIEventRaw event) override;
	virtual void postEvent(IRmGUIReactorRaw source, IRmGUIEventRaw event) override;
	virtual void layoutWidget(RmRect client) override;
	virtual void paintWidget(RmRect client) override;
	virtual void renderWidget(RmRect client) override;

private:
	RmGUIContextPrivateRaw m_Private;
};