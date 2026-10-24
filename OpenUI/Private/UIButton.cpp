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

static UIColor buttonColor(UIPropColor const& color)
{
	return { color.Value.R / 255.0f, color.Value.G / 255.0f, color.Value.B / 255.0f, color.Value.A / 255.0f };
}

static UIString buttonCssColor(UIColor color)
{
	return "rgba("
		+ std::to_string((int)(color.R * 255.0f + 0.5f)) + ","
		+ std::to_string((int)(color.G * 255.0f + 0.5f)) + ","
		+ std::to_string((int)(color.B * 255.0f + 0.5f)) + ","
		+ std::to_string((int)(color.A * 255.0f + 0.5f)) + ")";
}

struct UIButtonPrivate : UIPrivate
{
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
	this->setEmbedAttrib("textContent", PRIVATE()->Label->getText());
	this->addWidget(PRIVATE()->Label);

	auto label = PRIVATE()->Label->getStyle();
	auto align = UIFont::AlignCenter | UIFont::AlignVCenter;
	label.Normal.Foreground.Font.Align = align;
	label.Hover.Foreground.Font.Align = align;
	label.Disable.Foreground.Font.Align = align;
	PRIVATE()->Label->setStyle(label);
	auto bind = [&](UIString const& state, UIPen const& pen, UIBrush const& fill, UIBrush const& text)
	{
		auto suffix = state.empty() ? UIString() : ":" + state;
		this->setStyleText("border-color" + suffix, buttonCssColor(pen.Color));
		this->setStyleText("border-width" + suffix, std::to_string(pen.Width) + "px");
		this->setStyleText("background-color" + suffix, buttonCssColor(fill.Color));
		this->setStyleText("color" + suffix, buttonCssColor(text.Color));
	};
	bind("", UIAccentPen, UIFrameBrush, UITextBrush);
	bind("hover", UIAccentHoverPen, UIAccentHoverBrush, UIAccentBrush);
	bind("active", UIAccentPressPen, UIAccentPressBrush, UIAccentBrush);
	bind("disabled", UIDisabledPen, UIDisabledBrush, UIMutedBrush);
	bind("checked", UIAccentPressPen, UIAccentPressBrush, UIAccentBrush);
	this->setStyleText("border-radius", "2px");
}

UIButton::~UIButton()
{
	delete m_Private; m_Private = nullptr;
}

void UIButton::arrange(UIRect client)
{
	this->setFlexWrap({UI_CSS_FLEX_WRAP_NOWRAP});
	this->setAlignItems({UI_CSS_ALIGN_ITEMS_STRETCH});
	this->setJustifyContent({UI_CSS_JUSTIFY_CONTENT_SPACE_EVENLY});

	PRIVATE()->Label->setFlexGrow({1.0f});
}

void UIButton::paint(UIRect client, UIPainterRaw painter)
{
	UIWidget::paint(client, painter);

	UIWidgetStyle& style = getEnable() == false ? getStyleDisabled()
		: PRIVATE()->Pressed ? getStyleActive()
		: PRIVATE()->Checkable && PRIVATE()->Checked ? getStyleChecked()
		: PRIVATE()->Hovered ? getStyleHover()
		: getStyleNormal();
	auto border = buttonColor(style.BorderColor);
	auto fill = buttonColor(style.Background.Color);
	auto text = buttonColor(style.Foreground.Color);
	auto side = style.Border.Top;
	float borderWidth = 0.0f;
	if (side.Type == UI_CSS_BORDER_LENGTH) borderWidth = side.Value;
	else if (side.Type == UI_CSS_BORDER_THIN) borderWidth = 1.0f;
	else if (side.Type == UI_CSS_BORDER_MEDIUM) borderWidth = 3.0f;
	else if (side.Type == UI_CSS_BORDER_THICK) borderWidth = 5.0f;
	UIPen pen = UINoPen;
	if (border.A > 0.0f && borderWidth > 0.0f) pen = { .Color = border, .Width = borderWidth };
	UIBrush brush = UINoBrush;
	if (fill.A > 0.0f) brush = { .Color = fill };
	painter->setPen(pen);
	painter->setBrush(brush);

	auto radius = style.BorderRadius.TopLeft.Horizontal == 0.0f && style.BorderRadius.TopLeft.Vertical == 0.0f
		? getStyleNormal().BorderRadius.TopLeft : style.BorderRadius.TopLeft;
	auto rx = radius.HorizontalType == UI_CSS_BORDER_RADIUS_PERCENTAGE ? client.W * radius.Horizontal / 100.0f : radius.Horizontal;
	auto ry = radius.VerticalType == UI_CSS_BORDER_RADIUS_PERCENTAGE ? client.H * radius.Vertical / 100.0f : radius.Vertical;
	if (rx < 0.5f || ry < 0.5f)
		painter->drawRect(client.X + 1, client.Y + 1, client.W - 2, client.H - 2);
	else
		painter->drawRoundedRect(client.X + 1, client.Y + 1, client.W - 2, client.H - 2, rx, ry);

	auto label = PRIVATE()->Label->getStyle();
	UIBrush textBrush = text.A > 0.0f ? UIBrush{ .Color = text } : UINoBrush;
	label.Normal.Foreground.Brush = textBrush;
	label.Hover.Foreground.Brush = textBrush;
	label.Disable.Foreground.Brush = textBrush;
	PRIVATE()->Label->setStyle(label);
}

UIString UIButton::getText() const
{
	return PRIVATE()->Label->getText();
}

void UIButton::setText(UIString const& text)
{
	PRIVATE()->Label->setText(text);
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