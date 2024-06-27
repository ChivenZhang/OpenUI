#include "RmGUIScrollBar.h"
#include "RmGUIScrollBar.h"
#include "RmGUIScrollBar.h"
#include "RmGUIScrollBar.h"
#include "RmGUIButton.h"

class RmGUIScrollBarPrivate : public RmGUIWidgetPrivate
{
public:
	int32_t Maximum = 0, Minimum = 0, PageStep = 1, SingleStep = 1, SliderPosition = 0, Value = 0;
	bool Horizontal = false, SliderDown = false, Tracking = false;
	RmFloat2 MousePos;
	RmGUIButtonRef Slider;
	RmGUISignalAs<> OnSliderPressed;
	RmGUISignalAs<> OnSliderReleased;
	RmGUISignalAs<int32_t> OnSliderMoved;
	RmGUISignalAs<int32_t> OnValueChanged;
	RmGUISignalAs<int32_t, int32_t> OnRangeChanged;
};
#define PRIVATE() ((RmGUIScrollBarPrivate*)m_PrivateScrollBar)

RmGUIScrollBar::RmGUIScrollBar(IRmGUIWidgetRaw parent)
	:
	RmGUIControl(parent),
	m_PrivateScrollBar(nullptr)
{
	m_PrivateScrollBar = new RmGUIScrollBarPrivate;
	sliderMoved = &PRIVATE()->OnSliderMoved;
	sliderPressed = &PRIVATE()->OnSliderPressed;
	sliderReleased = &PRIVATE()->OnSliderReleased;
	valueChanged = &PRIVATE()->OnValueChanged;
	rangeChanged = &PRIVATE()->OnRangeChanged;
	PRIVATE()->Slider = RmNew<RmGUIButton>();
	addWidget(PRIVATE()->Slider);

	PRIVATE()->Slider->pressed->connect(this, [=]() { PRIVATE()->OnSliderPressed.emit(); });
	PRIVATE()->Slider->released->connect(this, [=]() {
		if (PRIVATE()->Tracking == false) if (PRIVATE()->Value != PRIVATE()->SliderPosition) PRIVATE()->OnValueChanged.emit(PRIVATE()->Value);
		PRIVATE()->OnSliderReleased.emit();
		});
}

RmGUIScrollBar::~RmGUIScrollBar()
{
	delete m_PrivateScrollBar; m_PrivateScrollBar = nullptr;
}

void RmGUIScrollBar::layout(RmRectRaw client)
{
	if (PRIVATE()->Horizontal)
	{
		auto range = std::max<float>(getWidth(), getMaximum() - getMinimum());
		auto sliderWidth = getWidth() * getWidth() / range;
		auto sliderPosition = (getWidth() - sliderWidth) * PRIVATE()->Value / range;
		PRIVATE()->Slider->setRect({ client->X + sliderPosition + 1, client->Y + 1, sliderWidth - 2, client->H - 2 });
	}
	else
	{
		auto range = std::max<float>(getHeight(), getMaximum() - getMinimum());
		auto sliderHeight = getHeight() * getHeight() / range;
		auto sliderPosition = (getHeight() - sliderHeight) * PRIVATE()->Value / range;
		PRIVATE()->Slider->setRect({ client->X + 1, client->Y + sliderPosition + 1, client->W - 2, sliderHeight - 2 });
	}
	auto rect = PRIVATE()->Slider->getRect();
	auto viewport = getViewport();
	PRIVATE()->Slider->setViewport(RmOverlap(viewport, rect));
}

void RmGUIScrollBar::paint(IRmGUIPainterRaw painter, RmRectRaw client)
{
	RmGUIWidget::paint(painter, client);
	painter->setPen({ .Color = { 108 / 255.0f, 110 / 255.0f, 111 / 255.0f, 1.0f }, });
	painter->setBrush({ .Color = { 238 / 255.0f, 238 / 255.0f, 242 / 255.0f, 1.0f }, });
	painter->drawRect(client->X + 1, client->Y + 1, client->W - 2, client->H - 2);
}

