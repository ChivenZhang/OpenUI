#include "PCSeparator.h"

PCHSeparator::PCHSeparator(UICanvasRaw canvas)
	:
	UIWidget(canvas)
{
	setMinSize(8, 1);
	PCPlate(getStyleNormal(), PCPen(PCLine()), UINoBrush, UINoBrush, 0);
}

PCHSeparator::~PCHSeparator() = default;

void PCHSeparator::paint(UIRect client, UIPainterRaw painter)
{
	UIWidget::paint(client, painter);
	auto face = PCFaceOf(getStyleNormal(), client);
	painter->setPen(face.Border);
	painter->setBrush(UINoBrush);
	float y = client.Y + client.H * 0.5f;
	painter->drawLine(client.X, y, client.X + client.W, y);
}

PCVSeparator::PCVSeparator(UICanvasRaw canvas)
	:
	UIWidget(canvas)
{
	setMinSize(1, 8);
	PCPlate(getStyleNormal(), PCPen(PCLine()), UINoBrush, UINoBrush, 0);
}

PCVSeparator::~PCVSeparator() = default;

void PCVSeparator::paint(UIRect client, UIPainterRaw painter)
{
	UIWidget::paint(client, painter);
	auto face = PCFaceOf(getStyleNormal(), client);
	painter->setPen(face.Border);
	painter->setBrush(UINoBrush);
	float x = client.X + client.W * 0.5f;
	painter->drawLine(x, client.Y, x, client.Y + client.H);
}
