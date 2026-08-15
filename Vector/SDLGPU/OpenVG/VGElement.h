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
#include "VGGradient.h"
class VGContext;
class VGTessellate;

struct VGFillStyle
{
	VGColor Color = { 1,1,1,1 };
	VGImage Image;
	VGGradientRef Gradient;
};
using VGFillStyleRef = VGRef<VGFillStyle>;
using VGFillStyleRaw = VGRaw<VGFillStyle>;

struct VGStrokeStyle
{
	VGColor Color = { 0,0,0,1 };
	VGImage Image;
	VGGradientRef Gradient;
	VGStrokeCap LineCap;
	VGStrokeJoin LineJoin;
	float LineWidth = 1;
	float MiterLimit = 4;
	float DashOffset = 0;
	VGList<float> DashControl;
};
using VGStrokeStyleRef = VGRef<VGStrokeStyle>;
using VGStrokeStyleRaw = VGRaw<VGStrokeStyle>;

/// @brief 
class OPENVG_API VGElement
{
public:
	VGElement();
	virtual ~VGElement();

	float getRotate() const;
	void setRotate(float value);

	VGFloat2 getScale() const;
	void setScale(float sx, float sy);
	void setScale(VGFloat2 value) { setScale(value.X, value.Y); }

	VGFloat2 getTranslate() const;
	void setTranslate(float tx, float ty);
	void setTranslate(VGFloat2 value) { setTranslate(value.X, value.Y); }

	VGFloat4 getScissor() const;
	void setScissor(float x, float y, float width, float height);
	void setScissor(VGFloat4 value) { setScissor(value[0], value[1], value[2], value[3]); }

protected:
	virtual void clip() = 0;
	virtual void fill() = 0;
	virtual void stroke() = 0;

	VGPrimitiveRaw getClipCache() const;
	void setClipCache(VGPrimitiveRef value);
	VGPrimitiveRaw getFillCache() const;
	void setFillCache(VGPrimitiveRef value);
	VGPrimitiveRaw getStrokeCache() const;
	void setStrokeCache(VGPrimitiveRef value);

protected:
	friend class VGContext;
	friend class VGPainter;
	friend class VGTessellate;

	VGPrivateRaw m_Private;
};
using VGElementRef = VGRef<VGElement>;
using VGElementRaw = VGRaw<VGElement>;