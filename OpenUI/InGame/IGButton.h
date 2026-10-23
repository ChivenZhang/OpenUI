#pragma once
#include "OpenUI/UIWidget.h"

/// @brief Clickable plate. Hover lifts the border, press darkens the fill.
class OPENUI_API IGButton : public UIWidget
{
public:
	explicit IGButton(UICanvasRaw canvas);
	~IGButton() override;
	void paint(UIRect client, UIPainterRaw painter) override;

	UIString getText() const;
	void setText(UIString const& value);

	UISignalAsRaw<> clicked;

protected:
	void mousePressEvent(UIMouseEventRaw event) override;
	void mouseReleaseEvent(UIMouseEventRaw event) override;
	void mouseMoveEvent(UIMouseEventRaw event) override;
	void leaveEvent(UIMouseEventRaw event) override;

private:
	UIPrivateRaw m_Private;
};
using IGButtonRef = UIRef<IGButton>;
using IGButtonRaw = UIRaw<IGButton>;
