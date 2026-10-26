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
#include "UIText.h"
#include "../UICanvas.h"
#include "../Private/UIPieceTableV2.h"
#include <algorithm>
#include <climits>

#include "OpenUI/UIDevice.h"

struct UITextPrivate : UIPrivate
{
	UITextStyle Style;
	UIPieceTableV2 PieceTable;
	UIList<UIRect> Selection;
	UIRect Caret;
	bool ReadOnly = false, MousePress = false, TabChangesFocus = false;
	UIString PlaceholderText;
	int32_t MaxLength = INT16_MAX;
	float ScrollX = 0, ScrollY = 0, PreferredX = -1;
	UISignalAs<int32_t, int32_t> OnCursorPositionChanged;
	UISignalAs<UIRect> OnEditingStarted;
	UISignalAs<> OnEditingFinished;
	UISignalAs<> OnInputRejected;
	UISignalAs<> OnSelectionChanged;
	UISignalAs<UIString const&> OnTextChanged;
	UISignalAs<UIString const&> OnTextEdited;
	UISignalAs<UIString&> OnTextPasted;
	UISignalAs<UIString const&> OnTextCopied;
};
#define PRIVATE() ((UITextPrivate*) m_Private)

struct TextState
{
	UIString Text;
	int32_t Cursor = 0;
	int32_t SelectionStart = -1;
	int32_t SelectionLength = 0;
};

struct TextBox
{
	float X = 0, Y = 0, W = 1, H = 1;
	float OriginX = 0, OriginY = 0;
};

static TextState textState(UITextPrivate const* data)
{
	TextState state;
	state.Text = data->PieceTable.text();
	state.Cursor = data->PieceTable.cursor();
	data->PieceTable.selection(state.SelectionStart, state.SelectionLength);
	return state;
}

static void textPublish(UITextPrivate* data, UICanvasRaw canvas, TextState const& previous, bool edited)
{
	TextState now = textState(data);
	if (previous.Text != now.Text)
	{
		data->OnTextChanged.signal(now.Text);
		if (edited) data->OnTextEdited.signal(now.Text);
	}
	if (previous.Cursor != now.Cursor)
		data->OnCursorPositionChanged.signal(previous.Cursor, now.Cursor);
	if (previous.SelectionStart != now.SelectionStart || previous.SelectionLength != now.SelectionLength)
		data->OnSelectionChanged.signal();
	if (canvas) canvas->layoutWidget();
}

static UIString textNormalize(UIString text)
{
	UIString result;
	result.reserve(text.size());
	for (size_t i = 0; i < text.size(); ++i)
	{
		if (text[i] == '\r')
		{
			if (i + 1 < text.size() && text[i + 1] == '\n') continue;
			result.push_back('\n');
		}
		else result.push_back(text[i]);
	}
	return result;
}

static void textTrimBytes(UIString& text, size_t room)
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

static size_t textRoom(UITextPrivate const* data)
{
	if (data->MaxLength <= 0) return 0;
	size_t current = data->PieceTable.size();
	size_t selected = (size_t)std::max(0, data->PieceTable.selectionLength());
	if (current >= selected) current -= selected;
	size_t limit = (size_t)data->MaxLength;
	if (limit <= current) return 0;
	return limit - current;
}

static bool textCommit(UITextPrivate* data, UICanvasRaw canvas, UIString text, bool merge)
{
	if (data->ReadOnly)
	{
		data->OnInputRejected.signal();
		return false;
	}
	text = textNormalize(std::move(text));
	if (text.empty()) return false;
	size_t room = textRoom(data);
	if (room == 0)
	{
		data->OnInputRejected.signal();
		return false;
	}
	textTrimBytes(text, room);
	if (text.empty())
	{
		data->OnInputRejected.signal();
		return false;
	}
	auto previous = textState(data);
	data->PreferredX = -1;
	if (data->PieceTable.insert(text, merge) == false) return false;
	textPublish(data, canvas, previous, true);
	return true;
}

static TextBox textBox(UITextPrivate const* data, UIRect client)
{
	TextBox box;
	box.X = client.X + 8;
	box.Y = client.Y + 8;
	box.W = std::max(1.0f, client.W - 16);
	box.H = std::max(1.0f, client.H - 16);
	box.OriginX = box.X + data->ScrollX;
	box.OriginY = box.Y + data->ScrollY;
	return box;
}

static UIRect textCaretAt(UIPainterRaw painter, UIFont const& font, TextBox const& box, UIString const& text, int32_t index)
{
	UIRect caret;
	painter->setFont(font);
	painter->boundingRect(box.OriginX, box.OriginY, box.W, 0, text, (float)index, &caret);
	if (caret.H < 1.0f) caret.H = std::max(1.0f, (float)font.Size);
	return caret;
}

