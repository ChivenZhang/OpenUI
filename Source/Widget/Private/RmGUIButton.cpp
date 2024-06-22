#include "RmGUIButton.h"

struct RmGUIButtonStyle
{
	RmPen Pen;
	RmBrush Brush;
	RmFloat2 Round = { 4, 4 };
};

struct RmGUIButtonTextStyle
{
	RmPen Pen;
	RmBrush Brush;
};

class RmGUIButtonPrivateData : public RmGUIWidgetPrivate
{
public:
	RmGUIButtonStyle Normal, Hover, Press, Disable;
	RmGUIButtonTextStyle TextStyle;
	RmString Text;
	bool Pressed = false;
	bool Hovered = false;
	RmGUISignalAs<> OnClicked;
};
#define PRIVATE() ((RmGUIButtonPrivateData*) m_PrivateButton)

RmGUIButton::RmGUIButton(IRmGUIWidgetRaw parent)
	:
	RmGUIControl(parent),
	m_PrivateButton(nullptr)
{
	m_PrivateButton = new RmGUIButtonPrivateData;

	PRIVATE()->Normal.Pen = { .Color = {0 / 255.0f, 120 / 255.0f, 212 / 255.0f, 1.0f} };
	PRIVATE()->Normal.Brush = { .Color = {253 / 255.0f, 253 / 255.0f, 253 / 255.0f, 1.0f} };
	PRIVATE()->Hover.Pen = { .Color = {0 / 255.0f, 120 / 255.0f, 212 / 255.0f, 1.0f} };
	PRIVATE()->Hover.Brush = { .Color = {224 / 255.0f, 238 / 255.0f, 249 / 255.0f, 1.0f} };
	PRIVATE()->Press.Pen = { .Color = {0 / 255.0f, 84 / 255.0f, 153 / 255.0f, 1.0f} };
	PRIVATE()->Press.Brush = { .Color = {204 / 255.0f, 228 / 255.0f, 247 / 255.0f, 1.0f} };
	PRIVATE()->TextStyle.Pen = { .Style = RmPen::NoPen, .Color = {0 / 255.0f, 0 / 255.0f, 0 / 255.0f, 1.0f} };
	PRIVATE()->TextStyle.Brush = { .Color = {0 / 255.0f, 0 / 255.0f, 0 / 255.0f, 1.0f} };
	PRIVATE()->Text = "Button";
	onClicked = &PRIVATE()->OnClicked;
}

RmGUIButton::~RmGUIButton()
{
	delete m_PrivateButton; m_PrivateButton = nullptr;
}

void RmGUIButton::paint(IRmGUIPainterRaw painter, RmRectRaw client)
{
	RmFloat2 round;
	if (getEnable() == false)
	{
		round = PRIVATE()->Disable.Round;
		painter->setPen(PRIVATE()->Disable.Pen);
		painter->setBrush(PRIVATE()->Disable.Brush);
	}
	else if (PRIVATE()->Pressed)
	{
		round = PRIVATE()->Press.Round;
		painter->setPen(PRIVATE()->Press.Pen);
		painter->setBrush(PRIVATE()->Press.Brush);
	}
	else if (PRIVATE()->Hovered)
	{
		round = PRIVATE()->Hover.Round;
		painter->setPen(PRIVATE()->Hover.Pen);
		painter->setBrush(PRIVATE()->Hover.Brush);
	}
	else
	{
		round = PRIVATE()->Normal.Round;
		painter->setPen(PRIVATE()->Normal.Pen);
		painter->setBrush(PRIVATE()->Normal.Brush);
	}

	painter->drawRoundedRect(client->X, client->Y, client->W, client->H, round.X, round.Y);

	if (PRIVATE()->Text.empty() == false)
	{
		painter->setPen(PRIVATE()->TextStyle.Pen);
		painter->setBrush(PRIVATE()->TextStyle.Brush);
		painter->drawText(client->X + 25, client->Y + 20, 0, 16, 0, PRIVATE()->Text);
	}
}

void RmGUIButton::mousePressEvent(IRmGUIMouseEventRaw event)
{
	auto client = getRect();
	if (client.X <= event->X && event->X <= client.X + client.W
		&& client.Y <= event->Y && event->Y <= client.Y + client.H)
	{
		if (event->Button == 1)
		{
			PRIVATE()->Pressed = true;
			PRIVATE()->OnClicked.emit();
		}
	}
}

void RmGUIButton::mouseReleaseEvent(IRmGUIMouseEventRaw event)
{
	if (event->Button == 1) PRIVATE()->Pressed = false;
}

void RmGUIButton::mouseMoveEvent(IRmGUIMouseEventRaw event)
{
	auto client = getRect();
	if (client.X <= event->X && event->X <= client.X + client.W
		&& client.Y <= event->Y && event->Y <= client.Y + client.H)
	{
		PRIVATE()->Hovered = true;
	}
	else
	{
		PRIVATE()->Hovered = false;
	}
}
