#include "PCGroupBox.h"
#include <algorithm>

struct PCGroupBoxPrivate : UIPrivate
{
	UIString Title;
	UIFont Font = PCFont(UIFont::AlignLeft | UIFont::AlignVCenter);
};
#define PRIVATE() ((PCGroupBoxPrivate*)m_Private)

PCGroupBox::PCGroupBox(UICanvasRaw canvas)
	:
	UIWidget(canvas)
{
	m_Private = new PCGroupBoxPrivate;
	setPadding({ 12.0f }, { 28.0f }, { 12.0f }, { 12.0f });
	auto border = PCPen(PCLine());
	auto fill = PCBrush(PCPaper());
	auto text = PCBrush(PCInk());
	PCPlate(getStyleNormal(), border, fill, text, 4);
	PCPlate(getStyleHover(), border, fill, text, 4);
	PCPlate(getStyleDisabled(), PCPen(PCDisabledLine()), PCBrush(PCDisabledFill()), PCBrush(PCDisabledInk()), 4);
	assignFontFamily(getStyleNormal(), PRIVATE()->Font.Family);
}

PCGroupBox::~PCGroupBox()
{
	delete m_Private; m_Private = nullptr;
}

void PCGroupBox::arrange(UIRect client)
{
	(void)client;
	setFlexDirection({ UI_CSS_FLEX_DIRECTION_COLUMN });
	setAlignItems({ UI_CSS_ALIGN_ITEMS_STRETCH });
	setFlexWrap({ UI_CSS_FLEX_WRAP_NOWRAP });
}

void PCGroupBox::paint(UIRect client, UIPainterRaw painter)
{
	UIWidget::paint(client, painter);
	auto& style = getEnable() ? getStyleNormal() : getStyleDisabled();
	auto face = PCFaceOf(style, client);
	PCDrawFace(painter, client, face);
	UIFont font = PRIVATE()->Font;
	applyFontFamily(style, font);
	UIRect title{ client.X + 10, client.Y + 2, std::max(0.0f, client.W - 20), 22 };
	painter->setPen(UINoPen);
	painter->setBrush(face.Fill);
	painter->drawRect(title.X, title.Y, std::min(title.W, 160.0f), title.H);
	PCDrawText(painter, title, PRIVATE()->Title, font, face.Text);
}

UIString PCGroupBox::getTitle() const
{
	return PRIVATE()->Title;
}

void PCGroupBox::setTitle(UIString const& value)
{
	PRIVATE()->Title = value;
}
#undef PRIVATE
