#include "PCLineEdit.h"
#include "OpenUI/UICanvas.h"
#include <algorithm>

struct PCLineEditPrivate : UIPrivate
{
	UIString Text;
	UIString Placeholder;
	UIFont Font = PCFont(UIFont::AlignLeft | UIFont::AlignVCenter);
	UISignalAs<UIString const&> OnTextChanged;
	UISignalAs<> OnEditingFinished;
	UISignalAs<> OnReturnPressed;
	int32_t Cursor = 0;
	bool Hovered = false;

	void insert(UIString const& extra)
	{
		Cursor = std::clamp(Cursor, 0, (int32_t)Text.size());
		Text.insert((size_t)Cursor, extra);
		Cursor += (int32_t)extra.size();
	}

	void eraseBefore()
	{
		if (Cursor <= 0 || Text.empty()) return;
		int32_t index = Cursor - 1;
		while (index > 0 && ((uint8_t)Text[(size_t)index] & 0xC0) == 0x80) --index;
		Text.erase((size_t)index, (size_t)(Cursor - index));
		Cursor = index;
	}
};
#define PRIVATE() ((PCLineEditPrivate*)m_Private)

PCLineEdit::PCLineEdit(UICanvasRaw canvas)
	:
	UIWidget(canvas)
{
	m_Private = new PCLineEditPrivate;
	textChanged = &PRIVATE()->OnTextChanged;
	editingFinished = &PRIVATE()->OnEditingFinished;
	returnPressed = &PRIVATE()->OnReturnPressed;
	setMinSize(120, 28);
	PCPlate(getStyleNormal(), PCPen(PCLine()), PCBrush(PCPaper()), PCBrush(PCInk()), 4);
	PCPlate(getStyleHover(), PCPen(PCLineHover()), PCBrush(PCPaper()), PCBrush(PCInk()), 4);
	PCPlate(getStyleActive(), PCPen(PCAccent()), PCBrush(PCPaper()), PCBrush(PCInk()), 4);
	PCPlate(getStyleDisabled(), PCPen(PCDisabledLine()), PCBrush(PCDisabledFill()), PCBrush(PCDisabledInk()), 4);
	assignFontFamily(getStyleNormal(), PRIVATE()->Font.Family);
}

PCLineEdit::~PCLineEdit()
{
	delete m_Private; m_Private = nullptr;
}

void PCLineEdit::paint(UIRect client, UIPainterRaw painter)
{
	UIWidget::paint(client, painter);
	bool focused = getCanvas() && getCanvas()->getFocus() == this;
	UIWidgetStyle& style = getEnable() == false ? getStyleDisabled()
		: focused ? getStyleActive()
		: PRIVATE()->Hovered ? getStyleHover()
		: getStyleNormal();
	auto face = PCFaceOf(style, client);
	PCDrawFace(painter, client, face);
	UIFont font = PRIVATE()->Font;
	applyFontFamily(style, font);
	UIRect field{ client.X + 8, client.Y, std::max(0.0f, client.W - 16), client.H };
	if (PRIVATE()->Text.empty() && focused == false)
		PCDrawText(painter, field, PRIVATE()->Placeholder, font, PCBrush(PCMuted()));
	else
		PCDrawText(painter, field, PRIVATE()->Text, font, face.Text);
	if (focused && painter)
	{
		UIRect caret{};
		painter->setFont(font);
		painter->boundingRect(field.X, field.Y, field.W, field.H, PRIVATE()->Text, PRIVATE()->Cursor, &caret);
		painter->setPen(face.Border.Style == UIPen::NoPen ? PCPen(PCAccent()) : face.Border);
		painter->setBrush(UINoBrush);
		painter->drawLine(caret.X, client.Y + 6, caret.X, client.Y + client.H - 6);
	}
}

UIString PCLineEdit::getText() const
{
	return PRIVATE()->Text;
}

void PCLineEdit::setText(UIString const& value)
{
	if (PRIVATE()->Text == value) return;
	PRIVATE()->Text = value;
	PRIVATE()->Cursor = (int32_t)value.size();
	PRIVATE()->OnTextChanged.signal(PRIVATE()->Text);
}