static int32_t textHit(UIPainterRaw painter, UIFont const& font, TextBox const& box, UIString const& text, float x, float y)
{
	int cursor = 0;
	painter->setFont(font);
	painter->boundingRect(box.OriginX, box.OriginY, box.W, 0, text, x, y, &cursor);
	return cursor;
}

static UIRect textIntersect(UIRect const& rect, TextBox const& view)
{
	float x0 = std::max(rect.X, view.X);
	float y0 = std::max(rect.Y, view.Y);
	float x1 = std::min(rect.X + rect.W, view.X + view.W);
	float y1 = std::min(rect.Y + rect.H, view.Y + view.H);
	return { x0, y0, std::max(0.0f, x1 - x0), std::max(0.0f, y1 - y0) };
}

static void textSelection(UIList<UIRect>& out, TextBox const& box, UIRect start, UIRect end)
{
	out.clear();
	float lineH = std::max(start.H, end.H);
	if (lineH < 1.0f) lineH = 1.0f;
	if (end.Y < start.Y - 0.5f || (std::abs(end.Y - start.Y) < lineH * 0.5f && end.X < start.X))
		std::swap(start, end);
	float left = box.OriginX;
	float right = box.OriginX + box.W;
	auto push = [&](UIRect rect)
	{
		rect = textIntersect(rect, box);
		if (rect.W > 0.5f && rect.H > 0.5f) out.push_back(rect);
	};
	if (std::abs(start.Y - end.Y) < lineH * 0.5f)
	{
		push({ std::min(start.X, end.X), start.Y, std::abs(end.X - start.X), lineH });
		return;
	}
	push({ start.X, start.Y, std::max(0.0f, right - start.X), start.H });
	float midTop = start.Y + start.H;
	if (end.Y > midTop) push({ left, midTop, box.W, end.Y - midTop });
	push({ left, end.Y, std::max(0.0f, end.X - left), end.H });
}

static int32_t textBlockStart(UIString const& text, int32_t cursor)
{
	if (cursor < 0) return 0;
	if (cursor > (int32_t)text.size()) cursor = (int32_t)text.size();
	while (cursor > 0 && text[(size_t)cursor - 1] != '\n') --cursor;
	return cursor;
}

static int32_t textNextBlock(UIString const& text, int32_t cursor)
{
	if (cursor < 0) cursor = 0;
	while (cursor < (int32_t)text.size() && text[(size_t)cursor] != '\n') ++cursor;
	if (cursor < (int32_t)text.size() && text[(size_t)cursor] == '\n') ++cursor;
	return cursor;
}

static void textMoveVertical(UITextPrivate* data, UIPainterRaw painter, UIFont const& font, TextBox box, int32_t direction, float distance, bool mark)
{
	UIString const& text = data->PieceTable.text();
	UIRect caret = textCaretAt(painter, font, box, text, data->PieceTable.cursor());
	if (data->PreferredX < 0) data->PreferredX = caret.X;
	float y = direction < 0 ? caret.Y - distance : caret.Y + std::max(caret.H, 1.0f) + distance;
	int32_t index = textHit(painter, font, box, text, data->PreferredX, y);
	UIRect next = textCaretAt(painter, font, box, text, index);
	if (direction < 0 && next.Y >= caret.Y - 0.5f) index = 0;
	if (direction > 0 && next.Y <= caret.Y + 0.5f) index = (int32_t)text.size();
	data->PieceTable.moveCaret(index, mark);
}

UIText::UIText(UICanvasRaw canvas)
	:
	UIWidget(canvas)
{
	m_Private = new UITextPrivate;
	cursorPositionChanged = &PRIVATE()->OnCursorPositionChanged;
	editingStarted = &PRIVATE()->OnEditingStarted;
	editingFinished = &PRIVATE()->OnEditingFinished;
	inputRejected = &PRIVATE()->OnInputRejected;
	selectionChanged = &PRIVATE()->OnSelectionChanged;
	textChanged = &PRIVATE()->OnTextChanged;
	textEdited = &PRIVATE()->OnTextEdited;
	textPasted = &PRIVATE()->OnTextPasted;
	textCopied = &PRIVATE()->OnTextCopied;
	setMinSize(160, 96);
	setStyle(PRIVATE()->Style);
}

UIText::~UIText()
{
	delete m_Private; m_Private = nullptr;
}

void UIText::arrange(UIRect client)
{
}

