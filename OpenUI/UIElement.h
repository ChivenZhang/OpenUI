#pragma once
#include "UI.h"

class UIElement;
using UIElementRef = UIRef<UIElement>;
using UIElementRaw = UIRaw<UIElement>;
class UIElementPrivate {};
using UIElementPrivateRaw = UIRaw<UIElementPrivate>;
class UIContext;
using UIContextRef = UIRef<UIContext>;
using UIContextRaw = UIRaw<UIContext>;

/// @brief 
class OPENUI_API UIElement
{
public:
	UIElement();
	virtual ~UIElement();
	UIElementRaw getParent() const;
	UIArrayView<const UIElementRef> getChildren() const;
	virtual UIString getStyle() const;
	virtual void setStyle(UIString value);
	virtual UIString getStyle(UIString name) const;
	virtual void setStyle(UIString name, UIString value);
	virtual bool addElement(UIElementRef value);
	virtual bool removeElement(UIElementRef value);
	virtual void removeElement();
	virtual void layout(UIRect client);
	virtual void paint(UIRect client, UIPainterRaw painter);

protected:
	virtual UIContextRaw getContext() const;
	virtual void setContext(UIContextRaw value);
	virtual void setParent(UIElementRaw value);

private:
	friend class UIContext;
	UIElementPrivateRaw m_Private;
};