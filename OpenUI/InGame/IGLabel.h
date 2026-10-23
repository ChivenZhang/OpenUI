#pragma once
#include "OpenUI/UIWidget.h"

/// @brief Single line of parchment text.
class OPENUI_API IGLabel : public UIWidget
{
public:
	explicit IGLabel(UICanvasRaw canvas);
	~IGLabel() override;
	void paint(UIRect client, UIPainterRaw painter) override;

	UIString getText() const;
	void setText(UIString const& value);
	void setFont(UIFont const& font);
	void setColor(UIBrush const& brush);

private:
	UIPrivateRaw m_Private;
};
using IGLabelRef = UIRef<IGLabel>;
using IGLabelRaw = UIRaw<IGLabel>;
