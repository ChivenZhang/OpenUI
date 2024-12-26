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
#include "../UIScrollBar.h"
#include "../UIContext.h"

class UIScrollBarPrivate : public UIElementPrivate
{
public:
	UIFloat2 MousePos;
	UIButtonRef Handle;
	UIScrollBarStyle Style;
	UI::Orientation Orientation = UI::Horizontal;
	bool SliderDown = false, Tracking = false;
	int32_t Maximum = 0, Minimum = 0, PageStep = 1, SingleStep = 1, SliderPosition = 0, Value = 0;

	UISignalAs<> OnSliderPressed;
	UISignalAs<> OnSliderReleased;
	UISignalAs<int32_t> OnSliderMoved;
	UISignalAs<int32_t> OnValueChanged;
	UISignalAs<int32_t, int32_t> OnRangeChanged;
};
#define PRIVATE() ((UIScrollBarPrivate*)m_PrivateScrollBar)

UIScrollBar::UIScrollBar()
{
	m_PrivateScrollBar = new UIScrollBarPrivate;

	sliderMoved = &PRIVATE()->OnSliderMoved;
	sliderPressed = &PRIVATE()->OnSliderPressed;
	sliderReleased = &PRIVATE()->OnSliderReleased;
	valueChanged = &PRIVATE()->OnValueChanged;
	rangeChanged = &PRIVATE()->OnRangeChanged;

	PRIVATE()->Handle = UINew<UIButton>();
	addElement(PRIVATE()->Handle);

	PRIVATE()->Handle->pressed->connect(this, [=]() { PRIVATE()->OnSliderPressed.signal(); });
	PRIVATE()->Handle->released->connect(this, [=]() {
		if (PRIVATE()->Tracking == false) if (PRIVATE()->Value != PRIVATE()->SliderPosition) PRIVATE()->OnValueChanged.signal(PRIVATE()->Value);
		PRIVATE()->OnSliderReleased.signal();
		});
	PRIVATE()->Handle->setEventFilter(this);
}

UIScrollBar::~UIScrollBar()
{
	delete m_PrivateScrollBar; m_PrivateScrollBar = nullptr;
}

bool UIScrollBar::filter(UIReactorRaw source, UIEventRaw _event)
{
	switch (_event->Type)
	{
	case UIHash("MouseDown"):
	{
		auto event = UICast<UIMouseEvent>(_event);
		if (PRIVATE()->Handle->inBounds(event->X, event->Y))
		{
			PRIVATE()->SliderPosition = PRIVATE()->Value;
			PRIVATE()->MousePos = UIFloat2{(float)event->X, (float)event->Y};
		}
	} break;
	case UIHash("MouseMove"):
	{
		auto event = UICast<UIMouseEvent>(_event);
		if (PRIVATE()->Handle->getDown())
		{
			auto oldValue = PRIVATE()->Value;
			auto offset = UIFloat2{event->X - PRIVATE()->MousePos.X, event->Y - PRIVATE()->MousePos.Y};
			if (PRIVATE()->Orientation == UI::Horizontal)
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
			PRIVATE()->OnSliderMoved.signal(PRIVATE()->Value);
			if (PRIVATE()->Tracking) if (PRIVATE()->Value != oldValue) PRIVATE()->OnValueChanged.signal(PRIVATE()->Value);

			if (getContext()) getContext()->layoutElement();
		}
	} break;
	}
	return false;
}

void UIScrollBar::layout(UIRect client)
{
	PRIVATE()->Value = std::clamp(PRIVATE()->Value, PRIVATE()->Minimum, PRIVATE()->Maximum);

	if (PRIVATE()->Orientation == UI::Horizontal)
	{
		auto range = std::max<float>(getWidth(), getMaximum() - getMinimum());
		auto sliderWidth = getWidth() * getWidth() / range;
		auto sliderPosition = (getWidth() - sliderWidth) * PRIVATE()->Value / range;
		if (std::isnan(sliderPosition)) sliderPosition = 0;
		auto rect = UIRect{ sliderPosition + 1, 1, sliderWidth - 2, client.H - 2 };
		PRIVATE()->Handle->setLocalBounds(rect);
		PRIVATE()->Handle->setViewport({ client.X + rect.X, client.Y + rect.Y, rect.W, rect.H });
	}
	else
	{
		auto range = std::max<float>(getHeight(), getMaximum() - getMinimum());
		auto sliderHeight = getHeight() * getHeight() / range;
		auto sliderPosition = (getHeight() - sliderHeight) * PRIVATE()->Value / range;
		if (std::isnan(sliderPosition)) sliderPosition = 0;
		auto rect = UIRect{ 1, sliderPosition + 1, client.W - 2, sliderHeight - 2 };
		PRIVATE()->Handle->setLocalBounds(rect);
		PRIVATE()->Handle->setViewport({ client.X + rect.X, client.Y + rect.Y, rect.W, rect.H });
	}
}

void UIScrollBar::paint(UIRect client, UIPainterRaw painter)
{
	UIElement::paint(client, painter);
	painter->setPen(PRIVATE()->Style.Pen);
	painter->setBrush(PRIVATE()->Style.Brush);
	painter->drawRect(client.X + 1, client.Y + 1, client.W - 2, client.H - 2);
}

void UIScrollBar::removeElement()
{
	UIList<UIElementRef> result;
	for (size_t i = 1; i < getChildren().size(); ++i) result.push_back(getChildren()[i]);
	for (size_t i = 0; i < result.size(); ++i) removeElement(result[i]);
}

UI::Orientation UIScrollBar::getOrientation() const
{
	return PRIVATE()->Orientation;
}

void UIScrollBar::setOrientation(UI::Orientation value)
{
	PRIVATE()->Orientation = value;
}

int32_t UIScrollBar::getMinimum() const
{
	return PRIVATE()->Minimum;
}

int32_t UIScrollBar::getMaximum() const
{
	return PRIVATE()->Maximum;
}

int32_t UIScrollBar::getRange() const
{
	return std::abs(getMaximum() - getMinimum());
}

void UIScrollBar::setRange(int32_t min, int32_t max)
{
	PRIVATE()->Minimum = min;
	PRIVATE()->Maximum = max;
	PRIVATE()->OnRangeChanged.signal(min, max);
}

int32_t UIScrollBar::getValue() const
{
	return std::clamp(PRIVATE()->Value, PRIVATE()->Minimum, PRIVATE()->Maximum);
}

void UIScrollBar::setValue(int32_t value)
{
	PRIVATE()->Value = value;
}

bool UIScrollBar::getTracking() const
{
	return PRIVATE()->Tracking;
}

void UIScrollBar::setTracking(bool value)
{
	PRIVATE()->Tracking = value;
}

int32_t UIScrollBar::getSingleStep() const
{
	return PRIVATE()->SingleStep;
}

void UIScrollBar::setSingleStep(int32_t value)
{
	PRIVATE()->SingleStep = std::max(1, value);
}

UIButtonRaw UIScrollBar::getHandle() const
{
	return PRIVATE()->Handle.get();
}

UIString UIScrollBarFactory::getTagName() const
{
	return "scrollbar";
}

UIElementRef UIScrollBarFactory::getElement(UIString style) const
{
	auto result = UINew<UIScrollBar>();
	result->setStyleText(style);
	return result;
}