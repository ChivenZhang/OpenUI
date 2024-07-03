#include "RmGUIText.h"
#include "RmGUIContext.h"
#include "RmPieceTable.h"

class RmGUITextPrivateData : public RmGUIWidgetPrivate
{
public:
	RmGUITextStyle Style;
	RmString Text, SelectedText, PlaceholderText;
	size_t Row = 0, Column = 0;
	int32_t CursorStart = -1, Cursor = 0, MaxLength = -1;
	RmRect Selection;
	RmGUITextEchoMode EchoMode = RmGUITextEchoMode::EchoModeNoEcho;
	bool ReadOnly = false, MousePress = false;
	RmPieceTable RedoUndo;
	RmGUISignalAs<int32_t /*oldPos*/, int32_t /*newPos*/> OnCursorPositionChanged;
	RmGUISignalAs<RmRect /*area*/> OnEditingStarted;
	RmGUISignalAs<> OnEditingFinished;
	RmGUISignalAs<> OnInputRejected;
	RmGUISignalAs<> OnReturnPressed;
	RmGUISignalAs<> OnSelectionChanged;
	RmGUISignalAs<RmString const& /*text*/> OnTextChanged;
	RmGUISignalAs<RmString const& /*text*/> OnTextEdited;
};
#define PRIVATE() ((RmGUITextPrivateData*) m_PrivateText)

RmGUIText::RmGUIText(IRmGUIWidgetRaw parent)
	:
	RmGUIControl(parent),
	m_PrivateText(nullptr),
	cursorPositionChanged(nullptr),
	editingStarted(nullptr),
	editingFinished(nullptr),
	inputRejected(nullptr),
	returnPressed(nullptr),
	selectionChanged(nullptr),
	textChanged(nullptr),
	textEdited(nullptr)
{
	m_PrivateText = new RmGUITextPrivateData;
	cursorPositionChanged = &PRIVATE()->OnCursorPositionChanged;
	editingStarted = &PRIVATE()->OnEditingStarted;
	editingFinished = &PRIVATE()->OnEditingFinished;
	inputRejected = &PRIVATE()->OnInputRejected;
	returnPressed = &PRIVATE()->OnReturnPressed;
	selectionChanged = &PRIVATE()->OnSelectionChanged;
	textChanged = &PRIVATE()->OnTextChanged;
	textEdited = &PRIVATE()->OnTextEdited;
	PRIVATE()->MaxLength = INT16_MAX;
	PRIVATE()->Style.Font.Align = RmFontAlign::AlignVCenter;
}

RmGUIText::~RmGUIText()
{
	delete m_PrivateText; m_PrivateText = nullptr;
}

void RmGUIText::layout(RmRectRaw client)
{
	auto painter = getPainter();
	if (painter == nullptr) painter = getContext()->getPainter();
	if (painter == nullptr) return;
	RmRect cursorRect;
	painter->setFont(PRIVATE()->Style.Font);
	int32_t row = (int32_t)PRIVATE()->Row, column = (int32_t)PRIVATE()->Column;
	painter->boundingRect(getRect().X, getRect().Y, -1, getRect().H, PRIVATE()->Text, row, column, &PRIVATE()->Cursor, &cursorRect);

	if (PRIVATE()->CursorStart != -1 && PRIVATE()->CursorStart != PRIVATE()->Cursor)
	{
		RmRect cursorStartRect;
		painter->boundingRect(getRect().X, getRect().Y, -1, getRect().H, PRIVATE()->Text, PRIVATE()->CursorStart, nullptr, nullptr, &cursorStartRect);

		auto selectionX = std::min<float>(cursorStartRect.X, cursorRect.X);
		auto selectionY = std::max<float>(cursorStartRect.X, cursorRect.X);
		PRIVATE()->Selection = { selectionX, client->Y, (selectionY - selectionX), client->H };
	}
	else
	{
		PRIVATE()->Selection = {};
	}
}

