#pragma once
#include "IGStyle.h"

/// Scrolls its first child. The child keeps its own height and is clipped to this box.
class OPENUI_API IGScroll : public UIWidget
{
public:
	explicit IGScroll(UICanvasRaw canvas);
	~IGScroll() override;
	void arrange(UIRect client) override;
	void paint(UIRect client, UIPainterRaw painter) override;

	float getOffset() const;
	void setOffset(float value);

	IGFrameStyle getStyle() const;
	void setStyle(IGFrameStyle value);

protected:
	void wheelEvent(UIMouseWheelEventRaw event) override;

private:
	UIPrivateRaw m_Private;
};
using IGScrollRef = UIRef<IGScroll>;
using IGScrollRaw = UIRaw<IGScroll>;
