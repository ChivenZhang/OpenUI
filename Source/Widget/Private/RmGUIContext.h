#pragma once
#include "Widget/IRmGUIContext.h"

class RmGUIContext : public IRmGUIContext
{
public:
	void sendEvent(rmreactor source, rmevent event) override;
	void postEvent(rmreactor source, rmevent event) override;
};