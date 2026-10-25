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


enum
{
    UI_CSS_ALIGN_CONTENT_FLEX_START,
    UI_CSS_ALIGN_CONTENT_FLEX_END,
    UI_CSS_ALIGN_CONTENT_CENTER,
    UI_CSS_ALIGN_CONTENT_SPACE_BETWEEN,
    UI_CSS_ALIGN_CONTENT_SPACE_AROUND,
    UI_CSS_ALIGN_CONTENT_STRETCH,
    UI_CSS_ALIGN_CONTENT_SPACE_EVENLY,
    UI_CSS_ALIGN_CONTENT_BASE_LINE,
    UI_CSS_ALIGN_CONTENT_START,
    UI_CSS_ALIGN_CONTENT_END,
};

typedef unsigned int ui_css_align_content_type_t;

enum
{
    UI_CSS_ALIGN_ITEMS_FLEX_START,
    UI_CSS_ALIGN_ITEMS_FLEX_END,
    UI_CSS_ALIGN_ITEMS_CENTER,
    UI_CSS_ALIGN_ITEMS_BASELINE,
    UI_CSS_ALIGN_ITEMS_STRETCH,
    UI_CSS_ALIGN_ITEMS_SPACE_EVENLY,
    UI_CSS_ALIGN_ITEMS_SPACE_AROUND,
    UI_CSS_ALIGN_ITEMS_START,
    UI_CSS_ALIGN_ITEMS_END,
};

typedef unsigned int ui_css_align_items_type_t;

enum
{
    UI_CSS_ALIGN_SELF_AUTO,
    UI_CSS_ALIGN_SELF_FLEX_START,
    UI_CSS_ALIGN_SELF_FLEX_END,
    UI_CSS_ALIGN_SELF_CENTER,
    UI_CSS_ALIGN_SELF_BASELINE,
    UI_CSS_ALIGN_SELF_STRETCH,
    UI_CSS_ALIGN_SPACE_EVENLY,
    UI_CSS_ALIGN_SPACE_AROUND,
    UI_CSS_ALIGN_SELF_START,
    UI_CSS_ALIGN_SELF_END,
};

typedef unsigned int ui_css_align_self_type_t;

enum
{
    UI_CSS_ALIGNMENT_BASELINE_BASELINE,
    UI_CSS_ALIGNMENT_BASELINE_TEXT_BOTTOM,
    UI_CSS_ALIGNMENT_BASELINE_ALPHABETIC,
    UI_CSS_ALIGNMENT_BASELINE_IDEOGRAPHIC,
    UI_CSS_ALIGNMENT_BASELINE_MIDDLE,
    UI_CSS_ALIGNMENT_BASELINE_CENTRAL,
    UI_CSS_ALIGNMENT_BASELINE_MATHEMATICAL,
    UI_CSS_ALIGNMENT_BASELINE_TEXT_TOP,
};

typedef unsigned int ui_css_alignment_baseline_type_t;


enum
{
    UI_CSS_BASELINE_SHIFT_LENGTH,
    UI_CSS_BASELINE_SHIFT_PERCENTAGE,
    UI_CSS_BASELINE_SHIFT_SUB,
    UI_CSS_BASELINE_SHIFT_SUPER,
    UI_CSS_BASELINE_SHIFT_TOP,
    UI_CSS_BASELINE_SHIFT_CENTER,
    UI_CSS_BASELINE_SHIFT_BOTTOM,
};

typedef unsigned int ui_css_baseline_shift_type_t;

enum
{
    UI_CSS_BASELINE_SOURCE_AUTO,
    UI_CSS_BASELINE_SOURCE_FIRST,
    UI_CSS_BASELINE_SOURCE_LAST,
};

typedef unsigned int ui_css_baseline_source_type_t;

enum
{
    UI_CSS_BORDER_THIN,
    UI_CSS_BORDER_MEDIUM,
    UI_CSS_BORDER_THICK,
    UI_CSS_BORDER_NONE,
    UI_CSS_BORDER_HIDDEN,
    UI_CSS_BORDER_DOTTED,
    UI_CSS_BORDER_DASHED,
    UI_CSS_BORDER_SOLID,
    UI_CSS_BORDER_DOUBLE,
    UI_CSS_BORDER_GROOVE,
    UI_CSS_BORDER_RIDGE,
    UI_CSS_BORDER_INSET,
    UI_CSS_BORDER_OUTSET,
    UI_CSS_BORDER_LENGTH,
};

typedef unsigned int ui_css_border_type_t, ui_css_border_bottom_type_t, ui_css_border_left_type_t,
                     ui_css_border_right_type_t, ui_css_border_top_type_t;

enum
{
    UI_CSS_BORDER_RADIUS_LENGTH,
    UI_CSS_BORDER_RADIUS_PERCENTAGE,
};

typedef unsigned int ui_css_border_radius_type_t, ui_css_border_top_left_radius_type_t,
                     ui_css_border_top_right_radius_type_t, ui_css_border_bottom_right_radius_type_t,
                     ui_css_border_bottom_left_radius_type_t;

enum
{
    UI_CSS_BOX_SIZING_CONTENT_BOX,
    UI_CSS_BOX_SIZING_BORDER_BOX,
};

typedef unsigned int ui_css_box_sizing_type_t;

enum
{
    UI_CSS_CLEAR_INLINE_START,
    UI_CSS_CLEAR_INLINE_END,
    UI_CSS_CLEAR_BLOCK_START,
    UI_CSS_CLEAR_BLOCK_END,
    UI_CSS_CLEAR_LEFT,
    UI_CSS_CLEAR_RIGHT,
    UI_CSS_CLEAR_TOP,
    UI_CSS_CLEAR_BOTTOM,
    UI_CSS_CLEAR_NONE,
};

typedef unsigned int ui_css_clear_type_t;

