#pragma once
/*=================================================
* Copyright © 2020-2026 ChivenZhang.
* All Tops Reserved.
* =====================Note=========================
*
*
* ====================History=======================
* Created by chivenzhang@gmail.com.
*
* =================================================*/
#include "UIPublic.h"
class UIComputedStyle;

class OPENUI_API UIStyleData
{
public:
    virtual ~UIStyleData() = default;
    virtual bool inherited() const = 0;
    virtual std::type_info const& getType() const = 0;
    virtual UIString getText() const = 0;
    virtual void setText(UIString const& text) = 0;
    virtual void* getData(std::type_info const& type) = 0;
    virtual const void* getData(std::type_info const& type) const = 0;
};

using UIStyleDataRef = UIRef<UIStyleData>;
using UIStyleDataRaw = UIRaw<UIStyleData>;

template <class T>
class UIStyleValue : public UIStyleData
{
public:
    explicit UIStyleValue(T const& value, bool inherited) : m_Data(value), m_Inherited(inherited) { }
    bool inherited() const override { return m_Inherited; }
    std::type_info const& getType() const override { return typeid(T); }
    UIString getText() const override { UIString result; UITypeC(m_Data, result); return result; }

    void setText(const UIString& text) override { UITypeC(text, m_Data); }
    void* getData(const std::type_info& type) override { return (type == typeid(T)) ? &m_Data : nullptr; }
    const void* getData(const std::type_info& type) const override { return (type == typeid(T)) ? &m_Data : nullptr; }

protected:
    T m_Data;
    bool m_Inherited;
};

template <class T>
class UIStyleRefer : public UIStyleData
{
public:
    explicit UIStyleRefer(T& value, bool inherited) : m_Data(value), m_Inherited(inherited) { }
    bool inherited() const override { return m_Inherited; }
    std::type_info const& getType() const override { return typeid(T); }
    UIString getText() const override { UIString result; UITypeC(m_Data, result); return result; }
    void setText(const UIString& text) override { UITypeC(text, m_Data); }
    void* getData(const std::type_info& type) override { return (type == typeid(T)) ? &m_Data : nullptr; }
    const void* getData(const std::type_info& type) const override { return (type == typeid(T)) ? &m_Data : nullptr; }

protected:
    T& m_Data;
    bool m_Inherited;
};

/// @brief
class OPENUI_API UIStyle
{
public:
    UIStyle();
    ~UIStyle();
    bool getDirty() const;
    void setDirty(bool value);
    UIStyleDataRef getStyle(UIString const& key) const;
    void setStyle(UIString const& key, UIStyleDataRef value);
    UIString getStyleText(UIString const& key) const;
    bool setStyleText(UIString const& key, UIString const& value);

    template <class T>
    T const& getStyle(UIString const& key, T const& value = T()) const
    {
        auto result = getStyle(key);
        if (result && result->getData(typeid(std::remove_cvref_t<T>))) return *(T*)result->getData(
            typeid(std::remove_cvref_t<T>));
        return value;
    }

    template <class T>
    void setStyle(UIString const& key, T const& value, bool inherited = false)
    {
        auto result = getStyle(key);
        if (result) *(T*)result->getData(typeid(std::remove_cvref_t<T>)) = value;
        else this->setStyle(key, UICast<UIStyleData>(UINew<UIStyleValue<std::remove_cvref_t<T>>>(value, inherited)));
    }

    template <class T>
    void setEmbedStyle(UIString const& key, T& value, bool inherited = false)
    {
        this->setStyle(key, UICast<UIStyleData>(UINew<UIStyleRefer<std::remove_cvref_t<T>>>(value, inherited)));
    }

private:
    bool m_IsDirty;
    friend class UIComputedStyle;
    UIPrivateRaw m_Private;
};

using UIStyleRef = UIRef<UIStyle>;
using UIStyleRaw = UIRaw<UIStyle>;

//================================================================================================================

/// @brief
class OPENUI_API UIComputedStyle
{
public:
    explicit UIComputedStyle(UIRaw<UIStyle> style);
    ~UIComputedStyle();
    bool compute(UIRaw<UIComputedStyle> parent);
    UIStyleDataRaw getStyle(UIString const& key) const;

    template <class T>
    T const& getStyle(UIString const& key, T const& value = T()) const
    {
        auto result = getStyle(key);
        if (result && result->getData(typeid(std::remove_cvref_t<T>))) return *(T*)result->getData(
            typeid(std::remove_cvref_t<T>));
        return value;
    }

private:
    UIPrivateRaw m_Private;
};

using UIComputedStyleRef = UIRef<UIComputedStyle>;
using UIComputedStyleRaw = UIRaw<UIComputedStyle>;

//================================================================================================================

#include "UIPublic.h"

struct UIPropAlignContent
{
    ui_css_align_content_type_t Value = UI_CSS_ALIGN_CONTENT_STRETCH;
};

struct UIPropAlignItems
{
    ui_css_align_items_type_t Value = UI_CSS_ALIGN_ITEMS_STRETCH;
};

struct UIPropAlignSelf
{
    ui_css_align_self_type_t Value = UI_CSS_ALIGN_SELF_AUTO;
};

struct UIPropAlignmentBaseline
{
    ui_css_alignment_baseline_type_t Value = UI_CSS_ALIGNMENT_BASELINE_BASELINE;
};

struct UIPropBaselineShift
{
    float Value = 0.0f;
    ui_css_baseline_shift_type_t Type = UI_CSS_BASELINE_SHIFT_LENGTH;
};

struct UIPropBaselineSource
{
    ui_css_baseline_source_type_t Value = UI_CSS_BASELINE_SOURCE_AUTO;
};

struct UIPropBorderTop
{
    float Value = 0.0f;
    ui_css_border_top_type_t Type = UI_CSS_BORDER_NONE;
};

struct UIPropBorderBottom
{
    float Value = 0.0f;
    ui_css_border_bottom_type_t Type = UI_CSS_BORDER_NONE;
};

struct UIPropBorderLeft
{
    float Value = 0.0f;
    ui_css_border_left_type_t Type = UI_CSS_BORDER_NONE;
};

struct UIPropBorderRight
{
    float Value = 0.0f;
    ui_css_border_right_type_t Type = UI_CSS_BORDER_NONE;
};

struct UIPropBorder
{
    UIPropBorderLeft Left;
    UIPropBorderTop Top;
    UIPropBorderRight Right;
    UIPropBorderBottom Bottom;
};

struct UIPropBoxSizing
{
    ui_css_box_sizing_type_t Value = UI_CSS_BOX_SIZING_CONTENT_BOX;
};

struct UIPropClear
{
    ui_css_clear_type_t Value = UI_CSS_CLEAR_NONE;
};

struct UIPropColor
{
    union
    {
        struct
        {
            uint8_t R, G, B, A;
        };
        uint32_t RGBA = 0;
    } Value;
    ui_css_color_type_t Type = UI_CSS_COLOR_CURRENTCOLOR;
};

struct UIPropDirection
{
    ui_css_direction_type_t Value = UI_CSS_DIRECTION_LTR;
};

struct UIPropDisplay
{
    ui_css_display_type_t Value = UI_CSS_DISPLAY_FLEX;
};

struct UIPropDominantBaseline
{
    ui_css_dominant_baseline_type_t Value = UI_CSS_DOMINANT_BASELINE_AUTO;
};

struct UIPropFlexBasis
{
    float Value = UINAN;
    ui_css_flex_basis_type_t Type = UI_CSS_FLEX_BASIS_AUTO;
};

struct UIPropFlexDirection
{
    ui_css_flex_direction_type_t Value = UI_CSS_FLEX_DIRECTION_ROW;
};

struct UIPropFlexGrow
{
    float Value = 0;
    ui_css_flex_grow_type_t Type = UI_CSS_FLEX_GROW_NUMBER;
};

struct UIPropFlexShrink
{
    float Value = 1;
    ui_css_flex_shrink_type_t Type = UI_CSS_FLEX_SHRINK_NUMBER;
};

struct UIPropFlexWrap
{
    ui_css_flex_wrap_type_t Value = UI_CSS_FLEX_WRAP_NOWRAP;
};

struct UIPropFlex
{
    UIPropFlexGrow Grow = {0, UI_CSS_FLEX_GROW_NUMBER};
    UIPropFlexShrink Shrink = {1, UI_CSS_FLEX_SHRINK_NUMBER};
    UIPropFlexBasis Basis = {};
};

struct UIGridTrack
{
    ui_css_grid_track_type_t Type = UI_CSS_GRID_TRACK_AUTO;
    float Value = 0;
    ui_css_grid_track_type_t MinType = UI_CSS_GRID_TRACK_AUTO;
    float MinValue = 0;
    ui_css_grid_track_type_t MaxType = UI_CSS_GRID_TRACK_AUTO;
    float MaxValue = 0;
};

inline UIGridTrack UIGridTrackAuto()
{
    return {};
}

inline UIGridTrack UIGridTrackPoints(float value)
{
    UIGridTrack track;
    track.Type = UI_CSS_GRID_TRACK_POINTS;
    track.Value = value;
    return track;
}

inline UIGridTrack UIGridTrackPercent(float value)
{
    UIGridTrack track;
    track.Type = UI_CSS_GRID_TRACK_PERCENT;
    track.Value = value;
    return track;
}

inline UIGridTrack UIGridTrackFr(float value)
{
    UIGridTrack track;
    track.Type = UI_CSS_GRID_TRACK_FR;
    track.Value = value;
    return track;
}

inline UIGridTrack UIGridTrackMinMax(ui_css_grid_track_type_t minType, float minValue, ui_css_grid_track_type_t maxType, float maxValue)
{
    UIGridTrack track;
    track.Type = UI_CSS_GRID_TRACK_MINMAX;
    track.MinType = minType;
    track.MinValue = minValue;
    track.MaxType = maxType;
    track.MaxValue = maxValue;
    return track;
}

struct UIPropGridTemplateColumns
{
    UIList<UIGridTrack> Tracks;
};

struct UIPropGridTemplateRows
{
    UIList<UIGridTrack> Tracks;
};

struct UIPropGridAutoColumns
{
    UIList<UIGridTrack> Tracks;
};

struct UIPropGridAutoRows
{
    UIList<UIGridTrack> Tracks;
};

struct UIPropGridColumnStart
{
    int Value = 0;
    ui_css_grid_line_type_t Type = UI_CSS_GRID_LINE_AUTO;
};

struct UIPropGridColumnEnd
{
    int Value = 0;
    ui_css_grid_line_type_t Type = UI_CSS_GRID_LINE_AUTO;
};

struct UIPropGridRowStart
{
    int Value = 0;
    ui_css_grid_line_type_t Type = UI_CSS_GRID_LINE_AUTO;
};

struct UIPropGridRowEnd
{
    int Value = 0;
    ui_css_grid_line_type_t Type = UI_CSS_GRID_LINE_AUTO;
};

struct UIPropGridColumnGap
{
    float Value = 0;
    ui_css_grid_gap_type_t Type = UI_CSS_GRID_GAP_LENGTH;
};

struct UIPropGridRowGap
{
    float Value = 0;
    ui_css_grid_gap_type_t Type = UI_CSS_GRID_GAP_LENGTH;
};

struct UIPropGap
{
    UIPropGridRowGap Row;
    UIPropGridColumnGap Column;
};

struct UIPropGridColumn
{
    UIPropGridColumnStart Start;
    UIPropGridColumnEnd End;
};

struct UIPropGridRow
{
    UIPropGridRowStart Start;
    UIPropGridRowEnd End;
};

struct UIPropFloat
{
    ui_css_float_type_t Value = UI_CSS_FLOAT_NONE;
};

struct UIPropFloatDefer
{
    ui_css_float_defer_type_t Value = UI_CSS_FLOAT_DEFER_NONE;
};

struct UIPropFloatOffset
{
    float Value = 0.0f;
    ui_css_float_offset_type_t Type = UI_CSS_FLOAT_OFFSET_LENGTH;
};

struct UIPropFloatReference
{
    ui_css_float_reference_type_t Value = UI_CSS_FLOAT_REFERENCE_INLINE;
};

struct UIPropFontFamily
{
    ui_css_font_family_type_t Value = UI_CSS_FONT_FAMILY_UI_SANS_SERIF;
};

struct UIPropFontSize
{
    float Value = 0.0f;
    ui_css_font_size_type_t Type = UI_CSS_FONT_SIZE_MEDIUM;
};

struct UIPropFontStretch
{
    float Value = 0.0f;
    ui_css_font_stretch_type_t Type = UI_CSS_FONT_STRETCH_NORMAL;
};

struct UIPropFontStyle
{
    ui_css_font_style_type_t Value = UI_CSS_FONT_STYLE_NORMAL;
};

struct UIPropFontWeight
{
    float Value = 0.0f;
    ui_css_font_weight_type_t Type = UI_CSS_FONT_WEIGHT_NORMAL;
};

struct UIPropHangingPunctuation
{
    ui_css_hanging_punctuation_type_t Value = UI_CSS_HANGING_PUNCTUATION_NONE;
};

struct UIPropHeight
{
    float Value = UINAN;
    ui_css_height_type_t Type = UI_CSS_HEIGHT_AUTO;
};

struct UIPropHyphens
{
    ui_css_hyphens_type_t Value = UI_CSS_HYPHENS_MANUAL;
};

struct UIPropInsetBlockEnd
{
    float Value = 0.0f;
    ui_css_inset_block_end_type_t Type = UI_CSS_INSET_BLOCK_END_AUTO;
};

struct UIPropInsetBlockStart
{
    float Value = 0.0f;
    ui_css_inset_block_start_type_t Type = UI_CSS_INSET_BLOCK_START_AUTO;
};

struct UIPropInsetInlineEnd
{
    float Value = 0.0f;
    ui_css_inset_inline_end_type_t Type = UI_CSS_INSET_INLINE_END_AUTO;
};

struct UIPropInsetInlineStart
{
    float Value = 0.0f;
    ui_css_inset_inline_start_type_t Type = UI_CSS_INSET_INLINE_START_AUTO;
};

struct UIPropJustifyContent
{
    ui_css_justify_content_type_t Value = UI_CSS_JUSTIFY_CONTENT_FLEX_START;
};

struct UIPropJustifyItems
{
    ui_css_justify_content_type_t Value = UI_CSS_JUSTIFY_CONTENT_STRETCH;
};

struct UIPropJustifySelf
{
    ui_css_justify_content_type_t Value = UI_CSS_JUSTIFY_CONTENT_AUTO;
};

struct UIPropAspectRatio
{
    float Value = UINAN;
    bool Auto = true;
};

struct UIPropLetterSpacing
{
    float Value = 0.0f;
    ui_css_letter_spacing_type_t Type = UI_CSS_LETTER_SPACING_NORMAL;
};

struct UIPropLineBreak
{
    ui_css_line_break_type_t Value = UI_CSS_LINE_BREAK_AUTO;
};

struct UIPropLineHeight
{
    float Value = 0.0f;
    ui_css_line_height_type_t Type = UI_CSS_LINE_HEIGHT_NORMAL;
};

struct UIPropMarginLeft
{
    float Value = 0;
    ui_css_margin_left_type_t Type = UI_CSS_MARGIN_LENGTH;
};

struct UIPropMarginRight
{
    float Value = 0;
    ui_css_margin_right_type_t Type = UI_CSS_MARGIN_LENGTH;
};

struct UIPropMarginTop
{
    float Value = 0;
    ui_css_margin_top_type_t Type = UI_CSS_MARGIN_LENGTH;
};

struct UIPropMarginBottom
{
    float Value = 0;
    ui_css_margin_bottom_type_t Type = UI_CSS_MARGIN_LENGTH;
};

struct UIPropMargin
{
    UIPropMarginLeft Left;
    UIPropMarginTop Top;
    UIPropMarginRight Right;
    UIPropMarginBottom Bottom;
};

struct UIPropMaxHeight
{
    float Value = 0.0f;
    ui_css_max_height_type_t Type = UI_CSS_MAX_HEIGHT_NONE;
};

struct UIPropMaxWidth
{
    float Value = 0.0f;
    ui_css_max_width_type_t Type = UI_CSS_MAX_WIDTH_NONE;
};

struct UIPropMinHeight
{
    float Value = 0.0f;
    ui_css_min_height_type_t Type = UI_CSS_MIN_HEIGHT_AUTO;
};

struct UIPropMinWidth
{
    float Value = 0.0f;
    ui_css_min_width_type_t Type = UI_CSS_MIN_WIDTH_AUTO;
};

struct UIPropOpacity
{
    float Value = 1.0f;
    ui_css_opacity_type_t Type = UI_CSS_OPACITY_NUMBER;
};

struct UIPropOrder
{
    int32_t Value = 0;
    ui_css_order_type_t Type = UI_CSS_ORDER_INTEGER;
};

