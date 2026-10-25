#pragma once
#include "PCStyle.h"

/// Row-major grid. Children fill equal cells.
class OPENUI_API PCGrid : public UIWidget
{
public:
	explicit PCGrid(UICanvasRaw canvas);
	~PCGrid() override;
	void arrange(UIRect client) override;
	void paint(UIRect client, UIPainterRaw painter) override;

	int32_t getColumnCount() const;
	void setColumnCount(int32_t value);

private:
	UIPrivateRaw m_Private;
};
using PCGridRef = UIRef<PCGrid>;
using PCGridRaw = UIRaw<PCGrid>;
