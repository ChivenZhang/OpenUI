#include "PCRadioButton.h"
#include <algorithm>

struct PCRadioButtonPrivate : UIPrivate
{
	UIString Text;
	UIFont Font = PCFont(UIFont::AlignLeft | UIFont::AlignVCenter);
	UISignalAs<bool> OnToggled;
	PCRadioGroup* Group = nullptr;
	bool Checked = false;
	bool Hovered = false;
};
#define PRIVATE() ((PCRadioButtonPrivate*)m_Private)

PCRadioGroup::~PCRadioGroup()
{
	auto items = m_Items;
	for (auto* item : items) item->setExclusive(nullptr);
}

void PCRadioGroup::add(PCRadioButton* radio)
{
	if (radio == nullptr) return;
	if (std::find(m_Items.begin(), m_Items.end(), radio) == m_Items.end()) m_Items.push_back(radio);
}

void PCRadioGroup::remove(PCRadioButton* radio)
{
	auto found = std::find(m_Items.begin(), m_Items.end(), radio);
	if (found != m_Items.end()) m_Items.erase(found);
}

void PCRadioGroup::select(PCRadioButton* radio)
{
	for (auto* item : m_Items)
		if (item != radio) item->setChecked(false);
	if (radio) radio->setChecked(true);
}

PCRadioButton::PCRadioButton(UICanvasRaw canvas)
	:
	UIWidget(canvas)
{
	m_Private = new PCRadioButtonPrivate;
	toggled = &PRIVATE()->OnToggled;
	setMinSize(24, 24);
	PCPlate(getStyleNormal(), PCPen(PCLine()), PCBrush(PCPaper()), PCBrush(PCInk()), 8);
	PCPlate(getStyleHover(), PCPen(PCAccent()), PCBrush(PCHover()), PCBrush(PCInk()), 8);
	PCPlate(getStyleActive(), PCPen(PCAccentPress()), PCBrush(PCPress()), PCBrush(PCInk()), 8);
	PCPlate(getStyleDisabled(), PCPen(PCDisabledLine()), PCBrush(PCDisabledFill()), PCBrush(PCDisabledInk()), 8);
	PCPlate(getStyleChecked(), PCPen(PCAccent()), PCBrush(PCPaper()), PCBrush(PCAccent()), 8);
	assignFontFamily(getStyleNormal(), PRIVATE()->Font.Family);
}

PCRadioButton::~PCRadioButton()
{
	if (PRIVATE()->Group) PRIVATE()->Group->remove(this);
	delete m_Private; m_Private = nullptr;
}

void PCRadioButton::paint(UIRect client, UIPainterRaw painter)
{
	UIWidget::paint(client, painter);
	UIWidgetStyle& style = getEnable() == false ? getStyleDisabled()
		: PRIVATE()->Checked ? getStyleChecked()
		: PRIVATE()->Hovered ? getStyleHover()
		: getStyleNormal();
	float box = std::min(16.0f, std::max(0.0f, client.H - 4));
	UIRect mark{ client.X + 2, client.Y + (client.H - box) * 0.5f, box, box };
	auto face = PCFaceOf(style, mark);
	face.Radius = box * 0.5f;
	PCDrawFace(painter, mark, face);
	if (PRIVATE()->Checked)
	{
		float dot = box * 0.45f;
		UIRect inner{ mark.X + (box - dot) * 0.5f, mark.Y + (box - dot) * 0.5f, dot, dot };
		PCDrawFace(painter, inner, { .Border = UINoPen, .Fill = face.Text, .Radius = dot * 0.5f });
	}
	UIFont font = PRIVATE()->Font;
	applyFontFamily(getStyleNormal(), font);
	UIRect text{ mark.X + mark.W + 8, client.Y, std::max(0.0f, client.W - mark.W - 12), client.H };
	auto label = PCFaceOf(getEnable() ? getStyleNormal() : getStyleDisabled(), text);
	PCDrawText(painter, text, PRIVATE()->Text, font, label.Text.Style == UIBrush::NoBrush ? PCBrush(PCInk()) : label.Text);
}

UIString PCRadioButton::getText() const
{
	return PRIVATE()->Text;
}

void PCRadioButton::setText(UIString const& value)
{
	PRIVATE()->Text = value;
}

bool PCRadioButton::getChecked() const
{
	return PRIVATE()->Checked;
}

void PCRadioButton::setChecked(bool value)
{
	if (PRIVATE()->Checked == value) return;
	PRIVATE()->Checked = value;
	PRIVATE()->OnToggled.signal(value);
	if (value && PRIVATE()->Group) PRIVATE()->Group->select(this);
}

void PCRadioButton::setExclusive(PCRadioGroup* group)
{
	if (PRIVATE()->Group == group) return;
	if (PRIVATE()->Group) PRIVATE()->Group->remove(this);
	PRIVATE()->Group = group;
	if (group) group->add(this);
}

void PCRadioButton::mousePressEvent(UIMouseEventRaw event)
{
	if (getEnable() == false || event == nullptr || event->Button != UIInputEnum::MOUSE_BUTTON_LEFT) return;
	if (inBounds(event->X, event->Y) == false) return;
	if (PRIVATE()->Group) PRIVATE()->Group->select(this);
	else setChecked(true);
	event->Accept = true;
}

void PCRadioButton::mouseMoveEvent(UIMouseEventRaw event)
{
	if (event == nullptr) return;
	PRIVATE()->Hovered = inBounds(event->X, event->Y);
}

void PCRadioButton::leaveEvent(UIMouseEventRaw event)
{
	(void)event;
	PRIVATE()->Hovered = false;
}
#undef PRIVATE