struct UIPropOverflowBlock
{
    ui_css_overflow_block_type_t Value = UI_CSS_OVERFLOW_BLOCK_VISIBLE;
};

struct UIPropOverflowInline
{
    ui_css_overflow_inline_type_t Value = UI_CSS_OVERFLOW_INLINE_VISIBLE;
};

struct UIPropOverflowWrap
{
    ui_css_overflow_wrap_type_t Value = UI_CSS_OVERFLOW_WRAP_NORMAL;
};

struct UIPropOverflowX
{
    ui_css_overflow_x_type_t Value = UI_CSS_OVERFLOW_X_VISIBLE;
};

struct UIPropOverflowY
{
    ui_css_overflow_y_type_t Value = UI_CSS_OVERFLOW_Y_VISIBLE;
};

struct UIPropOverflow
{
    UIPropOverflowX X;
    UIPropOverflowY Y;
};

struct UIPropPaddingTop
{
    float Value = 0;
    ui_css_padding_right_type_t Type = UI_CSS_PADDING_LENGTH;
};

struct UIPropPaddingBottom
{
    float Value = 0;
    ui_css_padding_bottom_type_t Type = UI_CSS_PADDING_LENGTH;
};

struct UIPropPaddingLeft
{
    float Value = 0;
    ui_css_padding_left_type_t Type = UI_CSS_PADDING_LENGTH;
};

struct UIPropPaddingRight
{
    float Value = 0;
    ui_css_padding_right_type_t Type = UI_CSS_PADDING_LENGTH;
};

struct UIPropPadding
{
    UIPropPaddingLeft Left;
    UIPropPaddingTop Top;
    UIPropPaddingRight Right;
    UIPropPaddingBottom Bottom;
};

struct UIPropPosition
{
    ui_css_position_type_t Value = UI_CSS_POSITION_STATIC;
};

struct UIPropTop
{
    float Value = UINAN;
    ui_css_top_type_t Type = UI_CSS_TOP_AUTO;
};

struct UIPropBottom
{
    float Value = 0.0f;
    ui_css_bottom_type_t Type = UI_CSS_BOTTOM_AUTO;
};

struct UIPropLeft
{
    float Value = 0.0f;
    ui_css_left_type_t Type = UI_CSS_LEFT_AUTO;
};

struct UIPropRight
{
    float Value = 0.0f;
    ui_css_right_type_t Type = UI_CSS_RIGHT_AUTO;
};

struct UIPropTabSize
{
    uint32_t Value = 8;
    ui_css_tab_size_type_t Type = UI_CSS_TAB_SIZE_NUMBER;
};

struct UIPropTextAlign
{
    ui_css_text_align_type_t Value = UI_CSS_TEXT_ALIGN_START;
};

struct UIPropTextAlignAll
{
    ui_css_text_align_all_type_t Value = UI_CSS_TEXT_ALIGN_ALL_START;
};

struct UIPropTextAlignLast
{
    ui_css_text_align_last_type_t Value = UI_CSS_TEXT_ALIGN_LAST_AUTO;
};

struct UIPropTextCombineUpright
{
    ui_css_text_combine_upright_type_t Value = UI_CSS_TEXT_COMBINE_UPRIGHT_NONE;
};

struct UIPropTextDecorationLine
{
    ui_css_text_decoration_line_type_t Value = UI_CSS_TEXT_DECORATION_LINE_NONE;
};

struct UIPropTextDecorationStyle
{
    ui_css_text_decoration_style_type_t Value = UI_CSS_TEXT_DECORATION_STYLE_SOLID;
};

struct UIPropTextIndent
{
    float Value = 0.0f;
    ui_css_text_indent_type_t Type = UI_CSS_TEXT_INDENT_LENGTH;
};

struct UIPropTextJustify
{
    ui_css_text_justify_type_t Value = UI_CSS_TEXT_JUSTIFY_AUTO;
};

struct UIPropTextOrientation
{
    ui_css_text_orientation_type_t Value = UI_CSS_TEXT_ORIENTATION_MIXED;
};

struct UIPropTextOverflow
{
    ui_css_text_overflow_type_t Value = UI_CSS_TEXT_OVERFLOW_CLIP;
};

struct UIPropTextTransform
{
    ui_css_text_transform_type_t Value = UI_CSS_TEXT_TRANSFORM_NONE;
};

struct UIPropUnicodeBidi
{
    ui_css_unicode_bidi_type_t Value = UI_CSS_UNICODE_BIDI_NORMAL;
};

struct UIPropVerticalAlign
{
    ui_css_vertical_align_type_t Value = UI_CSS_VERTICAL_ALIGN_FIRST;
};

struct UIPropVisibility
{
    ui_css_visibility_type_t Value = UI_CSS_VISIBILITY_VISIBLE;
};

struct UIPropWhiteSpace
{
    ui_css_white_space_type_t Value = UI_CSS_WHITE_SPACE_NORMAL;
};

struct UIPropWidth
{
    float Value = UINAN;
    ui_css_width_type_t Type = UI_CSS_WIDTH_AUTO;
};

struct UIPropWordBreak
{
    ui_css_word_break_type_t Value = UI_CSS_WORD_BREAK_NORMAL;
};

struct UIPropWordSpacing
{
    float Value = 0.0f;
    ui_css_word_spacing_type_t Type = UI_CSS_WORD_SPACING_NORMAL;
};

struct UIPropWordWrap
{
    ui_css_word_wrap_type_t Value = UI_CSS_WORD_WRAP_NORMAL;
};

struct UIPropWrapFlow
{
    ui_css_wrap_flow_type_t Value = UI_CSS_WRAP_FLOW_AUTO;
};

struct UIPropWrapThrough
{
    ui_css_wrap_through_type_t Value = UI_CSS_WRAP_THROUGH_NONE;
};

struct UIPropWritingMode
{
    ui_css_writing_mode_type_t Value = UI_CSS_WRITING_MODE_HORIZONTAL_TB;
};

struct UIPropZIndex
{
    int32_t Value = 0;
    ui_css_z_index_type_t Type = UI_CSS_Z_INDEX_AUTO;
};

struct UIPropFilter
{
    ui_css_filter_type_t Value = 0;
    UIString Func, Args;
};

// Helper: trim
static inline UIString UITrim(UIString s)
{
    size_t start = 0;
    while (start < s.size() && isspace((unsigned char)s[start])) ++start;
    size_t end = s.size();
    while (end > start && isspace((unsigned char)s[end - 1])) --end;
    return s.substr(start, end - start);
}

enum
{
    UI_CSS_MEASURE_KEYWORD,
    UI_CSS_MEASURE_LENGTH,
    UI_CSS_MEASURE_PERCENT,
};

struct UICssMeasure
{
    UIString Keyword;
    float Value = 0;
    int Kind = UI_CSS_MEASURE_KEYWORD;
};

inline bool UIParseMeasure(UIString const& src, UICssMeasure& dst)
{
    auto s = UITrim(src);
    if (s.empty()) return false;
    if (s.starts_with("fit-content"))
    {
        dst.Kind = UI_CSS_MEASURE_KEYWORD;
        dst.Keyword = "fit-content";
        return true;
    }
    if (s.ends_with('%'))
    {
        dst.Kind = UI_CSS_MEASURE_PERCENT;
        dst.Value = std::stof(s);
        return true;
    }
    if (s.ends_with("fr") && s.size() > 2 && (isdigit((unsigned char)s[0]) || s[0] == '.' || s[0] == '+' || s[0] == '-'))
    {
        dst.Kind = UI_CSS_MEASURE_KEYWORD;
        dst.Keyword = s;
        dst.Value = std::stof(s);
        return true;
    }
    if (s[0] == '+' || s[0] == '-' || s[0] == '.' || isdigit((unsigned char)s[0]))
    {
        dst.Kind = UI_CSS_MEASURE_LENGTH;
        dst.Value = std::stof(s);
        return true;
    }
    dst.Kind = UI_CSS_MEASURE_KEYWORD;
    dst.Keyword = s;
    return true;
}

inline UIList<UIString> UISplitCssTokens(UIString const& src)
{
    UIList<UIString> tokens;
    UIString current;
    int depth = 0;
    for (char ch : src)
    {
        if (ch == '(') { ++depth; current.push_back(ch); }
        else if (ch == ')')
        {
            if (depth > 0) --depth;
            current.push_back(ch);
        }
        else if (depth == 0 && isspace((unsigned char)ch))
        {
            if (current.empty() == false)
            {
                tokens.push_back(current);
                current.clear();
            }
        }
        else current.push_back(ch);
    }
    if (current.empty() == false) tokens.push_back(UITrim(current));
    return tokens;
}

inline UIList<UIString> UISplitCssSlash(UIString const& src)
{
    UIList<UIString> parts;
    UIString current;
    int depth = 0;
    for (char ch : src)
    {
        if (ch == '(') { ++depth; current.push_back(ch); }
        else if (ch == ')')
        {
            if (depth > 0) --depth;
            current.push_back(ch);
        }
        else if (ch == '/' && depth == 0)
        {
            parts.push_back(UITrim(current));
            current.clear();
        }
        else current.push_back(ch);
    }
    parts.push_back(UITrim(current));
    return parts;
}

inline bool UIParseGridTrack(UIString const& src, UIGridTrack& dst);
inline bool UIParseGridTracks(UIString const& src, UIList<UIGridTrack>& tracks);

inline bool UIParseGridTrack(UIString const& src, UIGridTrack& dst)
{
    auto s = UITrim(src);
    if (s == "auto" || s == "min-content" || s == "max-content" || s == "fit-content" || s.starts_with("fit-content("))
    {
        dst = UIGridTrackAuto();
        return true;
    }
    if (s.starts_with("minmax(") && s.ends_with(')'))
    {
        auto inner = s.substr(7, s.size() - 8);
        int depth = 0;
        size_t comma = UIString::npos;
        for (size_t i = 0; i < inner.size(); ++i)
        {
            if (inner[i] == '(') ++depth;
            else if (inner[i] == ')') { if (depth > 0) --depth; }
            else if (inner[i] == ',' && depth == 0) { comma = i; break; }
        }
        if (comma == UIString::npos) return false;
        UIGridTrack minTrack;
        UIGridTrack maxTrack;
        if (UIParseGridTrack(inner.substr(0, comma), minTrack) == false) return false;
        if (UIParseGridTrack(inner.substr(comma + 1), maxTrack) == false) return false;
        if (minTrack.Type == UI_CSS_GRID_TRACK_MINMAX || maxTrack.Type == UI_CSS_GRID_TRACK_MINMAX) return false;
        dst = UIGridTrackMinMax(minTrack.Type, minTrack.Value, maxTrack.Type, maxTrack.Value);
        return true;
    }
    UICssMeasure measure;
    if (UIParseMeasure(s, measure) == false) return false;
    if (measure.Kind == UI_CSS_MEASURE_PERCENT)
    {
        dst = UIGridTrackPercent(measure.Value);
        return true;
    }
    if (measure.Keyword.ends_with("fr"))
    {
        dst = UIGridTrackFr(measure.Value);
        return true;
    }
    if (measure.Kind == UI_CSS_MEASURE_LENGTH)
    {
        dst = UIGridTrackPoints(measure.Value);
        return true;
    }
    return false;
}

inline bool UIParseGridTracks(UIString const& src, UIList<UIGridTrack>& tracks)
{
    auto s = UITrim(src);
    if (s == "none")
    {
        tracks.clear();
        return true;
    }
    auto tokens = UISplitCssTokens(s);
    if (tokens.empty()) return false;
    tracks.clear();
    for (auto const& token : tokens)
    {
        if (token.starts_with("repeat(") && token.ends_with(')'))
        {
            auto inner = token.substr(7, token.size() - 8);
            int depth = 0;
            size_t comma = UIString::npos;
            for (size_t i = 0; i < inner.size(); ++i)
            {
                if (inner[i] == '(') ++depth;
                else if (inner[i] == ')') { if (depth > 0) --depth; }
                else if (inner[i] == ',' && depth == 0) { comma = i; break; }
            }
            if (comma == UIString::npos) return false;
            auto countText = UITrim(inner.substr(0, comma));
            if (countText.empty() || isdigit((unsigned char)countText[0]) == false) return false;
            int count = std::stoi(countText);
            UIList<UIGridTrack> repeated;
            if (UIParseGridTracks(inner.substr(comma + 1), repeated) == false) return false;
            for (int n = 0; n < count; ++n) tracks.insert(tracks.end(), repeated.begin(), repeated.end());
            continue;
        }
        UIGridTrack track;
        if (UIParseGridTrack(token, track) == false) return false;
        tracks.push_back(track);
    }
    return true;
}

inline UIString UIFormatGridTrack(UIGridTrack const& track)
{
    auto formatOne = [](ui_css_grid_track_type_t type, float value) -> UIString
    {
        switch (type)
        {
        case UI_CSS_GRID_TRACK_POINTS: return std::to_string(value) + "px";
        case UI_CSS_GRID_TRACK_PERCENT: return std::to_string(value) + "%";
        case UI_CSS_GRID_TRACK_FR: return std::to_string(value) + "fr";
        default: return "auto";
        }
    };
    if (track.Type == UI_CSS_GRID_TRACK_MINMAX)
        return "minmax(" + formatOne(track.MinType, track.MinValue) + ", " + formatOne(track.MaxType, track.MaxValue) + ")";
    return formatOne(track.Type, track.Value);
}

inline UIString UIFormatGridTracks(UIList<UIGridTrack> const& tracks)
{
    if (tracks.empty()) return "none";
    UIString text;
    for (size_t i = 0; i < tracks.size(); ++i)
    {
        if (i) text += " ";
        text += UIFormatGridTrack(tracks[i]);
    }
    return text;
}

inline bool UIParseGridLine(UIString const& src, int& value, ui_css_grid_line_type_t& type)
{
    auto tokens = UISplitCssTokens(src);
    if (tokens.size() == 1 && tokens[0] == "auto")
    {
        type = UI_CSS_GRID_LINE_AUTO;
        value = 0;
        return true;
    }
    if (tokens.size() == 1 && tokens[0] == "span")
    {
        type = UI_CSS_GRID_LINE_SPAN;
        value = 1;
        return true;
    }
    if (tokens.size() == 2 && tokens[0] == "span")
    {
        type = UI_CSS_GRID_LINE_SPAN;
        value = std::stoi(tokens[1]);
        return true;
    }
    if (tokens.size() == 1)
    {
        type = UI_CSS_GRID_LINE_INTEGER;
        value = std::stoi(tokens[0]);
        return true;
    }
    return false;
}

inline UIString UIFormatGridLine(ui_css_grid_line_type_t type, int value)
{
    if (type == UI_CSS_GRID_LINE_SPAN) return "span " + std::to_string(value);
    if (type == UI_CSS_GRID_LINE_INTEGER) return std::to_string(value);
    return "auto";
}

inline bool UIParseFlexBasis(UIString const& src, UIPropFlexBasis& dst)
{
    UICssMeasure measure;
    if (UIParseMeasure(src, measure) == false) return false;
    if (measure.Kind == UI_CSS_MEASURE_PERCENT)
    {
        dst.Type = UI_CSS_FLEX_BASIS_PERCENTAGE;
        dst.Value = measure.Value;
        return true;
    }
    if (measure.Kind == UI_CSS_MEASURE_LENGTH)
    {
        dst.Type = UI_CSS_FLEX_BASIS_LENGTH;
        dst.Value = measure.Value;
        return true;
    }
    if (measure.Keyword == "auto" || measure.Keyword == "content")
    {
        dst.Type = UI_CSS_FLEX_BASIS_AUTO;
        dst.Value = UINAN;
        return true;
    }
    if (measure.Keyword == "min-content") { dst.Type = UI_CSS_FLEX_BASIS_MIN_CONTENT; return true; }
    if (measure.Keyword == "max-content") { dst.Type = UI_CSS_FLEX_BASIS_MAX_CONTENT; return true; }
    if (measure.Keyword == "fit-content") { dst.Type = UI_CSS_FLEX_BASIS_FIT_CONTENT; return true; }
    if (measure.Keyword == "stretch") { dst.Type = UI_CSS_FLEX_BASIS_STRETCH; return true; }
    return false;
}

inline bool UIParseGap(UIString const& src, float& value, ui_css_grid_gap_type_t& type)
{
    auto s = UITrim(src);
    if (s == "normal")
    {
        value = 0;
        type = UI_CSS_GRID_GAP_LENGTH;
        return true;
    }
    UICssMeasure measure;
    if (UIParseMeasure(s, measure) == false) return false;
    if (measure.Kind == UI_CSS_MEASURE_PERCENT)
    {
        value = measure.Value;
        type = UI_CSS_GRID_GAP_PERCENTAGE;
        return true;
    }
    if (measure.Kind == UI_CSS_MEASURE_LENGTH)
    {
        value = measure.Value;
        type = UI_CSS_GRID_GAP_LENGTH;
        return true;
    }
    return false;
}

