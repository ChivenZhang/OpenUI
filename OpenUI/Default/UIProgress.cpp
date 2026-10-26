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
#include "UIProgress.h"
#include "../UICanvas.h"
#include <algorithm>

struct UIProgressPrivate : UIPrivate
{
	int32_t Minimum = 0;
	int32_t Maximum = 100;
	int32_t Value = 0;
	UIString Text;
	UISignalAs<int32_t> OnValueChanged;
};
#define PRIVATE() ((UIProgressPrivate*)m_Private)

UIProgress::UIProgress(UICanvasRaw canvas)
	:
	UIWidget(canvas)
{
	m_Private = new UIProgressPrivate;
	valueChanged = &PRIVATE()->OnValueChanged;
	setEmbedAttrib("textContent", PRIVATE()->Text);
	setMinSize(120, 18);
	UIPlate(getStyleNormal(), UICtrlAccentPen, UICtrlTrackBrush, UICtrlInkBrush, 4);
	UIPlate(getStyleDisabled(), UICtrlDisabledPen, UICtrlDisabledFillBrush, UICtrlDisabledInkBrush, 4);
}

UIProgress::~UIProgress()
{
	delete m_Private; m_Private = nullptr;
}

void UIProgress::paint(UIRect client, UIPainterRaw painter)
{
	UIWidget::paint(client, painter);
	auto& style = getEnable() ? getStyleNormal() : getStyleDisabled();
	auto face = UIFaceOf(style, client);
	UIDrawFace(painter, client, { .Border = UINoPen, .Fill = face.Fill, .Radius = face.Radius });
	float span = (float)std::max(1, PRIVATE()->Maximum - PRIVATE()->Minimum);
	float amount = std::clamp((PRIVATE()->Value - PRIVATE()->Minimum) / span, 0.0f, 1.0f);
	float width = std::max(0.0f, (client.W - 2) * amount);
	if (width > 0.5f)
	{
		UIColor bar = face.Border.Style == UIPen::NoPen ? UICtrlAccent : face.Border.Color;
		UIRect fill{ client.X + 1, client.Y + 1, width, std::max(0.0f, client.H - 2) };
		UIDrawFace(painter, fill, { .Border = UINoPen, .Fill = UIBrush{ .Color = bar }, .Radius = std::max(0.0f, face.Radius - 1) });
	}
	UIFont font;
	font.Align = UIFont::AlignCenter | UIFont::AlignVCenter;
	applyFontFamily(style, font);
	if (PRIVATE()->Text.empty() || face.Text.Style == UIBrush::NoBrush) return;
	painter->setPen(UINoPen);
	painter->setFont(font);
	painter->setBrush(face.Text);
	painter->drawText(client.X, client.Y, client.W, client.H, PRIVATE()->Text);
}

UIString UIProgress::getText() const
{
	return PRIVATE()->Text;
}

void UIProgress::setText(UIString const& text)
{
	PRIVATE()->Text = text;
}

int32_t UIProgress::getMinimum() const
{
	return PRIVATE()->Minimum;
}

int32_t UIProgress::getMaximum() const
{
	return PRIVATE()->Maximum;
}

void UIProgress::setRange(int32_t minimum, int32_t maximum)
{
	PRIVATE()->Minimum = minimum;
	PRIVATE()->Maximum = std::max(minimum, maximum);
	setValue(PRIVATE()->Value);
}

int32_t UIProgress::getValue() const
{
	return PRIVATE()->Value;
}

void UIProgress::setValue(int32_t value)
{
	value = std::clamp(value, PRIVATE()->Minimum, PRIVATE()->Maximum);
	if (PRIVATE()->Value == value) return;
	PRIVATE()->Value = value;
	PRIVATE()->OnValueChanged.signal(value);
}
#undef PRIVATE
