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
class UIVBoxPrivate : public UIElementPrivate
{
public:
	UIVBoxStyle Style;
};
#define PRIVATE() ((UIVBoxPrivate*) m_PrivateVBox)

UIVBox::UIVBox()
{
	m_PrivateVBox = new UIVBoxPrivate;
}

UIVBox::~UIVBox()
{
	delete m_PrivateVBox; m_PrivateVBox = nullptr;
}

void UIVBox::arrange(UIRect client)
{
	this->setFlexWrap(UI::FlexNoWrap);
	this->setAlignItems(UI::AlignStretch);
	this->setFlexDirection(UI::FlexDirectionColumn);
	this->setJustifyContent(UI::JustifySpaceEvenly);

	for (size_t i = 0; i < getChildren().size(); ++i)
	{
		auto child = getChildren()[i];
		if (child->getFixedHeight().Unit == UI::UnitNone) child->setFlexGrow(1.0f);
		else child->setFlexGrow(0.0f);
		if (child->getFixedWidth().Unit == UI::UnitNone) child->setAlignSelf(UI::AlignStretch);
		else child->setAlignSelf(UI::AlignCenter);
	}
}

void UIVBox::paint(UIRect client, UIPainterRaw painter)
{
	UIElement::paint(client, painter);
	painter->setPen(UINoPen);
	painter->setBrush(PRIVATE()->Style.Brush);
	painter->drawRect(client.X, client.Y, client.W, client.H);
}

void UIVBox::repaint(UIRect client, UIPainterRaw painter)
{
	UIElement::repaint(client, painter);
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

UIString UIVBoxFactory::getTagName() const
{
	return "vbox";
}

UIElementRef UIVBoxFactory::getElement(UIString style) const
{
	auto result = UINew<UIVBox>();
	result->setStyleText(style);
	return result;
}

