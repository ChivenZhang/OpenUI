/*=================================================
* Copyright © 2020-2026 ChivenZhang.
* All Rights Reserved.
* =====================Note=========================
*
*
* ====================History=======================
* Created by ChivenZhang@gmail.com.
*
* =================================================*/
#include "UIInput.h"
#include "../UICanvas.h"
#include "../Private/UIPieceTableV3.h"
#include "../Private/UIComposition.h"
#include <algorithm>
#include <climits>
#include "OpenUI/UIDevice.h"

struct UIInputPrivate : UIPrivate
{
	UIInputStyle Style;
	UIPieceTableV3 PieceTable;
	UIComposition Composition;
	UIRect Selection;
	bool ReadOnly = false, MousePress = false;
	UIString PlaceholderText;
	UIInput::echo_t EchoMode = UIInput::EchoModeNormal;
	int32_t MaxLength = INT16_MAX;
	float SelectOffset = 0, DragOffset = 0;

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
#define PRIVATE() ((UIInputPrivate*) m_Private)

struct InputState
{
	uint64_t Revision = 0;
	int32_t Cursor = 0;
	int32_t SelectionStart = -1;
	int32_t SelectionLength = 0;
};

static int32_t inputCodePoints(UIString const& text, int32_t byteIndex)
{
	byteIndex = std::clamp<int32_t>(byteIndex, 0, (int32_t)text.size());
	int32_t index = 0;
	int32_t count = 0;
	while (index < byteIndex)
	{
		int32_t bytes = ::UIUTF8Num(text[(size_t)index]);
		if (bytes <= 0) bytes = 1;
		if (index + bytes > byteIndex) break;
		index += bytes;
		++count;
	}
	return count;
}

static int32_t inputByteIndex(UIString const& text, int32_t codePoint)
{
	int32_t index = 0;
	int32_t count = 0;
	while (index < (int32_t)text.size() && count < codePoint)
	{
		int32_t bytes = ::UIUTF8Num(text[(size_t)index]);
		if (bytes <= 0) bytes = 1;
		index += bytes;
		++count;
	}
	return index;
}

static UIString inputMask(UIString const& text)
{
	return UIString((size_t)inputCodePoints(text, (int32_t)text.size()), '*');
}

struct UIInputDisplay
{
	UIString Text;
	int32_t Cursor = 0, CursorStart = -1;
	int32_t CompositionStart = -1, CompositionEnd = -1;
};

static bool inputMasked(UIInput::echo_t mode, bool focused)
{
	if (mode == UIInput::EchoModePassword) return true;
	if (mode == UIInput::EchoModePasswordOnEdit) return focused == false;
	return false;
}

static bool inputCopyAllowed(UIInput::echo_t mode)
{
	return mode != UIInput::EchoModeNoEcho && mode != UIInput::EchoModePassword;
}

static UIInputDisplay inputDisplay(UIInputPrivate const* data, bool focused)
{
	if (data->EchoMode == UIInput::EchoModeNoEcho) return {};
	auto composition = data->Composition.display(data->PieceTable, focused);
	UIInputDisplay shown{ composition.Text, composition.Cursor, composition.CursorStart,
		composition.CompositionStart, composition.CompositionEnd };
	if (inputMasked(data->EchoMode, focused))
	{
		shown.Cursor = inputCodePoints(shown.Text, shown.Cursor);
		if (shown.CursorStart >= 0) shown.CursorStart = inputCodePoints(shown.Text, shown.CursorStart);
		if (shown.CompositionStart >= 0)
		{
			shown.CompositionStart = inputCodePoints(shown.Text, shown.CompositionStart);
			shown.CompositionEnd = inputCodePoints(shown.Text, shown.CompositionEnd);
		}
		shown.Text = inputMask(shown.Text);
	}
	return shown;
}

static int32_t inputCursorFromDisplay(UIInputPrivate const* data, bool focused, int32_t displayCursor)
{
	if (data->EchoMode == UIInput::EchoModeNoEcho) return data->PieceTable.cursor();
	if (inputMasked(data->EchoMode, focused))
		displayCursor = inputByteIndex(data->Composition.display(data->PieceTable, focused).Text, displayCursor);
	if (focused) displayCursor = data->Composition.documentOffset(data->PieceTable, displayCursor);
	return std::clamp<int32_t>(displayCursor, 0, (int32_t)data->PieceTable.size());
}

static UIFont inputFont(UIFont font)
{
	font.LineWrap = false;
	font.Ellipsize = UIFont::EllipsizeNone;
	return font;
}

static UIRect inputField(UIRect bounds)
{
	return { bounds.X + 8.0f, bounds.Y, std::max(0.0f, bounds.W - 16.0f), bounds.H };
}

static void inputClampOffset(UIInputPrivate* data, UIRect const& field, float textWidth)
{
	if (textWidth <= field.W) data->SelectOffset = 0;
	else data->SelectOffset = std::clamp(data->SelectOffset, field.W - textWidth, 0.0f);
}

static void inputRevealCaret(UIInputPrivate* data, UIRect const& field, float textWidth, float caretLocal)
{
	inputClampOffset(data, field, textWidth);
	if (textWidth <= field.W) return;
	float caretX = field.X + data->SelectOffset + caretLocal;
	float right = field.X + field.W - 1.0f;
	if (caretX < field.X) data->SelectOffset += field.X - caretX;
	else if (caretX > right) data->SelectOffset -= caretX - right;
	inputClampOffset(data, field, textWidth);
}

static float inputOrigin(UIRect const& field, float offset, float textWidth)
{
	return textWidth <= field.W ? field.X : field.X + offset;
}

static float inputLayoutWidth(UIRect const& field, float textWidth)
{
	return textWidth <= field.W ? field.W : 0.0f;
}

static int32_t inputHitDisplay(UIPainterRaw painter, UIFont const& font, UIRect const& field, float offset, UIString const& text, float x, float y, UIRect* cursorRect)
{
	painter->setFont(font);
	float textWidth = painter->boundingRect(0, 0, 0, field.H, text, 0).W;
	float hitX = std::clamp(x, field.X, std::max(field.X, field.X + field.W - 1.0f));
	int cursor = 0;
	painter->boundingRect(inputOrigin(field, offset, textWidth), field.Y, inputLayoutWidth(field, textWidth), field.H, text, hitX, y, &cursor, cursorRect);
	return cursor;
}

static InputState inputState(UIInputPrivate const* data)
{
	InputState state;
	state.Revision = data->PieceTable.revision();
	state.Cursor = data->PieceTable.cursor();
	data->PieceTable.selection(state.SelectionStart, state.SelectionLength);
	return state;
}

static void inputPublish(UIInputPrivate* data, UICanvasRaw canvas, InputState const& previous, bool edited)
{
	data->Composition.clear();
	InputState now = inputState(data);
	if (previous.Revision != now.Revision)
	{
		// Keep signal arguments stable if a slot edits the widget again.
		UIString text = data->PieceTable.text();
		data->OnTextChanged.signal(text);
		if (edited) data->OnTextEdited.signal(text);
	}
	if (previous.Cursor != now.Cursor)
		data->OnCursorPositionChanged.signal(previous.Cursor, now.Cursor);
	if (previous.SelectionStart != now.SelectionStart || previous.SelectionLength != now.SelectionLength)
		data->OnSelectionChanged.signal();
	if (canvas) canvas->layoutWidget();
}

static UIString inputSingleLine(UIString text)
{
	text.erase(std::remove(text.begin(), text.end(), '\n'), text.end());
	text.erase(std::remove(text.begin(), text.end(), '\r'), text.end());
	return text;
}

static void inputTrimBytes(UIString& text, size_t room)
{
	if (text.size() <= room) return;
	size_t keep = 0;
	while (keep < text.size())
	{
		int32_t bytes = ::UIUTF8Num(text[keep]);
		if (bytes <= 0) bytes = 1;
		if (keep + (size_t)bytes > room) break;
		keep += (size_t)bytes;
	}
	text.resize(keep);
}

static size_t inputRoom(UIInputPrivate const* data)
{
	if (data->MaxLength <= 0) return 0;
	size_t current = data->PieceTable.size();
	size_t selected = (size_t)std::max(0, data->PieceTable.selectionLength());
	if (current >= selected) current -= selected;
	size_t limit = (size_t)data->MaxLength;
	if (limit <= current) return 0;
	return limit - current;
}

static bool inputCommit(UIInputPrivate* data, UICanvasRaw canvas, UIString text, bool merge)
{
	if (data->ReadOnly)
	{
		data->OnInputRejected.signal();
		return false;
	}
	text = inputSingleLine(std::move(text));
	if (text.empty()) return false;
	size_t room = inputRoom(data);
	if (room == 0)
	{
		data->OnInputRejected.signal();
		return false;
	}
	inputTrimBytes(text, room);
	if (text.empty())
	{
		data->OnInputRejected.signal();
		return false;
	}
	auto previous = inputState(data);
	if (data->PieceTable.insert(text, merge) == false) return false;
	inputPublish(data, canvas, previous, true);
	return true;
}

UIInput::UIInput(UICanvasRaw canvas)
	:
	UIWidget(canvas)
{
	m_Private = new UIInputPrivate;

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
	setStyle(PRIVATE()->Style);
}

UIInput::~UIInput()
{
	delete m_Private; m_Private = nullptr;
}

void UIInput::arrange(UIRect client)
{
}

void UIInput::layout(UIRect client)
{
	auto painter = getCanvas()->getPainter();
	bool focused = getCanvas() && getCanvas()->getFocus() == this;
	if (!focused) PRIVATE()->Composition.clear();
	auto shown = inputDisplay(PRIVATE(), focused);
	auto font = inputFont(getStyle().Foreground.Font);
	auto field = inputField(client);
	painter->setFont(font);
	UIRect caretNatural;
	auto natural = painter->boundingRect(0, 0, 0, field.H, shown.Text, shown.Cursor, &caretNatural);
	inputRevealCaret(PRIVATE(), field, natural.W, caretNatural.X);

	float origin = inputOrigin(field, PRIVATE()->SelectOffset, natural.W);
	float layoutWidth = inputLayoutWidth(field, natural.W);
	UIRect cursorRect;
	painter->boundingRect(origin, field.Y, layoutWidth, field.H, shown.Text, shown.Cursor, &cursorRect);
	if (shown.CursorStart != -1 && shown.CursorStart != shown.Cursor)
	{
		UIRect cursorStartRect;
		painter->boundingRect(origin, field.Y, layoutWidth, field.H, shown.Text, shown.CursorStart, &cursorStartRect);
		float x0 = std::min(cursorStartRect.X, cursorRect.X);
		float x1 = std::max(cursorStartRect.X, cursorRect.X);
		PRIVATE()->Selection = UIOverlap(field, UIRect{ x0, field.Y, x1 - x0, field.H });
	}
	else
	{
		PRIVATE()->Selection = {};
	}
}

void UIInput::paint(UIRect client, UIPainterRaw painter)
{
	UIWidget::paint(client, painter);
	bool focused = getCanvas() && getCanvas()->getFocus() == this;
	if (!focused) PRIVATE()->Composition.clear();
	auto& widgetStyle = getEnable() == false ? getStyleDisabled()
		: focused ? getStyleActive()
		: getStyleNormal();
	auto face = UIFaceOf(widgetStyle, client);
	UIDrawFace(painter, client, face);

	auto style = getStyle();
	auto font = inputFont(style.Foreground.Font);
	auto field = inputField(client);
	painter->setFont(font);
	painter->setClipRect(field.X, field.Y, field.W, field.H);
	if (PRIVATE()->PieceTable.empty() && focused == false)
	{
		painter->setPen(UINoPen);
		painter->setBrush(UICtrlMutedBrush);
		painter->drawText(field.X, field.Y, 0, field.H, PRIVATE()->PlaceholderText);
		return;
	}

	if (PRIVATE()->Selection.W != 0 && PRIVATE()->Selection.H != 0)
	{
		painter->setPen(UINoPen);
		painter->setBrush(UICtrlPressBrush);
		painter->drawRect(PRIVATE()->Selection.X, PRIVATE()->Selection.Y + 4, PRIVATE()->Selection.W, std::max(0.0f, PRIVATE()->Selection.H - 8));
	}

	auto shown = inputDisplay(PRIVATE(), focused);
	float textWidth = painter->boundingRect(0, 0, 0, field.H, shown.Text, 0).W;
	float origin = inputOrigin(field, PRIVATE()->SelectOffset, textWidth);
	painter->setPen(UINoPen);
	painter->setBrush(face.Text.Style == UIBrush::NoBrush ? style.Foreground.Brush : face.Text);
	UIRect cursorRect;
	painter->drawText(origin, field.Y, inputLayoutWidth(field, textWidth), field.H, shown.Text, nullptr, shown.Cursor, &cursorRect);
	if (shown.CompositionStart >= 0)
	{
		UIRect start, end;
		painter->boundingRect(origin, field.Y, inputLayoutWidth(field, textWidth), field.H, shown.Text, shown.CompositionStart, &start);
		painter->boundingRect(origin, field.Y, inputLayoutWidth(field, textWidth), field.H, shown.Text, shown.CompositionEnd, &end);
		painter->setPen(UICtrlAccentPen);
		painter->drawLine(start.X, start.Y + start.H - 1, end.X, end.Y + end.H - 1);
	}

	if (focused)
	{
		painter->setPen(face.Border.Style == UIPen::NoPen ? UICtrlAccentPen : face.Border);
		painter->setBrush(UINoBrush);
		painter->drawLine(cursorRect.X, client.Y + 6, cursorRect.X, client.Y + client.H - 6);
	}
}

UIInputStyle UIInput::getStyle() const
{
	UIInputStyle result = PRIVATE()->Style;
	readFrame(getStyleNormal(), result.Background.Pen, result.Background.Brush);
	applyFontFamily(getStyleNormal(), result.Foreground.Font);
	if (getStyleNormal().Foreground.Color.Type != UI_CSS_COLOR_CURRENTCOLOR)
	{
		auto text = readColor(getStyleNormal().Foreground.Color);
		result.Foreground.Brush = text.A <= 0.0f ? UIBrush{ .Style = UIBrush::NoBrush } : UIBrush{ .Color = text };
	}
	return result;
}

void UIInput::setStyle(UIInputStyle value)
{
	PRIVATE()->Style = value;
	auto text = value.Foreground.Brush.Style == UIBrush::NoBrush ? UINoBrush : value.Foreground.Brush;
	auto fill = value.Background.Brush.Style == UIBrush::NoBrush ? UINoBrush : value.Background.Brush;
	UIPlate(getStyleNormal(), value.Background.Pen, fill, text, 4);
	UIPlate(getStyleHover(), UICtrlLineHoverPen, fill, text, 4);
	UIPlate(getStyleActive(), UICtrlAccentPen, fill, text, 4);
	UIPlate(getStyleDisabled(), UICtrlDisabledPen, UICtrlDisabledFillBrush, UICtrlDisabledInkBrush, 4);
	assignFontFamily(getStyleNormal(), value.Foreground.Font.Family);
}

UIInput::echo_t UIInput::getEchoMode() const
{
	return PRIVATE()->EchoMode;
}

void UIInput::setEchoMode(echo_t value)
{
	PRIVATE()->EchoMode = value;
	if (getCanvas()) getCanvas()->layoutWidget();
}

int32_t UIInput::getMaxLength() const
{
	return PRIVATE()->MaxLength;
}

void UIInput::setMaxLength(int32_t value)
{
	PRIVATE()->MaxLength = value < 0 ? 0 : value;
	setText(PRIVATE()->PieceTable.text());
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
	if (value) PRIVATE()->Composition.clear();
	if (getCanvas()) getCanvas()->layoutWidget();
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
	return PRIVATE()->PieceTable.cursor();
}

int32_t UIInput::getCursorPosition(int32_t x, int32_t y) const
{
	auto painter = getCanvas()->getPainter();
	bool focused = getCanvas() && getCanvas()->getFocus() == this;
	auto shown = inputDisplay(PRIVATE(), focused);
	auto field = inputField(getBounds());
	int32_t displayCursor = inputHitDisplay(painter, inputFont(getStyle().Foreground.Font), field, PRIVATE()->SelectOffset, shown.Text, (float)x, (float)y, nullptr);
	return inputCursorFromDisplay(PRIVATE(), focused, displayCursor);
}

void UIInput::setCursorPosition(int32_t value)
{
	auto previous = inputState(PRIVATE());
	PRIVATE()->PieceTable.moveCaret(value, false);
	inputPublish(PRIVATE(), getCanvas(), previous, false);
}

UIString UIInput::getDisplayText() const
{
	bool focused = getCanvas() && getCanvas()->getFocus() == this;
	return inputDisplay(PRIVATE(), focused).Text;
}

UIString UIInput::getSelectedText() const
{
	return PRIVATE()->PieceTable.selectedText();
}

bool UIInput::hasSelectedText() const
{
	return PRIVATE()->PieceTable.hasSelection();
}

void UIInput::getSelection(int32_t& start, int32_t& length) const
{
	PRIVATE()->PieceTable.selection(start, length);
}

int32_t UIInput::getSelectionStart() const
{
	int32_t start = -1;
	int32_t length = 0;
	getSelection(start, length);
	return length > 0 ? start : -1;
}

int32_t UIInput::getSelectionEnd() const
{
	int32_t start = -1;
	int32_t length = 0;
	getSelection(start, length);
	return length > 0 ? start + length : -1;
}

int32_t UIInput::getSelectionLength() const
{
	return PRIVATE()->PieceTable.selectionLength();
}

void UIInput::setSelection(int32_t start, int32_t length)
{
	auto previous = inputState(PRIVATE());
	PRIVATE()->PieceTable.setSelection(start, length);
	inputPublish(PRIVATE(), getCanvas(), previous, false);
}

void UIInput::selectAll()
{
	auto previous = inputState(PRIVATE());
	PRIVATE()->PieceTable.selectAll();
	inputPublish(PRIVATE(), getCanvas(), previous, false);
}

UIString UIInput::getText() const
{
	return PRIVATE()->PieceTable.text();
}

void UIInput::setText(UIString const& value)
{
	PRIVATE()->Composition.clear();
	if (getCanvas()) getCanvas()->layoutWidget();
	UIString text = inputSingleLine(value);
	inputTrimBytes(text, PRIVATE()->MaxLength > 0 ? (size_t)PRIVATE()->MaxLength : 0);
	auto previous = inputState(PRIVATE());
	if (PRIVATE()->PieceTable.setText(text) == false) return;
	inputPublish(PRIVATE(), getCanvas(), previous, false);
}

bool UIInput::isModified() const
{
	return PRIVATE()->PieceTable.isModified();
}

void UIInput::setModified(bool value)
{
	PRIVATE()->PieceTable.setModified(value);
}

bool UIInput::isUndoAvailable() const
{
	return PRIVATE()->PieceTable.canUndo();
}

bool UIInput::isRedoAvailable() const
{
	return PRIVATE()->PieceTable.canRedo();
}

void UIInput::backspace()
{
	if (PRIVATE()->ReadOnly)
	{
		PRIVATE()->OnInputRejected.signal();
		return;
	}
	auto previous = inputState(PRIVATE());
	if (PRIVATE()->PieceTable.backspace() == false) return;
	inputPublish(PRIVATE(), getCanvas(), previous, true);
}

void UIInput::del()
{
	if (PRIVATE()->ReadOnly)
	{
		PRIVATE()->OnInputRejected.signal();
		return;
	}
	auto previous = inputState(PRIVATE());
	if (PRIVATE()->PieceTable.del() == false) return;
	inputPublish(PRIVATE(), getCanvas(), previous, true);
}

void UIInput::deselect()
{
	auto previous = inputState(PRIVATE());
	PRIVATE()->PieceTable.deselect();
	inputPublish(PRIVATE(), getCanvas(), previous, false);
}

void UIInput::insert(UIString const& text)
{
	inputCommit(PRIVATE(), getCanvas(), text, true);
}

void UIInput::cursorBackward(bool mark, int32_t steps)
{
	auto previous = inputState(PRIVATE());
	PRIVATE()->PieceTable.moveCodePoints(-steps, mark);
	inputPublish(PRIVATE(), getCanvas(), previous, false);
}

void UIInput::cursorForward(bool mark, int32_t steps)
{
	auto previous = inputState(PRIVATE());
	PRIVATE()->PieceTable.moveCodePoints(steps, mark);
	inputPublish(PRIVATE(), getCanvas(), previous, false);
}

void UIInput::cursorWordBackward(bool mark)
{
	auto previous = inputState(PRIVATE());
	PRIVATE()->PieceTable.moveWords(-1, mark);
	inputPublish(PRIVATE(), getCanvas(), previous, false);
}

void UIInput::cursorWordForward(bool mark)
{
	auto previous = inputState(PRIVATE());
	PRIVATE()->PieceTable.moveWords(1, mark);
	inputPublish(PRIVATE(), getCanvas(), previous, false);
}

void UIInput::home(bool mark)
{
	auto previous = inputState(PRIVATE());
	PRIVATE()->PieceTable.moveHome(mark);
	inputPublish(PRIVATE(), getCanvas(), previous, false);
}

void UIInput::end(bool mark)
{
	auto previous = inputState(PRIVATE());
	PRIVATE()->PieceTable.moveEnd(mark);
	inputPublish(PRIVATE(), getCanvas(), previous, false);
}

void UIInput::clear()
{
	setText(UIString());
}

UIString UIInput::copy() const
{
	if (inputCopyAllowed(PRIVATE()->EchoMode) == false) return UIString();
	return PRIVATE()->PieceTable.selectedText();
}

void UIInput::cut()
{
	if (PRIVATE()->ReadOnly)
	{
		PRIVATE()->OnInputRejected.signal();
		return;
	}
	if (inputCopyAllowed(PRIVATE()->EchoMode) == false) return;
	UIString text = PRIVATE()->PieceTable.selectedText();
	if (text.empty()) return;
	PRIVATE()->OnTextCopied.signal(text);
	auto previous = inputState(PRIVATE());
	if (PRIVATE()->PieceTable.removeSelection() == false) return;
	inputPublish(PRIVATE(), getCanvas(), previous, true);
}

void UIInput::paste()
{
	UIString text;
	PRIVATE()->OnTextPasted.signal(text);
	paste(text);
}

void UIInput::paste(UIString const& text)
{
	inputCommit(PRIVATE(), getCanvas(), text, false);
}

void UIInput::redo()
{
	if (PRIVATE()->ReadOnly) { PRIVATE()->OnInputRejected.signal(); return; }
	auto previous = inputState(PRIVATE());
	if (PRIVATE()->PieceTable.redo() == false) return;
	inputPublish(PRIVATE(), getCanvas(), previous, true);
}

void UIInput::undo()
{
	if (PRIVATE()->ReadOnly) { PRIVATE()->OnInputRejected.signal(); return; }
	auto previous = inputState(PRIVATE());
	if (PRIVATE()->PieceTable.undo() == false) return;
	inputPublish(PRIVATE(), getCanvas(), previous, true);
}

void UIInput::keyPressEvent(UIKeyEventRaw event)
{
	if (event == nullptr) return;
	if (getCanvas() == nullptr || getCanvas()->getFocus() != this) return;
	if (PRIVATE()->Composition.active(PRIVATE()->PieceTable))
	{
		// Navigation, deletion and Enter belong to the IME during composition.
		event->Accept = true;
		return;
	}
	if (event->Key == UIInputEnum::KEY_ENTER)
	{
		PRIVATE()->OnReturnPressed.signal();
		PRIVATE()->OnEditingFinished.signal();
		event->Accept = true;
		return;
	}

	bool shift = (event->Modifiers & UIInputEnum::KEY_MOD_SHIFT) != 0;
	bool ctrl = (event->Modifiers & UIInputEnum::KEY_MOD_CTRL) != 0;
	if (event->Key == UIInputEnum::KEY_BACKSPACE)
	{
		if (ctrl)
		{
			if (PRIVATE()->ReadOnly) PRIVATE()->OnInputRejected.signal();
			else
			{
				auto previous = inputState(PRIVATE());
				if (PRIVATE()->PieceTable.deletePreviousWord()) inputPublish(PRIVATE(), getCanvas(), previous, true);
			}
		}
		else backspace();
	}
	else if (event->Key == UIInputEnum::KEY_DELETE)
	{
		if (ctrl)
		{
			if (PRIVATE()->ReadOnly) PRIVATE()->OnInputRejected.signal();
			else
			{
				auto previous = inputState(PRIVATE());
				if (PRIVATE()->PieceTable.deleteNextWord()) inputPublish(PRIVATE(), getCanvas(), previous, true);
			}
		}
		else del();
	}
	else if (event->Key == UIInputEnum::KEY_LEFT)
	{
		if (ctrl) cursorWordBackward(shift);
		else cursorBackward(shift, 1);
	}
	else if (event->Key == UIInputEnum::KEY_RIGHT)
	{
		if (ctrl) cursorWordForward(shift);
		else cursorForward(shift, 1);
	}
	else if (event->Key == UIInputEnum::KEY_HOME)
	{
		home(shift);
	}
	else if (event->Key == UIInputEnum::KEY_END)
	{
		end(shift);
	}
	else if (event->Key == UIInputEnum::KEY_A && ctrl)
	{
		selectAll();
	}
	else if (event->Key == UIInputEnum::KEY_C && ctrl)
	{
		auto text = copy();
		if (text.empty() == false) PRIVATE()->OnTextCopied.signal(text);
	}
	else if (event->Key == UIInputEnum::KEY_X && ctrl)
	{
		cut();
	}
	else if (event->Key == UIInputEnum::KEY_V && ctrl)
	{
		paste();
	}
	else if (event->Key == UIInputEnum::KEY_Z && ctrl)
	{
		undo();
	}
	else if (event->Key == UIInputEnum::KEY_Y && ctrl)
	{
		redo();
	}
}

void UIInput::inputEvent(UITextInputEventRaw event)
{
	if (event == nullptr) return;
	if (getCanvas() == nullptr || getCanvas()->getFocus() != this) return;
	if (PRIVATE()->ReadOnly)
	{
		PRIVATE()->OnInputRejected.signal();
		return;
	}

	if (!event->Done)
		PRIVATE()->Composition.update(PRIVATE()->PieceTable, event->Text, event->Start, event->Length);
	else
	{
		bool composing = PRIVATE()->Composition.pending(PRIVATE()->PieceTable);
		PRIVATE()->Composition.clear();
		inputCommit(PRIVATE(), getCanvas(), event->Text, !composing);
		if (composing) PRIVATE()->PieceTable.endUndoGroup();
	}
	event->Accept = true;
	getCanvas()->layoutWidget();
	auto painter = getCanvas()->getPainter();
	if (painter == nullptr) return;
	layout(getBounds());
	auto shown = inputDisplay(PRIVATE(), true);
	auto font = inputFont(getStyle().Foreground.Font);
	auto field = inputField(getBounds());
	painter->setFont(font);
	float textWidth = painter->boundingRect(0, 0, 0, field.H, shown.Text, 0).W;
	UIRect cursorRect;
	painter->boundingRect(inputOrigin(field, PRIVATE()->SelectOffset, textWidth), field.Y, inputLayoutWidth(field, textWidth), field.H, shown.Text, shown.Cursor, &cursorRect);
	if (cursorRect.X < field.X) cursorRect.X = field.X;
	if (cursorRect.X > field.X + field.W) cursorRect.X = field.X + field.W;
	PRIVATE()->OnEditingStarted.signal(UIOverlap(getViewport(), cursorRect));
}

void UIInput::mouseDoubleEvent(UIMouseEventRaw event)
{
	mousePressEvent(event);
	if (PRIVATE()->MousePress == false) return;
	auto previous = inputState(PRIVATE());
	PRIVATE()->PieceTable.selectWord();
	inputPublish(PRIVATE(), getCanvas(), previous, false);
}

void UIInput::mousePressEvent(UIMouseEventRaw event)
{
	if (UIBounds(UIOverlap(getViewport(), getBounds()), event->X, event->Y))
	{
		if (event->Button == UIInputEnum::MOUSE_BUTTON_LEFT)
		{
			UIRect cursorRect;
			auto painter = getCanvas()->getPainter();
			bool focused = getCanvas() && getCanvas()->getFocus() == this;
			auto shown = inputDisplay(PRIVATE(), focused);
			auto field = inputField(getBounds());
			int32_t displayCursor = inputHitDisplay(painter, inputFont(getStyle().Foreground.Font), field, PRIVATE()->SelectOffset, shown.Text, event->X, event->Y, &cursorRect);
			auto previous = inputState(PRIVATE());
			PRIVATE()->PieceTable.beginDrag(inputCursorFromDisplay(PRIVATE(), focused, displayCursor));
			PRIVATE()->MousePress = true;
			PRIVATE()->DragOffset = PRIVATE()->SelectOffset;

			getCanvas()->setFocus(this);
			if (getCanvas()->getDevice()) getCanvas()->getDevice()->setKeyboard(true);
			inputPublish(PRIVATE(), getCanvas(), previous, false);
			PRIVATE()->OnEditingStarted.signal(UIOverlap(getViewport(), cursorRect));
		}
	}
	else
	{
		PRIVATE()->Composition.clear();
		if (getCanvas()->getFocus() == this && getCanvas()->getDevice())
		{
			getCanvas()->getDevice()->setKeyboard(false);
			getCanvas()->setFocus(nullptr);
		}
	}
}

void UIInput::mouseReleaseEvent(UIMouseEventRaw event)
{
	if (event->Button == UIInputEnum::MOUSE_BUTTON_LEFT)
	{
		PRIVATE()->MousePress = false;
		if (PRIVATE()->PieceTable.anchor() == PRIVATE()->PieceTable.cursor())
			PRIVATE()->PieceTable.deselect();
	}
}

void UIInput::mouseMoveEvent(UIMouseEventRaw event)
{
	if (event->Button == UIInputEnum::MOUSE_BUTTON_LEFT)
	{
		if (PRIVATE()->MousePress)
		{
			auto painter = getCanvas()->getPainter();
			bool focused = getCanvas() && getCanvas()->getFocus() == this;
			auto shown = inputDisplay(PRIVATE(), focused);
			auto font = inputFont(getStyle().Foreground.Font);
			auto field = inputField(getBounds());
			painter->setFont(font);
			float textWidth = painter->boundingRect(0, 0, 0, field.H, shown.Text, 0).W;
			if (textWidth > field.W)
			{
				float offset = PRIVATE()->DragOffset;
				if (event->X < field.X) offset += field.X - event->X;
				else if (event->X > field.X + field.W) offset -= event->X - (field.X + field.W);
				PRIVATE()->SelectOffset = offset;
				inputClampOffset(PRIVATE(), field, textWidth);
			}
			int32_t displayCursor = inputHitDisplay(painter, font, field, PRIVATE()->SelectOffset, shown.Text, event->X, event->Y, nullptr);
			auto previous = inputState(PRIVATE());
			PRIVATE()->PieceTable.moveCaret(inputCursorFromDisplay(PRIVATE(), focused, displayCursor), true);
			inputPublish(PRIVATE(), getCanvas(), previous, false);
		}
	}
}

void UIInput::focusInEvent(UIFocusEventRaw event)
{
	if (getCanvas()->getFocus() == this && getCanvas()->getDevice()) getCanvas()->getDevice()->setKeyboard(true);
}

void UIInput::focusOutEvent(UIFocusEventRaw event)
{
	PRIVATE()->Composition.clear();
	if (getCanvas()) getCanvas()->layoutWidget();
	PRIVATE()->PieceTable.endUndoGroup();
	if (getCanvas()->getFocus() == this && getCanvas()->getDevice()) getCanvas()->getDevice()->setKeyboard(false);

	PRIVATE()->OnEditingFinished.signal();
}
