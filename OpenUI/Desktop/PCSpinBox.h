#pragma once
#include "PCStyle.h"

class OPENUI_API PCSpinBox : public UIWidget
{
public:
	explicit PCSpinBox(UICanvasRaw canvas);
	~PCSpinBox() override;
	void paint(UIRect client, UIPainterRaw painter) override;

	int32_t getMinimum() const;
	int32_t getMaximum() const;
	void setRange(int32_t minimum, int32_t maximum);
	int32_t getSingleStep() const;
	void setSingleStep(int32_t value);
	int32_t getValue() const;
	void setValue(int32_t value);

	UISignalAsRaw<int32_t> valueChanged;

protected:
	void mousePressEvent(UIMouseEventRaw event) override;
	void mouseMoveEvent(UIMouseEventRaw event) override;
	void leaveEvent(UIMouseEventRaw event) override;
	void wheelEvent(UIMouseWheelEventRaw event) override;

private:
	UIPrivateRaw m_Private;
};
using PCSpinBoxRef = UIRef<PCSpinBox>;
using PCSpinBoxRaw = UIRaw<PCSpinBox>;
