#pragma once
#include "IGStyle.h"

/// Children are stacked in one rectangle. Later children paint above earlier ones.
class OPENUI_API IGStack : public UIWidget
{
public:
	explicit IGStack(UICanvasRaw canvas);
	~IGStack() override;
	void arrange(UIRect client) override;
	void paint(UIRect client, UIPainterRaw painter) override;

	IGFrameStyle getStyle() const;
	void setStyle(IGFrameStyle value);

private:
	UIPrivateRaw m_Private;
};
using IGStackRef = UIRef<IGStack>;
using IGStackRaw = UIRaw<IGStack>;
