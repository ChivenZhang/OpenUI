#include "IGProgress.h"
#include "IGStyle.h"
#include <algorithm>

struct IGProgressPrivate : UIPrivate
{
	float Value = 0;
	UIString Text;
	UIFont Font{ .Align = UIFont::AlignCenter | UIFont::AlignVCenter, };
};
#define PRIVATE() ((IGProgressPrivate*)m_Private)

IGProgress::IGProgress(UICanvasRaw canvas)
	:
	UIWidget(canvas)
{
	m_Private = new IGProgressPrivate;
}

IGProgress::~IGProgress()
{
	delete m_Private; m_Private = nullptr;
}

void IGProgress::paint(UIRect client, UIPainterRaw painter)
{
	UIWidget::paint(client, painter);
	IGDrawBox(painter, client, UIFramePen, UIInsetBrush, 2);
	float span = std::clamp(PRIVATE()->Value, 0.0f, 1.0f) * std::max(0.0f, client.W - 4);
	if (span > 0)
	{
		UIRect fill{ client.X + 2, client.Y + 2, span, std::max(0.0f, client.H - 4) };
		IGDrawBox(painter, fill, UIAccentPen, UIAccentBrush, 1);
	}
	IGDrawText(painter, client, PRIVATE()->Text, PRIVATE()->Font, UITextBrush);
}

float IGProgress::getValue() const
{
	return PRIVATE()->Value;
}

void IGProgress::setValue(float value)
{
	PRIVATE()->Value = std::clamp(value, 0.0f, 1.0f);
}

void IGProgress::setText(UIString const& text)
{
	PRIVATE()->Text = text;
}
#undef PRIVATE
