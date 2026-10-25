#pragma once
#include "PCStyle.h"

class OPENUI_API PCTabWidget : public UIWidget
{
public:
	explicit PCTabWidget(UICanvasRaw canvas);
	~PCTabWidget() override;
	void arrange(UIRect client) override;
	void paint(UIRect client, UIPainterRaw painter) override;

	int32_t addTab(UIString const& title, UIWidgetRef page);
	int32_t getCurrentIndex() const;
	void setCurrentIndex(int32_t value);
	UIString getTabText(int32_t index) const;
	void setTabText(int32_t index, UIString const& value);

	UISignalAsRaw<int32_t> currentChanged;

protected:
	void mousePressEvent(UIMouseEventRaw event) override;
	void mouseMoveEvent(UIMouseEventRaw event) override;
	void leaveEvent(UIMouseEventRaw event) override;

private:
	UIPrivateRaw m_Private;
};
using PCTabWidgetRef = UIRef<PCTabWidget>;
using PCTabWidgetRaw = UIRaw<PCTabWidget>;
