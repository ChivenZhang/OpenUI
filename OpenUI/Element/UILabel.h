#pragma once
#include "../UIElement.h"

class OPENUI_API UILabel : public UIElement
{
public:
	UILabel();
	~UILabel();
	virtual void paint(UIRect client, UIPainterRaw painter) override;

private:
	UIElementPrivateRaw m_PrivateLabel;
};