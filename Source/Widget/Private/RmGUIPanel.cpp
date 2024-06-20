#include "RmGUIPanel.h"

RmGUIPanel::RmGUIPanel(rmwidget parent)
	:
	RmGUIControl(parent)
{
}

void RmGUIPanel::paint(rmpainter painter, rmrect client)
{
	painter->setPen({ 0, 1, 1, 1 });
	painter->drawRect(client->X + 1, client->Y + 1, client->W - 2, client->H - 2);
	RmGUIWidget::paint(painter, client);
}