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
    explicit UIStyleValue(T const& value) : m_Data(value)
    {
    }

    std::type_info const& getType() const override { return typeid(T); }

    UIString getText() const override
    {
        UIString result;
        UITypeC(m_Data, result);
        return result;
    }

    void setText(const UIString& text) override { UITypeC(text, m_Data); }
    void* getData(const std::type_info& type) override { return (type == typeid(T)) ? &m_Data : nullptr; }
    const void* getData(const std::type_info& type) const override { return (type == typeid(T)) ? &m_Data : nullptr; }

protected:
    T m_Data;
};

template <class T>
class UIStyleRefer : public UIStyleData
{
public:
    explicit UIStyleRefer(T& value) : m_Data(value)
    {
    }

    std::type_info const& getType() const override { return typeid(T); }

    UIString getText() const override
    {
        UIString result;
        UITypeC(m_Data, result);
        return result;
    }

    void setText(const UIString& text) override { UITypeC(text, m_Data); }
    void* getData(const std::type_info& type) override { return (type == typeid(T)) ? &m_Data : nullptr; }
    const void* getData(const std::type_info& type) const override { return (type == typeid(T)) ? &m_Data : nullptr; }

protected:
    T& m_Data;
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
    void setStyle(UIString const& key, T const& value)
    {
        auto result = getStyle(key);
        if (result) *(T*)result->getData(typeid(std::remove_cvref_t<T>)) = value;
        else this->setStyle(key, UICast<UIStyleData>(UINew<UIStyleValue<std::remove_cvref_t<T>>>(value)));
    }

