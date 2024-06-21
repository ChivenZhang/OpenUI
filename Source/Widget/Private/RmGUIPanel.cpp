#include "RmGUIPanel.h"

RmGUIPanel::RmGUIPanel(IRmGUIWidgetRaw parent)
	:
	RmGUIControl(parent)
{
}

void RmGUIPanel::paint(IRmGUIPainterRaw painter, RmRectRaw client)
{
	RmGUIWidget::paint(painter, client);
	painter->setPen({ 0, 0, 1, 0.5 });
	painter->drawRect(client->X + 1, client->Y + 1, client->W - 2, client->H - 2);
}