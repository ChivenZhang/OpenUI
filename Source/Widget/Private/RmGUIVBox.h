#pragma once
#include "RmGUILayout.h"

struct RmGUIVBoxStyle
{
	RmPen Pen;
	RmBrush Brush;
};

/// @brief Vertical Layout
class RMGUI_API RmGUIVBox : public RmGUILayout
{
public:
	RmGUIVBox(IRmGUIWidgetRaw parent = nullptr);
	~RmGUIVBox();
	virtual void layout(RmRectRaw client) override;
	virtual void paint(IRmGUIPainterRaw painter, RmRectRaw client) override;
	RmGUIVBoxStyle getStyle() const;
	void setStyle(RmGUIVBoxStyle value);

private:
	RmGUIWidgetPrivateRaw m_PrivateHbox;
};