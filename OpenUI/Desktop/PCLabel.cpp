#include "PCLabel.h"
#include <algorithm>

struct PCLabelPrivate : UIPrivate
{
	UIString Text;
	UIFont Font = PCFont(UIFont::AlignLeft | UIFont::AlignVCenter);
};
#define PRIVATE() ((PCLabelPrivate*)m_Private)

PCLabel::PCLabel(UICanvasRaw canvas)
	:
	UIWidget(canvas)
{
	m_Private = new PCLabelPrivate;
	setMinHeight(18);
	auto text = PCBrush(PCInk());
	PCPlate(getStyleNormal(), UINoPen, UINoBrush, text, 0);
	PCPlate(getStyleHover(), UINoPen, UINoBrush, text, 0);
	PCPlate(getStyleDisabled(), UINoPen, UINoBrush, PCBrush(PCDisabledInk()), 0);
	assignFontFamily(getStyleNormal(), PRIVATE()->Font.Family);
}

PCLabel::~PCLabel()
{
	delete m_Private; m_Private = nullptr;
}

void PCLabel::paint(UIRect client, UIPainterRaw painter)
{
	UIWidget::paint(client, painter);
	auto& style = getEnable() ? getStyleNormal() : getStyleDisabled();
	auto face = PCFaceOf(style, client);
	PCDrawFace(painter, client, face);
	UIFont font = PRIVATE()->Font;
	applyFontFamily(style, font);
	PCDrawText(painter, client, PRIVATE()->Text, font, face.Text);
}

UIString PCLabel::getText() const
{
	return PRIVATE()->Text;
}

void PCLabel::setText(UIString const& value)
{
	PRIVATE()->Text = value;
}

void PCLabel::setFont(UIFont const& font)
{
	PRIVATE()->Font = font;
	assignFontFamily(getStyleNormal(), font.Family);
}
#undef PRIVATE
