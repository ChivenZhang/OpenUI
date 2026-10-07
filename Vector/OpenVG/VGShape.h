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
#include "VGElement.h"

/// @brief 
class OPENVG_API VGShape : public VGElement
{
public:
	VGShape();
	~VGShape();

	VGColor getFillColor() const;
	void setFillColor(VGColor value);

	VGColor getStrokeColor() const;
	void setStrokeColor(VGColor value);

	float getLineWidth() const;
	void setLineWidth(float value);

	float getMiterLimit() const;
	void setMiterLimit(float value);

	VGStrokeCap getLineCap() const;
	void setLineCap(VGStrokeCap value);

	VGStrokeJoin getLineJoin() const;
	void setLineJoin(VGStrokeJoin value);

	float getDashOffset() const;
	VGListView<const float> getLineDash() const;
	void setLineDash(VGList<float> value, float offset = 0);

	VGListView<const VGPoint> getPointList() const;
	VGListView<const VGPointType> getTypeList() const;

	void moveTo(float x, float y);
	void moveTo(VGPoint p) { moveTo(p.X, p.Y); }
	void lineTo(float x, float y);
	void lineTo(VGPoint p) { lineTo(p.X, p.Y); }
	void curveTo(float cx1, float cy1, float x, float y);
	void curveTo(VGPoint p1, VGPoint p2) { curveTo(p1.X, p1.Y, p2.X, p2.Y); }
	void cubicTo(float cx1, float cy1, float cx2, float cy2, float x, float y);
	void cubicTo(VGPoint p1, VGPoint p2, VGPoint p3) { cubicTo(p1.X, p1.Y, p2.X, p2.Y, p3.X, p3.Y); }
	void arcTo(float cx1, float cy1, float rx, float ry, float a1, float a2);
	void arcTo(VGPoint p, float rx, float ry, float a1, float a2) { arcTo(p.X, p.Y, rx, ry, a1, a2); }
	void pieTo(float cx1, float cy1, float rx, float ry, float a1, float a2);
	void pieTo(VGPoint p, float rx, float ry, float a1, float a2) { pieTo(p.X, p.Y, rx, ry, a1, a2); }
	void chordTo(float cx1, float cy1, float rx, float ry, float a1, float a2);
	void chordTo(VGPoint p, float rx, float ry, float a1, float a2) { chordTo(p.X, p.Y, rx, ry, a1, a2); }
	void close();
	void reset();

	void arc(float x, float y, float width, float height, float startAngle, float spanAngle);
	void pie(float x, float y, float width, float height, float startAngle, float spanAngle);
	void chord(float x, float y, float width, float height, float startAngle, float spanAngle);
	void ellipse(float x, float y, float width, float height);
	void rect(float x, float y, float width, float height, float roundX = 0, float roundY = 0);
	void line(float x1, float y1, float x2, float y2);
	void point(float x, float y);

	template<class... Args>
	void points(float x, float y, Args... args)
	{
		point(x, y);
		points(args...);
	}
	void points(float x, float y)
	{
		point(x, y);
	}

	template<class... Args>
	void points(VGPoint p, Args... args)
	{
		point(p.X, p.Y);
		points(args...);
	}
	void points(VGPoint p)
	{
		point(p.X, p.Y);
	}

	template<class... Args>
	void lines(VGPoint p1, VGPoint p2, Args... args)
	{
		line(p1.X, p1.Y, p2.X, p2.Y);
		lines(args...);
	}
	void lines(VGPoint p1, VGPoint p2)
	{
		line(p1.X, p1.Y, p2.X, p2.Y);
	}

	template<class... Args>
	void lines(float x1, float y1, float x2, float y2, Args... args)
	{
		line(x1, y1, x2, y2);
		lines(args...);
	}
	void lines(float x1, float y1, float x2, float y2)
	{
		line(x1, y1, x2, y2);
	}

protected:
	virtual void clip() override;
	virtual void fill() override;
	virtual void stroke() override;

private:
	VGPrivateRaw m_Private;
};
using VGShapeRef = VGRef<VGShape>;
using VGShapeRaw = VGRaw<VGShape>;