// Align Content
template <>
inline bool UITypeC(UIString const& src, UIPropAlignContent& dst)
{
    switch (UIHash(UITrim(src)))
    {
    default: return false;
    case UIHash("flex-start"): dst.Value = UI_CSS_ALIGN_CONTENT_FLEX_START;
        break;
    case UIHash("flex-end"): dst.Value = UI_CSS_ALIGN_CONTENT_FLEX_END;
        break;
    case UIHash("center"): dst.Value = UI_CSS_ALIGN_CONTENT_CENTER;
        break;
    case UIHash("space-between"): dst.Value = UI_CSS_ALIGN_CONTENT_SPACE_BETWEEN;
        break;
    case UIHash("space-around"): dst.Value = UI_CSS_ALIGN_CONTENT_SPACE_AROUND;
        break;
    case UIHash("space-evenly"): dst.Value = UI_CSS_ALIGN_CONTENT_SPACE_EVENLY;
        break;
    case UIHash("stretch"):
    case UIHash("normal"): dst.Value = UI_CSS_ALIGN_CONTENT_STRETCH;
        break;
    case UIHash("baseline"): dst.Value = UI_CSS_ALIGN_CONTENT_BASE_LINE;
        break;
    case UIHash("start"): dst.Value = UI_CSS_ALIGN_CONTENT_START;
        break;
    case UIHash("end"): dst.Value = UI_CSS_ALIGN_CONTENT_END;
        break;
    }
    return true;
}
template <>
inline bool UITypeC(UIPropAlignContent const& src, UIString& dst)
{
    switch (src.Value)
    {
    default: return false;
    case UI_CSS_ALIGN_CONTENT_FLEX_START: dst = "flex-start";
        break;
    case UI_CSS_ALIGN_CONTENT_FLEX_END: dst = "flex-end";
        break;
    case UI_CSS_ALIGN_CONTENT_CENTER: dst = "center";
        break;
    case UI_CSS_ALIGN_CONTENT_SPACE_BETWEEN: dst = "space-between";
        break;
    case UI_CSS_ALIGN_CONTENT_SPACE_AROUND: dst = "space-around";
        break;
    case UI_CSS_ALIGN_CONTENT_STRETCH: dst = "stretch";
        break;
    case UI_CSS_ALIGN_CONTENT_SPACE_EVENLY: dst = "space-evenly";
        break;
    case UI_CSS_ALIGN_CONTENT_BASE_LINE: dst = "baseline";
        break;
    case UI_CSS_ALIGN_CONTENT_START: dst = "start";
        break;
    case UI_CSS_ALIGN_CONTENT_END: dst = "end";
        break;
    }
    return true;
}

// Align Items
template <>
inline bool UITypeC(UIString const& src, UIPropAlignItems& dst)
{
    switch (UIHash(UITrim(src)))
    {
    default: return false;
    case UIHash("flex-start"): dst.Value = UI_CSS_ALIGN_ITEMS_FLEX_START;
        break;
    case UIHash("flex-end"): dst.Value = UI_CSS_ALIGN_ITEMS_FLEX_END;
        break;
    case UIHash("center"): dst.Value = UI_CSS_ALIGN_ITEMS_CENTER;
        break;
    case UIHash("baseline"): dst.Value = UI_CSS_ALIGN_ITEMS_BASELINE;
        break;
    case UIHash("stretch"):
    case UIHash("normal"): dst.Value = UI_CSS_ALIGN_ITEMS_STRETCH;
        break;
    case UIHash("space-evenly"): dst.Value = UI_CSS_ALIGN_ITEMS_SPACE_EVENLY;
        break;
    case UIHash("space-around"): dst.Value = UI_CSS_ALIGN_ITEMS_SPACE_AROUND;
        break;
    case UIHash("start"): dst.Value = UI_CSS_ALIGN_ITEMS_START;
        break;
    case UIHash("end"): dst.Value = UI_CSS_ALIGN_ITEMS_END;
        break;
    }
    return true;
}
template <>
inline bool UITypeC(UIPropAlignItems const& src, UIString& dst)
{
    switch (src.Value)
    {
    default: return false;
    case UI_CSS_ALIGN_ITEMS_FLEX_START: dst = "flex-start";
        break;
    case UI_CSS_ALIGN_ITEMS_FLEX_END: dst = "flex-end";
        break;
    case UI_CSS_ALIGN_ITEMS_CENTER: dst = "center";
        break;
    case UI_CSS_ALIGN_ITEMS_BASELINE: dst = "baseline";
        break;
    case UI_CSS_ALIGN_ITEMS_STRETCH: dst = "stretch";
        break;
    case UI_CSS_ALIGN_ITEMS_START: dst = "start";
        break;
    case UI_CSS_ALIGN_ITEMS_END: dst = "end";
        break;
    case UI_CSS_ALIGN_ITEMS_SPACE_EVENLY: dst = "space-evenly";
        break;
    case UI_CSS_ALIGN_ITEMS_SPACE_AROUND: dst = "space-around";
        break;
    }
    return true;
}

// Align Self
template <>
inline bool UITypeC(UIString const& src, UIPropAlignSelf& dst)
{
    switch (UIHash(UITrim(src)))
    {
    default: return false;
    case UIHash("auto"): dst.Value = UI_CSS_ALIGN_SELF_AUTO;
        break;
    case UIHash("flex-start"): dst.Value = UI_CSS_ALIGN_SELF_FLEX_START;
        break;
    case UIHash("flex-end"): dst.Value = UI_CSS_ALIGN_SELF_FLEX_END;
        break;
    case UIHash("center"): dst.Value = UI_CSS_ALIGN_SELF_CENTER;
        break;
    case UIHash("baseline"): dst.Value = UI_CSS_ALIGN_SELF_BASELINE;
        break;
    case UIHash("stretch"): dst.Value = UI_CSS_ALIGN_SELF_STRETCH;
        break;
    case UIHash("start"): dst.Value = UI_CSS_ALIGN_SELF_START;
        break;
    case UIHash("end"): dst.Value = UI_CSS_ALIGN_SELF_END;
        break;
    case UIHash("space-evenly"): dst.Value = UI_CSS_ALIGN_SPACE_EVENLY;
        break;
    case UIHash("space-around"): dst.Value = UI_CSS_ALIGN_SPACE_AROUND;
        break;
    }
    return true;
}

template <>
inline bool UITypeC(UIPropAlignSelf const& src, UIString& dst)
{
    switch (src.Value)
    {
    default: return false;
    case UI_CSS_ALIGN_SELF_AUTO: dst = "auto";
        break;
    case UI_CSS_ALIGN_SELF_FLEX_START: dst = "flex-start";
        break;
    case UI_CSS_ALIGN_SELF_FLEX_END: dst = "flex-end";
        break;
    case UI_CSS_ALIGN_SELF_CENTER: dst = "center";
        break;
    case UI_CSS_ALIGN_SELF_BASELINE: dst = "baseline";
        break;
    case UI_CSS_ALIGN_SELF_STRETCH: dst = "stretch";
        break;
    case UI_CSS_ALIGN_SELF_START: dst = "start";
        break;
    case UI_CSS_ALIGN_SELF_END: dst = "end";
        break;
    case UI_CSS_ALIGN_SPACE_EVENLY: dst = "space-evenly";
        break;
    case UI_CSS_ALIGN_SPACE_AROUND: dst = "space-around";
        break;
    }
    return true;
}

// Alignment Baseline
template <>
inline bool UITypeC(UIString const& src, UIPropAlignmentBaseline& dst)
{
    switch (UIHash(src))
    {
    default: return false;
    case UIHash("baseline"): dst.Value = UI_CSS_ALIGNMENT_BASELINE_BASELINE;
        break;
    case UIHash("text-bottom"): dst.Value = UI_CSS_ALIGNMENT_BASELINE_TEXT_BOTTOM;
        break;
    case UIHash("alphabetic"): dst.Value = UI_CSS_ALIGNMENT_BASELINE_ALPHABETIC;
        break;
    case UIHash("ideographic"): dst.Value = UI_CSS_ALIGNMENT_BASELINE_IDEOGRAPHIC;
        break;
    case UIHash("middle"): dst.Value = UI_CSS_ALIGNMENT_BASELINE_MIDDLE;
        break;
    case UIHash("central"): dst.Value = UI_CSS_ALIGNMENT_BASELINE_CENTRAL;
        break;
    case UIHash("mathematical"): dst.Value = UI_CSS_ALIGNMENT_BASELINE_MATHEMATICAL;
        break;
    case UIHash("text-top"): dst.Value = UI_CSS_ALIGNMENT_BASELINE_TEXT_TOP;
        break;
    }
    return true;
}

template <>
inline bool UITypeC(UIPropAlignmentBaseline const& src, UIString& dst)
{
    switch (src.Value)
    {
    default: return false;
    case UI_CSS_ALIGNMENT_BASELINE_BASELINE: dst = "baseline";
        break;
    case UI_CSS_ALIGNMENT_BASELINE_TEXT_BOTTOM: dst = "text-bottom";
        break;
    case UI_CSS_ALIGNMENT_BASELINE_ALPHABETIC: dst = "alphabetic";
        break;
    case UI_CSS_ALIGNMENT_BASELINE_IDEOGRAPHIC: dst = "ideographic";
        break;
    case UI_CSS_ALIGNMENT_BASELINE_MIDDLE: dst = "middle";
        break;
    case UI_CSS_ALIGNMENT_BASELINE_CENTRAL: dst = "central";
        break;
    case UI_CSS_ALIGNMENT_BASELINE_MATHEMATICAL: dst = "mathematical";
        break;
    case UI_CSS_ALIGNMENT_BASELINE_TEXT_TOP: dst = "text-top";
        break;
    }
    return true;
}

// Baseline Shift (supports keywords and numeric)
template <>
inline bool UITypeC(UIString const& src, UIPropBaselineShift& dst)
{
    auto s = UITrim(src);
    switch (UIHash(s))
    {
    default: break;
    case UIHash("sub"): dst.Type = UI_CSS_BASELINE_SHIFT_SUB;
        return true;
    case UIHash("super"): dst.Type = UI_CSS_BASELINE_SHIFT_SUPER;
        return true;
    case UIHash("top"): dst.Type = UI_CSS_BASELINE_SHIFT_TOP;
        return true;
    case UIHash("center"): dst.Type = UI_CSS_BASELINE_SHIFT_CENTER;
        return true;
    case UIHash("bottom"): dst.Type = UI_CSS_BASELINE_SHIFT_BOTTOM;
        return true;
    }
    if (src.ends_with('%'))
    {
        dst.Type = UI_CSS_BASELINE_SHIFT_PERCENTAGE;
        dst.Value = std::stof(src);
    }
    else
    {
        dst.Type = UI_CSS_BASELINE_SHIFT_LENGTH;
        dst.Value = std::stof(src);
    }
    return false;
}

template <>
inline bool UITypeC(UIPropBaselineShift const& src, UIString& dst)
{
    switch (src.Type)
    {
    default: return false;
    case UI_CSS_BASELINE_SHIFT_SUB: dst = "sub";
        return true;
    case UI_CSS_BASELINE_SHIFT_SUPER: dst = "super";
        return true;
    case UI_CSS_BASELINE_SHIFT_TOP: dst = "top";
        return true;
    case UI_CSS_BASELINE_SHIFT_CENTER: dst = "center";
        return true;
    case UI_CSS_BASELINE_SHIFT_BOTTOM: dst = "bottom";
        return true;
    case UI_CSS_BASELINE_SHIFT_PERCENTAGE: dst = std::to_string(src.Value) + "%";
        return true;
    case UI_CSS_BASELINE_SHIFT_LENGTH: dst = std::to_string(src.Value) + "px";
        return true;
    }
}

// Baseline Source
template <>
inline bool UITypeC(UIString const& src, UIPropBaselineSource& dst)
{
    switch (UIHash(src))
    {
    default: return false;
    case UIHash("auto"): dst.Value = UI_CSS_BASELINE_SOURCE_AUTO;
        break;
    case UIHash("first"): dst.Value = UI_CSS_BASELINE_SOURCE_FIRST;
        break;
    case UIHash("last"): dst.Value = UI_CSS_BASELINE_SOURCE_LAST;
        break;
    }
    return true;
}

template <>
inline bool UITypeC(UIPropBaselineSource const& src, UIString& dst)
{
    switch (src.Value)
    {
    default: return false;
    case UI_CSS_BASELINE_SOURCE_AUTO: dst = "auto";
        break;
    case UI_CSS_BASELINE_SOURCE_FIRST: dst = "first";
        break;
    case UI_CSS_BASELINE_SOURCE_LAST: dst = "last";
        break;
    }
    return true;
}

// Border (supports keywords and lengths)
template <>
inline bool UITypeC(UIString const& src, UIPropBorder& dst)
{
    auto parseSide = [](UIString const& token, float& value, ui_css_border_type_t& type) -> bool
    {
        auto s = UITrim(token);
        switch (UIHash(s))
        {
        case UIHash("thin"): type = UI_CSS_BORDER_THIN; return true;
        case UIHash("medium"): type = UI_CSS_BORDER_MEDIUM; return true;
        case UIHash("thick"): type = UI_CSS_BORDER_THICK; return true;
        case UIHash("none"): type = UI_CSS_BORDER_NONE; return true;
        case UIHash("hidden"): type = UI_CSS_BORDER_HIDDEN; return true;
        default: break;
        }
        UICssMeasure measure;
        if (UIParseMeasure(s, measure) == false || measure.Kind != UI_CSS_MEASURE_LENGTH) return false;
        type = UI_CSS_BORDER_LENGTH;
        value = measure.Value;
        return true;
    };
    auto tokens = UISplitCssTokens(src);
    float values[4] = {};
    ui_css_border_type_t types[4] = {};
    size_t count = 0;
    for (auto const& token : tokens)
    {
        float value = 0;
        ui_css_border_type_t type = UI_CSS_BORDER_NONE;
        if (parseSide(token, value, type) == false) continue;
        if (count == 4) return false;
        values[count] = value;
        types[count] = type;
        ++count;
    }
    if (count == 0) return false;
    auto rightIndex = count > 1 ? 1 : 0;
    auto bottomIndex = count > 2 ? 2 : 0;
    auto leftIndex = count > 3 ? 3 : rightIndex;
    dst.Top = {values[0], types[0]};
    dst.Right = {values[rightIndex], types[rightIndex]};
    dst.Bottom = {values[bottomIndex], types[bottomIndex]};
    dst.Left = {values[leftIndex], types[leftIndex]};
    return true;
}
template <>
inline bool UITypeC(UIPropBorder const& src, UIString& dst)
{
    auto formatSide = [](float value, ui_css_border_type_t type) -> UIString
    {
        switch (type)
        {
        case UI_CSS_BORDER_THIN: return "thin";
        case UI_CSS_BORDER_MEDIUM: return "medium";
        case UI_CSS_BORDER_THICK: return "thick";
        case UI_CSS_BORDER_NONE: return "none";
        case UI_CSS_BORDER_HIDDEN: return "hidden";
        case UI_CSS_BORDER_LENGTH: return std::to_string(value) + "px";
        default: return "none";
        }
    };
    dst = formatSide(src.Top.Value, src.Top.Type) + " " + formatSide(src.Right.Value, src.Right.Type) + " " + formatSide(src.Bottom.Value, src.Bottom.Type) + " " + formatSide(src.Left.Value, src.Left.Type);
    return true;
}

template <>
inline bool UITypeC(UIString const& src, UIPropBorderTop& dst)
{
    auto s = UITrim(src);
    switch (UIHash(s))
    {
    default: break;
    case UIHash("thin"): dst.Type = UI_CSS_BORDER_THIN;
        return true;
    case UIHash("medium"): dst.Type = UI_CSS_BORDER_MEDIUM;
        return true;
    case UIHash("thick"): dst.Type = UI_CSS_BORDER_THICK;
        return true;
    case UIHash("none"): dst.Type = UI_CSS_BORDER_NONE;
        return true;
    case UIHash("hidden"): dst.Type = UI_CSS_BORDER_HIDDEN;
        return true;
    case UIHash("dotted"): dst.Type = UI_CSS_BORDER_DOTTED;
        return true;
    case UIHash("dashed"): dst.Type = UI_CSS_BORDER_DASHED;
        return true;
    case UIHash("solid"): dst.Type = UI_CSS_BORDER_SOLID;
        return true;
    case UIHash("double"): dst.Type = UI_CSS_BORDER_DOUBLE;
        return true;
    case UIHash("groove"): dst.Type = UI_CSS_BORDER_GROOVE;
        return true;
    case UIHash("ridge"): dst.Type = UI_CSS_BORDER_RIDGE;
        return true;
    case UIHash("inset"): dst.Type = UI_CSS_BORDER_INSET;
        return true;
    case UIHash("outset"): dst.Type = UI_CSS_BORDER_OUTSET;
        return true;
    }
    dst.Type = UI_CSS_BORDER_LENGTH;
    dst.Value = std::stof(src);
    return true;
}
template <>
inline bool UITypeC(UIPropBorderTop const& src, UIString& dst)
{
    switch (src.Type)
    {
    default: return false;
    case UI_CSS_BORDER_THIN: dst = "thin";
        return true;
    case UI_CSS_BORDER_MEDIUM: dst = "medium";
        return true;
    case UI_CSS_BORDER_THICK: dst = "thick";
        return true;
    case UI_CSS_BORDER_NONE: dst = "none";
        return true;
    case UI_CSS_BORDER_HIDDEN: dst = "hidden";
        return true;
    case UI_CSS_BORDER_DOTTED: dst = "dotted";
        return true;
    case UI_CSS_BORDER_DASHED: dst = "dashed";
        return true;
    case UI_CSS_BORDER_SOLID: dst = "solid";
        return true;
    case UI_CSS_BORDER_DOUBLE: dst = "double";
        return true;
    case UI_CSS_BORDER_GROOVE: dst = "groove";
        return true;
    case UI_CSS_BORDER_RIDGE: dst = "ridge";
        return true;
    case UI_CSS_BORDER_INSET: dst = "inset";
        return true;
    case UI_CSS_BORDER_OUTSET: dst = "outset";
        return true;
    case UI_CSS_BORDER_LENGTH: dst = std::to_string(src.Value) + "px";
        return true;
    }
}

