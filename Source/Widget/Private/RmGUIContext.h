#pragma once
#include "Widget/IRmGUIContext.h"
#include "Widget/IRmGUIWidget.h"
#include "Widget/IRmGUISurface.h"

class RmGUIContext : public IRmGUIContext
{
public:
	virtual RmRaw<IRmGUISurface> getSurface() const override;
	virtual void setSurface(IRmGUISurfaceRef value) override;
	virtual bool addWidget(IRmGUIWidgetRef widget) override;
	virtual bool removeWidget(IRmGUIWidgetRef widget) override;
	virtual void sendEvent(IRmGUIReactorRaw source, IRmGUIEventRaw event) override;
	virtual void postEvent(IRmGUIReactorRaw source, IRmGUIEventRaw event) override;
	virtual void layoutWidget(RmRect client) override;
	virtual void paintWidget(RmRect client) override;

protected:
	RmRef<IRmGUISurface> m_Surface;
	RmRef<IRmGUIWidget> m_FocusWidget;
	RmVector<RmRef<IRmGUIWidget>> m_TopLevelList;
};