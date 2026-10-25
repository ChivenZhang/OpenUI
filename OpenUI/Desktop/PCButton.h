#pragma once
#include "PCStyle.h"

class OPENUI_API PCButton : public UIWidget
{
public:
	explicit PCButton(UICanvasRaw canvas);
	~PCButton() override;
	void paint(UIRect client, UIPainterRaw painter) override;

	UIString getText() const;
	void setText(UIString const& value);
	bool getCheckable() const;
	void setCheckable(bool value);
	bool getChecked() const;
	void setChecked(bool value);

	UISignalAsRaw<> clicked;
	UISignalAsRaw<> pressed;
	UISignalAsRaw<> released;
	UISignalAsRaw<bool> toggled;

protected:
	void mousePressEvent(UIMouseEventRaw event) override;
	void mouseReleaseEvent(UIMouseEventRaw event) override;
	void mouseMoveEvent(UIMouseEventRaw event) override;
	void leaveEvent(UIMouseEventRaw event) override;

private:
	UIPrivateRaw m_Private;
};
using PCButtonRef = UIRef<PCButton>;
using PCButtonRaw = UIRaw<PCButton>;