template <>
inline bool UITypeC(UIString const& src, UIPropBorderBottom& dst)
{
    auto s = UITrim(src);
    switch (UIHash(s))
    {
    default: break;
    case UIHash("thin"): dst.Type = UI_CSS_BORDER_THIN;
        return true;
    case UIHash("medium"): dst.Type = UI_CSS_BORDER_MEDIUM;
        return true;
    case UIHash("thick"): dst.Type = UI_CSS_BORDER_THICK;
        return true;
    case UIHash("none"): dst.Type = UI_CSS_BORDER_NONE;
        return true;
    case UIHash("hidden"): dst.Type = UI_CSS_BORDER_HIDDEN;
        return true;
    case UIHash("dotted"): dst.Type = UI_CSS_BORDER_DOTTED;
        return true;
    case UIHash("dashed"): dst.Type = UI_CSS_BORDER_DASHED;
        return true;
    case UIHash("solid"): dst.Type = UI_CSS_BORDER_SOLID;
        return true;
    case UIHash("double"): dst.Type = UI_CSS_BORDER_DOUBLE;
        return true;
    case UIHash("groove"): dst.Type = UI_CSS_BORDER_GROOVE;
        return true;
    case UIHash("ridge"): dst.Type = UI_CSS_BORDER_RIDGE;
        return true;
    case UIHash("inset"): dst.Type = UI_CSS_BORDER_INSET;
        return true;
    case UIHash("outset"): dst.Type = UI_CSS_BORDER_OUTSET;
        return true;
    }
    dst.Type = UI_CSS_BORDER_LENGTH;
    dst.Value = std::stof(src);
    return true;
}
template <>
inline bool UITypeC(UIPropBorderBottom const& src, UIString& dst)
{
    switch (src.Type)
    {
    default: return false;
    case UI_CSS_BORDER_THIN: dst = "thin";
        return true;
    case UI_CSS_BORDER_MEDIUM: dst = "medium";
        return true;
    case UI_CSS_BORDER_THICK: dst = "thick";
        return true;
    case UI_CSS_BORDER_NONE: dst = "none";
        return true;
    case UI_CSS_BORDER_HIDDEN: dst = "hidden";
        return true;
    case UI_CSS_BORDER_DOTTED: dst = "dotted";
        return true;
    case UI_CSS_BORDER_DASHED: dst = "dashed";
        return true;
    case UI_CSS_BORDER_SOLID: dst = "solid";
        return true;
    case UI_CSS_BORDER_DOUBLE: dst = "double";
        return true;
    case UI_CSS_BORDER_GROOVE: dst = "groove";
        return true;
    case UI_CSS_BORDER_RIDGE: dst = "ridge";
        return true;
    case UI_CSS_BORDER_INSET: dst = "inset";
        return true;
    case UI_CSS_BORDER_OUTSET: dst = "outset";
        return true;
    case UI_CSS_BORDER_LENGTH: dst = std::to_string(src.Value) + "px";
        return true;
    }
}

template <>
inline bool UITypeC(UIString const& src, UIPropBorderLeft& dst)
{
    auto s = UITrim(src);
    switch (UIHash(s))
    {
    default: break;
    case UIHash("thin"): dst.Type = UI_CSS_BORDER_THIN;
        return true;
    case UIHash("medium"): dst.Type = UI_CSS_BORDER_MEDIUM;
        return true;
    case UIHash("thick"): dst.Type = UI_CSS_BORDER_THICK;
        return true;
    case UIHash("none"): dst.Type = UI_CSS_BORDER_NONE;
        return true;
    case UIHash("hidden"): dst.Type = UI_CSS_BORDER_HIDDEN;
        return true;
    case UIHash("dotted"): dst.Type = UI_CSS_BORDER_DOTTED;
        return true;
    case UIHash("dashed"): dst.Type = UI_CSS_BORDER_DASHED;
        return true;
    case UIHash("solid"): dst.Type = UI_CSS_BORDER_SOLID;
        return true;
    case UIHash("double"): dst.Type = UI_CSS_BORDER_DOUBLE;
        return true;
    case UIHash("groove"): dst.Type = UI_CSS_BORDER_GROOVE;
        return true;
    case UIHash("ridge"): dst.Type = UI_CSS_BORDER_RIDGE;
        return true;
    case UIHash("inset"): dst.Type = UI_CSS_BORDER_INSET;
        return true;
    case UIHash("outset"): dst.Type = UI_CSS_BORDER_OUTSET;
        return true;
    }
    dst.Type = UI_CSS_BORDER_LENGTH;
    dst.Value = std::stof(src);
    return true;
}
template <>
inline bool UITypeC(UIPropBorderLeft const& src, UIString& dst)
{
    switch (src.Type)
    {
    default: return false;
    case UI_CSS_BORDER_THIN: dst = "thin";
        return true;
    case UI_CSS_BORDER_MEDIUM: dst = "medium";
        return true;
    case UI_CSS_BORDER_THICK: dst = "thick";
        return true;
    case UI_CSS_BORDER_NONE: dst = "none";
        return true;
    case UI_CSS_BORDER_HIDDEN: dst = "hidden";
        return true;
    case UI_CSS_BORDER_DOTTED: dst = "dotted";
        return true;
    case UI_CSS_BORDER_DASHED: dst = "dashed";
        return true;
    case UI_CSS_BORDER_SOLID: dst = "solid";
        return true;
    case UI_CSS_BORDER_DOUBLE: dst = "double";
        return true;
    case UI_CSS_BORDER_GROOVE: dst = "groove";
        return true;
    case UI_CSS_BORDER_RIDGE: dst = "ridge";
        return true;
    case UI_CSS_BORDER_INSET: dst = "inset";
        return true;
    case UI_CSS_BORDER_OUTSET: dst = "outset";
        return true;
    case UI_CSS_BORDER_LENGTH: dst = std::to_string(src.Value) + "px";
        return true;
    }
}

template <>
inline bool UITypeC(UIString const& src, UIPropBorderRight& dst)
{
    auto s = UITrim(src);
    switch (UIHash(s))
    {
    default: break;
    case UIHash("thin"): dst.Type = UI_CSS_BORDER_THIN;
        return true;
    case UIHash("medium"): dst.Type = UI_CSS_BORDER_MEDIUM;
        return true;
    case UIHash("thick"): dst.Type = UI_CSS_BORDER_THICK;
        return true;
    case UIHash("none"): dst.Type = UI_CSS_BORDER_NONE;
        return true;
    case UIHash("hidden"): dst.Type = UI_CSS_BORDER_HIDDEN;
        return true;
    case UIHash("dotted"): dst.Type = UI_CSS_BORDER_DOTTED;
        return true;
    case UIHash("dashed"): dst.Type = UI_CSS_BORDER_DASHED;
        return true;
    case UIHash("solid"): dst.Type = UI_CSS_BORDER_SOLID;
        return true;
    case UIHash("double"): dst.Type = UI_CSS_BORDER_DOUBLE;
        return true;
    case UIHash("groove"): dst.Type = UI_CSS_BORDER_GROOVE;
        return true;
    case UIHash("ridge"): dst.Type = UI_CSS_BORDER_RIDGE;
        return true;
    case UIHash("inset"): dst.Type = UI_CSS_BORDER_INSET;
        return true;
    case UIHash("outset"): dst.Type = UI_CSS_BORDER_OUTSET;
        return true;
    }
    dst.Type = UI_CSS_BORDER_LENGTH;
    dst.Value = std::stof(src);
    return true;
}
template <>
inline bool UITypeC(UIPropBorderRight const& src, UIString& dst)
{
    switch (src.Type)
    {
    default: return false;
    case UI_CSS_BORDER_THIN: dst = "thin";
        return true;
    case UI_CSS_BORDER_MEDIUM: dst = "medium";
        return true;
    case UI_CSS_BORDER_THICK: dst = "thick";
        return true;
    case UI_CSS_BORDER_NONE: dst = "none";
        return true;
    case UI_CSS_BORDER_HIDDEN: dst = "hidden";
        return true;
    case UI_CSS_BORDER_DOTTED: dst = "dotted";
        return true;
    case UI_CSS_BORDER_DASHED: dst = "dashed";
        return true;
    case UI_CSS_BORDER_SOLID: dst = "solid";
        return true;
    case UI_CSS_BORDER_DOUBLE: dst = "double";
        return true;
    case UI_CSS_BORDER_GROOVE: dst = "groove";
        return true;
    case UI_CSS_BORDER_RIDGE: dst = "ridge";
        return true;
    case UI_CSS_BORDER_INSET: dst = "inset";
        return true;
    case UI_CSS_BORDER_OUTSET: dst = "outset";
        return true;
    case UI_CSS_BORDER_LENGTH: dst = std::to_string(src.Value) + "px";
        return true;
    }
}

// Box sizing
template <>
inline bool UITypeC(UIString const& src, UIPropBoxSizing& dst)
{
    switch (UIHash(src))
    {
    default: return false;
    case UIHash("content-box"): dst.Value = UI_CSS_BOX_SIZING_CONTENT_BOX;
        break;
    case UIHash("border-box"): dst.Value = UI_CSS_BOX_SIZING_BORDER_BOX;
        break;
    }
    return true;
}

template <>
inline bool UITypeC(UIPropBoxSizing const& src, UIString& dst)
{
    switch (src.Value)
    {
    default: return false;
    case UI_CSS_BOX_SIZING_CONTENT_BOX: dst = "content-box";
        return true;
    case UI_CSS_BOX_SIZING_BORDER_BOX: dst = "border-box";
        return true;
    }
}

// Clear
template <>
inline bool UITypeC(UIString const& src, UIPropClear& dst)
{
    switch (UIHash(src))
    {
    default: return false;
    case UIHash("inline-start"): dst.Value = UI_CSS_CLEAR_INLINE_START;
        break;
    case UIHash("inline-end"): dst.Value = UI_CSS_CLEAR_INLINE_END;
        break;
    case UIHash("block-start"): dst.Value = UI_CSS_CLEAR_BLOCK_START;
        break;
    case UIHash("block-end"): dst.Value = UI_CSS_CLEAR_BLOCK_END;
        break;
    case UIHash("left"): dst.Value = UI_CSS_CLEAR_LEFT;
        break;
    case UIHash("right"): dst.Value = UI_CSS_CLEAR_TOP;
        break;
    case UIHash("top"): dst.Value = UI_CSS_CLEAR_TOP;
        break;
    case UIHash("bottom"): dst.Value = UI_CSS_CLEAR_BOTTOM;
        break;
    case UIHash("none"): dst.Value = UI_CSS_CLEAR_NONE;
        break;
    }
    return true;
}
template <>
inline bool UITypeC(UIPropClear const& src, UIString& dst)
{
    switch (src.Value)
    {
    default: return false;
    case UI_CSS_CLEAR_INLINE_START: dst = "inline-start";
        return true;
    case UI_CSS_CLEAR_INLINE_END: dst = "inline-end";
        return true;
    case UI_CSS_CLEAR_BLOCK_START: dst = "block-start";
        return true;
    case UI_CSS_CLEAR_BLOCK_END: dst = "block-end";
        return true;
    case UI_CSS_CLEAR_LEFT: dst = "left";
        return true;
    case UI_CSS_CLEAR_RIGHT: dst = "right";
        return true;
    case UI_CSS_CLEAR_TOP: dst = "top";
        return true;
    case UI_CSS_CLEAR_BOTTOM: dst = "bottom";
        return true;
    case UI_CSS_CLEAR_NONE: dst = "none";
        return true;
    }
}

template <>
inline bool UITypeC(UIString const& src, UIPropColor& dst)
{
    return std::sscanf(src.c_str(), "rgba(%hhu,%hhu,%hhu,%hhu)", &dst.Value.R, &dst.Value.G, &dst.Value.B, &dst.Value.A);
}
template <>
inline bool UITypeC(UIPropColor const& src, UIString& dst)
{
    char buffer[256] = {};
    auto result = std::snprintf(buffer, sizeof(buffer), "rgba(%hhu,%hhu,%hhu,%hhu)", src.Value.R, src.Value.G, src.Value.B, src.Value.A);
    if (result <= 0) return false;
    dst = {buffer, (size_t) result};
    return true;
}

// Direction
template <>
inline bool UITypeC(UIString const& src, UIPropDirection& dst)
{
    switch (UIHash(UITrim(src)))
    {
    default: return false;
    case UIHash("ltr"): dst.Value = UI_CSS_DIRECTION_LTR; return true;
    case UIHash("rtl"): dst.Value = UI_CSS_DIRECTION_RTL; return true;
    case UIHash("inherit"): dst.Value = UI_CSS_DIRECTION_INHERIT; return true;
    }
}
template <>
inline bool UITypeC(UIPropDirection const& src, UIString& dst)
{
    switch (src.Value)
    {
    default: return false;
    case UI_CSS_DIRECTION_LTR: dst = "ltr";
        return true;
    case UI_CSS_DIRECTION_RTL: dst = "rtl";
        return true;
    case UI_CSS_DIRECTION_INHERIT: dst = "inherit";
        return true;
    }
}

