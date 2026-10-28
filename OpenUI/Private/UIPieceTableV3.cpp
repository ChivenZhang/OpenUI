/*=================================================
* Copyright © 2020-2026 ChivenZhang.
* All Rights Reserved.
* =================================================*/
#include "UIPieceTableV3.h"
#include <climits>
#include <stdexcept>

// Architecture reference (independent C++/UTF-8 implementation):
// https://code.visualstudio.com/blogs/2018/03/23/text-buffer-reimplementation
// https://github.com/microsoft/vscode/tree/main/src/vs/editor/common/model/pieceTreeTextBuffer
namespace
{
constexpr size_t BufferSize = 64 * 1024;

struct Summary
{
    size_t Length = 0, Breaks = 0;
    bool StartsLF = false, EndsCR = false;
    bool operator==(const Summary&) const = default;
};

Summary combine(Summary a, Summary b)
{
    if (a.Length == 0) return b;
    if (b.Length == 0) return a;
    return { a.Length + b.Length, a.Breaks + b.Breaks - size_t(a.EndsCR && b.StartsLF), a.StartsLF, b.EndsCR };
}

struct Buffer
{
    UIString Text;
    // Positions of CR and standalone LF. Appending LF after CR never moves an
    // existing entry, so slices keep their line index even when a buffer grows.
    UIList<size_t> Breaks;

    Buffer() { Text.reserve(BufferSize); }

    void append(UIStringView text)
    {
        size_t start = Text.size();
        Text.append(text);
        for (size_t i = start; i < Text.size(); ++i)
            if (Text[i] == '\r' || (Text[i] == '\n' && (i == 0 || Text[i - 1] != '\r')))
                Breaks.push_back(i);
    }

    Summary summary(size_t start, size_t length) const
    {
        if (length == 0) return {};
        auto first = std::lower_bound(Breaks.begin(), Breaks.end(), start);
        auto end = std::lower_bound(first, Breaks.end(), start + length);
        bool splitLF = Text[start] == '\n' && start > 0 && Text[start - 1] == '\r';
        return { length, size_t(end - first) + size_t(splitLF), Text[start] == '\n', Text[start + length - 1] == '\r' };
    }
};

struct Piece
{
    std::shared_ptr<Buffer> Storage;
    size_t Start = 0;
    Summary Info;

    Piece slice(size_t offset, size_t length) const
    {
        return { Storage, Start + offset, Storage->summary(Start + offset, length) };
    }

    UIStringView view() const { return UIStringView(Storage->Text).substr(Start, Info.Length); }
};

using Pieces = UIList<Piece>;

void appendPiece(Pieces& pieces, Piece piece)
{
    if (piece.Info.Length == 0) return;
    if (!pieces.empty())
    {
        auto& last = pieces.back();
        if (last.Storage == piece.Storage && last.Start + last.Info.Length == piece.Start)
        {
            last.Info = combine(last.Info, piece.Info);
            return;
        }
    }
    pieces.push_back(std::move(piece));
}

size_t lengthOf(const Pieces& pieces)
{
    size_t length = 0;
    for (const auto& piece : pieces) length += piece.Info.Length;
    return length;
}

struct Node
{
    Piece Value;
    Summary Total;
    Node* Parent = nullptr;
    Node* Left = nullptr;
    Node* Right = nullptr;
    bool Red = true;
    explicit Node(Piece value) : Value(std::move(value)), Total(Value.Info) {}
};

Summary total(const Node* node) { return node ? node->Total : Summary{}; }
bool red(const Node* node) { return node && node->Red; }
void update(Node* node) { node->Total = combine(combine(total(node->Left), node->Value.Info), total(node->Right)); }
void refresh(Node* node) { for (; node; node = node->Parent) update(node); }
Node* first(Node* node) { if (node) while (node->Left) node = node->Left; return node; }
Node* last(Node* node) { if (node) while (node->Right) node = node->Right; return node; }

Node* successor(Node* node)
{
    if (node->Right) return first(node->Right);
    while (node->Parent && node == node->Parent->Right) node = node->Parent;
    return node->Parent;
}

Node* predecessor(Node* node)
{
    if (node->Left) return last(node->Left);
    while (node->Parent && node == node->Parent->Left) node = node->Parent;
    return node->Parent;
}

void destroy(Node* node)
{
    if (!node) return;
    destroy(node->Left);
    destroy(node->Right);
    delete node;
}

bool continuation(unsigned char value) { return (value & 0xC0) == 0x80; }
}

struct UIPieceTableV3::Snapshot::Data
{
    Pieces Slices;
    size_t Length = 0;
};

