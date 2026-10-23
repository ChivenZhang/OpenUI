#pragma once
#include "IGStyle.h"

class OPENUI_API IGGrid : public UIWidget
{
public:
	explicit IGGrid(UICanvasRaw canvas);
	~IGGrid() override;
	void arrange(UIRect client) override;
	void paint(UIRect client, UIPainterRaw painter) override;

	bool addWidget(UIWidgetRef value) override;
	bool addWidget(UIWidgetRef value, uint32_t row, uint32_t column, uint32_t rowSpan = 1, uint32_t columnSpan = 1);
	bool removeWidget(UIWidgetRef value) override;
	void removeWidget() override;

	void setRowStretch(UIList<uint32_t> stretch);
	void setColumnStretch(UIList<uint32_t> stretch);

	IGFrameStyle getStyle() const;
	void setStyle(IGFrameStyle value);

private:
	UIPrivateRaw m_Private;
};
using IGGridRef = UIRef<IGGrid>;
using IGGridRaw = UIRaw<IGGrid>;
