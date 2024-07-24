#include "../UILine.h"

class UILinePrivate : public UIElementPrivate
{
public:
	UILineStyle Style;
};
#define PRIVATE() ((UILinePrivate*)m_PrivateLine)

UIHLine::UIHLine()
{
	m_PrivateLine = new UILinePrivate;
}

UIHLine::~UIHLine()
{
	delete m_PrivateLine; m_PrivateLine = nullptr;
}

void UIHLine::paint(UIRect client, UIPainterRaw painter)
{
	UIElement::paint(client, painter);
	painter->setPen(PRIVATE()->Style.Pen);
	painter->setBrush(PRIVATE()->Style.Brush);
	painter->drawLine(client.X + 1, client.Y + client.H * 0.5f, client.X + 1 + client.W - 2, client.Y + client.H * 0.5f);
}

UILineStyle UIHLine::getStyle() const
{
	return PRIVATE()->Style;
}

void UIHLine::setStyle(UILineStyle value)
{
	PRIVATE()->Style = value;
}

UIString UIHLineFactory::getTagName() const
{
	return "hline";
}

UIElementRef UIHLineFactory::getElement(UIString style) const
{
	auto result = UINew<UIHLine>();
	result->setStyleText(style);
	return result;
}

UIVLine::UIVLine()
{
	m_PrivateLine = new UILinePrivate;
}

UIVLine::~UIVLine()
{
	delete m_PrivateLine; m_PrivateLine = nullptr;
}

void UIVLine::paint(UIRect client, UIPainterRaw painter)
{
	UIElement::paint(client, painter);
	painter->setPen(PRIVATE()->Style.Pen);
	painter->setBrush(PRIVATE()->Style.Brush);
	painter->drawLine(client.X + client.W * 0.5f, client.Y + 1, client.X + client.W * 0.5f, client.Y + 1 + client.H - 2);
}

UILineStyle UIVLine::getStyle() const
{
	return PRIVATE()->Style;
}

void UIVLine::setStyle(UILineStyle value)
{
	PRIVATE()->Style = value;
}

UIString UIVLineFactory::getTagName() const
{
	return "vline";
}

UIElementRef UIVLineFactory::getElement(UIString style) const
{
	auto result = UINew<UIVLine>();
	result->setStyleText(style);
	return result;
}
