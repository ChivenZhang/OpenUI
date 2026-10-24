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
#include "../UICheck.h"
#include "../UICanvas.h"

static UIString checkCssColor(UIColor color)
{
	return "rgba("
		+ std::to_string((int)(color.R * 255.0f + 0.5f)) + ","
		+ std::to_string((int)(color.G * 255.0f + 0.5f)) + ","
		+ std::to_string((int)(color.B * 255.0f + 0.5f)) + ","
		+ std::to_string((int)(color.A * 255.0f + 0.5f)) + ")";
}

static void checkBindFace(UIButtonRaw button, UIString const& state, UIPen const& pen, UIBrush const& brush)
{
	auto suffix = state.empty() ? UIString() : ":" + state;
	button->setStyleText("border-color" + suffix, pen.Style == UIPen::NoPen ? "rgba(0,0,0,0)" : checkCssColor(pen.Color));
	button->setStyleText("border-width" + suffix, std::to_string(pen.Style == UIPen::NoPen ? 0.0f : pen.Width) + "px");
	button->setStyleText("background-color" + suffix, brush.Style == UIBrush::NoBrush ? "rgba(0,0,0,0)" : checkCssColor(brush.Color));
}

struct UICheckPrivate : UIPrivate
{
	UICheckStyle Style;
	UIButtonRef Button;
	UILabelRef Label;
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

	PRIVATE()->Button = UINew<UIButton>(canvas);
	this->addWidget(PRIVATE()->Button);
	PRIVATE()->Button->setMinSize({16.0f, UI_CSS_MIN_WIDTH_LENGTH}, {16.0f, UI_CSS_MIN_HEIGHT_LENGTH});
	PRIVATE()->Button->setMargin(
		{ 8.0f },
		{ 8.0f },
		{ 0.0f },
		{ 0.0f }
		);
	PRIVATE()->Button->setCheckable(true);

	PRIVATE()->Button->setStyleText("border-radius", std::to_string(PRIVATE()->Style.Round.X) + "px");
	checkBindFace(PRIVATE()->Button.get(), "", PRIVATE()->Style.Normal.Pen, PRIVATE()->Style.Normal.Brush);
	checkBindFace(PRIVATE()->Button.get(), "hover", PRIVATE()->Style.Hover.Pen, PRIVATE()->Style.Hover.Brush);
	checkBindFace(PRIVATE()->Button.get(), "active", PRIVATE()->Style.Press.Pen, PRIVATE()->Style.Press.Brush);
	checkBindFace(PRIVATE()->Button.get(), "disabled", PRIVATE()->Style.Disable.Pen, PRIVATE()->Style.Disable.Brush);
	PRIVATE()->Button->setStyleText("color", checkCssColor(UIInkBrush.Color));
	PRIVATE()->Button->setStyleText("color:hover", checkCssColor(UIInkBrush.Color));
	PRIVATE()->Button->setStyleText("color:active", checkCssColor(UIInkBrush.Color));

	PRIVATE()->Label = UINew<UILabel>(canvas);
	this->addWidget(PRIVATE()->Label);
	{
		auto style = PRIVATE()->Label->getStyle();
		style.Normal.Background.Pen = style.Hover.Background.Pen = { .Style = UIPen::NoPen, };
		style.Normal.Background.Brush = style.Hover.Background.Brush = { .Style = UIBrush::NoBrush, };
		style.Normal.Foreground.Font = style.Hover.Foreground.Font = { .Align = UIFont::AlignLeft | UIFont::AlignVCenter, };
		PRIVATE()->Label->setStyle(style);
	}

	PRIVATE()->Button->clicked->connect(this, [=](bool checked) {
		PRIVATE()->Checked = checked;

		setChecked(PRIVATE()->Checked);

		PRIVATE()->OnClicked.signal(checked);
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

UICheck::~UICheck()
{
	delete m_Private; m_Private = nullptr;
}

void UICheck::arrange(UIRect client)
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
		{8 * getCanvas()->getConfig().DisplayScale, UI_CSS_MARGIN_LENGTH},
		{0, UI_CSS_MARGIN_LENGTH},
		{8 * getCanvas()->getConfig().DisplayScale, UI_CSS_MARGIN_LENGTH},
		{0, UI_CSS_MARGIN_LENGTH}
		);

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

void UICheck::layout(UIRect client)
{
}

void UICheck::paint(UIRect client, UIPainterRaw painter)
{
	UIWidget::paint(client, painter);
}

UIString UICheck::getText() const
{
	return PRIVATE()->Label->getText();
}

void UICheck::setText(UIString const& text)
{
	PRIVATE()->Label->setText(text);
}

UICheckStyle UICheck::getStyle() const
{
	return PRIVATE()->Style;
}

void UICheck::setStyle(UICheckStyle const& style)
{
	PRIVATE()->Style = style;
}

bool UICheck::getChecked() const
{
	return PRIVATE()->Checked;
}

void UICheck::setChecked(bool value)
{
	PRIVATE()->Checked = value;
	PRIVATE()->Button->setChecked(value);
	if (PRIVATE()->Checked) PRIVATE()->Button->setText("✔");
	else PRIVATE()->Button->setText("");
}

UIButtonRaw UICheck::getButton() const
{
	return UIButtonRaw();
}

UILabelRaw UICheck::getLabel() const
{
	return UILabelRaw();
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
