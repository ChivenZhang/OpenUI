#pragma once
#include "PCStyle.h"

class OPENUI_API PCFlow : public UIWidget
{
public:
	explicit PCFlow(UICanvasRaw canvas);
	~PCFlow() override;
	void arrange(UIRect client) override;
	void paint(UIRect client, UIPainterRaw painter) override;
};
using PCFlowRef = UIRef<PCFlow>;
using PCFlowRaw = UIRaw<PCFlow>;
