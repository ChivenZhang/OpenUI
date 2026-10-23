#include "IGButton.h"
#include "IGStyle.h"

struct IGButtonPrivate : UIPrivate
{
	UIString Text;
	UIFont Font{ .Align = UIFont::AlignCenter | UIFont::AlignVCenter, };
	UISignalAs<> OnClicked;
	bool Pressed = false;
	bool Hovered = false;
};
#define PRIVATE() ((IGButtonPrivate*)m_Private)

IGButton::IGButton(UICanvasRaw canvas)
	:
	UIWidget(canvas)
{
	m_Private = new IGButtonPrivate;
	clicked = &PRIVATE()->OnClicked;
}

IGButton::~IGButton()
{
	delete m_Private; m_Private = nullptr;
}

void IGButton::paint(UIRect client, UIPainterRaw painter)
{
	UIWidget::paint(client, painter);
	UIPen pen = UIAccentPen;
	UIBrush fill = UIFrameBrush;
	UIBrush text = UITextBrush;
	if (getEnable() == false)
	{
		pen = UIDisabledPen;
		fill = UIDisabledBrush;
		text = UIMutedBrush;
	}
	else if (PRIVATE()->Pressed)
	{
		pen = UIAccentPressPen;
		fill = UIAccentPressBrush;
		text = UIAccentBrush;
	}
	else if (PRIVATE()->Hovered)
	{
		pen = UIAccentHoverPen;
		fill = UIAccentHoverBrush;
		text = UIAccentBrush;
	}
	IGDrawBox(painter, client, pen, fill, 2);
	IGDrawText(painter, client, PRIVATE()->Text, PRIVATE()->Font, text);
}

UIString IGButton::getText() const
{
	return PRIVATE()->Text;
}

void IGButton::setText(UIString const& text)
{
	PRIVATE()->Text = text;
}

void IGButton::mousePressEvent(UIMouseEventRaw event)
{
	if (getEnable() == false || event == nullptr || event->Button != UIInputEnum::MOUSE_BUTTON_LEFT) return;
	if (inBounds(event->X, event->Y) == false) return;
	PRIVATE()->Pressed = true;
	event->Accept = true;
}

void IGButton::mouseReleaseEvent(UIMouseEventRaw event)
{
	if (event == nullptr || event->Button != UIInputEnum::MOUSE_BUTTON_LEFT) return;
	bool pressed = PRIVATE()->Pressed;
	PRIVATE()->Pressed = false;
	if (pressed && inBounds(event->X, event->Y))
	{
		PRIVATE()->OnClicked.signal();
		event->Accept = true;
	}
}

void IGButton::mouseMoveEvent(UIMouseEventRaw event)
{
	if (event == nullptr) return;
	PRIVATE()->Hovered = inBounds(event->X, event->Y);
}

void IGButton::leaveEvent(UIMouseEventRaw event)
{
	(void)event;
	PRIVATE()->Hovered = false;
	PRIVATE()->Pressed = false;
}
#undef PRIVATE
