#include "RmGUIText.h"
#include "RmGUIContext.h"
#include "RmPieceTable.h"

class RmGUITextPrivateData : public RmGUIWidgetPrivate
{
public:
	RmGUITextStyle Style;
	RmString Text, SelectedText, PlaceholderText;
	int32_t CursorStart = 0, Cursor = 0, MaxLength = -1;
	size_t Row = 0, Column = 0;
	RmGUITextEchoMode EchoMode = RmGUITextEchoMode::EchoModeNoEcho;
	bool ReadOnly;
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

	PRIVATE()->Style.Font.Align = RmFontAlign::AlignVCenter;
}

RmGUIText::~RmGUIText()
{
	delete m_PrivateText; m_PrivateText = nullptr;
}

void RmGUIText::layout(RmRectRaw client)
{
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
		painter->setPen(PRIVATE()->Style.Pen);
		painter->setFont(PRIVATE()->Style.Font);
		painter->setBrush(PRIVATE()->Style.Brush);

		RmRect cursorRect;
		painter->drawText(client->X, client->Y, client->W, client->H, PRIVATE()->Text, nullptr, PRIVATE()->Cursor, &cursorRect);
		painter->setPen({ .Color = {1, 0, 0, 1}, .Width = 1.0f });
		painter->setBrush({ .Style = RmBrush::NoBrush });
		painter->drawLine(cursorRect.X, cursorRect.Y - 2, cursorRect.X, cursorRect.Y + cursorRect.H + 2);
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
	return int32_t();
}

void RmGUIText::setCursorPosition(int32_t value)
{
	PRIVATE()->Cursor = value;
}

RmString RmGUIText::getSelectedText() const
{
	return PRIVATE()->SelectedText;
}

void RmGUIText::getSelection(int32_t& start, int32_t& length) const
{
}

void RmGUIText::setSelection(int32_t start, int32_t& length)
{
}

RmString RmGUIText::getText() const
{
	return PRIVATE()->Text;
}

void RmGUIText::setText(RmString const& value)
{
	PRIVATE()->RedoUndo.reset();
	PRIVATE()->Row = 0;
	PRIVATE()->Column = 0;
	PRIVATE()->Cursor = 0;
	PRIVATE()->RedoUndo.insert(PRIVATE()->Row, PRIVATE()->Column, value);
	PRIVATE()->Text.clear();
	PRIVATE()->RedoUndo.text(PRIVATE()->Text);
}

void RmGUIText::backspace()
{
}

void RmGUIText::del()
{
}

void RmGUIText::deselect()
{
}

void RmGUIText::insert(RmString const& text)
{
}

void RmGUIText::clear()
{
}

void RmGUIText::cut()
{
}

void RmGUIText::redo()
{
}

void RmGUIText::undo()
{
}

void RmGUIText::keyPressEvent(IRmGUIKeyEventRaw event)
{
}

void RmGUIText::inputEvent(IRmGUITextInputEventRaw event)
{
	if (getContext()->getFocus() == this)
	{
		if (event->Done)
		{
			PRIVATE()->RedoUndo.insert(PRIVATE()->Row, PRIVATE()->Column, event->Text);
			PRIVATE()->Text.clear();
			PRIVATE()->RedoUndo.text(PRIVATE()->Text);

			auto painter = getPainter();
			if (painter == nullptr) painter = getContext()->getPainter();
			if (painter == nullptr) return;
			RmRect cursorRect;
			painter->boundingRect(getRect().X, getRect().Y, getRect().W, getRect().H, PRIVATE()->Text, PRIVATE()->Row, PRIVATE()->Column, PRIVATE()->Cursor, &cursorRect);
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
		auto painter = getPainter();
		if (painter == nullptr) painter = getContext()->getPainter();
		if (painter == nullptr) return;
		RmRect cursorRect;
		int cursor = -1, row, column;
		painter->setFont(PRIVATE()->Style.Font);
		painter->boundingRect(getRect().X, getRect().Y, getRect().W, getRect().H, PRIVATE()->Text, event->X, event->Y, row, column, cursor, &cursorRect);
		if (cursor == -1) PRIVATE()->Cursor = 0;
		else PRIVATE()->Cursor = cursor;
		if (cursor != -1)
		{
			PRIVATE()->Cursor = cursor;
			PRIVATE()->Row = row;
			PRIVATE()->Column = column;
		}

		getContext()->setFocus(this);
		PRIVATE()->OnEditingStarted.emit(RmOverlap(viewport, cursorRect));
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
		auto painter = getPainter();
		if (painter == nullptr) painter = getContext()->getPainter();
		if (painter == nullptr) return;
		RmRect cursorRect;
		int cursor = -1, row, column;
		painter->setFont(PRIVATE()->Style.Font);
		painter->boundingRect(getRect().X, getRect().Y, getRect().W, getRect().H, PRIVATE()->Text, event->X, event->Y, row, column, cursor, &cursorRect);
		if (cursor == -1) PRIVATE()->Cursor = 0;
		else PRIVATE()->Cursor = cursor;
		if (cursor != -1)
		{
			PRIVATE()->Cursor = cursor;
			PRIVATE()->Row = row;
			PRIVATE()->Column = column;
		}

		getContext()->setFocus(this);
		PRIVATE()->OnEditingStarted.emit(RmOverlap(viewport, cursorRect));
	}
	else
	{
		getContext()->setFocus(nullptr);
	}
}

void RmGUIText::mouseReleaseEvent(IRmGUIMouseEventRaw event)
{
}

void RmGUIText::mouseMoveEvent(IRmGUIMouseEventRaw event)
{
}