#pragma once
#include "IGStyle.h"

class OPENUI_API IGHLine : public UIWidget
{
public:
	explicit IGHLine(UICanvasRaw canvas);
	~IGHLine() override;
	void paint(UIRect client, UIPainterRaw painter) override;

	IGFrameStyle getStyle() const;
	void setStyle(IGFrameStyle value);

private:
	UIPrivateRaw m_Private;
};
using IGHLineRef = UIRef<IGHLine>;
using IGHLineRaw = UIRaw<IGHLine>;

class OPENUI_API IGVLine : public UIWidget
{
public:
	explicit IGVLine(UICanvasRaw canvas);
	~IGVLine() override;
	void paint(UIRect client, UIPainterRaw painter) override;

	IGFrameStyle getStyle() const;
	void setStyle(IGFrameStyle value);

private:
	UIPrivateRaw m_Private;
};
using IGVLineRef = UIRef<IGVLine>;
using IGVLineRaw = UIRaw<IGVLine>;
