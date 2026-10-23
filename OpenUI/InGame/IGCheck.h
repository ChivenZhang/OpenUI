#pragma once
#include "OpenUI/UIWidget.h"

/// @brief Gold check box with a label.
class OPENUI_API IGCheck : public UIWidget
{
public:
	explicit IGCheck(UICanvasRaw canvas);
	~IGCheck() override;
	void paint(UIRect client, UIPainterRaw painter) override;

	UIString getText() const;
	void setText(UIString const& value);
	bool getChecked() const;
	void setChecked(bool value);

	UISignalAsRaw<bool> toggled;

protected:
	void mousePressEvent(UIMouseEventRaw event) override;
	void mouseMoveEvent(UIMouseEventRaw event) override;
	void leaveEvent(UIMouseEventRaw event) override;

private:
	UIPrivateRaw m_Private;
};
using IGCheckRef = UIRef<IGCheck>;
using IGCheckRaw = UIRaw<IGCheck>;
