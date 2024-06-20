#include "RmGUILabel.h"

RmGUILabel::RmGUILabel(rmwidget parent)
	:
	RmGUIControl(parent)
{
}

void RmGUILabel::paint(rmpainter painter, rmrect client)
{
	RmGUIWidget::paint(painter, client);
}