void RmGUIText::paint(IRmGUIPainterRaw painter, RmRectRaw client)
{
	RmGUIWidget::paint(painter, client);
	painter->setPen({ .Color = { 108 / 255.0f, 110 / 255.0f, 111 / 255.0f, 1.0f }, });
	painter->setBrush({ .Color = { 238 / 255.0f, 238 / 255.0f, 242 / 255.0f, 1.0f }, });
	painter->drawRect(client->X, client->Y, client->W, client->H);

	if (PRIVATE()->Text.empty() == false)
	{
		PRIVATE()->Style.Font.NoWrap = true;
		PRIVATE()->Style.Font.Ellipsize = RmFont::EllipsizeNone;
		painter->setPen(PRIVATE()->Style.Pen);
		painter->setFont(PRIVATE()->Style.Font);
		painter->setBrush(PRIVATE()->Style.Brush);

		RmRect cursorRect;
		painter->drawText(client->X, client->Y, -1, client->H, PRIVATE()->Text, nullptr, PRIVATE()->Cursor, &cursorRect);
		painter->setPen({ .Color = {0, 0, 0, 1}, .Width = 1.0f });
		painter->setBrush({ .Style = RmBrush::NoBrush });
		painter->drawLine(cursorRect.X, cursorRect.Y, cursorRect.X, cursorRect.Y + cursorRect.H);

		if (PRIVATE()->Selection.W != 0 && PRIVATE()->Selection.H != 0)
		{
			painter->setPen({ .Style = RmPen::NoPen });
			painter->setBrush({ .Color = {126 / 255.0f, 186 / 255.0f, 232 / 255.0f, 100 / 255.0f} });
			painter->drawRect(PRIVATE()->Selection.X, PRIVATE()->Selection.Y, PRIVATE()->Selection.W, PRIVATE()->Selection.H);
		}
	}
}

RmGUITextStyle RmGUIText::getStyle() const
{
	return PRIVATE()->Style;
}

void RmGUIText::setStyle(RmGUITextStyle value)
{
	PRIVATE()->Style = value;
}

RmGUIText::echo_t RmGUIText::getEchoMode() const
{
	return PRIVATE()->EchoMode;
}

void RmGUIText::setEchoMode(echo_t value)
{
	PRIVATE()->EchoMode = value;
}

int32_t RmGUIText::getMaxLength() const
{
	return PRIVATE()->MaxLength;
}

void RmGUIText::setMaxLength(int32_t value)
{
	PRIVATE()->MaxLength = value;
}

RmString RmGUIText::getPlaceholderText() const
{
	return PRIVATE()->PlaceholderText;
}

void RmGUIText::setPlaceholderText(RmString const& value)
{
	PRIVATE()->PlaceholderText = value;
}

bool RmGUIText::getReadOnly() const
{
	return PRIVATE()->ReadOnly;
}

void RmGUIText::setReadOnly(bool value)
{
	PRIVATE()->ReadOnly = value;
}

RmFontAligns RmGUIText::getAlignment() const
{
	return PRIVATE()->Style.Font.Align;
}

void RmGUIText::setAlignment(RmFontAligns value)
{
	PRIVATE()->Style.Font.Align = value;
}

int32_t RmGUIText::getCursorPosition() const
{
	return PRIVATE()->Cursor;
}

int32_t RmGUIText::getCursorPosition(int32_t x, int32_t y) const
{
	return PRIVATE()->Cursor;
}

void RmGUIText::setCursorPosition(int32_t value)
{
	auto painter = getPainter();
	if (painter == nullptr) painter = getContext()->getPainter();
	if (painter == nullptr) return;

	PRIVATE()->Cursor = std::clamp<int32_t>(value, 0, PRIVATE()->Text.length());

	RmRect cursorRect;
	painter->setFont(PRIVATE()->Style.Font);
	int32_t row = (int32_t)PRIVATE()->Row, column = (int32_t)PRIVATE()->Column;
	painter->boundingRect(getRect().X, getRect().Y, -1, getRect().H, PRIVATE()->Text, PRIVATE()->Cursor, &row, &column, &cursorRect);
	PRIVATE()->Row = row; PRIVATE()->Column = column;
}

