#include "RmGUIPanel.h"

RmGUIPanel::RmGUIPanel(rmwidget parent)
	:
	RmGUIWidget(parent)
{
}

void RmGUIPanel::paint(rmpainter painter, rmrect client)
{
}

void RmGUIPanel::mouseMoveEvent(rmevent_mouse event)
{
	printf("mouse move in panel\n");
}
