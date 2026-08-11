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
#include "../UIVBox.h"

/// @brief 
struct UIVBoxPrivate : UIPrivate
{
	UIVBoxStyle Style;
};
#define PRIVATE() ((UIVBoxPrivate*) m_Private)

UIVBox::UIVBox(UICanvasRaw canvas)
	:
	UIWidget(canvas)
{
	m_Private = new UIVBoxPrivate;
}

UIVBox::~UIVBox()
{
	delete m_Private; m_Private = nullptr;
}

void UIVBox::arrange(UIRect client)
{
	this->setFlexWrap({UI_CSS_FLEX_WRAP_NOWRAP});
	this->setAlignItems({UI_CSS_ALIGN_ITEMS_STRETCH});
	this->setFlexDirection({UI_CSS_FLEX_DIRECTION_COLUMN});
	this->setJustifyContent({UI_CSS_JUSTIFY_CONTENT_SPACE_EVENLY});

	for (size_t i = 0; i < getWidgets().size(); ++i)
	{
		auto child = getWidgets()[i];
		if (child->getFixedHeight().Type == UI_CSS_HEIGHT_AUTO) child->setFlexGrow({1.0f, UI_CSS_FLEX_GROW_NUMBER});
		else child->setFlexGrow({0.0f, UI_CSS_FLEX_GROW_NUMBER});
		if (child->getFixedWidth().Type == UI_CSS_WIDTH_AUTO) child->setAlignSelf({UI_CSS_ALIGN_ITEMS_STRETCH});
		else child->setAlignSelf({UI_CSS_ALIGN_SELF_CENTER});
	}
}

void UIVBox::paint(UIRect client, UIPainterRaw painter)
{
	UIWidget::paint(client, painter);
	painter->setPen(UINoPen);
	painter->setBrush(PRIVATE()->Style.Brush);
	painter->drawRect(client.X, client.Y, client.W, client.H);
}

void UIVBox::repaint(UIRect client, UIPainterRaw painter)
{
	UIWidget::repaint(client, painter);
	painter->setPen(PRIVATE()->Style.Pen);
	painter->setBrush(UINoBrush);
	painter->drawRect(client.X, client.Y, client.W, client.H);
}

UIVBoxStyle UIVBox::getStyle() const
{
	return PRIVATE()->Style;
}

void UIVBox::setStyle(UIVBoxStyle value)
{
	PRIVATE()->Style = value;
}