RmString RmGUIText::getSelectedText() const
{
	return PRIVATE()->SelectedText;
}

void RmGUIText::getSelection(int32_t& start, int32_t& length) const
{
	start = std::min(PRIVATE()->CursorStart, PRIVATE()->Cursor);
	length = std::abs(PRIVATE()->CursorStart - PRIVATE()->Cursor);
}

void RmGUIText::setSelection(int32_t start, int32_t length)
{
	auto painter = getPainter();
	if (painter == nullptr) painter = getContext()->getPainter();
	if (painter == nullptr) return;

	PRIVATE()->CursorStart = std::clamp<int32_t>(start, 0, PRIVATE()->Text.size());
	PRIVATE()->Cursor = std::clamp<int32_t>(start + std::max<int32_t>(0, length), 0, PRIVATE()->Text.size());

	RmRect cursorRect;
	painter->setFont(PRIVATE()->Style.Font);
	int32_t row = (int32_t)PRIVATE()->Row, column = (int32_t)PRIVATE()->Column;
	painter->boundingRect(getRect().X, getRect().Y, -1, getRect().H, PRIVATE()->Text, PRIVATE()->Cursor, &row, &column, &cursorRect);
	PRIVATE()->Row = row; PRIVATE()->Column = column;
}

RmString RmGUIText::getText() const
{
	return PRIVATE()->Text;
}

void RmGUIText::setText(RmString const& value)
{
	PRIVATE()->Row = 0;
	PRIVATE()->Column = 0;
	PRIVATE()->Cursor = 0;
	PRIVATE()->RedoUndo.reset();
	auto row = PRIVATE()->Row, column = PRIVATE()->Column;
	PRIVATE()->RedoUndo.locate(row, column, true);
	PRIVATE()->RedoUndo.insert(row, column, PRIVATE()->Text.substr(0, PRIVATE()->MaxLength));
	PRIVATE()->RedoUndo.locate(row, column, false);
	PRIVATE()->Row = row; PRIVATE()->Column = column;
	PRIVATE()->Text.clear();
	PRIVATE()->RedoUndo.text(PRIVATE()->Text);

	deselect();
}

void RmGUIText::backspace()
{

	if (PRIVATE()->CursorStart != -1 && PRIVATE()->CursorStart != PRIVATE()->Cursor)
	{
		int32_t start, length;
		getSelection(start, length);
		size_t row = PRIVATE()->Row, column = start;
		size_t row2 = PRIVATE()->Row, column2 = start + length;
		PRIVATE()->RedoUndo.locate(row, column, true);
		PRIVATE()->RedoUndo.locate(row2, column2, true);
		PRIVATE()->RedoUndo.remove(row, column, column2 - column);
		PRIVATE()->RedoUndo.locate(row, column, false);
		PRIVATE()->Row = row; PRIVATE()->Column = column;
	}
	else
	{
		auto row = PRIVATE()->Row, column = PRIVATE()->Column;
		column = column - 1;
		PRIVATE()->RedoUndo.locate(row, column, true);
		PRIVATE()->RedoUndo.remove(row, column, 1);
		PRIVATE()->RedoUndo.locate(row, column, false);
		PRIVATE()->Row = row; PRIVATE()->Column = column;
	}
	PRIVATE()->Text.clear();
	PRIVATE()->RedoUndo.text(PRIVATE()->Text);

	deselect();
}

void RmGUIText::del()
{
	if (PRIVATE()->CursorStart != -1 && PRIVATE()->CursorStart != PRIVATE()->Cursor)
	{
		int32_t start, length;
		getSelection(start, length);
		size_t row = PRIVATE()->Row, column = start;
		size_t row2 = PRIVATE()->Row, column2 = start + length;
		PRIVATE()->RedoUndo.locate(row, column, true);
		PRIVATE()->RedoUndo.locate(row2, column2, true);
		PRIVATE()->RedoUndo.remove(row, column, column2 - column);
		PRIVATE()->RedoUndo.locate(row, column, false);
		PRIVATE()->Row = row; PRIVATE()->Column = column;
	}
	else
	{
		auto row = PRIVATE()->Row, column = PRIVATE()->Column;
		PRIVATE()->RedoUndo.locate(row, column, true);
		PRIVATE()->RedoUndo.remove(row, column, 1);
		PRIVATE()->RedoUndo.locate(row, column, false);
		PRIVATE()->Row = row; PRIVATE()->Column = column;
	}
	PRIVATE()->Text.clear();
	PRIVATE()->RedoUndo.text(PRIVATE()->Text);

	deselect();
}

