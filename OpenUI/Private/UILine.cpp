/*=================================================
* Copyright © 2020-2025 ChivenZhang.
* All Rights Reserved.
* =====================Note=========================
*
*
* ====================History=======================
* Created by ChivenZhang@gmail.com.
*
* =================================================*/
#include "../UILine.h"

struct UILinePrivate : UIPrivate
{
};
#define PRIVATE() ((UILinePrivate*)m_Private)

UIHLine::UIHLine(UICanvasRaw canvas)
	:
	UIWidget(canvas)
{
	m_Private = new UILinePrivate;
	writeFrame(getStyleNormal(), UIFramePen, UINoBrush);
}

UIHLine::~UIHLine()
{
	delete m_Private; m_Private = nullptr;
}

void UIHLine::paint(UIRect client, UIPainterRaw painter)
{
	UIWidget::paint(client, painter);
	UIPen pen;
	UIBrush brush;
	readFrame(getStyleNormal(), pen, brush);
	painter->setPen(pen);
	painter->setBrush(brush);
	painter->drawLine(client.X + 1, client.Y + client.H * 0.5f, client.X + 1 + client.W - 2, client.Y + client.H * 0.5f);
}

UILineStyle UIHLine::getStyle() const
{
	UILineStyle result;
	readFrame(getStyleNormal(), result.Pen, result.Brush);
	return result;
}

void UIHLine::setStyle(UILineStyle value)
{
	writeFrame(getStyleNormal(), value.Pen, value.Brush);
}

UIVLine::UIVLine(UICanvasRaw canvas)
	:
	UIWidget(canvas)
{
	m_Private = new UILinePrivate;
	writeFrame(getStyleNormal(), UIFramePen, UINoBrush);
}

UIVLine::~UIVLine()
{
	delete m_Private; m_Private = nullptr;
}

void UIVLine::paint(UIRect client, UIPainterRaw painter)
{
	UIWidget::paint(client, painter);
	UIPen pen;
	UIBrush brush;
	readFrame(getStyleNormal(), pen, brush);
	painter->setPen(pen);
	painter->setBrush(brush);
	painter->drawLine(client.X + client.W * 0.5f, client.Y + 1, client.X + client.W * 0.5f, client.Y + 1 + client.H - 2);
}

UILineStyle UIVLine::getStyle() const
{
	UILineStyle result;
	readFrame(getStyleNormal(), result.Pen, result.Brush);
	return result;
}

void UIVLine::setStyle(UILineStyle value)
{
	writeFrame(getStyleNormal(), value.Pen, value.Brush);
}