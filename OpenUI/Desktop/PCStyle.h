#pragma once
#include "OpenUI/UIWidget.h"

inline UIColor PCRgb(int r, int g, int b, int a = 255)
{
	return { r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f };
}

inline UIPen PCPen(UIColor color, float width = 1.0f)
{
	return { .Color = color, .Width = width };
}

inline UIBrush PCBrush(UIColor color)
{
	return { .Color = color };
}

inline UIColor PCInk() { return UICtrlInk; }
inline UIColor PCMuted() { return UICtrlMuted; }
inline UIColor PCLine() { return UICtrlLine; }
inline UIColor PCLineHover() { return UICtrlLineHover; }
inline UIColor PCPaper() { return UICtrlPaper; }
inline UIColor PCCanvas() { return UICtrlCanvas; }
inline UIColor PCHover() { return UICtrlHover; }
inline UIColor PCPress() { return UICtrlPress; }
inline UIColor PCTrack() { return UICtrlTrack; }
inline UIColor PCAccent() { return UICtrlAccent; }
inline UIColor PCAccentHover() { return UICtrlAccentHover; }
inline UIColor PCAccentPress() { return UICtrlAccentPress; }
inline UIColor PCOnAccent() { return UICtrlOnAccent; }
inline UIColor PCDisabledLine() { return UICtrlDisabledLine; }
inline UIColor PCDisabledFill() { return UICtrlDisabledFill; }
inline UIColor PCDisabledInk() { return UICtrlDisabledInk; }

inline UIFont PCFont(uint32_t align)
{
	UIFont font;
	font.Family = "Segoe UI";
	font.Size = 13;
	font.Align = align;
	font.Ellipsize = UIFont::EllipsizeEnd;
	return font;
}

inline UIPropColor PCProp(UIColor const& color)
{
	UIPropColor result;
	result.Type = UI_CSS_COLOR_RGBA;
	result.Value.R = (uint8_t)(color.R * 255.0f + 0.5f);
	result.Value.G = (uint8_t)(color.G * 255.0f + 0.5f);
	result.Value.B = (uint8_t)(color.B * 255.0f + 0.5f);
	result.Value.A = (uint8_t)(color.A * 255.0f + 0.5f);
	return result;
}

inline UIColor PCRead(UIPropColor const& color)
{
	return { color.Value.R / 255.0f, color.Value.G / 255.0f, color.Value.B / 255.0f, color.Value.A / 255.0f };
}

inline void PCRadius(UIWidgetStyle& style, float radius)
{
	UIPropBorderRadiusCorner corner;
	corner.Horizontal = radius;
	corner.Vertical = radius;
	corner.HorizontalType = UI_CSS_BORDER_RADIUS_LENGTH;
	corner.VerticalType = UI_CSS_BORDER_RADIUS_LENGTH;
	style.BorderRadius.TopLeft = corner;
	style.BorderRadius.TopRight = corner;
	style.BorderRadius.BottomRight = corner;
	style.BorderRadius.BottomLeft = corner;
}

/// Writes border, background, foreground, and radius into one state. CSS setStyleText later overwrites the same fields.
inline void PCPlate(UIWidgetStyle& style, UIPen const& border, UIBrush const& fill, UIBrush const& text, float radius)
{
	if (border.Style == UIPen::NoPen)
	{
		style.BorderColor = {};
		style.Border.Left = {};
		style.Border.Top = {};
		style.Border.Right = {};
		style.Border.Bottom = {};
	}
	else
	{
		style.BorderColor = PCProp(border.Color);
		style.Border.Left = { border.Width, UI_CSS_BORDER_LENGTH };
		style.Border.Top = { border.Width, UI_CSS_BORDER_LENGTH };
		style.Border.Right = { border.Width, UI_CSS_BORDER_LENGTH };
		style.Border.Bottom = { border.Width, UI_CSS_BORDER_LENGTH };
	}
	style.Background.Color = fill.Style == UIBrush::NoBrush ? UIPropColor{} : PCProp(fill.Color);
	style.Foreground.Color = text.Style == UIBrush::NoBrush ? UIPropColor{} : PCProp(text.Color);
	PCRadius(style, radius);
}

struct PCFace
{
	UIPen Border = UINoPen;
	UIBrush Fill{ .Style = UIBrush::NoBrush };
	UIBrush Text{ .Style = UIBrush::NoBrush };
	float Radius = 0;
};

inline PCFace PCFaceOf(UIWidgetStyle const& style, UIRect const& client)
{
	PCFace face;
	auto border = PCRead(style.BorderColor);
	float width = 0.0f;
	if (style.Border.Top.Type == UI_CSS_BORDER_LENGTH) width = style.Border.Top.Value;
	else if (style.Border.Top.Type == UI_CSS_BORDER_THIN) width = 1.0f;
	else if (style.Border.Top.Type == UI_CSS_BORDER_MEDIUM) width = 3.0f;
	else if (style.Border.Top.Type == UI_CSS_BORDER_THICK) width = 5.0f;
	if (border.A > 0.0f && width > 0.0f) face.Border = PCPen(border, width);
	auto fill = PCRead(style.Background.Color);
	if (fill.A > 0.0f) face.Fill = PCBrush(fill);
	auto text = PCRead(style.Foreground.Color);
	if (text.A > 0.0f) face.Text = PCBrush(text);
	auto radius = style.BorderRadius.TopLeft;
	face.Radius = radius.HorizontalType == UI_CSS_BORDER_RADIUS_PERCENTAGE ? client.W * radius.Horizontal / 100.0f : radius.Horizontal;
	return face;
}

inline void PCDrawFace(UIPainterRaw painter, UIRect rect, PCFace const& face)
{
	if (painter == nullptr) return;
	if (face.Border.Style == UIPen::NoPen && face.Fill.Style == UIBrush::NoBrush) return;
	painter->setPen(face.Border);
	painter->setBrush(face.Fill);
	if (face.Radius < 0.5f) painter->drawRect(rect.X, rect.Y, rect.W, rect.H);
	else painter->drawRoundedRect(rect.X, rect.Y, rect.W, rect.H, face.Radius, face.Radius);
}

inline void PCDrawText(UIPainterRaw painter, UIRect rect, UIString const& text, UIFont const& font, UIBrush const& brush)
{
	if (painter == nullptr || text.empty() || brush.Style == UIBrush::NoBrush) return;
	painter->setPen(UINoPen);
	painter->setFont(font);
	painter->setBrush(brush);
	painter->drawText(rect.X, rect.Y, rect.W, rect.H, text);
}
