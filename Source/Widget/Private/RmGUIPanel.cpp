#include "RmGUIPanel.h"

RmGUIPanel::RmGUIPanel(IRmGUIWidgetRaw parent)
	:
	RmGUIControl(parent)
{
}

void RmGUIPanel::paint(IRmGUIPainterRaw painter, RmRectRaw client)
{
	RmGUIWidget::paint(painter, client);

	painter->setFont({ .Size = 25 });
	painter->setPen({ .Color = { 1,0,0, 1.0f }, });
	painter->setBrush({ .Color = { 0,1,0, 1.0f }, });
	painter->drawText(client->X, client->Y, client->W, client->H, "这是一段多行文本。当文本长度超过指定宽度时，pango 会自动进行换行。这是一段多行文本。当文本长度超过指定宽度时，pango 会自动进行换行。这是一段多行文本。当文本长度超过指定宽度时，pango 会自动进行换行。");
}