enum
{
    UI_CSS_COLOR_CURRENTCOLOR,
    UI_CSS_COLOR_TRANSPARENT,
    UI_CSS_COLOR_HEX,
    UI_CSS_COLOR_ALICEBLUE,
    UI_CSS_COLOR_ANTIQUEWHITE,
    UI_CSS_COLOR_AQUA,
    UI_CSS_COLOR_AQUAMARINE,
    UI_CSS_COLOR_AZURE,
    UI_CSS_COLOR_BEIGE,
    UI_CSS_COLOR_BISQUE,
    UI_CSS_COLOR_BLACK,
    UI_CSS_COLOR_BLANCHEDALMOND,
    UI_CSS_COLOR_BLUE,
    UI_CSS_COLOR_BLUEVIOLET,
    UI_CSS_COLOR_BROWN,
    UI_CSS_COLOR_BURLYWOOD,
    UI_CSS_COLOR_CADETBLUE,
    UI_CSS_COLOR_CHARTREUSE,
    UI_CSS_COLOR_CHOCOLATE,
    UI_CSS_COLOR_CORAL,
    UI_CSS_COLOR_CORNFLOWERBLUE,
    UI_CSS_COLOR_CORNSILK,
    UI_CSS_COLOR_CRIMSON,
    UI_CSS_COLOR_CYAN,
    UI_CSS_COLOR_DARKBLUE,
    UI_CSS_COLOR_DARKCYAN,
    UI_CSS_COLOR_DARKGOLDENROD,
    UI_CSS_COLOR_DARKGRAY,
    UI_CSS_COLOR_DARKGREEN,
    UI_CSS_COLOR_DARKGREY,
    UI_CSS_COLOR_DARKKHAKI,
    UI_CSS_COLOR_DARKMAGENTA,
    UI_CSS_COLOR_DARKOLIVEGREEN,
    UI_CSS_COLOR_DARKORANGE,
    UI_CSS_COLOR_DARKORCHID,
    UI_CSS_COLOR_DARKRED,
    UI_CSS_COLOR_DARKSALMON,
    UI_CSS_COLOR_DARKSEAGREEN,
    UI_CSS_COLOR_DARKSLATEBLUE,
    UI_CSS_COLOR_DARKSLATEGRAY,
    UI_CSS_COLOR_DARKSLATEGREY,
    UI_CSS_COLOR_DARKTURQUOISE,
    UI_CSS_COLOR_DARKVIOLET,
    UI_CSS_COLOR_DEEPPINK,
    UI_CSS_COLOR_DEEPSKYBLUE,
    UI_CSS_COLOR_DIMGRAY,
    UI_CSS_COLOR_DIMGREY,
    UI_CSS_COLOR_DODGERBLUE,
    UI_CSS_COLOR_FIREBRICK,
    UI_CSS_COLOR_FLORALWHITE,
    UI_CSS_COLOR_FORESTGREEN,
    UI_CSS_COLOR_FUCHSIA,
    UI_CSS_COLOR_GAINSBORO,
    UI_CSS_COLOR_GHOSTWHITE,
    UI_CSS_COLOR_GOLD,
    UI_CSS_COLOR_GOLDENROD,
    UI_CSS_COLOR_GRAY,
    UI_CSS_COLOR_GREEN,
    UI_CSS_COLOR_GREENYELLOW,
    UI_CSS_COLOR_GREY,
    UI_CSS_COLOR_HONEYDEW,
    UI_CSS_COLOR_HOTPINK,
    UI_CSS_COLOR_INDIANRED,
    UI_CSS_COLOR_INDIGO,
    UI_CSS_COLOR_IVORY,
    UI_CSS_COLOR_KHAKI,
    UI_CSS_COLOR_LAVENDER,
    UI_CSS_COLOR_LAVENDERBLUSH,
    UI_CSS_COLOR_LAWNGREEN,
    UI_CSS_COLOR_LEMONCHIFFON,
    UI_CSS_COLOR_LIGHTBLUE,
    UI_CSS_COLOR_LIGHTCORAL,
    UI_CSS_COLOR_LIGHTCYAN,
    UI_CSS_COLOR_LIGHTGOLDENRODYELLOW,
    UI_CSS_COLOR_LIGHTGRAY,
    UI_CSS_COLOR_LIGHTGREEN,
    UI_CSS_COLOR_LIGHTGREY,
    UI_CSS_COLOR_LIGHTPINK,
    UI_CSS_COLOR_LIGHTSALMON,
    UI_CSS_COLOR_LIGHTSEAGREEN,
    UI_CSS_COLOR_LIGHTSKYBLUE,
    UI_CSS_COLOR_LIGHTSLATEGRAY,
    UI_CSS_COLOR_LIGHTSLATEGREY,
    UI_CSS_COLOR_LIGHTSTEELBLUE,
    UI_CSS_COLOR_LIGHTYELLOW,
    UI_CSS_COLOR_LIME,
    UI_CSS_COLOR_LIMEGREEN,
    UI_CSS_COLOR_LINEN,
    UI_CSS_COLOR_MAGENTA,
    UI_CSS_COLOR_MAROON,
    UI_CSS_COLOR_MEDIUMAQUAMARINE,
    UI_CSS_COLOR_MEDIUMBLUE,
    UI_CSS_COLOR_MEDIUMORCHID,
    UI_CSS_COLOR_MEDIUMPURPLE,
    UI_CSS_COLOR_MEDIUMSEAGREEN,
    UI_CSS_COLOR_MEDIUMSLATEBLUE,
    UI_CSS_COLOR_MEDIUMSPRINGGREEN,
    UI_CSS_COLOR_MEDIUMTURQUOISE,
    UI_CSS_COLOR_MEDIUMVIOLETRED,
    UI_CSS_COLOR_MIDNIGHTBLUE,
    UI_CSS_COLOR_MINTCREAM,
    UI_CSS_COLOR_MISTYROSE,
    UI_CSS_COLOR_MOCCASIN,
    UI_CSS_COLOR_NAVAJOWHITE,
    UI_CSS_COLOR_NAVY,
    UI_CSS_COLOR_OLDLACE,
    UI_CSS_COLOR_OLIVE,
    UI_CSS_COLOR_OLIVEDRAB,
    UI_CSS_COLOR_ORANGE,
    UI_CSS_COLOR_ORANGERED,
    UI_CSS_COLOR_ORCHID,
    UI_CSS_COLOR_PALEGOLDENROD,
    UI_CSS_COLOR_PALEGREEN,
    UI_CSS_COLOR_PALETURQUOISE,
    UI_CSS_COLOR_PALEVIOLETRED,
    UI_CSS_COLOR_PAPAYAWHIP,
    UI_CSS_COLOR_PEACHPUFF,
    UI_CSS_COLOR_PERU,
    UI_CSS_COLOR_PINK,
    UI_CSS_COLOR_PLUM,
    UI_CSS_COLOR_POWDERBLUE,
    UI_CSS_COLOR_PURPLE,
    UI_CSS_COLOR_REBECCAPURPLE,
    UI_CSS_COLOR_RED,
    UI_CSS_COLOR_ROSYBROWN,
    UI_CSS_COLOR_ROYALBLUE,
    UI_CSS_COLOR_SADDLEBROWN,
    UI_CSS_COLOR_SALMON,
    UI_CSS_COLOR_SANDYBROWN,
    UI_CSS_COLOR_SEAGREEN,
    UI_CSS_COLOR_SEASHELL,
    UI_CSS_COLOR_SIENNA,
    UI_CSS_COLOR_SILVER,
    UI_CSS_COLOR_SKYBLUE,
    UI_CSS_COLOR_SLATEBLUE,
    UI_CSS_COLOR_SLATEGRAY,
    UI_CSS_COLOR_SLATEGREY,
    UI_CSS_COLOR_SNOW,
    UI_CSS_COLOR_SPRINGGREEN,
    UI_CSS_COLOR_STEELBLUE,
    UI_CSS_COLOR_TAN,
    UI_CSS_COLOR_TEAL,
    UI_CSS_COLOR_THISTLE,
    UI_CSS_COLOR_TOMATO,
    UI_CSS_COLOR_TURQUOISE,
    UI_CSS_COLOR_VIOLET,
    UI_CSS_COLOR_WHEAT,
    UI_CSS_COLOR_WHITE,
    UI_CSS_COLOR_WHITESMOKE,
    UI_CSS_COLOR_YELLOW,
    UI_CSS_COLOR_YELLOWGREEN,
    UI_CSS_COLOR_CANVAS,
    UI_CSS_COLOR_CANVASTEXT,
    UI_CSS_COLOR_LINKTEXT,
    UI_CSS_COLOR_VISITEDTEXT,
    UI_CSS_COLOR_ACTIVETEXT,
    UI_CSS_COLOR_BUTTONFACE,
    UI_CSS_COLOR_BUTTONTEXT,
    UI_CSS_COLOR_BUTTONBORDER,
    UI_CSS_COLOR_FIELD,
    UI_CSS_COLOR_FIELDTEXT,
    UI_CSS_COLOR_HIGHLIGHT,
    UI_CSS_COLOR_HIGHLIGHTTEXT,
    UI_CSS_COLOR_SELECTEDITEM,
    UI_CSS_COLOR_SELECTEDITEMTEXT,
    UI_CSS_COLOR_MARK,
    UI_CSS_COLOR_MARKTEXT,
    UI_CSS_COLOR_GRAYTEXT,
    UI_CSS_COLOR_ACCENTCOLOR,
    UI_CSS_COLOR_ACCENTCOLORTEXT,
    UI_CSS_COLOR_RGB,
    UI_CSS_COLOR_RGBA,
    UI_CSS_COLOR_HSL,
    UI_CSS_COLOR_HSLA,
    UI_CSS_COLOR_HWB,
    UI_CSS_COLOR_LAB,
    UI_CSS_COLOR_LCH,
    UI_CSS_COLOR_OKLAB,
    UI_CSS_COLOR_OKLCH,
    UI_CSS_COLOR_COLOR,
};

typedef unsigned int ui_css_color_type_t;

enum
{
    UI_CSS_DIRECTION_LTR,
    UI_CSS_DIRECTION_RTL,
    UI_CSS_DIRECTION_INHERIT,
};

typedef unsigned int ui_css_direction_type_t;

enum
{
    UI_CSS_DISPLAY_BLOCK,
    UI_CSS_DISPLAY_INLINE,
    UI_CSS_DISPLAY_RUN_IN,
    UI_CSS_DISPLAY_FLOW,
    UI_CSS_DISPLAY_FLOW_ROOT,
    UI_CSS_DISPLAY_TABLE,
    UI_CSS_DISPLAY_FLEX,
    UI_CSS_DISPLAY_GRID,
    UI_CSS_DISPLAY_RUBY,
    UI_CSS_DISPLAY_LIST_ITEM,
    UI_CSS_DISPLAY_TABLE_ROW_GROUP,
    UI_CSS_DISPLAY_TABLE_HEADER_GROUP,
    UI_CSS_DISPLAY_TABLE_FOOTER_GROUP,
    UI_CSS_DISPLAY_TABLE_ROW,
    UI_CSS_DISPLAY_TABLE_CELL,
    UI_CSS_DISPLAY_TABLE_COLUMN_GROUP,
    UI_CSS_DISPLAY_TABLE_COLUMN,
    UI_CSS_DISPLAY_TABLE_CAPTION,
    UI_CSS_DISPLAY_RUBY_BASE,
    UI_CSS_DISPLAY_RUBY_TEXT,
    UI_CSS_DISPLAY_RUBY_BASE_CONTAINER,
    UI_CSS_DISPLAY_RUBY_TEXT_CONTAINER,
    UI_CSS_DISPLAY_CONTENTS,
    UI_CSS_DISPLAY_NONE,
    UI_CSS_DISPLAY_INLINE_BLOCK,
    UI_CSS_DISPLAY_INLINE_TABLE,
    UI_CSS_DISPLAY_INLINE_FLEX,
    UI_CSS_DISPLAY_INLINE_GRID,
};

typedef unsigned int ui_css_display_type_t;

enum
{
    UI_CSS_DOMINANT_BASELINE_AUTO,
    UI_CSS_DOMINANT_BASELINE_TEXT_BOTTOM,
    UI_CSS_DOMINANT_BASELINE_ALPHABETIC,
    UI_CSS_DOMINANT_BASELINE_IDEOGRAPHIC,
    UI_CSS_DOMINANT_BASELINE_MIDDLE,
    UI_CSS_DOMINANT_BASELINE_CENTRAL,
    UI_CSS_DOMINANT_BASELINE_MATHEMATICAL,
    UI_CSS_DOMINANT_BASELINE_HANGING,
    UI_CSS_DOMINANT_BASELINE_TEXT_TOP,
};

typedef unsigned int ui_css_dominant_baseline_type_t;

enum
{
    UI_CSS_FLEX_NONE,
};

typedef unsigned int ui_css_flex_type_t;

