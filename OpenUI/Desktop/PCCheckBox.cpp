#include "PCCheckBox.h"
#include <algorithm>

struct PCCheckBoxPrivate : UIPrivate
{
	UIString Text;
	UIFont Font = PCFont(UIFont::AlignLeft | UIFont::AlignVCenter);
	UISignalAs<bool> OnToggled;
	UISignalAs<> OnClicked;
	bool Checked = false;
	bool Hovered = false;
};
#define PRIVATE() ((PCCheckBoxPrivate*)m_Private)

PCCheckBox::PCCheckBox(UICanvasRaw canvas)
	:
	UIWidget(canvas)
{
	m_Private = new PCCheckBoxPrivate;
	toggled = &PRIVATE()->OnToggled;
	clicked = &PRIVATE()->OnClicked;
	setMinSize(24, 24);
	PCPlate(getStyleNormal(), PCPen(PCLine()), PCBrush(PCPaper()), PCBrush(PCInk()), 3);
	PCPlate(getStyleHover(), PCPen(PCAccent()), PCBrush(PCHover()), PCBrush(PCInk()), 3);
	PCPlate(getStyleActive(), PCPen(PCAccentPress()), PCBrush(PCPress()), PCBrush(PCInk()), 3);
	PCPlate(getStyleDisabled(), PCPen(PCDisabledLine()), PCBrush(PCDisabledFill()), PCBrush(PCDisabledInk()), 3);
	PCPlate(getStyleChecked(), PCPen(PCAccent()), PCBrush(PCAccent()), PCBrush(PCOnAccent()), 3);
	assignFontFamily(getStyleNormal(), PRIVATE()->Font.Family);
}

PCCheckBox::~PCCheckBox()
{
	delete m_Private; m_Private = nullptr;
}

void PCCheckBox::paint(UIRect client, UIPainterRaw painter)
{
	UIWidget::paint(client, painter);
	UIWidgetStyle& style = getEnable() == false ? getStyleDisabled()
		: PRIVATE()->Checked ? getStyleChecked()
		: PRIVATE()->Hovered ? getStyleHover()
		: getStyleNormal();
	float box = std::min(16.0f, std::max(0.0f, client.H - 4));
	UIRect mark{ client.X + 2, client.Y + (client.H - box) * 0.5f, box, box };
	auto face = PCFaceOf(style, mark);
	PCDrawFace(painter, mark, face);
	if (PRIVATE()->Checked)
	{
		UIFont markFont = PCFont(UIFont::AlignCenter | UIFont::AlignVCenter);
		markFont.Size = 12;
		PCDrawText(painter, mark, "✓", markFont, face.Text);
	}
	UIFont font = PRIVATE()->Font;
	applyFontFamily(getStyleNormal(), font);
	UIRect text{ mark.X + mark.W + 8, client.Y, std::max(0.0f, client.W - mark.W - 12), client.H };
	auto label = PCFaceOf(getEnable() ? getStyleNormal() : getStyleDisabled(), text);
	PCDrawText(painter, text, PRIVATE()->Text, font, label.Text);
}

UIString PCCheckBox::getText() const
{
	return PRIVATE()->Text;
}

void PCCheckBox::setText(UIString const& value)
{
	PRIVATE()->Text = value;
}

bool PCCheckBox::getChecked() const
{
	return PRIVATE()->Checked;
}

void PCCheckBox::setChecked(bool value)
{
	if (PRIVATE()->Checked == value) return;
	PRIVATE()->Checked = value;
	PRIVATE()->OnToggled.signal(value);
}

void PCCheckBox::mousePressEvent(UIMouseEventRaw event)
{
	if (getEnable() == false || event == nullptr || event->Button != UIInputEnum::MOUSE_BUTTON_LEFT) return;
	if (inBounds(event->X, event->Y) == false) return;
	setChecked(!PRIVATE()->Checked);
	PRIVATE()->OnClicked.signal();
	event->Accept = true;
}

void PCCheckBox::mouseMoveEvent(UIMouseEventRaw event)
{
	if (event == nullptr) return;
	PRIVATE()->Hovered = inBounds(event->X, event->Y);
}

void PCCheckBox::leaveEvent(UIMouseEventRaw event)
{
	(void)event;
	PRIVATE()->Hovered = false;
}
#undef PRIVATE
