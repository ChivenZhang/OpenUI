#pragma once
#include "UI.h"

class UIElement;
using UIElementRef = UIRef<UIElement>;
using UIElementRaw = UIRaw<UIElement>;
class UIElementPrivate {};
using UIElementPrivateRaw = UIRaw<UIElementPrivate>;

/// @brief 
class OPENUI_API UIElement
{
public:
	UIElement();
	virtual ~UIElement();
	UIElementRaw getParent() const;
	UIArrayView<const UIElementRef> getChildren() const;
	virtual bool addElement(UIElementRef value);
	virtual bool removeElement(UIElementRef value);
	virtual UIString getStyle() const;
	virtual void setStyle(UIString value);
	virtual UIString getStyle(UIString name) const;
	virtual void setStyle(UIString name, UIString value);
	virtual void layout(UIRect client);
	virtual void paint(UIRect client, UIPainterRaw painter);

private:
	UIElementPrivateRaw m_Private;
};