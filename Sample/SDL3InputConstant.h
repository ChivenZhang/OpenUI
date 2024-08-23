#pragma once
#include <OpenUI/UI.h>

/// @brief 
class SDL3InputConstant
{
public:
	static UIInputEnum GetMouseEnum(int key);
	static UIInputEnum GetKeyboardEnum(int key);
	static UIInputEnum GetGamepadEnum(int key);
	static UIInputEnums GetModifierEnum(int key);
};
