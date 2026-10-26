/*=================================================
* Copyright © 2020-2025 ChivenZhang.
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
#include "../Private/UIPieceTable.h"
#include <algorithm>

#include "OpenUI/UIDevice.h"

struct UIInputPrivate : UIPrivate
{
	UIInputStyle Style;
	UIRef<UIPieceTable> PieceTable;
	UIRect Selection;
	bool ReadOnly = false, MousePress = false;
	UIString Text, SelectedText, PlaceholderText;
	UIInput::echo_t EchoMode = UIInput::EchoModeNormal;
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
#define PRIVATE() ((UIInputPrivate*) m_Private)

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
	int32_t Cursor = 0;
	int32_t CursorStart = -1;
};

static bool inputMasked(UIInput::echo_t mode, bool focused)
{
	if (mode == UIInput::EchoModePassword) return true;
	if (mode == UIInput::EchoModePasswordOnEdit) return focused == false;
	return false;
}

static UIInputDisplay inputDisplay(UIInputPrivate* data, bool focused)
{
	UIInputDisplay shown;
	if (data->EchoMode == UIInput::EchoModeNoEcho) return shown;
	if (inputMasked(data->EchoMode, focused))
	{
		shown.Text = inputMask(data->Text);
		shown.Cursor = inputCodePoints(data->Text, data->Cursor);
		shown.CursorStart = data->CursorStart < 0 ? -1 : inputCodePoints(data->Text, data->CursorStart);
		return shown;
	}
	shown.Text = data->Text;
	shown.Cursor = data->Cursor;
	shown.CursorStart = data->CursorStart;
	return shown;
}

static int32_t inputCursorFromDisplay(UIInputPrivate* data, bool focused, int32_t displayCursor)
{
	if (data->EchoMode == UIInput::EchoModeNoEcho) return data->Cursor;
	if (inputMasked(data->EchoMode, focused)) return inputByteIndex(data->Text, displayCursor);
	return std::clamp<int32_t>(displayCursor, 0, (int32_t)data->Text.size());
}

static void inputNotifyText(UIInputPrivate* data, UIString const& previous, bool edited)
{
	if (previous == data->Text) return;
	data->OnTextChanged.signal(data->Text);
	if (edited) data->OnTextEdited.signal(data->Text);
}

static void inputNotifyCursor(UIInputPrivate* data, int32_t previous)
{
	if (previous == data->Cursor) return;
	data->OnCursorPositionChanged.signal(previous, data->Cursor);
}

UIInput::UIInput(UICanvasRaw canvas)
	:
	UIWidget(canvas)
{
	m_Private = new UIInputPrivate;
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
	UIRect cursorRect;
	auto painter = getCanvas()->getPainter();
	bool focused = getCanvas() && getCanvas()->getFocus() == this;
	auto shown = inputDisplay(PRIVATE(), focused);
	painter->setFont(getStyle().Foreground.Font);
	painter->boundingRect(PRIVATE()->SelectOffset + getBounds().X + 8, getBounds().Y, std::max(0.0f, getBounds().W - 16), getBounds().H, shown.Text, shown.Cursor, &cursorRect);

	if (shown.CursorStart != -1 && shown.CursorStart != shown.Cursor)
	{
		UIRect cursorStartRect;
		painter->boundingRect(PRIVATE()->SelectOffset + getBounds().X + 8, getBounds().Y, std::max(0.0f, getBounds().W - 16), getBounds().H, shown.Text, shown.CursorStart, &cursorStartRect);

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
	UIWidget::paint(client, painter);
	bool focused = getCanvas() && getCanvas()->getFocus() == this;
	auto& widgetStyle = getEnable() == false ? getStyleDisabled()
		: focused ? getStyleActive()
		: getStyleNormal();
	auto face = UIFaceOf(widgetStyle, client);
	UIDrawFace(painter, client, face);

	auto style = getStyle();
	painter->setFont(style.Foreground.Font);
	UIRect field{ PRIVATE()->SelectOffset + client.X, client.Y, std::max(0.0f, client.W - 16), client.H };
	if (PRIVATE()->Text.empty() && focused == false)
	{
		painter->setPen(UINoPen);
		painter->setBrush(UICtrlMutedBrush);
		painter->drawText(field.X + 8, field.Y, field.W, field.H, PRIVATE()->PlaceholderText);
		return;
	}

	if (PRIVATE()->Selection.W != 0 && PRIVATE()->Selection.H != 0)
	{
		painter->setPen(UINoPen);
		painter->setBrush(UICtrlPressBrush);
		painter->drawRect(PRIVATE()->Selection.X, PRIVATE()->Selection.Y + 4, PRIVATE()->Selection.W, std::max(0.0f, PRIVATE()->Selection.H - 8));
	}

	auto shown = inputDisplay(PRIVATE(), focused);
	painter->setPen(UINoPen);
	painter->setBrush(face.Text.Style == UIBrush::NoBrush ? style.Foreground.Brush : face.Text);
	UIRect cursorRect;
	painter->drawText(field.X + 8, field.Y, field.W, field.H, shown.Text, nullptr, shown.Cursor, &cursorRect);

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
	int32_t cursor = -1;
	auto painter = getCanvas()->getPainter();
	bool focused = getCanvas() && getCanvas()->getFocus() == this;
	auto shown = inputDisplay(PRIVATE(), focused);
	painter->setFont(getStyle().Foreground.Font);
	int32_t displayCursor = -1;
	painter->boundingRect(PRIVATE()->SelectOffset + getBounds().X + 8, getBounds().Y, std::max(0.0f, getBounds().W - 16), getBounds().H, shown.Text, x, y, &displayCursor);
	return inputCursorFromDisplay(PRIVATE(), focused, displayCursor);
}

void UIInput::setCursorPosition(int32_t value)
{
	auto previous = PRIVATE()->Cursor;
	PRIVATE()->Cursor = std::clamp<int32_t>(value, 0, PRIVATE()->Text.length());
	inputNotifyCursor(PRIVATE(), previous);
	if (getCanvas()) getCanvas()->layoutWidget();
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
	auto painter = getCanvas()->getPainter();
	PRIVATE()->CursorStart = std::clamp<int32_t>(start, 0, PRIVATE()->Text.size());
	PRIVATE()->Cursor = std::clamp<int32_t>(start + std::max<int32_t>(0, length), 0, PRIVATE()->Text.size());

	if (getCanvas()) getCanvas()->layoutWidget();
}

UIString UIInput::getText() const
{
	return PRIVATE()->Text;
}

void UIInput::setText(UIString const& value)
{
	auto previous = PRIVATE()->Text;
	auto previousCursor = PRIVATE()->Cursor;
	PRIVATE()->Cursor = 0;
	PRIVATE()->CursorStart = -1;
	PRIVATE()->PieceTable = UINew<UIPieceTable>(value);
	PRIVATE()->Text = PRIVATE()->PieceTable->getText();
	PRIVATE()->SelectedText = UIString();
	inputNotifyText(PRIVATE(), previous, false);
	inputNotifyCursor(PRIVATE(), previousCursor);
}

void UIInput::backspace()
{
	if (PRIVATE()->ReadOnly)
	{
		PRIVATE()->OnInputRejected.signal();
		return;
	}
	auto previous = PRIVATE()->Text;
	auto previousCursor = PRIVATE()->Cursor;
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
	inputNotifyText(PRIVATE(), previous, true);
	inputNotifyCursor(PRIVATE(), previousCursor);
}

void UIInput::del()
{
	if (PRIVATE()->ReadOnly)
	{
		PRIVATE()->OnInputRejected.signal();
		return;
	}
	auto previous = PRIVATE()->Text;
	auto previousCursor = PRIVATE()->Cursor;
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
	inputNotifyText(PRIVATE(), previous, true);
	inputNotifyCursor(PRIVATE(), previousCursor);
}

void UIInput::deselect()
{
	PRIVATE()->CursorStart = -1;

	if (getCanvas()) getCanvas()->layoutWidget();
}

void UIInput::insert(UIString const& text)
{
	if (PRIVATE()->ReadOnly || PRIVATE()->MaxLength <= PRIVATE()->Text.size())
	{
		PRIVATE()->OnInputRejected.signal();
		return;
	}
	auto previous = PRIVATE()->Text;
	auto previousCursor = PRIVATE()->Cursor;
	PRIVATE()->PieceTable->insert(PRIVATE()->Cursor, text);
	PRIVATE()->Cursor += text.length();
	PRIVATE()->Text = PRIVATE()->PieceTable->getText();
	PRIVATE()->Cursor = std::clamp<int32_t>(PRIVATE()->Cursor, 0, PRIVATE()->Text.length());
	inputNotifyText(PRIVATE(), previous, true);
	inputNotifyCursor(PRIVATE(), previousCursor);
}

void UIInput::clear()
{
	auto previous = PRIVATE()->Text;
	auto previousCursor = PRIVATE()->Cursor;
	PRIVATE()->Cursor = 0;
	PRIVATE()->PieceTable = UINew<UIPieceTable>();
	PRIVATE()->Text = PRIVATE()->PieceTable->getText();
	deselect();
	inputNotifyText(PRIVATE(), previous, false);
	inputNotifyCursor(PRIVATE(), previousCursor);
}

UIString UIInput::copy()
{
	return getSelectedText();
}

void UIInput::cut()
{
	if (PRIVATE()->ReadOnly)
	{
		PRIVATE()->OnInputRejected.signal();
		return;
	}
	auto previous = PRIVATE()->Text;
	auto previousCursor = PRIVATE()->Cursor;
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
		inputNotifyText(PRIVATE(), previous, true);
		inputNotifyCursor(PRIVATE(), previousCursor);
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
	if (event->Key == UIInputEnum::KEY_ENTER)
	{
		if (getCanvas() && getCanvas()->getFocus() == this)
		{
			PRIVATE()->OnReturnPressed.signal();
			PRIVATE()->OnEditingFinished.signal();
			event->Accept = true;
		}
		return;
	}
	auto previousCursor = PRIVATE()->Cursor;
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
		inputNotifyCursor(PRIVATE(), previousCursor);
	}
	// Right
	if (event->Key == UIInputEnum::KEY_RIGHT)
	{
		auto bytes = ::UIUTF8Num(PRIVATE()->Text[PRIVATE()->Cursor]);
		PRIVATE()->Cursor = PRIVATE()->Cursor + bytes;
		PRIVATE()->Cursor = std::clamp<int32_t>(PRIVATE()->Cursor, 0, PRIVATE()->Text.length());
		deselect();
		inputNotifyCursor(PRIVATE(), previousCursor);
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
}

void UIInput::inputEvent(UITextInputEventRaw event)
{
	if (getCanvas()->getFocus() == this)
	{
		if (event->Done)
		{
			if (PRIVATE()->ReadOnly)
			{
				PRIVATE()->OnInputRejected.signal();
				return;
			}

			cut();
			insert(event->Text);
			deselect();
			auto shown = inputDisplay(PRIVATE(), true);
			UIRect cursorRect;
			auto painter = getCanvas()->getPainter();
			painter->setFont(getStyle().Foreground.Font);
			painter->boundingRect(PRIVATE()->SelectOffset + getBounds().X + 8, getBounds().Y, std::max(0.0f, getBounds().W - 16), getBounds().H, shown.Text, shown.Cursor, &cursorRect);
			PRIVATE()->OnEditingStarted.signal(UIOverlap(getViewport(), cursorRect));
		}
	}
}

void UIInput::mouseDoubleEvent(UIMouseEventRaw event)
{
	mousePressEvent(event);
}

void UIInput::mousePressEvent(UIMouseEventRaw event)
{
	if (UIBounds(UIOverlap(getViewport(), getBounds()), event->X, event->Y))
	{
		if (event->Button == UIInputEnum::MOUSE_BUTTON_LEFT)
		{
			UIRect cursorRect;
			auto painter = getCanvas()->getPainter();
			bool focused = true;
			auto shown = inputDisplay(PRIVATE(), focused);
			auto previousCursor = PRIVATE()->Cursor;
			int32_t displayCursor = PRIVATE()->Cursor;
			painter->setFont(getStyle().Foreground.Font);
			painter->boundingRect(PRIVATE()->SelectOffset + getBounds().X + 8, getBounds().Y, std::max(0.0f, getBounds().W - 16), getBounds().H, shown.Text, event->X, event->Y, &displayCursor, &cursorRect);
			PRIVATE()->Cursor = inputCursorFromDisplay(PRIVATE(), focused, displayCursor);
			inputNotifyCursor(PRIVATE(), previousCursor);
			PRIVATE()->CursorStart = PRIVATE()->Cursor;
			PRIVATE()->MousePress = true;

			getCanvas()->setFocus(this);
			if (getCanvas()->getDevice()) getCanvas()->getDevice()->setKeyboard(true);

			PRIVATE()->OnEditingStarted.signal(UIOverlap(getViewport(), cursorRect));
		}
	}
	else
	{
		if (getCanvas()->getFocus() == this && getCanvas()->getDevice()) getCanvas()->getDevice()->setKeyboard(false);

		getCanvas()->setFocus(nullptr);
	}
}

void UIInput::mouseReleaseEvent(UIMouseEventRaw event)
{
	if (event->Button == UIInputEnum::MOUSE_BUTTON_LEFT)
	{
		PRIVATE()->MousePress = false;
		if (PRIVATE()->CursorStart == PRIVATE()->Cursor)
		{
			PRIVATE()->CursorStart = -1;
			PRIVATE()->Selection = {};
		}
	}
}

void UIInput::mouseMoveEvent(UIMouseEventRaw event)
{
	if (event->Button == UIInputEnum::MOUSE_BUTTON_LEFT)
	{
		if (PRIVATE()->MousePress)
		{
			auto viewport = UIOverlap(getViewport(), getBounds());
			auto painter = getCanvas()->getPainter();
			bool focused = getCanvas() && getCanvas()->getFocus() == this;
			auto shown = inputDisplay(PRIVATE(), focused);
			auto previousCursor = PRIVATE()->Cursor;
			int32_t displayCursor = 0;
			painter->setFont(getStyle().Foreground.Font);
			auto boundRect = painter->boundingRect(PRIVATE()->SelectOffset + getBounds().X + 8, getBounds().Y, std::max(0.0f, getBounds().W - 16), getBounds().H, shown.Text, event->X, event->Y, &displayCursor);
			PRIVATE()->Cursor = inputCursorFromDisplay(PRIVATE(), focused, displayCursor);
			inputNotifyCursor(PRIVATE(), previousCursor);
			if (event->X < viewport.X && boundRect.X < viewport.X) PRIVATE()->SelectOffset += 1;
			if (event->X > (viewport.X + viewport.W) && boundRect.X + boundRect.W > (viewport.X + viewport.W)) PRIVATE()->SelectOffset -= 1;

			if (getCanvas()) getCanvas()->layoutWidget();
		}
	}
}

void UIInput::focusInEvent(UIFocusEventRaw event)
{
	if (getCanvas()->getFocus() == this && getCanvas()->getDevice()) getCanvas()->getDevice()->setKeyboard(true);
}

void UIInput::focusOutEvent(UIFocusEventRaw event)
{
	if (getCanvas()->getFocus() == this && getCanvas()->getDevice()) getCanvas()->getDevice()->setKeyboard(false);

	PRIVATE()->OnEditingFinished.signal();
}
