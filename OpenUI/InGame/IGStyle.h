#pragma once
#include "OpenUI/UIWidget.h"

struct IGFrameStyle
{
	UIPen Pen{ .Style = UIPen::NoPen, };
	UIBrush Brush{ .Style = UIBrush::NoBrush, };
};

inline void IGDrawBox(UIPainterRaw painter, UIRect rect, UIPen const& pen, UIBrush const& brush, float radius = 2.0f)
{
	if (painter == nullptr) return;
	if (pen.Style == UIPen::NoPen && brush.Style == UIBrush::NoBrush) return;
	painter->setPen(pen);
	painter->setBrush(brush);
	if (radius < 0.5f) painter->drawRect(rect.X, rect.Y, rect.W, rect.H);
	else painter->drawRoundedRect(rect.X, rect.Y, rect.W, rect.H, radius, radius);
}

inline void IGDrawText(UIPainterRaw painter, UIRect rect, UIString const& text, UIFont const& font, UIBrush const& brush)
{
	if (painter == nullptr || text.empty()) return;
	painter->setPen(UINoPen);
	painter->setFont(font);
	painter->setBrush(brush);
	painter->drawText(rect.X, rect.Y, rect.W, rect.H, text);
}
