#pragma once
#include "UIPieceTableV3.h"
#include <algorithm>

// IME preedit is a display overlay. The document/selection stay untouched until
// a committed text event arrives, so cancellation never creates an undo entry.
class UIComposition
{
public:
    struct Display
    {
        const UIString& Text;
        int32_t Cursor = 0;
        int32_t CursorStart = -1;
        int32_t CompositionStart = -1;
        int32_t CompositionEnd = -1;
    };

    bool pending(const UIPieceTableV3& table) const
    {
        return m_Pending && m_Revision == table.revision()
            && m_Cursor == table.cursor() && m_Anchor == table.anchor();
    }

    bool active(const UIPieceTableV3& table) const { return pending(table) && !m_Text.empty(); }

    void update(UIPieceTableV3& table, UIString text, uint32_t start, uint32_t length)
    {
        if (!pending(table))
        {
            clear();
            if (text.empty()) return;
            m_Pending = true;
            m_Revision = table.revision();
            m_Cursor = table.cursor();
            m_Anchor = table.anchor();
            table.endUndoGroup();
        }
        m_Text = std::move(text);
        // SDL's preedit selection is in UTF-8 characters; painter offsets are bytes.
        m_Start = byteOffset(m_Text, start);
        m_End = byteOffset(m_Text, uint64_t(start) + length);
        m_Display = table.text();
        int32_t offset = table.hasSelection() ? std::min(table.cursor(), table.anchor()) : table.cursor();
        m_Display.replace(size_t(offset), size_t(table.selectionLength()), m_Text);
        // Keep the session marker on an empty update: SDL may send it immediately
        // before the commit event. The committed word must remain one undo step.
    }

    void clear()
    {
        m_Text.clear();
        m_Display.clear();
        m_Pending = false;
        m_Start = m_End = 0;
    }

    Display display(const UIPieceTableV3& table, bool focused) const
    {
        if (!focused || !active(table)) return { table.text(), table.cursor(), table.anchor() };
        int32_t start = table.hasSelection() ? std::min(table.cursor(), table.anchor()) : table.cursor();
        Display result{ m_Display };
        result.CompositionStart = start;
        result.CompositionEnd = start + int32_t(m_Text.size());
        result.Cursor = start + int32_t(m_Start);
        result.CursorStart = m_Start == m_End ? -1 : start + int32_t(m_End);
        return result;
    }

    int32_t documentOffset(const UIPieceTableV3& table, int32_t displayOffset) const
    {
        if (!active(table)) return displayOffset;
        int32_t start = table.hasSelection() ? std::min(table.cursor(), table.anchor()) : table.cursor();
        if (displayOffset <= start) return displayOffset;
        if (size_t(displayOffset - start) <= m_Text.size()) return start;
        return displayOffset - int32_t(m_Text.size()) + table.selectionLength();
    }

private:
    static size_t byteOffset(const UIString& text, uint64_t characters)
    {
        size_t offset = 0;
        while (offset < text.size() && characters--)
        {
            size_t bytes = size_t(std::max(1, UIUTF8Num(text[offset])));
            offset += std::min(bytes, text.size() - offset);
        }
        return offset;
    }

    UIString m_Text, m_Display;
    bool m_Pending = false;
    uint64_t m_Revision = 0;
    int32_t m_Cursor = 0, m_Anchor = -1;
    size_t m_Start = 0, m_End = 0;
};
