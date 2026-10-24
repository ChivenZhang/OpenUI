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
#include "../UICanvas.h"

static UIString radioCssColor(UIColor color)
{
	return "rgba("
		+ std::to_string((int)(color.R * 255.0f + 0.5f)) + ","
		+ std::to_string((int)(color.G * 255.0f + 0.5f)) + ","
		+ std::to_string((int)(color.B * 255.0f + 0.5f)) + ","
		+ std::to_string((int)(color.A * 255.0f + 0.5f)) + ")";
}

static void radioBindFace(UIButtonRaw button, UIString const& state, UIPen const& pen, UIBrush const& brush)
{
	auto suffix = state.empty() ? UIString() : ":" + state;
	button->setStyleText("border-color" + suffix, pen.Style == UIPen::NoPen ? "rgba(0,0,0,0)" : radioCssColor(pen.Color));
	button->setStyleText("border-width" + suffix, std::to_string(pen.Style == UIPen::NoPen ? 0.0f : pen.Width) + "px");
	button->setStyleText("background-color" + suffix, brush.Style == UIBrush::NoBrush ? "rgba(0,0,0,0)" : radioCssColor(brush.Color));
}

/// @brief 
struct UIRadioPrivate : UIPrivate
{
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

	PRIVATE()->Button = UINew<UIButton>(canvas);
	addWidget(PRIVATE()->Button);
	PRIVATE()->Button->setMinSize({16, UI_CSS_MIN_WIDTH_LENGTH}, {16, UI_CSS_MIN_HEIGHT_LENGTH});
	PRIVATE()->Button->setMargin(
		{ 8, UI_CSS_MARGIN_LENGTH},
		{ 0, UI_CSS_MARGIN_LENGTH},
		{ 8, UI_CSS_MARGIN_LENGTH},
		{ 0, UI_CSS_MARGIN_LENGTH }
		);
	PRIVATE()->Button->setCheckable(true);

	PRIVATE()->Label = UINew<UILabel>(canvas);
	addWidget(PRIVATE()->Label);

	PRIVATE()->Button->setStyleText("border-radius", "8px");
	radioBindFace(PRIVATE()->Button.get(), "", PRIVATE()->Style.Normal.Pen, PRIVATE()->Style.Normal.Brush);
	radioBindFace(PRIVATE()->Button.get(), "hover", PRIVATE()->Style.Hover.Pen, PRIVATE()->Style.Hover.Brush);
	radioBindFace(PRIVATE()->Button.get(), "active", PRIVATE()->Style.Press.Pen, PRIVATE()->Style.Press.Brush);
	radioBindFace(PRIVATE()->Button.get(), "disabled", PRIVATE()->Style.Disable.Pen, PRIVATE()->Style.Disable.Brush);
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
	if (PRIVATE()->OtherGroup && PRIVATE()->OtherGroup->Active == this) PRIVATE()->OtherGroup->Active = nullptr;
	PRIVATE()->OtherGroup = nullptr;
	delete m_Private; m_Private = nullptr;
}

void UIRadio::arrange(UIRect client)
{
	this->setFlexWrap({UI_CSS_FLEX_WRAP_NOWRAP});
	this->setAlignSelf({UI_CSS_ALIGN_SELF_STRETCH});
	this->setJustifyContent({UI_CSS_JUSTIFY_CONTENT_SPACE_EVENLY});

	PRIVATE()->Button->setFlexGrow({0, UI_CSS_FLEX_GROW_NUMBER});
	PRIVATE()->Button->setAlignSelf({UI_CSS_ALIGN_SELF_CENTER});

	PRIVATE()->Label->setFlexGrow({1, UI_CSS_FLEX_GROW_NUMBER});
	PRIVATE()->Label->setAlignSelf({UI_CSS_ALIGN_SELF_STRETCH});

	auto painter = getCanvas()->getPainter();
	auto fontSize = painter->getFont().Size * 1.0f;
	PRIVATE()->Button->setFixedSize(
		{fontSize * getCanvas()->getConfig().DisplayScale, UI_CSS_WIDTH_LENGTH},
		{fontSize * getCanvas()->getConfig().DisplayScale, UI_CSS_HEIGHT_LENGTH}
		);
	PRIVATE()->Button->setMargin(
		{ 8 * getCanvas()->getConfig().DisplayScale, UI_CSS_MARGIN_LENGTH},
		{0, UI_CSS_MARGIN_LENGTH},
		{8 * getCanvas()->getConfig().DisplayScale, UI_CSS_MARGIN_LENGTH},
		{0, UI_CSS_MARGIN_LENGTH}
		);
	auto round = fontSize * getCanvas()->getConfig().DisplayScale * 0.5f + 1;
	PRIVATE()->Button->setStyleText("border-radius", std::to_string(round) + "px");

	PRIVATE()->Label->setMinHeight(
		{fontSize * getCanvas()->getConfig().DisplayScale, UI_CSS_HEIGHT_LENGTH}
		);
	PRIVATE()->Label->setMargin(
		{0 * getCanvas()->getConfig().DisplayScale, UI_CSS_MARGIN_LENGTH},
		{0, UI_CSS_MARGIN_LENGTH},
		{0 * getCanvas()->getConfig().DisplayScale, UI_CSS_MARGIN_LENGTH},
		{0, UI_CSS_MARGIN_LENGTH}
		);
}

void UIRadio::layout(UIRect client)
{
}

void UIRadio::paint(UIRect client, UIPainterRaw painter)
{
	UIWidget::paint(client, painter);
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