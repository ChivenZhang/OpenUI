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
#include "UIRadio.h"
#include "../UICanvas.h"
#include <algorithm>

/// @brief 
struct UIRadioPrivate : UIPrivate
{
	UIString Text;
	UIRadioGroupRef SelfGroup, OtherGroup;
	UISignalAs<bool> OnClicked;
	UISignalAs<> OnPressed;
	UISignalAs<> OnReleased;
	UISignalAs<> OnHovered;
	bool Pressed = false;
	bool Hovered = false;
	bool Checked = false;
};
#define PRIVATE() ((UIRadioPrivate*) m_Private)

UIRadio::UIRadio(UICanvasRaw canvas)
	:
	UIWidget(canvas)
{
	m_Private = new UIRadioPrivate;

	clicked = &PRIVATE()->OnClicked;
	pressed = &PRIVATE()->OnPressed;
	released = &PRIVATE()->OnReleased;
	hovered = &PRIVATE()->OnHovered;

	PRIVATE()->SelfGroup = UINew<UIRadioGroup>();
	setEmbedAttrib("textContent", PRIVATE()->Text);
	setMinSize(24, 24);
	UIPlate(getStyleNormal(), UICtrlLinePen, UICtrlPaperBrush, UICtrlInkBrush, 8);
	UIPlate(getStyleHover(), UICtrlAccentPen, UICtrlHoverBrush, UICtrlInkBrush, 8);
	UIPlate(getStyleActive(), UICtrlAccentPressPen, UICtrlPressBrush, UICtrlInkBrush, 8);
	UIPlate(getStyleDisabled(), UICtrlDisabledPen, UICtrlDisabledFillBrush, UICtrlDisabledInkBrush, 8);
	UIPlate(getStyleChecked(), UICtrlAccentPen, UICtrlPaperBrush, UICtrlAccentBrush, 8);
}

UIRadio::~UIRadio()
{
	if (PRIVATE()->SelfGroup->Active == this) PRIVATE()->SelfGroup->Active = nullptr;
	if (PRIVATE()->OtherGroup && PRIVATE()->OtherGroup->Active == this) PRIVATE()->OtherGroup->Active = nullptr;
	PRIVATE()->OtherGroup = nullptr;
	delete m_Private; m_Private = nullptr;
}

void UIRadio::arrange(UIRect client)
{
	this->setFlexWrap({UI_CSS_FLEX_WRAP_NOWRAP});
	this->setAlignSelf({UI_CSS_ALIGN_SELF_STRETCH});
	this->setJustifyContent({UI_CSS_JUSTIFY_CONTENT_SPACE_EVENLY});
}

void UIRadio::paint(UIRect client, UIPainterRaw painter)
{
	UIWidget::paint(client, painter);
	UIWidgetStyle& style = getEnable() == false ? getStyleDisabled()
		: PRIVATE()->Checked ? getStyleChecked()
		: PRIVATE()->Hovered ? getStyleHover()
		: getStyleNormal();
	float box = std::min(16.0f, std::max(0.0f, client.H - 4));
	UIRect mark{ client.X + 2, client.Y + (client.H - box) * 0.5f, box, box };
	auto face = UIFaceOf(style, mark);
	face.Radius = box * 0.5f;
	UIDrawFace(painter, mark, face);
	if (PRIVATE()->Checked)
	{
		float dot = box * 0.45f;
		UIRect inner{ mark.X + (box - dot) * 0.5f, mark.Y + (box - dot) * 0.5f, dot, dot };
		UIDrawFace(painter, inner, { .Border = UINoPen, .Fill = face.Text, .Radius = dot * 0.5f });
	}
	UIWidgetStyle& textStyle = getEnable() ? getStyleNormal() : getStyleDisabled();
	uint32_t horizontal = UIFont::AlignLeft;
	switch (textStyle.TextAlign.Value)
	{
	case UI_CSS_TEXT_ALIGN_END:
	case UI_CSS_TEXT_ALIGN_RIGHT: horizontal = UIFont::AlignRight; break;
	case UI_CSS_TEXT_ALIGN_CENTER: horizontal = UIFont::AlignCenter; break;
	case UI_CSS_TEXT_ALIGN_JUSTIFY:
	case UI_CSS_TEXT_ALIGN_JUSTIFY_ALL: horizontal = UIFont::AlignJustify; break;
	default: break;
	}
	UIFont font;
	font.Align = horizontal | UIFont::AlignVCenter;
	applyFontFamily(textStyle, font);
	UIRect text{ mark.X + mark.W + 8, client.Y, std::max(0.0f, client.W - mark.W - 12), client.H };
	auto label = UIFaceOf(textStyle, text);
	UIBrush brush = label.Text.Style == UIBrush::NoBrush ? UICtrlInkBrush : label.Text;
	auto content = getText();
	if (content.empty() || brush.Style == UIBrush::NoBrush) return;
	painter->setPen(UINoPen);
	painter->setFont(font);
	painter->setBrush(brush);
	painter->drawText(text.X, text.Y, text.W, text.H, content);
}

UIString UIRadio::getText() const
{
	return PRIVATE()->Text;
}

void UIRadio::setText(UIString const& text)
{
	PRIVATE()->Text = text;
}

bool UIRadio::getChecked() const
{
	return PRIVATE()->Checked;
}

void UIRadio::setChecked(bool value)
{
	PRIVATE()->Checked = value;

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