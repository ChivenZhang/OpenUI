#pragma once
#include "RmGUILayout.h"

/// @brief Grid Layout
class RMGUI_API RmGUIGrid : public RmGUILayout
{
public:
	RmGUIGrid(IRmGUIWidgetRaw parent = nullptr);
	~RmGUIGrid();

private:
	RmGUIWidgetPrivateRaw m_PrivateGrid;
};