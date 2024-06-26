#include "RmGUILayer.h"
#include <taitank.h>
namespace flex = taitank;

RmGUILayer::RmGUILayer(IRmGUIWidgetRaw parent)
	:
	RmGUILayout(parent)
{
}

void RmGUILayer::layout(RmRectRaw client)
{
	auto childList = getChildren();
	for (size_t i = 0; i < childList.size(); ++i)
	{
		childList[i]->setRect(getRect());
		childList[i]->setViewport(getViewport());
	}
}

void RmGUILayer::paint(IRmGUIPainterRaw painter, RmRectRaw client)
{
}