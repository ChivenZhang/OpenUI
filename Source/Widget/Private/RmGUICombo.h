#pragma once
#include "RmGUIWidget.h"

/// @brief Combo Box
class RMGUI_API RmGUICombo : public RmGUIWidget
{
public:
	RmGUICombo(IRmGUIWidgetRaw parent = nullptr);
	~RmGUICombo();

private:
	RmGUIWidgetPrivateRaw m_PrivateCombo;
};