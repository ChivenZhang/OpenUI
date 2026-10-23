#pragma once
#include "IGStyle.h"

class OPENUI_API IGVBox : public UIWidget
{
public:
	explicit IGVBox(UICanvasRaw canvas);
	~IGVBox() override;
	void arrange(UIRect client) override;
	void paint(UIRect client, UIPainterRaw painter) override;

	IGFrameStyle getStyle() const;
	void setStyle(IGFrameStyle value);

private:
	UIPrivateRaw m_Private;
};
using IGVBoxRef = UIRef<IGVBox>;
using IGVBoxRaw = UIRaw<IGVBox>;