// Display
template <>
inline bool UITypeC(UIString const& src, UIPropDisplay& dst)
{
    auto s = UITrim(src);
    switch (UIHash(s))
    {
    default: return false;
    case UIHash("block"): dst.Value = UI_CSS_DISPLAY_BLOCK; return true;
    case UIHash("inline"): dst.Value = UI_CSS_DISPLAY_INLINE; return true;
    case UIHash("run-in"): dst.Value = UI_CSS_DISPLAY_RUN_IN; return true;
    case UIHash("flow-root"): dst.Value = UI_CSS_DISPLAY_FLOW_ROOT; return true;
    case UIHash("flow"): dst.Value = UI_CSS_DISPLAY_FLOW; return true;
    case UIHash("table"): dst.Value = UI_CSS_DISPLAY_TABLE; return true;
    case UIHash("flex"): dst.Value = UI_CSS_DISPLAY_FLEX; return true;
    case UIHash("grid"): dst.Value = UI_CSS_DISPLAY_GRID; return true;
    case UIHash("ruby"): dst.Value = UI_CSS_DISPLAY_RUBY; return true;
    case UIHash("list-item"): dst.Value = UI_CSS_DISPLAY_LIST_ITEM; return true;
    case UIHash("table-row-group"): dst.Value = UI_CSS_DISPLAY_TABLE_ROW_GROUP; return true;
    case UIHash("table-header-group"): dst.Value = UI_CSS_DISPLAY_TABLE_HEADER_GROUP; return true;
    case UIHash("table-footer-group"): dst.Value = UI_CSS_DISPLAY_TABLE_FOOTER_GROUP; return true;
    case UIHash("table-row"): dst.Value = UI_CSS_DISPLAY_TABLE_ROW; return true;
    case UIHash("table-cell"): dst.Value = UI_CSS_DISPLAY_TABLE_CELL; return true;
    case UIHash("table-column-group"): dst.Value = UI_CSS_DISPLAY_TABLE_COLUMN_GROUP; return true;
    case UIHash("table-column"): dst.Value = UI_CSS_DISPLAY_TABLE_COLUMN; return true;
    case UIHash("table-caption"): dst.Value = UI_CSS_DISPLAY_TABLE_CAPTION; return true;
    case UIHash("ruby-base"): dst.Value = UI_CSS_DISPLAY_RUBY_BASE; return true;
    case UIHash("ruby-text"): dst.Value = UI_CSS_DISPLAY_RUBY_TEXT; return true;
    case UIHash("ruby-base-container"): dst.Value = UI_CSS_DISPLAY_RUBY_BASE_CONTAINER; return true;
    case UIHash("ruby-text-container"): dst.Value = UI_CSS_DISPLAY_RUBY_TEXT_CONTAINER; return true;
    case UIHash("contents"): dst.Value = UI_CSS_DISPLAY_CONTENTS; return true;
    case UIHash("none"): dst.Value = UI_CSS_DISPLAY_NONE; return true;
    case UIHash("inline-block"): dst.Value = UI_CSS_DISPLAY_INLINE_BLOCK; return true;
    case UIHash("inline-table"): dst.Value = UI_CSS_DISPLAY_INLINE_TABLE; return true;
    case UIHash("inline-flex"): dst.Value = UI_CSS_DISPLAY_INLINE_FLEX; return true;
    case UIHash("inline-grid"): dst.Value = UI_CSS_DISPLAY_INLINE_GRID; return true;
    }
}
inline bool UITypeC(UIPropDisplay const& src, UIString& dst)
{
    switch (src.Value)
    {
    default: return false;
    case UI_CSS_DISPLAY_BLOCK: dst = "block"; return true;
    case UI_CSS_DISPLAY_INLINE: dst = "inline"; return true;
    case UI_CSS_DISPLAY_RUN_IN: dst = "run-in"; return true;
    case UI_CSS_DISPLAY_FLOW: dst = "flow"; return true;
    case UI_CSS_DISPLAY_FLOW_ROOT: dst = "flow-root"; return true;
    case UI_CSS_DISPLAY_TABLE: dst = "table"; return true;
    case UI_CSS_DISPLAY_FLEX: dst = "flex"; return true;
    case UI_CSS_DISPLAY_GRID: dst = "grid"; return true;
    case UI_CSS_DISPLAY_RUBY: dst = "ruby"; return true;
    case UI_CSS_DISPLAY_LIST_ITEM: dst = "list-item"; return true;
    case UI_CSS_DISPLAY_TABLE_ROW_GROUP: dst = "table-row-group"; return true;
    case UI_CSS_DISPLAY_TABLE_HEADER_GROUP: dst = "table-header-group"; return true;
    case UI_CSS_DISPLAY_TABLE_FOOTER_GROUP: dst = "table-footer-group"; return true;
    case UI_CSS_DISPLAY_TABLE_ROW: dst = "table-row"; return true;
    case UI_CSS_DISPLAY_TABLE_CELL: dst = "table-cell"; return true;
    case UI_CSS_DISPLAY_TABLE_COLUMN_GROUP: dst = "table-column-group"; return true;
    case UI_CSS_DISPLAY_TABLE_COLUMN: dst = "table-column"; return true;
    case UI_CSS_DISPLAY_TABLE_CAPTION: dst = "table-caption"; return true;
    case UI_CSS_DISPLAY_RUBY_BASE: dst = "ruby-base"; return true;
    case UI_CSS_DISPLAY_RUBY_TEXT: dst = "ruby-text"; return true;
    case UI_CSS_DISPLAY_RUBY_BASE_CONTAINER: dst = "ruby-base-container"; return true;
    case UI_CSS_DISPLAY_RUBY_TEXT_CONTAINER: dst = "ruby-text-container"; return true;
    case UI_CSS_DISPLAY_CONTENTS: dst = "contents"; return true;
    case UI_CSS_DISPLAY_NONE: dst = "none"; return true;
    case UI_CSS_DISPLAY_INLINE_BLOCK: dst = "inline-block"; return true;
    case UI_CSS_DISPLAY_INLINE_TABLE: dst = "inline-table"; return true;
    case UI_CSS_DISPLAY_INLINE_FLEX: dst = "inline-flex"; return true;
    case UI_CSS_DISPLAY_INLINE_GRID: dst = "inline-grid"; return true;
    }
}

// Flex Basis (supports content and numeric)
template <>
inline bool UITypeC(UIString const& src, UIPropFlexBasis& dst)
{
    return UIParseFlexBasis(src, dst);
}
template <>
inline bool UITypeC(UIPropFlexBasis const& src, UIString& dst)
{
    switch (src.Type)
    {
    default: return false;
    case UI_CSS_FLEX_BASIS_AUTO: dst = "auto";
        return true;
    case UI_CSS_FLEX_BASIS_LENGTH: dst = std::to_string(src.Value) + "px";
        return true;
    case UI_CSS_FLEX_BASIS_PERCENTAGE: dst = std::to_string(src.Value) + "%";
        return true;
    case UI_CSS_FLEX_BASIS_MIN_CONTENT: dst = "min-content";
        return true;
    case UI_CSS_FLEX_BASIS_MAX_CONTENT: dst = "max-content";
        return true;
    case UI_CSS_FLEX_BASIS_FIT_CONTENT: dst = "fit-content";
        return true;
    case UI_CSS_FLEX_BASIS_STRETCH: dst = "stretch";
        return true;
    }
}

// Flex Grow/Shrink
template <>
inline bool UITypeC(UIString const& src, UIPropFlexGrow& dst)
{
    dst.Type = UI_CSS_FLEX_GROW_NUMBER;
    dst.Value = std::stof(src);
    return true;
}
template <>
inline bool UITypeC(UIPropFlexGrow const& src, UIString& dst)
{
    dst = std::to_string(src.Value);
    return true;
}

template <>
inline bool UITypeC(UIString const& src, UIPropFlexShrink& dst)
{
    dst.Type = UI_CSS_FLEX_SHRINK_NUMBER;
    dst.Value = std::stof(src);
    return true;
}
template <>
inline bool UITypeC(UIPropFlexShrink const& src, UIString& dst)
{
    dst = std::to_string(src.Value);
    return true;
}

// Float offset (length/percentage)
template <>
inline bool UITypeC(UIString const& src, UIPropFloatOffset& dst)
{
    auto s = UITrim(src);
    if (s.ends_with('%'))
    {
        dst.Type = UI_CSS_FLOAT_OFFSET_PERCENTAGE;
        dst.Value = std::stof(s);
    }
    else
    {
        dst.Type = UI_CSS_FLOAT_OFFSET_LENGTH;
        dst.Value = std::stof(s);
    }
    return true;
}
template <>
inline bool UITypeC(UIPropFloatOffset const& src, UIString& dst)
{
    switch (src.Type)
    {
    default: return false;
    case UI_CSS_FLOAT_OFFSET_LENGTH: dst = std::to_string(src.Value) + "px";
        return true;
    case UI_CSS_FLOAT_OFFSET_PERCENTAGE: dst = std::to_string(src.Value) + "%";
        return true;
    }
}

// Font size
template <>
inline bool UITypeC(UIString const& src, UIPropFontSize& dst)
{
    return false;
}
template <>
inline bool UITypeC(UIPropFontSize const& src, UIString& dst)
{
    return false;
}

// Font stretch: accept percentage
template <>
inline bool UITypeC(UIString const& src, UIPropFontStretch& dst)
{
    return false;
}
template <>
inline bool UITypeC(UIPropFontStretch const& src, UIString& dst)
{
    return false;
}

// Font weight (number)
template <>
inline bool UITypeC(UIString const& src, UIPropFontWeight& dst)
{
    return false;
}
template <>
inline bool UITypeC(UIPropFontWeight const& src, UIString& dst)
{
    return false;
}

// Insets (block/inline start/end) - expanded (no macros)
template<>
inline bool UITypeC(UIString const& src, UIPropInsetBlockEnd& dst)
{
    auto s = UITrim(src);
    if (s == "auto")
    {
        dst.Type = UI_CSS_INSET_BLOCK_END_AUTO;
    }
    else if (s.ends_with('%'))
    {
        dst.Type = UI_CSS_INSET_BLOCK_END_PERCENTAGE;
        dst.Value = std::stof(s);
    }
    else
    {
        dst.Type = UI_CSS_INSET_BLOCK_END_LENGTH;
        dst.Value = std::stof(s);
    }
    return true;
}
template<>
inline bool UITypeC(UIPropInsetBlockEnd const& src, UIString& dst)
{
    switch (src.Type) {
    default: return false;
    case UI_CSS_INSET_BLOCK_END_AUTO: dst = "auto"; return true;
    case UI_CSS_INSET_BLOCK_END_LENGTH: dst = std::to_string(src.Value) + "px"; return true;
    case UI_CSS_INSET_BLOCK_END_PERCENTAGE: dst = std::to_string(src.Value) + "%"; return true;
    }
}

template<>
inline bool UITypeC(UIString const& src, UIPropInsetBlockStart& dst)
{
    auto s = UITrim(src);
    if (s == "auto")
    {
        dst.Type = UI_CSS_INSET_BLOCK_START_AUTO;
    }
    else if (s.ends_with('%'))
    {
        dst.Type = UI_CSS_INSET_BLOCK_START_PERCENTAGE;
        dst.Value = std::stof(s);
    }
    else
    {
        dst.Type = UI_CSS_INSET_BLOCK_START_LENGTH;
        dst.Value = std::stof(s);
    }
    return true;
}
template<>
inline bool UITypeC(UIPropInsetBlockStart const& src, UIString& dst)
{
    switch (src.Type) {
    default: return false;
    case UI_CSS_INSET_BLOCK_START_AUTO: dst = "auto"; return true;
    case UI_CSS_INSET_BLOCK_START_LENGTH: dst = std::to_string(src.Value) + "px"; return true;
    case UI_CSS_INSET_BLOCK_START_PERCENTAGE: dst = std::to_string(src.Value) + "%"; return true;
    }
}

template<>
inline bool UITypeC(UIString const& src, UIPropInsetInlineEnd& dst)
{
    auto s = UITrim(src);
    if (s == "auto")
    {
        dst.Type = UI_CSS_INSET_INLINE_END_AUTO;
    }
    else if (s.ends_with('%'))
    {
        dst.Type = UI_CSS_INSET_INLINE_END_PERCENTAGE;
        dst.Value = std::stof(s);
    }
    else
    {
        dst.Type = UI_CSS_INSET_INLINE_END_LENGTH;
        dst.Value = std::stof(s);
    }
    return true;
}
template<>
inline bool UITypeC(UIPropInsetInlineEnd const& src, UIString& dst)
{
    switch (src.Type) {
    default: return false;
    case UI_CSS_INSET_INLINE_END_AUTO: dst = "auto"; return true;
    case UI_CSS_INSET_INLINE_END_LENGTH: dst = std::to_string(src.Value) + "px"; return true;
    case UI_CSS_INSET_INLINE_END_PERCENTAGE: dst = std::to_string(src.Value) + "%"; return true;
    }
}

template<>
inline bool UITypeC(UIString const& src, UIPropInsetInlineStart& dst)
{
    auto s = UITrim(src);
    if (s == "auto")
    {
        dst.Type = UI_CSS_INSET_INLINE_START_AUTO;
    }
    else if (s.ends_with('%'))
    {
        dst.Type = UI_CSS_INSET_INLINE_START_PERCENTAGE;
        dst.Value = std::stof(s);
    }
    else
    {
        dst.Type = UI_CSS_INSET_INLINE_START_LENGTH;
        dst.Value = std::stof(s);
    }
    return true;
}
template<>
inline bool UITypeC(UIPropInsetInlineStart const& src, UIString& dst)
{
    switch (src.Type) {
    default: return false;
    case UI_CSS_INSET_INLINE_START_AUTO: dst = "auto"; return true;
    case UI_CSS_INSET_INLINE_START_LENGTH: dst = std::to_string(src.Value) + "px"; return true;
    case UI_CSS_INSET_INLINE_START_PERCENTAGE: dst = std::to_string(src.Value) + "%"; return true;
    }
}

// Left/Top/Top/Width (length/percentage) - expanded (no macros)
template<>
inline bool UITypeC(UIString const& src, UIPropLeft& dst)
{
    auto s = UITrim(src);
    if (s == "auto")
    {
        dst.Type = UI_CSS_LEFT_AUTO;
    }
    else if (s.ends_with('%'))
    {
        dst.Type = UI_CSS_LEFT_PERCENTAGE;
        dst.Value = std::stof(s);
    }
    else
    {
        dst.Type = UI_CSS_LEFT_LENGTH;
        dst.Value = std::stof(s);
    }
    return true;
}
template<>
inline bool UITypeC(UIPropLeft const& src, UIString& dst)
{
    switch (src.Type) {
    default: return false;
    case UI_CSS_LEFT_AUTO: dst = "auto"; return true;
    case UI_CSS_LEFT_LENGTH: dst = std::to_string(src.Value) + "px"; return true;
    case UI_CSS_LEFT_PERCENTAGE: dst = std::to_string(src.Value) + "%"; return true;
    }
}
template<>
inline bool UITypeC(UIString const& src, UIPropRight& dst)
{
    auto s = UITrim(src);
    if (s == "auto")
    {
        dst.Type = UI_CSS_RIGHT_AUTO;
    }
    else if (s.ends_with('%'))
    {
        dst.Type = UI_CSS_RIGHT_PERCENTAGE;
        dst.Value = std::stof(s);
    }
    else
    {
        dst.Type = UI_CSS_RIGHT_LENGTH;
        dst.Value = std::stof(s);
    }
    return true;
}
template<>
inline bool UITypeC(UIPropRight const& src, UIString& dst)
{
    switch (src.Type) {
    default: return false;
    case UI_CSS_RIGHT_AUTO: dst = "auto"; return true;
    case UI_CSS_RIGHT_LENGTH: dst = std::to_string(src.Value) + "px"; return true;
    case UI_CSS_RIGHT_PERCENTAGE: dst = std::to_string(src.Value) + "%"; return true;
    }
}

template<>
inline bool UITypeC(UIString const& src, UIPropTop& dst)
{
    auto s = UITrim(src);
    if (s == "auto")
    {
        dst.Type = UI_CSS_TOP_AUTO;
    }
    else if (s.ends_with('%'))
    {
        dst.Type = UI_CSS_TOP_PERCENTAGE;
        dst.Value = std::stof(s);
    }
    else
    {
        dst.Type = UI_CSS_TOP_LENGTH;
        dst.Value = std::stof(s);
    }
    return true;
}
template<>
inline bool UITypeC(UIPropTop const& src, UIString& dst)
{
    switch (src.Type) {
    default: return false;
    case UI_CSS_TOP_AUTO: dst = "auto"; return true;
    case UI_CSS_TOP_LENGTH: dst = std::to_string(src.Value) + "px"; return true;
    case UI_CSS_TOP_PERCENTAGE: dst = std::to_string(src.Value) + "%"; return true;
    }
}

// Bottom (length/percentage)
template <>
inline bool UITypeC(UIString const& src, UIPropBottom& dst)
{
    auto s = UITrim(src);
    if (s == "auto")
    {
        dst.Type = UI_CSS_BOTTOM_AUTO;
    }
    else if (s.ends_with('%'))
    {
        dst.Type = UI_CSS_BOTTOM_PERCENTAGE;
        dst.Value = std::stof(s);
    }
    else
    {
        dst.Type = UI_CSS_BOTTOM_LENGTH;
        dst.Value = std::stof(s);
    }
    return true;
}
template <>
inline bool UITypeC(UIPropBottom const& src, UIString& dst)
{
    switch (src.Type)
    {
    default: return false;
    case UI_CSS_BOTTOM_AUTO: dst = "auto";
        return true;
    case UI_CSS_BOTTOM_LENGTH: dst = std::to_string(src.Value) + "px";
        return true;
    case UI_CSS_BOTTOM_PERCENTAGE: dst = std::to_string(src.Value) + "%";
        return true;
    }
}

template<>
inline bool UITypeC(UIString const& src, UIPropWidth& dst)
{
    UICssMeasure measure;
    if (UIParseMeasure(src, measure) == false) return false;
    if (measure.Kind == UI_CSS_MEASURE_PERCENT) { dst.Type = UI_CSS_WIDTH_PERCENTAGE; dst.Value = measure.Value; return true; }
    if (measure.Kind == UI_CSS_MEASURE_LENGTH) { dst.Type = UI_CSS_WIDTH_LENGTH; dst.Value = measure.Value; return true; }
    if (measure.Keyword == "auto") { dst.Type = UI_CSS_WIDTH_AUTO; return true; }
    if (measure.Keyword == "min-content") { dst.Type = UI_CSS_WIDTH_MIN_CONTENT; return true; }
    if (measure.Keyword == "max-content") { dst.Type = UI_CSS_WIDTH_MAX_CONTENT; return true; }
    if (measure.Keyword == "fit-content") { dst.Type = UI_CSS_WIDTH_FIT_CONTENT; return true; }
    if (measure.Keyword == "stretch") { dst.Type = UI_CSS_WIDTH_STRETCH; return true; }
    return false;
}
template<>
inline bool UITypeC(UIPropWidth const& src, UIString& dst)
{
    switch (src.Type) {
    default: return false;
    case UI_CSS_WIDTH_AUTO: dst = "auto"; return true;
    case UI_CSS_WIDTH_MIN_CONTENT: dst = "min-content"; return true;
    case UI_CSS_WIDTH_MAX_CONTENT: dst = "max-content"; return true;
    case UI_CSS_WIDTH_FIT_CONTENT: dst = "fit-content"; return true;
    case UI_CSS_WIDTH_STRETCH: dst = "stretch"; return true;
    case UI_CSS_WIDTH_LENGTH: dst = std::to_string(src.Value) + "px"; return true;
    case UI_CSS_WIDTH_PERCENTAGE: dst = std::to_string(src.Value) + "%"; return true;
    case UI_CSS_WIDTH_NUMBER: dst = std::to_string(src.Value); return true;
    }
}

