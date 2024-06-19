#pragma once
#include "Widget/IRmGUIContext.h"

class RmGUIContext : public IRmGUIContext
{
public:
	virtual void sendEvent(rmreactor source, rmevent event) override;
	virtual void postEvent(rmreactor source, rmevent event) override;
};