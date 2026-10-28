/*=================================================
* Copyright © 2020-2026 ChivenZhang.
* All Rights Reserved.
* =================================================*/
#pragma once
#include "../UI.h"

/// UTF-8 piece tree with indexed lines, selections, snapshots and undo/redo.
/// Offsets/columns are bytes; line numbers are zero based. LF, CR and CRLF are
/// preserved. Caret operations respect UTF-8/CRLF boundaries; range edits are
/// byte-exact. Document size is limited to INT32_MAX for OpenUI's cursor API.
/// Like the widgets, this class is used from one thread.
/// Edits update affected pieces and tree ancestors; they never rebuild the full
/// text. text() materializes a lazy cache; indexed line/range reads do not.
class UIPieceTableV3
{
public:
    struct Position
    {
        size_t Line = 0;
        size_t Column = 0;
        bool operator==(const Position&) const = default;
    };

    /// Owns immutable buffer slices, and remains valid after edits/reset/destruction.
    class Snapshot
    {
    public:
        Snapshot() = default;
        size_t size() const;
        UIString text() const;
        bool read(UIString& chunk);
        void rewind();

    private:
        friend class UIPieceTableV3;
        struct Data;
        std::shared_ptr<const Data> m_Data;
        size_t m_Index = 0;
    };

    struct Statistics
    {
        size_t Pieces = 0;
        size_t TreeHeight = 0;
        size_t CachedBytes = 0;
    };

    UIPieceTableV3();
    explicit UIPieceTableV3(UIStringView text);
    ~UIPieceTableV3();
    UIPieceTableV3(UIPieceTableV3&&) noexcept;
    UIPieceTableV3& operator=(UIPieceTableV3&&) noexcept;
    UIPieceTableV3(const UIPieceTableV3&) = delete;
    UIPieceTableV3& operator=(const UIPieceTableV3&) = delete;

    // The cached string reference is valid until the next content change.
    UIString const& text() const;
    UIString textAt(size_t position, size_t length = UIString::npos) const;
    size_t size() const;
    bool empty() const;
    char charAt(size_t position) const;
    uint64_t revision() const;
    Snapshot snapshot();

    size_t lineCount() const;
    size_t lineStart(size_t line) const;
    size_t lineEnd(size_t line) const;
    UIString lineText(size_t line) const;
    Position positionAt(size_t offset) const;
    size_t offsetAt(size_t line, size_t column = 0) const;

    // Range edits clamp offsets/lengths to the document and are undoable.
    bool insertAt(size_t position, UIStringView text);
    bool erase(size_t position, size_t length);
    bool replace(size_t position, size_t length, UIStringView text);

    int32_t cursor() const;
    int32_t anchor() const;
    bool hasSelection() const;
    void selection(int32_t& start, int32_t& length) const;
    int32_t selectionLength() const;
    UIString selectedText() const;
    void moveCaret(int32_t position, bool mark = false);
    void beginDrag(int32_t position);
    void setSelection(int32_t start, int32_t length);
    void selectAll();
    void selectWord();
    void deselect();
    void moveCodePoints(int32_t steps, bool mark = false);
    void moveWords(int32_t steps, bool mark = false);
    void moveHome(bool mark = false);
    void moveEnd(bool mark = false);
    void moveLineHome(bool mark = false);
    void moveLineEnd(bool mark = false);

    bool insert(UIString const& text, bool merge = true);
    bool backspace();
    bool del();
    bool deletePreviousWord();
    bool deleteNextWord();
    bool removeSelection();
    // Loading text clears history and establishes a clean save point.
    bool setText(UIString const& text);
    bool clear();

    bool undo();
    bool redo();
    bool canUndo() const;
    bool canRedo() const;
    void beginUndoGroup();
    void endUndoGroup();
    void clearHistory();
    void setUndoLimit(size_t groups);
    bool isModified() const;
    void setModified(bool value);

    // Diagnostics for tree invariants and fragmentation (O(number of pieces)).
    bool validate() const;
    Statistics statistics() const;

private:
    struct Impl;
    std::unique_ptr<Impl> m_Impl;
};