struct UIPieceTableV3::Impl
{
    enum class Merge { None, Insert, Backspace, Delete };
    struct Edit
    {
        size_t Offset = 0;
        Pieces Removed, Inserted;
        size_t RemovedLength = 0, InsertedLength = 0;
    };
    struct UndoGroup
    {
        UIList<Edit> Edits;
        int32_t CursorBefore = 0, AnchorBefore = -1, CursorAfter = 0, AnchorAfter = -1;
        uint64_t StateBefore = 0, StateAfter = 0;
        Merge Kind = Merge::None;
    };

    Node* Root = nullptr;
    std::shared_ptr<Buffer> AddBuffer;
    mutable UIString Cache;
    mutable bool CacheValid = false;
    uint64_t Revision = 0, State = 0, SavedState = 0, NextState = 0;
    bool ForcedModified = false, Sealed = true;
    int32_t Caret = 0, Anchor = -1;
    UIList<UndoGroup> History;
    size_t HistoryIndex = 0, UndoLimit = 256, GroupDepth = 0;

    ~Impl() { destroy(Root); }
    size_t size() const { return total(Root).Length; }

    void rotateLeft(Node* node)
    {
        Node* pivot = node->Right;
        node->Right = pivot->Left;
        if (node->Right) node->Right->Parent = node;
        pivot->Parent = node->Parent;
        if (!node->Parent) Root = pivot;
        else if (node == node->Parent->Left) node->Parent->Left = pivot;
        else node->Parent->Right = pivot;
        pivot->Left = node;
        node->Parent = pivot;
        update(node);
        update(pivot);
    }

    void rotateRight(Node* node)
    {
        Node* pivot = node->Left;
        node->Left = pivot->Right;
        if (node->Left) node->Left->Parent = node;
        pivot->Parent = node->Parent;
        if (!node->Parent) Root = pivot;
        else if (node == node->Parent->Left) node->Parent->Left = pivot;
        else node->Parent->Right = pivot;
        pivot->Right = node;
        node->Parent = pivot;
        update(node);
        update(pivot);
    }

    void fixInsert(Node* node)
    {
        while (red(node->Parent))
        {
            Node* parent = node->Parent;
            Node* grand = parent->Parent;
            if (parent == grand->Left)
            {
                Node* uncle = grand->Right;
                if (red(uncle))
                {
                    parent->Red = uncle->Red = false;
                    grand->Red = true;
                    node = grand;
                }
                else
                {
                    if (node == parent->Right) { node = parent; rotateLeft(node); }
                    node->Parent->Red = false;
                    node->Parent->Parent->Red = true;
                    rotateRight(node->Parent->Parent);
                }
            }
            else
            {
                Node* uncle = grand->Left;
                if (red(uncle))
                {
                    parent->Red = uncle->Red = false;
                    grand->Red = true;
                    node = grand;
                }
                else
                {
                    if (node == parent->Left) { node = parent; rotateRight(node); }
                    node->Parent->Red = false;
                    node->Parent->Parent->Red = true;
                    rotateLeft(node->Parent->Parent);
                }
            }
        }
        Root->Red = false;
    }

    Node* insertBefore(Node* next, Piece piece)
    {
        auto node = std::make_unique<Node>(std::move(piece));
        Node* parent = next ? (next->Left ? last(next->Left) : next) : last(Root);
        node->Parent = parent;
        if (!parent) Root = node.get();
        else if (parent == next) parent->Left = node.get();
        else parent->Right = node.get();
        Node* result = node.release();
        refresh(result);
        fixInsert(result);
        return result;
    }

    void transplant(Node* node, Node* replacement)
    {
        if (!node->Parent) Root = replacement;
        else if (node == node->Parent->Left) node->Parent->Left = replacement;
        else node->Parent->Right = replacement;
        if (replacement) replacement->Parent = node->Parent;
    }

    void fixDelete(Node* node, Node* parent)
    {
        while (node != Root && !red(node))
        {
            if (node == parent->Left)
            {
                Node* sibling = parent->Right;
                if (red(sibling))
                {
                    sibling->Red = false; parent->Red = true;
                    rotateLeft(parent); sibling = parent->Right;
                }
                if (!sibling || (!red(sibling->Left) && !red(sibling->Right)))
                {
                    if (sibling) sibling->Red = true;
                    node = parent; parent = node->Parent;
                }
                else
                {
                    if (!red(sibling->Right))
                    {
                        sibling->Left->Red = false; sibling->Red = true;
                        rotateRight(sibling); sibling = parent->Right;
                    }
                    sibling->Red = parent->Red; parent->Red = false;
                    sibling->Right->Red = false;
                    rotateLeft(parent); node = Root;
                }
            }
            else
            {
                Node* sibling = parent->Left;
                if (red(sibling))
                {
                    sibling->Red = false; parent->Red = true;
                    rotateRight(parent); sibling = parent->Left;
                }
                if (!sibling || (!red(sibling->Left) && !red(sibling->Right)))
                {
                    if (sibling) sibling->Red = true;
                    node = parent; parent = node->Parent;
                }
                else
                {
                    if (!red(sibling->Left))
                    {
                        sibling->Right->Red = false; sibling->Red = true;
                        rotateLeft(sibling); sibling = parent->Left;
                    }
                    sibling->Red = parent->Red; parent->Red = false;
                    sibling->Left->Red = false;
                    rotateRight(parent); node = Root;
                }
            }
        }
        if (node) node->Red = false;
    }

