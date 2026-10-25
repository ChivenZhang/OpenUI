#pragma once
#include "PCStyle.h"

class OPENUI_API PCCheckBox : public UIWidget
{
public:
	explicit PCCheckBox(UICanvasRaw canvas);
	~PCCheckBox() override;
	void paint(UIRect client, UIPainterRaw painter) override;

	UIString getText() const;
	void setText(UIString const& value);
	bool getChecked() const;
	void setChecked(bool value);

	UISignalAsRaw<bool> toggled;
	UISignalAsRaw<> clicked;

protected:
	void mousePressEvent(UIMouseEventRaw event) override;
	void mouseMoveEvent(UIMouseEventRaw event) override;
	void leaveEvent(UIMouseEventRaw event) override;

private:
	UIPrivateRaw m_Private;
};
using PCCheckBoxRef = UIRef<PCCheckBox>;
using PCCheckBoxRaw = UIRaw<PCCheckBox>;
