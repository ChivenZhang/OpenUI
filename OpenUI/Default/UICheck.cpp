/*=================================================
* Copyright © 2020-2025 ChivenZhang.
* All Rights Reserved.
* =====================Note=========================
*
*
*=====================History========================
* Created by ChivenZhang@gmail.com.
*
* =================================================*/
#include "UICheck.h"
#include "../UICanvas.h"
#include <algorithm>

struct UICheckPrivate : UIPrivate
{
	UIString Text;
	UISignalAs<bool> OnClicked;
	UISignalAs<> OnPressed;
	UISignalAs<> OnReleased;
	UISignalAs<> OnHovered;
	bool Pressed = false;
	bool Hovered = false;
	bool Checked = false;
};
#define PRIVATE() ((UICheckPrivate*) m_Private)

UICheck::UICheck(UICanvasRaw canvas)
	:
	UIWidget(canvas)
{
	m_Private = new UICheckPrivate;

	clicked = &PRIVATE()->OnClicked;
	pressed = &PRIVATE()->OnPressed;
	released = &PRIVATE()->OnReleased;
	hovered = &PRIVATE()->OnHovered;

	setEmbedAttrib("textContent", PRIVATE()->Text);
	setMinSize(24, 24);
	UIPlate(getStyleNormal(), UICtrlLinePen, UICtrlPaperBrush, UICtrlInkBrush, 3);
	UIPlate(getStyleHover(), UICtrlAccentPen, UICtrlHoverBrush, UICtrlInkBrush, 3);
	UIPlate(getStyleActive(), UICtrlAccentPressPen, UICtrlPressBrush, UICtrlInkBrush, 3);
	UIPlate(getStyleDisabled(), UICtrlDisabledPen, UICtrlDisabledFillBrush, UICtrlDisabledInkBrush, 3);
	UIPlate(getStyleChecked(), UICtrlAccentPen, UICtrlAccentBrush, UICtrlOnAccentBrush, 3);
}

UICheck::~UICheck()
{
	delete m_Private; m_Private = nullptr;
}

void UICheck::arrange(UIRect client)
{
	this->setFlexWrap({UI_CSS_FLEX_WRAP_NOWRAP});
	this->setAlignSelf({UI_CSS_ALIGN_SELF_STRETCH});
	this->setJustifyContent({UI_CSS_JUSTIFY_CONTENT_SPACE_EVENLY});
}

void UICheck::layout(UIRect client)
{
}

void UICheck::paint(UIRect client, UIPainterRaw painter)
{
	UIWidget::paint(client, painter);
	UIWidgetStyle& style = getEnable() == false ? getStyleDisabled()
		: PRIVATE()->Checked ? getStyleChecked()
		: PRIVATE()->Hovered ? getStyleHover()
		: getStyleNormal();
	float box = std::min(16.0f, std::max(0.0f, client.H - 4));
	UIRect mark{ client.X + 2, client.Y + (client.H - box) * 0.5f, box, box };
	auto face = UIFaceOf(style, mark);
	UIDrawFace(painter, mark, face);
	if (PRIVATE()->Checked)
	{
		UIFont markFont;
		markFont.Align = UIFont::AlignCenter | UIFont::AlignVCenter;
		markFont.Size = 12;
		UIBrush markBrush = face.Text.Style == UIBrush::NoBrush ? UICtrlOnAccentBrush : face.Text;
		painter->setPen(UINoPen);
		painter->setFont(markFont);
		painter->setBrush(markBrush);
		painter->drawText(mark.X, mark.Y, mark.W, mark.H, "\xE2\x9C\x93");
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
	if (PRIVATE()->Text.empty() || brush.Style == UIBrush::NoBrush) return;
	painter->setPen(UINoPen);
	painter->setFont(font);
	painter->setBrush(brush);
	painter->drawText(text.X, text.Y, text.W, text.H, PRIVATE()->Text);
}

UIString UICheck::getText() const
{
	return PRIVATE()->Text;
}

void UICheck::setText(UIString const& text)
{
	PRIVATE()->Text = text;
}

bool UICheck::getChecked() const
{
	return PRIVATE()->Checked;
}

void UICheck::setChecked(bool value)
{
	PRIVATE()->Checked = value;
}

void UICheck::mouseDoubleEvent(UIMouseEventRaw event)
{
	if (inBounds(event->X, event->Y))
	{
		if (event->Button == UIInputEnum::MOUSE_BUTTON_LEFT)
		{
			PRIVATE()->Pressed = true;
			PRIVATE()->Checked = !PRIVATE()->Checked;

			setChecked(PRIVATE()->Checked);

			PRIVATE()->OnPressed.signal();
			PRIVATE()->OnClicked.signal(PRIVATE()->Checked);
		}
	}
}

void UICheck::mousePressEvent(UIMouseEventRaw event)
{
	if (inBounds(event->X, event->Y))
	{
		if (event->Button == UIInputEnum::MOUSE_BUTTON_LEFT)
		{
			PRIVATE()->Pressed = true;
			PRIVATE()->Checked = !PRIVATE()->Checked;

			setChecked(PRIVATE()->Checked);

			PRIVATE()->OnPressed.signal();
			PRIVATE()->OnClicked.signal(PRIVATE()->Checked);

			event->Accept = true;
		}
	}
}

void UICheck::mouseReleaseEvent(UIMouseEventRaw event)
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

void UICheck::mouseMoveEvent(UIMouseEventRaw event)
{
	auto viewport = UIOverlap(getViewport(), getBounds());
	if (viewport.X <= event->X && event->X <= viewport.X + viewport.W
		&& viewport.Y <= event->Y && event->Y <= viewport.Y + viewport.H)
	{
		PRIVATE()->Hovered = true;
		PRIVATE()->OnHovered.signal();
	}
	else
	{
		PRIVATE()->Hovered = false;
	}
}

void UICheck::enterEvent(UIMouseEventRaw event)
{
}

void UICheck::leaveEvent(UIMouseEventRaw event)
{
	PRIVATE()->Hovered = false;
}
