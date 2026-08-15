#pragma once
/*=================================================
* Copyright © 2020-2025 ChivenZhang.
* All Rights Reserved.
* =====================Note=========================
*
*
* ====================History=======================
* Created by ChivenZhang@gmail.com.
*
* =================================================*/
#include "VGShape.h"

/// @brief 
class OPENVG_API VGTessellate
{
public:
	static bool Fill(VGShapeRaw element, VGPrimitiveRaw result);

	static bool Stroke(VGShapeRaw element, VGPrimitiveRaw result);
};