#pragma once
#include "UIElement.h"

/// @brief Base interface of factories
class OPENUI_API UIFactory
{
public:
	virtual ~UIFactory() = default;

	virtual UIString getTagName() const = 0;

	virtual UIElementRef getElement(UIString style) const = 0;
};
using UIFactoryRef = UIRef<UIFactory>;
using UIFactoryRaw = UIRaw<UIFactory>;