void UIText::layout(UIRect client)
{
	auto painter = getCanvas()->getPainter();
	auto font = getStyle().Foreground.Font;
	UIString const& text = PRIVATE()->PieceTable.text();
	auto measure = [&](TextBox const& box, UIRect& caret)
	{
		painter->setFont(font);
		UIRect bounds = painter->boundingRect(box.OriginX, box.OriginY, box.W, 0, text, (float)PRIVATE()->PieceTable.cursor(), &caret);
		if (caret.H < 1.0f) caret.H = std::max(1.0f, (float)font.Size);
		return bounds;
	};
	TextBox box = textBox(PRIVATE(), client);
	UIRect caret;
	UIRect bounds = measure(box, caret);
	if (bounds.H <= box.H) PRIVATE()->ScrollY = 0;
	else PRIVATE()->ScrollY = std::clamp(PRIVATE()->ScrollY, box.H - bounds.H, 0.0f);
	if (bounds.W <= box.W) PRIVATE()->ScrollX = 0;
	else PRIVATE()->ScrollX = std::clamp(PRIVATE()->ScrollX, box.W - bounds.W, 0.0f);
	box = textBox(PRIVATE(), client);
	bounds = measure(box, caret);
	if (caret.Y < box.Y) PRIVATE()->ScrollY += box.Y - caret.Y;
	else if (caret.Y + caret.H > box.Y + box.H) PRIVATE()->ScrollY -= (caret.Y + caret.H) - (box.Y + box.H);
	if (caret.X < box.X) PRIVATE()->ScrollX += box.X - caret.X;
	else if (caret.X > box.X + box.W - 1.0f) PRIVATE()->ScrollX -= caret.X - (box.X + box.W - 1.0f);
	if (bounds.H <= box.H) PRIVATE()->ScrollY = 0;
	else PRIVATE()->ScrollY = std::clamp(PRIVATE()->ScrollY, box.H - bounds.H, 0.0f);
	if (bounds.W <= box.W) PRIVATE()->ScrollX = 0;
	else PRIVATE()->ScrollX = std::clamp(PRIVATE()->ScrollX, box.W - bounds.W, 0.0f);
	box = textBox(PRIVATE(), client);
	measure(box, caret);
	PRIVATE()->Caret = caret;

	PRIVATE()->Selection.clear();
	int32_t start = 0, length = 0;
	PRIVATE()->PieceTable.selection(start, length);
	if (length > 0)
	{
		UIRect end = textCaretAt(painter, font, box, text, start + length);
		UIRect from = textCaretAt(painter, font, box, text, start);
		textSelection(PRIVATE()->Selection, box, from, end);
	}
}

void UIText::paint(UIRect client, UIPainterRaw painter)
{
	UIWidget::paint(client, painter);
	bool focused = getCanvas() && getCanvas()->getFocus() == this;
	auto& widgetStyle = getEnable() == false ? getStyleDisabled()
		: focused ? getStyleActive()
		: getStyleNormal();
	auto face = UIFaceOf(widgetStyle, client);
	UIDrawFace(painter, client, face);

	auto style = getStyle();
	painter->setFont(style.Foreground.Font);
	TextBox box = textBox(PRIVATE(), client);
	if (PRIVATE()->PieceTable.empty() && focused == false)
	{
		painter->setPen(UINoPen);
		painter->setBrush(UICtrlMutedBrush);
		painter->drawText(box.OriginX, box.OriginY, box.W, 0, PRIVATE()->PlaceholderText);
		return;
	}

	if (PRIVATE()->Selection.empty() == false)
	{
		painter->setPen(UINoPen);
		painter->setBrush(UICtrlPressBrush);
		for (auto const& rect : PRIVATE()->Selection)
			painter->drawRect(rect.X, rect.Y, rect.W, rect.H);
	}

	painter->setPen(UINoPen);
	painter->setBrush(face.Text.Style == UIBrush::NoBrush ? style.Foreground.Brush : face.Text);
	UIRect caret;
	painter->drawText(box.OriginX, box.OriginY, box.W, 0, PRIVATE()->PieceTable.text(), nullptr, (float)PRIVATE()->PieceTable.cursor(), &caret);
	if (caret.H < 1.0f) caret.H = std::max(1.0f, (float)style.Foreground.Font.Size);

	if (focused)
	{
		painter->setPen(face.Border.Style == UIPen::NoPen ? UICtrlAccentPen : face.Border);
		painter->setBrush(UINoBrush);
		painter->drawLine(caret.X, caret.Y, caret.X, caret.Y + caret.H);
	}
}

