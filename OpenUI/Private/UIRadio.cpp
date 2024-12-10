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
#include "../UIRadio.h"
#include "../UIContext.h"

/// @brief 
class UIRadioPrivate : public UIElementPrivate
{
public:
	UIRadioStyle Style;
	UIButtonRef Button;
	UILabelRef Label;
	UIRadioGroupRef SelfGroup, OtherGroup;
	UISignalAs<bool> OnClicked;
	UISignalAs<> OnPressed;
	UISignalAs<> OnReleased;
	UISignalAs<> OnHovered;
	bool Pressed = false;
	bool Hovered = false;
	bool Checked = false;
};
#define PRIVATE() ((UIRadioPrivate*) m_PrivateRadio)

UIRadio::UIRadio()
{
	m_PrivateRadio = new UIRadioPrivate;

	clicked = &PRIVATE()->OnClicked;
	pressed = &PRIVATE()->OnPressed;
	released = &PRIVATE()->OnReleased;
	hovered = &PRIVATE()->OnHovered;

	PRIVATE()->SelfGroup = UINew<UIRadioGroup>();

	PRIVATE()->Button = UINew<UIButton>();
	addElement(PRIVATE()->Button);
	PRIVATE()->Button->setMinSize(16, 16);
	PRIVATE()->Button->setMargin({ 8, 0, 8, 0 });
	PRIVATE()->Button->setCheckable(true);

	PRIVATE()->Label = UINew<UILabel>();
	addElement(PRIVATE()->Label);

	{
		PRIVATE()->Style.Round = { 4,4 };
		PRIVATE()->Style.Normal.Pen = PRIVATE()->Style.Hover.Pen = PRIVATE()->Style.Press.Pen = { .Color = {0 / 255.0f, 120 / 255.0f, 212 / 255.0f, 1.0f} };
		PRIVATE()->Style.Normal.Brush = PRIVATE()->Style.Hover.Brush = PRIVATE()->Style.Press.Brush = { .Color = {253 / 255.0f, 253 / 255.0f, 253 / 255.0f, 1.0f} };
	}
	{
		auto style = PRIVATE()->Button->getStyle();
		style.Round = { 8, 8 };
		style.Normal.Brush.Style = UIBrush::NoBrush;
		style.Press.Brush.Color = { 0 / 255.0f, 120 / 255.0f, 212 / 255.0f, 1.0f };
		style.Label.Normal.Background.Pen.Style = style.Label.Hover.Background.Pen.Style = UIPen::NoPen;
		style.Label.Normal.Background.Brush.Style = style.Label.Hover.Background.Brush.Style = UIBrush::NoBrush;
		style.Label.Normal.Foreground.Brush.Color = style.Label.Hover.Foreground.Brush.Color = PRIVATE()->Button->getStyle().Normal.Pen.Color;
		PRIVATE()->Button->setStyle(style);
	}
	{
		auto style = PRIVATE()->Label->getStyle();
		style.Normal.Background.Pen = style.Hover.Background.Pen = { .Style = UIPen::NoPen, };
		style.Normal.Background.Brush = style.Hover.Background.Brush = { .Style = UIBrush::NoBrush, };
		style.Normal.Foreground.Font = style.Hover.Foreground.Font = { .Align = UIFont::AlignLeft | UIFont::AlignVCenter, };
		PRIVATE()->Label->setStyle(style);
	}

	PRIVATE()->Button->clicked->connect(this, [=](bool checked) {
		setChecked(true);

		PRIVATE()->OnClicked.signal(getChecked());
		});

	PRIVATE()->Button->pressed->connect(this, [=]() {
		PRIVATE()->OnPressed.signal();
		});

	PRIVATE()->Button->released->connect(this, [=]() {
		PRIVATE()->OnReleased.signal();
		});

	PRIVATE()->Button->hovered->connect(this, [=]() {
		PRIVATE()->OnHovered.signal();
		});
}

UIRadio::~UIRadio()
{
	if (PRIVATE()->SelfGroup->Active == this) PRIVATE()->SelfGroup->Active = nullptr;
	if (PRIVATE()->OtherGroup->Active == this) PRIVATE()->OtherGroup->Active = nullptr;
	PRIVATE()->OtherGroup = nullptr;
	delete m_PrivateRadio; m_PrivateRadio = nullptr;
}

