#include "RmGUILabel.h"

class RmGUILabelPrivate : public RmGUIWidgetPrivate
{
public:
	RmGUILabelTextStyle TextStyle;
	RmString Text;
	RmGUISignalAs<RmString> LinkHovered;
	RmGUISignalAs<RmString> LinkActivated;
};
#define PRIVATE() ((RmGUILabelPrivate*)m_PrivateLabel)

RmGUILabel::RmGUILabel(IRmGUIWidgetRaw parent)
	:
	RmGUIControl(parent),
	m_PrivateLabel(nullptr),
	linkHovered(nullptr),
	linkActivated(nullptr)
{
	m_PrivateLabel = new RmGUILabelPrivate;
	PRIVATE()->TextStyle.Pen = { .Style = RmPen::NoPen, };
	PRIVATE()->TextStyle.Font = { .Align = RmFont::AlignLeft | RmFont::AlignVCenter, };
	PRIVATE()->TextStyle.Brush = {};
	linkHovered = &PRIVATE()->LinkHovered;
	linkActivated = &PRIVATE()->LinkActivated;
}

RmGUILabel::~RmGUILabel()
{
	delete m_PrivateLabel; m_PrivateLabel = nullptr;
}

void RmGUILabel::paint(IRmGUIPainterRaw painter, RmRectRaw client)
{
	RmGUIWidget::paint(painter, client);
	if (PRIVATE()->Text.empty() == false)
	{
		painter->setPen(PRIVATE()->TextStyle.Pen);
		painter->setFont(PRIVATE()->TextStyle.Font);
		painter->setBrush(PRIVATE()->TextStyle.Brush);
		painter->drawText(client->X, client->Y, client->W, client->H, PRIVATE()->Text);
	}
}

RmString RmGUILabel::getText() const
{
	return PRIVATE()->Text;
}

void RmGUILabel::setText(RmString const& text)
{
	PRIVATE()->Text = text;
}

RmGUILabelTextStyle RmGUILabel::getStyle() const
{
	return PRIVATE()->TextStyle;
}

void RmGUILabel::setStyle(RmGUILabelTextStyle const& style)
{
	PRIVATE()->TextStyle = style;
}

RmFontAligns RmGUILabel::getAlignment() const
{
	return PRIVATE()->TextStyle.Font.Align;
}

void RmGUILabel::setAlignment(RmFontAligns value)
{
	PRIVATE()->TextStyle.Font.Align = value;
}

void RmGUILabel::mousePressEvent(IRmGUIMouseEventRaw event)
{
	auto client = getRect();
	auto viewport = getViewport();
	if (client.X <= event->X && event->X <= client.X + client.W
		&& client.Y <= event->Y && event->Y <= client.Y + client.H
		&& viewport.X <= event->X && event->X <= viewport.X + viewport.W
		&& viewport.Y <= event->Y && event->Y <= viewport.Y + viewport.H)
	{
		if (event->Button == 1)
		{
			PRIVATE()->LinkActivated.emit(PRIVATE()->Text);
		}
	}
}

void RmGUILabel::mouseMoveEvent(IRmGUIMouseEventRaw event)
{
	auto client = getRect();
	auto viewport = getViewport();
	if (client.X <= event->X && event->X <= client.X + client.W
		&& client.Y <= event->Y && event->Y <= client.Y + client.H
		&& viewport.X <= event->X && event->X <= viewport.X + viewport.W
		&& viewport.Y <= event->Y && event->Y <= viewport.Y + viewport.H)
	{
		PRIVATE()->LinkHovered.emit(PRIVATE()->Text);
	}
}