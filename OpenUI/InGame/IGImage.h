#pragma once
#include "IGStyle.h"

class OPENUI_API IGImage : public UIWidget
{
public:
	explicit IGImage(UICanvasRaw canvas);
	~IGImage() override;
	void paint(UIRect client, UIPainterRaw painter) override;

	void setImage(UIImage image);

private:
	UIPrivateRaw m_Private;
};
using IGImageRef = UIRef<IGImage>;
using IGImageRaw = UIRaw<IGImage>;