UITextStyle UIText::getStyle() const
{
	UITextStyle result = PRIVATE()->Style;
	readFrame(getStyleNormal(), result.Background.Pen, result.Background.Brush);
	applyFontFamily(getStyleNormal(), result.Foreground.Font);
	if (getStyleNormal().Foreground.Color.Type != UI_CSS_COLOR_CURRENTCOLOR)
	{
		auto text = readColor(getStyleNormal().Foreground.Color);
		result.Foreground.Brush = text.A <= 0.0f ? UIBrush{ .Style = UIBrush::NoBrush } : UIBrush{ .Color = text };
	}
	return result;
}

void UIText::setStyle(UITextStyle value)
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

int32_t UIText::getMaxLength() const
{
	return PRIVATE()->MaxLength;
}

void UIText::setMaxLength(int32_t value)
{
	PRIVATE()->MaxLength = value < 0 ? 0 : value;
	setText(PRIVATE()->PieceTable.text());
}

UIString UIText::getPlaceholderText() const
{
	return PRIVATE()->PlaceholderText;
}

void UIText::setPlaceholderText(UIString const& value)
{
	PRIVATE()->PlaceholderText = value;
}

bool UIText::getReadOnly() const
{
	return PRIVATE()->ReadOnly;
}

void UIText::setReadOnly(bool value)
{
	PRIVATE()->ReadOnly = value;
}

bool UIText::getLineWrap() const
{
	return getStyle().Foreground.Font.LineWrap;
}

void UIText::setLineWrap(bool value)
{
	PRIVATE()->Style.Foreground.Font.LineWrap = value;
	if (getCanvas()) getCanvas()->layoutWidget();
}

bool UIText::getTabChangesFocus() const
{
	return PRIVATE()->TabChangesFocus;
}

void UIText::setTabChangesFocus(bool value)
{
	PRIVATE()->TabChangesFocus = value;
}

UIFontAligns UIText::getAlignment() const
{
	return PRIVATE()->Style.Foreground.Font.Align;
}

void UIText::setAlignment(UIFontAligns value)
{
	PRIVATE()->Style.Foreground.Font.Align = value;
}

int32_t UIText::getCursorPosition() const
{
	return PRIVATE()->PieceTable.cursor();
}

int32_t UIText::getCursorPosition(int32_t x, int32_t y) const
{
	auto painter = getCanvas()->getPainter();
	auto font = getStyle().Foreground.Font;
	TextBox box = textBox(PRIVATE(), getBounds());
	return textHit(painter, font, box, PRIVATE()->PieceTable.text(), (float)x, (float)y);
}

void UIText::setCursorPosition(int32_t value)
{
	auto previous = textState(PRIVATE());
	PRIVATE()->PreferredX = -1;
	PRIVATE()->PieceTable.moveCaret(value, false);
	textPublish(PRIVATE(), getCanvas(), previous, false);
}

UIString UIText::getSelectedText() const
{
	return PRIVATE()->PieceTable.selectedText();
}

bool UIText::hasSelectedText() const
{
	return PRIVATE()->PieceTable.hasSelection();
}

void UIText::getSelection(int32_t& start, int32_t& length) const
{
	PRIVATE()->PieceTable.selection(start, length);
}

int32_t UIText::getSelectionStart() const
{
	int32_t start = -1, length = 0;
	getSelection(start, length);
	return length > 0 ? start : -1;
}

int32_t UIText::getSelectionEnd() const
{
	int32_t start = -1, length = 0;
	getSelection(start, length);
	return length > 0 ? start + length : -1;
}

int32_t UIText::getSelectionLength() const
{
	return PRIVATE()->PieceTable.selectionLength();
}

void UIText::setSelection(int32_t start, int32_t length)
{
	auto previous = textState(PRIVATE());
	PRIVATE()->PreferredX = -1;
	PRIVATE()->PieceTable.setSelection(start, length);
	textPublish(PRIVATE(), getCanvas(), previous, false);
}

void UIText::selectAll()
{
	auto previous = textState(PRIVATE());
	PRIVATE()->PreferredX = -1;
	PRIVATE()->PieceTable.selectAll();
	textPublish(PRIVATE(), getCanvas(), previous, false);
}

UIString UIText::getText() const
{
	return PRIVATE()->PieceTable.text();
}

void UIText::setText(UIString const& value)
{
	UIString text = textNormalize(value);
	textTrimBytes(text, PRIVATE()->MaxLength > 0 ? (size_t)PRIVATE()->MaxLength : 0);
	auto previous = textState(PRIVATE());
	PRIVATE()->PreferredX = -1;
	if (PRIVATE()->PieceTable.setText(text) == false) return;
	textPublish(PRIVATE(), getCanvas(), previous, false);
}

