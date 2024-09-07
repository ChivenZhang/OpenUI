#include "../UIInput.h"
#include "../UIContext.h"
#include "UIPieceTable.h"

class UIInputPrivate : public UIElementPrivate
{
public:
	UIInputStyle Style;
	UIRef<UIPieceTable> PieceTable;
	UIRect Selection;
	bool ReadOnly = false, MousePress = false;
	UIString Text, SelectedText, PlaceholderText;
	UIInput::echo_t EchoMode = UIInput::EchoModeNoEcho;
	int32_t CursorStart = -1, Cursor = 0, MaxLength = INT16_MAX, SelectOffset = 0;

	UISignalAs<int32_t /*oldPos*/, int32_t /*newPos*/> OnCursorPositionChanged;
	UISignalAs<UIRect /*area*/> OnEditingStarted;
	UISignalAs<> OnEditingFinished;
	UISignalAs<> OnInputRejected;
	UISignalAs<> OnReturnPressed;
	UISignalAs<> OnSelectionChanged;
	UISignalAs<UIString const& /*text*/> OnTextChanged;
	UISignalAs<UIString const& /*text*/> OnTextEdited;
	UISignalAs<UIString& /*text*/> OnTextPasted;
	UISignalAs<UIString const& /*text*/> OnTextCopied;
};
#define PRIVATE() ((UIInputPrivate*) m_PrivateInput)

UIInput::UIInput()
{
	m_PrivateInput = new UIInputPrivate;
	PRIVATE()->PieceTable = UINew<UIPieceTable>();

	cursorPositionChanged = &PRIVATE()->OnCursorPositionChanged;
	editingStarted = &PRIVATE()->OnEditingStarted;
	editingFinished = &PRIVATE()->OnEditingFinished;
	inputRejected = &PRIVATE()->OnInputRejected;
	returnPressed = &PRIVATE()->OnReturnPressed;
	selectionChanged = &PRIVATE()->OnSelectionChanged;
	textChanged = &PRIVATE()->OnTextChanged;
	textEdited = &PRIVATE()->OnTextEdited;
	textPasted = &PRIVATE()->OnTextPasted;
	textCopied = &PRIVATE()->OnTextCopied;
}

UIInput::~UIInput()
{
	delete m_PrivateInput; m_PrivateInput = nullptr;
}

void UIInput::arrange(UIRect client)
{
}

void UIInput::layout(UIRect client)
{
	auto painter = getPainter();
	if (painter == nullptr) painter = getContext()->getPainter();
	if (painter == nullptr) return;
	UIRect cursorRect;
	painter->setFont(PRIVATE()->Style.Foreground.Font);
	painter->boundingRect(PRIVATE()->SelectOffset + getBounds().X, getBounds().Y, -1, getBounds().H, PRIVATE()->Text, PRIVATE()->Cursor, &cursorRect);

	if (PRIVATE()->CursorStart != -1 && PRIVATE()->CursorStart != PRIVATE()->Cursor)
	{
		UIRect cursorStartRect;
		painter->boundingRect(PRIVATE()->SelectOffset + getBounds().X, getBounds().Y, -1, getBounds().H, PRIVATE()->Text, PRIVATE()->CursorStart, &cursorStartRect);

		auto selectionX = std::min<float>(cursorStartRect.X, cursorRect.X);
		auto selectionY = std::max<float>(cursorStartRect.X, cursorRect.X);
		PRIVATE()->Selection = { selectionX, client.Y, (selectionY - selectionX), client.H };
	}
	else
	{
		PRIVATE()->Selection = {};
	}
}

