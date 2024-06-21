#include "RmGUILabel.h"

RmGUILabel::RmGUILabel(IRmGUIWidgetRaw parent)
	:
	RmGUIControl(parent)
{
}

void RmGUILabel::paint(IRmGUIPainterRaw painter, rmrect client)
{
	RmGUIWidget::paint(painter, client);
}