enum
{
    UI_CSS_FLEX_BASIS_AUTO,
    UI_CSS_FLEX_BASIS_LENGTH,
    UI_CSS_FLEX_BASIS_PERCENTAGE,
    UI_CSS_FLEX_BASIS_MIN_CONTENT,
    UI_CSS_FLEX_BASIS_MAX_CONTENT,
    UI_CSS_FLEX_BASIS_FIT_CONTENT,
    UI_CSS_FLEX_BASIS_STRETCH,
};

typedef unsigned int ui_css_flex_basis_type_t;

enum
{
    UI_CSS_FLEX_DIRECTION_ROW,
    UI_CSS_FLEX_DIRECTION_ROW_REVERSE,
    UI_CSS_FLEX_DIRECTION_COLUMN,
    UI_CSS_FLEX_DIRECTION_COLUMN_REVERSE,
};

typedef unsigned int ui_css_flex_direction_type_t;


enum
{
    UI_CSS_FLEX_GROW_NUMBER,
};

typedef unsigned int ui_css_flex_grow_type_t;

enum
{
    UI_CSS_FLEX_SHRINK_NUMBER,
};

typedef unsigned int ui_css_flex_shrink_type_t;

enum
{
    UI_CSS_FLEX_WRAP_NOWRAP,
    UI_CSS_FLEX_WRAP_WRAP,
    UI_CSS_FLEX_WRAP_WRAP_REVERSE,
};

typedef unsigned int ui_css_flex_wrap_type_t;

enum
{
    UI_CSS_FLOAT_BLOCK_START,
    UI_CSS_FLOAT_BLOCK_END,
    UI_CSS_FLOAT_INLINE_START,
    UI_CSS_FLOAT_INLINE_END,
    UI_CSS_FLOAT_SNAP_BLOCK,
    UI_CSS_FLOAT_START,
    UI_CSS_FLOAT_END,
    UI_CSS_FLOAT_NEAR,
    UI_CSS_FLOAT_SNAP_INLINE,
    UI_CSS_FLOAT_LEFT,
    UI_CSS_FLOAT_RIGHT,
    UI_CSS_FLOAT_TOP,
    UI_CSS_FLOAT_BOTTOM,
    UI_CSS_FLOAT_NONE,
};

typedef unsigned int ui_css_float_type_t;

enum
{
    UI_CSS_FLOAT_DEFER_INTEGER,
    UI_CSS_FLOAT_DEFER_LAST,
    UI_CSS_FLOAT_DEFER_NONE,
};

typedef unsigned int ui_css_float_defer_type_t;

enum
{
    UI_CSS_FLOAT_OFFSET_LENGTH,
    UI_CSS_FLOAT_OFFSET_PERCENTAGE,
};

typedef unsigned int ui_css_float_offset_type_t;

enum
{
    UI_CSS_FLOAT_REFERENCE_INLINE,
    UI_CSS_FLOAT_REFERENCE_COLUMN,
    UI_CSS_FLOAT_REFERENCE_REGION,
    UI_CSS_FLOAT_REFERENCE_PAGE,
};

typedef unsigned int ui_css_float_reference_type_t;

enum
{
    UI_CSS_FONT_FAMILY_SERIF,
    UI_CSS_FONT_FAMILY_SANS_SERIF,
    UI_CSS_FONT_FAMILY_CURSIVE,
    UI_CSS_FONT_FAMILY_FANTASY,
    UI_CSS_FONT_FAMILY_MONOSPACE,
    UI_CSS_FONT_FAMILY_SYSTEM_UI,
    UI_CSS_FONT_FAMILY_EMOJI,
    UI_CSS_FONT_FAMILY_MATH,
    UI_CSS_FONT_FAMILY_FANGSONG,
    UI_CSS_FONT_FAMILY_UI_SERIF,
    UI_CSS_FONT_FAMILY_UI_SANS_SERIF,
    UI_CSS_FONT_FAMILY_UI_MONOSPACE,
    UI_CSS_FONT_FAMILY_UI_ROUNDED,
    UI_CSS_FONT_FAMILY_CUSTOM,
};

typedef unsigned int ui_css_font_family_type_t;

enum
{
    UI_CSS_FONT_SIZE_XX_SMALL,
    UI_CSS_FONT_SIZE_X_SMALL,
    UI_CSS_FONT_SIZE_SMALL,
    UI_CSS_FONT_SIZE_MEDIUM,
    UI_CSS_FONT_SIZE_LARGE,
    UI_CSS_FONT_SIZE_X_LARGE,
    UI_CSS_FONT_SIZE_XX_LARGE,
    UI_CSS_FONT_SIZE_XXX_LARGE,
    UI_CSS_FONT_SIZE_LARGER,
    UI_CSS_FONT_SIZE_SMALLER,
    UI_CSS_FONT_SIZE_MATH,
    UI_CSS_FONT_SIZE_LENGTH,
};

typedef unsigned int ui_css_font_size_type_t;

enum
{
    UI_CSS_FONT_STRETCH_NORMAL,
    UI_CSS_FONT_STRETCH_PERCENTAGE,
    UI_CSS_FONT_STRETCH_ULTRA_CONDENSED,
    UI_CSS_FONT_STRETCH_EXTRA_CONDENSED,
    UI_CSS_FONT_STRETCH_CONDENSED,
    UI_CSS_FONT_STRETCH_SEMI_CONDENSED,
    UI_CSS_FONT_STRETCH_SEMI_EXPANDED,
    UI_CSS_FONT_STRETCH_EXPANDED,
    UI_CSS_FONT_STRETCH_EXTRA_EXPANDED,
    UI_CSS_FONT_STRETCH_ULTRA_EXPANDED,
};

typedef unsigned int ui_css_font_stretch_type_t;

enum
{
    UI_CSS_FONT_STYLE_NORMAL,
    UI_CSS_FONT_STYLE_ITALIC,
    UI_CSS_FONT_STYLE_OBLIQUE,
};

typedef unsigned int ui_css_font_style_type_t;

enum
{
    UI_CSS_FONT_WEIGHT_NORMAL,
    UI_CSS_FONT_WEIGHT_BOLD,
    UI_CSS_FONT_WEIGHT_NUMBER,
    UI_CSS_FONT_WEIGHT_BOLDER,
    UI_CSS_FONT_WEIGHT_LIGHTER,
};

typedef unsigned int ui_css_font_weight_type_t;

enum
{
    UI_CSS_HANGING_PUNCTUATION_NONE,
    UI_CSS_HANGING_PUNCTUATION_FIRST,
    UI_CSS_HANGING_PUNCTUATION_FORCE_END,
    UI_CSS_HANGING_PUNCTUATION_ALLOW_END,
    UI_CSS_HANGING_PUNCTUATION_LAST,
};

typedef unsigned int ui_css_hanging_punctuation_type_t;

enum
{
    UI_CSS_HEIGHT_AUTO,
    UI_CSS_HEIGHT_MIN_CONTENT,
    UI_CSS_HEIGHT_MAX_CONTENT,
    UI_CSS_HEIGHT_LENGTH,
    UI_CSS_HEIGHT_PERCENTAGE,
    UI_CSS_HEIGHT_NUMBER,
    UI_CSS_HEIGHT_ANGLE,
    UI_CSS_HEIGHT_FIT_CONTENT,
    UI_CSS_HEIGHT_STRETCH,
};

typedef unsigned int ui_css_height_type_t;

enum
{
    UI_CSS_HYPHENS_NONE,
    UI_CSS_HYPHENS_MANUAL,
    UI_CSS_HYPHENS_AUTO,
};

typedef unsigned int ui_css_hyphens_type_t;

enum
{
    UI_CSS_INSET_BLOCK_END_AUTO,
    UI_CSS_INSET_BLOCK_END_LENGTH,
    UI_CSS_INSET_BLOCK_END_PERCENTAGE,
};

typedef unsigned int ui_css_inset_block_end_type_t;

enum
{
    UI_CSS_INSET_BLOCK_START_AUTO,
    UI_CSS_INSET_BLOCK_START_LENGTH,
    UI_CSS_INSET_BLOCK_START_PERCENTAGE,
};

typedef unsigned int ui_css_inset_block_start_type_t;

enum
{
    UI_CSS_INSET_INLINE_END_AUTO,
    UI_CSS_INSET_INLINE_END_LENGTH,
    UI_CSS_INSET_INLINE_END_PERCENTAGE,
};

typedef unsigned int ui_css_inset_inline_end_type_t;

enum
{
    UI_CSS_INSET_INLINE_START_AUTO,
    UI_CSS_INSET_INLINE_START_LENGTH,
    UI_CSS_INSET_INLINE_START_PERCENTAGE,
};

typedef unsigned int ui_css_inset_inline_start_type_t;

enum
{
    UI_CSS_JUSTIFY_CONTENT_FLEX_START,
    UI_CSS_JUSTIFY_CONTENT_FLEX_END,
    UI_CSS_JUSTIFY_CONTENT_CENTER,
    UI_CSS_JUSTIFY_CONTENT_SPACE_BETWEEN,
    UI_CSS_JUSTIFY_CONTENT_SPACE_AROUND,
    UI_CSS_JUSTIFY_CONTENT_SPACE_EVENLY,
    UI_CSS_JUSTIFY_CONTENT_START,
    UI_CSS_JUSTIFY_CONTENT_END,
    UI_CSS_JUSTIFY_CONTENT_STRETCH,
    UI_CSS_JUSTIFY_CONTENT_AUTO,
};

typedef unsigned int ui_css_justify_content_type_t;

