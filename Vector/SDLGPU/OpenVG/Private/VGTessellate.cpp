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
#include "../VGTessellate.h"
// #include <skia/core/SkPath.h>
// #include <skia/core/SkPaint.h>
// #include <skia/core/SkRefCnt.h>
// #include <skia/core/SkStrokeRec.h>
// #include <skia/core/SkPathEffect.h>
// #include <skia/effects/SkDashPathEffect.h>
// #include <skia/private/base/SkTemplates.h>
// #include <skia/src/gpu/ganesh/GrEagerVertexAllocator.h>
// #include <skia/src/gpu/ganesh/geometry/GrPathUtils.h>
// #include <skia/src/gpu/ganesh/geometry/GrTriangulator.h>
//
// class SimpleVertexAllocator : public GrEagerVertexAllocator
// {
// public:
// 	void* lock(size_t stride, int eagerCount) override
// 	{
// 		SkASSERT(!fPoints);
// 		SkASSERT(stride == sizeof(SkPoint));
// 		fPoints.reset(eagerCount);
// 		return fPoints;
// 	}
// 	void unlock(int actualCount) override {}
// 	SkPoint operator[](int idx) const { return fPoints[idx]; }
// 	skia_private::AutoTMalloc<SkPoint> fPoints;
// };

bool VGTessellate::Fill(VGShapeRaw element, VGPrimitiveRaw result)
{
	// auto points = element->getPointList();
	// auto types = element->getTypeList();
	//
	// SkPath path;
	// for (size_t i = 0, k = 0; i < types.size(); ++i)
	// {
	// 	switch (types[i])
	// 	{
	// 	case VGPointType::MoveTo:
	// 	{
	// 		path.moveTo({ points[k].X, points[k].Y });
	// 		k += 1;
	// 	} break;
	// 	case VGPointType::LineTo:
	// 	{
	// 		path.lineTo({ points[k].X, points[k].Y });
	// 		k += 1;
	// 	} break;
	// 	case VGPointType::CurveTo:
	// 	{
	// 		auto c1 = points[k + 0];
	// 		auto last = points[k + 1];
	// 		path.quadTo({ c1.X, c1.Y }, { last.X, last.Y });
	// 		k += 2;
	// 	} break;
	// 	case VGPointType::CubicTo:
	// 	{
	// 		auto c1 = points[k + 0];
	// 		auto c2 = points[k + 1];
	// 		auto end = points[k + 2];
	// 		path.cubicTo({ c1.X, c1.Y }, { c2.X, c2.Y }, { end.X, end.Y });
	// 		k += 3;
	// 	} break;
	// 	case VGPointType::ArcTo:
	// 	{
	// 		auto c1 = points[k + 0];
	// 		auto r1 = points[k + 1];
	// 		auto a1 = points[k + 2];
	// 		path.arcTo({ c1.X - r1.X, c1.Y - r1.Y, c1.X + r1.X, c1.Y + r1.Y }, a1.X, a1.Y, false);
	// 		k += 3;
	// 	} break;
	// 	case VGPointType::PieTo:
	// 	{
	// 		auto c1 = points[k + 0];
	// 		auto r1 = points[k + 1];
	// 		auto a1 = points[k + 2];
	// 		path.arcTo({ c1.X - r1.X, c1.Y - r1.Y, c1.X + r1.X, c1.Y + r1.Y }, a1.X, a1.Y, false);
	// 		k += 3;
	// 	} break;
	// 	case VGPointType::ChordTo:
	// 	{
	// 		auto c1 = points[k + 0];
	// 		auto r1 = points[k + 1];
	// 		auto a1 = points[k + 2];
	// 		path.arcTo({ c1.X - r1.X, c1.Y - r1.Y, c1.X + r1.X, c1.Y + r1.Y }, a1.X, a1.Y, false);
	// 		k += 3;
	// 	} break;
	// 	case VGPointType::Close:
	// 	{
	// 		path.close();
	// 	} break;
	// 	}
	// }
	//
	// bool isLinear = false;
	// SimpleVertexAllocator allocator;
	// auto count = GrTriangulator::PathToTriangles(path, GrPathUtils::kDefaultTolerance, path.getBounds(), &allocator, &isLinear);
	// if (count)
	// {
	// 	auto& vertices = allocator.fPoints;
	// 	auto& outPoints = result->PointList;
	// 	for (size_t i = 0; i + 3 <= count; i += 3)
	// 	{
	// 		outPoints.push_back({ vertices[i + 0].fX, vertices[i + 0].fY, -1, -1, 0, 0 });
	// 		outPoints.push_back({ vertices[i + 1].fX, vertices[i + 1].fY, -1, -1, 0, 0 });
	// 		outPoints.push_back({ vertices[i + 2].fX, vertices[i + 2].fY, -1, -1, 0, 0 });
	// 	}
	// 	return true;
	// }
	return false;
}

