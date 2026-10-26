#pragma once
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
#include "../UIWidget.h"
#include "../UIFactory.h"

/// @brief
struct UITextStyle
{
	struct
	{
		UIPen Pen{ .Style = UIPen::NoPen, };
		UIFont Font{ .LineWrap = true, .Align = UIFont::AlignLeft | UIFont::AlignTop, .Ellipsize = UIFont::EllipsizeNone, };
		UIBrush Brush = UICtrlInkBrush;
	} Foreground;
	struct
	{
		UIPen Pen = UICtrlLinePen;
		UIBrush Brush = UICtrlPaperBrush;
	} Background;
};

/// @brief Multi-line plain text input.
class OPENUI_API UIText : public UIWidget
{
public:
	explicit UIText(UICanvasRaw canvas);
	~UIText() override;
	void arrange(UIRect client) override;
	void layout(UIRect client) override;
	void paint(UIRect client, UIPainterRaw painter) override;

	UITextStyle getStyle() const;
	void setStyle(UITextStyle value);

	int32_t getMaxLength() const;
	void setMaxLength(int32_t value);

	UIString getPlaceholderText() const;
	void setPlaceholderText(UIString const& value);

	bool getReadOnly() const;
	void setReadOnly(bool value);

	bool getLineWrap() const;
	void setLineWrap(bool value);

	bool getTabChangesFocus() const;
	void setTabChangesFocus(bool value);

	UIFontAligns getAlignment() const;
	void setAlignment(UIFontAligns value);

	int32_t getCursorPosition() const;
	int32_t getCursorPosition(int32_t x, int32_t y) const;
	void setCursorPosition(int32_t value);

	UIString getSelectedText() const;
	bool hasSelectedText() const;
	void getSelection(int32_t& start, int32_t& length) const;
	int32_t getSelectionStart() const;
	int32_t getSelectionEnd() const;
	int32_t getSelectionLength() const;
	void setSelection(int32_t start, int32_t length);
	void selectAll();

	UIString getText() const;
	void setText(UIString const& value);
	void append(UIString const& text);
	int32_t getBlockCount() const;

	bool isModified() const;
	void setModified(bool value);
	bool isUndoAvailable() const;
	bool isRedoAvailable() const;

	void backspace();
	void del();
	void deselect();
	void insert(UIString const& text);
	void cursorBackward(bool mark, int32_t steps = 1);
	void cursorForward(bool mark, int32_t steps = 1);
	void cursorWordBackward(bool mark);
	void cursorWordForward(bool mark);
	void cursorUp(bool mark);
	void cursorDown(bool mark);
	void home(bool mark);
	void end(bool mark);

	void clear();
	UIString copy() const;
	void cut();
	void paste();
	void paste(UIString const& text);
	void redo();
	void undo();

protected:
	void keyPressEvent(UIKeyEventRaw event) override;
	void inputEvent(UITextInputEventRaw event) override;
	void wheelEvent(UIMouseWheelEventRaw event) override;
	void mouseDoubleEvent(UIMouseEventRaw event) override;
	void mousePressEvent(UIMouseEventRaw event) override;
	void mouseReleaseEvent(UIMouseEventRaw event) override;
	void mouseMoveEvent(UIMouseEventRaw event) override;
	void focusInEvent(UIFocusEventRaw event) override;
	void focusOutEvent(UIFocusEventRaw event) override;

public:
	UISignalAsRaw<int32_t /*oldPos*/, int32_t /*newPos*/> cursorPositionChanged;
	UISignalAsRaw<UIRect /*area*/> editingStarted;
	UISignalAsRaw<> editingFinished;
	UISignalAsRaw<> inputRejected;
	UISignalAsRaw<> selectionChanged;
	UISignalAsRaw<UIString const& /*text*/> textChanged;
	UISignalAsRaw<UIString const& /*text*/> textEdited;
	UISignalAsRaw<UIString& /*text*/> textPasted;
	UISignalAsRaw<UIString const& /*text*/> textCopied;

private:
	UIPrivateRaw m_Private;
};