// Height
template <>
inline bool UITypeC(UIString const& src, UIPropHeight& dst)
{
    UICssMeasure measure;
    if (UIParseMeasure(src, measure) == false) return false;
    if (measure.Kind == UI_CSS_MEASURE_PERCENT) { dst.Type = UI_CSS_HEIGHT_PERCENTAGE; dst.Value = measure.Value; return true; }
    if (measure.Kind == UI_CSS_MEASURE_LENGTH) { dst.Type = UI_CSS_HEIGHT_LENGTH; dst.Value = measure.Value; return true; }
    if (measure.Keyword == "auto") { dst.Type = UI_CSS_HEIGHT_AUTO; return true; }
    if (measure.Keyword == "min-content") { dst.Type = UI_CSS_HEIGHT_MIN_CONTENT; return true; }
    if (measure.Keyword == "max-content") { dst.Type = UI_CSS_HEIGHT_MAX_CONTENT; return true; }
    if (measure.Keyword == "fit-content") { dst.Type = UI_CSS_HEIGHT_FIT_CONTENT; return true; }
    if (measure.Keyword == "stretch") { dst.Type = UI_CSS_HEIGHT_STRETCH; return true; }
    return false;
}
template <>
inline bool UITypeC(UIPropHeight const& src, UIString& dst)
{
    switch (src.Type)
    {
    default: return false;
    case UI_CSS_HEIGHT_AUTO: dst = "auto";
        return true;
    case UI_CSS_HEIGHT_MIN_CONTENT: dst = "min-content";
        return true;
    case UI_CSS_HEIGHT_MAX_CONTENT: dst = "max-content";
        return true;
    case UI_CSS_HEIGHT_FIT_CONTENT: dst = "fit-content";
        return true;
    case UI_CSS_HEIGHT_STRETCH: dst = "stretch";
        return true;
    case UI_CSS_HEIGHT_LENGTH: dst = std::to_string(src.Value) + "px";
        return true;
    case UI_CSS_HEIGHT_PERCENTAGE: dst = std::to_string(src.Value) + "%";
        return true;
    }
}

// Min/Max width/height
template<>
inline bool UITypeC(UIString const& src, UIPropMinWidth& dst)
{
    UICssMeasure measure;
    if (UIParseMeasure(src, measure) == false) return false;
    if (measure.Kind == UI_CSS_MEASURE_PERCENT) { dst.Type = UI_CSS_MIN_WIDTH_PERCENTAGE; dst.Value = measure.Value; return true; }
    if (measure.Kind == UI_CSS_MEASURE_LENGTH) { dst.Type = UI_CSS_MIN_WIDTH_LENGTH; dst.Value = measure.Value; return true; }
    if (measure.Keyword == "auto") { dst.Type = UI_CSS_MIN_WIDTH_AUTO; return true; }
    if (measure.Keyword == "min-content") { dst.Type = UI_CSS_MIN_WIDTH_MIN_CONTENT; return true; }
    if (measure.Keyword == "max-content") { dst.Type = UI_CSS_MIN_WIDTH_MAX_CONTENT; return true; }
    if (measure.Keyword == "fit-content") { dst.Type = UI_CSS_MIN_WIDTH_FIT_CONTENT; return true; }
    if (measure.Keyword == "stretch") { dst.Type = UI_CSS_MIN_WIDTH_STRETCH; return true; }
    return false;
}
template<>
inline bool UITypeC(UIPropMinWidth const& src, UIString& dst)
{
    switch (src.Type) {
    default: return false;
    case UI_CSS_MIN_WIDTH_AUTO: dst = "auto"; return true;
    case UI_CSS_MIN_WIDTH_MIN_CONTENT: dst = "min-content"; return true;
    case UI_CSS_MIN_WIDTH_MAX_CONTENT: dst = "max-content"; return true;
    case UI_CSS_MIN_WIDTH_FIT_CONTENT: dst = "fit-content"; return true;
    case UI_CSS_MIN_WIDTH_STRETCH: dst = "stretch"; return true;
    case UI_CSS_MIN_WIDTH_LENGTH: dst = std::to_string(src.Value) + "px"; return true;
    case UI_CSS_MIN_WIDTH_PERCENTAGE: dst = std::to_string(src.Value) + "%"; return true;
    }
}

template<>
inline bool UITypeC(UIString const& src, UIPropMaxWidth& dst)
{
    UICssMeasure measure;
    if (UIParseMeasure(src, measure) == false) return false;
    if (measure.Kind == UI_CSS_MEASURE_PERCENT) { dst.Type = UI_CSS_MAX_WIDTH_PERCENTAGE; dst.Value = measure.Value; return true; }
    if (measure.Kind == UI_CSS_MEASURE_LENGTH) { dst.Type = UI_CSS_MAX_WIDTH_LENGTH; dst.Value = measure.Value; return true; }
    if (measure.Keyword == "none") { dst.Type = UI_CSS_MAX_WIDTH_NONE; return true; }
    if (measure.Keyword == "min-content") { dst.Type = UI_CSS_MAX_WIDTH_MIN_CONTENT; return true; }
    if (measure.Keyword == "max-content") { dst.Type = UI_CSS_MAX_WIDTH_MAX_CONTENT; return true; }
    if (measure.Keyword == "fit-content") { dst.Type = UI_CSS_MAX_WIDTH_FIT_CONTENT; return true; }
    if (measure.Keyword == "stretch") { dst.Type = UI_CSS_MAX_WIDTH_STRETCH; return true; }
    return false;
}
template<>
inline bool UITypeC(UIPropMaxWidth const& src, UIString& dst)
{
    switch (src.Type) {
    default: return false;
    case UI_CSS_MAX_WIDTH_NONE: dst = "none"; return true;
    case UI_CSS_MAX_WIDTH_MIN_CONTENT: dst = "min-content"; return true;
    case UI_CSS_MAX_WIDTH_MAX_CONTENT: dst = "max-content"; return true;
    case UI_CSS_MAX_WIDTH_FIT_CONTENT: dst = "fit-content"; return true;
    case UI_CSS_MAX_WIDTH_STRETCH: dst = "stretch"; return true;
    case UI_CSS_MAX_WIDTH_LENGTH: dst = std::to_string(src.Value) + "px"; return true;
    case UI_CSS_MAX_WIDTH_PERCENTAGE: dst = std::to_string(src.Value) + "%"; return true;
    }
}

template<>
inline bool UITypeC(UIString const& src, UIPropMinHeight& dst)
{
    UICssMeasure measure;
    if (UIParseMeasure(src, measure) == false) return false;
    if (measure.Kind == UI_CSS_MEASURE_PERCENT) { dst.Type = UI_CSS_MIN_HEIGHT_PERCENTAGE; dst.Value = measure.Value; return true; }
    if (measure.Kind == UI_CSS_MEASURE_LENGTH) { dst.Type = UI_CSS_MIN_HEIGHT_LENGTH; dst.Value = measure.Value; return true; }
    if (measure.Keyword == "auto") { dst.Type = UI_CSS_MIN_HEIGHT_AUTO; return true; }
    if (measure.Keyword == "min-content") { dst.Type = UI_CSS_MIN_HEIGHT_MIN_CONTENT; return true; }
    if (measure.Keyword == "max-content") { dst.Type = UI_CSS_MIN_HEIGHT_MAX_CONTENT; return true; }
    if (measure.Keyword == "fit-content") { dst.Type = UI_CSS_MIN_HEIGHT_FIT_CONTENT; return true; }
    if (measure.Keyword == "stretch") { dst.Type = UI_CSS_MIN_HEIGHT_STRETCH; return true; }
    return false;
}
template<>
inline bool UITypeC(UIPropMinHeight const& src, UIString& dst)
{
    switch (src.Type) {
    default: return false;
    case UI_CSS_MIN_HEIGHT_AUTO: dst = "auto"; return true;
    case UI_CSS_MIN_HEIGHT_MIN_CONTENT: dst = "min-content"; return true;
    case UI_CSS_MIN_HEIGHT_MAX_CONTENT: dst = "max-content"; return true;
    case UI_CSS_MIN_HEIGHT_FIT_CONTENT: dst = "fit-content"; return true;
    case UI_CSS_MIN_HEIGHT_STRETCH: dst = "stretch"; return true;
    case UI_CSS_MIN_HEIGHT_LENGTH: dst = std::to_string(src.Value) + "px"; return true;
    case UI_CSS_MIN_HEIGHT_PERCENTAGE: dst = std::to_string(src.Value) + "%"; return true;
    }
}

template<>
inline bool UITypeC(UIString const& src, UIPropMaxHeight& dst)
{
    UICssMeasure measure;
    if (UIParseMeasure(src, measure) == false) return false;
    if (measure.Kind == UI_CSS_MEASURE_PERCENT) { dst.Type = UI_CSS_MAX_HEIGHT_PERCENTAGE; dst.Value = measure.Value; return true; }
    if (measure.Kind == UI_CSS_MEASURE_LENGTH) { dst.Type = UI_CSS_MAX_HEIGHT_LENGTH; dst.Value = measure.Value; return true; }
    if (measure.Keyword == "none") { dst.Type = UI_CSS_MAX_HEIGHT_NONE; return true; }
    if (measure.Keyword == "min-content") { dst.Type = UI_CSS_MAX_HEIGHT_MIN_CONTENT; return true; }
    if (measure.Keyword == "max-content") { dst.Type = UI_CSS_MAX_HEIGHT_MAX_CONTENT; return true; }
    if (measure.Keyword == "fit-content") { dst.Type = UI_CSS_MAX_HEIGHT_FIT_CONTENT; return true; }
    if (measure.Keyword == "stretch") { dst.Type = UI_CSS_MAX_HEIGHT_STRETCH; return true; }
    return false;
}
template<>
inline bool UITypeC(UIPropMaxHeight const& src, UIString& dst)
{
    switch (src.Type) {
    default: return false;
    case UI_CSS_MAX_HEIGHT_NONE: dst = "none"; return true;
    case UI_CSS_MAX_HEIGHT_MIN_CONTENT: dst = "min-content"; return true;
    case UI_CSS_MAX_HEIGHT_MAX_CONTENT: dst = "max-content"; return true;
    case UI_CSS_MAX_HEIGHT_FIT_CONTENT: dst = "fit-content"; return true;
    case UI_CSS_MAX_HEIGHT_STRETCH: dst = "stretch"; return true;
    case UI_CSS_MAX_HEIGHT_LENGTH: dst = std::to_string(src.Value) + "px"; return true;
    case UI_CSS_MAX_HEIGHT_PERCENTAGE: dst = std::to_string(src.Value) + "%"; return true;
    }
}

// Letter spacing, line-height, text-indent, tab-size, margins, paddings, max/min sizes, opacity, word-spacing - expanded (no macros)

template<>
inline bool UITypeC(UIString const& src, UIPropLetterSpacing& dst)
{
    auto s = UITrim(src);
    if (s == "normal")
    {
        dst.Type = UI_CSS_LETTER_SPACING_NORMAL;
    }
    else
    {
        dst.Type = UI_CSS_LETTER_SPACING_LENGTH;
        dst.Value = std::stof(s);
    }
    return true;
}
template<>
inline bool UITypeC(UIPropLetterSpacing const& src, UIString& dst)
{
    switch (src.Type) {
    default: return false;
    case UI_CSS_LETTER_SPACING_NORMAL: dst = "normal"; return true;
    case UI_CSS_LETTER_SPACING_LENGTH: dst = std::to_string(src.Value) + "px"; return true;
    }
}

template<>
inline bool UITypeC(UIString const& src, UIPropLineHeight& dst)
{
    auto s = UITrim(src);
    if (s == "normal")
    {
        dst.Type = UI_CSS_LINE_HEIGHT_NORMAL;
    }
    else if (s.ends_with('%'))
    {
        dst.Type = UI_CSS_LINE_HEIGHT_PERCENTAGE;
        dst.Value = std::stof(s);
    }
    else
    {
        dst.Type = UI_CSS_LINE_HEIGHT_LENGTH;
        dst.Value = std::stof(s);
    }
    return true;
}
template<>
inline bool UITypeC(UIPropLineHeight const& src, UIString& dst)
{
    switch (src.Type) {
    default: return false;
    case UI_CSS_LINE_HEIGHT_NORMAL: dst = "normal"; return true;
    case UI_CSS_LINE_HEIGHT_LENGTH: dst = std::to_string(src.Value) + "px"; return true;
    case UI_CSS_LINE_HEIGHT_PERCENTAGE: dst = std::to_string(src.Value) + "%"; return true;
    }
}

template<>
inline bool UITypeC(UIString const& src, UIPropTextIndent& dst)
{
    auto s = UITrim(src);
    if (s.ends_with('%'))
    {
        dst.Type = UI_CSS_TEXT_INDENT_PERCENTAGE;
        dst.Value = std::stof(s);
    }
    else
    {
        dst.Type = UI_CSS_TEXT_INDENT_LENGTH;
        dst.Value = std::stof(s);
    }
    return true;
}
template<>
inline bool UITypeC(UIPropTextIndent const& src, UIString& dst)
{
    switch (src.Type) {
    default: return false;
    case UI_CSS_TEXT_INDENT_LENGTH: dst = std::to_string(src.Value) + "px"; return true;
    case UI_CSS_TEXT_INDENT_PERCENTAGE: dst = std::to_string(src.Value) + "%"; return true;
    }
}

template<>
inline bool UITypeC(UIString const& src, UIPropTabSize& dst)
{
    auto s = UITrim(src);
    dst.Type = UI_CSS_TAB_SIZE_LENGTH;
    dst.Value = std::stof(s);
    return true;
}
template<>
inline bool UITypeC(UIPropTabSize const& src, UIString& dst)
{
    switch (src.Type) {
    default: return false;
    case UI_CSS_TAB_SIZE_NUMBER: dst = std::to_string(src.Value); return true;
    case UI_CSS_TAB_SIZE_LENGTH: dst = std::to_string(src.Value) + "px"; return true;
    }
}

template<>
inline bool UITypeC(UIString const& src, UIPropMargin& dst)
{
    auto parseSide = [](UIString const& token, float& value, ui_css_margin_type_t& type) -> bool
    {
        auto s = UITrim(token);
        if (s == "auto") { type = UI_CSS_MARGIN_AUTO; value = 0; return true; }
        UICssMeasure measure;
        if (UIParseMeasure(s, measure) == false) return false;
        if (measure.Kind == UI_CSS_MEASURE_PERCENT) { type = UI_CSS_MARGIN_PERCENTAGE; value = measure.Value; return true; }
        if (measure.Kind == UI_CSS_MEASURE_LENGTH) { type = UI_CSS_MARGIN_LENGTH; value = measure.Value; return true; }
        return false;
    };
    auto tokens = UISplitCssTokens(src);
    if (tokens.empty() || tokens.size() > 4) return false;
    float values[4] = {};
    ui_css_margin_type_t types[4] = {};
    for (size_t i = 0; i < tokens.size(); ++i)
        if (parseSide(tokens[i], values[i], types[i]) == false) return false;
    auto right = tokens.size() > 1 ? 1 : 0;
    auto bottom = tokens.size() > 2 ? 2 : 0;
    auto left = tokens.size() > 3 ? 3 : right;
    dst.Top = {values[0], types[0]};
    dst.Right = {values[right], types[right]};
    dst.Bottom = {values[bottom], types[bottom]};
    dst.Left = {values[left], types[left]};
    return true;
}
template<>
inline bool UITypeC(UIPropMargin const& src, UIString& dst)
{
    auto formatSide = [](float value, ui_css_margin_type_t type) -> UIString
    {
        if (type == UI_CSS_MARGIN_AUTO) return "auto";
        if (type == UI_CSS_MARGIN_PERCENTAGE) return std::to_string(value) + "%";
        return std::to_string(value) + "px";
    };
    dst = formatSide(src.Top.Value, src.Top.Type) + " " + formatSide(src.Right.Value, src.Right.Type) + " " + formatSide(src.Bottom.Value, src.Bottom.Type) + " " + formatSide(src.Left.Value, src.Left.Type);
    return true;
}

