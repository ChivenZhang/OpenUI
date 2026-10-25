#include "PCProgressBar.h"
#include <algorithm>

struct PCProgressBarPrivate : UIPrivate
{
	int32_t Minimum = 0;
	int32_t Maximum = 100;
	int32_t Value = 0;
	UIString Text;
	UIFont Font = PCFont(UIFont::AlignCenter | UIFont::AlignVCenter);
	UISignalAs<int32_t> OnValueChanged;
};
#define PRIVATE() ((PCProgressBarPrivate*)m_Private)

PCProgressBar::PCProgressBar(UICanvasRaw canvas)
	:
	UIWidget(canvas)
{
	m_Private = new PCProgressBarPrivate;
	valueChanged = &PRIVATE()->OnValueChanged;
	setMinSize(120, 18);
	PCPlate(getStyleNormal(), PCPen(PCAccent()), PCBrush(PCTrack()), PCBrush(PCInk()), 4);
	PCPlate(getStyleDisabled(), PCPen(PCDisabledLine()), PCBrush(PCDisabledFill()), PCBrush(PCDisabledInk()), 4);
	assignFontFamily(getStyleNormal(), PRIVATE()->Font.Family);
}

PCProgressBar::~PCProgressBar()
{
	delete m_Private; m_Private = nullptr;
}

void PCProgressBar::paint(UIRect client, UIPainterRaw painter)
{
	UIWidget::paint(client, painter);
	auto& style = getEnable() ? getStyleNormal() : getStyleDisabled();
	auto face = PCFaceOf(style, client);
	PCDrawFace(painter, client, { .Border = UINoPen, .Fill = face.Fill, .Radius = face.Radius });
	float span = std::max(1, PRIVATE()->Maximum - PRIVATE()->Minimum);
	float t = std::clamp((PRIVATE()->Value - PRIVATE()->Minimum) / span, 0.0f, 1.0f);
	float width = std::max(0.0f, (client.W - 2) * t);
	if (width > 0.5f)
	{
		UIColor bar = face.Border.Style == UIPen::NoPen ? PCAccent() : face.Border.Color;
		PCDrawFace(painter, { client.X + 1, client.Y + 1, width, std::max(0.0f, client.H - 2) }, { .Border = UINoPen, .Fill = PCBrush(bar), .Radius = std::max(0.0f, face.Radius - 1) });
	}
	UIFont font = PRIVATE()->Font;
	applyFontFamily(style, font);
	PCDrawText(painter, client, PRIVATE()->Text, font, face.Text);
}

int32_t PCProgressBar::getMinimum() const
{
	return PRIVATE()->Minimum;
}

int32_t PCProgressBar::getMaximum() const
{
	return PRIVATE()->Maximum;
}

void PCProgressBar::setRange(int32_t minimum, int32_t maximum)
{
	PRIVATE()->Minimum = minimum;
	PRIVATE()->Maximum = std::max(minimum, maximum);
	setValue(PRIVATE()->Value);
}

int32_t PCProgressBar::getValue() const
{
	return PRIVATE()->Value;
}

void PCProgressBar::setValue(int32_t value)
{
	value = std::clamp(value, PRIVATE()->Minimum, PRIVATE()->Maximum);
	if (PRIVATE()->Value == value) return;
	PRIVATE()->Value = value;
	PRIVATE()->OnValueChanged.signal(value);
}

void PCProgressBar::setText(UIString const& value)
{
	PRIVATE()->Text = value;
}
#undef PRIVATE