void RmGUIText::deselect()
{
	PRIVATE()->CursorStart = -1;
}

void RmGUIText::insert(RmString const& text)
{
	auto row = PRIVATE()->Row, column = PRIVATE()->Column;
	PRIVATE()->RedoUndo.locate(row, column, true);
	PRIVATE()->RedoUndo.insert(row, column, text.substr(0, PRIVATE()->MaxLength));
	PRIVATE()->RedoUndo.locate(row, column, false);
	PRIVATE()->Row = row; PRIVATE()->Column = column;
	PRIVATE()->Text.clear();
	PRIVATE()->RedoUndo.text(PRIVATE()->Text);
}

void RmGUIText::clear()
{
	PRIVATE()->Row = 0;
	PRIVATE()->Column = 0;
	PRIVATE()->Cursor = 0;
	PRIVATE()->RedoUndo.reset();
	PRIVATE()->RedoUndo.text(PRIVATE()->Text);

	deselect();
}

void RmGUIText::cut()
{
}

void RmGUIText::redo()
{
	PRIVATE()->RedoUndo.redo(PRIVATE()->Row, PRIVATE()->Column);
	PRIVATE()->Text.clear();
	PRIVATE()->RedoUndo.text(PRIVATE()->Text);
}

void RmGUIText::undo()
{
	PRIVATE()->RedoUndo.undo(PRIVATE()->Row, PRIVATE()->Column);
	PRIVATE()->Text.clear();
	PRIVATE()->RedoUndo.text(PRIVATE()->Text);
}

void RmGUIText::keyPressEvent(IRmGUIKeyEventRaw event)
{
	// Backspace
	if (event->Key == 8)
	{
		this->backspace();
	}
	// Del
	if (event->Key == 127)
	{
		this->del();
	}
	// Enter
	if (event->Key == 13)
	{
	}
	// Left
	if (event->Key == 1073741904)
	{
		auto row = PRIVATE()->Row, column = PRIVATE()->Column;
		PRIVATE()->RedoUndo.locate(row, column, true);
		column = column - 1;
		PRIVATE()->RedoUndo.locate(row, column, false);
		PRIVATE()->Row = row; PRIVATE()->Column = column;
	}
	// Right
	if (event->Key == 1073741903)
	{
		auto row = PRIVATE()->Row, column = PRIVATE()->Column;
		PRIVATE()->RedoUndo.locate(row, column, true);
		column = column + 1;
		PRIVATE()->RedoUndo.locate(row, column, false);
		PRIVATE()->Row = row; PRIVATE()->Column = column;
	}
	// CTRL + A
	if (event->Key == 0 && (event->Modifiers & 0))
	{
	}
	// CTRL + C
	if (event->Key == 0 && (event->Modifiers & 0))
	{
	}
	// CTRL + V
	if (event->Key == 0 && (event->Modifiers & 0))
	{
	}
	// CTRL + Z
	if (event->Key == 0 && (event->Modifiers & 0))
	{
	}
	// CTRL + C
	if (event->Key == 0 && (event->Modifiers & 0))
	{
	}
	// CTRL + Z
	if (event->Key == 0 && (event->Modifiers & 0))
	{
	}
	// CTRL + Y
	if (event->Key == 0 && (event->Modifiers & 0))
	{
	}
	// CTRL + X
	if (event->Key == 0 && (event->Modifiers & 0))
	{
	}
}