    void eraseNode(Node* node)
    {
        Node* removed = node;
        bool wasRed = removed->Red;
        Node* child;
        Node* parent;
        if (!node->Left || !node->Right)
        {
            child = node->Left ? node->Left : node->Right;
            parent = node->Parent;
            transplant(node, child);
            refresh(parent);
        }
        else
        {
            removed = first(node->Right);
            wasRed = removed->Red;
            child = removed->Right;
            if (removed->Parent == node) parent = removed;
            else
            {
                parent = removed->Parent;
                transplant(removed, child);
                removed->Right = node->Right;
                removed->Right->Parent = removed;
            }
            transplant(node, removed);
            removed->Left = node->Left;
            removed->Left->Parent = removed;
            removed->Red = node->Red;
            refresh(parent);
        }
        delete node;
        if (!wasRed) fixDelete(child, parent);
    }

    std::pair<Node*, size_t> find(size_t offset) const
    {
        Node* node = Root;
        while (node)
        {
            size_t left = total(node->Left).Length;
            if (offset < left) node = node->Left;
            else if (offset < left + node->Value.Info.Length) return { node, offset - left };
            else { offset -= left + node->Value.Info.Length; node = node->Right; }
        }
        return { nullptr, 0 };
    }

    char charAt(size_t offset) const
    {
        auto [node, remainder] = find(offset);
        return node ? node->Value.Storage->Text[node->Value.Start + remainder] : '\0';
    }

    Pieces range(size_t offset, size_t length) const
    {
        Pieces pieces;
        if (offset >= size()) return pieces;
        length = std::min(length, size() - offset);
        auto [node, remainder] = find(offset);
        while (length)
        {
            size_t count = std::min(length, node->Value.Info.Length - remainder);
            appendPiece(pieces, node->Value.slice(remainder, count));
            length -= count;
            node = successor(node);
            remainder = 0;
        }
        return pieces;
    }

    Pieces store(UIStringView text)
    {
        Pieces pieces;
        while (!text.empty())
        {
            if (!AddBuffer || AddBuffer->Text.size() == BufferSize) AddBuffer = std::make_shared<Buffer>();
            size_t offset = AddBuffer->Text.size();
            size_t count = std::min(text.size(), BufferSize - offset);
            AddBuffer->append(text.substr(0, count));
            pieces.push_back({ AddBuffer, offset, AddBuffer->summary(offset, count) });
            text.remove_prefix(count);
        }
        return pieces;
    }

    Node* splitAt(size_t offset)
    {
        auto [node, remainder] = find(offset);
        if (!node || remainder == 0) return node;
        Piece right = node->Value.slice(remainder, node->Value.Info.Length - remainder);
        Node* split = insertBefore(successor(node), std::move(right));
        node->Value = node->Value.slice(0, remainder);
        refresh(node);
        return split;
    }

    void mergeAt(size_t offset)
    {
        auto [right, remainder] = find(offset);
        if (!right || remainder != 0) return;
        Node* left = predecessor(right);
        if (!left || left->Value.Storage != right->Value.Storage
            || left->Value.Start + left->Value.Info.Length != right->Value.Start) return;
        left->Value.Info = combine(left->Value.Info, right->Value.Info);
        eraseNode(right);
        refresh(left);
    }

    void splice(size_t offset, size_t removedLength, const Pieces& inserted)
    {
        Node* begin = splitAt(offset);
        Node* end = splitAt(offset + removedLength);
        // splitAt may split begin when the entire edit is inside one piece.
        if (removedLength == 0) begin = end;
        while (begin != end)
        {
            Node* next = successor(begin);
            eraseNode(begin);
            begin = next;
        }
        for (const auto& piece : inserted) insertBefore(end, piece);
        mergeAt(offset);
        mergeAt(offset + lengthOf(inserted));
        CacheValid = false;
        Cache.clear();
    }

    Summary prefix(size_t length) const
    {
        Summary result;
        Node* node = Root;
        while (node && length)
        {
            auto left = total(node->Left);
            if (length < left.Length) { node = node->Left; continue; }
            result = combine(result, left);
            length -= left.Length;
            size_t count = std::min(length, node->Value.Info.Length);
            result = combine(result, node->Value.slice(0, count).Info);
            length -= count;
            if (!length) break;
            node = node->Right;
        }
        return result;
    }