// Values match YGGridTrackType in External/yoga/yoga/YGEnums.h
enum
{
    UI_CSS_GRID_TRACK_AUTO,
    UI_CSS_GRID_TRACK_POINTS,
    UI_CSS_GRID_TRACK_PERCENT,
    UI_CSS_GRID_TRACK_FR,
    UI_CSS_GRID_TRACK_MINMAX,
};

typedef unsigned int ui_css_grid_track_type_t;

enum
{
    UI_CSS_GRID_LINE_AUTO,
    UI_CSS_GRID_LINE_INTEGER,
    UI_CSS_GRID_LINE_SPAN,
};

typedef unsigned int ui_css_grid_line_type_t;

enum
{
    UI_CSS_GRID_GAP_LENGTH,
    UI_CSS_GRID_GAP_PERCENTAGE,
};

typedef unsigned int ui_css_grid_gap_type_t;

enum
{
    UI_CSS_LETTER_SPACING_NORMAL,
    UI_CSS_LETTER_SPACING_LENGTH,
};

typedef unsigned int ui_css_letter_spacing_type_t;

enum
{
    UI_CSS_LINE_BREAK_AUTO,
    UI_CSS_LINE_BREAK_LOOSE,
    UI_CSS_LINE_BREAK_NORMAL,
    UI_CSS_LINE_BREAK_STRICT,
    UI_CSS_LINE_BREAK_ANYWHERE,
};

typedef unsigned int ui_css_line_break_type_t;

enum
{
    UI_CSS_LINE_HEIGHT_NORMAL,
    UI_CSS_LINE_HEIGHT_NUMBER,
    UI_CSS_LINE_HEIGHT_LENGTH,
    UI_CSS_LINE_HEIGHT_PERCENTAGE,
};

typedef unsigned int ui_css_line_height_type_t;

enum
{
    UI_CSS_MARGIN_AUTO,
    UI_CSS_MARGIN_LENGTH,
    UI_CSS_MARGIN_PERCENTAGE,
};

typedef unsigned int ui_css_margin_type_t, ui_css_margin_bottom_type_t, ui_css_margin_left_type_t,
                     ui_css_margin_right_type_t, ui_css_margin_top_type_t;

enum
{
    UI_CSS_MAX_HEIGHT_NONE,
    UI_CSS_MAX_HEIGHT_MIN_CONTENT,
    UI_CSS_MAX_HEIGHT_MAX_CONTENT,
    UI_CSS_MAX_HEIGHT_LENGTH,
    UI_CSS_MAX_HEIGHT_PERCENTAGE,
    UI_CSS_MAX_HEIGHT_NUMBER,
    UI_CSS_MAX_HEIGHT_ANGLE,
    UI_CSS_MAX_HEIGHT_FIT_CONTENT,
    UI_CSS_MAX_HEIGHT_STRETCH,
};

typedef unsigned int ui_css_max_height_type_t;

enum
{
    UI_CSS_MAX_WIDTH_NONE,
    UI_CSS_MAX_WIDTH_MIN_CONTENT,
    UI_CSS_MAX_WIDTH_MAX_CONTENT,
    UI_CSS_MAX_WIDTH_LENGTH,
    UI_CSS_MAX_WIDTH_PERCENTAGE,
    UI_CSS_MAX_WIDTH_NUMBER,
    UI_CSS_MAX_WIDTH_ANGLE,
    UI_CSS_MAX_WIDTH_FIT_CONTENT,
    UI_CSS_MAX_WIDTH_STRETCH,
};

typedef unsigned int ui_css_max_width_type_t;

enum
{
    UI_CSS_MIN_HEIGHT_AUTO,
    UI_CSS_MIN_HEIGHT_MIN_CONTENT,
    UI_CSS_MIN_HEIGHT_MAX_CONTENT,
    UI_CSS_MIN_HEIGHT_LENGTH,
    UI_CSS_MIN_HEIGHT_PERCENTAGE,
    UI_CSS_MIN_HEIGHT_NUMBER,
    UI_CSS_MIN_HEIGHT_ANGLE,
    UI_CSS_MIN_HEIGHT_FIT_CONTENT,
    UI_CSS_MIN_HEIGHT_STRETCH,
};

typedef unsigned int ui_css_min_height_type_t;

enum
{
    UI_CSS_MIN_WIDTH_AUTO,
    UI_CSS_MIN_WIDTH_MIN_CONTENT,
    UI_CSS_MIN_WIDTH_MAX_CONTENT,
    UI_CSS_MIN_WIDTH_LENGTH,
    UI_CSS_MIN_WIDTH_PERCENTAGE,
    UI_CSS_MIN_WIDTH_NUMBER,
    UI_CSS_MIN_WIDTH_ANGLE,
    UI_CSS_MIN_WIDTH_FIT_CONTENT,
    UI_CSS_MIN_WIDTH_STRETCH,
};

typedef unsigned int ui_css_min_width_type_t;

enum
{
    UI_CSS_OPACITY_NUMBER,
    UI_CSS_OPACITY_PERCENTAGE,
};

typedef unsigned int ui_css_opacity_type_t;

enum
{
    UI_CSS_ORDER_INTEGER,
};

typedef unsigned int ui_css_order_type_t;

enum
{
    UI_CSS_OVERFLOW_BLOCK_VISIBLE,
    UI_CSS_OVERFLOW_BLOCK_HIDDEN,
    UI_CSS_OVERFLOW_BLOCK_CLIP,
    UI_CSS_OVERFLOW_BLOCK_SCROLL,
    UI_CSS_OVERFLOW_BLOCK_AUTO,
};

typedef unsigned int ui_css_overflow_block_type_t;

enum
{
    UI_CSS_OVERFLOW_INLINE_VISIBLE,
    UI_CSS_OVERFLOW_INLINE_HIDDEN,
    UI_CSS_OVERFLOW_INLINE_CLIP,
    UI_CSS_OVERFLOW_INLINE_SCROLL,
    UI_CSS_OVERFLOW_INLINE_AUTO,
};

typedef unsigned int ui_css_overflow_inline_type_t;

enum
{
    UI_CSS_OVERFLOW_WRAP_NORMAL,
    UI_CSS_OVERFLOW_WRAP_BREAK_WORD,
    UI_CSS_OVERFLOW_WRAP_ANYWHERE,
};

typedef unsigned int ui_css_overflow_wrap_type_t;

enum
{
    UI_CSS_OVERFLOW_X_VISIBLE,
    UI_CSS_OVERFLOW_X_HIDDEN,
    UI_CSS_OVERFLOW_X_CLIP,
    UI_CSS_OVERFLOW_X_SCROLL,
    UI_CSS_OVERFLOW_X_AUTO,
};

typedef unsigned int ui_css_overflow_x_type_t;

enum
{
    UI_CSS_OVERFLOW_Y_VISIBLE,
    UI_CSS_OVERFLOW_Y_HIDDEN,
    UI_CSS_OVERFLOW_Y_CLIP,
    UI_CSS_OVERFLOW_Y_SCROLL,
    UI_CSS_OVERFLOW_Y_AUTO,
};

typedef unsigned int ui_css_overflow_y_type_t;

enum
{
    UI_CSS_PADDING_AUTO,
    UI_CSS_PADDING_LENGTH,
    UI_CSS_PADDING_PERCENTAGE,
};

typedef unsigned int ui_css_padding_type_t, ui_css_padding_bottom_type_t, ui_css_padding_left_type_t,
                     ui_css_padding_right_type_t, ui_css_padding_top_type_t;

enum
{
    UI_CSS_POSITION_STATIC,
    UI_CSS_POSITION_RELATIVE,
    UI_CSS_POSITION_ABSOLUTE,
    UI_CSS_POSITION_STICKY,
    UI_CSS_POSITION_FIXED,
};

typedef unsigned int ui_css_position_type_t;

enum
{
    UI_CSS_TOP_AUTO,
    UI_CSS_TOP_LENGTH,
    UI_CSS_TOP_PERCENTAGE,
};

typedef unsigned int ui_css_top_type_t;

enum
{
    UI_CSS_BOTTOM_AUTO,
    UI_CSS_BOTTOM_LENGTH,
    UI_CSS_BOTTOM_PERCENTAGE,
};

typedef unsigned int ui_css_bottom_type_t;

enum
{
    UI_CSS_LEFT_AUTO,
    UI_CSS_LEFT_LENGTH,
    UI_CSS_LEFT_PERCENTAGE,
};

typedef unsigned int ui_css_left_type_t;

enum
{
    UI_CSS_RIGHT_AUTO,
    UI_CSS_RIGHT_LENGTH,
    UI_CSS_RIGHT_PERCENTAGE,
};

typedef unsigned int ui_css_right_type_t;

enum
{
    UI_CSS_TAB_SIZE_NUMBER,
    UI_CSS_TAB_SIZE_LENGTH,
};

typedef unsigned int ui_css_tab_size_type_t;

enum
{
    UI_CSS_TEXT_ALIGN_START,
    UI_CSS_TEXT_ALIGN_END,
    UI_CSS_TEXT_ALIGN_LEFT,
    UI_CSS_TEXT_ALIGN_RIGHT,
    UI_CSS_TEXT_ALIGN_CENTER,
    UI_CSS_TEXT_ALIGN_JUSTIFY,
    UI_CSS_TEXT_ALIGN_MATCH_PARENT,
    UI_CSS_TEXT_ALIGN_JUSTIFY_ALL,
};

typedef unsigned int ui_css_text_align_type_t;

