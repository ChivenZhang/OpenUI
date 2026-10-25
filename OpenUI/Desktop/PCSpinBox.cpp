#include "PCSpinBox.h"
#include <algorithm>

struct PCSpinBoxPrivate : UIPrivate
{
	int32_t Minimum = 0;
	int32_t Maximum = 99;
	int32_t Step = 1;
	int32_t Value = 0;
	UIFont Font = PCFont(UIFont::AlignLeft | UIFont::AlignVCenter);
	UISignalAs<int32_t> OnValueChanged;
	bool Hovered = false;
	int HoverButton = 0;
};
#define PRIVATE() ((PCSpinBoxPrivate*)m_Private)

static int PCSpinButton(UIRect const& client, float x, float y)
{
	if (x < client.X + client.W - 18) return 0;
	if (y < client.Y + client.H * 0.5f) return 1;
	return -1;
}

PCSpinBox::PCSpinBox(UICanvasRaw canvas)
	:
	UIWidget(canvas)
{
	m_Private = new PCSpinBoxPrivate;
	valueChanged = &PRIVATE()->OnValueChanged;
	setMinSize(100, 28);
	PCPlate(getStyleNormal(), PCPen(PCLine()), PCBrush(PCPaper()), PCBrush(PCInk()), 4);
	PCPlate(getStyleHover(), PCPen(PCLineHover()), PCBrush(PCPaper()), PCBrush(PCInk()), 4);
	PCPlate(getStyleActive(), PCPen(PCAccent()), PCBrush(PCHover()), PCBrush(PCInk()), 4);
	PCPlate(getStyleDisabled(), PCPen(PCDisabledLine()), PCBrush(PCDisabledFill()), PCBrush(PCDisabledInk()), 4);
	assignFontFamily(getStyleNormal(), PRIVATE()->Font.Family);
}

PCSpinBox::~PCSpinBox()
{
	delete m_Private; m_Private = nullptr;
}

void PCSpinBox::paint(UIRect client, UIPainterRaw painter)
{
	UIWidget::paint(client, painter);
	UIWidgetStyle& style = getEnable() == false ? getStyleDisabled() : PRIVATE()->Hovered ? getStyleHover() : getStyleNormal();
	auto face = PCFaceOf(style, client);
	PCDrawFace(painter, client, face);
	UIFont font = PRIVATE()->Font;
	applyFontFamily(style, font);
	PCDrawText(painter, { client.X + 8, client.Y, std::max(0.0f, client.W - 28), client.H }, std::to_string(PRIVATE()->Value), font, face.Text);
	UIRect buttons{ client.X + client.W - 18, client.Y + 1, 17, std::max(0.0f, client.H - 2) };
	auto button = PRIVATE()->HoverButton > 0 ? PCFaceOf(getStyleActive(), buttons) : face;
	painter->setPen(face.Border);
	painter->setBrush(PRIVATE()->HoverButton != 0 ? button.Fill : UINoBrush);
	painter->drawLine(buttons.X, client.Y + 4, buttons.X, client.Y + client.H - 4);
	painter->drawLine(buttons.X + 3, client.Y + buttons.H * 0.5f, buttons.X + 14, client.Y + buttons.H * 0.5f);
	UIFont arrow = PCFont(UIFont::AlignCenter | UIFont::AlignVCenter);
	arrow.Size = 9;
	PCDrawText(painter, { buttons.X, buttons.Y, buttons.W, buttons.H * 0.5f }, "+", arrow, face.Text);
	PCDrawText(painter, { buttons.X, buttons.Y + buttons.H * 0.5f, buttons.W, buttons.H * 0.5f }, "-", arrow, face.Text);
}

int32_t PCSpinBox::getMinimum() const { return PRIVATE()->Minimum; }
int32_t PCSpinBox::getMaximum() const { return PRIVATE()->Maximum; }
int32_t PCSpinBox::getSingleStep() const { return PRIVATE()->Step; }
int32_t PCSpinBox::getValue() const { return PRIVATE()->Value; }

void PCSpinBox::setRange(int32_t minimum, int32_t maximum)
{
	PRIVATE()->Minimum = minimum;
	PRIVATE()->Maximum = std::max(minimum, maximum);
	setValue(PRIVATE()->Value);
}

void PCSpinBox::setSingleStep(int32_t value)
{
	PRIVATE()->Step = std::max(1, value);
}

void PCSpinBox::setValue(int32_t value)
{
	value = std::clamp(value, PRIVATE()->Minimum, PRIVATE()->Maximum);
	if (PRIVATE()->Value == value) return;
	PRIVATE()->Value = value;
	PRIVATE()->OnValueChanged.signal(value);
}

void PCSpinBox::mousePressEvent(UIMouseEventRaw event)
{
	if (getEnable() == false || event == nullptr || event->Button != UIInputEnum::MOUSE_BUTTON_LEFT) return;
	if (inBounds(event->X, event->Y) == false) return;
	int hit = PCSpinButton(getBounds(), (float)event->X, (float)event->Y);
	if (hit > 0) setValue(PRIVATE()->Value + PRIVATE()->Step);
	else if (hit < 0) setValue(PRIVATE()->Value - PRIVATE()->Step);
	event->Accept = true;
}

void PCSpinBox::mouseMoveEvent(UIMouseEventRaw event)
{
	if (event == nullptr) return;
	PRIVATE()->Hovered = inBounds(event->X, event->Y);
	PRIVATE()->HoverButton = PRIVATE()->Hovered ? PCSpinButton(getBounds(), (float)event->X, (float)event->Y) : 0;
}

void PCSpinBox::leaveEvent(UIMouseEventRaw event)
{
	(void)event;
	PRIVATE()->Hovered = false;
	PRIVATE()->HoverButton = 0;
}

void PCSpinBox::wheelEvent(UIMouseWheelEventRaw event)
{
	if (event == nullptr || getEnable() == false || inBounds(event->X, event->Y) == false) return;
	int delta = event->PixelY != 0 ? event->PixelY : event->AngleY;
	if (delta > 0) setValue(PRIVATE()->Value + PRIVATE()->Step);
	else if (delta < 0) setValue(PRIVATE()->Value - PRIVATE()->Step);
	event->Accept = true;
}
#undef PRIVATE