template<>
inline bool UITypeC(UIString const& src, UIPropMarginLeft& dst)
{
    auto s = UITrim(src);
    if (s == "auto")
    {
        dst.Type = UI_CSS_MARGIN_AUTO;
    }
    else if (s.ends_with('%'))
    {
        dst.Type = UI_CSS_MARGIN_PERCENTAGE;
        dst.Value = std::stof(s);
    }
    else
    {
        dst.Type = UI_CSS_MARGIN_LENGTH;
        dst.Value = std::stof(s);
    }
    return true;
}
template<>
inline bool UITypeC(UIPropMarginLeft const& src, UIString& dst)
{
    switch (src.Type) {
    default: return false;
    case UI_CSS_MARGIN_AUTO: dst = "auto"; return true;
    case UI_CSS_MARGIN_LENGTH: dst = std::to_string(src.Value) + "px"; return true;
    case UI_CSS_MARGIN_PERCENTAGE: dst = std::to_string(src.Value) + "%"; return true;
    }
}

template<>
inline bool UITypeC(UIString const& src, UIPropMarginRight& dst)
{
    auto s = UITrim(src);
    if (s == "auto")
    {
        dst.Type = UI_CSS_MARGIN_AUTO;
    }
    else if (s.ends_with('%'))
    {
        dst.Type = UI_CSS_MARGIN_PERCENTAGE;
        dst.Value = std::stof(s);
    }
    else
    {
        dst.Type = UI_CSS_MARGIN_LENGTH;
        dst.Value = std::stof(s);
    }
    return true;
}
template<>
inline bool UITypeC(UIPropMarginRight const& src, UIString& dst)
{
    switch (src.Type) {
    default: return false;
    case UI_CSS_MARGIN_AUTO: dst = "auto"; return true;
    case UI_CSS_MARGIN_LENGTH: dst = std::to_string(src.Value) + "px"; return true;
    case UI_CSS_MARGIN_PERCENTAGE: dst = std::to_string(src.Value) + "%"; return true;
    }
}

template<>
inline bool UITypeC(UIString const& src, UIPropMarginTop& dst)
{
    auto s = UITrim(src);
    if (s == "auto")
    {
        dst.Type = UI_CSS_MARGIN_AUTO;
    }
    else if (s.ends_with('%'))
    {
        dst.Type = UI_CSS_MARGIN_PERCENTAGE;
        dst.Value = std::stof(s);
    }
    else
    {
        dst.Type = UI_CSS_MARGIN_LENGTH;
        dst.Value = std::stof(s);
    }
    return true;
}
template<>
inline bool UITypeC(UIPropMarginTop const& src, UIString& dst)
{
    switch (src.Type) {
    default: return false;
    case UI_CSS_MARGIN_AUTO: dst = "auto"; return true;
    case UI_CSS_MARGIN_LENGTH: dst = std::to_string(src.Value) + "px"; return true;
    case UI_CSS_MARGIN_PERCENTAGE: dst = std::to_string(src.Value) + "%"; return true;
    }
}

template<>
inline bool UITypeC(UIString const& src, UIPropMarginBottom& dst)
{
    auto s = UITrim(src);
    if (s == "auto")
    {
        dst.Type = UI_CSS_MARGIN_AUTO;
    }
    else if (s.ends_with('%'))
    {
        dst.Type = UI_CSS_MARGIN_PERCENTAGE;
        dst.Value = std::stof(s);
    }
    else
    {
        dst.Type = UI_CSS_MARGIN_LENGTH;
        dst.Value = std::stof(s);
    }
    return true;
}
template<>
inline bool UITypeC(UIPropMarginBottom const& src, UIString& dst)
{
    switch (src.Type) {
    default: return false;
    case UI_CSS_MARGIN_AUTO: dst = "auto"; return true;
    case UI_CSS_MARGIN_LENGTH: dst = std::to_string(src.Value) + "px"; return true;
    case UI_CSS_MARGIN_PERCENTAGE: dst = std::to_string(src.Value) + "%"; return true;
    }
}

// Padding
template<>
inline bool UITypeC(UIString const& src, UIPropPadding& dst)
{
    auto parseSide = [](UIString const& token, float& value, ui_css_padding_type_t& type) -> bool
    {
        auto s = UITrim(token);
        if (s == "auto") { type = UI_CSS_PADDING_AUTO; value = 0; return true; }
        UICssMeasure measure;
        if (UIParseMeasure(s, measure) == false) return false;
        if (measure.Kind == UI_CSS_MEASURE_PERCENT) { type = UI_CSS_PADDING_PERCENTAGE; value = measure.Value; return true; }
        if (measure.Kind == UI_CSS_MEASURE_LENGTH) { type = UI_CSS_PADDING_LENGTH; value = measure.Value; return true; }
        return false;
    };
    auto tokens = UISplitCssTokens(src);
    if (tokens.empty() || tokens.size() > 4) return false;
    float values[4] = {};
    ui_css_padding_type_t types[4] = {};
    for (size_t i = 0; i < tokens.size(); ++i)
        if (parseSide(tokens[i], values[i], types[i]) == false) return false;
    auto right = tokens.size() > 1 ? 1 : 0;
    auto bottom = tokens.size() > 2 ? 2 : 0;
    auto left = tokens.size() > 3 ? 3 : right;
    dst.Top = {values[0], types[0]};
    dst.Right = {values[right], types[right]};
    dst.Bottom = {values[bottom], types[bottom]};
    dst.Left = {values[left], types[left]};
    return true;
}
template<>
inline bool UITypeC(UIPropPadding const& src, UIString& dst)
{
    auto formatSide = [](float value, ui_css_padding_type_t type) -> UIString
    {
        if (type == UI_CSS_PADDING_AUTO) return "auto";
        if (type == UI_CSS_PADDING_PERCENTAGE) return std::to_string(value) + "%";
        return std::to_string(value) + "px";
    };
    dst = formatSide(src.Top.Value, src.Top.Type) + " " + formatSide(src.Right.Value, src.Right.Type) + " " + formatSide(src.Bottom.Value, src.Bottom.Type) + " " + formatSide(src.Left.Value, src.Left.Type);
    return true;
}

template<>
inline bool UITypeC(UIString const& src, UIPropPaddingLeft& dst)
{
    auto s = UITrim(src);
    if (s.ends_with('%'))
    {
        dst.Type = UI_CSS_PADDING_PERCENTAGE;
        dst.Value = std::stof(s);
    }
    else
    {
        dst.Type = UI_CSS_PADDING_LENGTH;
        dst.Value = std::stof(s);
    }
    return true;
}
template<>
inline bool UITypeC(UIPropPaddingLeft const& src, UIString& dst)
{
    switch (src.Type) {
    default: return false;
    case UI_CSS_PADDING_LENGTH: dst = std::to_string(src.Value) + "px"; return true;
    case UI_CSS_PADDING_PERCENTAGE: dst = std::to_string(src.Value) + "%"; return true;
    }
}

template<>
inline bool UITypeC(UIString const& src, UIPropPaddingRight& dst)
{
    auto s = UITrim(src);
    if (s.ends_with('%'))
    {
        dst.Type = UI_CSS_PADDING_PERCENTAGE;
        dst.Value = std::stof(s);
    }
    else
    {
        dst.Type = UI_CSS_PADDING_LENGTH;
        dst.Value = std::stof(s);
    }
    return true;
}
template<>
inline bool UITypeC(UIPropPaddingRight const& src, UIString& dst)
{
    switch (src.Type) {
    default: return false;
    case UI_CSS_PADDING_LENGTH: dst = std::to_string(src.Value) + "px"; return true;
    case UI_CSS_PADDING_PERCENTAGE: dst = std::to_string(src.Value) + "%"; return true;
    }
}

template<>
inline bool UITypeC(UIString const& src, UIPropPaddingTop& dst)
{
    auto s = UITrim(src);
    if (s.ends_with('%'))
    {
        dst.Type = UI_CSS_PADDING_PERCENTAGE;
        dst.Value = std::stof(s);
    }
    else
    {
        dst.Type = UI_CSS_PADDING_LENGTH;
        dst.Value = std::stof(s);
    }
    return true;
}
template<>
inline bool UITypeC(UIPropPaddingTop const& src, UIString& dst)
{
    switch (src.Type) {
    default: return false;
    case UI_CSS_PADDING_LENGTH: dst = std::to_string(src.Value) + "px"; return true;
    case UI_CSS_PADDING_PERCENTAGE: dst = std::to_string(src.Value) + "%"; return true;
    }
}

template<>
inline bool UITypeC(UIString const& src, UIPropPaddingBottom& dst)
{
    auto s = UITrim(src);
    if (s.ends_with('%'))
    {
        dst.Type = UI_CSS_PADDING_PERCENTAGE;
        dst.Value = std::stof(s);
    }
    else
    {
        dst.Type = UI_CSS_PADDING_LENGTH;
        dst.Value = std::stof(s);
    }
    return true;
}
template<>
inline bool UITypeC(UIPropPaddingBottom const& src, UIString& dst)
{
    switch (src.Type) {
    default: return false;
    case UI_CSS_PADDING_LENGTH: dst = std::to_string(src.Value) + "px"; return true;
    case UI_CSS_PADDING_PERCENTAGE: dst = std::to_string(src.Value) + "%"; return true;
    }
}

// Overflow keywords (partial)
template <>
inline bool UITypeC(UIString const& src, UIPropOverflowX& dst)
{
    auto s = UITrim(src);
    switch (UIHash(s))
    {
    default: return false;
    case UIHash("visible"): dst.Value = UI_CSS_OVERFLOW_X_VISIBLE;
        return true;
    case UIHash("hidden"): dst.Value = UI_CSS_OVERFLOW_X_HIDDEN;
        return true;
    case UIHash("scroll"): dst.Value = UI_CSS_OVERFLOW_X_SCROLL;
        return true;
    case UIHash("auto"): dst.Value = UI_CSS_OVERFLOW_X_AUTO;
        return true;
    case UIHash("clip"): dst.Value = UI_CSS_OVERFLOW_X_CLIP;
        return true;
    }
}
template <>
inline bool UITypeC(UIPropOverflowX const& src, UIString& dst)
{
    switch (src.Value)
    {
    default: return false;
    case UI_CSS_OVERFLOW_X_VISIBLE: dst = "visible";
        return true;
    case UI_CSS_OVERFLOW_X_HIDDEN: dst = "hidden";
        return true;
    case UI_CSS_OVERFLOW_X_SCROLL: dst = "scroll";
        return true;
    case UI_CSS_OVERFLOW_X_AUTO: dst = "auto";
        return true;
    case UI_CSS_OVERFLOW_X_CLIP: dst = "clip";
        return true;
    }
}
template <>
inline bool UITypeC(UIString const& src, UIPropOverflowY& dst)
{
    auto s = UITrim(src);
    switch (UIHash(s))
    {
    default: return false;
    case UIHash("visible"): dst.Value = UI_CSS_OVERFLOW_Y_VISIBLE;
        return true;
    case UIHash("hidden"): dst.Value = UI_CSS_OVERFLOW_Y_HIDDEN;
        return true;
    case UIHash("scroll"): dst.Value = UI_CSS_OVERFLOW_Y_SCROLL;
        return true;
    case UIHash("auto"): dst.Value = UI_CSS_OVERFLOW_Y_AUTO;
        return true;
    case UIHash("clip"): dst.Value = UI_CSS_OVERFLOW_Y_CLIP;
        return true;
    }
}
template <>
inline bool UITypeC(UIPropOverflowY const& src, UIString& dst)
{
    switch (src.Value)
    {
    default: return false;
    case UI_CSS_OVERFLOW_Y_VISIBLE: dst = "visible";
        return true;
    case UI_CSS_OVERFLOW_Y_HIDDEN: dst = "hidden";
        return true;
    case UI_CSS_OVERFLOW_Y_SCROLL: dst = "scroll";
        return true;
    case UI_CSS_OVERFLOW_Y_AUTO: dst = "auto";
        return true;
    case UI_CSS_OVERFLOW_Y_CLIP: dst = "clip";
        return true;
    }
}

// Justify content
template <>
inline bool UITypeC(UIString const& src, UIPropJustifyContent& dst)
{
    switch (UIHash(UITrim(src)))
    {
    default: return false;
    case UIHash("flex-start"): dst.Value = UI_CSS_JUSTIFY_CONTENT_FLEX_START;
        break;
    case UIHash("flex-end"): dst.Value = UI_CSS_JUSTIFY_CONTENT_FLEX_END;
        break;
    case UIHash("center"): dst.Value = UI_CSS_JUSTIFY_CONTENT_CENTER;
        break;
    case UIHash("space-between"): dst.Value = UI_CSS_JUSTIFY_CONTENT_SPACE_BETWEEN;
        break;
    case UIHash("space-around"): dst.Value = UI_CSS_JUSTIFY_CONTENT_SPACE_AROUND;
        break;
    case UIHash("space-evenly"): dst.Value = UI_CSS_JUSTIFY_CONTENT_SPACE_EVENLY;
        break;
    case UIHash("start"): dst.Value = UI_CSS_JUSTIFY_CONTENT_START;
        break;
    case UIHash("end"): dst.Value = UI_CSS_JUSTIFY_CONTENT_END;
        break;
    case UIHash("stretch"): dst.Value = UI_CSS_JUSTIFY_CONTENT_STRETCH;
        break;
    case UIHash("auto"): dst.Value = UI_CSS_JUSTIFY_CONTENT_AUTO;
        break;
    }
    return true;
}
template <>
inline bool UITypeC(UIPropJustifyContent const& src, UIString& dst)
{
    switch (src.Value)
    {
    default: return false;
    case UI_CSS_JUSTIFY_CONTENT_FLEX_START: dst = "flex-start";
        return true;
    case UI_CSS_JUSTIFY_CONTENT_FLEX_END: dst = "flex-end";
        return true;
    case UI_CSS_JUSTIFY_CONTENT_CENTER: dst = "center";
        return true;
    case UI_CSS_JUSTIFY_CONTENT_SPACE_BETWEEN: dst = "space-between";
        return true;
    case UI_CSS_JUSTIFY_CONTENT_SPACE_AROUND: dst = "space-around";
        return true;
    case UI_CSS_JUSTIFY_CONTENT_SPACE_EVENLY: dst = "space-evenly";
        return true;
    case UI_CSS_JUSTIFY_CONTENT_START: dst = "start";
        return true;
    case UI_CSS_JUSTIFY_CONTENT_END: dst = "end";
        return true;
    case UI_CSS_JUSTIFY_CONTENT_STRETCH: dst = "stretch";
        return true;
    case UI_CSS_JUSTIFY_CONTENT_AUTO: dst = "auto";
        return true;
    }
}

// Remaining enum-only props: visibility, position, writing-mode, white-space, text-transform, etc. Implement a few common ones:

// Position
template <>
inline bool UITypeC(UIString const& src, UIPropPosition& dst)
{
    switch (UIHash(UITrim(src)))
    {
    default: return false;
    case UIHash("static"): dst.Value = UI_CSS_POSITION_STATIC; return true;
    case UIHash("relative"): dst.Value = UI_CSS_POSITION_RELATIVE; return true;
    case UIHash("absolute"): dst.Value = UI_CSS_POSITION_ABSOLUTE; return true;
    case UIHash("fixed"): dst.Value = UI_CSS_POSITION_FIXED; return true;
    case UIHash("sticky"): dst.Value = UI_CSS_POSITION_STICKY; return true;
    }
}

template <>
inline bool UITypeC(UIPropPosition const& src, UIString& dst)
{
    switch (src.Value)
    {
    default: return false;
    case UI_CSS_POSITION_STATIC: dst = "static";
        return true;
    case UI_CSS_POSITION_RELATIVE: dst = "relative";
        return true;
    case UI_CSS_POSITION_ABSOLUTE: dst = "absolute";
        return true;
    case UI_CSS_POSITION_FIXED: dst = "fixed";
        return true;
    case UI_CSS_POSITION_STICKY: dst = "sticky";
        return true;
    }
}

// Visibility
template <>
inline bool UITypeC(UIString const& src, UIPropVisibility& dst)
{
    switch (UIHash(src))
    {
    default: return false;
    case UIHash("visible"): dst.Value = UI_CSS_VISIBILITY_VISIBLE;
        return true;
    case UIHash("hidden"): dst.Value = UI_CSS_VISIBILITY_HIDDEN;
        return true;
    case UIHash("collapse"): dst.Value = UI_CSS_VISIBILITY_COLLAPSE;
        return true;
    }
}
template <>
inline bool UITypeC(UIPropVisibility const& src, UIString& dst)
{
    switch (src.Value)
    {
    default: return false;
    case UI_CSS_VISIBILITY_VISIBLE: dst = "visible";
        return true;
    case UI_CSS_VISIBILITY_HIDDEN: dst = "hidden";
        return true;
    case UI_CSS_VISIBILITY_COLLAPSE: dst = "collapse";
        return true;
    }
}

