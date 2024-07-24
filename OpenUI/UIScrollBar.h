#pragma once
#include "UIElement.h"
#include "UIFactory.h"
#include "UIButton.h"

/// @brief 
struct UIScrollBarStyle
{
	UIPen Pen = UINoPen;
	UIBrush Brush = UIFrameBrush;
};

/// @brief Scroll bar
class OPENUI_API UIScrollBar : public UIElement
{
public:
	UIScrollBar();
	~UIScrollBar();
	virtual bool filter(UIReactorRaw source, UIEventRaw event) override;
	virtual void layout(UIRect client) override;
	virtual void paint(UIRect client, UIPainterRaw painter) override;
	using UIElement::removeElement;
	virtual void removeElement() override;

	UI::Orientation getOrientation() const;
	void setOrientation(UI::Orientation value);

	int32_t getMinimum() const;
	int32_t getMaximum() const;
	int32_t getRange() const;
	void setRange(int32_t min, int32_t max);

	int32_t getValue() const;
	void setValue(int32_t value);

	bool getTracking() const;
	void setTracking(bool value);

	int32_t getSingleStep() const;
	void setSingleStep(int32_t value);

	UIButtonRaw getHandle() const;

public:
	UISignalAsRaw<> sliderPressed;
	UISignalAsRaw<> sliderReleased;
	UISignalAsRaw<int32_t /*value*/> sliderMoved;
	UISignalAsRaw<int32_t /*value*/> valueChanged;
	UISignalAsRaw<int32_t /*min*/, int32_t /*max*/> rangeChanged;

private:
	UIElementPrivateRaw m_PrivateScrollBar;
};
using UIScrollBarRef = UIRef<UIScrollBar>;
using UIScrollBarRaw = UIRaw<UIScrollBar>;

/// @brief Scroll bar factory
class OPENUI_API UIScrollBarFactory : public UIFactory
{
public:
	virtual UIString getTagName() const override;
	virtual UIElementRef getElement(UIString style) const override;
};