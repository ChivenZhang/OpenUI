#include "RmGUIContext.h"

RmRef<IRmGUIContext> IRmGUIContext::GetInstance()
{
	return RmNew<RmGUIContext>();
}

void RmGUIContext::sendEvent(rmreactor source, rmevent event)
{
}

void RmGUIContext::postEvent(rmreactor source, rmevent event)
{
}
