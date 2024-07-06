#pragma once
#include "RmGUILayout.h"

struct RmGUIHBoxStyle
{
	RmPen Pen;
	RmBrush Brush;
};

/// @brief Horizontal Layout
class RMGUI_API RmGUIHBox : public RmGUILayout
{
public:
	RmGUIHBox(IRmGUIWidgetRaw parent = nullptr);
	~RmGUIHBox();
	virtual void layout(RmRectRaw client) override;
	virtual void paint(IRmGUIPainterRaw painter, RmRectRaw client) override;
	RmGUIHBoxStyle getStyle() const;
	void setStyle(RmGUIHBoxStyle value);

private:
	RmGUIWidgetPrivateRaw m_PrivateHbox;
};