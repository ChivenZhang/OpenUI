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
	UIHBoxStyle Style;
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
		if (child->getFixedWidth().Type == UI_CSS_WIDTH_AUTO) child->setFlexGrow({1.0f, UI_CSS_FLEX_GROW_NUMBER});
		else child->setFlexGrow({0.0f, UI_CSS_FLEX_GROW_NUMBER});
		if (child->getFixedHeight().Type == UI_CSS_HEIGHT_AUTO) child->setAlignSelf({UI_CSS_ALIGN_ITEMS_STRETCH});
		else child->setAlignSelf({UI_CSS_ALIGN_SELF_CENTER});
	}
}

void UIHBox::paint(UIRect client, UIPainterRaw painter)
{
	UIWidget::paint(client, painter);
	painter->setPen(UINoPen);
	painter->setBrush(PRIVATE()->Style.Brush);
	painter->drawRect(client.X, client.Y, client.W, client.H);
}

void UIHBox::repaint(UIRect client, UIPainterRaw painter)
{
	UIWidget::repaint(client, painter);
	painter->setPen(PRIVATE()->Style.Pen);
	painter->setBrush(UINoBrush);
	painter->drawRect(client.X, client.Y, client.W, client.H);
}

UIHBoxStyle UIHBox::getStyle() const
{
	return PRIVATE()->Style;
}

void UIHBox::setStyle(UIHBoxStyle value)
{
	PRIVATE()->Style = value;
}