void UIInput::paint(UIRect client, UIPainterRaw painter)
{
	UIElement::paint(client, painter);
	painter->setPen(UINoPen);
	painter->setBrush(PRIVATE()->Style.Background.Brush);
	painter->drawRect(client.X, client.Y, client.W, client.H);

	if (PRIVATE()->Text.empty() == false)
	{
		if (PRIVATE()->Selection.W != 0 && PRIVATE()->Selection.H != 0)
		{
			painter->setPen({ .Style = UIPen::NoPen });
			painter->setBrush({ .Color = { 153 / 255.0f, 201 / 255.0f, 239 / 255.0f, 255 / 255.0f} });
			painter->drawRect(PRIVATE()->Selection.X, PRIVATE()->Selection.Y + 1, PRIVATE()->Selection.W, PRIVATE()->Selection.H - 2);
		}

		painter->setPen(UINoPen);
		painter->setFont(PRIVATE()->Style.Foreground.Font);
		painter->setBrush(PRIVATE()->Style.Foreground.Brush);
		UIRect cursorRect;
		painter->drawText(PRIVATE()->SelectOffset + client.X, client.Y, client.W, client.H, PRIVATE()->Text, nullptr, PRIVATE()->Cursor, &cursorRect);

		painter->setPen(PRIVATE()->Style.Background.Pen);
		painter->setBrush({ .Style = UIBrush::NoBrush });
		painter->drawLine(cursorRect.X, cursorRect.Y, cursorRect.X, cursorRect.Y + cursorRect.H);
	}
}

void UIInput::repaint(UIRect client, UIPainterRaw painter)
{
	UIElement::repaint(client, painter);
	painter->setPen(PRIVATE()->Style.Background.Pen);
	painter->setBrush(UINoBrush);
	painter->drawRect(client.X, client.Y, client.W, client.H);
}

UIInputStyle UIInput::getStyle() const
{
	return PRIVATE()->Style;
}

void UIInput::setStyle(UIInputStyle value)
{
	PRIVATE()->Style = value;
}

UIInput::echo_t UIInput::getEchoMode() const
{
	return PRIVATE()->EchoMode;
}

void UIInput::setEchoMode(echo_t value)
{
	PRIVATE()->EchoMode = value;
}

int32_t UIInput::getMaxLength() const
{
	return PRIVATE()->MaxLength;
}

void UIInput::setMaxLength(int32_t value)
{
	PRIVATE()->MaxLength = value;
}

UIString UIInput::getPlaceholderText() const
{
	return PRIVATE()->PlaceholderText;
}

void UIInput::setPlaceholderText(UIString const& value)
{
	PRIVATE()->PlaceholderText = value;
}

bool UIInput::getReadOnly() const
{
	return PRIVATE()->ReadOnly;
}

void UIInput::setReadOnly(bool value)
{
	PRIVATE()->ReadOnly = value;
}

UIFontAligns UIInput::getAlignment() const
{
	return PRIVATE()->Style.Foreground.Font.Align;
}

void UIInput::setAlignment(UIFontAligns value)
{
	PRIVATE()->Style.Foreground.Font.Align = value;
}

int32_t UIInput::getCursorPosition() const
{
	return PRIVATE()->Cursor;
}

int32_t UIInput::getCursorPosition(int32_t x, int32_t y) const
{
	auto painter = getPainter();
	if (painter == nullptr) painter = getContext()->getPainter();
	if (painter == nullptr) return -1;
	int32_t cursor = -1;
	painter->setFont(PRIVATE()->Style.Foreground.Font);
	painter->boundingRect(PRIVATE()->SelectOffset + getBounds().X, getBounds().Y, -1, getBounds().H, PRIVATE()->Text, x, y, &cursor);
	return cursor;
}

void UIInput::setCursorPosition(int32_t value)
{
	auto painter = getPainter();
	if (painter == nullptr) painter = getContext()->getPainter();
	if (painter == nullptr) return;
	PRIVATE()->Cursor = std::clamp<int32_t>(value, 0, PRIVATE()->Text.length());
}

UIString UIInput::getSelectedText() const
{
	int32_t start, length;
	getSelection(start, length);
	return PRIVATE()->Text.substr(start, length);
}

void UIInput::getSelection(int32_t& start, int32_t& length) const
{
	start = std::min(PRIVATE()->CursorStart, PRIVATE()->Cursor);
	length = std::abs(PRIVATE()->CursorStart - PRIVATE()->Cursor);
}

