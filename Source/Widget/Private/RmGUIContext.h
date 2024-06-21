#pragma once
#include "Widget/IRmGUIContext.h"
#include "Widget/IRmGUIWidget.h"
#include "Widget/IRmGUISurface.h"
#include <cairo/cairo.h>

class RmGUIContext : public IRmGUIContext
{
public:
	virtual RmRaw<IRmGUISurface> getSurface() const override;
	virtual void setSurface(RmRef<IRmGUISurface> value) override;
	virtual bool addWidget(RmRef<IRmGUIWidget> widget) override;
	virtual bool removeWidget(RmRef<IRmGUIWidget> widget) override;
	virtual void sendEvent(IRmGUIReactorRaw source, IRmGUIEventRaw event) override;
	virtual void postEvent(IRmGUIReactorRaw source, IRmGUIEventRaw event) override;
	virtual void layoutWidget(RmRect client) override;
	virtual bool paintWidget(RmRect client) override;

protected:
	RmRef<IRmGUISurface> m_Surface;
	RmRef<IRmGUIWidget> m_FocusWidget;
	RmVector<RmRef<IRmGUIWidget>> m_TopLevelList;
};