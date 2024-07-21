#pragma once
#include "OpenUI/UIElement.h"
#include "OpenUI/UIFactory.h"

/// @brief 
struct UILabelStyle
{
	struct LabelMode
	{
		struct
		{
			UIPen Pen = { .Style = UIPen::NoPen, };
			UIFont Font = { .Align = UIFont::AlignLeft | UIFont::AlignVCenter, };
			UIBrush Brush = { .Color = {0,0,0,1}, };
		} Foreground;
		struct
		{
			UIPen Pen = { .Color = { 108 / 255.0f, 110 / 255.0f, 111 / 255.0f, 1.0f }, };
			UIBrush Brush = { .Color = { 238 / 255.0f, 238 / 255.0f, 242 / 255.0f, 1.0f }, };
		} Background;
	};
	LabelMode Normal, Hover, Disable;
};

/// @brief Label
class OPENUI_API UILabel : public UIElement
{
public:
	using scale_t = enum
	{
		ScaleFixed = 0,
		ScaleAuto = 1,
		ScaleNoRatio = 2,
		ScaleKeepRatio = 3,
	};

public:
	UILabel();
	~UILabel();
	virtual void layout(UIRect client) override;
	virtual void paint(UIRect client, UIPainterRaw painter) override;

	UIString getText() const;
	void setText(UIString const& text);

	UILabelStyle getStyle() const;
	void setStyle(UILabelStyle const& style);

	UIFontAligns getAlignment() const;
	void setAlignment(UIFontAligns value);

	UIImage getPixmap() const;
	void setPixmap(UIImage image);

	scale_t getScaledContents() const;
	void setScaledContents(scale_t value);

protected:
	virtual void mouseDoubleEvent(UIMouseEventRaw event) override;
	virtual void mousePressEvent(UIMouseEventRaw event) override;
	virtual void mouseReleaseEvent(UIMouseEventRaw event) override;
	virtual void mouseMoveEvent(UIMouseEventRaw event) override;
	virtual void enterEvent(UIMouseEventRaw event) override;
	virtual void leaveEvent(UIMouseEventRaw event) override;

public:
	UISignalAsRaw<UIString> linkHovered;
	UISignalAsRaw<UIString> linkActivated;

private:
	UIElementPrivateRaw m_PrivateLabel;
};
using UILabelRef = UIRef<UILabel>;
using UILabelRaw = UIRaw<UILabel>;

/// @brief Label factory
class OPENUI_API UILabelFactory : public UIFactory
{
public:
	virtual UIString getTagName() const override;
	virtual UIElementRef getElement(UIString style) const override;
};