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

class UIFlowPrivateData : public UIElementPrivate
{
public:
	UIFlowStyle Style;
};
#define PRIVATE() ((UIFlowPrivateData*) m_PrivateFlow)

UIFlow::UIFlow(UIContextRaw context)
	:
	UIElement(context)
{
	m_PrivateFlow = new UIFlowPrivateData;
}

UIFlow::~UIFlow()
{
	delete m_PrivateFlow; m_PrivateFlow = nullptr;
}

void UIFlow::arrange(UIRect client)
{
	this->setFlexWrap(UI::FlexDoWrap);
	this->setAlignItems(UI::AlignFlexStart);
	this->setAlignContent(UI::AlignFlexStart);
	this->setFlexDirection(UI::FlexDirectionRow);
	this->setJustifyContent(UI::JustifyFlexStart);

	for (size_t i = 0; i < getChildren().size(); ++i)
	{
		auto child = getChildren()[i];
		if (child->getFixedWidth().Unit == UI::UnitNone) child->setFlexGrow(1.0f);
		else child->setFlexGrow(0.0f);
		if (child->getFixedHeight().Unit == UI::UnitNone) child->setAlignSelf(UI::AlignStretch);
		else child->setAlignSelf(UI::AlignFlexStart);
	}
}

void UIFlow::paint(UIRect client, UIPainterRaw painter)
{
	UIElement::paint(client, painter);
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

UIString UIFlowFactory::getTagName() const
{
	return "flow";
}

UIElementRef UIFlowFactory::getElement(UIString style) const
{
	auto result = UINew<UIFlow>(getContext());
	result->setStyleText(style);
	return result;
}