void UIInput::setSelection(int32_t start, int32_t length)
{
	auto painter = getPainter();
	if (painter == nullptr) painter = getContext()->getPainter();
	if (painter == nullptr) return;
	PRIVATE()->CursorStart = std::clamp<int32_t>(start, 0, PRIVATE()->Text.size());
	PRIVATE()->Cursor = std::clamp<int32_t>(start + std::max<int32_t>(0, length), 0, PRIVATE()->Text.size());
}

UIString UIInput::getText() const
{
	return PRIVATE()->Text;
}

void UIInput::setText(UIString const& value)
{
	PRIVATE()->Cursor = 0;
	PRIVATE()->CursorStart = -1;
	PRIVATE()->PieceTable = UINew<UIPieceTable>(value);
	PRIVATE()->Text = PRIVATE()->PieceTable->getText();
	PRIVATE()->SelectedText = UIString();
}

void UIInput::backspace()
{
	if (PRIVATE()->CursorStart != -1 && PRIVATE()->CursorStart != PRIVATE()->Cursor)
	{
		int32_t start, length;
		getSelection(start, length);
		PRIVATE()->PieceTable->remove(start, length);
		PRIVATE()->Text = PRIVATE()->PieceTable->getText();
		PRIVATE()->Cursor = std::min(PRIVATE()->CursorStart, PRIVATE()->Cursor);
		PRIVATE()->CursorStart = -1;
	}
	else
	{
		int32_t bytes = 0;
		for (size_t i = 0; i < 6 && i + 1 <= PRIVATE()->Cursor; ++i)
		{
			bytes = ::UIUTF8Num(PRIVATE()->Text[PRIVATE()->Cursor - i - 1]);
			if (bytes) break;
		}
		PRIVATE()->PieceTable->remove(PRIVATE()->Cursor - bytes, bytes);
		PRIVATE()->Cursor = PRIVATE()->Cursor - bytes;
	}
	PRIVATE()->Text = PRIVATE()->PieceTable->getText();
	PRIVATE()->Cursor = std::clamp<int32_t>(PRIVATE()->Cursor, 0, PRIVATE()->Text.length());
	deselect();
}

void UIInput::del()
{
	if (PRIVATE()->CursorStart != -1 && PRIVATE()->CursorStart != PRIVATE()->Cursor)
	{
		int32_t start, length;
		getSelection(start, length);
		PRIVATE()->PieceTable->remove(start, length);
		PRIVATE()->Cursor = std::min(PRIVATE()->CursorStart, PRIVATE()->Cursor);
		PRIVATE()->CursorStart = -1;
	}
	else
	{
		auto bytes = ::UIUTF8Num(PRIVATE()->Text[PRIVATE()->Cursor]);
		PRIVATE()->PieceTable->remove(PRIVATE()->Cursor, bytes);
	}
	PRIVATE()->Text = PRIVATE()->PieceTable->getText();
	PRIVATE()->Cursor = std::clamp<int32_t>(PRIVATE()->Cursor, 0, PRIVATE()->Text.length());
	deselect();
}

void UIInput::deselect()
{
	PRIVATE()->CursorStart = -1;
}

void UIInput::insert(UIString const& text)
{
	if (PRIVATE()->MaxLength <= PRIVATE()->Text.size()) return;
	PRIVATE()->PieceTable->insert(PRIVATE()->Cursor, text);
	PRIVATE()->Cursor += text.length();
	PRIVATE()->Text = PRIVATE()->PieceTable->getText();
	PRIVATE()->Cursor = std::clamp<int32_t>(PRIVATE()->Cursor, 0, PRIVATE()->Text.length());
}

void UIInput::clear()
{
	PRIVATE()->Cursor = 0;
	PRIVATE()->PieceTable = UINew<UIPieceTable>();
	PRIVATE()->Text = PRIVATE()->PieceTable->getText();
	deselect();
}

UIString UIInput::copy()
{
	return getSelectedText();
}

