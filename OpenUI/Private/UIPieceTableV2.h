/*=================================================
* Copyright © 2020-2026 ChivenZhang.
* All Rights Reserved.
* =====================Note=========================
*
* Piece table text buffer. original keeps the initial text, added keeps every
* insertion, and pieces are the live document. Undo stores the replaced span.
*
* ====================History=======================
* Created by ChivenZhang@gmail.com.
*
* =================================================*/
#pragma once
#include "../UI.h"

/// @brief Piece-table buffer with caret, selection, and undo/redo.
class UIPieceTableV2
{
public:
	UIPieceTableV2();
	explicit UIPieceTableV2(UIString const& text);

	UIString const& text() const;
	size_t size() const;
	bool empty() const;
	UIString textAt(size_t position, size_t length) const;

	int32_t cursor() const;
	int32_t anchor() const;
	bool hasSelection() const;
	void selection(int32_t& start, int32_t& length) const;
	int32_t selectionLength() const;
	UIString selectedText() const;

	void moveCaret(int32_t position, bool mark);
	void beginDrag(int32_t position);
	void setSelection(int32_t start, int32_t length);
	void selectAll();
	void selectWord();
	void deselect();
	void moveCodePoints(int32_t steps, bool mark);
	void moveWords(int32_t steps, bool mark);
	void moveHome(bool mark);
	void moveEnd(bool mark);

	bool insert(UIString const& text, bool merge);
	bool backspace();
	bool del();
	bool deletePreviousWord();
	bool deleteNextWord();
	bool removeSelection();

	bool setText(UIString const& text);
	bool clear();
	bool undo();
	bool redo();
	bool canUndo() const;
	bool canRedo() const;
	void endUndoGroup();

	bool isModified() const;
	void setModified(bool value);

private:
	enum class Buffer : uint8_t
	{
		Original,
		Add,
	};
	enum class Merge : uint8_t
	{
		None,
		Insert,
		Backspace,
		Delete,
	};
	struct Piece
	{
		Buffer buffer = Buffer::Original;
		size_t start = 0;
		size_t length = 0;
	};
	struct Edit
	{
		size_t Pos = 0;
		UIString Removed;
		UIString Inserted;
		int32_t CursorBefore = 0;
		int32_t AnchorBefore = -1;
		int32_t CursorAfter = 0;
		int32_t AnchorAfter = -1;
		Merge Kind = Merge::None;
	};

	int32_t limit() const;
	int32_t previous(int32_t index) const;
	int32_t next(int32_t index) const;
	bool isSpace(int32_t index) const;
	bool isWord(int32_t index) const;
	int32_t wordForward(int32_t index) const;
	int32_t wordBackward(int32_t index) const;
	void clampCaret();
	void reset(UIString const& text);
	void relink(size_t position, size_t removeLength, size_t addStart, size_t addLength);
	void splice(size_t position, UIString const& eraseText, UIString const& insertText);
	bool change(int32_t position, int32_t removeLength, UIString const& text, int32_t cursorAfter, Merge kind);
	void push(Edit edit);
	void rebuild();

	UIString original;
	UIString added;
	UIString document;
	UIList<Piece> pieces;
	UIList<Edit> undoStack;
	size_t undoIndex = 0;
	int32_t caret = 0;
	int32_t markAnchor = -1;
	bool seal = true;
	bool modified = false;
};
