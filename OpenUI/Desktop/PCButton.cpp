#include "PCButton.h"

struct PCButtonPrivate : UIPrivate
{
	UIString Text;
	UIFont Font = PCFont(UIFont::AlignCenter | UIFont::AlignVCenter);
	UISignalAs<> OnClicked;
	UISignalAs<> OnPressed;
	UISignalAs<> OnReleased;
	UISignalAs<bool> OnToggled;
	bool Pressed = false;
	bool Hovered = false;
	bool Checked = false;
	bool Checkable = false;
};
#define PRIVATE() ((PCButtonPrivate*)m_Private)

PCButton::PCButton(UICanvasRaw canvas)
	:
	UIWidget(canvas)
{
	m_Private = new PCButtonPrivate;
	clicked = &PRIVATE()->OnClicked;
	pressed = &PRIVATE()->OnPressed;
	released = &PRIVATE()->OnReleased;
	toggled = &PRIVATE()->OnToggled;
	setMinSize(72, 28);
	PCPlate(getStyleNormal(), PCPen(PCLine()), PCBrush(PCPaper()), PCBrush(PCInk()), 4);
	PCPlate(getStyleHover(), PCPen(PCAccent()), PCBrush(PCHover()), PCBrush(PCInk()), 4);
	PCPlate(getStyleActive(), PCPen(PCAccentPress()), PCBrush(PCPress()), PCBrush(PCInk()), 4);
	PCPlate(getStyleDisabled(), PCPen(PCDisabledLine()), PCBrush(PCDisabledFill()), PCBrush(PCDisabledInk()), 4);
	PCPlate(getStyleChecked(), PCPen(PCAccent()), PCBrush(PCAccent()), PCBrush(PCOnAccent()), 4);
	assignFontFamily(getStyleNormal(), PRIVATE()->Font.Family);
}

PCButton::~PCButton()
{
	delete m_Private; m_Private = nullptr;
}

void PCButton::paint(UIRect client, UIPainterRaw painter)
{
	UIWidget::paint(client, painter);
	UIWidgetStyle& style = getEnable() == false ? getStyleDisabled()
		: PRIVATE()->Pressed ? getStyleActive()
		: PRIVATE()->Checkable && PRIVATE()->Checked ? getStyleChecked()
		: PRIVATE()->Hovered ? getStyleHover()
		: getStyleNormal();
	auto face = PCFaceOf(style, client);
	PCDrawFace(painter, client, face);
	UIFont font = PRIVATE()->Font;
	applyFontFamily(style, font);
	PCDrawText(painter, client, PRIVATE()->Text, font, face.Text);
}

UIString PCButton::getText() const
{
	return PRIVATE()->Text;
}

void PCButton::setText(UIString const& value)
{
	PRIVATE()->Text = value;
}

bool PCButton::getCheckable() const
{
	return PRIVATE()->Checkable;
}

void PCButton::setCheckable(bool value)
{
	PRIVATE()->Checkable = value;
}

bool PCButton::getChecked() const
{
	return PRIVATE()->Checked;
}

void PCButton::setChecked(bool value)
{
	if (PRIVATE()->Checked == value) return;
	PRIVATE()->Checked = value;
	PRIVATE()->OnToggled.signal(value);
}

void PCButton::mousePressEvent(UIMouseEventRaw event)
{
	if (getEnable() == false || event == nullptr || event->Button != UIInputEnum::MOUSE_BUTTON_LEFT) return;
	if (inBounds(event->X, event->Y) == false) return;
	PRIVATE()->Pressed = true;
	PRIVATE()->OnPressed.signal();
	event->Accept = true;
}

void PCButton::mouseReleaseEvent(UIMouseEventRaw event)
{
	if (event == nullptr || event->Button != UIInputEnum::MOUSE_BUTTON_LEFT) return;
	bool pressed = PRIVATE()->Pressed;
	PRIVATE()->Pressed = false;
	if (pressed) PRIVATE()->OnReleased.signal();
	if (pressed && getEnable() && inBounds(event->X, event->Y))
	{
		if (PRIVATE()->Checkable) setChecked(!PRIVATE()->Checked);
		PRIVATE()->OnClicked.signal();
		event->Accept = true;
	}
}

void PCButton::mouseMoveEvent(UIMouseEventRaw event)
{
	if (event == nullptr) return;
	PRIVATE()->Hovered = inBounds(event->X, event->Y);
}

void PCButton::leaveEvent(UIMouseEventRaw event)
{
	(void)event;
	PRIVATE()->Hovered = false;
	PRIVATE()->Pressed = false;
}
#undef PRIVATE