void UIInput::cut()
{
	if (PRIVATE()->CursorStart != -1 && PRIVATE()->CursorStart != PRIVATE()->Cursor)
	{
		int32_t start, length;
		getSelection(start, length);
		PRIVATE()->PieceTable->remove(start, length);
		PRIVATE()->Cursor = std::min(PRIVATE()->CursorStart, PRIVATE()->Cursor);
		PRIVATE()->CursorStart = -1;
		PRIVATE()->Text = PRIVATE()->PieceTable->getText();
		PRIVATE()->Cursor = std::clamp<int32_t>(PRIVATE()->Cursor, 0, PRIVATE()->Text.length());
		deselect();
	}
}

void UIInput::paste(UIString const& text)
{
	cut();
	insert(text);
}

void UIInput::redo()
{
	/*PRIVATE()->PieceTable->redo();
	PRIVATE()->Text = PRIVATE()->PieceTable->getText();*/
}

void UIInput::undo()
{
	/*PRIVATE()->PieceTable->undo();
	PRIVATE()->Text = PRIVATE()->PieceTable->getText();*/
}

void UIInput::keyPressEvent(UIKeyEventRaw event)
{
	// Backspace
	if (event->Key == UIInputEnum::KEY_BACKSPACE)
	{
		this->backspace();
	}
	// Del
	if (event->Key == UIInputEnum::KEY_DELETE)
	{
		this->del();
	}
	// Left
	if (event->Key == UIInputEnum::KEY_LEFT)
	{
		int32_t bytes = 0;
		for (size_t i = 0; i < 6 && i + 1 <= PRIVATE()->Cursor; ++i)
		{
			bytes = ::UIUTF8Num(PRIVATE()->Text[PRIVATE()->Cursor - i - 1]);
			if (bytes) break;
		}
		PRIVATE()->Cursor = PRIVATE()->Cursor - bytes;
		PRIVATE()->Cursor = std::clamp<int32_t>(PRIVATE()->Cursor, 0, PRIVATE()->Text.length());
		deselect();
	}
	// Right
	if (event->Key == UIInputEnum::KEY_RIGHT)
	{
		auto bytes = ::UIUTF8Num(PRIVATE()->Text[PRIVATE()->Cursor]);
		PRIVATE()->Cursor = PRIVATE()->Cursor + bytes;
		PRIVATE()->Cursor = std::clamp<int32_t>(PRIVATE()->Cursor, 0, PRIVATE()->Text.length());
		deselect();
	}
	// CTRL + A
	if (event->Key == UIInputEnum::KEY_A && (event->Modifiers & UIInputEnum::KEY_MOD_CTRL))
	{
		setSelection(0, PRIVATE()->Text.length());
	}
	// CTRL + C
	if (event->Key == UIInputEnum::KEY_C && (event->Modifiers & UIInputEnum::KEY_MOD_CTRL))
	{
		auto text = copy();
		PRIVATE()->OnTextCopied.signal(text);
	}
	// CTRL + V
	if (event->Key == UIInputEnum::KEY_V && (event->Modifiers & UIInputEnum::KEY_MOD_CTRL))
	{
		UIString text;
		PRIVATE()->OnTextPasted.signal(text);
		paste(text);
	}
	// CTRL + Z
	if (event->Key == UIInputEnum::KEY_Z && (event->Modifiers & UIInputEnum::KEY_MOD_CTRL))
	{
		undo();
	}
	// CTRL + Y
	if (event->Key == UIInputEnum::KEY_Y && (event->Modifiers & UIInputEnum::KEY_MOD_CTRL))
	{
		redo();
	}
	// CTRL + X
	if (event->Key == UIInputEnum::KEY_X && (event->Modifiers & UIInputEnum::KEY_MOD_CTRL))
	{
		cut();
	}
	getContext()->paintElement();
}

void UIInput::inputEvent(UITextInputEventRaw event)
{
	if (getContext()->getFocus() == this)
	{
		if (event->Done)
		{
			if (PRIVATE()->MaxLength <= PRIVATE()->Text.size()) return;

			cut();
			insert(event->Text);
			deselect();
			auto painter = getPainter();
			if (painter == nullptr) painter = getContext()->getPainter();
			if (painter == nullptr) return;
			UIRect cursorRect;
			painter->setFont(PRIVATE()->Style.Foreground.Font);
			painter->boundingRect(PRIVATE()->SelectOffset + getBounds().X, getBounds().Y, -1, getBounds().H, PRIVATE()->Text, PRIVATE()->Cursor, &cursorRect);
			PRIVATE()->OnEditingStarted.signal(UIOverlap(getViewport(), cursorRect));
		}
		getContext()->paintElement();
	}
}

