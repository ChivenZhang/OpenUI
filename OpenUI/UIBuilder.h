#pragma once
#include "UIFactory.h"

class OPENUI_API UIBuilder
{
public:
	UIBuilder();
	virtual ~UIBuilder();
	virtual bool addFactory(UIFactoryRef value);
	virtual bool removeFactory(UIString name);
	virtual void removeFactory();
	virtual UIElementRef buildElement(UIString html, UIString css = UIString()) const;

private:
	UIElementPrivateRaw m_Private;
};