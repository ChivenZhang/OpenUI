#pragma once
#include "PCStyle.h"

class OPENUI_API PCHBox : public UIWidget
{
public:
	explicit PCHBox(UICanvasRaw canvas);
	~PCHBox() override;
	void arrange(UIRect client) override;
	void paint(UIRect client, UIPainterRaw painter) override;
};
using PCHBoxRef = UIRef<PCHBox>;
using PCHBoxRaw = UIRaw<PCHBox>;
