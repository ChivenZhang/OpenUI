#include "PCComboBox.h"
#include "PCListBox.h"
#include "OpenUI/UICanvas.h"
#include <algorithm>

struct PCComboBoxPrivate : UIPrivate
{
	UIStringList Items;
	UIFont Font = PCFont(UIFont::AlignLeft | UIFont::AlignVCenter);
	PCListBoxRef Popup;
	UISignalAs<int32_t> OnCurrentIndexChanged;
	UISignalAs<UIString const&> OnCurrentTextChanged;
	UISignalAs<int32_t> OnActivated;
	int32_t Index = -1;
	bool Hovered = false;
	bool Open = false;
};
#define PRIVATE() ((PCComboBoxPrivate*)m_Private)

PCComboBox::PCComboBox(UICanvasRaw canvas)
	:
	UIWidget(canvas)
{
	m_Private = new PCComboBoxPrivate;
	currentIndexChanged = &PRIVATE()->OnCurrentIndexChanged;
	currentTextChanged = &PRIVATE()->OnCurrentTextChanged;
	activated = &PRIVATE()->OnActivated;
	setMinSize(140, 28);
	PCPlate(getStyleNormal(), PCPen(PCLine()), PCBrush(PCPaper()), PCBrush(PCInk()), 4);
	PCPlate(getStyleHover(), PCPen(PCAccent()), PCBrush(PCPaper()), PCBrush(PCInk()), 4);
	PCPlate(getStyleActive(), PCPen(PCAccent()), PCBrush(PCHover()), PCBrush(PCInk()), 4);
	PCPlate(getStyleDisabled(), PCPen(PCDisabledLine()), PCBrush(PCDisabledFill()), PCBrush(PCDisabledInk()), 4);
	assignFontFamily(getStyleNormal(), PRIVATE()->Font.Family);

	PRIVATE()->Popup = UINew<PCListBox>(canvas);
	PRIVATE()->Popup->setEventFilter(this);
	PRIVATE()->Popup->itemClicked->connect(this, [=](int32_t row) {
		setCurrentIndex(row);
		PRIVATE()->OnActivated.signal(row);
		closePopup();
		});
}

PCComboBox::~PCComboBox()
{
	closePopup();
	delete m_Private; m_Private = nullptr;
}

void PCComboBox::paint(UIRect client, UIPainterRaw painter)
{
	UIWidget::paint(client, painter);
	UIWidgetStyle& style = getEnable() == false ? getStyleDisabled()
		: PRIVATE()->Open ? getStyleActive()
		: PRIVATE()->Hovered ? getStyleHover()
		: getStyleNormal();
	auto face = PCFaceOf(style, client);
	PCDrawFace(painter, client, face);
	UIFont font = PRIVATE()->Font;
	applyFontFamily(style, font);
	PCDrawText(painter, { client.X + 8, client.Y, std::max(0.0f, client.W - 28), client.H }, getCurrentText(), font, face.Text);
	painter->setPen(face.Border.Style == UIPen::NoPen ? PCPen(PCMuted()) : face.Border);
	painter->setBrush(UINoBrush);
	float x = client.X + client.W - 16;
	float y = client.Y + client.H * 0.5f;
	painter->drawLine(x - 4, y - 2, x, y + 2);
	painter->drawLine(x, y + 2, x + 4, y - 2);
}

UIStringList const& PCComboBox::getItems() const
{
	return PRIVATE()->Items;
}

void PCComboBox::setItems(UIStringList const& items)
{
	PRIVATE()->Items = items;
	if (PRIVATE()->Index >= (int32_t)items.size()) setCurrentIndex(items.empty() ? -1 : 0);
	if (PRIVATE()->Popup) PRIVATE()->Popup->setItems(items);
}

int32_t PCComboBox::getCurrentIndex() const
{
	return PRIVATE()->Index;
}

void PCComboBox::setCurrentIndex(int32_t value)
{
	if (PRIVATE()->Items.empty()) value = -1;
	else value = std::clamp(value, -1, (int32_t)PRIVATE()->Items.size() - 1);
	if (PRIVATE()->Index == value) return;
	PRIVATE()->Index = value;
	PRIVATE()->OnCurrentIndexChanged.signal(value);
	PRIVATE()->OnCurrentTextChanged.signal(getCurrentText());
}

UIString PCComboBox::getCurrentText() const
{
	if (PRIVATE()->Index < 0 || PRIVATE()->Index >= (int32_t)PRIVATE()->Items.size()) return {};
	return PRIVATE()->Items[PRIVATE()->Index];
}

void PCComboBox::openPopup()
{
	if (PRIVATE()->Open || PRIVATE()->Popup == nullptr || getCanvas() == nullptr) return;
	auto bounds = getBounds();
	float height = std::clamp(PRIVATE()->Items.size() * 28.0f + 4.0f, 32.0f, 240.0f);
	PRIVATE()->Popup->setItems(PRIVATE()->Items);
	PRIVATE()->Popup->setCurrentRow(PRIVATE()->Index);
	PRIVATE()->Popup->setPositionType({ UI_CSS_POSITION_ABSOLUTE });
	PRIVATE()->Popup->setFixedPos(bounds.X, bounds.Y + bounds.H + 2);
	PRIVATE()->Popup->setFixedSize(std::max(120.0f, bounds.W), height);
	getCanvas()->addWidget(PRIVATE()->Popup, 1);
	PRIVATE()->Open = true;
}

void PCComboBox::closePopup()
{
	if (PRIVATE() == nullptr || PRIVATE()->Popup == nullptr) return;
	if (getCanvas()) getCanvas()->removeWidget(PRIVATE()->Popup);
	PRIVATE()->Open = false;
}

bool PCComboBox::filter(UIReactorRaw source, UIEventRaw event)
{
	(void)source;
	if (PRIVATE()->Open == false || event == nullptr || event->Type != UIHash("MouseDown")) return false;
	auto mouse = UICast<UIMouseEvent>(event);
	if (mouse == nullptr) return false;
	if (inBounds(mouse->X, mouse->Y)) return false;
	if (PRIVATE()->Popup && PRIVATE()->Popup->inBounds(mouse->X, mouse->Y)) return false;
	closePopup();
	return false;
}

void PCComboBox::mousePressEvent(UIMouseEventRaw event)
{
	if (getEnable() == false || event == nullptr || event->Button != UIInputEnum::MOUSE_BUTTON_LEFT) return;
	if (inBounds(event->X, event->Y) == false) return;
	if (PRIVATE()->Open) closePopup();
	else openPopup();
	event->Accept = true;
}

void PCComboBox::mouseMoveEvent(UIMouseEventRaw event)
{
	if (event == nullptr) return;
	PRIVATE()->Hovered = inBounds(event->X, event->Y);
}

void PCComboBox::leaveEvent(UIMouseEventRaw event)
{
	(void)event;
	PRIVATE()->Hovered = false;
}
#undef PRIVATE