bool VGTessellate::Stroke(VGShapeRaw element, VGPrimitiveRaw result)
{
	// auto points = element->getPointList();
	// auto types = element->getTypeList();
	//
	// SkPath path;
	// for (size_t i = 0, k = 0; i < types.size(); ++i)
	// {
	// 	switch (types[i])
	// 	{
	// 	case VGPointType::MoveTo:
	// 	{
	// 		path.moveTo({ points[k].X, points[k].Y });
	// 		k += 1;
	// 	} break;
	// 	case VGPointType::LineTo:
	// 	{
	// 		path.lineTo({ points[k].X, points[k].Y });
	// 		k += 1;
	// 	} break;
	// 	case VGPointType::CurveTo:
	// 	{
	// 		auto c1 = points[k + 0];
	// 		auto last = points[k + 1];
	// 		path.quadTo({ c1.X, c1.Y }, { last.X, last.Y });
	// 		k += 2;
	// 	} break;
	// 	case VGPointType::CubicTo:
	// 	{
	// 		auto c1 = points[k + 0];
	// 		auto c2 = points[k + 1];
	// 		auto end = points[k + 2];
	// 		path.cubicTo({ c1.X, c1.Y }, { c2.X, c2.Y }, { end.X, end.Y });
	// 		k += 3;
	// 	} break;
	// 	case VGPointType::ArcTo:
	// 	{
	// 		auto c1 = points[k + 0];
	// 		auto r1 = points[k + 1];
	// 		auto a1 = points[k + 2];
	// 		path.arcTo({ c1.X - r1.X, c1.Y - r1.Y, c1.X + r1.X, c1.Y + r1.Y }, a1.X, a1.Y, false);
	// 		k += 3;
	// 	} break;
	// 	case VGPointType::PieTo:
	// 	{
	// 		auto c1 = points[k + 0];
	// 		auto r1 = points[k + 1];
	// 		auto a1 = points[k + 2];
	// 		path.arcTo({ c1.X - r1.X, c1.Y - r1.Y, c1.X + r1.X, c1.Y + r1.Y }, a1.X, a1.Y, false);
	// 		k += 3;
	// 	} break;
	// 	case VGPointType::ChordTo:
	// 	{
	// 		auto c1 = points[k + 0];
	// 		auto r1 = points[k + 1];
	// 		auto a1 = points[k + 2];
	// 		path.arcTo({ c1.X - r1.X, c1.Y - r1.Y, c1.X + r1.X, c1.Y + r1.Y }, a1.X, a1.Y, false);
	// 		k += 3;
	// 	} break;
	// 	case VGPointType::Close:
	// 	{
	// 		path.close();
	// 	} break;
	// 	}
	// }
	//
	// auto strokeWidth = element->getLineWidth();
	// auto strokeMiter = element->getMiterLimit();
	// auto dashControl = element->getLineDash();
	// auto dashOffset = (int32_t)element->getDashOffset();
	// auto strokeCap = SkPaint::kButt_Cap;
	// auto strokeJoin = SkPaint::kBevel_Join;
	// switch (element->getLineCap())
	// {
	// case VGStrokeCap::Butt: strokeCap = SkPaint::kButt_Cap; break;
	// case VGStrokeCap::Round: strokeCap = SkPaint::kRound_Cap; break;
	// case VGStrokeCap::Square: strokeCap = SkPaint::kSquare_Cap; break;
	// }
	// switch (element->getLineJoin())
	// {
	// case VGStrokeJoin::Bevel: strokeJoin = SkPaint::kBevel_Join; break;
	// case VGStrokeJoin::Miter: strokeJoin = SkPaint::kMiter_Join; break;
	// case VGStrokeJoin::Round: strokeJoin = SkPaint::kRound_Join; break;
	// }
	// SkPaint paint;
	// paint.setStyle(SkPaint::kStroke_Style);
	// paint.setStrokeWidth(strokeWidth);
	// paint.setStrokeCap(strokeCap);
	// paint.setStrokeJoin(strokeJoin);
	// paint.setStrokeMiter(strokeMiter);
	// if (dashControl.size())
	// {
	// 	auto dashEffect = SkDashPathEffect::Make(dashControl.data(), dashControl.size(), dashOffset);
	// 	paint.setPathEffect(dashEffect);
	// }
	// SkStrokeRec rec(paint, 1.0f);
	// rec.applyToPath(&path, path);
	//
	// bool isLinear = false;
	// SimpleVertexAllocator allocator;
	// auto count = GrTriangulator::PathToTriangles(path, GrPathUtils::kDefaultTolerance, path.getBounds(), &allocator, &isLinear);
	// if (count)
	// {
	// 	auto& vertices = allocator.fPoints;
	// 	auto& outPoints = result->PointList;
	// 	for (size_t i = 0; i + 3 <= count; i += 3)
	// 	{
	// 		outPoints.push_back({ vertices[i + 0].fX, vertices[i + 0].fY, -1, -1, 0, 0 });
	// 		outPoints.push_back({ vertices[i + 1].fX, vertices[i + 1].fY, -1, -1, 0, 0 });
	// 		outPoints.push_back({ vertices[i + 2].fX, vertices[i + 2].fY, -1, -1, 0, 0 });
	// 	}
	// 	return true;
	// }
	return false;
}