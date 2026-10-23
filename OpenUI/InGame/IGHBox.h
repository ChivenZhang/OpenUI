#pragma once
#include "IGStyle.h"

class OPENUI_API IGHBox : public UIWidget
{
public:
	explicit IGHBox(UICanvasRaw canvas);
	~IGHBox() override;
	void arrange(UIRect client) override;
	void paint(UIRect client, UIPainterRaw painter) override;

	IGFrameStyle getStyle() const;
	void setStyle(IGFrameStyle value);

private:
	UIPrivateRaw m_Private;
};
using IGHBoxRef = UIRef<IGHBox>;
using IGHBoxRaw = UIRaw<IGHBox>;