// Writing mode
template <>
inline bool UITypeC(UIString const& src, UIPropWritingMode& dst)
{
    switch (UIHash(src))
    {
    default: return false;
    case UIHash("horizontal-tb"): dst.Value = UI_CSS_WRITING_MODE_HORIZONTAL_TB;
        return true;
    case UIHash("vertical-rl"): dst.Value = UI_CSS_WRITING_MODE_VERTICAL_RL;
        return true;
    case UIHash("vertical-lr"): dst.Value = UI_CSS_WRITING_MODE_VERTICAL_LR;
        return true;
    }
}
template <>
inline bool UITypeC(UIPropWritingMode const& src, UIString& dst)
{
    switch (src.Value)
    {
    default: return false;
    case UI_CSS_WRITING_MODE_HORIZONTAL_TB: dst = "horizontal-tb";
        return true;
    case UI_CSS_WRITING_MODE_VERTICAL_RL: dst = "vertical-rl";
        return true;
    case UI_CSS_WRITING_MODE_VERTICAL_LR: dst = "vertical-lr";
        return true;
    }
}

// Z-index (number)
template <>
inline bool UITypeC(UIString const& src, UIPropZIndex& dst)
{
    auto s = UITrim(src);
    switch (UIHash(s))
    {
    default: break;
    case UIHash("auto"): dst.Type = UI_CSS_Z_INDEX_AUTO;
        return true;
    }
    dst.Type = UI_CSS_Z_INDEX_INTEGER;
    dst.Value = std::stoi(s);
    return true;
}
template <>
inline bool UITypeC(UIPropZIndex const& src, UIString& dst)
{
    switch (src.Type)
    {
    default: return false;
    case UI_CSS_Z_INDEX_AUTO: dst = "auto";
        return true;
    case UI_CSS_Z_INDEX_INTEGER: dst = std::to_string(src.Value);
        return true;
    }
}

// Filter (func)
template <>
inline bool UITypeC(UIString const& src, UIPropFilter& dst)
{
    auto s = UITrim(src);
    switch (UIHash(s))
    {
    default: break;
    case UIHash("none"): dst.Value = UI_CSS_FILTER_NONE;
        return true;
    }
    dst.Value = UI_CSS_FILTER_CUSTOM0;
    char buffer0[256] = {};
    char buffer1[256] = {};
    std::sscanf(src.c_str(), "%255[^(](%255[^)]", buffer0, buffer1);
    dst.Func = buffer0;
    dst.Args = buffer1;
    return true;
}
template <>
inline bool UITypeC(UIPropFilter const& src, UIString& dst)
{
    switch (src.Value)
    {
    default: return false;
    case UI_CSS_FILTER_NONE: dst = "none";
        return true;
    case UI_CSS_FILTER_CUSTOM0: dst = src.Func + "(" + src.Args + ")";
        return true;
    }
}

template <>
inline bool UITypeC(UIString const& src, UIPropFlexDirection& dst)
{
    switch (UIHash(UITrim(src)))
    {
    default: return false;
    case UIHash("row"): dst.Value = UI_CSS_FLEX_DIRECTION_ROW; return true;
    case UIHash("row-reverse"): dst.Value = UI_CSS_FLEX_DIRECTION_ROW_REVERSE; return true;
    case UIHash("column"): dst.Value = UI_CSS_FLEX_DIRECTION_COLUMN; return true;
    case UIHash("column-reverse"): dst.Value = UI_CSS_FLEX_DIRECTION_COLUMN_REVERSE; return true;
    }
}
template <>
inline bool UITypeC(UIPropFlexDirection const& src, UIString& dst)
{
    switch (src.Value)
    {
    default: return false;
    case UI_CSS_FLEX_DIRECTION_ROW: dst = "row"; return true;
    case UI_CSS_FLEX_DIRECTION_ROW_REVERSE: dst = "row-reverse"; return true;
    case UI_CSS_FLEX_DIRECTION_COLUMN: dst = "column"; return true;
    case UI_CSS_FLEX_DIRECTION_COLUMN_REVERSE: dst = "column-reverse"; return true;
    }
}

template <>
inline bool UITypeC(UIString const& src, UIPropFlexWrap& dst)
{
    switch (UIHash(UITrim(src)))
    {
    default: return false;
    case UIHash("nowrap"): dst.Value = UI_CSS_FLEX_WRAP_NOWRAP; return true;
    case UIHash("wrap"): dst.Value = UI_CSS_FLEX_WRAP_WRAP; return true;
    case UIHash("wrap-reverse"): dst.Value = UI_CSS_FLEX_WRAP_WRAP_REVERSE; return true;
    }
}
template <>
inline bool UITypeC(UIPropFlexWrap const& src, UIString& dst)
{
    switch (src.Value)
    {
    default: return false;
    case UI_CSS_FLEX_WRAP_NOWRAP: dst = "nowrap"; return true;
    case UI_CSS_FLEX_WRAP_WRAP: dst = "wrap"; return true;
    case UI_CSS_FLEX_WRAP_WRAP_REVERSE: dst = "wrap-reverse"; return true;
    }
}

template <>
inline bool UITypeC(UIString const& src, UIPropFlex& dst)
{
    auto tokens = UISplitCssTokens(src);
    if (tokens.size() == 1 && tokens[0] == "none")
    {
        dst.Grow.Value = 0;
        dst.Shrink.Value = 0;
        dst.Basis = {};
        return true;
    }
    if (tokens.size() == 1 && tokens[0] == "auto")
    {
        dst.Grow.Value = 1;
        dst.Shrink.Value = 1;
        dst.Basis = {};
        return true;
    }
    UIList<float> factors;
    UIString basis;
    for (auto const& token : tokens)
    {
        UICssMeasure measure;
        if (UIParseMeasure(token, measure) == false) return false;
        if (measure.Kind == UI_CSS_MEASURE_LENGTH && token.find('%') == UIString::npos && token.find("px") == UIString::npos && token.find("fr") == UIString::npos)
            factors.push_back(measure.Value);
        else
        {
            if (basis.empty() == false) return false;
            basis = token;
        }
    }
    dst.Grow.Value = 1;
    dst.Shrink.Value = 1;
    dst.Basis.Type = UI_CSS_FLEX_BASIS_LENGTH;
    dst.Basis.Value = 0;
    if (basis.empty() == false && UIParseFlexBasis(basis, dst.Basis) == false) return false;
    if (factors.size() == 1) dst.Grow.Value = factors[0];
    else if (factors.size() == 2)
    {
        dst.Grow.Value = factors[0];
        dst.Shrink.Value = factors[1];
    }
    else if (factors.empty() && basis.empty() == false)
    {
        dst.Grow.Value = 1;
        dst.Shrink.Value = 1;
    }
    else return false;
    if (basis.empty())
    {
        dst.Basis.Type = UI_CSS_FLEX_BASIS_LENGTH;
        dst.Basis.Value = 0;
    }
    return true;
}
template <>
inline bool UITypeC(UIPropFlex const& src, UIString& dst)
{
    if (src.Grow.Value == 0 && src.Shrink.Value == 0 && src.Basis.Type == UI_CSS_FLEX_BASIS_AUTO) { dst = "none"; return true; }
    if (src.Grow.Value == 1 && src.Shrink.Value == 1 && src.Basis.Type == UI_CSS_FLEX_BASIS_AUTO) { dst = "auto"; return true; }
    UIString basis;
    if (UITypeC(src.Basis, basis) == false) return false;
    dst = std::to_string(src.Grow.Value) + " " + std::to_string(src.Shrink.Value) + " " + basis;
    return true;
}

template <>
inline bool UITypeC(UIString const& src, UIPropJustifyItems& dst)
{
    UIPropJustifyContent content;
    if (UITypeC(src, content) == false) return false;
    dst.Value = content.Value;
    return true;
}
template <>
inline bool UITypeC(UIPropJustifyItems const& src, UIString& dst)
{
    return UITypeC(UIPropJustifyContent{src.Value}, dst);
}

template <>
inline bool UITypeC(UIString const& src, UIPropJustifySelf& dst)
{
    UIPropJustifyContent content;
    if (UITypeC(src, content) == false) return false;
    dst.Value = content.Value;
    return true;
}
template <>
inline bool UITypeC(UIPropJustifySelf const& src, UIString& dst)
{
    return UITypeC(UIPropJustifyContent{src.Value}, dst);
}

template <>
inline bool UITypeC(UIString const& src, UIPropAspectRatio& dst)
{
    auto s = UITrim(src);
    if (s == "auto")
    {
        dst.Auto = true;
        dst.Value = UINAN;
        return true;
    }
    auto slash = s.find('/');
    if (slash != UIString::npos)
    {
        auto left = std::stof(s.substr(0, slash));
        auto right = std::stof(s.substr(slash + 1));
        if (right == 0) return false;
        dst.Auto = false;
        dst.Value = left / right;
        return true;
    }
    dst.Auto = false;
    dst.Value = std::stof(s);
    return true;
}
template <>
inline bool UITypeC(UIPropAspectRatio const& src, UIString& dst)
{
    if (src.Auto) { dst = "auto"; return true; }
    dst = std::to_string(src.Value);
    return true;
}

template <>
inline bool UITypeC(UIString const& src, UIPropGridColumnGap& dst)
{
    return UIParseGap(src, dst.Value, dst.Type);
}
template <>
inline bool UITypeC(UIPropGridColumnGap const& src, UIString& dst)
{
    if (src.Type == UI_CSS_GRID_GAP_PERCENTAGE) dst = std::to_string(src.Value) + "%";
    else dst = std::to_string(src.Value) + "px";
    return true;
}

template <>
inline bool UITypeC(UIString const& src, UIPropGridRowGap& dst)
{
    return UIParseGap(src, dst.Value, dst.Type);
}
template <>
inline bool UITypeC(UIPropGridRowGap const& src, UIString& dst)
{
    if (src.Type == UI_CSS_GRID_GAP_PERCENTAGE) dst = std::to_string(src.Value) + "%";
    else dst = std::to_string(src.Value) + "px";
    return true;
}

template <>
inline bool UITypeC(UIString const& src, UIPropGap& dst)
{
    auto tokens = UISplitCssTokens(src);
    if (tokens.size() == 1)
    {
        if (UIParseGap(tokens[0], dst.Row.Value, dst.Row.Type) == false) return false;
        dst.Column.Value = dst.Row.Value;
        dst.Column.Type = dst.Row.Type;
        return true;
    }
    if (tokens.size() == 2)
    {
        if (UIParseGap(tokens[0], dst.Row.Value, dst.Row.Type) == false) return false;
        if (UIParseGap(tokens[1], dst.Column.Value, dst.Column.Type) == false) return false;
        return true;
    }
    return false;
}
template <>
inline bool UITypeC(UIPropGap const& src, UIString& dst)
{
    UIString row, column;
    if (UITypeC(src.Row, row) == false || UITypeC(src.Column, column) == false) return false;
    dst = row + " " + column;
    return true;
}

template <>
inline bool UITypeC(UIString const& src, UIPropGridTemplateColumns& dst)
{
    return UIParseGridTracks(src, dst.Tracks);
}
template <>
inline bool UITypeC(UIPropGridTemplateColumns const& src, UIString& dst)
{
    dst = UIFormatGridTracks(src.Tracks);
    return true;
}

template <>
inline bool UITypeC(UIString const& src, UIPropGridTemplateRows& dst)
{
    return UIParseGridTracks(src, dst.Tracks);
}
template <>
inline bool UITypeC(UIPropGridTemplateRows const& src, UIString& dst)
{
    dst = UIFormatGridTracks(src.Tracks);
    return true;
}

template <>
inline bool UITypeC(UIString const& src, UIPropGridAutoColumns& dst)
{
    return UIParseGridTracks(src, dst.Tracks);
}
template <>
inline bool UITypeC(UIPropGridAutoColumns const& src, UIString& dst)
{
    dst = UIFormatGridTracks(src.Tracks);
    return true;
}

template <>
inline bool UITypeC(UIString const& src, UIPropGridAutoRows& dst)
{
    return UIParseGridTracks(src, dst.Tracks);
}
template <>
inline bool UITypeC(UIPropGridAutoRows const& src, UIString& dst)
{
    dst = UIFormatGridTracks(src.Tracks);
    return true;
}

template <>
inline bool UITypeC(UIString const& src, UIPropGridColumnStart& dst)
{
    return UIParseGridLine(src, dst.Value, dst.Type);
}
template <>
inline bool UITypeC(UIPropGridColumnStart const& src, UIString& dst)
{
    dst = UIFormatGridLine(src.Type, src.Value);
    return true;
}

template <>
inline bool UITypeC(UIString const& src, UIPropGridColumnEnd& dst)
{
    return UIParseGridLine(src, dst.Value, dst.Type);
}
template <>
inline bool UITypeC(UIPropGridColumnEnd const& src, UIString& dst)
{
    dst = UIFormatGridLine(src.Type, src.Value);
    return true;
}

template <>
inline bool UITypeC(UIString const& src, UIPropGridRowStart& dst)
{
    return UIParseGridLine(src, dst.Value, dst.Type);
}
template <>
inline bool UITypeC(UIPropGridRowStart const& src, UIString& dst)
{
    dst = UIFormatGridLine(src.Type, src.Value);
    return true;
}

template <>
inline bool UITypeC(UIString const& src, UIPropGridRowEnd& dst)
{
    return UIParseGridLine(src, dst.Value, dst.Type);
}
template <>
inline bool UITypeC(UIPropGridRowEnd const& src, UIString& dst)
{
    dst = UIFormatGridLine(src.Type, src.Value);
    return true;
}

template <>
inline bool UITypeC(UIString const& src, UIPropGridColumn& dst)
{
    auto parts = UISplitCssSlash(src);
    if (parts.empty() || parts.size() > 2) return false;
    if (UIParseGridLine(parts[0], dst.Start.Value, dst.Start.Type) == false) return false;
    if (parts.size() == 1)
    {
        dst.End.Type = UI_CSS_GRID_LINE_AUTO;
        dst.End.Value = 0;
        return true;
    }
    return UIParseGridLine(parts[1], dst.End.Value, dst.End.Type);
}
template <>
inline bool UITypeC(UIPropGridColumn const& src, UIString& dst)
{
    dst = UIFormatGridLine(src.Start.Type, src.Start.Value) + " / " + UIFormatGridLine(src.End.Type, src.End.Value);
    return true;
}

template <>
inline bool UITypeC(UIString const& src, UIPropGridRow& dst)
{
    auto parts = UISplitCssSlash(src);
    if (parts.empty() || parts.size() > 2) return false;
    if (UIParseGridLine(parts[0], dst.Start.Value, dst.Start.Type) == false) return false;
    if (parts.size() == 1)
    {
        dst.End.Type = UI_CSS_GRID_LINE_AUTO;
        dst.End.Value = 0;
        return true;
    }
    return UIParseGridLine(parts[1], dst.End.Value, dst.End.Type);
}
template <>
inline bool UITypeC(UIPropGridRow const& src, UIString& dst)
{
    dst = UIFormatGridLine(src.Start.Type, src.Start.Value) + " / " + UIFormatGridLine(src.End.Type, src.End.Value);
    return true;
}

template <>
inline bool UITypeC(UIString const& src, UIPropOverflow& dst)
{
    auto tokens = UISplitCssTokens(src);
    if (tokens.empty() || tokens.size() > 2) return false;
    auto parseOne = [](UIString const& token, unsigned int& value) -> bool
    {
        switch (UIHash(UITrim(token)))
        {
        case UIHash("visible"): value = UI_CSS_OVERFLOW_X_VISIBLE; return true;
        case UIHash("hidden"): value = UI_CSS_OVERFLOW_X_HIDDEN; return true;
        case UIHash("clip"): value = UI_CSS_OVERFLOW_X_CLIP; return true;
        case UIHash("scroll"): value = UI_CSS_OVERFLOW_X_SCROLL; return true;
        case UIHash("auto"): value = UI_CSS_OVERFLOW_X_AUTO; return true;
        default: return false;
        }
    };
    unsigned int x = UI_CSS_OVERFLOW_X_VISIBLE;
    unsigned int y = UI_CSS_OVERFLOW_Y_VISIBLE;
    if (parseOne(tokens[0], x) == false) return false;
    if (tokens.size() == 2)
    {
        if (parseOne(tokens[1], y) == false) return false;
    }
    else y = x;
    dst.X.Value = x;
    dst.Y.Value = y;
    return true;
}
template <>
inline bool UITypeC(UIPropOverflow const& src, UIString& dst)
{
    UIString x, y;
    if (UITypeC(src.X, x) == false || UITypeC(src.Y, y) == false) return false;
    dst = x + " " + y;
    return true;
}

// Note: Not all properties' tokens/keywords are exhaustively implemented here. This provides full numeric parsing for length/percentage/number where applicable and common keyword mappings. Further refinements can be added per-property on request.


