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
#include "VGText.h"

class OPENVG_API VGTrueType
{
public:
	using point_t = VGPoint;
	using index_t = uint32_t;
	using image_t = VGImage;

public:
	static bool Path(VGTextRaw element, VGRect rect, VGString const& text, VGList<VGPoint>& points, VGList<VGPointType>& types);

	static bool Fill(VGTextRaw element, VGRect client, VGString const& text, VGPrimitiveRaw result);

	static bool Stroke(VGTextRaw element, VGRect client, VGString const& text, VGPrimitiveRaw result);

	static bool Measure(VGTextRaw element, VGRect client, VGString const& text, float cursor, VGRect* cursorRect, VGRect& result);

	static bool Measure(VGTextRaw element, VGRect client, VGString const& text, float posX, float posY, float* cursor, VGRect* cursorRect, VGRect& result);
};