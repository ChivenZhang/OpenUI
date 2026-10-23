#include "IGCheck.h"
#include "IGStyle.h"

struct IGCheckPrivate : UIPrivate
{
	UIString Text;
	UIFont Font{ .Align = UIFont::AlignLeft | UIFont::AlignVCenter, };
	UISignalAs<bool> OnToggled;
	bool Checked = false;
	bool Hovered = false;
};
#define PRIVATE() ((IGCheckPrivate*)m_Private)

IGCheck::IGCheck(UICanvasRaw canvas)
	:
	UIWidget(canvas)
{
	m_Private = new IGCheckPrivate;
	toggled = &PRIVATE()->OnToggled;
}

IGCheck::~IGCheck()
{
	delete m_Private; m_Private = nullptr;
}

void IGCheck::paint(UIRect client, UIPainterRaw painter)
{
	UIWidget::paint(client, painter);
	float box = std::min(18.0f, client.H - 4);
	UIRect mark{ client.X + 2, client.Y + (client.H - box) * 0.5f, box, box };
	UIPen pen = PRIVATE()->Hovered ? UIAccentHoverPen : UIFramePen;
	UIBrush fill = PRIVATE()->Checked ? UIAccentBrush : UIInsetBrush;
	if (PRIVATE()->Checked) pen = UIAccentPen;
	IGDrawBox(painter, mark, pen, fill, 2);
	UIRect text{ mark.X + mark.W + 8, client.Y, std::max(0.0f, client.W - mark.W - 12), client.H };
	IGDrawText(painter, text, PRIVATE()->Text, PRIVATE()->Font, getEnable() ? UITextBrush : UIMutedBrush);
}

UIString IGCheck::getText() const
{
	return PRIVATE()->Text;
}

void IGCheck::setText(UIString const& text)
{
	PRIVATE()->Text = text;
}

bool IGCheck::getChecked() const
{
	return PRIVATE()->Checked;
}

void IGCheck::setChecked(bool value)
{
	if (PRIVATE()->Checked == value) return;
	PRIVATE()->Checked = value;
	PRIVATE()->OnToggled.signal(value);
}

void IGCheck::mousePressEvent(UIMouseEventRaw event)
{
	if (getEnable() == false || event == nullptr || event->Button != UIInputEnum::MOUSE_BUTTON_LEFT) return;
	if (inBounds(event->X, event->Y) == false) return;
	setChecked(!PRIVATE()->Checked);
	event->Accept = true;
}

void IGCheck::mouseMoveEvent(UIMouseEventRaw event)
{
	if (event == nullptr) return;
	PRIVATE()->Hovered = inBounds(event->X, event->Y);
}

void IGCheck::leaveEvent(UIMouseEventRaw event)
{
	(void)event;
	PRIVATE()->Hovered = false;
}
#undef PRIVATE
