#include "RmGUILabel.h"

RmGUILabel::RmGUILabel(IRmGUIWidgetRaw parent)
	:
	RmGUIControl(parent)
{
}

void RmGUILabel::paint(IRmGUIPainterRaw painter, RmRectRaw client)
{
	RmGUIWidget::paint(painter, client);
}