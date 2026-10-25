#pragma once
#include "PCStyle.h"

class OPENUI_API PCHSeparator : public UIWidget
{
public:
	explicit PCHSeparator(UICanvasRaw canvas);
	~PCHSeparator() override;
	void paint(UIRect client, UIPainterRaw painter) override;
};
using PCHSeparatorRef = UIRef<PCHSeparator>;
using PCHSeparatorRaw = UIRaw<PCHSeparator>;

class OPENUI_API PCVSeparator : public UIWidget
{
public:
	explicit PCVSeparator(UICanvasRaw canvas);
	~PCVSeparator() override;
	void paint(UIRect client, UIPainterRaw painter) override;
};
using PCVSeparatorRef = UIRef<PCVSeparator>;
using PCVSeparatorRaw = UIRaw<PCVSeparator>;
