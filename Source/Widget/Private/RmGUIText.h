#pragma once
#include "RmGUIControl.h"

/// @brief Line Edit
class RMGUI_API RmGUIText : public RmGUIControl
{
public:
	RmGUIText(IRmGUIWidgetRaw parent = nullptr);
	~RmGUIText();

private:
	RmGUIWidgetPrivateRaw m_PrivateInput;
};