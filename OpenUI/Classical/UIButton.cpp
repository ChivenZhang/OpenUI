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
#include "UIButton.h"
#include "UICombo.h"
#include "../UICanvas.h"
#include <algorithm>

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

	UIPlate(getStyleNormal(), UICtrlLinePen, UICtrlPaperBrush, UICtrlInkBrush, 4);
	UIPlate(getStyleHover(), UICtrlAccentPen, UICtrlHoverBrush, UICtrlInkBrush, 4);
	UIPlate(getStyleActive(), UICtrlAccentPressPen, UICtrlPressBrush, UICtrlInkBrush, 4);
	UIPlate(getStyleDisabled(), UICtrlDisabledPen, UICtrlDisabledFillBrush, UICtrlDisabledInkBrush, 4);
	UIPlate(getStyleChecked(), UICtrlAccentPen, UICtrlAccentBrush, UICtrlOnAccentBrush, 4);
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
	auto face = UIFaceOf(style, client);
	if (style.BorderRadius.TopLeft.Horizontal == 0.0f && style.BorderRadius.TopLeft.Vertical == 0.0f)
		face.Radius = UIFaceOf(getStyleNormal(), client).Radius;
	UIDrawFace(painter, client, face);

	if (PRIVATE()->Checkable && PRIVATE()->Checked && PRIVATE()->Label->getText().empty())
	{
		float box = std::min(client.W, client.H);
		float dot = box * 0.45f;
		UIRect inner{ client.X + (client.W - dot) * 0.5f, client.Y + (client.H - dot) * 0.5f, dot, dot };
		UIBrush dotFill = face.Text.Style == UIBrush::NoBrush ? UICtrlAccentBrush : face.Text;
		UIDrawFace(painter, inner, { .Border = UINoPen, .Fill = dotFill, .Radius = dot * 0.5f });
	}

	if (dynamic_cast<UICombo*>(getParent()) != nullptr)
	{
		UIPen pen = face.Border.Style == UIPen::NoPen ? UICtrlMutedPen : face.Border;
		painter->setPen(pen);
		painter->setBrush(UINoBrush);
		float x = client.X + client.W - 16;
		float y = client.Y + client.H * 0.5f;
		painter->drawLine(x - 4, y - 2, x, y + 2);
		painter->drawLine(x, y + 2, x + 4, y - 2);
	}

	auto label = PRIVATE()->Label->getStyle();
	UIBrush textBrush = face.Text.Style == UIBrush::NoBrush ? UIBrush{ .Style = UIBrush::NoBrush } : face.Text;
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

	UIWidgetStyle* style = nullptr;
	switch (UIHash(state))
	{
	case UIHash(""):
	case UIHash("normal"): style = &getStyleNormal(); break;
	case UIHash("hover"): style = &getStyleHover(); break;
	case UIHash("active"): style = &getStyleActive(); break;
	case UIHash("disabled"): style = &getStyleDisabled(); break;
	case UIHash("checked"): style = &getStyleChecked(); break;
	default: return UIWidget::setStyleText(name, value);
	}

	auto group = [&](char const* prop) -> UIString
	{
		if (state.empty()) return prop;
		return UIString(prop) + ":" + state;
	};
	switch (UIHash(property))
	{
	case UIHash("border-color"):
		setEmbedStyle(name, style->BorderColor);
		break;
	case UIHash("border-width"):
		setEmbedStyle(group("border"), style->Border);
		setEmbedStyle(name, style->Border);
		break;
	case UIHash("background"):
		setEmbedStyle(name, style->Background);
		setEmbedStyle(group("background-color"), style->Background.Color);
		break;
	case UIHash("background-color"):
		setEmbedStyle(group("background"), style->Background);
		setEmbedStyle(name, style->Background.Color);
		break;
	case UIHash("color"):
		setEmbedStyle(group("foreground"), style->Foreground, true);
		setEmbedStyle(name, style->Foreground.Color, true);
		break;
	case UIHash("foreground"):
		setEmbedStyle(name, style->Foreground, true);
		setEmbedStyle(group("color"), style->Foreground.Color, true);
		break;
	case UIHash("border-radius"):
		setEmbedStyle(name, style->BorderRadius);
		break;
	case UIHash("font-family"):
		setEmbedStyle(name, style->FontFamily, true);
		if (PRIVATE()->Label) PRIVATE()->Label->setStyleText(name, value);
		break;
	case UIHash("font-size"):
	case UIHash("font-style"):
	case UIHash("font-weight"):
	case UIHash("font-stretch"):
	case UIHash("letter-spacing"):
	case UIHash("line-height"):
	case UIHash("text-align"):
	case UIHash("text-indent"):
	case UIHash("text-overflow"):
	case UIHash("white-space"):
	case UIHash("word-spacing"):
	{
		auto result = UIWidget::setStyleText(name, value);
		if (result && PRIVATE()->Label) PRIVATE()->Label->setStyleText(name, value);
		return result;
	}
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