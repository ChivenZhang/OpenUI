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
#include "../VGGradient.h"

VGLinearGradient::VGLinearGradient(float x0, float y0, float x1, float y1)
{
}

VGPoint VGLinearGradient::getStartPos() const
{
	return VGPoint();
}

VGPoint VGLinearGradient::getEndPos() const
{
	return VGPoint();
}

VGRadialGradient::VGRadialGradient(float x0, float y0, float r0)
{
}

VGPoint VGRadialGradient::getCenterPos() const
{
	return VGPoint();
}

float VGRadialGradient::getRadius() const
{
	return 0.0f;
}
