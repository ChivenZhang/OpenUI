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
#include "../UIHBox.h"

/// @brief 
struct UIHboxPrivateData : UIPrivate
{
};
#define PRIVATE() ((UIHboxPrivateData*) m_Private)

UIHBox::UIHBox(UICanvasRaw canvas)
	:
	UIWidget(canvas)
{
	m_Private = new UIHboxPrivateData;
}

UIHBox::~UIHBox()
{
	delete m_Private; m_Private = nullptr;
}

void UIHBox::arrange(UIRect client)
{
	this->setFlexWrap({UI_CSS_FLEX_WRAP_NOWRAP});
	this->setAlignItems({UI_CSS_ALIGN_ITEMS_STRETCH});
	this->setFlexDirection({UI_CSS_FLEX_DIRECTION_ROW});
	this->setJustifyContent({UI_CSS_JUSTIFY_CONTENT_SPACE_EVENLY});

	for (size_t i = 0; i < getWidgets().size(); ++i)
	{
		auto child = getWidgets()[i];
		// A minimum width holds the item at its content size. Only an item with neither a
		// fixed width nor a minimum width grows into the free space.
		if (child->getFixedWidth().Type == UI_CSS_WIDTH_AUTO && child->getMinWidth().Type == UI_CSS_MIN_WIDTH_AUTO)
			child->setFlexGrow({1.0f, UI_CSS_FLEX_GROW_NUMBER});
		else child->setFlexGrow({0.0f, UI_CSS_FLEX_GROW_NUMBER});
		if (child->getFixedHeight().Type == UI_CSS_HEIGHT_AUTO) child->setAlignSelf({UI_CSS_ALIGN_SELF_AUTO});
		else child->setAlignSelf({UI_CSS_ALIGN_SELF_CENTER});
	}
}

void UIHBox::paint(UIRect client, UIPainterRaw painter)
{
	UIWidget::paint(client, painter);

	UIPen pen;
	UIBrush brush;
	readFrame(getStyleNormal(), pen, brush);
	painter->setPen(pen);
	painter->setBrush(brush);
	painter->drawRect(client.X, client.Y, client.W, client.H);
}

UIHBoxStyle UIHBox::getStyle() const
{
	UIHBoxStyle result;
	readFrame(getStyleNormal(), result.Pen, result.Brush);
	return result;
}

void UIHBox::setStyle(UIHBoxStyle value)
{
	writeFrame(getStyleNormal(), value.Pen, value.Brush);
}