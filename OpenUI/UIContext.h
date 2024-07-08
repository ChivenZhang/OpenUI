#pragma once
#include "UI.h"

class UIContextPrivate {};
using UIContextPrivateRaw = UIRaw<UIContextPrivate>;

/// @brief 
class OPENUI_API UIContext
{
public:
	UIContext();
	virtual ~UIContext();

private:
	UIContextPrivateRaw m_Private;
};
using UIContextRef = UIRef<UIContext>;
using UIContextRaw = UIRaw<UIContext>;