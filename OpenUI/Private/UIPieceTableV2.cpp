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
#include "UIPieceTableV2.h"
#include <algorithm>
#include <cctype>
#include <climits>

UIPieceTableV2::UIPieceTableV2()
{
}

UIPieceTableV2::UIPieceTableV2(UIString const& text)
{
	reset(text);
}

UIString const& UIPieceTableV2::text() const
{
	return document;
}

size_t UIPieceTableV2::size() const
{
	return document.size();
}

bool UIPieceTableV2::empty() const
{
	return document.empty();
}

UIString UIPieceTableV2::textAt(size_t position, size_t length) const
{
	if (position >= document.size() || length == 0) return UIString();
	return document.substr(position, length);
}

int32_t UIPieceTableV2::cursor() const
{
	return caret;
}

int32_t UIPieceTableV2::anchor() const
{
	return markAnchor;
}

bool UIPieceTableV2::hasSelection() const
{
	return markAnchor >= 0 && markAnchor != caret;
}

void UIPieceTableV2::selection(int32_t& start, int32_t& length) const
{
	if (markAnchor < 0 || markAnchor == caret)
	{
		start = -1;
		length = 0;
		return;
	}
	start = std::min(markAnchor, caret);
	length = std::max(markAnchor, caret) - start;
}

int32_t UIPieceTableV2::selectionLength() const
{
	int32_t start = 0;
	int32_t length = 0;
	selection(start, length);
	return length;
}

UIString UIPieceTableV2::selectedText() const
{
	int32_t start = 0;
	int32_t length = 0;
	selection(start, length);
	if (length <= 0) return UIString();
	return document.substr((size_t)start, (size_t)length);
}

void UIPieceTableV2::moveCaret(int32_t position, bool mark)
{
	endUndoGroup();
	int32_t n = limit();
	if (position < 0) position = 0;
	if (position > n) position = n;
	if (!mark) markAnchor = -1;
	else if (markAnchor < 0) markAnchor = caret;
	caret = position;
}

void UIPieceTableV2::beginDrag(int32_t position)
{
	endUndoGroup();
	int32_t n = limit();
	if (position < 0) position = 0;
	if (position > n) position = n;
	caret = position;
	markAnchor = position;
}

void UIPieceTableV2::setSelection(int32_t start, int32_t length)
{
	endUndoGroup();
	int32_t n = limit();
	int64_t end = (int64_t)start + (int64_t)length;
	int32_t from = std::clamp(start, 0, n);
	int32_t to = (int32_t)std::clamp<int64_t>(end, (int64_t)0, (int64_t)n);
	if (length < 0) std::swap(from, to);
	caret = length < 0 ? from : to;
	markAnchor = from == to ? -1 : (length < 0 ? to : from);
}

void UIPieceTableV2::selectAll()
{
	endUndoGroup();
	int32_t n = limit();
	caret = n;
	markAnchor = n == 0 ? -1 : 0;
}

void UIPieceTableV2::selectWord()
{
	endUndoGroup();
	if (document.empty())
	{
		caret = 0;
		markAnchor = -1;
		return;
	}
	int32_t index = caret;
	if (index >= limit()) index = previous(index);
	bool space = isSpace(index);
	bool word = space == false && isWord(index);
	int32_t begin = index;
	while (begin > 0)
	{
		int32_t prev = previous(begin);
		if (isSpace(prev) != space) break;
		if (space == false && isWord(prev) != word) break;
		begin = prev;
	}
	int32_t end = next(index);
	while (end < limit())
	{
		if (isSpace(end) != space) break;
		if (space == false && isWord(end) != word) break;
		end = next(end);
	}
	markAnchor = begin == end ? -1 : begin;
	caret = end;
}

void UIPieceTableV2::deselect()
{
	endUndoGroup();
	markAnchor = -1;
}

void UIPieceTableV2::moveCodePoints(int32_t steps, bool mark)
{
	int32_t position = caret;
	if (steps > 0)
	{
		while (steps > 0 && position < limit())
		{
			position = next(position);
			--steps;
		}
	}
	else
	{
		while (steps < 0 && position > 0)
		{
			position = previous(position);
			++steps;
		}
	}
	moveCaret(position, mark);
}

