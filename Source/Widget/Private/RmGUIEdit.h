#pragma once
#include "RmGUIControl.h"

/// @brief Text Edit
class RMGUI_API RmGUIEdit : public RmGUIControl
{
public:
	RmGUIEdit(IRmGUIWidgetRaw parent = nullptr);
	~RmGUIEdit();

private:
	RmGUIWidgetPrivateRaw m_PrivateEdit;
};