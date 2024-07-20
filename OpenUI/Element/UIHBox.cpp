#include "UIHBox.h"

/// @brief 
class UIHboxPrivateData : public UIElementPrivate
{
public:
	UIHBoxStyle Style;
};
#define PRIVATE() ((UIHboxPrivateData*) m_PrivateHbox)

UIHBox::UIHBox()
{
	m_PrivateHbox = new UIHboxPrivateData;

	PRIVATE()->Style.Pen = { .Color = { 108 / 255.0f, 110 / 255.0f, 111 / 255.0f, 1.0f }, };
	PRIVATE()->Style.Brush = { .Color = { 238 / 255.0f, 238 / 255.0f, 242 / 255.0f, 1.0f }, };
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
	painter->setPen(PRIVATE()->Style.Pen);
	painter->setBrush(PRIVATE()->Style.Brush);
	painter->drawRect(client.X + 1, client.Y + 1, client.W - 2, client.H - 2);
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
	auto result = UINew<UIHBox>();
	result->setStyleText(style);
	return result;
}