#include "IGLine.h"

struct IGLinePrivate : UIPrivate
{
	IGFrameStyle Style;
};
#define PRIVATE() ((IGLinePrivate*)m_Private)

IGHLine::IGHLine(UICanvasRaw canvas)
	:
	UIWidget(canvas)
{
	m_Private = new IGLinePrivate;
	PRIVATE()->Style.Pen = UIFramePen;
	PRIVATE()->Style.Brush = { .Style = UIBrush::NoBrush, };
}

IGHLine::~IGHLine()
{
	delete m_Private; m_Private = nullptr;
}

void IGHLine::paint(UIRect client, UIPainterRaw painter)
{
	UIWidget::paint(client, painter);
	painter->setPen(PRIVATE()->Style.Pen);
	painter->setBrush(PRIVATE()->Style.Brush);
	painter->drawLine(client.X, client.Y + client.H * 0.5f, client.X + client.W, client.Y + client.H * 0.5f);
}

IGFrameStyle IGHLine::getStyle() const
{
	return PRIVATE()->Style;
}

void IGHLine::setStyle(IGFrameStyle value)
{
	PRIVATE()->Style = value;
}

IGVLine::IGVLine(UICanvasRaw canvas)
	:
	UIWidget(canvas)
{
	m_Private = new IGLinePrivate;
	PRIVATE()->Style.Pen = UIFramePen;
	PRIVATE()->Style.Brush = { .Style = UIBrush::NoBrush, };
}

IGVLine::~IGVLine()
{
	delete m_Private; m_Private = nullptr;
}

void IGVLine::paint(UIRect client, UIPainterRaw painter)
{
	UIWidget::paint(client, painter);
	painter->setPen(PRIVATE()->Style.Pen);
	painter->setBrush(PRIVATE()->Style.Brush);
	painter->drawLine(client.X + client.W * 0.5f, client.Y, client.X + client.W * 0.5f, client.Y + client.H);
}

IGFrameStyle IGVLine::getStyle() const
{
	return PRIVATE()->Style;
}

void IGVLine::setStyle(IGFrameStyle value)
{
	PRIVATE()->Style = value;
}
#undef PRIVATE
