#include "RmGUIEdit.h"

RmGUIEdit::RmGUIEdit(IRmGUIWidgetRaw parent)
	:
	RmGUIControl(parent),
	m_PrivateEdit(nullptr)
{
}

RmGUIEdit::~RmGUIEdit()
{
}