void UIInput::mouseDoubleEvent(UIMouseEventRaw event)
{
	if (UIBounds(UIOverlap(getViewport(), getBounds()), event->X, event->Y))
	{
		if (event->Button == UIInputEnum::MOUSE_BUTTON_LEFT || event->Button == UIInputEnum::MOUSE_BUTTON_RIGHT)
		{
			auto painter = getPainter();
			if (painter == nullptr) painter = getContext()->getPainter();
			if (painter == nullptr) return;
			UIRect cursorRect;
			painter->setFont(PRIVATE()->Style.Foreground.Font);
			painter->boundingRect(PRIVATE()->SelectOffset + getBounds().X, getBounds().Y, -1, getBounds().H, PRIVATE()->Text, event->X, event->Y, &PRIVATE()->Cursor, &cursorRect);
			PRIVATE()->CursorStart = PRIVATE()->Cursor;
			PRIVATE()->MousePress = true;

			getContext()->setFocus(this);
			PRIVATE()->OnEditingStarted.signal(UIOverlap(getViewport(), cursorRect));
		}
	}
	else
	{
		getContext()->setFocus(nullptr);
	}
}

void UIInput::mousePressEvent(UIMouseEventRaw event)
{
	if (UIBounds(UIOverlap(getViewport(), getBounds()), event->X, event->Y))
	{
		if (event->Button == UIInputEnum::MOUSE_BUTTON_LEFT || event->Button == UIInputEnum::MOUSE_BUTTON_RIGHT)
		{
			auto painter = getPainter();
			if (painter == nullptr) painter = getContext()->getPainter();
			if (painter == nullptr) return;
			UIRect cursorRect;
			painter->setFont(PRIVATE()->Style.Foreground.Font);
			painter->boundingRect(PRIVATE()->SelectOffset + getBounds().X, getBounds().Y, -1, getBounds().H, PRIVATE()->Text, event->X, event->Y, &PRIVATE()->Cursor, &cursorRect);
			PRIVATE()->CursorStart = PRIVATE()->Cursor;
			PRIVATE()->MousePress = true;

			getContext()->setFocus(this);
			PRIVATE()->OnEditingStarted.signal(UIOverlap(getViewport(), cursorRect));
		}
	}
	else
	{
		getContext()->setFocus(nullptr);
	}
}

void UIInput::mouseReleaseEvent(UIMouseEventRaw event)
{
	PRIVATE()->MousePress = false;
	if (PRIVATE()->CursorStart == PRIVATE()->Cursor)
	{
		PRIVATE()->CursorStart = -1;
	}
}

void UIInput::mouseMoveEvent(UIMouseEventRaw event)
{
	if (PRIVATE()->MousePress)
	{
		auto viewport = UIOverlap(getViewport(), getBounds());
		auto painter = getPainter();
		if (painter == nullptr) painter = getContext()->getPainter();
		if (painter == nullptr) return;
		painter->setFont(PRIVATE()->Style.Foreground.Font);
		auto boundRect = painter->boundingRect(PRIVATE()->SelectOffset + getBounds().X, getBounds().Y, -1, getBounds().H, PRIVATE()->Text, event->X, event->Y, &PRIVATE()->Cursor);
		if (event->X < viewport.X && boundRect.X < viewport.X) PRIVATE()->SelectOffset += 1;
		if (event->X > (viewport.X + viewport.W) && boundRect.X + boundRect.W > (viewport.X)) PRIVATE()->SelectOffset -= 1;

		getContext()->paintElement();
	}
}

UIString UIInputFactory::getTagName() const
{
	return "input";
}

UIElementRef UIInputFactory::getElement(UIString style) const
{
	auto result = UINew<UIInput>();
	result->setStyleText(style);
	return result;
}
