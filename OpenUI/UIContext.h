#pragma once
#include "UIEvent.h"
#include "UIElement.h"

class UIContextPrivate {};
using UIContextPrivateRaw = UIRaw<UIContextPrivate>;

/// @brief 
class OPENUI_API UIContext
{
public:
	UIContext();
	virtual ~UIContext();
	virtual bool addElement(UIElementRef value);
	virtual bool removeElement(UIElementRef value);
	bool sendEvent()

private:
	UIContextPrivateRaw m_Private;
};
using UIContextRef = UIRef<UIContext>;
using UIContextRaw = UIRaw<UIContext>;