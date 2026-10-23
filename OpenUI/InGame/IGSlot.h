#pragma once
#include "OpenUI/UIWidget.h"

enum class IGRarity
{
	Common,
	Uncommon,
	Rare,
	Epic,
	Legendary,
};

/// @brief Inventory cell. Optional icon, stack count, and a click signal.
class OPENUI_API IGSlot : public UIWidget
{
public:
	explicit IGSlot(UICanvasRaw canvas);
	~IGSlot() override;
	void paint(UIRect client, UIPainterRaw painter) override;

	void setImage(UIImage image);
	void setCount(int32_t value);
	void setRarity(IGRarity rarity);
	void setText(UIString const& value);

	UISignalAsRaw<> clicked;

protected:
	void mousePressEvent(UIMouseEventRaw event) override;
	void mouseMoveEvent(UIMouseEventRaw event) override;
	void leaveEvent(UIMouseEventRaw event) override;

private:
	UIPrivateRaw m_Private;
};
using IGSlotRef = UIRef<IGSlot>;
using IGSlotRaw = UIRaw<IGSlot>;
