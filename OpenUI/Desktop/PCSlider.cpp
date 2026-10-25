#include "PCSlider.h"
#include <algorithm>
#include <cmath>

struct PCSliderPrivate : UIPrivate
{
	int32_t Minimum = 0;
	int32_t Maximum = 100;
	int32_t Value = 0;
	UISignalAs<int32_t> OnValueChanged;
	UISignalAs<int32_t> OnSliderMoved;
	bool Dragging = false;
	bool Hovered = false;

	void pick(PCSlider* self, float x)
	{
		float width = std::max(1.0f, self->getWidth() - 16.0f);
		float t = std::clamp((x - self->getPosX() - 8.0f) / width, 0.0f, 1.0f);
		int32_t value = Minimum + (int32_t)std::lround(t * (Maximum - Minimum));
		if (value == Value) return;
		Value = std::clamp(value, Minimum, Maximum);
		OnSliderMoved.signal(Value);
		OnValueChanged.signal(Value);
	}
};
#define PRIVATE() ((PCSliderPrivate*)m_Private)

PCSlider::PCSlider(UICanvasRaw canvas)
	:
	UIWidget(canvas)
{
	m_Private = new PCSliderPrivate;
	valueChanged = &PRIVATE()->OnValueChanged;
	sliderMoved = &PRIVATE()->OnSliderMoved;
	setMinSize(120, 20);
	PCPlate(getStyleNormal(), PCPen(PCAccent()), PCBrush(PCTrack()), PCBrush(PCPaper()), 7);
	PCPlate(getStyleHover(), PCPen(PCAccentHover()), PCBrush(PCTrack()), PCBrush(PCPaper()), 7);
	PCPlate(getStyleActive(), PCPen(PCAccentPress()), PCBrush(PCTrack()), PCBrush(PCHover()), 7);
	PCPlate(getStyleDisabled(), PCPen(PCDisabledLine()), PCBrush(PCDisabledFill()), PCBrush(PCDisabledFill()), 7);
}

PCSlider::~PCSlider()
{
	delete m_Private; m_Private = nullptr;
}

void PCSlider::paint(UIRect client, UIPainterRaw painter)
{
	UIWidget::paint(client, painter);
	UIWidgetStyle& style = getEnable() == false ? getStyleDisabled()
		: PRIVATE()->Dragging ? getStyleActive()
		: PRIVATE()->Hovered ? getStyleHover()
		: getStyleNormal();
	auto face = PCFaceOf(style, client);
	float mid = client.Y + client.H * 0.5f;
	painter->setPen(PCPen(face.Fill.Style == UIBrush::NoBrush ? PCTrack() : face.Fill.Color, 4));
	painter->setBrush(UINoBrush);
	painter->drawLine(client.X + 8, mid, client.X + client.W - 8, mid);
	float span = std::max(1, PRIVATE()->Maximum - PRIVATE()->Minimum);
	float t = (PRIVATE()->Value - PRIVATE()->Minimum) / span;
	float handle = client.X + 8 + t * std::max(0.0f, client.W - 16);
	UIRect knob{ handle - 7, mid - 7, 14, 14 };
	PCDrawFace(painter, knob, { .Border = face.Border, .Fill = face.Text, .Radius = 7 });
}

int32_t PCSlider::getMinimum() const
{
	return PRIVATE()->Minimum;
}

int32_t PCSlider::getMaximum() const
{
	return PRIVATE()->Maximum;
}

void PCSlider::setRange(int32_t minimum, int32_t maximum)
{
	PRIVATE()->Minimum = minimum;
	PRIVATE()->Maximum = std::max(minimum, maximum);
	setValue(PRIVATE()->Value);
}

int32_t PCSlider::getValue() const
{
	return PRIVATE()->Value;
}

void PCSlider::setValue(int32_t value)
{
	value = std::clamp(value, PRIVATE()->Minimum, PRIVATE()->Maximum);
	if (PRIVATE()->Value == value) return;
	PRIVATE()->Value = value;
	PRIVATE()->OnValueChanged.signal(value);
}

void PCSlider::mousePressEvent(UIMouseEventRaw event)
{
	if (getEnable() == false || event == nullptr || event->Button != UIInputEnum::MOUSE_BUTTON_LEFT) return;
	if (inBounds(event->X, event->Y) == false) return;
	PRIVATE()->Dragging = true;
	PRIVATE()->pick(this, (float)event->X);
	event->Accept = true;
}

void PCSlider::mouseReleaseEvent(UIMouseEventRaw event)
{
	if (event == nullptr || event->Button != UIInputEnum::MOUSE_BUTTON_LEFT) return;
	PRIVATE()->Dragging = false;
}

void PCSlider::mouseMoveEvent(UIMouseEventRaw event)
{
	if (event == nullptr) return;
	PRIVATE()->Hovered = inBounds(event->X, event->Y);
	if (PRIVATE()->Dragging) PRIVATE()->pick(this, (float)event->X);
}
#undef PRIVATE
