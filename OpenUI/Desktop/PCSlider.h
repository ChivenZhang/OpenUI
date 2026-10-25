#pragma once
#include "PCStyle.h"

class OPENUI_API PCSlider : public UIWidget
{
public:
	explicit PCSlider(UICanvasRaw canvas);
	~PCSlider() override;
	void paint(UIRect client, UIPainterRaw painter) override;

	int32_t getMinimum() const;
	int32_t getMaximum() const;
	void setRange(int32_t minimum, int32_t maximum);
	int32_t getValue() const;
	void setValue(int32_t value);

	UISignalAsRaw<int32_t> valueChanged;
	UISignalAsRaw<int32_t> sliderMoved;

protected:
	void mousePressEvent(UIMouseEventRaw event) override;
	void mouseReleaseEvent(UIMouseEventRaw event) override;
	void mouseMoveEvent(UIMouseEventRaw event) override;

private:
	UIPrivateRaw m_Private;
};
using PCSliderRef = UIRef<PCSlider>;
using PCSliderRaw = UIRaw<PCSlider>;
