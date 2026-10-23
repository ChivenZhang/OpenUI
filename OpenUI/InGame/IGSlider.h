#pragma once
#include "OpenUI/UIWidget.h"

/// @brief Horizontal value slider with a round handle.
class OPENUI_API IGSlider : public UIWidget
{
public:
	explicit IGSlider(UICanvasRaw canvas);
	~IGSlider() override;
	void paint(UIRect client, UIPainterRaw painter) override;

	int32_t getMinimum() const;
	int32_t getMaximum() const;
	void setRange(int32_t minimum, int32_t maximum);
	int32_t getValue() const;
	void setValue(int32_t value);

	UISignalAsRaw<int32_t> valueChanged;

protected:
	void mousePressEvent(UIMouseEventRaw event) override;
	void mouseReleaseEvent(UIMouseEventRaw event) override;
	void mouseMoveEvent(UIMouseEventRaw event) override;

private:
	UIPrivateRaw m_Private;
};
using IGSliderRef = UIRef<IGSlider>;
using IGSliderRaw = UIRaw<IGSlider>;