void UIText::append(UIString const& text)
{
	UIString block = textNormalize(text);
	if (PRIVATE()->PieceTable.empty() == false) block.insert(0, 1, '\n');
	PRIVATE()->PieceTable.moveCaret((int32_t)PRIVATE()->PieceTable.size(), false);
	textCommit(PRIVATE(), getCanvas(), block, false);
}

int32_t UIText::getBlockCount() const
{
	int32_t count = 1;
	for (char value : PRIVATE()->PieceTable.text())
		if (value == '\n') ++count;
	return count;
}

bool UIText::isModified() const
{
	return PRIVATE()->PieceTable.isModified();
}

void UIText::setModified(bool value)
{
	PRIVATE()->PieceTable.setModified(value);
}

bool UIText::isUndoAvailable() const
{
	return PRIVATE()->PieceTable.canUndo();
}

bool UIText::isRedoAvailable() const
{
	return PRIVATE()->PieceTable.canRedo();
}

void UIText::backspace()
{
	if (PRIVATE()->ReadOnly)
	{
		PRIVATE()->OnInputRejected.signal();
		return;
	}
	auto previous = textState(PRIVATE());
	PRIVATE()->PreferredX = -1;
	if (PRIVATE()->PieceTable.backspace() == false) return;
	textPublish(PRIVATE(), getCanvas(), previous, true);
}

void UIText::del()
{
	if (PRIVATE()->ReadOnly)
	{
		PRIVATE()->OnInputRejected.signal();
		return;
	}
	auto previous = textState(PRIVATE());
	PRIVATE()->PreferredX = -1;
	if (PRIVATE()->PieceTable.del() == false) return;
	textPublish(PRIVATE(), getCanvas(), previous, true);
}

void UIText::deselect()
{
	auto previous = textState(PRIVATE());
	PRIVATE()->PieceTable.deselect();
	textPublish(PRIVATE(), getCanvas(), previous, false);
}

void UIText::insert(UIString const& text)
{
	textCommit(PRIVATE(), getCanvas(), text, true);
}

void UIText::cursorBackward(bool mark, int32_t steps)
{
	auto previous = textState(PRIVATE());
	PRIVATE()->PreferredX = -1;
	PRIVATE()->PieceTable.moveCodePoints(-steps, mark);
	textPublish(PRIVATE(), getCanvas(), previous, false);
}

void UIText::cursorForward(bool mark, int32_t steps)
{
	auto previous = textState(PRIVATE());
	PRIVATE()->PreferredX = -1;
	PRIVATE()->PieceTable.moveCodePoints(steps, mark);
	textPublish(PRIVATE(), getCanvas(), previous, false);
}

void UIText::cursorWordBackward(bool mark)
{
	auto previous = textState(PRIVATE());
	PRIVATE()->PreferredX = -1;
	PRIVATE()->PieceTable.moveWords(-1, mark);
	textPublish(PRIVATE(), getCanvas(), previous, false);
}

void UIText::cursorWordForward(bool mark)
{
	auto previous = textState(PRIVATE());
	PRIVATE()->PreferredX = -1;
	PRIVATE()->PieceTable.moveWords(1, mark);
	textPublish(PRIVATE(), getCanvas(), previous, false);
}

void UIText::cursorUp(bool mark)
{
	if (getCanvas() == nullptr || getCanvas()->getPainter() == nullptr) return;
	auto previous = textState(PRIVATE());
	auto font = getStyle().Foreground.Font;
	textMoveVertical(PRIVATE(), getCanvas()->getPainter(), font, textBox(PRIVATE(), getBounds()), -1, 1.0f, mark);
	textPublish(PRIVATE(), getCanvas(), previous, false);
}

void UIText::cursorDown(bool mark)
{
	if (getCanvas() == nullptr || getCanvas()->getPainter() == nullptr) return;
	auto previous = textState(PRIVATE());
	auto font = getStyle().Foreground.Font;
	textMoveVertical(PRIVATE(), getCanvas()->getPainter(), font, textBox(PRIVATE(), getBounds()), 1, 1.0f, mark);
	textPublish(PRIVATE(), getCanvas(), previous, false);
}

