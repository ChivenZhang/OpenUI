/*=================================================
* Copyright © 2020-2025 ChivenZhang.
* All Rights Reserved.
* =====================Note=========================
*
*
* ====================History=======================
* Created by ChivenZhang@gmail.com.
*
* =================================================*/
#include "../UISlider.h"
#include "../UIContext.h"

/// @brief 
class UISliderPrivate : public UIElementPrivate
{
public:
	UIFloat2 MousePos;
	UIButtonRef Handle;
	UISliderStyle Style;
	UI::Orientation Orientation = UI::Horizontal;
	int32_t Maximum = 0, Minimum = 0, PageStep = 1, SingleStep = 1, SliderPosition = 0, Value = 0;
	bool SliderDown = false, Tracking = false;
	int32_t TickInterval = 0;
	UISlider::ticks_t TickPosition;

	UISignalAs<> OnSliderPressed;
	UISignalAs<> OnSliderReleased;
	UISignalAs<int32_t> OnSliderMoved;
	UISignalAs<int32_t> OnValueChanged;
	UISignalAs<int32_t, int32_t> OnRangeChanged;
};
#define PRIVATE() ((UISliderPrivate*)m_PrivateSlider)

UISlider::UISlider(UIContextRaw context)
	:
	UIElement(context)
{
	m_PrivateSlider = new UISliderPrivate;

	sliderMoved = &PRIVATE()->OnSliderMoved;
	sliderPressed = &PRIVATE()->OnSliderPressed;
	sliderReleased = &PRIVATE()->OnSliderReleased;
	valueChanged = &PRIVATE()->OnValueChanged;
	rangeChanged = &PRIVATE()->OnRangeChanged;

	PRIVATE()->Handle = UINew<UIButton>(context);
	addElement(PRIVATE()->Handle);
	PRIVATE()->Handle->setFixedSize(16, 16);

	PRIVATE()->Handle->pressed->connect(this, [=]() { PRIVATE()->OnSliderPressed.signal(); });
	PRIVATE()->Handle->released->connect(this, [=]() {
		if (PRIVATE()->Tracking == false) if (PRIVATE()->Value != PRIVATE()->SliderPosition) PRIVATE()->OnValueChanged.signal(PRIVATE()->Value);
		PRIVATE()->OnSliderReleased.signal();
		});
}

UISlider::~UISlider()
{
	delete m_PrivateSlider; m_PrivateSlider = nullptr;
}

void UISlider::arrange(UIRect client)
{
	PRIVATE()->Handle->setFixedSize(16 * getContext()->getConfig().DisplayScale, 16 * getContext()->getConfig().DisplayScale);
}

bool UISlider::filter(UIReactorRaw source, UIEventRaw _event)
{
	switch (_event->Type)
	{
	case UIHash("MouseDown"):
	{
		auto event = UICast<UIMouseEvent>(_event);
		if (PRIVATE()->Handle->inBounds(event->X, event->Y))
		{
			PRIVATE()->SliderPosition = std::clamp(PRIVATE()->Value, getMinimum(), getMaximum());
			PRIVATE()->MousePos = UIFloat2{ (float)event->X, (float)event->Y };
		}
	} break;
	case UIHash("MouseMove"):
	{
		auto event = UICast<UIMouseEvent>(_event);
		if (PRIVATE()->Handle->getDown())
		{
			auto oldValue = std::clamp(PRIVATE()->Value, getMinimum(), getMaximum());
			auto offset = UIFloat2{event->X - PRIVATE()->MousePos.X, event->Y - PRIVATE()->MousePos.Y};
			if (PRIVATE()->Orientation == UI::Horizontal)
			{
				auto range = getMaximum() - getMinimum();
				auto sliderWidth = PRIVATE()->Handle->getWidth();
				auto sliderPosition = (getWidth() - sliderWidth) * PRIVATE()->SliderPosition / range;
				auto newPosition = sliderPosition + offset.X;
				auto newValue = newPosition * range / (getWidth() - sliderWidth);
				if (std::isinf(newValue)) PRIVATE()->Value = 0;
				else PRIVATE()->Value = newValue;
			}
			else
			{
				auto range = getMaximum() - getMinimum();
				auto sliderHeight = PRIVATE()->Handle->getHeight();
				auto sliderPosition = (getHeight() - sliderHeight) * PRIVATE()->SliderPosition / range;
				auto newPosition = sliderPosition + offset.Y;
				auto newValue = newPosition * range / (getHeight() - sliderHeight);
				if (std::isinf(newValue)) PRIVATE()->Value = 0;
				else PRIVATE()->Value = newValue;
			}
			PRIVATE()->Value = std::clamp<int32_t>(PRIVATE()->Value, PRIVATE()->Minimum, PRIVATE()->Maximum);
			PRIVATE()->OnSliderMoved.signal(PRIVATE()->Value);
			if (PRIVATE()->Tracking) if (PRIVATE()->Value != oldValue) PRIVATE()->OnValueChanged.signal(PRIVATE()->Value);

			if (getContext()) getContext()->layoutElement();
		}
	} break;
	}
	return false;
}