enum
{
    UI_CSS_TEXT_ALIGN_ALL_START,
    UI_CSS_TEXT_ALIGN_ALL_END,
    UI_CSS_TEXT_ALIGN_ALL_LEFT,
    UI_CSS_TEXT_ALIGN_ALL_RIGHT,
    UI_CSS_TEXT_ALIGN_ALL_CENTER,
    UI_CSS_TEXT_ALIGN_ALL_JUSTIFY,
    UI_CSS_TEXT_ALIGN_ALL_MATCH_PARENT,
};

typedef unsigned int ui_css_text_align_all_type_t;

enum
{
    UI_CSS_TEXT_ALIGN_LAST_AUTO,
    UI_CSS_TEXT_ALIGN_LAST_START,
    UI_CSS_TEXT_ALIGN_LAST_END,
    UI_CSS_TEXT_ALIGN_LAST_LEFT,
    UI_CSS_TEXT_ALIGN_LAST_RIGHT,
    UI_CSS_TEXT_ALIGN_LAST_CENTER,
    UI_CSS_TEXT_ALIGN_LAST_JUSTIFY,
    UI_CSS_TEXT_ALIGN_LAST_MATCH_PARENT,
};

typedef unsigned int ui_css_text_align_last_type_t;

enum
{
    UI_CSS_TEXT_COMBINE_UPRIGHT_NONE,
    UI_CSS_TEXT_COMBINE_UPRIGHT_ALL,
    UI_CSS_TEXT_COMBINE_UPRIGHT_DIGITS,
};

typedef unsigned int ui_css_text_combine_upright_type_t;


enum
{
    UI_CSS_TEXT_DECORATION_LINE_NONE,
    UI_CSS_TEXT_DECORATION_LINE_UNDERLINE,
    UI_CSS_TEXT_DECORATION_LINE_OVERLINE,
    UI_CSS_TEXT_DECORATION_LINE_LINE_THROUGH,
    UI_CSS_TEXT_DECORATION_LINE_BLINK,
};

typedef unsigned int ui_css_text_decoration_line_type_t;

enum
{
    UI_CSS_TEXT_DECORATION_STYLE_SOLID,
    UI_CSS_TEXT_DECORATION_STYLE_DOUBLE,
    UI_CSS_TEXT_DECORATION_STYLE_DOTTED,
    UI_CSS_TEXT_DECORATION_STYLE_DASHED,
    UI_CSS_TEXT_DECORATION_STYLE_WAVY,
};

typedef unsigned int ui_css_text_decoration_style_type_t;

enum
{
    UI_CSS_TEXT_INDENT_LENGTH,
    UI_CSS_TEXT_INDENT_PERCENTAGE,
    UI_CSS_TEXT_INDENT_HANGING,
    UI_CSS_TEXT_INDENT_EACH_LINE,
};

typedef unsigned int ui_css_text_indent_type_t;

enum
{
    UI_CSS_TEXT_JUSTIFY_AUTO,
    UI_CSS_TEXT_JUSTIFY_NONE,
    UI_CSS_TEXT_JUSTIFY_INTER_WORD,
    UI_CSS_TEXT_JUSTIFY_INTER_CHARACTER,
};

typedef unsigned int ui_css_text_justify_type_t;

enum
{
    UI_CSS_TEXT_ORIENTATION_MIXED,
    UI_CSS_TEXT_ORIENTATION_UPRIGHT,
    UI_CSS_TEXT_ORIENTATION_SIDEWAYS,
};

typedef unsigned int ui_css_text_orientation_type_t;

enum
{
    UI_CSS_TEXT_OVERFLOW_CLIP,
    UI_CSS_TEXT_OVERFLOW_ELLIPSIS,
};

typedef unsigned int ui_css_text_overflow_type_t;

enum
{
    UI_CSS_TEXT_TRANSFORM_NONE,
    UI_CSS_TEXT_TRANSFORM_CAPITALIZE,
    UI_CSS_TEXT_TRANSFORM_UPPERCASE,
    UI_CSS_TEXT_TRANSFORM_LOWERCASE,
    UI_CSS_TEXT_TRANSFORM_FULL_WIDTH,
    UI_CSS_TEXT_TRANSFORM_FULL_SIZE_KANA,
};

typedef unsigned int ui_css_text_transform_type_t;

enum
{
    UI_CSS_UNICODE_BIDI_NORMAL,
    UI_CSS_UNICODE_BIDI_EMBED,
    UI_CSS_UNICODE_BIDI_ISOLATE,
    UI_CSS_UNICODE_BIDI_BIDI_OVERRIDE,
    UI_CSS_UNICODE_BIDI_ISOLATE_OVERRIDE,
    UI_CSS_UNICODE_BIDI_PLAINTEXT,
};

typedef unsigned int ui_css_unicode_bidi_type_t;

enum
{
    UI_CSS_VERTICAL_ALIGN_FIRST,
    UI_CSS_VERTICAL_ALIGN_LAST,
};

typedef unsigned int ui_css_vertical_align_type_t;

enum
{
    UI_CSS_VISIBILITY_VISIBLE,
    UI_CSS_VISIBILITY_HIDDEN,
    UI_CSS_VISIBILITY_COLLAPSE,
};

typedef unsigned int ui_css_visibility_type_t;

enum
{
    UI_CSS_WHITE_SPACE_NORMAL,
    UI_CSS_WHITE_SPACE_PRE,
    UI_CSS_WHITE_SPACE_NOWRAP,
    UI_CSS_WHITE_SPACE_PRE_WRAP,
    UI_CSS_WHITE_SPACE_BREAK_SPACES,
    UI_CSS_WHITE_SPACE_PRE_LINE,
};

typedef unsigned int ui_css_white_space_type_t;

enum
{
    UI_CSS_WIDTH_AUTO,
    UI_CSS_WIDTH_MIN_CONTENT,
    UI_CSS_WIDTH_MAX_CONTENT,
    UI_CSS_WIDTH_LENGTH,
    UI_CSS_WIDTH_PERCENTAGE,
    UI_CSS_WIDTH_NUMBER,
    UI_CSS_WIDTH_ANGLE,
    UI_CSS_WIDTH_FIT_CONTENT,
    UI_CSS_WIDTH_STRETCH,
};

typedef unsigned int ui_css_width_type_t;

enum
{
    UI_CSS_WORD_BREAK_NORMAL,
    UI_CSS_WORD_BREAK_KEEP_ALL,
    UI_CSS_WORD_BREAK_BREAK_ALL,
    UI_CSS_WORD_BREAK_BREAK_WORD,
};

typedef unsigned int ui_css_word_break_type_t;

enum
{
    UI_CSS_WORD_SPACING_NORMAL,
    UI_CSS_WORD_SPACING_LENGTH,
};

typedef unsigned int ui_css_word_spacing_type_t;

enum
{
    UI_CSS_WORD_WRAP_NORMAL,
    UI_CSS_WORD_WRAP_BREAK_WORD,
    UI_CSS_WORD_WRAP_ANYWHERE,
};

typedef unsigned int ui_css_word_wrap_type_t;

enum
{
    UI_CSS_WRAP_FLOW_AUTO,
    UI_CSS_WRAP_FLOW_BOTH,
    UI_CSS_WRAP_FLOW_START,
    UI_CSS_WRAP_FLOW_END,
    UI_CSS_WRAP_FLOW_MINIMUM,
    UI_CSS_WRAP_FLOW_MAXIMUM,
    UI_CSS_WRAP_FLOW_CLEAR,
};

typedef unsigned int ui_css_wrap_flow_type_t;

enum
{
    UI_CSS_WRAP_THROUGH_WRAP,
    UI_CSS_WRAP_THROUGH_NONE,
};

typedef unsigned int ui_css_wrap_through_type_t;

enum
{
    UI_CSS_WRITING_MODE_HORIZONTAL_TB,
    UI_CSS_WRITING_MODE_VERTICAL_RL,
    UI_CSS_WRITING_MODE_VERTICAL_LR,
    UI_CSS_WRITING_MODE_SIDEWAYS_RL,
    UI_CSS_WRITING_MODE_SIDEWAYS_LR,
};

typedef unsigned int ui_css_writing_mode_type_t;

enum
{
    UI_CSS_Z_INDEX_AUTO,
    UI_CSS_Z_INDEX_INTEGER,
};

typedef unsigned int ui_css_z_index_type_t;

enum
{
    UI_CSS_FILTER_NONE,
    UI_CSS_FILTER_CUSTOM0,
    UI_CSS_FILTER_CUSTOM1,
    UI_CSS_FILTER_CUSTOM2,
    UI_CSS_FILTER_CUSTOM3,
    UI_CSS_FILTER_CUSTOM4,
    UI_CSS_FILTER_CUSTOM5,
    UI_CSS_FILTER_CUSTOM6,
    UI_CSS_FILTER_CUSTOM7,
    UI_CSS_FILTER_CUSTOM8,
    UI_CSS_FILTER_CUSTOM9,
};

typedef unsigned int ui_css_filter_type_t;

//================================================================================================================

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

struct UIPropBorderRadiusCorner
{
    float Horizontal = 0.0f;
    float Vertical = 0.0f;
    ui_css_border_radius_type_t HorizontalType = UI_CSS_BORDER_RADIUS_LENGTH;
    ui_css_border_radius_type_t VerticalType = UI_CSS_BORDER_RADIUS_LENGTH;
};

