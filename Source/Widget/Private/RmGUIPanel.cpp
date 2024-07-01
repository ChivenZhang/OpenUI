#include "RmGUIPanel.h"

class RmGUIPanelPrivateData : public RmGUIWidgetPrivate
{
public:
	RmImage Image;
	RmVector<uint8_t> ImageData;
};
#define PRIVATE() ((RmGUIPanelPrivateData*) m_PrivatePanel)

RmGUIPanel::RmGUIPanel(IRmGUIWidgetRaw parent)
	:
	RmGUIControl(parent)
{
	m_PrivatePanel = new RmGUIPanelPrivateData;
	PRIVATE()->Image.Width = 100;
	PRIVATE()->Image.Height = 100;
	PRIVATE()->Image.Stride = PRIVATE()->Image.Width * 4;
	PRIVATE()->ImageData.resize(PRIVATE()->Image.Height * PRIVATE()->Image.Stride, 255);
	PRIVATE()->Image.Data = PRIVATE()->ImageData;
}

RmGUIPanel::~RmGUIPanel()
{
	delete m_PrivatePanel; m_PrivatePanel = nullptr;
}

void RmGUIPanel::paint(IRmGUIPainterRaw painter, RmRectRaw client)
{
	RmGUIWidget::paint(painter, client);

	painter->setFont({ .Size = 25 });
	painter->setPen({ .Color = { 1,0,0, 1.0f }, });
	painter->setBrush({ .Color = { 0,1,0, 1.0f }, });
	painter->drawText(client->X, client->Y, client->W, client->H, "هذا هو متعدد خط النص . عند طول النص يتجاوز العرض المحدد ، بانغو تلقائيا يكسر خط .这是一段多行文本。当文本长度超过指定宽度时，pango 会自动进行换行。这是一段多行文本。当文本长度超过指定宽度时，pango 会自动进行换行。");
}