    // Find the start byte of a logical newline, carrying CR across piece edges.
    size_t breakOffset(size_t ordinal) const
    {
        Node* node = Root;
        size_t offset = 0;
        bool previousCR = false;
        while (node)
        {
            auto left = total(node->Left);
            size_t count = left.Breaks - size_t(left.Length && previousCR && left.StartsLF);
            if (ordinal <= count) { node = node->Left; continue; }
            ordinal -= count;
            offset += left.Length;
            if (left.Length) previousCR = left.EndsCR;
            const Piece& piece = node->Value;
            count = piece.Info.Breaks - size_t(previousCR && piece.Info.StartsLF);
            if (ordinal <= count)
            {
                const auto& buffer = *piece.Storage;
                bool splitLF = piece.Info.StartsLF && piece.Start > 0 && buffer.Text[piece.Start - 1] == '\r';
                if (splitLF && !previousCR)
                {
                    if (ordinal == 1) return offset;
                    --ordinal;
                }
                size_t start = piece.Start + size_t(previousCR && piece.Info.StartsLF);
                auto firstBreak = std::lower_bound(buffer.Breaks.begin(), buffer.Breaks.end(), start);
                return offset + firstBreak[ordinal - 1] - piece.Start;
            }
            ordinal -= count;
            offset += piece.Info.Length;
            previousCR = piece.Info.EndsCR;
            node = node->Right;
        }
        return size();
    }

    size_t lineStart(size_t line) const
    {
        if (line == 0) return 0;
        if (line > total(Root).Breaks) return size();
        size_t offset = breakOffset(line);
        return offset + ((charAt(offset) == '\r' && charAt(offset + 1) == '\n') ? 2 : 1);
    }

    size_t lineEnd(size_t line) const
    {
        if (line >= total(Root).Breaks) return size();
        size_t end = lineStart(line + 1);
        if (end && charAt(end - 1) == '\n') --end;
        if (end && charAt(end - 1) == '\r') --end;
        return end;
    }

    size_t lineAt(size_t offset) const
    {
        offset = std::min(offset, size());
        auto info = prefix(offset);
        return info.Breaks - size_t(info.EndsCR && charAt(offset) == '\n');
    }

    int32_t next(int32_t offset) const;
    int32_t previous(int32_t offset) const;
    int32_t boundary(int64_t offset) const;
    int wordClass(int32_t offset) const;
    int32_t wordForward(int32_t offset) const;
    int32_t wordBackward(int32_t offset) const;
    bool change(size_t offset, size_t length, UIStringView text, Merge kind);
    void seal() { if (GroupDepth == 0) Sealed = true; }
    void trimHistory()
    {
        if (History.size() <= UndoLimit) return;
        size_t remove = History.size() - UndoLimit;
        // Prefer discarding old undo groups; discard far-future redo groups next.
        size_t front = std::min(remove, HistoryIndex);
        History.erase(History.begin(), History.begin() + front);
        HistoryIndex -= front;
        History.resize(History.size() - (remove - front));
    }
};

int32_t UIPieceTableV3::Impl::next(int32_t offset) const
{
    size_t start = size_t(std::clamp<int64_t>(offset, 0, size()));
    if (start == size()) return int32_t(start);
    auto lead = static_cast<unsigned char>(charAt(start));
    if (lead == '\r' && charAt(start + 1) == '\n') return int32_t(start + 2);
    size_t bytes = lead >= 0xC2 && lead <= 0xDF ? 2 : lead >= 0xE0 && lead <= 0xEF ? 3
        : lead >= 0xF0 && lead <= 0xF4 ? 4 : 1;
    if (bytes > size() - start) bytes = 1;
    for (size_t i = 1; i < bytes; ++i)
        if (!continuation(static_cast<unsigned char>(charAt(start + i)))) { bytes = 1; break; }
    auto second = static_cast<unsigned char>(charAt(start + 1));
    if ((lead == 0xE0 && second < 0xA0) || (lead == 0xED && second >= 0xA0)
        || (lead == 0xF0 && second < 0x90) || (lead == 0xF4 && second >= 0x90)) bytes = 1;
    return int32_t(start + bytes);
}

int32_t UIPieceTableV3::Impl::previous(int32_t offset) const
{
    offset = int32_t(std::clamp<int64_t>(offset, 0, size()));
    if (offset == 0) return 0;
    if (offset >= 2 && charAt(offset - 1) == '\n' && charAt(offset - 2) == '\r') return offset - 2;
    int32_t start = offset - 1;
    for (int i = 0; i < 3 && start > 0 && continuation(static_cast<unsigned char>(charAt(start))); ++i) --start;
    return next(start) == offset ? start : offset - 1;
}

