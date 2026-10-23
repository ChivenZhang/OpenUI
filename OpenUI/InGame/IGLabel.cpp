#include "IGLabel.h"
#include "IGStyle.h"

struct IGLabelPrivate : UIPrivate
{
	UIString Text;
	UIFont Font{ .Align = UIFont::AlignLeft | UIFont::AlignVCenter, };
	UIBrush Color = UITextBrush;
};
#define PRIVATE() ((IGLabelPrivate*)m_Private)

IGLabel::IGLabel(UICanvasRaw canvas)
	:
	UIWidget(canvas)
{
	m_Private = new IGLabelPrivate;
}

IGLabel::~IGLabel()
{
	delete m_Private; m_Private = nullptr;
}

void IGLabel::paint(UIRect client, UIPainterRaw painter)
{
	UIWidget::paint(client, painter);
	IGDrawText(painter, client, PRIVATE()->Text, PRIVATE()->Font, PRIVATE()->Color);
}

UIString IGLabel::getText() const
{
	return PRIVATE()->Text;
}

void IGLabel::setText(UIString const& text)
{
	PRIVATE()->Text = text;
}

void IGLabel::setFont(UIFont const& font)
{
	PRIVATE()->Font = font;
}

void IGLabel::setColor(UIBrush const& brush)
{
	PRIVATE()->Color = brush;
}
#undef PRIVATE
