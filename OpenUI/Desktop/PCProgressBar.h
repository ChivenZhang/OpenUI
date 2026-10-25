#pragma once
#include "PCStyle.h"

class OPENUI_API PCProgressBar : public UIWidget
{
public:
	explicit PCProgressBar(UICanvasRaw canvas);
	~PCProgressBar() override;
	void paint(UIRect client, UIPainterRaw painter) override;

	int32_t getMinimum() const;
	int32_t getMaximum() const;
	void setRange(int32_t minimum, int32_t maximum);
	int32_t getValue() const;
	void setValue(int32_t value);
	void setText(UIString const& value);

	UISignalAsRaw<int32_t> valueChanged;

private:
	UIPrivateRaw m_Private;
};
using PCProgressBarRef = UIRef<PCProgressBar>;
using PCProgressBarRaw = UIRaw<PCProgressBar>;
