#pragma once
#include "PCStyle.h"

/// One child is visible. The rest stay in the tree with display none.
class OPENUI_API PCStack : public UIWidget
{
public:
	explicit PCStack(UICanvasRaw canvas);
	~PCStack() override;
	void arrange(UIRect client) override;
	void paint(UIRect client, UIPainterRaw painter) override;

	int32_t getCurrentIndex() const;
	void setCurrentIndex(int32_t value);

	UISignalAsRaw<int32_t> currentChanged;

private:
	UIPrivateRaw m_Private;
};
using PCStackRef = UIRef<PCStack>;
using PCStackRaw = UIRaw<PCStack>;