int32_t UIPieceTableV3::Impl::boundary(int64_t offset) const
{
    auto index = int32_t(std::clamp<int64_t>(offset, 0, size()));
    if (index == 0 || size_t(index) == size()) return index;
    if (charAt(index) == '\n' && charAt(index - 1) == '\r') return index - 1;
    int32_t start = index;
    for (int i = 0; i < 3 && start > 0 && continuation(static_cast<unsigned char>(charAt(start))); ++i) --start;
    return next(start) > index ? start : index;
}

int UIPieceTableV3::Impl::wordClass(int32_t offset) const
{
    unsigned char value = static_cast<unsigned char>(charAt(offset));
    if (value == ' ' || value == '\t' || value == '\r' || value == '\n' || value == '\f' || value == '\v') return 0;
    if (value >= 0x80 || (value >= 'a' && value <= 'z') || (value >= 'A' && value <= 'Z')
        || (value >= '0' && value <= '9') || value == '_') return 1;
    return 2;
}

int32_t UIPieceTableV3::Impl::wordForward(int32_t offset) const
{
    if (size_t(offset) >= size()) return int32_t(size());
    int category = wordClass(offset);
    while (size_t(offset) < size() && wordClass(offset) == category) offset = next(offset);
    while (size_t(offset) < size() && wordClass(offset) == 0) offset = next(offset);
    return offset;
}

int32_t UIPieceTableV3::Impl::wordBackward(int32_t offset) const
{
    if (offset == 0) return 0;
    offset = previous(offset);
    while (offset > 0 && wordClass(offset) == 0) offset = previous(offset);
    int category = wordClass(offset);
    while (offset > 0 && wordClass(previous(offset)) == category) offset = previous(offset);
    return offset;
}

bool UIPieceTableV3::Impl::change(size_t offset, size_t length, UIStringView text, Merge kind)
{
    offset = std::min(offset, size());
    length = std::min(length, size() - offset);
    if (text.size() > size_t(INT32_MAX) - (size() - length)) throw std::length_error("UIPieceTableV3 exceeds INT32_MAX bytes");
    if (length == 0 && text.empty()) return false;

    Edit edit;
    edit.Offset = offset;
    edit.RemovedLength = length;
    edit.InsertedLength = text.size();
    edit.Removed = range(offset, length);
    bool identical = length == text.size();
    size_t compared = 0;
    if (identical)
        for (const auto& piece : edit.Removed)
        {
            if (piece.view() != text.substr(compared, piece.Info.Length)) { identical = false; break; }
            compared += piece.Info.Length;
        }
    if (identical)
    {
        int32_t placed = boundary(offset + text.size());
        bool moved = Caret != placed || Anchor != -1;
        Caret = placed; Anchor = -1;
        seal();
        return moved;
    }

    edit.Inserted = store(text);
    bool merge = false;
    if (!Sealed && HistoryIndex > 0 && HistoryIndex == History.size())
    {
        const auto& group = History.back();
        const auto& previousEdit = group.Edits.back();
        if (GroupDepth) merge = true;
        else if (group.Kind == kind && Caret == group.CursorAfter && Anchor == group.AnchorAfter)
        {
            if (kind == Merge::Insert && length == 0 && previousEdit.RemovedLength == 0)
                merge = offset == previousEdit.Offset + previousEdit.InsertedLength;
            else if (kind == Merge::Backspace && text.empty())
                merge = offset + length == previousEdit.Offset;
            else if (kind == Merge::Delete && text.empty()) merge = offset == previousEdit.Offset;
        }
    }

    int32_t cursorBefore = Caret, anchorBefore = Anchor;
    uint64_t stateBefore = State;
    splice(offset, length, edit.Inserted);
    Caret = boundary(offset + text.size());
    Anchor = -1;
    State = ++NextState;
    ++Revision;

    History.erase(History.begin() + HistoryIndex, History.end());
    if (UndoLimit != 0)
    {
        if (!merge)
        {
            UndoGroup group;
            group.CursorBefore = cursorBefore;
            group.AnchorBefore = anchorBefore;
            group.StateBefore = stateBefore;
            group.Kind = kind;
            History.push_back(std::move(group));
        }
        auto& group = History.back();
        group.Edits.push_back(std::move(edit));
        group.CursorAfter = Caret;
        group.AnchorAfter = Anchor;
        group.StateAfter = State;
    }
    HistoryIndex = History.size();
    trimHistory();
    Sealed = false;
    return true;
}

UIPieceTableV3::UIPieceTableV3() : m_Impl(std::make_unique<Impl>()) {}

