#include "RmGUIScroll.h"

RmGUIScroll::RmGUIScroll(IRmGUIWidgetRaw parent)
	:
	RmGUILayout(parent)
{
}

void RmGUIScroll::layout(RmRectRaw client)
{
	RmGUIWidget::layout(client);
}

void RmGUIScroll::paint(IRmGUIPainterRaw painter, RmRectRaw client)
{
	RmGUIWidget::paint(painter, client);
}