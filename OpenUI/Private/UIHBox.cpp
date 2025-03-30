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
class UIHboxPrivateData : public UIElementPrivate
{
public:
	UIHBoxStyle Style;
};
#define PRIVATE() ((UIHboxPrivateData*) m_PrivateHbox)

UIHBox::UIHBox(UIContextRaw context)
	:
	UIElement(context)
{
	m_PrivateHbox = new UIHboxPrivateData;
}

UIHBox::~UIHBox()
{
	delete m_PrivateHbox; m_PrivateHbox = nullptr;
}

void UIHBox::arrange(UIRect client)
{
	this->setFlexWrap(UI::FlexNoWrap);
	this->setAlignItems(UI::AlignStretch);
	this->setFlexDirection(UI::FlexDirectionRow);
	this->setJustifyContent(UI::JustifySpaceEvenly);

	for (size_t i = 0; i < getChildren().size(); ++i)
	{
		auto child = getChildren()[i];
		if (child->getFixedWidth().Unit == UI::UnitNone) child->setFlexGrow(1.0f);
		else child->setFlexGrow(0.0f);
		if (child->getFixedHeight().Unit == UI::UnitNone) child->setAlignSelf(UI::AlignStretch);
		else child->setAlignSelf(UI::AlignCenter);
	}
}

void UIHBox::paint(UIRect client, UIPainterRaw painter)
{
	UIElement::paint(client, painter);
	painter->setPen(UINoPen);
	painter->setBrush(PRIVATE()->Style.Brush);
	painter->drawRect(client.X, client.Y, client.W, client.H);
}

void UIHBox::repaint(UIRect client, UIPainterRaw painter)
{
	UIElement::repaint(client, painter);
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

UIString UIHBoxFactory::getTagName() const
{
	return "hbox";
}

UIElementRef UIHBoxFactory::getElement(UIString style) const
{
	auto result = UINew<UIHBox>(getContext());
	result->setStyleText(style);
	return result;
}