#pragma once
#include "PCStyle.h"

class OPENUI_API PCFrame : public UIWidget
{
public:
	explicit PCFrame(UICanvasRaw canvas);
	~PCFrame() override;
	void arrange(UIRect client) override;
	void paint(UIRect client, UIPainterRaw painter) override;

	UIString getTitle() const;
	void setTitle(UIString const& value);

private:
	UIPrivateRaw m_Private;
};
using PCFrameRef = UIRef<PCFrame>;
using PCFrameRaw = UIRaw<PCFrame>;