void UIText::home(bool mark)
{
	if (getCanvas() == nullptr || getCanvas()->getPainter() == nullptr) return;
	auto previous = textState(PRIVATE());
	PRIVATE()->PreferredX = -1;
	auto font = getStyle().Foreground.Font;
	TextBox box = textBox(PRIVATE(), getBounds());
	UIRect caret = textCaretAt(getCanvas()->getPainter(), font, box, PRIVATE()->PieceTable.text(), PRIVATE()->PieceTable.cursor());
	int32_t index = textHit(getCanvas()->getPainter(), font, box, PRIVATE()->PieceTable.text(), box.OriginX, caret.Y + caret.H * 0.5f);
	PRIVATE()->PieceTable.moveCaret(index, mark);
	textPublish(PRIVATE(), getCanvas(), previous, false);
}

void UIText::end(bool mark)
{
	if (getCanvas() == nullptr || getCanvas()->getPainter() == nullptr) return;
	auto previous = textState(PRIVATE());
	PRIVATE()->PreferredX = -1;
	auto font = getStyle().Foreground.Font;
	TextBox box = textBox(PRIVATE(), getBounds());
	UIRect caret = textCaretAt(getCanvas()->getPainter(), font, box, PRIVATE()->PieceTable.text(), PRIVATE()->PieceTable.cursor());
	int32_t index = textHit(getCanvas()->getPainter(), font, box, PRIVATE()->PieceTable.text(), box.OriginX + box.W - 1.0f, caret.Y + caret.H * 0.5f);
	PRIVATE()->PieceTable.moveCaret(index, mark);
	textPublish(PRIVATE(), getCanvas(), previous, false);
}

void UIText::clear()
{
	setText(UIString());
}

UIString UIText::copy() const
{
	return PRIVATE()->PieceTable.selectedText();
}

void UIText::cut()
{
	if (PRIVATE()->ReadOnly)
	{
		PRIVATE()->OnInputRejected.signal();
		return;
	}
	UIString text = PRIVATE()->PieceTable.selectedText();
	if (text.empty()) return;
	PRIVATE()->OnTextCopied.signal(text);
	auto previous = textState(PRIVATE());
	PRIVATE()->PreferredX = -1;
	if (PRIVATE()->PieceTable.removeSelection() == false) return;
	textPublish(PRIVATE(), getCanvas(), previous, true);
}

void UIText::paste()
{
	UIString text;
	PRIVATE()->OnTextPasted.signal(text);
	paste(text);
}

void UIText::paste(UIString const& text)
{
	textCommit(PRIVATE(), getCanvas(), text, false);
}

void UIText::redo()
{
	auto previous = textState(PRIVATE());
	PRIVATE()->PreferredX = -1;
	if (PRIVATE()->PieceTable.redo() == false) return;
	textPublish(PRIVATE(), getCanvas(), previous, true);
}

void UIText::undo()
{
	auto previous = textState(PRIVATE());
	PRIVATE()->PreferredX = -1;
	if (PRIVATE()->PieceTable.undo() == false) return;
	textPublish(PRIVATE(), getCanvas(), previous, true);
}