UIString PCLineEdit::getPlaceholder() const
{
	return PRIVATE()->Placeholder;
}

void PCLineEdit::setPlaceholder(UIString const& value)
{
	PRIVATE()->Placeholder = value;
}

void PCLineEdit::mousePressEvent(UIMouseEventRaw event)
{
	if (getEnable() == false || event == nullptr || event->Button != UIInputEnum::MOUSE_BUTTON_LEFT) return;
	PRIVATE()->Hovered = inBounds(event->X, event->Y);
	if (PRIVATE()->Hovered == false) return;
	if (getCanvas()) getCanvas()->setFocus(this);
	event->Accept = true;
}

void PCLineEdit::keyPressEvent(UIKeyEventRaw event)
{
	if (event == nullptr || getEnable() == false || getCanvas() == nullptr || getCanvas()->getFocus() != this) return;
	auto key = event->Key;
	if (key == UIInputEnum::KEY_BACKSPACE)
	{
		PRIVATE()->eraseBefore();
		PRIVATE()->OnTextChanged.signal(PRIVATE()->Text);
		event->Accept = true;
	}
	else if (key == UIInputEnum::KEY_DELETE)
	{
		if (PRIVATE()->Cursor < (int32_t)PRIVATE()->Text.size())
		{
			int32_t saved = PRIVATE()->Cursor;
			PRIVATE()->Cursor += 1;
			while (PRIVATE()->Cursor < (int32_t)PRIVATE()->Text.size() && ((uint8_t)PRIVATE()->Text[(size_t)PRIVATE()->Cursor] & 0xC0) == 0x80)
				PRIVATE()->Cursor += 1;
			PRIVATE()->Text.erase((size_t)saved, (size_t)(PRIVATE()->Cursor - saved));
			PRIVATE()->Cursor = saved;
			PRIVATE()->OnTextChanged.signal(PRIVATE()->Text);
		}
		event->Accept = true;
	}
	else if (key == UIInputEnum::KEY_LEFT)
	{
		if (PRIVATE()->Cursor > 0) PRIVATE()->Cursor -= 1;
		while (PRIVATE()->Cursor > 0 && ((uint8_t)PRIVATE()->Text[(size_t)PRIVATE()->Cursor] & 0xC0) == 0x80) PRIVATE()->Cursor -= 1;
		event->Accept = true;
	}
	else if (key == UIInputEnum::KEY_RIGHT)
	{
		if (PRIVATE()->Cursor < (int32_t)PRIVATE()->Text.size()) PRIVATE()->Cursor += 1;
		while (PRIVATE()->Cursor < (int32_t)PRIVATE()->Text.size() && ((uint8_t)PRIVATE()->Text[(size_t)PRIVATE()->Cursor] & 0xC0) == 0x80) PRIVATE()->Cursor += 1;
		event->Accept = true;
	}
	else if (key == UIInputEnum::KEY_HOME)
	{
		PRIVATE()->Cursor = 0;
		event->Accept = true;
	}
	else if (key == UIInputEnum::KEY_END)
	{
		PRIVATE()->Cursor = (int32_t)PRIVATE()->Text.size();
		event->Accept = true;
	}
	else if (key == UIInputEnum::KEY_ENTER)
	{
		PRIVATE()->OnReturnPressed.signal();
		PRIVATE()->OnEditingFinished.signal();
		event->Accept = true;
	}
}

void PCLineEdit::inputEvent(UITextInputEventRaw event)
{
	if (event == nullptr || event->Done == false || getEnable() == false) return;
	if (getCanvas() == nullptr || getCanvas()->getFocus() != this) return;
	if (event->Text.empty()) return;
	PRIVATE()->insert(event->Text);
	PRIVATE()->OnTextChanged.signal(PRIVATE()->Text);
	event->Accept = true;
}

void PCLineEdit::focusOutEvent(UIFocusEventRaw event)
{
	(void)event;
	PRIVATE()->OnEditingFinished.signal();
}
#undef PRIVATE
