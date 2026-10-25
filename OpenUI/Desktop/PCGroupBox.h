#pragma once
#include "PCStyle.h"

class OPENUI_API PCGroupBox : public UIWidget
{
public:
	explicit PCGroupBox(UICanvasRaw canvas);
	~PCGroupBox() override;
	void arrange(UIRect client) override;
	void paint(UIRect client, UIPainterRaw painter) override;

	UIString getTitle() const;
	void setTitle(UIString const& value);

private:
	UIPrivateRaw m_Private;
};
using PCGroupBoxRef = UIRef<PCGroupBox>;
using PCGroupBoxRaw = UIRaw<PCGroupBox>;
