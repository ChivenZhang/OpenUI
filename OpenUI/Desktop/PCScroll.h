#pragma once
#include "PCStyle.h"

class OPENUI_API PCScroll : public UIWidget
{
public:
	explicit PCScroll(UICanvasRaw canvas);
	~PCScroll() override;
	void arrange(UIRect client) override;
	void paint(UIRect client, UIPainterRaw painter) override;

	float getOffset() const;
	void setOffset(float value);

	UISignalAsRaw<float> scrolled;

protected:
	void wheelEvent(UIMouseWheelEventRaw event) override;

private:
	UIPrivateRaw m_Private;
};
using PCScrollRef = UIRef<PCScroll>;
using PCScrollRaw = UIRaw<PCScroll>;
