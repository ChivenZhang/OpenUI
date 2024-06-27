#pragma once
#include "RmGUILayout.h"
#include "RmGUIScrollBar.h"

/// @brief Scroll Layout
class RMGUI_API RmGUIScroll : public RmGUILayout
{
public:
	using policy_t = enum
	{
		PolicyAsNeeded = 0,
		PolicyAlwaysOff = 1,
		PolicyAlwaysOn = 2,
	};

public:
	RmGUIScroll(IRmGUIWidgetRaw parent = nullptr);
	~RmGUIScroll();
	virtual void layout(RmRectRaw client) override;
	virtual void paint(IRmGUIPainterRaw painter, RmRectRaw client) override;
	using RmGUILayout::removeWidget;
	virtual void removeWidget() override;
	policy_t getHorizontalPolicy() const;
	void setHorizontalPolicy(policy_t value);
	RmGUIScrollBarRaw getHorizontalBar() const;
	policy_t getVerticalPolicy() const;
	void setVerticalPolicy(policy_t value);
	RmGUIScrollBarRaw getVerticalBar() const;

protected:
	virtual void mouseMoveEvent(IRmGUIMouseEventRaw event) override;
	virtual void wheelEvent(IRmGUIMouseWheelEventRaw event) override;

private:
	RmGUIWidgetPrivateRaw m_PrivateScroll;
};
using RmGUIScrollRef = RmRef<RmGUIScroll>;
using RmGUIScrollRaw = RmRaw<RmGUIScroll>();