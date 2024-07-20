#include "UIVBox.h"

class UIVBoxPrivateData : public UIElementPrivate
{
public:
	UIVBoxStyle Style;
};
#define PRIVATE() ((UIVBoxPrivateData*) m_PrivateVBox)

UIVBox::UIVBox()
{
	m_PrivateVBox = new UIVBoxPrivateData;

	PRIVATE()->Style.Pen = { .Color = { 108 / 255.0f, 110 / 255.0f, 111 / 255.0f, 1.0f }, };
	PRIVATE()->Style.Brush = { .Color = { 238 / 255.0f, 238 / 255.0f, 242 / 255.0f, 1.0f }, };
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
	painter->setPen(PRIVATE()->Style.Pen);
	painter->setBrush(PRIVATE()->Style.Brush);
	painter->drawRect(client.X + 1, client.Y + 1, client.W - 2, client.H - 2);
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

