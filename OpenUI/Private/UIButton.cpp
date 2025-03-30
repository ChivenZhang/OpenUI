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
#include "../UIButton.h"
#include "../UIContext.h"

class UIButtonPrivate : public UIElementPrivate
{
public:
	UIButtonStyle Style;
	UILabelRef Label;
	UISignalAs<bool> OnClicked;
	UISignalAs<> OnPressed;
	UISignalAs<> OnReleased;
	UISignalAs<> OnHovered;
	UISignalAs<bool> OnToggled;
	bool Pressed = false;
	bool Hovered = false;
	bool Checked = false;
	bool Checkable = false;
};
#define PRIVATE() ((UIButtonPrivate*) m_PrivateButton)

UIButton::UIButton(UIContextRaw context)
	:
	UIElement(context)
{
	m_PrivateButton = new UIButtonPrivate;

	clicked = &PRIVATE()->OnClicked;
	pressed = &PRIVATE()->OnPressed;
	released = &PRIVATE()->OnReleased;
	hovered = &PRIVATE()->OnHovered;
	toggled = &PRIVATE()->OnToggled;

	PRIVATE()->Label = UINew<UILabel>(context);
	addElement(PRIVATE()->Label);

	setStyle(PRIVATE()->Style);
}

UIButton::~UIButton()
{
	delete m_PrivateButton; m_PrivateButton = nullptr;
}

void UIButton::arrange(UIRect client)
{
	this->setAlignItems(UI::AlignStretch);
	this->setJustifyContent(UI::JustifySpaceEvenly);

	PRIVATE()->Label->setFlexGrow(1);
}

void UIButton::paint(UIRect client, UIPainterRaw painter)
{
	UIElement::paint(client, painter);

	painter->setPen(UINoPen);
	if (getEnable() == false)
	{
		painter->setBrush(PRIVATE()->Style.Disable.Brush);
	}
	else if (PRIVATE()->Checkable && PRIVATE()->Checked || PRIVATE()->Checkable == false && PRIVATE()->Pressed)
	{
		painter->setBrush(PRIVATE()->Style.Press.Brush);
	}
	else if (PRIVATE()->Hovered)
	{
		painter->setBrush(PRIVATE()->Style.Hover.Brush);
	}
	else
	{
		painter->setBrush(PRIVATE()->Style.Normal.Brush);
	}

	auto round = PRIVATE()->Style.Round;
	if (round.X < 0.5f || round.Y < 0.5f)
		painter->drawRect(client.X + 1, client.Y + 1, client.W - 2, client.H - 2);
	else
		painter->drawRoundedRect(client.X + 1, client.Y + 1, client.W - 2, client.H - 2, round.X, round.Y);
}

void UIButton::repaint(UIRect client, UIPainterRaw painter)
{
	UIElement::repaint(client, painter);

	painter->setBrush(UINoBrush);
	if (getEnable() == false)
	{
		painter->setPen(PRIVATE()->Style.Disable.Pen);
	}
	else if (PRIVATE()->Checkable && PRIVATE()->Checked || PRIVATE()->Checkable == false && PRIVATE()->Pressed)
	{
		painter->setPen(PRIVATE()->Style.Press.Pen);
	}
	else if (PRIVATE()->Hovered)
	{
		painter->setPen(PRIVATE()->Style.Hover.Pen);
	}
	else
	{
		painter->setPen(PRIVATE()->Style.Normal.Pen);
	}

	auto round = PRIVATE()->Style.Round;
	if (round.X < 0.5f || round.Y < 0.5f)
		painter->drawRect(client.X + 1, client.Y + 1, client.W - 2, client.H - 2);
	else
		painter->drawRoundedRect(client.X + 1, client.Y + 1, client.W - 2, client.H - 2, round.X, round.Y);
}

UIString UIButton::getText() const
{
	return PRIVATE()->Label->getText();
}

void UIButton::setText(UIString const& text)
{
	PRIVATE()->Label->setText(text);
}

UIButtonStyle UIButton::getStyle() const
{
	return PRIVATE()->Style;
}

void UIButton::setStyle(UIButtonStyle const& style)
{
	PRIVATE()->Style = style;
	PRIVATE()->Label->setStyle(PRIVATE()->Style.Label);
}

bool UIButton::getChecked() const
{
	return PRIVATE()->Checked;
}

void UIButton::setChecked(bool value)
{
	PRIVATE()->Checked = value;
}

bool UIButton::getCheckable() const
{
	return PRIVATE()->Checkable;
}

void UIButton::setCheckable(bool value)
{
	if (PRIVATE()->Checkable != value) PRIVATE()->Checked = false;
	PRIVATE()->Checkable = value;
}

bool UIButton::getDown() const
{
	return PRIVATE()->Pressed;
}

UILabelRaw UIButton::getLabel() const
{
	return PRIVATE()->Label.get();
}

void UIButton::mouseDoubleEvent(UIMouseEventRaw event)
{
	if (inBounds(event->X, event->Y))
	{
		if (event->Button == UIInputEnum::MOUSE_BUTTON_LEFT)
		{
			PRIVATE()->Pressed = true;
			if (PRIVATE()->Checkable) PRIVATE()->Checked = !PRIVATE()->Checked;
			PRIVATE()->OnPressed.signal();
			PRIVATE()->OnClicked.signal(PRIVATE()->Checked);
			if (getContext()) getContext()->paintElement();

			event->Accept = true;
		}
	}
}

void UIButton::mousePressEvent(UIMouseEventRaw event)
{
	if (inBounds(event->X, event->Y))
	{
		if (event->Button == UIInputEnum::MOUSE_BUTTON_LEFT)
		{
			PRIVATE()->Pressed = true;
			if (PRIVATE()->Checkable) PRIVATE()->Checked = !PRIVATE()->Checked;
			PRIVATE()->OnPressed.signal();
			PRIVATE()->OnClicked.signal(PRIVATE()->Checked);
			if (getContext()) getContext()->paintElement();

			event->Accept = true;
		}
	}
}

void UIButton::mouseReleaseEvent(UIMouseEventRaw event)
{
	if (event->Button == UIInputEnum::MOUSE_BUTTON_LEFT)
	{
		if (PRIVATE()->Pressed)
		{
			PRIVATE()->Pressed = false;
			PRIVATE()->OnReleased.signal();
			if (getContext()) getContext()->paintElement();

			event->Accept = true;
		}
	}
}

void UIButton::mouseMoveEvent(UIMouseEventRaw event)
{
	if (inBounds(event->X, event->Y))
	{
		PRIVATE()->Hovered = true;
		PRIVATE()->OnHovered.signal();
		if (getContext()) getContext()->paintElement();
	}
	else
	{
		PRIVATE()->Hovered = false;
		if (getContext()) getContext()->paintElement();
	}
}

void UIButton::enterEvent(UIMouseEventRaw event)
{
}

void UIButton::leaveEvent(UIMouseEventRaw event)
{
	PRIVATE()->Hovered = false;
	if (getContext()) getContext()->paintElement();
}

UIString UIButtonFactory::getTagName() const
{
	return "button";
}

UIElementRef UIButtonFactory::getElement(UIString style) const
{
	auto result = UINew<UIButton>(getContext());
	result->setStyleText(style);
	return result;
}
