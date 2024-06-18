#pragma once
#include "RmGUI.h"

/// @brief 
class HashUtility
{
public:
	inline static constexpr uint32_t Encrypt(RmCString value)
	{
		uint32_t hash = 0; // From JDK
		if (value == nullptr) return hash;
		while (*value) hash = hash * 31 + (*value++);
		return hash;
	}
};