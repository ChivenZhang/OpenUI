#include "RmGUILabel.h"

RmGUILabel::RmGUILabel(rmwidget parent)
	:
	RmGUIWidget(parent)
{
}

void RmGUILabel::paint(rmpainter painter, rmrect client)
{
}

void RmGUILabel::mouseMoveEvent(rmevent_mouse event)
{
	printf("mouse move in label\n");
}