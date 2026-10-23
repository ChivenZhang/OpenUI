#include "IGSlider.h"
#include "IGStyle.h"
#include "OpenUI/UICanvas.h"
#include <algorithm>
#include <cmath>

struct IGSliderPrivate : UIPrivate
{
	int32_t Minimum = 0, Maximum = 100, Value = 0;
	UISignalAs<int32_t> OnValueChanged;
	bool Dragging = false;

	void pick(IGSlider* self, float x)
	{
		float width = std::max(1.0f, self->getWidth());
		float t = std::clamp((x - self->getPosX()) / width, 0.0f, 1.0f);
		int32_t value = Minimum + (int32_t)std::lround(t * (Maximum - Minimum));
		self->setValue(value);
	}
};
#define PRIVATE() ((IGSliderPrivate*)m_Private)

IGSlider::IGSlider(UICanvasRaw canvas)
	:
	UIWidget(canvas)
{
	m_Private = new IGSliderPrivate;
	valueChanged = &PRIVATE()->OnValueChanged;
}

IGSlider::~IGSlider()
{
	delete m_Private; m_Private = nullptr;
}

void IGSlider::paint(UIRect client, UIPainterRaw painter)
{
	UIWidget::paint(client, painter);
	float mid = client.Y + client.H * 0.5f;
	painter->setPen(UIPen{ .Color { 58 / 255.0f, 50 / 255.0f, 40 / 255.0f, 1.0f }, .Width = 4.0f, });
	painter->setBrush({ .Style = UIBrush::NoBrush, });
	painter->drawLine(client.X + 8, mid, client.X + client.W - 8, mid);

	float span = std::max(1, PRIVATE()->Maximum - PRIVATE()->Minimum);
	float t = (PRIVATE()->Value - PRIVATE()->Minimum) / span;
	float handle = client.X + 8 + t * std::max(0.0f, client.W - 16);
	UIRect knob{ handle - 7, mid - 7, 14, 14 };
	IGDrawBox(painter, knob, UIAccentPen, UIFrameBrush, 7);
}

int32_t IGSlider::getMinimum() const
{
	return PRIVATE()->Minimum;
}

int32_t IGSlider::getMaximum() const
{
	return PRIVATE()->Maximum;
}

void IGSlider::setRange(int32_t min, int32_t max)
{
	PRIVATE()->Minimum = min;
	PRIVATE()->Maximum = std::max(min, max);
	setValue(PRIVATE()->Value);
}

int32_t IGSlider::getValue() const
{
	return PRIVATE()->Value;
}

void IGSlider::setValue(int32_t value)
{
	value = std::clamp(value, PRIVATE()->Minimum, PRIVATE()->Maximum);
	if (PRIVATE()->Value == value) return;
	PRIVATE()->Value = value;
	PRIVATE()->OnValueChanged.signal(value);
}

void IGSlider::mousePressEvent(UIMouseEventRaw event)
{
	if (getEnable() == false || event == nullptr || event->Button != UIInputEnum::MOUSE_BUTTON_LEFT) return;
	if (inBounds(event->X, event->Y) == false) return;
	PRIVATE()->Dragging = true;
	PRIVATE()->pick(this, (float)event->X);
	event->Accept = true;
}

void IGSlider::mouseReleaseEvent(UIMouseEventRaw event)
{
	if (event == nullptr || event->Button != UIInputEnum::MOUSE_BUTTON_LEFT) return;
	PRIVATE()->Dragging = false;
}

void IGSlider::mouseMoveEvent(UIMouseEventRaw event)
{
	if (event == nullptr || PRIVATE()->Dragging == false) return;
	PRIVATE()->pick(this, (float)event->X);
}
#undef PRIVATE
