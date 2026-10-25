#pragma once
#include "PCStyle.h"

class OPENUI_API PCLabel : public UIWidget
{
public:
	explicit PCLabel(UICanvasRaw canvas);
	~PCLabel() override;
	void paint(UIRect client, UIPainterRaw painter) override;

	UIString getText() const;
	void setText(UIString const& value);
	void setFont(UIFont const& font);

private:
	UIPrivateRaw m_Private;
};
using PCLabelRef = UIRef<PCLabel>;
using PCLabelRaw = UIRaw<PCLabel>;
