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
#include "UILine.h"

struct UILinePrivate : UIPrivate
{
};
#define PRIVATE() ((UILinePrivate*)m_Private)

UIHLine::UIHLine(UICanvasRaw canvas)
	:
	UIWidget(canvas)
{
	m_Private = new UILinePrivate;
	writeFrame(getStyleNormal(), UICtrlLinePen, UINoBrush);
}

UIHLine::~UIHLine()
{
	delete m_Private; m_Private = nullptr;
}

void UIHLine::paint(UIRect client, UIPainterRaw painter)
{
	UIWidget::paint(client, painter);
	auto face = UIFaceOf(getStyleNormal(), client);
	painter->setPen(face.Border.Style == UIPen::NoPen ? UICtrlLinePen : face.Border);
	painter->setBrush(UINoBrush);
	float y = client.Y + client.H * 0.5f;
	painter->drawLine(client.X, y, client.X + client.W, y);
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
	writeFrame(getStyleNormal(), UICtrlLinePen, UINoBrush);
}

UIVLine::~UIVLine()
{
	delete m_Private; m_Private = nullptr;
}

void UIVLine::paint(UIRect client, UIPainterRaw painter)
{
	UIWidget::paint(client, painter);
	auto face = UIFaceOf(getStyleNormal(), client);
	painter->setPen(face.Border.Style == UIPen::NoPen ? UICtrlLinePen : face.Border);
	painter->setBrush(UINoBrush);
	float x = client.X + client.W * 0.5f;
	painter->drawLine(x, client.Y, x, client.Y + client.H);
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