    template <class T>
    void setEmbedStyle(UIString const& key, T& value)
    {
        this->setStyle(key, UICast<UIStyleData>(UINew<UIStyleRefer<std::remove_cvref_t<T>>>(value)));
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
    ui_css_alignment_baseline_type_t Value;
};

struct UIPropBaselineShift
{
    ui_css_baseline_shift_type_t Type;
    float Value;
};

struct UIPropBaselineSource
{
    ui_css_baseline_source_type_t Value;
};

struct UIPropBorder
{
    ui_css_border_type_t Type;
    float Value;
};

struct UIPropBorderTop
{
    ui_css_border_top_type_t Type;
    float Value;
};

struct UIPropBorderBottom
{
    ui_css_border_bottom_type_t Type;
    float Value;
};

struct UIPropBorderLeft
{
    ui_css_border_left_type_t Type;
    float Value;
};

struct UIPropBorderRight
{
    ui_css_border_right_type_t Type;
    float Value;
};

struct UIPropBoxSizing
{
    ui_css_box_sizing_type_t Value;
};

struct UIPropClear
{
    ui_css_clear_type_t Value;
};

struct UIPropColor
{
    ui_css_color_type_t Value;
};

struct UIPropDirection
{
    ui_css_direction_type_t Value;
};

struct UIPropDisplay
{
    ui_css_display_type_t Value = UI_CSS_DISPLAY_INLINE;
};

struct UIPropDominantBaseline
{
    ui_css_dominant_baseline_type_t Value;
};

struct UIPropFlex
{
    ui_css_flex_type_t Value;
};

struct UIPropFlexBasis
{
    ui_css_flex_basis_type_t Type = UI_CSS_FLEX_BASIS_CONTENT;
    float Value = UINAN;
};

struct UIPropFlexDirection
{
    ui_css_flex_direction_type_t Value = UI_CSS_FLEX_DIRECTION_ROW;
};

struct UIPropFlexGrow
{
    ui_css_flex_grow_type_t Type = UI_CSS_FLEX_GROW__NUMBER;
    float Value = 0;
};

struct UIPropFlexShrink
{
    ui_css_flex_shrink_type_t Type = UI_CSS_FLEX_SHRINK__NUMBER;
    float Value = 1;
};

struct UIPropFlexWrap
{
    ui_css_flex_wrap_type_t Value = UI_CSS_FLEX_WRAP_NOWRAP;
};

struct UIPropFloat
{
    ui_css_float_type_t Value;
};

struct UIPropFloatDefer
{
    ui_css_float_defer_type_t Value;
};

struct UIPropFloatOffset
{
    ui_css_float_offset_type_t Type;
    float Value;
};

struct UIPropFloatReference
{
    ui_css_float_reference_type_t Value;
};

struct UIPropFontFamily
{
    ui_css_font_family_type_t Value;
};

struct UIPropFontSize
{
    ui_css_font_size_type_t Type;
    float Value;
};

struct UIPropFontStretch
{
    ui_css_font_stretch_type_t Type;
    float Value;
};

struct UIPropFontStyle
{
    ui_css_font_style_type_t Value;
};

struct UIPropFontWeight
{
    ui_css_font_weight_type_t Type;
    float Value;
};

struct UIPropHangingPunctuation
{
    ui_css_hanging_punctuation_type_t Value;
};

struct UIPropHeight
{
    ui_css_height_type_t Type = UI_CSS_HEIGHT_AUTO;
    float Value = UINAN;
};

struct UIPropHyphens
{
    ui_css_hyphens_type_t Value;
};

struct UIPropInsetBlockEnd
{
    ui_css_inset_block_end_type_t Type;
    float Value;
};

struct UIPropInsetBlockStart
{
    ui_css_inset_block_start_type_t Type;
    float Value;
};

struct UIPropInsetInlineEnd
{
    ui_css_inset_inline_end_type_t Type;
    float Value;
};

struct UIPropInsetInlineStart
{
    ui_css_inset_inline_start_type_t Type;
    float Value;
};

struct UIPropJustifyContent
{
    ui_css_justify_content_type_t Value = UI_CSS_JUSTIFY_CONTENT_FLEX_START;
};

struct UIPropLetterSpacing
{
    ui_css_letter_spacing_type_t Type;
    float Value;
};

struct UIPropLineBreak
{
    ui_css_line_break_type_t Value;
};

struct UIPropLineHeight
{
    ui_css_line_height_type_t Type;
    float Value;
};

struct UIPropMargin
{
    ui_css_margin_type_t Type = UI_CSS_MARGIN__LENGTH;
    float Value = 0;
};

struct UIPropMarginLeft
{
    ui_css_margin_left_type_t Type = UI_CSS_MARGIN_LEFT__LENGTH;
    float Value = 0;
};

struct UIPropMarginRight
{
    ui_css_margin_right_type_t Type = UI_CSS_MARGIN_RIGHT__LENGTH;
    float Value = 0;
};

struct UIPropMarginTop
{
    ui_css_margin_top_type_t Type = UI_CSS_MARGIN_TOP__LENGTH;
    float Value = 0;
};

struct UIPropMarginBottom
{
    ui_css_margin_bottom_type_t Type = UI_CSS_MARGIN_BOTTOM__LENGTH;
    float Value = 0;
};

struct UIPropMaxHeight
{
    ui_css_max_height_type_t Type;
    float Value;
};

struct UIPropMaxWidth
{
    ui_css_max_width_type_t Type;
    float Value;
};

struct UIPropMinHeight
{
    ui_css_min_height_type_t Type;
    float Value;
};

struct UIPropMinWidth
{
    ui_css_min_width_type_t Type;
    float Value;
};

struct UIPropOpacity
{
    ui_css_opacity_type_t Type = UI_CSS_OPACITY__NUMBER;
    float Value = 1.0f;
};

struct UIPropOrder
{
    ui_css_order_type_t Type;
    int32_t Value;
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
    ui_css_overflow_wrap_type_t Value;
};

struct UIPropOverflowX
{
    ui_css_overflow_x_type_t Value = UI_CSS_OVERFLOW_X_VISIBLE;
};

struct UIPropOverflowY
{
    ui_css_overflow_y_type_t Value = UI_CSS_OVERFLOW_Y_VISIBLE;
};

struct UIPropPadding
{
    ui_css_padding_type_t Type = UI_CSS_PADDING__LENGTH;
    float Value = 0;
};

struct UIPropPaddingTop
{
    ui_css_padding_right_type_t Type = UI_CSS_PADDING_TOP__LENGTH;
    float Value = 0;
};

struct UIPropPaddingBottom
{
    ui_css_padding_bottom_type_t Type = UI_CSS_PADDING_BOTTOM__LENGTH;
    float Value = 0;
};

struct UIPropPaddingLeft
{
    ui_css_padding_left_type_t Type = UI_CSS_PADDING_LEFT__LENGTH;
    float Value = 0;
};

struct UIPropPaddingRight
{
    ui_css_padding_right_type_t Type = UI_CSS_PADDING_RIGHT__LENGTH;
    float Value = 0;
};

struct UIPropPosition
{
    ui_css_position_type_t Value = UI_CSS_POSITION_STATIC;
};

struct UIPropTop
{
    ui_css_top_type_t Type = UI_CSS_TOP_AUTO;
    float Value = UINAN;
};

struct UIPropBottom
{
    ui_css_bottom_type_t Type;
    float Value;
};

struct UIPropLeft
{
    ui_css_left_type_t Type;
    float Value;
};

struct UIPropRight
{
    ui_css_right_type_t Type;
    float Value;
};

struct UIPropTabSize
{
    ui_css_tab_size_type_t Type;
    uint32_t Value;
};

struct UIPropTextAlign
{
    ui_css_text_align_type_t Value;
};

struct UIPropTextAlignAll
{
    ui_css_text_align_all_type_t Value;
};

struct UIPropTextAlignLast
{
    ui_css_text_align_last_type_t Value;
};

struct UIPropTextCombineUpright
{
    ui_css_text_combine_upright_type_t Value;
};

struct UIPropTextDecorationLine
{
    ui_css_text_decoration_line_type_t Value;
};

struct UIPropTextDecorationStyle
{
    ui_css_text_decoration_style_type_t Value;
};

struct UIPropTextIndent
{
    ui_css_text_indent_type_t Type;
    float Value;
};

struct UIPropTextJustify
{
    ui_css_text_justify_type_t Value;
};

struct UIPropTextOrientation
{
    ui_css_text_orientation_type_t Value;
};

struct UIPropTextOverflow
{
    ui_css_text_overflow_type_t Value;
};

struct UIPropTextTransform
{
    ui_css_text_transform_type_t Value;
};

struct UIPropUnicodeBidi
{
    ui_css_unicode_bidi_type_t Value;
};

struct UIPropVerticalAlign
{
    ui_css_vertical_align_type_t Value;
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
    ui_css_width_type_t Type = UI_CSS_WIDTH_AUTO;
    float Value = UINAN;
};

struct UIPropWordBreak
{
    ui_css_word_break_type_t Value;
};

struct UIPropWordSpacing
{
    ui_css_word_spacing_type_t Type;
    float Value;
};

struct UIPropWordWrap
{
    ui_css_word_wrap_type_t Value;
};

struct UIPropWrapFlow
{
    ui_css_wrap_flow_type_t Value;
};

struct UIPropWrapThrough
{
    ui_css_wrap_through_type_t Value;
};

struct UIPropWritingMode
{
    ui_css_writing_mode_type_t Value;
};

struct UIPropZIndex
{
    ui_css_z_index_type_t Type;
    int32_t Value;
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

// Align Content
template <>
inline bool UITypeC(UIString const& src, UIPropAlignContent& dst)
{
    switch (UIHash(src))
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
    case UIHash("stretch"): dst.Value = UI_CSS_ALIGN_CONTENT_STRETCH;
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
    }
    return true;
}

// Align Items
template <>
inline bool UITypeC(UIString const& src, UIPropAlignItems& dst)
{
    switch (UIHash(src))
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
    case UIHash("stretch"): dst.Value = UI_CSS_ALIGN_ITEMS_STRETCH;
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
    }
    return true;
}

// Align Self
template <>
inline bool UITypeC(UIString const& src, UIPropAlignSelf& dst)
{
    switch (UIHash(src))
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
        dst.Type = UI_CSS_BASELINE_SHIFT__PERCENTAGE;
        dst.Value = std::stof(src);
    }
    else
    {
        dst.Type = UI_CSS_BASELINE_SHIFT__LENGTH;
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
    case UI_CSS_BASELINE_SHIFT__PERCENTAGE: dst = std::to_string(src.Value) + "%";
        return true;
    case UI_CSS_BASELINE_SHIFT__LENGTH: dst = std::to_string(src.Value) + "px";
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
    dst.Type = UI_CSS_BORDER__LENGTH;
    dst.Value = std::stof(src);
    return true;
}
template <>
inline bool UITypeC(UIPropBorder const& src, UIString& dst)
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
    case UI_CSS_BORDER__LENGTH: dst = std::to_string(src.Value) + "px";
        return true;
    }
}

