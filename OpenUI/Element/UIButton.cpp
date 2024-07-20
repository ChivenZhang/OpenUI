#include "UIButton.h"

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

UIButton::UIButton()
{
	m_PrivateButton = new UIButtonPrivate;

	clicked = &PRIVATE()->OnClicked;
	pressed = &PRIVATE()->OnPressed;
	released = &PRIVATE()->OnReleased;
	hovered = &PRIVATE()->OnHovered;
	toggled = &PRIVATE()->OnToggled;

	PRIVATE()->Style.Round = { 5, 5 };
	PRIVATE()->Style.Normal.Pen = { .Color = {0 / 255.0f, 120 / 255.0f, 212 / 255.0f, 1.0f} };
	PRIVATE()->Style.Normal.Brush = { .Color = {253 / 255.0f, 253 / 255.0f, 253 / 255.0f, 1.0f} };
	PRIVATE()->Style.Hover.Pen = { .Color = {0 / 255.0f, 120 / 255.0f, 212 / 255.0f, 1.0f} };
	PRIVATE()->Style.Hover.Brush = { .Color = {224 / 255.0f, 238 / 255.0f, 249 / 255.0f, 1.0f} };
	PRIVATE()->Style.Press.Pen = { .Color = {0 / 255.0f, 84 / 255.0f, 153 / 255.0f, 1.0f} };
	PRIVATE()->Style.Press.Brush = { .Color = {204 / 255.0f, 228 / 255.0f, 247 / 255.0f, 1.0f} };

	PRIVATE()->Label = UINew<UILabel>();
	addElement(PRIVATE()->Label);
	auto style = PRIVATE()->Label->getStyle();
	style.Normal.Background.Pen = style.Hover.Background.Pen = { .Style = UIPen::NoPen, };
	style.Normal.Background.Brush = style.Hover.Background.Brush = { .Style = UIBrush::NoBrush, };
	style.Normal.Foreground.Font = style.Hover.Foreground.Font = { .Align = UIFont::AlignCenter | UIFont::AlignVCenter, };
	PRIVATE()->Label->setStyle(style);
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

	if (getEnable() == false)
	{
		painter->setPen(PRIVATE()->Style.Disable.Pen);
		painter->setBrush(PRIVATE()->Style.Disable.Brush);
	}
	else if (PRIVATE()->Checkable && PRIVATE()->Checked || PRIVATE()->Checkable == false && PRIVATE()->Pressed)
	{
		painter->setPen(PRIVATE()->Style.Press.Pen);
		painter->setBrush(PRIVATE()->Style.Press.Brush);
	}
	else if (PRIVATE()->Hovered)
	{
		painter->setPen(PRIVATE()->Style.Hover.Pen);
		painter->setBrush(PRIVATE()->Style.Hover.Brush);
	}
	else
	{
		painter->setPen(PRIVATE()->Style.Normal.Pen);
		painter->setBrush(PRIVATE()->Style.Normal.Brush);
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
		if (event->Button == 1)
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
		if (event->Button == 1)
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
	if (event->Button == 1)
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

UIString UIButtonFactory::getTagName() const
{
	return "button";
}

UIElementRef UIButtonFactory::getElement(UIString style) const
{
	auto result = UINew<UIButton>();
	result->setStyleText(style);
	return result;
}
