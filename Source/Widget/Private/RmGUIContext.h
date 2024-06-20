#pragma once
#include "Widget/IRmGUIContext.h"
#include "Widget/IRmGUIWidget.h"
#include <cairo/cairo.h>

class RmGUIContext : public IRmGUIContext
{
public:
	virtual bool addWidget(RmRef<IRmGUIWidget> widget) override;
	virtual bool removeWidget(RmRef<IRmGUIWidget> widget) override;
	virtual void sendEvent(rmreactor source, rmevent event) override;
	virtual void postEvent(rmreactor source, rmevent event) override;
	virtual bool renderWidget(RmRaw<IRmGUIPainter> painter, RmRect client) override;

protected:
	RmRef<IRmGUIWidget> m_FocusWidget;
	RmVector<RmRef<IRmGUIWidget>> m_TopLevelList;
	cairo_surface_t* m_NativeSurface;
};