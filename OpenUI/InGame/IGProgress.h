#pragma once
#include "OpenUI/UIWidget.h"

/// @brief Filled bar, for health, mana, experience, or a cast.
class OPENUI_API IGProgress : public UIWidget
{
public:
	explicit IGProgress(UICanvasRaw canvas);
	~IGProgress() override;
	void paint(UIRect client, UIPainterRaw painter) override;

	float getValue() const;
	void setValue(float value);
	void setText(UIString const& value);

private:
	UIPrivateRaw m_Private;
};
using IGProgressRef = UIRef<IGProgress>;
using IGProgressRaw = UIRaw<IGProgress>;
