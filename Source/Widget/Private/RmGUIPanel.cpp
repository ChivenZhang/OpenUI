#include "RmGUIPanel.h"

RmGUIPanel::RmGUIPanel(IRmGUIWidgetRaw parent)
	:
	RmGUIControl(parent)
{
}

void RmGUIPanel::paint(IRmGUIPainterRaw painter, RmRectRaw client)
{
	RmGUIWidget::paint(painter, client);
	painter->setPen({ .Color = { 204 / 255.0f, 216 / 255.0f, 219 / 255.0f, 1.0f }, });
	painter->setBrush({ .Color = { 255 / 255.0f, 255 / 255.0f, 255 / 255.0f, 1.0f }, });
	painter->drawRect(client->X + 1, client->Y + 1, client->W - 2, client->H - 2);

	painter->setBrush({ .Color = { 255 / 255.0f, 128 / 255.0f, 0 / 255.0f, 1.0f }, });
	painter->drawPie(client->X + 100, client->Y + 50, 200, 100, 0, 300);

	painter->drawRoundedRect(client->X + 200, client->Y + 50, 100, 100, 20, 20);

	painter->setPen({ .Style = RmPen::NoPen, .Color = { 1,0,0, 1.0f }, });
	painter->setBrush({ .Color = { 0,0,0, 1.0f }, });
	painter->drawText(client->X, client->Y + 20, 100, 20, 0, "AbCdEfGhIjKlMnOpQrStUvWxYz");
}