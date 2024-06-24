#pragma once
#include "RmGUIControl.h"

class RMGUI_API RmGUIScrollBar : public RmGUIControl
{
public:
	RmGUIScrollBar(IRmGUIWidgetRaw parent = nullptr);
	~RmGUIScrollBar();
	virtual void layout(RmRectRaw client) override;
	virtual void paint(IRmGUIPainterRaw painter, RmRectRaw client) override;
	bool getOrientation() const;
	void setOrientation(bool horizontal);
	int32_t getMinimum() const;
	int32_t getMaximum() const;
	void setRange(int32_t min, int32_t max);
	int32_t getValue() const;
	void setValue(int32_t value);
	bool getTracking() const;
	void setTracking(bool value);

protected:
	virtual void mousePressEvent(IRmGUIMouseEventRaw event) override;
	virtual void mouseMoveEvent(IRmGUIMouseEventRaw event) override;

public:
	IRmGUISignalAsRaw<int32_t, int32_t> rangeChanged;
	IRmGUISignalAsRaw<int32_t> sliderMoved;
	IRmGUISignalAsRaw<> sliderPressed;
	IRmGUISignalAsRaw<> sliderReleased;
	IRmGUISignalAsRaw<int32_t> valueChanged;

private:
	RmGUIWidgetPrivateRaw m_PrivateScrollBar;
};