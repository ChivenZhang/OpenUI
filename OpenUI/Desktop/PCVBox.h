#pragma once
#include "PCStyle.h"

class OPENUI_API PCVBox : public UIWidget
{
public:
	explicit PCVBox(UICanvasRaw canvas);
	~PCVBox() override;
	void arrange(UIRect client) override;
	void paint(UIRect client, UIPainterRaw painter) override;
};
using PCVBoxRef = UIRef<PCVBox>;
using PCVBoxRaw = UIRaw<PCVBox>;