template <>
inline bool UITypeC(UIString const& src, UIPropBorderTop& dst)
{
    auto s = UITrim(src);
    switch (UIHash(s))
    {
    default: break;
    case UIHash("thin"): dst.Type = UI_CSS_BORDER_TOP_THIN;
        return true;
    case UIHash("medium"): dst.Type = UI_CSS_BORDER_TOP_MEDIUM;
        return true;
    case UIHash("thick"): dst.Type = UI_CSS_BORDER_TOP_THICK;
        return true;
    case UIHash("none"): dst.Type = UI_CSS_BORDER_TOP_NONE;
        return true;
    case UIHash("hidden"): dst.Type = UI_CSS_BORDER_TOP_HIDDEN;
        return true;
    case UIHash("dotted"): dst.Type = UI_CSS_BORDER_TOP_DOTTED;
        return true;
    case UIHash("dashed"): dst.Type = UI_CSS_BORDER_TOP_DASHED;
        return true;
    case UIHash("solid"): dst.Type = UI_CSS_BORDER_TOP_SOLID;
        return true;
    case UIHash("double"): dst.Type = UI_CSS_BORDER_TOP_DOUBLE;
        return true;
    case UIHash("groove"): dst.Type = UI_CSS_BORDER_TOP_GROOVE;
        return true;
    case UIHash("ridge"): dst.Type = UI_CSS_BORDER_TOP_RIDGE;
        return true;
    case UIHash("inset"): dst.Type = UI_CSS_BORDER_TOP_INSET;
        return true;
    case UIHash("outset"): dst.Type = UI_CSS_BORDER_TOP_OUTSET;
        return true;
    }
    dst.Type = UI_CSS_BORDER_TOP__LENGTH;
    dst.Value = std::stof(src);
    return true;
}
template <>
inline bool UITypeC(UIPropBorderTop const& src, UIString& dst)
{
    switch (src.Type)
    {
    default: return false;
    case UI_CSS_BORDER_TOP_THIN: dst = "thin";
        return true;
    case UI_CSS_BORDER_TOP_MEDIUM: dst = "medium";
        return true;
    case UI_CSS_BORDER_TOP_THICK: dst = "thick";
        return true;
    case UI_CSS_BORDER_TOP_NONE: dst = "none";
        return true;
    case UI_CSS_BORDER_TOP_HIDDEN: dst = "hidden";
        return true;
    case UI_CSS_BORDER_TOP_DOTTED: dst = "dotted";
        return true;
    case UI_CSS_BORDER_TOP_DASHED: dst = "dashed";
        return true;
    case UI_CSS_BORDER_TOP_SOLID: dst = "solid";
        return true;
    case UI_CSS_BORDER_TOP_DOUBLE: dst = "double";
        return true;
    case UI_CSS_BORDER_TOP_GROOVE: dst = "groove";
        return true;
    case UI_CSS_BORDER_TOP_RIDGE: dst = "ridge";
        return true;
    case UI_CSS_BORDER_TOP_INSET: dst = "inset";
        return true;
    case UI_CSS_BORDER_TOP_OUTSET: dst = "outset";
        return true;
    case UI_CSS_BORDER_TOP__LENGTH: dst = std::to_string(src.Value) + "px";
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
    case UIHash("thin"): dst.Type = UI_CSS_BORDER_BOTTOM_THIN;
        return true;
    case UIHash("medium"): dst.Type = UI_CSS_BORDER_BOTTOM_MEDIUM;
        return true;
    case UIHash("thick"): dst.Type = UI_CSS_BORDER_BOTTOM_THICK;
        return true;
    case UIHash("none"): dst.Type = UI_CSS_BORDER_BOTTOM_NONE;
        return true;
    case UIHash("hidden"): dst.Type = UI_CSS_BORDER_BOTTOM_HIDDEN;
        return true;
    case UIHash("dotted"): dst.Type = UI_CSS_BORDER_BOTTOM_DOTTED;
        return true;
    case UIHash("dashed"): dst.Type = UI_CSS_BORDER_BOTTOM_DASHED;
        return true;
    case UIHash("solid"): dst.Type = UI_CSS_BORDER_BOTTOM_SOLID;
        return true;
    case UIHash("double"): dst.Type = UI_CSS_BORDER_BOTTOM_DOUBLE;
        return true;
    case UIHash("groove"): dst.Type = UI_CSS_BORDER_BOTTOM_GROOVE;
        return true;
    case UIHash("ridge"): dst.Type = UI_CSS_BORDER_BOTTOM_RIDGE;
        return true;
    case UIHash("inset"): dst.Type = UI_CSS_BORDER_BOTTOM_INSET;
        return true;
    case UIHash("outset"): dst.Type = UI_CSS_BORDER_BOTTOM_OUTSET;
        return true;
    }
    dst.Type = UI_CSS_BORDER_BOTTOM__LENGTH;
    dst.Value = std::stof(src);
    return true;
}
template <>
inline bool UITypeC(UIPropBorderBottom const& src, UIString& dst)
{
    switch (src.Type)
    {
    default: return false;
    case UI_CSS_BORDER_BOTTOM_THIN: dst = "thin";
        return true;
    case UI_CSS_BORDER_BOTTOM_MEDIUM: dst = "medium";
        return true;
    case UI_CSS_BORDER_BOTTOM_THICK: dst = "thick";
        return true;
    case UI_CSS_BORDER_BOTTOM_NONE: dst = "none";
        return true;
    case UI_CSS_BORDER_BOTTOM_HIDDEN: dst = "hidden";
        return true;
    case UI_CSS_BORDER_BOTTOM_DOTTED: dst = "dotted";
        return true;
    case UI_CSS_BORDER_BOTTOM_DASHED: dst = "dashed";
        return true;
    case UI_CSS_BORDER_BOTTOM_SOLID: dst = "solid";
        return true;
    case UI_CSS_BORDER_BOTTOM_DOUBLE: dst = "double";
        return true;
    case UI_CSS_BORDER_BOTTOM_GROOVE: dst = "groove";
        return true;
    case UI_CSS_BORDER_BOTTOM_RIDGE: dst = "ridge";
        return true;
    case UI_CSS_BORDER_BOTTOM_INSET: dst = "inset";
        return true;
    case UI_CSS_BORDER_BOTTOM_OUTSET: dst = "outset";
        return true;
    case UI_CSS_BORDER_BOTTOM__LENGTH: dst = std::to_string(src.Value) + "px";
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
    case UIHash("thin"): dst.Type = UI_CSS_BORDER_LEFT_THIN;
        return true;
    case UIHash("medium"): dst.Type = UI_CSS_BORDER_LEFT_MEDIUM;
        return true;
    case UIHash("thick"): dst.Type = UI_CSS_BORDER_LEFT_THICK;
        return true;
    case UIHash("none"): dst.Type = UI_CSS_BORDER_LEFT_NONE;
        return true;
    case UIHash("hidden"): dst.Type = UI_CSS_BORDER_LEFT_HIDDEN;
        return true;
    case UIHash("dotted"): dst.Type = UI_CSS_BORDER_LEFT_DOTTED;
        return true;
    case UIHash("dashed"): dst.Type = UI_CSS_BORDER_LEFT_DASHED;
        return true;
    case UIHash("solid"): dst.Type = UI_CSS_BORDER_LEFT_SOLID;
        return true;
    case UIHash("double"): dst.Type = UI_CSS_BORDER_LEFT_DOUBLE;
        return true;
    case UIHash("groove"): dst.Type = UI_CSS_BORDER_LEFT_GROOVE;
        return true;
    case UIHash("ridge"): dst.Type = UI_CSS_BORDER_LEFT_RIDGE;
        return true;
    case UIHash("inset"): dst.Type = UI_CSS_BORDER_LEFT_INSET;
        return true;
    case UIHash("outset"): dst.Type = UI_CSS_BORDER_LEFT_OUTSET;
        return true;
    }
    dst.Type = UI_CSS_BORDER_LEFT__LENGTH;
    dst.Value = std::stof(src);
    return true;
}
template <>
inline bool UITypeC(UIPropBorderLeft const& src, UIString& dst)
{
    switch (src.Type)
    {
    default: return false;
    case UI_CSS_BORDER_LEFT_THIN: dst = "thin";
        return true;
    case UI_CSS_BORDER_LEFT_MEDIUM: dst = "medium";
        return true;
    case UI_CSS_BORDER_LEFT_THICK: dst = "thick";
        return true;
    case UI_CSS_BORDER_LEFT_NONE: dst = "none";
        return true;
    case UI_CSS_BORDER_LEFT_HIDDEN: dst = "hidden";
        return true;
    case UI_CSS_BORDER_LEFT_DOTTED: dst = "dotted";
        return true;
    case UI_CSS_BORDER_LEFT_DASHED: dst = "dashed";
        return true;
    case UI_CSS_BORDER_LEFT_SOLID: dst = "solid";
        return true;
    case UI_CSS_BORDER_LEFT_DOUBLE: dst = "double";
        return true;
    case UI_CSS_BORDER_LEFT_GROOVE: dst = "groove";
        return true;
    case UI_CSS_BORDER_LEFT_RIDGE: dst = "ridge";
        return true;
    case UI_CSS_BORDER_LEFT_INSET: dst = "inset";
        return true;
    case UI_CSS_BORDER_LEFT_OUTSET: dst = "outset";
        return true;
    case UI_CSS_BORDER_LEFT__LENGTH: dst = std::to_string(src.Value) + "px";
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
    case UIHash("thin"): dst.Type = UI_CSS_BORDER_RIGHT_THIN;
        return true;
    case UIHash("medium"): dst.Type = UI_CSS_BORDER_RIGHT_MEDIUM;
        return true;
    case UIHash("thick"): dst.Type = UI_CSS_BORDER_RIGHT_THICK;
        return true;
    case UIHash("none"): dst.Type = UI_CSS_BORDER_RIGHT_NONE;
        return true;
    case UIHash("hidden"): dst.Type = UI_CSS_BORDER_RIGHT_HIDDEN;
        return true;
    case UIHash("dotted"): dst.Type = UI_CSS_BORDER_RIGHT_DOTTED;
        return true;
    case UIHash("dashed"): dst.Type = UI_CSS_BORDER_RIGHT_DASHED;
        return true;
    case UIHash("solid"): dst.Type = UI_CSS_BORDER_RIGHT_SOLID;
        return true;
    case UIHash("double"): dst.Type = UI_CSS_BORDER_RIGHT_DOUBLE;
        return true;
    case UIHash("groove"): dst.Type = UI_CSS_BORDER_RIGHT_GROOVE;
        return true;
    case UIHash("ridge"): dst.Type = UI_CSS_BORDER_RIGHT_RIDGE;
        return true;
    case UIHash("inset"): dst.Type = UI_CSS_BORDER_RIGHT_INSET;
        return true;
    case UIHash("outset"): dst.Type = UI_CSS_BORDER_RIGHT_OUTSET;
        return true;
    }
    dst.Type = UI_CSS_BORDER_RIGHT__LENGTH;
    dst.Value = std::stof(src);
    return true;
}
template <>
inline bool UITypeC(UIPropBorderRight const& src, UIString& dst)
{
    switch (src.Type)
    {
    default: return false;
    case UI_CSS_BORDER_RIGHT_THIN: dst = "thin";
        return true;
    case UI_CSS_BORDER_RIGHT_MEDIUM: dst = "medium";
        return true;
    case UI_CSS_BORDER_RIGHT_THICK: dst = "thick";
        return true;
    case UI_CSS_BORDER_RIGHT_NONE: dst = "none";
        return true;
    case UI_CSS_BORDER_RIGHT_HIDDEN: dst = "hidden";
        return true;
    case UI_CSS_BORDER_RIGHT_DOTTED: dst = "dotted";
        return true;
    case UI_CSS_BORDER_RIGHT_DASHED: dst = "dashed";
        return true;
    case UI_CSS_BORDER_RIGHT_SOLID: dst = "solid";
        return true;
    case UI_CSS_BORDER_RIGHT_DOUBLE: dst = "double";
        return true;
    case UI_CSS_BORDER_RIGHT_GROOVE: dst = "groove";
        return true;
    case UI_CSS_BORDER_RIGHT_RIDGE: dst = "ridge";
        return true;
    case UI_CSS_BORDER_RIGHT_INSET: dst = "inset";
        return true;
    case UI_CSS_BORDER_RIGHT_OUTSET: dst = "outset";
        return true;
    case UI_CSS_BORDER_RIGHT__LENGTH: dst = std::to_string(src.Value) + "px";
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

// Direction
template <>
inline bool UITypeC(UIString const& src, UIPropDirection& dst)
{
    switch (UIHash(UITrim(src)))
    {
    default: return false;
    case UIHash("ltr"): dst.Value = UI_CSS_DIRECTION_LTR; return true;
    case UIHash("rtl"): dst.Value = UI_CSS_DIRECTION_RTL; return true;
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
    auto s = UITrim(src);
    if (s == "content")
    {
        dst.Type = UI_CSS_FLEX_BASIS_CONTENT;
    }
    else if (s.ends_with('%'))
    {
        dst.Type = UI_CSS_BOTTOM__PERCENTAGE;
        dst.Value = std::stof(s);
    }
    else
    {
        dst.Type = UI_CSS_BOTTOM__LENGTH;
        dst.Value = std::stof(s);
    }
    return true;
}
template <>
inline bool UITypeC(UIPropFlexBasis const& src, UIString& dst)
{
    // Note: fallback uses px/percent/number
    switch (src.Type)
    {
    default: return false;
    case UI_CSS_FLEX_BASIS_CONTENT: dst = "content";
        return true;
    case UI_CSS_FLEX_BASIS__LENGTH: dst = std::to_string(src.Value) + "px";
        return true;
    case UI_CSS_FLEX_BASIS__PERCENTAGE: dst = std::to_string(src.Value) + "%";
        return true;
    }
}

// Flex Grow/Shrink
template <>
inline bool UITypeC(UIString const& src, UIPropFlexGrow& dst)
{
    dst.Type = UI_CSS_FLEX_GROW__NUMBER;
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
    dst.Type = UI_CSS_FLEX_SHRINK__NUMBER;
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
        dst.Type = UI_CSS_FLOAT_OFFSET__PERCENTAGE;
        dst.Value = std::stof(s);
    }
    else
    {
        dst.Type = UI_CSS_FLOAT_OFFSET__LENGTH;
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
    case UI_CSS_FLOAT_OFFSET__LENGTH: dst = std::to_string(src.Value) + "px";
        return true;
    case UI_CSS_FLOAT_OFFSET__PERCENTAGE: dst = std::to_string(src.Value) + "%";
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
        dst.Type = UI_CSS_INSET_BLOCK_END__PERCENTAGE;
        dst.Value = std::stof(s);
    }
    else
    {
        dst.Type = UI_CSS_INSET_BLOCK_END__LENGTH;
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
    case UI_CSS_INSET_BLOCK_END__LENGTH: dst = std::to_string(src.Value) + "px"; return true;
    case UI_CSS_INSET_BLOCK_END__PERCENTAGE: dst = std::to_string(src.Value) + "%"; return true;
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
        dst.Type = UI_CSS_INSET_BLOCK_START__PERCENTAGE;
        dst.Value = std::stof(s);
    }
    else
    {
        dst.Type = UI_CSS_INSET_BLOCK_START__LENGTH;
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
    case UI_CSS_INSET_BLOCK_START__LENGTH: dst = std::to_string(src.Value) + "px"; return true;
    case UI_CSS_INSET_BLOCK_START__PERCENTAGE: dst = std::to_string(src.Value) + "%"; return true;
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
        dst.Type = UI_CSS_INSET_INLINE_END__PERCENTAGE;
        dst.Value = std::stof(s);
    }
    else
    {
        dst.Type = UI_CSS_INSET_INLINE_END__LENGTH;
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
    case UI_CSS_INSET_INLINE_END__LENGTH: dst = std::to_string(src.Value) + "px"; return true;
    case UI_CSS_INSET_INLINE_END__PERCENTAGE: dst = std::to_string(src.Value) + "%"; return true;
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
        dst.Type = UI_CSS_INSET_INLINE_START__PERCENTAGE;
        dst.Value = std::stof(s);
    }
    else
    {
        dst.Type = UI_CSS_INSET_INLINE_START__LENGTH;
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
    case UI_CSS_INSET_INLINE_START__LENGTH: dst = std::to_string(src.Value) + "px"; return true;
    case UI_CSS_INSET_INLINE_START__PERCENTAGE: dst = std::to_string(src.Value) + "%"; return true;
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
        dst.Type = UI_CSS_LEFT__PERCENTAGE;
        dst.Value = std::stof(s);
    }
    else
    {
        dst.Type = UI_CSS_LEFT__LENGTH;
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
    case UI_CSS_LEFT__LENGTH: dst = std::to_string(src.Value) + "px"; return true;
    case UI_CSS_LEFT__PERCENTAGE: dst = std::to_string(src.Value) + "%"; return true;
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
        dst.Type = UI_CSS_RIGHT__PERCENTAGE;
        dst.Value = std::stof(s);
    }
    else
    {
        dst.Type = UI_CSS_RIGHT__LENGTH;
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
    case UI_CSS_RIGHT__LENGTH: dst = std::to_string(src.Value) + "px"; return true;
    case UI_CSS_RIGHT__PERCENTAGE: dst = std::to_string(src.Value) + "%"; return true;
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
        dst.Type = UI_CSS_TOP__PERCENTAGE;
        dst.Value = std::stof(s);
    }
    else
    {
        dst.Type = UI_CSS_TOP__LENGTH;
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
    case UI_CSS_TOP__LENGTH: dst = std::to_string(src.Value) + "px"; return true;
    case UI_CSS_TOP__PERCENTAGE: dst = std::to_string(src.Value) + "%"; return true;
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
        dst.Type = UI_CSS_BOTTOM__PERCENTAGE;
        dst.Value = std::stof(s);
    }
    else
    {
        dst.Type = UI_CSS_BOTTOM__LENGTH;
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
    case UI_CSS_BOTTOM__LENGTH: dst = std::to_string(src.Value) + "px";
        return true;
    case UI_CSS_BOTTOM__PERCENTAGE: dst = std::to_string(src.Value) + "%";
        return true;
    }
}

template<>
inline bool UITypeC(UIString const& src, UIPropWidth& dst)
{
    auto s = UITrim(src);
    if (s == "auto")
    {
        dst.Type = UI_CSS_WIDTH_AUTO;
    }
    else if (s.ends_with('%'))
    {
        dst.Type = UI_CSS_WIDTH__PERCENTAGE;
        dst.Value = std::stof(s);
    }
    else
    {
        dst.Type = UI_CSS_WIDTH__LENGTH;
        dst.Value = std::stof(s);
    }
    return true;
}
template<>
inline bool UITypeC(UIPropWidth const& src, UIString& dst)
{
    switch (src.Type) {
    default: return false;
    case UI_CSS_WIDTH_AUTO: dst = "auto"; return true;
    case UI_CSS_WIDTH__LENGTH: dst = std::to_string(src.Value) + "px"; return true;
    case UI_CSS_WIDTH__PERCENTAGE: dst = std::to_string(src.Value) + "%"; return true;
    case UI_CSS_WIDTH__NUMBER: dst = std::to_string(src.Value); return true;
    }
}

// Height
template <>
inline bool UITypeC(UIString const& src, UIPropHeight& dst)
{
    auto s = UITrim(src);
    if (s == "auto")
    {
        dst.Type = UI_CSS_HEIGHT_AUTO;
    }
    else if (s.ends_with('%'))
    {
        dst.Type = UI_CSS_HEIGHT__PERCENTAGE;
        dst.Value = std::stof(s);
    }
    else
    {
        dst.Type = UI_CSS_HEIGHT__LENGTH;
        dst.Value = std::stof(s);
    }
    return true;
}
template <>
inline bool UITypeC(UIPropHeight const& src, UIString& dst)
{
    switch (src.Type)
    {
    default: return false;
    case UI_CSS_HEIGHT_AUTO: dst = "auto";
        return true;
    case UI_CSS_HEIGHT__LENGTH: dst = std::to_string(src.Value) + "px";
        return true;
    case UI_CSS_HEIGHT__PERCENTAGE: dst = std::to_string(src.Value) + "%";
        return true;
    }
}

// Min/Max width/height
template<>
inline bool UITypeC(UIString const& src, UIPropMinWidth& dst)
{
    auto s = UITrim(src);
    if (s == "auto")
    {
        dst.Type = UI_CSS_MIN_WIDTH_AUTO;
    }
    else if (s.ends_with('%'))
    {
        dst.Type = UI_CSS_MIN_WIDTH__PERCENTAGE;
        dst.Value = std::stof(s);
    }
    else
    {
        dst.Type = UI_CSS_MIN_WIDTH__LENGTH;
        dst.Value = std::stof(s);
    }
    return true;
}
template<>
inline bool UITypeC(UIPropMinWidth const& src, UIString& dst)
{
    switch (src.Type) {
    default: return false;
    case UI_CSS_MIN_WIDTH_AUTO: dst = "auto"; return true;
    case UI_CSS_MIN_WIDTH__LENGTH: dst = std::to_string(src.Value) + "px"; return true;
    case UI_CSS_MIN_WIDTH__PERCENTAGE: dst = std::to_string(src.Value) + "%"; return true;
    }
}

template<>
inline bool UITypeC(UIString const& src, UIPropMaxWidth& dst)
{
    auto s = UITrim(src);
    if (s == "none")
    {
        dst.Type = UI_CSS_MAX_WIDTH_NONE;
    }
    else if (s.ends_with('%'))
    {
        dst.Type = UI_CSS_MAX_WIDTH__PERCENTAGE;
        dst.Value = std::stof(s);
    }
    else
    {
        dst.Type = UI_CSS_MAX_WIDTH__LENGTH;
        dst.Value = std::stof(s);
    }
    return true;
}
template<>
inline bool UITypeC(UIPropMaxWidth const& src, UIString& dst)
{
    switch (src.Type) {
    default: return false;
    case UI_CSS_MAX_WIDTH_NONE: dst = "none"; return true;
    case UI_CSS_MAX_WIDTH__LENGTH: dst = std::to_string(src.Value) + "px"; return true;
    case UI_CSS_MAX_WIDTH__PERCENTAGE: dst = std::to_string(src.Value) + "%"; return true;
    }
}

template<>
inline bool UITypeC(UIString const& src, UIPropMinHeight& dst)
{
    auto s = UITrim(src);
    if (s == "auto")
    {
        dst.Type = UI_CSS_MIN_HEIGHT_AUTO;
    }
    else if (s.ends_with('%'))
    {
        dst.Type = UI_CSS_MIN_HEIGHT__PERCENTAGE;
        dst.Value = std::stof(s);
    }
    else
    {
        dst.Type = UI_CSS_MIN_HEIGHT__LENGTH;
        dst.Value = std::stof(s);
    }
    return true;
}
template<>
inline bool UITypeC(UIPropMinHeight const& src, UIString& dst)
{
    switch (src.Type) {
    default: return false;
    case UI_CSS_MIN_HEIGHT_AUTO: dst = "auto"; return true;
    case UI_CSS_MIN_HEIGHT__LENGTH: dst = std::to_string(src.Value) + "px"; return true;
    case UI_CSS_MIN_HEIGHT__PERCENTAGE: dst = std::to_string(src.Value) + "%"; return true;
    }
}

template<>
inline bool UITypeC(UIString const& src, UIPropMaxHeight& dst)
{
    auto s = UITrim(src);
    if (s == "none")
    {
        dst.Type = UI_CSS_MAX_HEIGHT_NONE;
    }
    else if (s.ends_with('%'))
    {
        dst.Type = UI_CSS_MAX_HEIGHT__PERCENTAGE;
        dst.Value = std::stof(s);
    }
    else
    {
        dst.Type = UI_CSS_MAX_HEIGHT__LENGTH;
        dst.Value = std::stof(s);
    }
    return true;
}
template<>
inline bool UITypeC(UIPropMaxHeight const& src, UIString& dst)
{
    switch (src.Type) {
    default: return false;
    case UI_CSS_MAX_HEIGHT_NONE: dst = "none"; return true;
    case UI_CSS_MAX_HEIGHT__LENGTH: dst = std::to_string(src.Value) + "px"; return true;
    case UI_CSS_MAX_HEIGHT__PERCENTAGE: dst = std::to_string(src.Value) + "%"; return true;
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
        dst.Type = UI_CSS_LETTER_SPACING__LENGTH;
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
    case UI_CSS_LETTER_SPACING__LENGTH: dst = std::to_string(src.Value) + "px"; return true;
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
        dst.Type = UI_CSS_LINE_HEIGHT__PERCENTAGE;
        dst.Value = std::stof(s);
    }
    else
    {
        dst.Type = UI_CSS_LINE_HEIGHT__LENGTH;
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
    case UI_CSS_LINE_HEIGHT__LENGTH: dst = std::to_string(src.Value) + "px"; return true;
    case UI_CSS_LINE_HEIGHT__PERCENTAGE: dst = std::to_string(src.Value) + "%"; return true;
    }
}

template<>
inline bool UITypeC(UIString const& src, UIPropTextIndent& dst)
{
    auto s = UITrim(src);
    if (s.ends_with('%'))
    {
        dst.Type = UI_CSS_TEXT_INDENT__PERCENTAGE;
        dst.Value = std::stof(s);
    }
    else
    {
        dst.Type = UI_CSS_TEXT_INDENT__LENGTH;
        dst.Value = std::stof(s);
    }
    return true;
}
template<>
inline bool UITypeC(UIPropTextIndent const& src, UIString& dst)
{
    switch (src.Type) {
    default: return false;
    case UI_CSS_TEXT_INDENT__LENGTH: dst = std::to_string(src.Value) + "px"; return true;
    case UI_CSS_TEXT_INDENT__PERCENTAGE: dst = std::to_string(src.Value) + "%"; return true;
    }
}

template<>
inline bool UITypeC(UIString const& src, UIPropTabSize& dst)
{
    auto s = UITrim(src);
    dst.Type = UI_CSS_TAB_SIZE__LENGTH;
    dst.Value = std::stof(s);
    return true;
}
template<>
inline bool UITypeC(UIPropTabSize const& src, UIString& dst)
{
    switch (src.Type) {
    default: return false;
    case UI_CSS_TAB_SIZE__NUMBER: dst = std::to_string(src.Value); return true;
    case UI_CSS_TAB_SIZE__LENGTH: dst = std::to_string(src.Value) + "px"; return true;
    }
}

template<>
inline bool UITypeC(UIString const& src, UIPropMargin& dst)
{
    auto s = UITrim(src);
    if (s == "auto")
    {
        dst.Type = UI_CSS_MARGIN_AUTO;
    }
    else if (s.ends_with('%'))
    {
        dst.Type = UI_CSS_MARGIN__PERCENTAGE;
        dst.Value = std::stof(s);
    }
    else
    {
        dst.Type = UI_CSS_MARGIN__LENGTH;
        dst.Value = std::stof(s);
    }
    return true;
}
template<>
inline bool UITypeC(UIPropMargin const& src, UIString& dst)
{
    switch (src.Type) {
    default: return false;
    case UI_CSS_MARGIN_AUTO: dst = "auto"; return true;
    case UI_CSS_MARGIN__LENGTH: dst = std::to_string(src.Value) + "px"; return true;
    case UI_CSS_MARGIN__PERCENTAGE: dst = std::to_string(src.Value) + "%"; return true;
    }
}

template<>
inline bool UITypeC(UIString const& src, UIPropMarginLeft& dst)
{
    auto s = UITrim(src);
    if (s == "auto")
    {
        dst.Type = UI_CSS_MARGIN_LEFT_AUTO;
    }
    else if (s.ends_with('%'))
    {
        dst.Type = UI_CSS_MARGIN_LEFT__PERCENTAGE;
        dst.Value = std::stof(s);
    }
    else
    {
        dst.Type = UI_CSS_MARGIN_LEFT__LENGTH;
        dst.Value = std::stof(s);
    }
    return true;
}
template<>
inline bool UITypeC(UIPropMarginLeft const& src, UIString& dst)
{
    switch (src.Type) {
    default: return false;
    case UI_CSS_MARGIN_LEFT_AUTO: dst = "auto"; return true;
    case UI_CSS_MARGIN_LEFT__LENGTH: dst = std::to_string(src.Value) + "px"; return true;
    case UI_CSS_MARGIN_LEFT__PERCENTAGE: dst = std::to_string(src.Value) + "%"; return true;
    }
}

template<>
inline bool UITypeC(UIString const& src, UIPropMarginRight& dst)
{
    auto s = UITrim(src);
    if (s == "auto")
    {
        dst.Type = UI_CSS_MARGIN_RIGHT_AUTO;
    }
    else if (s.ends_with('%'))
    {
        dst.Type = UI_CSS_MARGIN_RIGHT__PERCENTAGE;
        dst.Value = std::stof(s);
    }
    else
    {
        dst.Type = UI_CSS_MARGIN_RIGHT__LENGTH;
        dst.Value = std::stof(s);
    }
    return true;
}
template<>
inline bool UITypeC(UIPropMarginRight const& src, UIString& dst)
{
    switch (src.Type) {
    default: return false;
    case UI_CSS_MARGIN_RIGHT_AUTO: dst = "auto"; return true;
    case UI_CSS_MARGIN_RIGHT__LENGTH: dst = std::to_string(src.Value) + "px"; return true;
    case UI_CSS_MARGIN_RIGHT__PERCENTAGE: dst = std::to_string(src.Value) + "%"; return true;
    }
}

template<>
inline bool UITypeC(UIString const& src, UIPropMarginTop& dst)
{
    auto s = UITrim(src);
    if (s == "auto")
    {
        dst.Type = UI_CSS_MARGIN_TOP_AUTO;
    }
    else if (s.ends_with('%'))
    {
        dst.Type = UI_CSS_MARGIN_TOP__PERCENTAGE;
        dst.Value = std::stof(s);
    }
    else
    {
        dst.Type = UI_CSS_MARGIN_TOP__LENGTH;
        dst.Value = std::stof(s);
    }
    return true;
}
template<>
inline bool UITypeC(UIPropMarginTop const& src, UIString& dst)
{
    switch (src.Type) {
    default: return false;
    case UI_CSS_MARGIN_TOP_AUTO: dst = "auto"; return true;
    case UI_CSS_MARGIN_TOP__LENGTH: dst = std::to_string(src.Value) + "px"; return true;
    case UI_CSS_MARGIN_TOP__PERCENTAGE: dst = std::to_string(src.Value) + "%"; return true;
    }
}

template<>
inline bool UITypeC(UIString const& src, UIPropMarginBottom& dst)
{
    auto s = UITrim(src);
    if (s == "auto")
    {
        dst.Type = UI_CSS_MARGIN_BOTTOM_AUTO;
    }
    else if (s.ends_with('%'))
    {
        dst.Type = UI_CSS_MARGIN_BOTTOM__PERCENTAGE;
        dst.Value = std::stof(s);
    }
    else
    {
        dst.Type = UI_CSS_MARGIN_BOTTOM__LENGTH;
        dst.Value = std::stof(s);
    }
    return true;
}
template<>
inline bool UITypeC(UIPropMarginBottom const& src, UIString& dst)
{
    switch (src.Type) {
    default: return false;
    case UI_CSS_MARGIN_BOTTOM_AUTO: dst = "auto"; return true;
    case UI_CSS_MARGIN_BOTTOM__LENGTH: dst = std::to_string(src.Value) + "px"; return true;
    case UI_CSS_MARGIN_BOTTOM__PERCENTAGE: dst = std::to_string(src.Value) + "%"; return true;
    }
}

// Padding
template<>
inline bool UITypeC(UIString const& src, UIPropPadding& dst)
{
    auto s = UITrim(src);
    if (s.ends_with('%'))
    {
        dst.Type = UI_CSS_PADDING__PERCENTAGE;
        dst.Value = std::stof(s);
    }
    else
    {
        dst.Type = UI_CSS_PADDING__LENGTH;
        dst.Value = std::stof(s);
    }
    return true;
}
template<>
inline bool UITypeC(UIPropPadding const& src, UIString& dst)
{
    switch (src.Type) {
    default: return false;
    case UI_CSS_PADDING__LENGTH: dst = std::to_string(src.Value) + "px"; return true;
    case UI_CSS_PADDING__PERCENTAGE: dst = std::to_string(src.Value) + "%"; return true;
    }
}

template<>
inline bool UITypeC(UIString const& src, UIPropPaddingLeft& dst)
{
    auto s = UITrim(src);
    if (s.ends_with('%'))
    {
        dst.Type = UI_CSS_PADDING_LEFT__PERCENTAGE;
        dst.Value = std::stof(s);
    }
    else
    {
        dst.Type = UI_CSS_PADDING_LEFT__LENGTH;
        dst.Value = std::stof(s);
    }
    return true;
}
template<>
inline bool UITypeC(UIPropPaddingLeft const& src, UIString& dst)
{
    switch (src.Type) {
    default: return false;
    case UI_CSS_PADDING_LEFT__LENGTH: dst = std::to_string(src.Value) + "px"; return true;
    case UI_CSS_PADDING_LEFT__PERCENTAGE: dst = std::to_string(src.Value) + "%"; return true;
    }
}

template<>
inline bool UITypeC(UIString const& src, UIPropPaddingRight& dst)
{
    auto s = UITrim(src);
    if (s.ends_with('%'))
    {
        dst.Type = UI_CSS_PADDING_RIGHT__PERCENTAGE;
        dst.Value = std::stof(s);
    }
    else
    {
        dst.Type = UI_CSS_PADDING_RIGHT__LENGTH;
        dst.Value = std::stof(s);
    }
    return true;
}
template<>
inline bool UITypeC(UIPropPaddingRight const& src, UIString& dst)
{
    switch (src.Type) {
    default: return false;
    case UI_CSS_PADDING_RIGHT__LENGTH: dst = std::to_string(src.Value) + "px"; return true;
    case UI_CSS_PADDING_RIGHT__PERCENTAGE: dst = std::to_string(src.Value) + "%"; return true;
    }
}

template<>
inline bool UITypeC(UIString const& src, UIPropPaddingTop& dst)
{
    auto s = UITrim(src);
    if (s.ends_with('%'))
    {
        dst.Type = UI_CSS_PADDING_TOP__PERCENTAGE;
        dst.Value = std::stof(s);
    }
    else
    {
        dst.Type = UI_CSS_PADDING_TOP__LENGTH;
        dst.Value = std::stof(s);
    }
    return true;
}
template<>
inline bool UITypeC(UIPropPaddingTop const& src, UIString& dst)
{
    switch (src.Type) {
    default: return false;
    case UI_CSS_PADDING_TOP__LENGTH: dst = std::to_string(src.Value) + "px"; return true;
    case UI_CSS_PADDING_TOP__PERCENTAGE: dst = std::to_string(src.Value) + "%"; return true;
    }
}

template<>
inline bool UITypeC(UIString const& src, UIPropPaddingBottom& dst)
{
    auto s = UITrim(src);
    if (s.ends_with('%'))
    {
        dst.Type = UI_CSS_PADDING_BOTTOM__PERCENTAGE;
        dst.Value = std::stof(s);
    }
    else
    {
        dst.Type = UI_CSS_PADDING_BOTTOM__LENGTH;
        dst.Value = std::stof(s);
    }
    return true;
}
template<>
inline bool UITypeC(UIPropPaddingBottom const& src, UIString& dst)
{
    switch (src.Type) {
    default: return false;
    case UI_CSS_PADDING_BOTTOM__LENGTH: dst = std::to_string(src.Value) + "px"; return true;
    case UI_CSS_PADDING_BOTTOM__PERCENTAGE: dst = std::to_string(src.Value) + "%"; return true;
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
    }
}

// Justify content
template <>
inline bool UITypeC(UIString const& src, UIPropJustifyContent& dst)
{
    switch (UIHash(src))
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
    dst.Type = UI_CSS_Z_INDEX__INTEGER;
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
    case UI_CSS_Z_INDEX__INTEGER: dst = std::to_string(src.Value);
        return true;
    }
}

// Note: Not all properties' tokens/keywords are exhaustively implemented here. This provides full numeric parsing for length/percentage/number where applicable and common keyword mappings. Further refinements can be added per-property on request.


