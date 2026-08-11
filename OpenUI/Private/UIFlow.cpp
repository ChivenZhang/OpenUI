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
#include "../UIFlow.h"

struct UIFlowPrivate : UIPrivate
{
	UIFlowStyle Style;
};
#define PRIVATE() ((UIFlowPrivate*) m_Private)

UIFlow::UIFlow(UICanvasRaw canvas)
	:
	UIWidget(canvas)
{
	m_Private = new UIFlowPrivate;
}

UIFlow::~UIFlow()
{
	delete m_Private; m_Private = nullptr;
}

void UIFlow::arrange(UIRect client)
{
	this->setFlexWrap({UI_CSS_FLEX_WRAP_WRAP});
	this->setAlignItems({UI_CSS_ALIGN_ITEMS_FLEX_START});
	this->setAlignContent({UI_CSS_ALIGN_CONTENT_FLEX_START});
	this->setFlexDirection({UI_CSS_FLEX_DIRECTION_ROW});
	this->setJustifyContent({UI_CSS_JUSTIFY_CONTENT_FLEX_START});

	for (size_t i = 0; i < getWidgets().size(); ++i)
	{
		auto child = getWidgets()[i];
		if (child->getFixedWidth().Type == UI_CSS_WIDTH_AUTO) child->setFlexGrow({1.0f, UI_CSS_FLEX_GROW_NUMBER});
		else child->setFlexGrow({0.0f, UI_CSS_FLEX_GROW_NUMBER});
		if (child->getFixedHeight().Type == UI_CSS_HEIGHT_AUTO) child->setAlignSelf({UI_CSS_ALIGN_SELF_STRETCH});
		else child->setAlignSelf({UI_CSS_ALIGN_SELF_FLEX_START});
	}
}

void UIFlow::paint(UIRect client, UIPainterRaw painter)
{
	UIWidget::paint(client, painter);
	painter->setPen(PRIVATE()->Style.Pen);
	painter->setBrush(PRIVATE()->Style.Brush);
	painter->drawRect(client.X + 1, client.Y + 1, client.W - 2, client.H - 2);
}

UIFlowStyle UIFlow::getStyle() const
{
	return PRIVATE()->Style;
}

void UIFlow::setStyle(UIFlowStyle value)
{
	PRIVATE()->Style = value;
}