#pragma once
#include "IGStyle.h"

class OPENUI_API IGFlow : public UIWidget
{
public:
	explicit IGFlow(UICanvasRaw canvas);
	~IGFlow() override;
	void arrange(UIRect client) override;
	void paint(UIRect client, UIPainterRaw painter) override;

	IGFrameStyle getStyle() const;
	void setStyle(IGFrameStyle value);

private:
	UIPrivateRaw m_Private;
};
using IGFlowRef = UIRef<IGFlow>;
using IGFlowRaw = UIRaw<IGFlow>;