UIPieceTableV3::UIPieceTableV3(UIStringView text) : UIPieceTableV3()
{
    if (text.size() > size_t(INT32_MAX)) throw std::length_error("UIPieceTableV3 exceeds INT32_MAX bytes");
    auto pieces = m_Impl->store(text);
    for (const auto& piece : pieces) m_Impl->insertBefore(nullptr, piece);
    // Initial buffers are immutable. Subsequent edits use new append buffers.
    m_Impl->AddBuffer.reset();
    m_Impl->Caret = int32_t(text.size());
}

UIPieceTableV3::~UIPieceTableV3() = default;
UIPieceTableV3::UIPieceTableV3(UIPieceTableV3&&) noexcept = default;
UIPieceTableV3& UIPieceTableV3::operator=(UIPieceTableV3&&) noexcept = default;

UIString const& UIPieceTableV3::text() const
{
    if (!m_Impl->CacheValid)
    {
        UIString text;
        text.reserve(size());
        for (Node* node = first(m_Impl->Root); node; node = successor(node)) text.append(node->Value.view());
        m_Impl->Cache = std::move(text);
        m_Impl->CacheValid = true;
    }
    return m_Impl->Cache;
}

UIString UIPieceTableV3::textAt(size_t position, size_t length) const
{
    UIString result;
    if (position >= size()) return result;
    length = std::min(length, size() - position);
    result.reserve(length);
    auto [node, remainder] = m_Impl->find(position);
    while (length)
    {
        size_t count = std::min(length, node->Value.Info.Length - remainder);
        result.append(node->Value.view().substr(remainder, count));
        length -= count;
        node = successor(node);
        remainder = 0;
    }
    return result;
}

size_t UIPieceTableV3::size() const { return m_Impl->size(); }
bool UIPieceTableV3::empty() const { return size() == 0; }
char UIPieceTableV3::charAt(size_t position) const { return m_Impl->charAt(position); }
uint64_t UIPieceTableV3::revision() const { return m_Impl->Revision; }

UIPieceTableV3::Snapshot UIPieceTableV3::snapshot()
{
    Snapshot result;
    auto data = std::make_shared<Snapshot::Data>();
    data->Slices = m_Impl->range(0, size());
    data->Length = size();
    result.m_Data = std::move(data);
    // Freeze all referenced storage, including the partially filled append buffer.
    m_Impl->AddBuffer.reset();
    return result;
}

size_t UIPieceTableV3::Snapshot::size() const { return m_Data ? m_Data->Length : 0; }
UIString UIPieceTableV3::Snapshot::text() const
{
    UIString result;
    result.reserve(size());
    if (m_Data) for (const auto& piece : m_Data->Slices) result.append(piece.view());
    return result;
}
bool UIPieceTableV3::Snapshot::read(UIString& chunk)
{
    if (!m_Data || m_Index == m_Data->Slices.size()) { chunk.clear(); return false; }
    chunk = m_Data->Slices[m_Index++].view();
    return true;
}
void UIPieceTableV3::Snapshot::rewind() { m_Index = 0; }

size_t UIPieceTableV3::lineCount() const { return total(m_Impl->Root).Breaks + 1; }
size_t UIPieceTableV3::lineStart(size_t line) const { return m_Impl->lineStart(line); }
size_t UIPieceTableV3::lineEnd(size_t line) const { return m_Impl->lineEnd(line); }
UIString UIPieceTableV3::lineText(size_t line) const { return textAt(lineStart(line), lineEnd(line) - lineStart(line)); }
UIPieceTableV3::Position UIPieceTableV3::positionAt(size_t offset) const
{
    offset = std::min(offset, size());
    size_t line = m_Impl->lineAt(offset);
    return { line, std::min(offset, lineEnd(line)) - lineStart(line) };
}
size_t UIPieceTableV3::offsetAt(size_t line, size_t column) const
{
    size_t start = lineStart(line);
    return start + std::min(column, lineEnd(line) - start);
}

bool UIPieceTableV3::insertAt(size_t position, UIStringView text) { return replace(position, 0, text); }
bool UIPieceTableV3::erase(size_t position, size_t length) { return replace(position, length, {}); }
bool UIPieceTableV3::replace(size_t position, size_t length, UIStringView text)
{
    return m_Impl->change(position, length, text, Impl::Merge::None);
}

int32_t UIPieceTableV3::cursor() const { return m_Impl->Caret; }
int32_t UIPieceTableV3::anchor() const { return m_Impl->Anchor; }
bool UIPieceTableV3::hasSelection() const { return anchor() >= 0 && anchor() != cursor(); }
void UIPieceTableV3::selection(int32_t& start, int32_t& length) const
{
    start = hasSelection() ? std::min(cursor(), anchor()) : -1;
    length = hasSelection() ? std::max(cursor(), anchor()) - start : 0;
}
int32_t UIPieceTableV3::selectionLength() const { return hasSelection() ? std::abs(cursor() - anchor()) : 0; }
UIString UIPieceTableV3::selectedText() const
{
    int32_t start, length;
    selection(start, length);
    return length ? textAt(size_t(start), size_t(length)) : UIString{};
}

