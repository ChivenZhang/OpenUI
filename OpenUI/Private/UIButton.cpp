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

static UIPropColor buttonColor(UIColor color)
{
	UIPropColor result;
	result.Type = UI_CSS_COLOR_RGBA;
	result.Value.R = (uint8_t)(color.R * 255.0f + 0.5f);
	result.Value.G = (uint8_t)(color.G * 255.0f + 0.5f);
	result.Value.B = (uint8_t)(color.B * 255.0f + 0.5f);
	result.Value.A = (uint8_t)(color.A * 255.0f + 0.5f);
	return result;
}

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

struct UIButtonFace
{
	UIPropColor Border;
	UIPropColor Background;
	UIPropColor Foreground;
	UIPropWidth BorderWidth{ .Value = 1.0f, .Type = UI_CSS_WIDTH_LENGTH };
};

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

	UIButtonFace Hover
	{
		.Border = buttonColor(UIAccentHoverPen.Color),
		.Background = buttonColor(UIAccentHoverBrush.Color),
		.Foreground = buttonColor(UIAccentBrush.Color),
	};
	UIButtonFace Active
	{
		.Border = buttonColor(UIAccentPressPen.Color),
		.Background = buttonColor(UIAccentPressBrush.Color),
		.Foreground = buttonColor(UIAccentBrush.Color),
	};
	UIButtonFace Disabled
	{
		.Border = buttonColor(UIDisabledPen.Color),
		.Background = buttonColor(UIDisabledBrush.Color),
		.Foreground = buttonColor(UIMutedBrush.Color),
	};
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
	this->setStyleText("border-color", buttonCssColor(UIAccentPen.Color));
	this->setStyleText("border-width", std::to_string(UIAccentPen.Width) + "px");
	this->setStyleText("background-color", buttonCssColor(UIFrameBrush.Color));
	this->setStyleText("color", buttonCssColor(UITextBrush.Color));
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

	UIButtonFace* face = nullptr;
	if (getEnable() == false) face = &PRIVATE()->Disabled;
	else if (PRIVATE()->Checkable && PRIVATE()->Checked || PRIVATE()->Checkable == false && PRIVATE()->Pressed) face = &PRIVATE()->Active;
	else if (PRIVATE()->Hovered) face = &PRIVATE()->Hover;

	UIColor border = face ? buttonColor(face->Border) : buttonColor(getBorderColor());
	UIColor fill = face ? buttonColor(face->Background) : buttonColor(getBackgroundColor());
	UIColor text = face ? buttonColor(face->Foreground) : buttonColor(getForegroundColor());
	float borderWidth = 0.0f;
	if (face)
	{
		if (face->BorderWidth.Type == UI_CSS_WIDTH_LENGTH) borderWidth = face->BorderWidth.Value;
	}
	else
	{
		auto side = getBorderTop();
		if (side.Type == UI_CSS_BORDER_LENGTH) borderWidth = side.Value;
		else if (side.Type == UI_CSS_BORDER_THIN) borderWidth = 1.0f;
		else if (side.Type == UI_CSS_BORDER_MEDIUM) borderWidth = 3.0f;
		else if (side.Type == UI_CSS_BORDER_THICK) borderWidth = 5.0f;
	}
	UIPen pen = UINoPen;
	if (border.A > 0.0f && borderWidth > 0.0f) pen = { .Color = border, .Width = borderWidth };
	UIBrush brush = UINoBrush;
	if (fill.A > 0.0f) brush = { .Color = fill };
	painter->setPen(pen);
	painter->setBrush(brush);

	auto radius = getBorderTopLeftRadius();
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

bool UIButton::setStyleText(UIString name, UIString value)
{
	UIString property = name;
	UIString state;
	auto split = name.find(':');
	if (split != UIString::npos)
	{
		property = name.substr(0, split);
		state = name.substr(split + 1);
	}

	if (state.empty() || state == "normal") return UIWidget::setStyleText(name, value);

	UIButtonFace* face = nullptr;
	switch (UIHash(state))
	{
	case UIHash("hover"): face = &PRIVATE()->Hover; break;
	case UIHash("active"): face = &PRIVATE()->Active; break;
	case UIHash("disabled"):
	case UIHash("disable"): face = &PRIVATE()->Disabled; break;
	default: return UIWidget::setStyleText(name, value);
	}

	switch (UIHash(property))
	{
	case UIHash("border-color"):
		setEmbedStyle(name, face->Border);
		break;
	case UIHash("background-color"):
		setEmbedStyle(name, face->Background);
		break;
	case UIHash("color"):
		setEmbedStyle(name, face->Foreground);
		break;
	case UIHash("border-width"):
		setEmbedStyle(name, face->BorderWidth);
		break;
	default:
		return UIWidget::setStyleText(name, value);
	}
	return getStyles()->setStyleText(name, value);
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