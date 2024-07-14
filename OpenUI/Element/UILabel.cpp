#include "UILabel.h"

class UILabelPrivate : public UIElementPrivate
{
public:

};
#define PRIVATE() ((UILabelPrivate*)m_PrivateLabel)

UILabel::UILabel()
	:
	m_PrivateLabel(nullptr)
{
	m_PrivateLabel = new UILabelPrivate;
}

UILabel::~UILabel()
{
	delete m_PrivateLabel; m_PrivateLabel = nullptr;
}

void UILabel::paint(UIRect client, UIPainterRaw painter)
{
	UIElement::paint(client, painter);
	painter->setPen({ .Color = { 108 / 255.0f, 110 / 255.0f, 111 / 255.0f, 1.0f }, });
	painter->setBrush({ .Color = { 238 / 255.0f, 238 / 255.0f, 242 / 255.0f, 1.0f }, });
	painter->drawRect(client.X + 1, client.Y + 1, client.W - 2, client.H - 2);
}