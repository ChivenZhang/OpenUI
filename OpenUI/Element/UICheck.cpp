#include "UICheck.h"
#include "OpenUI/UIContext.h"

class UICheckPrivate : public UIElementPrivate
{
public:
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
#define PRIVATE() ((UICheckPrivate*) m_PrivateCheck)

UICheck::UICheck()
{
	m_PrivateCheck = new UICheckPrivate;

	clicked = &PRIVATE()->OnClicked;
	pressed = &PRIVATE()->OnPressed;
	released = &PRIVATE()->OnReleased;
	hovered = &PRIVATE()->OnHovered;

	PRIVATE()->Button = UINew<UIButton>();
	addElement(PRIVATE()->Button);
	PRIVATE()->Button->setMinSize(16, 16);
	PRIVATE()->Button->setMargin({ 8, 0, 8, 0 });
	PRIVATE()->Button->setCheckable(true);

	{
		PRIVATE()->Style.Round = { 4,4 };
		PRIVATE()->Style.Normal.Pen = PRIVATE()->Style.Hover.Pen = PRIVATE()->Style.Press.Pen = { .Color = {0 / 255.0f, 120 / 255.0f, 212 / 255.0f, 1.0f} };
		PRIVATE()->Style.Normal.Brush = PRIVATE()->Style.Hover.Brush = PRIVATE()->Style.Press.Brush = { .Color = {253 / 255.0f, 253 / 255.0f, 253 / 255.0f, 1.0f} };
	}
	{
		auto style = PRIVATE()->Button->getStyle();
		style.Round = { 2, 2 };
		style.Normal.Brush.Style = style.Press.Brush.Style = style.Hover.Brush.Style = UIBrush::NoBrush;
		style.Label.Normal.Background.Brush.Style = style.Label.Hover.Background.Brush.Style = UIBrush::NoBrush;
		style.Label.Normal.Foreground.Brush.Color = style.Label.Hover.Foreground.Brush.Color = PRIVATE()->Button->getStyle().Normal.Pen.Color;
		PRIVATE()->Button->setStyle(style);
	}

	PRIVATE()->Label = UINew<UILabel>();
	addElement(PRIVATE()->Label);
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
	delete m_PrivateCheck; m_PrivateCheck = nullptr;
}

void UICheck::arrange(UIRect client)
{
	this->setAlignSelf(UI::AlignStretch);
	this->setJustifyContent(UI::JustifySpaceEvenly);

	PRIVATE()->Button->setFlexGrow(0);
	PRIVATE()->Button->setAlignSelf(UI::AlignCenter);

	PRIVATE()->Label->setFlexGrow(1);
	PRIVATE()->Label->setAlignSelf(UI::AlignStretch);

	auto painter = getPainter();
	if (painter == nullptr) painter = getContext()->getPainter();
	if (painter == nullptr) return;
	auto fontSize = painter->getFont().Size * 1.0f;
	PRIVATE()->Button->setFixedSize(fontSize, fontSize);
}

void UICheck::layout(UIRect client)
{
}

void UICheck::paint(UIRect client, UIPainterRaw painter)
{
	UIElement::paint(client, painter);
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
		if (event->Button == 1)
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
		if (event->Button == 1)
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

UIString UICheckFactory::getTagName() const
{
	return "check";
}

UIElementRef UICheckFactory::getElement(UIString style) const
{
	auto result = UINew<UICheck>();
	result->setStyleText(style);
	return result;
}
