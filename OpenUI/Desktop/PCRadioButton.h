#pragma once
#include "PCStyle.h"

class PCRadioButton;
class PCRadioGroup;

class OPENUI_API PCRadioGroup
{
public:
	~PCRadioGroup();
	void add(PCRadioButton* radio);
	void remove(PCRadioButton* radio);
	void select(PCRadioButton* radio);

private:
	UIList<PCRadioButton*> m_Items;
};

class OPENUI_API PCRadioButton : public UIWidget
{
public:
	explicit PCRadioButton(UICanvasRaw canvas);
	~PCRadioButton() override;
	void paint(UIRect client, UIPainterRaw painter) override;

	UIString getText() const;
	void setText(UIString const& value);
	bool getChecked() const;
	void setChecked(bool value);
	void setExclusive(PCRadioGroup* group);

	UISignalAsRaw<bool> toggled;

protected:
	void mousePressEvent(UIMouseEventRaw event) override;
	void mouseMoveEvent(UIMouseEventRaw event) override;
	void leaveEvent(UIMouseEventRaw event) override;

private:
	UIPrivateRaw m_Private;
};
using PCRadioButtonRef = UIRef<PCRadioButton>;
using PCRadioButtonRaw = UIRaw<PCRadioButton>;