void UIText::keyPressEvent(UIKeyEventRaw event)
{
	if (event == nullptr) return;
	bool shift = (event->Modifiers & UIInputEnum::KEY_MOD_SHIFT) != 0;
	bool ctrl = (event->Modifiers & UIInputEnum::KEY_MOD_CTRL) != 0;
	if (event->Key == UIInputEnum::KEY_ENTER)
	{
		insert("\n");
		event->Accept = true;
		return;
	}
	if (event->Key == UIInputEnum::KEY_TAB)
	{
		if (PRIVATE()->TabChangesFocus) return;
		insert("\t");
		event->Accept = true;
		return;
	}
	if (event->Key == UIInputEnum::KEY_BACKSPACE)
	{
		if (ctrl)
		{
			if (PRIVATE()->ReadOnly) PRIVATE()->OnInputRejected.signal();
			else
			{
				auto previous = textState(PRIVATE());
				PRIVATE()->PreferredX = -1;
				if (PRIVATE()->PieceTable.deletePreviousWord()) textPublish(PRIVATE(), getCanvas(), previous, true);
			}
		}
		else backspace();
		event->Accept = true;
	}
	else if (event->Key == UIInputEnum::KEY_DELETE)
	{
		if (ctrl)
		{
			if (PRIVATE()->ReadOnly) PRIVATE()->OnInputRejected.signal();
			else
			{
				auto previous = textState(PRIVATE());
				PRIVATE()->PreferredX = -1;
				if (PRIVATE()->PieceTable.deleteNextWord()) textPublish(PRIVATE(), getCanvas(), previous, true);
			}
		}
		else del();
		event->Accept = true;
	}
	else if (event->Key == UIInputEnum::KEY_LEFT)
	{
		if (ctrl) cursorWordBackward(shift);
		else cursorBackward(shift, 1);
		event->Accept = true;
	}
	else if (event->Key == UIInputEnum::KEY_RIGHT)
	{
		if (ctrl) cursorWordForward(shift);
		else cursorForward(shift, 1);
		event->Accept = true;
	}
	else if (event->Key == UIInputEnum::KEY_UP)
	{
		if (ctrl)
		{
			auto previous = textState(PRIVATE());
			PRIVATE()->PreferredX = -1;
			int32_t cursor = PRIVATE()->PieceTable.cursor();
			int32_t start = textBlockStart(PRIVATE()->PieceTable.text(), cursor);
			int32_t index = start == 0 ? 0 : textBlockStart(PRIVATE()->PieceTable.text(), start - 1);
			PRIVATE()->PieceTable.moveCaret(index, shift);
			textPublish(PRIVATE(), getCanvas(), previous, false);
		}
		else cursorUp(shift);
		event->Accept = true;
	}
	else if (event->Key == UIInputEnum::KEY_DOWN)
	{
		if (ctrl)
		{
			auto previous = textState(PRIVATE());
			PRIVATE()->PreferredX = -1;
			int32_t index = textNextBlock(PRIVATE()->PieceTable.text(), PRIVATE()->PieceTable.cursor());
			PRIVATE()->PieceTable.moveCaret(index, shift);
			textPublish(PRIVATE(), getCanvas(), previous, false);
		}
		else cursorDown(shift);
		event->Accept = true;
	}
	else if (event->Key == UIInputEnum::KEY_PAGE_UP || event->Key == UIInputEnum::KEY_PAGE_DOWN)
	{
		if (getCanvas() && getCanvas()->getPainter())
		{
			auto previous = textState(PRIVATE());
			auto font = getStyle().Foreground.Font;
			TextBox box = textBox(PRIVATE(), getBounds());
			int32_t direction = event->Key == UIInputEnum::KEY_PAGE_UP ? -1 : 1;
			textMoveVertical(PRIVATE(), getCanvas()->getPainter(), font, box, direction, box.H, shift);
			textPublish(PRIVATE(), getCanvas(), previous, false);
		}
		event->Accept = true;
	}
	else if (event->Key == UIInputEnum::KEY_HOME)
	{
		if (ctrl)
		{
			auto previous = textState(PRIVATE());
			PRIVATE()->PreferredX = -1;
			PRIVATE()->PieceTable.moveCaret(0, shift);
			textPublish(PRIVATE(), getCanvas(), previous, false);
		}
		else home(shift);
		event->Accept = true;
	}
	else if (event->Key == UIInputEnum::KEY_END)
	{
		if (ctrl)
		{
			auto previous = textState(PRIVATE());
			PRIVATE()->PreferredX = -1;
			PRIVATE()->PieceTable.moveCaret((int32_t)PRIVATE()->PieceTable.size(), shift);
			textPublish(PRIVATE(), getCanvas(), previous, false);
		}
		else end(shift);
		event->Accept = true;
	}
	else if (event->Key == UIInputEnum::KEY_A && ctrl)
	{
		selectAll();
		event->Accept = true;
	}
	else if (event->Key == UIInputEnum::KEY_C && ctrl)
	{
		auto text = copy();
		if (text.empty() == false) PRIVATE()->OnTextCopied.signal(text);
		event->Accept = true;
	}
	else if (event->Key == UIInputEnum::KEY_X && ctrl)
	{
		cut();
		event->Accept = true;
	}
	else if (event->Key == UIInputEnum::KEY_V && ctrl)
	{
		paste();
		event->Accept = true;
	}
	else if (event->Key == UIInputEnum::KEY_Z && ctrl)
	{
		if (shift) redo();
		else undo();
		event->Accept = true;
	}
	else if (event->Key == UIInputEnum::KEY_Y && ctrl)
	{
		redo();
		event->Accept = true;
	}
}

void UIText::inputEvent(UITextInputEventRaw event)
{
	if (event == nullptr || event->Done == false) return;
	if (getCanvas()->getFocus() != this) return;
	if (PRIVATE()->ReadOnly)
	{
		PRIVATE()->OnInputRejected.signal();
		return;
	}
	insert(event->Text);
	TextBox box = textBox(PRIVATE(), getBounds());
	UIRect caret = textCaretAt(getCanvas()->getPainter(), getStyle().Foreground.Font, box, PRIVATE()->PieceTable.text(), PRIVATE()->PieceTable.cursor());
	PRIVATE()->OnEditingStarted.signal(UIOverlap(getViewport(), caret));
}