void UIPieceTableV3::moveCaret(int32_t position, bool mark)
{
    m_Impl->seal();
    if (!mark) m_Impl->Anchor = -1;
    else if (anchor() < 0) m_Impl->Anchor = cursor();
    m_Impl->Caret = m_Impl->boundary(position);
}
void UIPieceTableV3::beginDrag(int32_t position)
{
    moveCaret(position, false);
    m_Impl->Anchor = cursor();
}
void UIPieceTableV3::setSelection(int32_t start, int32_t length)
{
    m_Impl->seal();
    m_Impl->Anchor = m_Impl->boundary(start);
    m_Impl->Caret = m_Impl->boundary(int64_t(start) + length);
    if (anchor() == cursor()) m_Impl->Anchor = -1;
}
void UIPieceTableV3::selectAll() { setSelection(0, int32_t(size())); }
void UIPieceTableV3::deselect() { m_Impl->seal(); m_Impl->Anchor = -1; }
void UIPieceTableV3::selectWord()
{
    m_Impl->seal();
    if (empty()) { m_Impl->Caret = 0; m_Impl->Anchor = -1; return; }
    int32_t index = size_t(cursor()) == size() ? m_Impl->previous(cursor()) : cursor();
    int category = m_Impl->wordClass(index);
    int32_t begin = index, end = m_Impl->next(index);
    while (begin > 0 && m_Impl->wordClass(m_Impl->previous(begin)) == category) begin = m_Impl->previous(begin);
    while (size_t(end) < size() && m_Impl->wordClass(end) == category) end = m_Impl->next(end);
    m_Impl->Anchor = begin;
    m_Impl->Caret = end;
}
void UIPieceTableV3::moveCodePoints(int32_t steps, bool mark)
{
    int32_t offset = cursor();
    while (steps > 0 && size_t(offset) < size()) { offset = m_Impl->next(offset); --steps; }
    while (steps < 0 && offset > 0) { offset = m_Impl->previous(offset); ++steps; }
    moveCaret(offset, mark);
}
void UIPieceTableV3::moveWords(int32_t steps, bool mark)
{
    int32_t offset = cursor();
    while (steps > 0 && size_t(offset) < size()) { offset = m_Impl->wordForward(offset); --steps; }
    while (steps < 0 && offset > 0) { offset = m_Impl->wordBackward(offset); ++steps; }
    moveCaret(offset, mark);
}
void UIPieceTableV3::moveHome(bool mark) { moveCaret(0, mark); }
void UIPieceTableV3::moveEnd(bool mark) { moveCaret(int32_t(size()), mark); }
void UIPieceTableV3::moveLineHome(bool mark) { moveCaret(int32_t(lineStart(m_Impl->lineAt(cursor()))), mark); }
void UIPieceTableV3::moveLineEnd(bool mark) { moveCaret(int32_t(lineEnd(m_Impl->lineAt(cursor()))), mark); }

bool UIPieceTableV3::insert(UIString const& text, bool merge)
{
    bool selected = hasSelection();
    size_t offset = selected ? size_t(std::min(cursor(), anchor())) : size_t(cursor());
    return m_Impl->change(offset, size_t(selectionLength()), text,
        merge && !selected ? Impl::Merge::Insert : Impl::Merge::None);
}
bool UIPieceTableV3::backspace()
{
    if (hasSelection()) return removeSelection();
    if (cursor() == 0) return false;
    int32_t begin = m_Impl->previous(cursor());
    return m_Impl->change(begin, cursor() - begin, {}, Impl::Merge::Backspace);
}
bool UIPieceTableV3::del()
{
    if (hasSelection()) return removeSelection();
    if (size_t(cursor()) == size()) return false;
    return m_Impl->change(cursor(), m_Impl->next(cursor()) - cursor(), {}, Impl::Merge::Delete);
}
bool UIPieceTableV3::deletePreviousWord()
{
    if (hasSelection()) return removeSelection();
    int32_t begin = m_Impl->wordBackward(cursor());
    return m_Impl->change(begin, cursor() - begin, {}, Impl::Merge::None);
}
bool UIPieceTableV3::deleteNextWord()
{
    if (hasSelection()) return removeSelection();
    return m_Impl->change(cursor(), m_Impl->wordForward(cursor()) - cursor(), {}, Impl::Merge::None);
}
bool UIPieceTableV3::removeSelection()
{
    if (!hasSelection()) return false;
    return m_Impl->change(std::min(cursor(), anchor()), selectionLength(), {}, Impl::Merge::None);
}
bool UIPieceTableV3::setText(UIString const& value)
{
    if (size() == value.size() && textAt(0, size()) == value) return false;
    UIPieceTableV3 replacement(value);
    replacement.m_Impl->Revision = revision() + 1;
    replacement.m_Impl->UndoLimit = m_Impl->UndoLimit;
    m_Impl.swap(replacement.m_Impl);
    return true;
}
bool UIPieceTableV3::clear() { return setText({}); }

