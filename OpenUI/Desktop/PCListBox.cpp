#include "PCListBox.h"
#include "OpenUI/UICanvas.h"
#include <algorithm>

struct PCListBoxPrivate : UIPrivate
{
	UIStringList Items;
	UIFont Font = PCFont(UIFont::AlignLeft | UIFont::AlignVCenter);
	UISignalAs<int32_t> OnCurrentRowChanged;
	UISignalAs<int32_t> OnItemClicked;
	UISignalAs<int32_t> OnItemActivated;
	int32_t Row = -1;
	float Offset = 0;
	bool Hovered = false;
	int32_t HoverRow = -1;
};
#define PRIVATE() ((PCListBoxPrivate*)m_Private)

static float PCListRow = 28.0f;

PCListBox::PCListBox(UICanvasRaw canvas)
	:
	UIWidget(canvas)
{
	m_Private = new PCListBoxPrivate;
	currentRowChanged = &PRIVATE()->OnCurrentRowChanged;
	itemClicked = &PRIVATE()->OnItemClicked;
	itemActivated = &PRIVATE()->OnItemActivated;
	setMinSize(160, 120);
	PCPlate(getStyleNormal(), PCPen(PCLine()), PCBrush(PCPaper()), PCBrush(PCInk()), 4);
	PCPlate(getStyleHover(), PCPen(PCLine()), PCBrush(PCHover()), PCBrush(PCInk()), 4);
	PCPlate(getStyleChecked(), PCPen(PCAccent()), PCBrush(PCPress()), PCBrush(PCInk()), 0);
	PCPlate(getStyleDisabled(), PCPen(PCDisabledLine()), PCBrush(PCDisabledFill()), PCBrush(PCDisabledInk()), 4);
	assignFontFamily(getStyleNormal(), PRIVATE()->Font.Family);
}

PCListBox::~PCListBox()
{
	delete m_Private; m_Private = nullptr;
}

void PCListBox::paint(UIRect client, UIPainterRaw painter)
{
	UIWidget::paint(client, painter);
	auto& plate = getEnable() ? getStyleNormal() : getStyleDisabled();
	auto face = PCFaceOf(plate, client);
	PCDrawFace(painter, client, face);
	UIFont font = PRIVATE()->Font;
	applyFontFamily(plate, font);
	float y = client.Y + 2 - PRIVATE()->Offset;
	for (int32_t i = 0; i < (int32_t)PRIVATE()->Items.size(); ++i)
	{
		UIRect row{ client.X + 2, y, std::max(0.0f, client.W - 4), PCListRow };
		if (row.Y + row.H >= client.Y && row.Y <= client.Y + client.H)
		{
			if (i == PRIVATE()->Row) PCDrawFace(painter, row, PCFaceOf(getStyleChecked(), row));
			else if (i == PRIVATE()->HoverRow) PCDrawFace(painter, row, PCFaceOf(getStyleHover(), row));
			auto text = i == PRIVATE()->Row || getEnable() ? face.Text : PCBrush(PCDisabledInk());
			if (i == PRIVATE()->Row)
			{
				auto selected = PCFaceOf(getStyleChecked(), row);
				if (selected.Text.Style != UIBrush::NoBrush) text = selected.Text;
			}
			PCDrawText(painter, { row.X + 8, row.Y, std::max(0.0f, row.W - 12), row.H }, PRIVATE()->Items[i], font, text);
		}
		y += PCListRow;
	}
}

UIStringList const& PCListBox::getItems() const
{
	return PRIVATE()->Items;
}

void PCListBox::setItems(UIStringList const& items)
{
	PRIVATE()->Items = items;
	PRIVATE()->Offset = 0;
	if (PRIVATE()->Row >= (int32_t)items.size()) setCurrentRow(items.empty() ? -1 : 0);
}

int32_t PCListBox::getCurrentRow() const
{
	return PRIVATE()->Row;
}

void PCListBox::setCurrentRow(int32_t value)
{
	if (PRIVATE()->Items.empty()) value = -1;
	else value = std::clamp(value, -1, (int32_t)PRIVATE()->Items.size() - 1);
	if (PRIVATE()->Row == value) return;
	PRIVATE()->Row = value;
	PRIVATE()->OnCurrentRowChanged.signal(value);
}

int32_t PCListBox::rowAt(float y) const
{
	auto bounds = getBounds();
	int32_t row = (int32_t)((y - bounds.Y - 2 + PRIVATE()->Offset) / PCListRow);
	if (row < 0 || row >= (int32_t)PRIVATE()->Items.size()) return -1;
	return row;
}

void PCListBox::mousePressEvent(UIMouseEventRaw event)
{
	if (getEnable() == false || event == nullptr || event->Button != UIInputEnum::MOUSE_BUTTON_LEFT) return;
	if (inBounds(event->X, event->Y) == false) return;
	if (getCanvas()) getCanvas()->setFocus(this);
	int32_t row = rowAt((float)event->Y);
	if (row >= 0)
	{
		setCurrentRow(row);
		PRIVATE()->OnItemClicked.signal(row);
	}
	event->Accept = true;
}

void PCListBox::mouseDoubleEvent(UIMouseEventRaw event)
{
	if (getEnable() == false || event == nullptr) return;
	int32_t row = rowAt((float)event->Y);
	if (row >= 0)
	{
		setCurrentRow(row);
		PRIVATE()->OnItemActivated.signal(row);
		event->Accept = true;
	}
}

void PCListBox::mouseMoveEvent(UIMouseEventRaw event)
{
	if (event == nullptr) return;
	PRIVATE()->Hovered = inBounds(event->X, event->Y);
	PRIVATE()->HoverRow = PRIVATE()->Hovered ? rowAt((float)event->Y) : -1;
}

void PCListBox::leaveEvent(UIMouseEventRaw event)
{
	(void)event;
	PRIVATE()->Hovered = false;
	PRIVATE()->HoverRow = -1;
}

void PCListBox::wheelEvent(UIMouseWheelEventRaw event)
{
	if (event == nullptr || inBounds(event->X, event->Y) == false) return;
	float delta = event->PixelY != 0 ? (float)event->PixelY : (float)event->AngleY;
	float limit = std::max(0.0f, PRIVATE()->Items.size() * PCListRow - getHeight());
	PRIVATE()->Offset = std::clamp(PRIVATE()->Offset - delta, 0.0f, limit);
	event->Accept = true;
}

void PCListBox::keyPressEvent(UIKeyEventRaw event)
{
	if (event == nullptr || getCanvas() == nullptr || getCanvas()->getFocus() != this) return;
	if (event->Key == UIInputEnum::KEY_DOWN) setCurrentRow(PRIVATE()->Row + 1);
	else if (event->Key == UIInputEnum::KEY_UP) setCurrentRow(std::max(0, PRIVATE()->Row - 1));
	else if (event->Key == UIInputEnum::KEY_ENTER && PRIVATE()->Row >= 0) PRIVATE()->OnItemActivated.signal(PRIVATE()->Row);
	else return;
	event->Accept = true;
}
#undef PRIVATE