using UIPropBorderTopLeftRadius = UIPropBorderRadiusCorner;
using UIPropBorderTopRightRadius = UIPropBorderRadiusCorner;
using UIPropBorderBottomRightRadius = UIPropBorderRadiusCorner;
using UIPropBorderBottomLeftRadius = UIPropBorderRadiusCorner;

struct UIPropBorderRadius
{
    UIPropBorderTopLeftRadius TopLeft;
    UIPropBorderTopRightRadius TopRight;
    UIPropBorderBottomRightRadius BottomRight;
    UIPropBorderBottomLeftRadius BottomLeft;
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

struct UIPropBackground
{
    UIPropColor Color;
};

struct UIPropForeground
{
    UIPropColor Color;
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
    UIString Name;
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

static inline UIString UICssKey(UIString const& src)
{
    auto s = UITrim(src);
    for (auto& ch : s) ch = (char)std::tolower((unsigned char)ch);
    return s;
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

inline bool UIParseRadiusToken(UIString const& token, float& value, ui_css_border_radius_type_t& type)
{
    UICssMeasure measure;
    if (UIParseMeasure(token, measure) == false) return false;
    if (measure.Kind == UI_CSS_MEASURE_PERCENT) { type = UI_CSS_BORDER_RADIUS_PERCENTAGE; value = measure.Value; return true; }
    if (measure.Kind == UI_CSS_MEASURE_LENGTH) { type = UI_CSS_BORDER_RADIUS_LENGTH; value = measure.Value; return true; }
    return false;
}

inline UIString UIFormatRadiusToken(float value, ui_css_border_radius_type_t type)
{
    if (type == UI_CSS_BORDER_RADIUS_PERCENTAGE) return std::to_string(value) + "%";
    return std::to_string(value) + "px";
}

inline bool UIApplyRadiusSides(UIString const& text, UIPropBorderRadiusCorner* corners, bool vertical)
{
    auto tokens = UISplitCssTokens(text);
    if (tokens.empty() || tokens.size() > 4) return false;
    float values[4] = {};
    ui_css_border_radius_type_t types[4] = {};
    for (size_t i = 0; i < tokens.size(); ++i)
        if (UIParseRadiusToken(tokens[i], values[i], types[i]) == false) return false;
    size_t topRight = tokens.size() > 1 ? 1 : 0;
    size_t bottomRight = tokens.size() > 2 ? 2 : 0;
    size_t bottomLeft = tokens.size() > 3 ? 3 : topRight;
    size_t index[4] = {0, topRight, bottomRight, bottomLeft};
    for (size_t i = 0; i < 4; ++i)
    {
        if (vertical)
        {
            corners[i].Vertical = values[index[i]];
            corners[i].VerticalType = types[index[i]];
        }
        else
        {
            corners[i].Horizontal = values[index[i]];
            corners[i].HorizontalType = types[index[i]];
        }
    }
    return true;
}

template <>
inline bool UITypeC(UIString const& src, UIPropBorderRadius& dst)
{
    auto parts = UISplitCssSlash(src);
    if (parts.empty() || parts.size() > 2 || parts[0].empty()) return false;
    UIPropBorderRadiusCorner corners[4];
    if (UIApplyRadiusSides(parts[0], corners, false) == false) return false;
    if (parts.size() == 1)
    {
        for (auto& corner : corners)
        {
            corner.Vertical = corner.Horizontal;
            corner.VerticalType = corner.HorizontalType;
        }
    }
    else if (parts[1].empty() || UIApplyRadiusSides(parts[1], corners, true) == false) return false;
    dst.TopLeft = corners[0];
    dst.TopRight = corners[1];
    dst.BottomRight = corners[2];
    dst.BottomLeft = corners[3];
    return true;
}
template <>
inline bool UITypeC(UIPropBorderRadius const& src, UIString& dst)
{
    auto horizontal = UIFormatRadiusToken(src.TopLeft.Horizontal, src.TopLeft.HorizontalType) + " "
        + UIFormatRadiusToken(src.TopRight.Horizontal, src.TopRight.HorizontalType) + " "
        + UIFormatRadiusToken(src.BottomRight.Horizontal, src.BottomRight.HorizontalType) + " "
        + UIFormatRadiusToken(src.BottomLeft.Horizontal, src.BottomLeft.HorizontalType);
    auto elliptical = src.TopLeft.Horizontal != src.TopLeft.Vertical || src.TopLeft.HorizontalType != src.TopLeft.VerticalType
        || src.TopRight.Horizontal != src.TopRight.Vertical || src.TopRight.HorizontalType != src.TopRight.VerticalType
        || src.BottomRight.Horizontal != src.BottomRight.Vertical || src.BottomRight.HorizontalType != src.BottomRight.VerticalType
        || src.BottomLeft.Horizontal != src.BottomLeft.Vertical || src.BottomLeft.HorizontalType != src.BottomLeft.VerticalType;
    if (elliptical == false)
    {
        dst = horizontal;
        return true;
    }
    dst = horizontal + " / "
        + UIFormatRadiusToken(src.TopLeft.Vertical, src.TopLeft.VerticalType) + " "
        + UIFormatRadiusToken(src.TopRight.Vertical, src.TopRight.VerticalType) + " "
        + UIFormatRadiusToken(src.BottomRight.Vertical, src.BottomRight.VerticalType) + " "
        + UIFormatRadiusToken(src.BottomLeft.Vertical, src.BottomLeft.VerticalType);
    return true;
}

template <>
inline bool UITypeC(UIString const& src, UIPropBorderRadiusCorner& dst)
{
    auto tokens = UISplitCssTokens(src);
    if (tokens.size() == 1)
    {
        if (UIParseRadiusToken(tokens[0], dst.Horizontal, dst.HorizontalType) == false) return false;
        dst.Vertical = dst.Horizontal;
        dst.VerticalType = dst.HorizontalType;
        return true;
    }
    if (tokens.size() == 2)
    {
        if (UIParseRadiusToken(tokens[0], dst.Horizontal, dst.HorizontalType) == false) return false;
        return UIParseRadiusToken(tokens[1], dst.Vertical, dst.VerticalType);
    }
    return false;
}
template <>
inline bool UITypeC(UIPropBorderRadiusCorner const& src, UIString& dst)
{
    dst = UIFormatRadiusToken(src.Horizontal, src.HorizontalType);
    if (src.Horizontal != src.Vertical || src.HorizontalType != src.VerticalType)
        dst += " " + UIFormatRadiusToken(src.Vertical, src.VerticalType);
    return true;
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
    auto text = UITrim(src);
    if (text.size() >= 7 && text[0] == '#')
    {
        unsigned red = 0, green = 0, blue = 0, alpha = 255;
        int count = 0;
        if (text.size() == 7) count = std::sscanf(text.c_str(), "#%2x%2x%2x", &red, &green, &blue);
        else if (text.size() == 9) count = std::sscanf(text.c_str(), "#%2x%2x%2x%2x", &red, &green, &blue, &alpha);
        if ((text.size() == 7 && count == 3) || (text.size() == 9 && count == 4))
        {
            dst.Value.R = (uint8_t)red;
            dst.Value.G = (uint8_t)green;
            dst.Value.B = (uint8_t)blue;
            dst.Value.A = (uint8_t)alpha;
            dst.Type = UI_CSS_COLOR_HEX;
            return true;
        }
    }
    if (std::sscanf(text.c_str(), "rgba(%hhu,%hhu,%hhu,%hhu)", &dst.Value.R, &dst.Value.G, &dst.Value.B, &dst.Value.A) != 4) return false;
    dst.Type = UI_CSS_COLOR_RGBA;
    return true;
}
template <>
inline bool UITypeC(UIPropColor const& src, UIString& dst)
{
    char buffer[256] = {};
    int result = 0;
    if (src.Type == UI_CSS_COLOR_HEX)
    {
        if (src.Value.A == 255) result = std::snprintf(buffer, sizeof(buffer), "#%02x%02x%02x", src.Value.R, src.Value.G, src.Value.B);
        else result = std::snprintf(buffer, sizeof(buffer), "#%02x%02x%02x%02x", src.Value.R, src.Value.G, src.Value.B, src.Value.A);
    }
    else result = std::snprintf(buffer, sizeof(buffer), "rgba(%hhu,%hhu,%hhu,%hhu)", src.Value.R, src.Value.G, src.Value.B, src.Value.A);
    if (result <= 0) return false;
    dst = {buffer, (size_t) result};
    return true;
}

template <>
inline bool UITypeC(UIString const& src, UIPropBackground& dst)
{
    return UITypeC(src, dst.Color);
}
template <>
inline bool UITypeC(UIPropBackground const& src, UIString& dst)
{
    return UITypeC(src.Color, dst);
}

template <>
inline bool UITypeC(UIString const& src, UIPropForeground& dst)
{
    return UITypeC(src, dst.Color);
}
template <>
inline bool UITypeC(UIPropForeground const& src, UIString& dst)
{
    return UITypeC(src.Color, dst);
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
    default:
    case UI_CSS_DISPLAY_NONE: dst = "none"; return true;
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

// Font family: first name in the list, or a generic keyword
template <>
inline bool UITypeC(UIString const& src, UIPropFontFamily& dst)
{
    UIString token;
    bool quoted = false;
    char mark = 0;
    for (char c : src)
    {
        if (quoted == false && (c == '"' || c == '\''))
        {
            quoted = true;
            mark = c;
            continue;
        }
        if (quoted && c == mark) break;
        if (quoted == false && c == ',') break;
        token.push_back(c);
    }
    token = UITrim(token);
    if (token.empty()) return false;
    UIString key = token;
    for (auto& ch : key) ch = (char)std::tolower((unsigned char)ch);
    auto generic = [&](ui_css_font_family_type_t type, char const* face)
    {
        dst.Value = type;
        dst.Name = face;
    };
    switch (UIHash(key))
    {
    case UIHash("inherit"): dst = {}; return true;
    case UIHash("serif"): generic(UI_CSS_FONT_FAMILY_SERIF, "Times New Roman"); return true;
    case UIHash("sans-serif"): generic(UI_CSS_FONT_FAMILY_SANS_SERIF, "Arial"); return true;
    case UIHash("cursive"): generic(UI_CSS_FONT_FAMILY_CURSIVE, "Segoe Script"); return true;
    case UIHash("fantasy"): generic(UI_CSS_FONT_FAMILY_FANTASY, "Impact"); return true;
    case UIHash("monospace"): generic(UI_CSS_FONT_FAMILY_MONOSPACE, "Consolas"); return true;
    case UIHash("system-ui"): generic(UI_CSS_FONT_FAMILY_SYSTEM_UI, "Segoe UI"); return true;
    case UIHash("emoji"): generic(UI_CSS_FONT_FAMILY_EMOJI, "Segoe UI Emoji"); return true;
    case UIHash("math"): generic(UI_CSS_FONT_FAMILY_MATH, "Cambria Math"); return true;
    case UIHash("fangsong"): generic(UI_CSS_FONT_FAMILY_FANGSONG, "FangSong"); return true;
    case UIHash("ui-serif"): generic(UI_CSS_FONT_FAMILY_UI_SERIF, "Times New Roman"); return true;
    case UIHash("ui-sans-serif"): generic(UI_CSS_FONT_FAMILY_UI_SANS_SERIF, "Segoe UI"); return true;
    case UIHash("ui-monospace"): generic(UI_CSS_FONT_FAMILY_UI_MONOSPACE, "Consolas"); return true;
    case UIHash("ui-rounded"): generic(UI_CSS_FONT_FAMILY_UI_ROUNDED, "Segoe UI"); return true;
    default:
        dst.Value = UI_CSS_FONT_FAMILY_CUSTOM;
        dst.Name = token;
        return true;
    }
}
template <>
inline bool UITypeC(UIPropFontFamily const& src, UIString& dst)
{
    switch (src.Value)
    {
    case UI_CSS_FONT_FAMILY_SERIF: dst = "serif"; return true;
    case UI_CSS_FONT_FAMILY_SANS_SERIF: dst = "sans-serif"; return true;
    case UI_CSS_FONT_FAMILY_CURSIVE: dst = "cursive"; return true;
    case UI_CSS_FONT_FAMILY_FANTASY: dst = "fantasy"; return true;
    case UI_CSS_FONT_FAMILY_MONOSPACE: dst = "monospace"; return true;
    case UI_CSS_FONT_FAMILY_SYSTEM_UI: dst = "system-ui"; return true;
    case UI_CSS_FONT_FAMILY_EMOJI: dst = "emoji"; return true;
    case UI_CSS_FONT_FAMILY_MATH: dst = "math"; return true;
    case UI_CSS_FONT_FAMILY_FANGSONG: dst = "fangsong"; return true;
    case UI_CSS_FONT_FAMILY_UI_SERIF: dst = "ui-serif"; return true;
    case UI_CSS_FONT_FAMILY_UI_SANS_SERIF: dst = "ui-sans-serif"; return true;
    case UI_CSS_FONT_FAMILY_UI_MONOSPACE: dst = "ui-monospace"; return true;
    case UI_CSS_FONT_FAMILY_UI_ROUNDED: dst = "ui-rounded"; return true;
    case UI_CSS_FONT_FAMILY_CUSTOM:
        if (src.Name.empty()) return false;
        if (src.Name.find(' ') != UIString::npos || src.Name.find(',') != UIString::npos)
            dst = "\"" + src.Name + "\"";
        else
            dst = src.Name;
        return true;
    default: return false;
    }
}

// Font size
template <>
inline bool UITypeC(UIString const& src, UIPropFontSize& dst)
{
    auto s = UICssKey(src);
    if (s.empty()) return false;
    switch (UIHash(s))
    {
    case UIHash("inherit"): dst = {}; return true;
    case UIHash("xx-small"): dst.Type = UI_CSS_FONT_SIZE_XX_SMALL; return true;
    case UIHash("x-small"): dst.Type = UI_CSS_FONT_SIZE_X_SMALL; return true;
    case UIHash("small"): dst.Type = UI_CSS_FONT_SIZE_SMALL; return true;
    case UIHash("medium"): dst.Type = UI_CSS_FONT_SIZE_MEDIUM; return true;
    case UIHash("large"): dst.Type = UI_CSS_FONT_SIZE_LARGE; return true;
    case UIHash("x-large"): dst.Type = UI_CSS_FONT_SIZE_X_LARGE; return true;
    case UIHash("xx-large"): dst.Type = UI_CSS_FONT_SIZE_XX_LARGE; return true;
    case UIHash("xxx-large"): dst.Type = UI_CSS_FONT_SIZE_XXX_LARGE; return true;
    case UIHash("larger"): dst.Type = UI_CSS_FONT_SIZE_LARGER; return true;
    case UIHash("smaller"): dst.Type = UI_CSS_FONT_SIZE_SMALLER; return true;
    case UIHash("math"): dst.Type = UI_CSS_FONT_SIZE_MATH; return true;
    default: break;
    }
    UICssMeasure measure;
    if (UIParseMeasure(src, measure) == false || measure.Kind != UI_CSS_MEASURE_LENGTH) return false;
    dst.Type = UI_CSS_FONT_SIZE_LENGTH;
    dst.Value = measure.Value;
    return true;
}
template <>
inline bool UITypeC(UIPropFontSize const& src, UIString& dst)
{
    switch (src.Type)
    {
    case UI_CSS_FONT_SIZE_XX_SMALL: dst = "xx-small"; return true;
    case UI_CSS_FONT_SIZE_X_SMALL: dst = "x-small"; return true;
    case UI_CSS_FONT_SIZE_SMALL: dst = "small"; return true;
    case UI_CSS_FONT_SIZE_MEDIUM: dst = "medium"; return true;
    case UI_CSS_FONT_SIZE_LARGE: dst = "large"; return true;
    case UI_CSS_FONT_SIZE_X_LARGE: dst = "x-large"; return true;
    case UI_CSS_FONT_SIZE_XX_LARGE: dst = "xx-large"; return true;
    case UI_CSS_FONT_SIZE_XXX_LARGE: dst = "xxx-large"; return true;
    case UI_CSS_FONT_SIZE_LARGER: dst = "larger"; return true;
    case UI_CSS_FONT_SIZE_SMALLER: dst = "smaller"; return true;
    case UI_CSS_FONT_SIZE_MATH: dst = "math"; return true;
    case UI_CSS_FONT_SIZE_LENGTH: dst = std::to_string(src.Value) + "px"; return true;
    default: return false;
    }
}

// Font stretch: accept percentage
template <>
inline bool UITypeC(UIString const& src, UIPropFontStretch& dst)
{
    auto s = UICssKey(src);
    if (s.empty()) return false;
    switch (UIHash(s))
    {
    case UIHash("inherit"):
    case UIHash("normal"): dst = {}; return true;
    case UIHash("ultra-condensed"): dst.Type = UI_CSS_FONT_STRETCH_ULTRA_CONDENSED; return true;
    case UIHash("extra-condensed"): dst.Type = UI_CSS_FONT_STRETCH_EXTRA_CONDENSED; return true;
    case UIHash("condensed"): dst.Type = UI_CSS_FONT_STRETCH_CONDENSED; return true;
    case UIHash("semi-condensed"): dst.Type = UI_CSS_FONT_STRETCH_SEMI_CONDENSED; return true;
    case UIHash("semi-expanded"): dst.Type = UI_CSS_FONT_STRETCH_SEMI_EXPANDED; return true;
    case UIHash("expanded"): dst.Type = UI_CSS_FONT_STRETCH_EXPANDED; return true;
    case UIHash("extra-expanded"): dst.Type = UI_CSS_FONT_STRETCH_EXTRA_EXPANDED; return true;
    case UIHash("ultra-expanded"): dst.Type = UI_CSS_FONT_STRETCH_ULTRA_EXPANDED; return true;
    default: break;
    }
    if (s.ends_with('%') == false) return false;
    dst.Type = UI_CSS_FONT_STRETCH_PERCENTAGE;
    dst.Value = std::stof(s);
    return true;
}
template <>
inline bool UITypeC(UIPropFontStretch const& src, UIString& dst)
{
    switch (src.Type)
    {
    case UI_CSS_FONT_STRETCH_NORMAL: dst = "normal"; return true;
    case UI_CSS_FONT_STRETCH_ULTRA_CONDENSED: dst = "ultra-condensed"; return true;
    case UI_CSS_FONT_STRETCH_EXTRA_CONDENSED: dst = "extra-condensed"; return true;
    case UI_CSS_FONT_STRETCH_CONDENSED: dst = "condensed"; return true;
    case UI_CSS_FONT_STRETCH_SEMI_CONDENSED: dst = "semi-condensed"; return true;
    case UI_CSS_FONT_STRETCH_SEMI_EXPANDED: dst = "semi-expanded"; return true;
    case UI_CSS_FONT_STRETCH_EXPANDED: dst = "expanded"; return true;
    case UI_CSS_FONT_STRETCH_EXTRA_EXPANDED: dst = "extra-expanded"; return true;
    case UI_CSS_FONT_STRETCH_ULTRA_EXPANDED: dst = "ultra-expanded"; return true;
    case UI_CSS_FONT_STRETCH_PERCENTAGE: dst = std::to_string(src.Value) + "%"; return true;
    default: return false;
    }
}

template <>
inline bool UITypeC(UIString const& src, UIPropFontStyle& dst)
{
    auto s = UICssKey(src);
    switch (UIHash(s))
    {
    case UIHash("inherit"):
    case UIHash("normal"): dst = {}; return true;
    case UIHash("italic"): dst.Value = UI_CSS_FONT_STYLE_ITALIC; return true;
    case UIHash("oblique"): dst.Value = UI_CSS_FONT_STYLE_OBLIQUE; return true;
    default: return false;
    }
}
template <>
inline bool UITypeC(UIPropFontStyle const& src, UIString& dst)
{
    switch (src.Value)
    {
    case UI_CSS_FONT_STYLE_NORMAL: dst = "normal"; return true;
    case UI_CSS_FONT_STYLE_ITALIC: dst = "italic"; return true;
    case UI_CSS_FONT_STYLE_OBLIQUE: dst = "oblique"; return true;
    default: return false;
    }
}

// Font weight (number)
template <>
inline bool UITypeC(UIString const& src, UIPropFontWeight& dst)
{
    auto s = UICssKey(src);
    if (s.empty()) return false;
    if (s[0] == '+' || s[0] == '-' || s[0] == '.' || isdigit((unsigned char)s[0]))
    {
        dst.Type = UI_CSS_FONT_WEIGHT_NUMBER;
        dst.Value = std::stof(s);
        return true;
    }
    auto number = [&](float value)
    {
        dst.Type = UI_CSS_FONT_WEIGHT_NUMBER;
        dst.Value = value;
    };
    switch (UIHash(s))
    {
    case UIHash("inherit"):
    case UIHash("normal"): dst = {}; return true;
    case UIHash("bold"): dst.Type = UI_CSS_FONT_WEIGHT_BOLD; return true;
    case UIHash("bolder"): dst.Type = UI_CSS_FONT_WEIGHT_BOLDER; return true;
    case UIHash("lighter"): dst.Type = UI_CSS_FONT_WEIGHT_LIGHTER; return true;
    case UIHash("thin"): number(100); return true;
    case UIHash("extra-light"):
    case UIHash("ultra-light"): number(200); return true;
    case UIHash("light"): number(300); return true;
    case UIHash("medium"): number(500); return true;
    case UIHash("semi-bold"):
    case UIHash("demi-bold"): number(600); return true;
    case UIHash("extra-bold"):
    case UIHash("ultra-bold"): number(800); return true;
    case UIHash("black"):
    case UIHash("heavy"): number(900); return true;
    default: return false;
    }
}
template <>
inline bool UITypeC(UIPropFontWeight const& src, UIString& dst)
{
    switch (src.Type)
    {
    case UI_CSS_FONT_WEIGHT_NORMAL: dst = "normal"; return true;
    case UI_CSS_FONT_WEIGHT_BOLD: dst = "bold"; return true;
    case UI_CSS_FONT_WEIGHT_BOLDER: dst = "bolder"; return true;
    case UI_CSS_FONT_WEIGHT_LIGHTER: dst = "lighter"; return true;
    case UI_CSS_FONT_WEIGHT_NUMBER: dst = std::to_string((int)(src.Value + 0.5f)); return true;
    default: return false;
    }
}

template <>
inline bool UITypeC(UIString const& src, UIPropTextAlign& dst)
{
    auto s = UICssKey(src);
    switch (UIHash(s))
    {
    case UIHash("inherit"):
    case UIHash("start"): dst.Value = UI_CSS_TEXT_ALIGN_START; return true;
    case UIHash("end"): dst.Value = UI_CSS_TEXT_ALIGN_END; return true;
    case UIHash("left"): dst.Value = UI_CSS_TEXT_ALIGN_LEFT; return true;
    case UIHash("right"): dst.Value = UI_CSS_TEXT_ALIGN_RIGHT; return true;
    case UIHash("center"): dst.Value = UI_CSS_TEXT_ALIGN_CENTER; return true;
    case UIHash("justify"): dst.Value = UI_CSS_TEXT_ALIGN_JUSTIFY; return true;
    case UIHash("match-parent"): dst.Value = UI_CSS_TEXT_ALIGN_MATCH_PARENT; return true;
    case UIHash("justify-all"): dst.Value = UI_CSS_TEXT_ALIGN_JUSTIFY_ALL; return true;
    default: return false;
    }
}
template <>
inline bool UITypeC(UIPropTextAlign const& src, UIString& dst)
{
    switch (src.Value)
    {
    case UI_CSS_TEXT_ALIGN_START: dst = "start"; return true;
    case UI_CSS_TEXT_ALIGN_END: dst = "end"; return true;
    case UI_CSS_TEXT_ALIGN_LEFT: dst = "left"; return true;
    case UI_CSS_TEXT_ALIGN_RIGHT: dst = "right"; return true;
    case UI_CSS_TEXT_ALIGN_CENTER: dst = "center"; return true;
    case UI_CSS_TEXT_ALIGN_JUSTIFY: dst = "justify"; return true;
    case UI_CSS_TEXT_ALIGN_MATCH_PARENT: dst = "match-parent"; return true;
    case UI_CSS_TEXT_ALIGN_JUSTIFY_ALL: dst = "justify-all"; return true;
    default: return false;
    }
}

template <>
inline bool UITypeC(UIString const& src, UIPropTextOverflow& dst)
{
    auto s = UICssKey(src);
    switch (UIHash(s))
    {
    case UIHash("clip"): dst.Value = UI_CSS_TEXT_OVERFLOW_CLIP; return true;
    case UIHash("ellipsis"): dst.Value = UI_CSS_TEXT_OVERFLOW_ELLIPSIS; return true;
    default: return false;
    }
}
template <>
inline bool UITypeC(UIPropTextOverflow const& src, UIString& dst)
{
    switch (src.Value)
    {
    case UI_CSS_TEXT_OVERFLOW_CLIP: dst = "clip"; return true;
    case UI_CSS_TEXT_OVERFLOW_ELLIPSIS: dst = "ellipsis"; return true;
    default: return false;
    }
}

template <>
inline bool UITypeC(UIString const& src, UIPropWhiteSpace& dst)
{
    auto s = UICssKey(src);
    switch (UIHash(s))
    {
    case UIHash("inherit"):
    case UIHash("normal"): dst.Value = UI_CSS_WHITE_SPACE_NORMAL; return true;
    case UIHash("pre"): dst.Value = UI_CSS_WHITE_SPACE_PRE; return true;
    case UIHash("nowrap"): dst.Value = UI_CSS_WHITE_SPACE_NOWRAP; return true;
    case UIHash("pre-wrap"): dst.Value = UI_CSS_WHITE_SPACE_PRE_WRAP; return true;
    case UIHash("break-spaces"): dst.Value = UI_CSS_WHITE_SPACE_BREAK_SPACES; return true;
    case UIHash("pre-line"): dst.Value = UI_CSS_WHITE_SPACE_PRE_LINE; return true;
    default: return false;
    }
}
template <>
inline bool UITypeC(UIPropWhiteSpace const& src, UIString& dst)
{
    switch (src.Value)
    {
    case UI_CSS_WHITE_SPACE_NORMAL: dst = "normal"; return true;
    case UI_CSS_WHITE_SPACE_PRE: dst = "pre"; return true;
    case UI_CSS_WHITE_SPACE_NOWRAP: dst = "nowrap"; return true;
    case UI_CSS_WHITE_SPACE_PRE_WRAP: dst = "pre-wrap"; return true;
    case UI_CSS_WHITE_SPACE_BREAK_SPACES: dst = "break-spaces"; return true;
    case UI_CSS_WHITE_SPACE_PRE_LINE: dst = "pre-line"; return true;
    default: return false;
    }
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
inline bool UITypeC(UIString const& src, UIPropWordSpacing& dst)
{
    auto s = UITrim(src);
    if (s == "normal" || s == "inherit")
    {
        dst.Type = UI_CSS_WORD_SPACING_NORMAL;
    }
    else
    {
        dst.Type = UI_CSS_WORD_SPACING_LENGTH;
        dst.Value = std::stof(s);
    }
    return true;
}
template<>
inline bool UITypeC(UIPropWordSpacing const& src, UIString& dst)
{
    switch (src.Type) {
    default: return false;
    case UI_CSS_WORD_SPACING_NORMAL: dst = "normal"; return true;
    case UI_CSS_WORD_SPACING_LENGTH: dst = std::to_string(src.Value) + "px"; return true;
    }
}

template<>
inline bool UITypeC(UIString const& src, UIPropLineHeight& dst)
{
    auto s = UITrim(src);
    if (s.empty()) return false;
    if (s == "normal" || s == "inherit")
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
        bool unit = false;
        for (unsigned char ch : s) if (std::isalpha(ch)) unit = true;
        dst.Type = unit ? UI_CSS_LINE_HEIGHT_LENGTH : UI_CSS_LINE_HEIGHT_NUMBER;
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
    case UI_CSS_LINE_HEIGHT_NUMBER: dst = std::to_string(src.Value); return true;
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


