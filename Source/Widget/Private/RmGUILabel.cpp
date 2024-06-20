#include "RmGUILabel.h"

RmGUILabel::RmGUILabel(rmwidget parent)
	:
	RmGUIWidget(parent)
{
}

void RmGUILabel::paint(rmpainter painter, rmrect client)
{
	RmGUIWidget::paint(painter, client);
}