void UIPieceTableV2::moveWords(int32_t steps, bool mark)
{
	int32_t position = caret;
	if (steps > 0)
	{
		while (steps > 0)
		{
			int32_t nextPos = wordForward(position);
			if (nextPos == position) break;
			position = nextPos;
			--steps;
		}
	}
	else
	{
		while (steps < 0)
		{
			int32_t nextPos = wordBackward(position);
			if (nextPos == position) break;
			position = nextPos;
			++steps;
		}
	}
	moveCaret(position, mark);
}

void UIPieceTableV2::moveHome(bool mark)
{
	moveCaret(0, mark);
}

void UIPieceTableV2::moveEnd(bool mark)
{
	moveCaret(limit(), mark);
}

bool UIPieceTableV2::insert(UIString const& text, bool merge)
{
	Merge kind = merge ? Merge::Insert : Merge::None;
	if (markAnchor >= 0 && markAnchor != caret)
	{
		int32_t start = std::min(markAnchor, caret);
		int32_t length = std::max(markAnchor, caret) - start;
		int32_t placed = start + (int32_t)std::min(text.size(), (size_t)INT32_MAX);
		return change(start, length, text, placed, kind);
	}
	if (text.empty()) return false;
	int32_t placed = caret + (int32_t)std::min(text.size(), (size_t)INT32_MAX);
	return change(caret, 0, text, placed, kind);
}

bool UIPieceTableV2::backspace()
{
	if (markAnchor >= 0 && markAnchor != caret)
	{
		int32_t start = std::min(markAnchor, caret);
		int32_t length = std::max(markAnchor, caret) - start;
		return change(start, length, UIString(), start, Merge::None);
	}
	if (caret <= 0) return false;
	int32_t prev = previous(caret);
	return change(prev, caret - prev, UIString(), prev, Merge::Backspace);
}

bool UIPieceTableV2::del()
{
	if (markAnchor >= 0 && markAnchor != caret)
	{
		int32_t start = std::min(markAnchor, caret);
		int32_t length = std::max(markAnchor, caret) - start;
		return change(start, length, UIString(), start, Merge::None);
	}
	if (caret >= limit()) return false;
	int32_t nextPos = next(caret);
	return change(caret, nextPos - caret, UIString(), caret, Merge::Delete);
}

bool UIPieceTableV2::deletePreviousWord()
{
	if (hasSelection()) return backspace();
	int32_t begin = wordBackward(caret);
	if (begin == caret) return false;
	return change(begin, caret - begin, UIString(), begin, Merge::None);
}

bool UIPieceTableV2::deleteNextWord()
{
	if (hasSelection()) return del();
	int32_t end = wordForward(caret);
	if (end == caret) return false;
	return change(caret, end - caret, UIString(), caret, Merge::None);
}

bool UIPieceTableV2::removeSelection()
{
	if (hasSelection() == false) return false;
	int32_t start = std::min(markAnchor, caret);
	int32_t length = std::max(markAnchor, caret) - start;
	return change(start, length, UIString(), start, Merge::None);
}

bool UIPieceTableV2::setText(UIString const& text)
{
	if (document == text) return false;
	reset(text);
	return true;
}

bool UIPieceTableV2::clear()
{
	return setText(UIString());
}

bool UIPieceTableV2::undo()
{
	if (undoIndex == 0) return false;
	seal = true;
	--undoIndex;
	Edit const& edit = undoStack[undoIndex];
	splice(edit.Pos, edit.Inserted, edit.Removed);
	caret = edit.CursorBefore;
	markAnchor = edit.AnchorBefore;
	clampCaret();
	if (markAnchor == caret) markAnchor = -1;
	return true;
}

bool UIPieceTableV2::redo()
{
	if (undoIndex >= undoStack.size()) return false;
	Edit const& edit = undoStack[undoIndex];
	splice(edit.Pos, edit.Removed, edit.Inserted);
	caret = edit.CursorAfter;
	markAnchor = edit.AnchorAfter;
	++undoIndex;
	seal = true;
	clampCaret();
	if (markAnchor == caret) markAnchor = -1;
	return true;
}

bool UIPieceTableV2::canUndo() const
{
	return undoIndex > 0;
}

