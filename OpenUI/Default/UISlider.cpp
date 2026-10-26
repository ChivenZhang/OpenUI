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
#include "UISlider.h"
#include "../UICanvas.h"

/// @brief 
struct UISliderPrivate : UIPrivate
{
	UIFloat2 MousePos;
	UIButtonRef Handle;
	UI::Orientation Orientation = UI::Horizontal;
	int32_t Maximum = 0, Minimum = 0, PageStep = 1, SingleStep = 1, SliderPosition = 0, Value = 0;
	bool SliderDown = false, Tracking = false, Hovered = false;
	int32_t TickInterval = 0;
	UISlider::ticks_t TickPosition;

	UISignalAs<> OnSliderPressed;
	UISignalAs<> OnSliderReleased;
	UISignalAs<int32_t> OnSliderMoved;
	UISignalAs<int32_t> OnValueChanged;
	UISignalAs<int32_t, int32_t> OnRangeChanged;
};
#define PRIVATE() ((UISliderPrivate*)m_Private)

UISlider::UISlider(UICanvasRaw canvas)
	:
	UIWidget(canvas)
{
	m_Private = new UISliderPrivate;
	UIPlate(getStyleNormal(), UICtrlAccentPen, UICtrlTrackBrush, UICtrlPaperBrush, 7);
	UIPlate(getStyleHover(), UICtrlAccentHoverPen, UICtrlTrackBrush, UICtrlPaperBrush, 7);
	UIPlate(getStyleActive(), UICtrlAccentPressPen, UICtrlTrackBrush, UICtrlHoverBrush, 7);
	UIPlate(getStyleDisabled(), UICtrlDisabledPen, UICtrlDisabledFillBrush, UICtrlDisabledFillBrush, 7);

	sliderMoved = &PRIVATE()->OnSliderMoved;
	sliderPressed = &PRIVATE()->OnSliderPressed;
	sliderReleased = &PRIVATE()->OnSliderReleased;
	valueChanged = &PRIVATE()->OnValueChanged;
	rangeChanged = &PRIVATE()->OnRangeChanged;

	PRIVATE()->Handle = UINew<UIButton>(canvas);
	addWidget(PRIVATE()->Handle);
	PRIVATE()->Handle->setFixedSize({16, UI_CSS_WIDTH_LENGTH}, {16, UI_CSS_HEIGHT_LENGTH});
	{
		auto handle = PRIVATE()->Handle.get();
		char const* clear = "rgba(0,0,0,0)";
		handle->setStyleText("background-color", clear);
		handle->setStyleText("background-color:hover", clear);
		handle->setStyleText("background-color:active", clear);
		handle->setStyleText("background-color:disabled", clear);
		handle->setStyleText("border-color", clear);
		handle->setStyleText("border-color:hover", clear);
		handle->setStyleText("border-color:active", clear);
		handle->setStyleText("border-color:disabled", clear);
		handle->setStyleText("border-width", "0px");
		handle->setStyleText("color", clear);
	}

	PRIVATE()->Handle->pressed->connect(this, [=]() { PRIVATE()->OnSliderPressed.signal(); });
	PRIVATE()->Handle->released->connect(this, [=]() {
		if (PRIVATE()->Tracking == false) if (PRIVATE()->Value != PRIVATE()->SliderPosition) PRIVATE()->OnValueChanged.signal(PRIVATE()->Value);
		PRIVATE()->OnSliderReleased.signal();
		});
}

UISlider::~UISlider()
{
	delete m_Private; m_Private = nullptr;
}

void UISlider::arrange(UIRect client)
{
	PRIVATE()->Handle->setFixedSize({16 * getCanvas()->getConfig().DisplayScale, UI_CSS_WIDTH_LENGTH}, {16 * getCanvas()->getConfig().DisplayScale, UI_CSS_HEIGHT_LENGTH});
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
		PRIVATE()->Hovered = PRIVATE()->Handle->inBounds(event->X, event->Y);
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

			if (getCanvas()) getCanvas()->layoutWidget();
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
	UIWidget::paint(client, painter);
	UIWidgetStyle& style = getEnable() == false ? getStyleDisabled()
		: PRIVATE()->Handle && PRIVATE()->Handle->getDown() ? getStyleActive()
		: PRIVATE()->Hovered ? getStyleHover()
		: getStyleNormal();
	auto face = UIFaceOf(style, client);
	UIColor track = face.Fill.Style == UIBrush::NoBrush ? UICtrlTrack : face.Fill.Color;
	painter->setPen({ .Color = track, .Width = 4.0f });
	painter->setBrush(UINoBrush);
	float knobX = client.X + client.W * 0.5f;
	float knobY = client.Y + client.H * 0.5f;
	if (PRIVATE()->Orientation == UI::Horizontal)
	{
		knobY = client.Y + client.H * 0.5f;
		painter->drawLine(client.X + 8, knobY, client.X + client.W - 8, knobY);
	}
	else
	{
		knobX = client.X + client.W * 0.5f;
		painter->drawLine(knobX, client.Y + 8, knobX, client.Y + client.H - 8);
	}
	if (PRIVATE()->Handle)
	{
		auto bounds = PRIVATE()->Handle->getBounds();
		if (PRIVATE()->Orientation == UI::Horizontal) knobX = bounds.X + bounds.W * 0.5f;
		else knobY = bounds.Y + bounds.H * 0.5f;
	}
	UIRect knob{ knobX - 7.0f, knobY - 7.0f, 14.0f, 14.0f };
	UIPen border = face.Border.Style == UIPen::NoPen ? UICtrlAccentPen : face.Border;
	UIBrush fill = face.Text.Style == UIBrush::NoBrush ? UICtrlPaperBrush : face.Text;
	UIDrawFace(painter, knob, { .Border = border, .Fill = fill, .Radius = 7 });
}

void UISlider::removeWidget()
{
	UIList<UIWidgetRef> result;
	for (size_t i = 1; i < getWidgets().size(); ++i) result.push_back(getWidgets()[i]);
	for (size_t i = 0; i < result.size(); ++i) removeWidget(result[i]);
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