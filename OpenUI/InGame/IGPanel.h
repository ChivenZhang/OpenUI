#pragma once
#include "IGStyle.h"

/// @brief Framed column. Children stack vertically inside a bronze plate.
class OPENUI_API IGPanel : public UIWidget
{
public:
	explicit IGPanel(UICanvasRaw canvas);
	~IGPanel() override;
	void arrange(UIRect client) override;
	void paint(UIRect client, UIPainterRaw painter) override;

	UIString getTitle() const;
	void setTitle(UIString const& text);
	IGFrameStyle getStyle() const;
	void setStyle(IGFrameStyle value);

private:
	UIPrivateRaw m_Private;
};
using IGPanelRef = UIRef<IGPanel>;
using IGPanelRaw = UIRaw<IGPanel>;
