#pragma once
#include "RmGUIControl.h"

/// @brief Scroll Bar
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
	int32_t getSingleStep() const;
	void setSingleStep(int32_t value);

protected:
	virtual void mousePressEvent(IRmGUIMouseEventRaw event) override;
	virtual void mouseMoveEvent(IRmGUIMouseEventRaw event) override;

public:
	IRmGUISignalAsRaw<> sliderPressed;
	IRmGUISignalAsRaw<> sliderReleased;
	IRmGUISignalAsRaw<int32_t /*value*/> sliderMoved;
	IRmGUISignalAsRaw<int32_t /*value*/> valueChanged;
	IRmGUISignalAsRaw<int32_t /*min*/, int32_t /*max*/> rangeChanged;

private:
	RmGUIWidgetPrivateRaw m_PrivateScrollBar;
};
using RmGUIScrollBarRef = RmRef<RmGUIScrollBar>;
using RmGUIScrollBarRaw = RmRaw<RmGUIScrollBar>;