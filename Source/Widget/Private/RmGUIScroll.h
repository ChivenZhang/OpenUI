#pragma once
#include "RmGUILayout.h"
#include "RmGUIScrollBar.h"

/// @brief Scroll Layout
class RMGUI_API RmGUIScroll : public RmGUILayout
{
public:
	using policy_t = enum
	{
		ScrollBarAsNeeded = 0,
		ScrollBarAlwaysOff = 1,
		ScrollBarAlwaysOn = 2,
	};

public:
	RmGUIScroll(IRmGUIWidgetRaw parent = nullptr);
	~RmGUIScroll();
	virtual void layout(RmRectRaw client) override;
	virtual void paint(IRmGUIPainterRaw painter, RmRectRaw client) override;
	policy_t getHorizontalPolicy() const;
	void setHorizontalPolicy(policy_t value);
	RmGUIScrollBarRaw getHorizontalBar() const;
	policy_t getVerticalPolicy() const;
	void setVerticalPolicy(policy_t value);
	RmGUIScrollBarRaw getVerticalBar() const;

protected:
	virtual void wheelEvent(IRmGUIMouseWheelEventRaw event) override;

private:
	RmGUIWidgetPrivateRaw m_PrivateScroll;
};