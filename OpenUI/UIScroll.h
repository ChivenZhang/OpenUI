#pragma once
#include "UIElement.h"
#include "UIFactory.h"
#include "UIScrollBar.h"

/// @brief 
struct UIScrollStyle
{
	UIPen Pen = UIFramePen;
	UIBrush Brush = UIFrameBrush;
};

/// @brief Scroll
class OPENUI_API UIScroll : public UIElement
{
public:
	using policy_t = enum
	{
		PolicyAsNeeded,
		PolicyAlwaysOff,
		PolicyAlwaysOn,
	};

public:
	UIScroll();
	~UIScroll();
	virtual void arrange(UIRect client) override;
	virtual void layout(UIRect client) override;
	virtual void paint(UIRect client, UIPainterRaw painter) override;
	virtual void repaint(UIRect client, UIPainterRaw painter) override;
	virtual bool addElement(UIElementRef value) override;
	virtual bool removeElement(UIElementRef value) override;
	virtual void removeElement() override;

	UIScrollStyle getStyle() const;
	void setStyle(UIScrollStyle value);

	policy_t getHorizontalPolicy() const;
	void setHorizontalPolicy(policy_t value);

	policy_t getVerticalPolicy() const;
	void setVerticalPolicy(policy_t value);

	void setScrollPolicy(policy_t horizontal, policy_t vertical);

	float getHorizontalValue() const;
	void setHorizontalValue(float value);

	float getVerticalValue() const;
	void setVerticallValue(float value);

	UIScrollBarRaw getHorizontalBar() const;
	UIScrollBarRaw getVerticalBar() const;
	UIElementRaw getContentView() const;

protected:
	virtual void wheelEvent(UIMouseWheelEventRaw event) override;

private:
	UIElementPrivateRaw m_PrivateScroll;
};
using UIScrollRef = UIRef<UIScroll>;
using UIScrollRaw = UIRaw<UIScroll>;

/// @brief Scroll factory
class OPENUI_API UIScrollFactory : public UIFactory
{
public:
	virtual UIString getTagName() const override;
	virtual UIElementRef getElement(UIString style) const override;
};