bool UIPieceTableV2::canRedo() const
{
	return undoIndex < undoStack.size();
}

void UIPieceTableV2::endUndoGroup()
{
	seal = true;
}

bool UIPieceTableV2::isModified() const
{
	return modified;
}

void UIPieceTableV2::setModified(bool value)
{
	modified = value;
}

int32_t UIPieceTableV2::limit() const
{
	if (document.size() > (size_t)INT32_MAX) return INT32_MAX;
	return (int32_t)document.size();
}

int32_t UIPieceTableV2::previous(int32_t index) const
{
	if (index <= 0) return 0;
	--index;
	while (index > 0 && ((uint8_t)document[(size_t)index] & 0xC0) == 0x80) --index;
	return index;
}

int32_t UIPieceTableV2::next(int32_t index) const
{
	if (index >= limit()) return limit();
	int32_t bytes = ::UIUTF8Num(document[(size_t)index]);
	if (bytes <= 0) bytes = 1;
	index += bytes;
	if (index > limit() || index < 0) return limit();
	return index;
}

bool UIPieceTableV2::isSpace(int32_t index) const
{
	if (index < 0 || index >= limit()) return false;
	char value = document[(size_t)index];
	return value == ' ' || value == '\t' || value == '\n' || value == '\r' || value == '\f' || value == '\v';
}

bool UIPieceTableV2::isWord(int32_t index) const
{
	if (index < 0 || index >= limit()) return false;
	unsigned char value = (unsigned char)document[(size_t)index];
	if ((value & 0xC0) == 0x80) return false;
	if (value >= 0xC0) return true;
	return std::isalnum(value) != 0 || value == '_';
}

int32_t UIPieceTableV2::wordForward(int32_t index) const
{
	int32_t n = limit();
	if (index >= n) return n;
	if (isSpace(index))
	{
		while (index < n && isSpace(index)) index = next(index);
		return index;
	}
	bool word = isWord(index);
	while (index < n && isSpace(index) == false && isWord(index) == word) index = next(index);
	while (index < n && isSpace(index)) index = next(index);
	return index;
}

int32_t UIPieceTableV2::wordBackward(int32_t index) const
{
	if (index <= 0) return 0;
	index = previous(index);
	while (index > 0 && isSpace(index)) index = previous(index);
	if (isSpace(index)) return 0;
	bool word = isWord(index);
	while (index > 0)
	{
		int32_t prev = previous(index);
		if (isSpace(prev) || isWord(prev) != word) break;
		index = prev;
	}
	return index;
}

void UIPieceTableV2::clampCaret()
{
	int32_t n = limit();
	if (caret < 0) caret = 0;
	if (caret > n) caret = n;
	if (markAnchor > n) markAnchor = n;
	if (markAnchor < -1) markAnchor = -1;
}

void UIPieceTableV2::reset(UIString const& text)
{
	original = text;
	added.clear();
	pieces.clear();
	if (text.empty() == false) pieces.push_back(Piece{ Buffer::Original, 0, text.size() });
	document = text;
	caret = limit();
	markAnchor = -1;
	undoStack.clear();
	undoIndex = 0;
	seal = true;
	modified = false;
}

void UIPieceTableV2::relink(size_t position, size_t removeLength, size_t addStart, size_t addLength)
{
	if (position > document.size()) position = document.size();
	if (removeLength > document.size() - position) removeLength = document.size() - position;

	size_t end = position + removeLength;
	UIList<Piece> kept;
	kept.reserve(pieces.size() + 2);
	size_t current = 0;
	for (auto const& piece : pieces)
	{
		size_t pieceEnd = current + piece.length;
		if (piece.length == 0)
		{
			current = pieceEnd;
			continue;
		}
		if (pieceEnd <= position || current >= end)
		{
			kept.push_back(piece);
		}
		else
		{
			if (current < position)
				kept.push_back(Piece{ piece.buffer, piece.start, position - current });
			if (pieceEnd > end)
				kept.push_back(Piece{ piece.buffer, piece.start + (end - current), pieceEnd - end });
		}
		current = pieceEnd;
	}

	if (addLength > 0)
	{
		UIList<Piece> withAdd;
		withAdd.reserve(kept.size() + 1);
		size_t offset = 0;
		bool placed = false;
		Piece addedPiece{ Buffer::Add, addStart, addLength };
		for (auto const& piece : kept)
		{
			if (placed == false && offset >= position)
			{
				withAdd.push_back(addedPiece);
				placed = true;
			}
			withAdd.push_back(piece);
			offset += piece.length;
		}
		if (placed == false) withAdd.push_back(addedPiece);
		kept.swap(withAdd);
	}

	UIList<Piece> merged;
	merged.reserve(kept.size());
	for (auto const& piece : kept)
	{
		if (piece.length == 0) continue;
		if (merged.empty() == false)
		{
			Piece& tail = merged.back();
			if (tail.buffer == piece.buffer && tail.start + tail.length == piece.start)
			{
				tail.length += piece.length;
				continue;
			}
		}
		merged.push_back(piece);
	}
	pieces.swap(merged);
	rebuild();
}

