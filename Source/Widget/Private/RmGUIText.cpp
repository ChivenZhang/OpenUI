#include "RmGUIText.h"
#include "RmGUIText.h"
#include "RmGUIText.h"
#include "RmPieceTable.h"

class RmGUITextPrivateData : public RmGUIWidgetPrivate
{
public:
	RmGUITextStyle Style;
	RmString Text, SelectedText, PlaceholderText;
	uint32_t CursorStart = 0, Cursor = 0, MaxLength = -1;
	RmGUITextEchoMode EchoMode = RmGUITextEchoMode::EchoModeNoEcho;
	bool ReadOnly;
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

void RmGUIText::layout(RmRectRaw client)
{
}

void RmGUIText::paint(IRmGUIPainterRaw painter, RmRectRaw client)
{
	if (PRIVATE()->Text.empty() == false)
	{
		PRIVATE()->Style.Font.NoWrap = true;
		painter->setPen(PRIVATE()->Style.Pen);
		painter->setFont(PRIVATE()->Style.Font);
		painter->setBrush(PRIVATE()->Style.Brush);
		painter->drawText(client->X, client->Y, client->W, client->H, PRIVATE()->Text);
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
	PRIVATE()->Text = value;
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