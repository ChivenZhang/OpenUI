#include "RmGUIPanel.h"
#include <cairo/cairo.h>

RmGUIPanel::RmGUIPanel(rmwidget parent)
	:
	RmGUIWidget(parent)
{
}

void RmGUIPanel::paint(rmpainter painter, rmrect client)
{
	painter->setPen({ 0, 1, 1, 1 });
	painter->drawRect(10, 10, 100, 65);
	RmGUIWidget::paint(painter, client);
}