void UIPieceTableV2::splice(size_t position, UIString const& eraseText, UIString const& insertText)
{
	size_t addStart = 0;
	if (insertText.empty() == false)
	{
		addStart = added.size();
		added.append(insertText);
	}
	relink(position, eraseText.size(), addStart, insertText.size());
}

bool UIPieceTableV2::change(int32_t position, int32_t removeLength, UIString const& text, int32_t cursorAfter, Merge kind)
{
	int32_t n = limit();
	if (position < 0) position = 0;
	if (position > n) position = n;
	if (removeLength < 0) removeLength = 0;
	if (removeLength > n - position) removeLength = n - position;
	if (removeLength == 0 && text.empty()) return false;

	Edit edit;
	edit.Pos = (size_t)position;
	edit.Removed = document.substr((size_t)position, (size_t)removeLength);
	edit.Inserted = text;
	edit.CursorBefore = caret;
	edit.AnchorBefore = markAnchor;
	edit.CursorAfter = cursorAfter;
	edit.AnchorAfter = -1;
	edit.Kind = kind;
	splice(edit.Pos, edit.Removed, edit.Inserted);
	caret = cursorAfter;
	markAnchor = -1;
	clampCaret();
	edit.CursorAfter = caret;
	push(std::move(edit));
	modified = true;
	return true;
}

void UIPieceTableV2::push(Edit edit)
{
	if (seal == false && undoIndex > 0)
	{
		Edit& last = undoStack[undoIndex - 1];
		if (edit.Kind == Merge::Insert && last.Kind == Merge::Insert && edit.Removed.empty()
			&& edit.Pos == last.Pos + last.Inserted.size())
		{
			last.Inserted += edit.Inserted;
			last.CursorAfter = edit.CursorAfter;
			last.AnchorAfter = edit.AnchorAfter;
			return;
		}
		if (edit.Kind == Merge::Backspace && last.Kind == Merge::Backspace
			&& edit.Pos + edit.Removed.size() == last.Pos)
		{
			last.Removed.insert(0, edit.Removed);
			last.Pos = edit.Pos;
			last.CursorAfter = edit.CursorAfter;
			last.AnchorAfter = edit.AnchorAfter;
			return;
		}
		if (edit.Kind == Merge::Delete && last.Kind == Merge::Delete && edit.Pos == last.Pos)
		{
			last.Removed += edit.Removed;
			last.CursorAfter = edit.CursorAfter;
			last.AnchorAfter = edit.AnchorAfter;
			return;
		}
	}

	if (undoIndex < undoStack.size())
		undoStack.erase(undoStack.begin() + (std::ptrdiff_t)undoIndex, undoStack.end());
	undoStack.push_back(std::move(edit));
	++undoIndex;
	seal = false;
	if (undoStack.size() > 256)
	{
		undoStack.erase(undoStack.begin());
		if (undoIndex > 0) --undoIndex;
	}
}

void UIPieceTableV2::rebuild()
{
	document.clear();
	size_t bytes = 0;
	for (auto const& piece : pieces) bytes += piece.length;
	document.reserve(bytes);
	for (auto const& piece : pieces)
	{
		UIString const& buffer = piece.buffer == Buffer::Original ? original : added;
		document.append(buffer, piece.start, piece.length);
	}
}
