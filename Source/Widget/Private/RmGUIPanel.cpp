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

	painter->setFont({ .Size = 25 });
	painter->setPen({ .Color = { 1,0,0, 1.0f }, });
	painter->setBrush({ .Color = { 0,1,0, 1.0f }, });
	painter->drawText(client->X, client->Y, client->W, client->H, R"(金樽清酒斗十千，玉盘珍羞直万钱。
停杯投箸不能食，拔剑四顾心茫然。
欲渡黄河冰塞川，将登太行雪满山。
闲来垂钓碧溪上，忽复乘舟梦日边。
行路难，行路难，多歧路，今安在？
长风破浪会有时，直挂云帆济沧海。)");

}