void UIText::wheelEvent(UIMouseWheelEventRaw event)
{
	if (event == nullptr || inBounds(event->X, event->Y) == false) return;
	float lines = event->PixelY != 0 ? (float)event->PixelY : (float)event->AngleY;
	float columns = event->PixelX != 0 ? (float)event->PixelX : (float)event->AngleX;
	if (lines == 0 && columns == 0) return;
	float step = std::max(12.0f, (float)getStyle().Foreground.Font.Size);
	PRIVATE()->ScrollY += lines * step;
	PRIVATE()->ScrollX += columns * step;
	if (getCanvas()) getCanvas()->layoutWidget();
	event->Accept = true;
}

void UIText::mouseDoubleEvent(UIMouseEventRaw event)
{
	mousePressEvent(event);
	if (PRIVATE()->MousePress == false) return;
	auto previous = textState(PRIVATE());
	PRIVATE()->PieceTable.selectWord();
	textPublish(PRIVATE(), getCanvas(), previous, false);
}

void UIText::mousePressEvent(UIMouseEventRaw event)
{
	if (UIBounds(UIOverlap(getViewport(), getBounds()), event->X, event->Y))
	{
		if (event->Button == UIInputEnum::MOUSE_BUTTON_LEFT)
		{
			auto painter = getCanvas()->getPainter();
			auto font = getStyle().Foreground.Font;
			TextBox box = textBox(PRIVATE(), getBounds());
			int32_t cursor = textHit(painter, font, box, PRIVATE()->PieceTable.text(), (float)event->X, (float)event->Y);
			auto previous = textState(PRIVATE());
			PRIVATE()->PreferredX = -1;
			PRIVATE()->PieceTable.beginDrag(cursor);
			PRIVATE()->MousePress = true;
			getCanvas()->setFocus(this);
			if (getCanvas()->getDevice()) getCanvas()->getDevice()->setKeyboard(true);
			textPublish(PRIVATE(), getCanvas(), previous, false);
			UIRect caret = textCaretAt(painter, font, textBox(PRIVATE(), getBounds()), PRIVATE()->PieceTable.text(), PRIVATE()->PieceTable.cursor());
			PRIVATE()->OnEditingStarted.signal(UIOverlap(getViewport(), caret));
		}
	}
	else
	{
		if (getCanvas()->getFocus() == this && getCanvas()->getDevice())
		{
			getCanvas()->getDevice()->setKeyboard(false);
			getCanvas()->setFocus(nullptr);
		}
	}
}

void UIText::mouseReleaseEvent(UIMouseEventRaw event)
{
	if (event->Button == UIInputEnum::MOUSE_BUTTON_LEFT)
	{
		PRIVATE()->MousePress = false;
		if (PRIVATE()->PieceTable.anchor() == PRIVATE()->PieceTable.cursor())
			PRIVATE()->PieceTable.deselect();
	}
}

void UIText::mouseMoveEvent(UIMouseEventRaw event)
{
	if (event->Button != UIInputEnum::MOUSE_BUTTON_LEFT || PRIVATE()->MousePress == false) return;
	auto viewport = UIOverlap(getViewport(), getBounds());
	if (event->Y < viewport.Y) PRIVATE()->ScrollY += 8;
	if (event->Y > viewport.Y + viewport.H) PRIVATE()->ScrollY -= 8;
	if (event->X < viewport.X) PRIVATE()->ScrollX += 8;
	if (event->X > viewport.X + viewport.W) PRIVATE()->ScrollX -= 8;
	auto painter = getCanvas()->getPainter();
	auto font = getStyle().Foreground.Font;
	TextBox box = textBox(PRIVATE(), getBounds());
	int32_t cursor = textHit(painter, font, box, PRIVATE()->PieceTable.text(), (float)event->X, (float)event->Y);
	auto previous = textState(PRIVATE());
	PRIVATE()->PieceTable.moveCaret(cursor, true);
	textPublish(PRIVATE(), getCanvas(), previous, false);
}

void UIText::focusInEvent(UIFocusEventRaw event)
{
	if (getCanvas()->getFocus() == this && getCanvas()->getDevice()) getCanvas()->getDevice()->setKeyboard(true);
}

void UIText::focusOutEvent(UIFocusEventRaw event)
{
	PRIVATE()->PieceTable.endUndoGroup();
	if (getCanvas()->getFocus() == this && getCanvas()->getDevice()) getCanvas()->getDevice()->setKeyboard(false);
	PRIVATE()->OnEditingFinished.signal();
}
#undef PRIVATE
