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
#include "../UICanvas.h"

struct UIButtonPrivate : UIPrivate
{
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
#define PRIVATE() ((UIButtonPrivate*) m_Private)

UIButton::UIButton(UICanvasRaw canvas)
	:
	UIWidget(canvas)
{
	m_Private = new UIButtonPrivate;

	clicked = &PRIVATE()->OnClicked;
	pressed = &PRIVATE()->OnPressed;
	released = &PRIVATE()->OnReleased;
	hovered = &PRIVATE()->OnHovered;
	toggled = &PRIVATE()->OnToggled;

	PRIVATE()->Label = UINew<UILabel>(canvas);
	addWidget(PRIVATE()->Label);

	setStyle(PRIVATE()->Style);
}

UIButton::~UIButton()
{
	delete m_Private; m_Private = nullptr;
}

void UIButton::arrange(UIRect client)
{
	this->setAlignItems({UI_CSS_ALIGN_ITEMS_STRETCH});
	this->setJustifyContent({UI_CSS_JUSTIFY_CONTENT_SPACE_EVENLY});

	PRIVATE()->Label->setFlexGrow({1.0f});
}

void UIButton::paint(UIRect client, UIPainterRaw painter)
{
	UIWidget::paint(client, painter);

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
	UIWidget::repaint(client, painter);

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
	}
	else
	{
		PRIVATE()->Hovered = false;
	}
}

void UIButton::enterEvent(UIMouseEventRaw event)
{
}

void UIButton::leaveEvent(UIMouseEventRaw event)
{
	PRIVATE()->Hovered = false;
}