void RmGUIScrollBar::removeWidget()
{
	RmVector<IRmGUIWidgetRef> result;
	for (size_t i = 1; i < getChildren().size(); ++i) result.push_back(getChildren()[i]);
	for (size_t i = 0; i < result.size(); ++i) removeWidget(result[i]);
}

bool RmGUIScrollBar::getOrientation() const
{
	return PRIVATE()->Horizontal;
}

void RmGUIScrollBar::setOrientation(bool horizontal)
{
	PRIVATE()->Horizontal = horizontal;
}

int32_t RmGUIScrollBar::getMinimum() const
{
	return PRIVATE()->Minimum;
}

int32_t RmGUIScrollBar::getMaximum() const
{
	return PRIVATE()->Maximum;
}

void RmGUIScrollBar::setRange(int32_t min, int32_t max)
{
	PRIVATE()->Minimum = min;
	PRIVATE()->Maximum = max;
	PRIVATE()->OnRangeChanged.emit(min, max);
}

int32_t RmGUIScrollBar::getValue() const
{
	return PRIVATE()->Value;
}

void RmGUIScrollBar::setValue(int32_t value)
{
	PRIVATE()->Value = std::clamp(value, PRIVATE()->Minimum, PRIVATE()->Maximum);
}

bool RmGUIScrollBar::getTracking() const
{
	return PRIVATE()->Tracking;
}

void RmGUIScrollBar::setTracking(bool value)
{
	PRIVATE()->Tracking = value;
}

int32_t RmGUIScrollBar::getSingleStep() const
{
	return PRIVATE()->SingleStep;
}

void RmGUIScrollBar::setSingleStep(int32_t value)
{
	PRIVATE()->SingleStep = std::max(1, value);
}

void RmGUIScrollBar::mousePressEvent(IRmGUIMouseEventRaw event)
{
	if (PRIVATE()->Slider->getDown())
	{
		PRIVATE()->SliderPosition = PRIVATE()->Value;
		PRIVATE()->MousePos = RmFloat2(event->X, event->Y);
	}
}

void RmGUIScrollBar::mouseMoveEvent(IRmGUIMouseEventRaw event)
{
	if (PRIVATE()->Slider->getDown())
	{
		auto oldValue = PRIVATE()->Value;
		auto offset = RmFloat2(event->X - PRIVATE()->MousePos.X, event->Y - PRIVATE()->MousePos.Y);
		if (PRIVATE()->Horizontal)
		{
			auto range = std::max<float>(getWidth(), getMaximum() - getMinimum());
			auto sliderWidth = getWidth() * getWidth() / range;
			auto sliderPosition = (getWidth() - sliderWidth) * PRIVATE()->SliderPosition / range;
			auto newPosition = sliderPosition + offset.X;
			auto newValue = newPosition * range / (getWidth() - sliderWidth);
			if (std::isinf(newValue)) PRIVATE()->Value = 0;
			else PRIVATE()->Value = newValue;
		}
		else
		{
			auto range = std::max<float>(getHeight(), getMaximum() - getMinimum());
			auto sliderHeight = getHeight() * getHeight() / range;
			auto sliderPosition = (getHeight() - sliderHeight) * PRIVATE()->SliderPosition / range;
			auto newPosition = sliderPosition + offset.Y;
			auto newValue = newPosition * range / (getHeight() - sliderHeight);
			if (std::isinf(newValue)) PRIVATE()->Value = 0;
			else PRIVATE()->Value = newValue;
		}
		PRIVATE()->Value = std::clamp<int32_t>(PRIVATE()->Value, PRIVATE()->Minimum, PRIVATE()->Maximum);
		PRIVATE()->OnSliderMoved.emit(PRIVATE()->Value);
		if (PRIVATE()->Tracking) if (PRIVATE()->Value != oldValue) PRIVATE()->OnValueChanged.emit(PRIVATE()->Value);
	}
}