void RmGUIText::inputEvent(IRmGUITextInputEventRaw event)
{
	if (getContext()->getFocus() == this)
	{
		if (event->Done)
		{
			if (PRIVATE()->MaxLength <= PRIVATE()->Text.size()) return;

			insert(event->Text);
			deselect();

			auto painter = getPainter();
			if (painter == nullptr) painter = getContext()->getPainter();
			if (painter == nullptr) return;
			RmRect cursorRect;
			int row = 0, column = 0;
			painter->setFont(PRIVATE()->Style.Font);
			painter->boundingRect(getRect().X, getRect().Y, -1, getRect().H, PRIVATE()->Text, row, column, nullptr, &cursorRect);
			PRIVATE()->OnEditingStarted.emit(RmOverlap(getViewport(), cursorRect));
		}
	}
}

void RmGUIText::mouseDoubleEvent(IRmGUIMouseEventRaw event)
{
	auto viewport = RmOverlap(getViewport(), getRect());
	if (viewport.X <= event->X && event->X <= viewport.X + viewport.W
		&& viewport.Y <= event->Y && event->Y <= viewport.Y + viewport.H)
	{
		if (event->Button == 1 || event->Button == 3)
		{
			auto painter = getPainter();
			if (painter == nullptr) painter = getContext()->getPainter();
			if (painter == nullptr) return;
			RmRect cursorRect;
			int row = 0, column = 0;
			painter->setFont(PRIVATE()->Style.Font);
			painter->boundingRect(getRect().X, getRect().Y, -1, getRect().H, PRIVATE()->Text, event->X, event->Y, &row, &column, &PRIVATE()->Cursor, &cursorRect);
			PRIVATE()->Row = row;
			PRIVATE()->Column = column;
			PRIVATE()->CursorStart = PRIVATE()->Cursor;
			PRIVATE()->MousePress = true;

			getContext()->setFocus(this);
			PRIVATE()->OnEditingStarted.emit(RmOverlap(viewport, cursorRect));
		}
	}
	else
	{
		getContext()->setFocus(nullptr);
	}
}

void RmGUIText::mousePressEvent(IRmGUIMouseEventRaw event)
{
	auto viewport = RmOverlap(getViewport(), getRect());
	if (viewport.X <= event->X && event->X <= viewport.X + viewport.W
		&& viewport.Y <= event->Y && event->Y <= viewport.Y + viewport.H)
	{
		if (event->Button == 1 || event->Button == 3)
		{
			auto painter = getPainter();
			if (painter == nullptr) painter = getContext()->getPainter();
			if (painter == nullptr) return;
			RmRect cursorRect;
			int row = 0, column = 0;
			painter->setFont(PRIVATE()->Style.Font);
			painter->boundingRect(getRect().X, getRect().Y, -1, getRect().H, PRIVATE()->Text, event->X, event->Y, &row, &column, &PRIVATE()->Cursor, &cursorRect);
			PRIVATE()->Row = row;
			PRIVATE()->Column = column;
			PRIVATE()->CursorStart = PRIVATE()->Cursor;
			PRIVATE()->MousePress = true;

			getContext()->setFocus(this);
			PRIVATE()->OnEditingStarted.emit(RmOverlap(viewport, cursorRect));
		}
	}
	else
	{
		getContext()->setFocus(nullptr);
	}
}

void RmGUIText::mouseReleaseEvent(IRmGUIMouseEventRaw event)
{
	PRIVATE()->MousePress = false;
	if (PRIVATE()->CursorStart == PRIVATE()->Cursor)
	{
		PRIVATE()->CursorStart = -1;
	}
}

void RmGUIText::mouseMoveEvent(IRmGUIMouseEventRaw event)
{
	if (PRIVATE()->MousePress)
	{
		auto painter = getPainter();
		if (painter == nullptr) painter = getContext()->getPainter();
		if (painter == nullptr) return;
		RmRect cursorRect;
		int row = 0, column = 0;
		painter->setFont(PRIVATE()->Style.Font);
		painter->boundingRect(getRect().X, getRect().Y, -1, getRect().H, PRIVATE()->Text, event->X, event->Y, &row, &column, &PRIVATE()->Cursor, &cursorRect);
		PRIVATE()->Row = row;
		PRIVATE()->Column = column;
	}
}