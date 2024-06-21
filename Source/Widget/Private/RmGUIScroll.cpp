#include "RmGUIScroll.h"

RmGUIScroll::RmGUIScroll(IRmGUIWidgetRaw parent)
	:
	RmGUILayout(parent)
{
}

void RmGUIScroll::layout(rmrect client)
{
	RmGUIWidget::layout(client);
}

void RmGUIScroll::paint(IRmGUIPainterRaw painter, rmrect client)
{
	RmGUIWidget::paint(painter, client);
}