bool UIPieceTableV3::undo()
{
    if (!canUndo()) return false;
    auto& state = *m_Impl;
    const auto& group = state.History[state.HistoryIndex - 1];
    for (auto edit = group.Edits.rbegin(); edit != group.Edits.rend(); ++edit)
        state.splice(edit->Offset, edit->InsertedLength, edit->Removed);
    state.Caret = group.CursorBefore;
    state.Anchor = group.AnchorBefore;
    state.State = group.StateBefore;
    --state.HistoryIndex;
    ++state.Revision;
    state.Sealed = true;
    state.GroupDepth = 0;
    return true;
}
bool UIPieceTableV3::redo()
{
    if (!canRedo()) return false;
    auto& state = *m_Impl;
    const auto& group = state.History[state.HistoryIndex];
    for (const auto& edit : group.Edits) state.splice(edit.Offset, edit.RemovedLength, edit.Inserted);
    state.Caret = group.CursorAfter;
    state.Anchor = group.AnchorAfter;
    state.State = group.StateAfter;
    ++state.HistoryIndex;
    ++state.Revision;
    state.Sealed = true;
    state.GroupDepth = 0;
    return true;
}
bool UIPieceTableV3::canUndo() const { return m_Impl->HistoryIndex != 0; }
bool UIPieceTableV3::canRedo() const { return m_Impl->HistoryIndex < m_Impl->History.size(); }
void UIPieceTableV3::beginUndoGroup() { if (m_Impl->GroupDepth++ == 0) m_Impl->Sealed = true; }
void UIPieceTableV3::endUndoGroup()
{
    if (m_Impl->GroupDepth) --m_Impl->GroupDepth;
    m_Impl->seal();
}
void UIPieceTableV3::clearHistory()
{
    m_Impl->History.clear();
    m_Impl->HistoryIndex = m_Impl->GroupDepth = 0;
    m_Impl->Sealed = true;
}
void UIPieceTableV3::setUndoLimit(size_t groups)
{
    m_Impl->UndoLimit = groups;
    m_Impl->trimHistory();
    m_Impl->Sealed = true;
}
bool UIPieceTableV3::isModified() const { return m_Impl->ForcedModified || m_Impl->State != m_Impl->SavedState; }
void UIPieceTableV3::setModified(bool value)
{
    m_Impl->ForcedModified = value;
    if (!value)
    {
        m_Impl->SavedState = m_Impl->State;
        m_Impl->Sealed = true;
    }
}

bool UIPieceTableV3::validate() const
{
    if (m_Impl->Root && (m_Impl->Root->Parent || m_Impl->Root->Red)) return false;
    auto visit = [&](auto&& self, Node* node, Node* parent) -> int
    {
        if (!node) return 1;
        if (node->Parent != parent || (node->Red && (red(node->Left) || red(node->Right)))) return -1;
        const auto& piece = node->Value;
        if (!piece.Storage || piece.Info.Length == 0 || piece.Start > piece.Storage->Text.size()
            || piece.Info.Length > piece.Storage->Text.size() - piece.Start) return -1;
        if (piece.Info != piece.Storage->summary(piece.Start, piece.Info.Length)) return -1;
        if (node->Total != combine(combine(total(node->Left), piece.Info), total(node->Right))) return -1;
        int left = self(self, node->Left, node), right = self(self, node->Right, node);
        return left < 0 || left != right ? -1 : left + int(!node->Red);
    };
    return visit(visit, m_Impl->Root, nullptr) > 0 && cursor() >= 0 && size_t(cursor()) <= size()
        && anchor() >= -1 && (anchor() < 0 || size_t(anchor()) <= size()) && m_Impl->HistoryIndex <= m_Impl->History.size();
}

UIPieceTableV3::Statistics UIPieceTableV3::statistics() const
{
    Statistics result;
    auto visit = [&](auto&& self, Node* node, size_t depth) -> void
    {
        if (!node) return;
        ++result.Pieces;
        result.TreeHeight = std::max(result.TreeHeight, depth);
        self(self, node->Left, depth + 1);
        self(self, node->Right, depth + 1);
    };
    visit(visit, m_Impl->Root, 1);
    result.CachedBytes = m_Impl->CacheValid ? m_Impl->Cache.size() : 0;
    return result;
}