void UIRadio::arrange(UIRect client)
{
	this->setAlignSelf(UI::AlignStretch);
	this->setJustifyContent(UI::JustifySpaceEvenly);

	PRIVATE()->Button->setFlexGrow(0);
	PRIVATE()->Button->setAlignSelf(UI::AlignCenter);

	PRIVATE()->Label->setFlexGrow(1);
	PRIVATE()->Label->setAlignSelf(UI::AlignStretch);

	auto painter = getPainter();
	if (painter == nullptr) painter = getContext()->getPainter();
	if (painter == nullptr) return;
	auto fontSize = painter->getFont().Size * 1.0f;
	PRIVATE()->Button->setFixedSize(fontSize * getContext()->getConfig().DisplayScale, fontSize * getContext()->getConfig().DisplayScale);
	PRIVATE()->Button->setMargin({ 8 * getContext()->getConfig().DisplayScale, 0, 8 * getContext()->getConfig().DisplayScale, 0 });
	auto style = PRIVATE()->Button->getStyle();
	style.Round = { (fontSize * getContext()->getConfig().DisplayScale * 0.5f + 1) , (fontSize * getContext()->getConfig().DisplayScale * 0.5f + 1) };
	PRIVATE()->Button->setStyle(style);
}

void UIRadio::layout(UIRect client)
{
}

void UIRadio::paint(UIRect client, UIPainterRaw painter)
{
	UIElement::paint(client, painter);
}

UIString UIRadio::getText() const
{
	return PRIVATE()->Label->getText();
}

void UIRadio::setText(UIString const& text)
{
	PRIVATE()->Label->setText(text);
}

UIRadioStyle UIRadio::getStyle() const
{
	return PRIVATE()->Style;
}

void UIRadio::setStyle(UIRadioStyle const& style)
{
	PRIVATE()->Style = style;
}

bool UIRadio::getChecked() const
{
	return PRIVATE()->Checked;
}

void UIRadio::setChecked(bool value)
{
	PRIVATE()->Checked = value;
	PRIVATE()->Button->setChecked(value);

	if (PRIVATE()->Checked)
	{
		if (PRIVATE()->OtherGroup)
		{
			if (PRIVATE()->OtherGroup->Active && PRIVATE()->OtherGroup->Active != this) PRIVATE()->OtherGroup->Active->setChecked(false);
			PRIVATE()->OtherGroup->Active = this;
		}
		if (PRIVATE()->SelfGroup)
		{
			if (PRIVATE()->SelfGroup->Active && PRIVATE()->SelfGroup->Active != this) PRIVATE()->SelfGroup->Active->setChecked(false);
			PRIVATE()->SelfGroup->Active = this;
		}
	}
}

UIRadioGroupRef UIRadio::getExclusive() const
{
	if (PRIVATE()->OtherGroup) return PRIVATE()->OtherGroup;
	return PRIVATE()->SelfGroup;
}

void UIRadio::setExclusive(UIRadioGroupRef group)
{
	if (PRIVATE()->SelfGroup == group) return;
	PRIVATE()->OtherGroup = group;
	setChecked(PRIVATE()->Checked);
}

UIButtonRaw UIRadio::getButton() const
{
	return PRIVATE()->Button.get();
}

UILabelRaw UIRadio::getLabel() const
{
	return PRIVATE()->Label.get();
}

void UIRadio::mouseDoubleEvent(UIMouseEventRaw event)
{
	if (inBounds(event->X, event->Y))
	{
		if (event->Button == UIInputEnum::MOUSE_BUTTON_LEFT)
		{
			PRIVATE()->Pressed = true;
			PRIVATE()->Checked = true;

			setChecked(PRIVATE()->Checked);

			PRIVATE()->OnPressed.signal();
			PRIVATE()->OnClicked.signal(PRIVATE()->Checked);

			event->Accept = true;
		}
	}
}

void UIRadio::mousePressEvent(UIMouseEventRaw event)
{
	if (inBounds(event->X, event->Y))
	{
		if (event->Button == UIInputEnum::MOUSE_BUTTON_LEFT)
		{
			PRIVATE()->Pressed = true;
			PRIVATE()->Checked = true;

			setChecked(PRIVATE()->Checked);

			PRIVATE()->OnPressed.signal();
			PRIVATE()->OnClicked.signal(PRIVATE()->Checked);

			event->Accept = true;
		}
	}
}

void UIRadio::mouseReleaseEvent(UIMouseEventRaw event)
{
	if (event->Button == UIInputEnum::MOUSE_BUTTON_LEFT)
	{
		if (PRIVATE()->Pressed)
		{
			PRIVATE()->Pressed = false;
			PRIVATE()->OnReleased.signal();

			getContext()->paintElement();

			event->Accept = true;
		}
	}
}

void UIRadio::mouseMoveEvent(UIMouseEventRaw event)
{
	if (inBounds(event->X, event->Y))
	{
		PRIVATE()->Hovered = true;
		PRIVATE()->OnHovered.signal();
	}
	else
	{
		PRIVATE()->Hovered = false;
	}
}

void UIRadio::enterEvent(UIMouseEventRaw event)
{
}

void UIRadio::leaveEvent(UIMouseEventRaw event)
{
	PRIVATE()->Hovered = false;
}

UIString UIRadioFactory::getTagName() const
{
	return "radio";
}

UIElementRef UIRadioFactory::getElement(UIString style) const
{
	auto result = UINew<UIRadio>();
	result->setStyleText(style);
	return result;
}
