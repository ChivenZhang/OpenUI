#include "RmGUIText.h"

RmGUIText::RmGUIText(IRmGUIWidgetRaw parent)
	:
	RmGUIControl(parent),
	m_PrivateInput(nullptr)
{
}

RmGUIText::~RmGUIText()
{
}