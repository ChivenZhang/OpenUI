#pragma once
#include "Widget/IRmGUIContext.h"
#include "Widget/IRmGUIWidget.h"

class RmGUIContext : public IRmGUIContext
{
public:
	virtual bool appendWidget(RmRef<IRmGUIWidget> widget) override;
	virtual bool removeWidget(RmRef<IRmGUIWidget> widget) override;
	virtual void sendEvent(rmreactor source, rmevent event) override;
	virtual void postEvent(rmreactor source, rmevent event) override;

protected:
	RmVector<RmRef<IRmGUIWidget>> m_TopLevelList;
};