void UISlider::layout(UIRect client)
{
	if (PRIVATE()->Orientation == UI::Horizontal)
	{
		auto range = getMaximum() - getMinimum();
		auto sliderWidth = PRIVATE()->Handle->getWidth();
		auto sliderPosition = (getWidth() - sliderWidth) * getValue() / range;
		if (std::isnan(sliderPosition)) sliderPosition = 0;
		auto rect = UIRect{ sliderPosition + 1, 1, sliderWidth - 2, client.H - 2 };
		PRIVATE()->Handle->setLocalBounds(rect);
		PRIVATE()->Handle->setViewport({ client.X + rect.X, client.Y + rect.Y, rect.W, rect.H });
	}
	else
	{
		auto range = getMaximum() - getMinimum();
		auto sliderHeight = PRIVATE()->Handle->getHeight();
		auto sliderPosition = (getHeight() - sliderHeight) * getValue() / range;
		if (std::isnan(sliderPosition)) sliderPosition = 0;
		auto rect = UIRect{ 1, sliderPosition + 1, client.W - 2, sliderHeight - 2 };
		PRIVATE()->Handle->setLocalBounds(rect);
		PRIVATE()->Handle->setViewport({ client.X + rect.X, client.Y + rect.Y, rect.W, rect.H });
	}
}

void UISlider::paint(UIRect client, UIPainterRaw painter)
{
	UIElement::paint(client, painter);
	painter->setPen(PRIVATE()->Style.Pen);
	painter->setBrush(PRIVATE()->Style.Brush);

	if (PRIVATE()->Orientation == UI::Horizontal)
	{
		painter->drawLine(client.X + 1, client.Y + client.H * 0.5f, client.X + 1 + client.W - 2, client.Y + client.H * 0.5f);
	}
	else
	{
		painter->drawLine(client.X + client.W * 0.5f, client.Y + 1, client.X + client.W * 0.5f, client.Y + 1 + client.H - 2);
	}
}

void UISlider::removeElement()
{
	UIList<UIElementRef> result;
	for (size_t i = 1; i < getChildren().size(); ++i) result.push_back(getChildren()[i]);
	for (size_t i = 0; i < result.size(); ++i) removeElement(result[i]);
}

UI::Orientation UISlider::getOrientation() const
{
	return PRIVATE()->Orientation;
}

void UISlider::setOrientation(UI::Orientation value)
{
	PRIVATE()->Orientation = value;
}

int32_t UISlider::getMinimum() const
{
	return PRIVATE()->Minimum;
}

int32_t UISlider::getMaximum() const
{
	return PRIVATE()->Maximum;
}

int32_t UISlider::getRange() const
{
	return std::abs(getMaximum() - getMinimum());
}

void UISlider::setRange(int32_t min, int32_t max)
{
	PRIVATE()->Minimum = min;
	PRIVATE()->Maximum = max;
	PRIVATE()->OnRangeChanged.signal(min, max);
}

int32_t UISlider::getValue() const
{
	return PRIVATE()->Value;
}

void UISlider::setValue(int32_t value)
{
	PRIVATE()->Value = value;
}

bool UISlider::getTracking() const
{
	return PRIVATE()->Tracking;
}

void UISlider::setTracking(bool value)
{
	PRIVATE()->Tracking = value;
}

int32_t UISlider::getSingleStep() const
{
	return PRIVATE()->SingleStep;
}

void UISlider::setSingleStep(int32_t value)
{
	PRIVATE()->SingleStep = std::max(1, value);
}

int32_t UISlider::getTickInterval() const
{
	return PRIVATE()->TickInterval;
}

void UISlider::setTickInterval(int32_t value)
{
	PRIVATE()->TickInterval = value;
}

UISlider::ticks_t UISlider::getTickPosition() const
{
	return PRIVATE()->TickPosition;
}

void UISlider::setTickPosition(ticks_t value)
{
	PRIVATE()->TickPosition = value;
}

UIButtonRaw UISlider::getHandle() const
{
	return PRIVATE()->Handle.get();
}

UIString UISliderFactory::getTagName() const
{
	return "slider";
}

UIElementRef UISliderFactory::getElement(UIString style) const
{
	auto result = UINew<UISlider>(getContext());
	result->setStyleText(style);
	return result;
}