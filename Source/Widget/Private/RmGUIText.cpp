#include "RmGUIText.h"
#include "RmPieceTable.h"

class RmGUITextPrivateData : public RmGUIWidgetPrivate
{
public:
	RmGUITextStyle Style;
	RmString Text;
	uint32_t CursorStart = 0, Cursor = 0;
	RmPieceTable UndoRedo;
	RmGUISignalAs<int32_t /*oldPos*/, int32_t /*newPos*/> OnCursorPositionChanged;
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
	editingFinished(nullptr),
	inputRejected(nullptr),
	returnPressed(nullptr),
	selectionChanged(nullptr),
	textChanged(nullptr),
	textEdited(nullptr)
{
	m_PrivateText = new RmGUITextPrivateData;
	cursorPositionChanged = &PRIVATE()->OnCursorPositionChanged;
	editingFinished = &PRIVATE()->OnEditingFinished;
	inputRejected = &PRIVATE()->OnInputRejected;
	returnPressed = &PRIVATE()->OnReturnPressed;
	selectionChanged = &PRIVATE()->OnSelectionChanged;
	textChanged = &PRIVATE()->OnTextChanged;
	textEdited = &PRIVATE()->OnTextEdited;
}

RmGUIText::~RmGUIText()
{
	delete m_PrivateText; m_PrivateText = nullptr;
}

RmGUITextStyle RmGUIText::getStyle() const
{
	return RmGUITextStyle();
}

void RmGUIText::setStyle(RmGUITextStyle value)
{
}

RmGUIText::echo_t RmGUIText::getEchoMode() const
{
	return echo_t();
}

void RmGUIText::setEchoMode(echo_t value)
{
}

int32_t RmGUIText::getMaxLength() const
{
	return 0;
}

void RmGUIText::setMaxLength(int32_t value)
{
}

RmString RmGUIText::getPlaceholderText() const
{
	return RmString();
}

void RmGUIText::setPlaceholderText(RmString const& value)
{
}

bool RmGUIText::getReadOnly() const
{
	return false;
}

void RmGUIText::setReadOnly(bool value)
{
}

RmFontAligns RmGUIText::getAlignment() const
{
	return RmFontAligns();
}

void RmGUIText::setAlignment(RmFontAligns value)
{
}

int32_t RmGUIText::getCursorPosition() const
{
	return 0;
}

int32_t RmGUIText::getCursorPosition(int32_t x, int32_t y) const
{
	return 0;
}

void RmGUIText::setCursorPosition(int32_t value)
{
}

RmString RmGUIText::getSelectedText() const
{
	return RmString();
}

void RmGUIText::setSelectedText(RmString const value)
{
}

void RmGUIText::getSelection(int32_t& start, int32_t& length) const
{
}

void RmGUIText::setSelection(int32_t start, int32_t& length)
{
}

RmString RmGUIText::getText() const
{
	return RmString();
}

void RmGUIText::setText(RmString const& value)
{
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