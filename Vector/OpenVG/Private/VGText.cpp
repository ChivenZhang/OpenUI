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
#include "../VGText.h"
#include "../VGTrueType.h"

struct VGTextPrivate : VGPrivate
{
	uint32_t Size = 14;
	uint32_t Spacing = 0;
	float LineSpacing = 0.5;
	bool LineWrap = false;
	VGString Text;
	VGRect Client;
	VGFillStyleRef FillStyle;
	VGStrokeStyleRef StrokeStyle;
	VGString Family = "微软雅黑,Segoe UI Emoji";
	VGTextStyle Style = VGTextStyle::StyleNormal;
	VGTextWeight Weight = VGTextWeight::WeightNormal;
	VGTextAlgins Aligns = VGTextAlgin::AlignLeft | VGTextAlgin::AlignTop;
	VGTextDirection Direction = VGTextDirection::DirectionAutoLayout;
	VGTextEllipsize Ellipsize = VGTextEllipsize::EllipsizeNone;
};
#define PRIVATE() ((VGTextPrivate*)m_Private)

VGText::VGText()
{
	m_Private = new VGTextPrivate;
}

VGText::~VGText()
{
	delete m_Private; m_Private = nullptr;
}

VGColor VGText::getFillColor() const
{
	if (PRIVATE()->FillStyle == nullptr) return VGColor();
	return PRIVATE()->FillStyle->Color;
}

void VGText::setFillColor(VGColor value)
{
	if (PRIVATE()->FillStyle == nullptr) PRIVATE()->FillStyle = VGNew<VGFillStyle>();
	PRIVATE()->FillStyle->Color = value;
}

VGColor VGText::getStrokeColor() const
{
	if (PRIVATE()->StrokeStyle == nullptr) return VGColor();
	return PRIVATE()->StrokeStyle->Color;
}

void VGText::setStrokeColor(VGColor value)
{
	if (PRIVATE()->StrokeStyle == nullptr) PRIVATE()->StrokeStyle = VGNew<VGStrokeStyle>();
	PRIVATE()->StrokeStyle->Color = value;
}

float VGText::getLineWidth() const
{
	if (PRIVATE()->StrokeStyle == nullptr) return float();
	return PRIVATE()->StrokeStyle->LineWidth;
}

void VGText::setLineWidth(float value)
{
	if (PRIVATE()->StrokeStyle == nullptr) PRIVATE()->StrokeStyle = VGNew<VGStrokeStyle>();
	PRIVATE()->StrokeStyle->LineWidth = value;
}

float VGText::getMiterLimit() const
{
	if (PRIVATE()->StrokeStyle == nullptr) return float();
	return PRIVATE()->StrokeStyle->MiterLimit;
}

void VGText::setMiterLimit(float value)
{
	if (PRIVATE()->StrokeStyle == nullptr) PRIVATE()->StrokeStyle = VGNew<VGStrokeStyle>();
	PRIVATE()->StrokeStyle->MiterLimit = value;
}

VGStrokeCap VGText::getLineCap() const
{
	if (PRIVATE()->StrokeStyle == nullptr) return VGStrokeCap::Square;
	return PRIVATE()->StrokeStyle->LineCap;
}

void VGText::setLineCap(VGStrokeCap value)
{
	if (PRIVATE()->StrokeStyle == nullptr) PRIVATE()->StrokeStyle = VGNew<VGStrokeStyle>();
	PRIVATE()->StrokeStyle->LineCap = value;
}

VGStrokeJoin VGText::getLineJoin() const
{
	if (PRIVATE()->StrokeStyle == nullptr) return VGStrokeJoin::Bevel;
	return PRIVATE()->StrokeStyle->LineJoin;
}

void VGText::setLineJoin(VGStrokeJoin value)
{
	if (PRIVATE()->StrokeStyle == nullptr) PRIVATE()->StrokeStyle = VGNew<VGStrokeStyle>();
	PRIVATE()->StrokeStyle->LineJoin = value;
}

float VGText::getDashOffset() const
{
	if (PRIVATE()->StrokeStyle == nullptr) return float();
	return PRIVATE()->StrokeStyle->DashOffset;
}

VGListView<const float> VGText::getLineDash() const
{
	if (PRIVATE()->StrokeStyle == nullptr) return VGListView<const float>();
	return PRIVATE()->StrokeStyle->DashControl;
}

void VGText::setLineDash(VGList<float> value, float offset)
{
	if (PRIVATE()->StrokeStyle == nullptr) PRIVATE()->StrokeStyle = VGNew<VGStrokeStyle>();
	PRIVATE()->StrokeStyle->DashControl = value;
	PRIVATE()->StrokeStyle->DashOffset = offset;
}

VGString VGText::getFamily() const
{
	return PRIVATE()->Family;
}

void VGText::setFamily(VGString value)
{
	setClipCache(nullptr);
	setFillCache(nullptr);
	setStrokeCache(nullptr);
	PRIVATE()->Family = value;
}

uint32_t VGText::getSize() const
{
	return PRIVATE()->Size;
}

void VGText::setSize(uint32_t value)
{
	setClipCache(nullptr);
	setFillCache(nullptr);
	setStrokeCache(nullptr);
	PRIVATE()->Size = value;
}

uint32_t VGText::getSpacing() const
{
	return PRIVATE()->Spacing;
}

void VGText::setSpacing(uint32_t value)
{
	setClipCache(nullptr);
	setFillCache(nullptr);
	setStrokeCache(nullptr);
	PRIVATE()->Spacing = value;
}

float VGText::getLineSpacing() const
{
	return PRIVATE()->LineSpacing;
}

void VGText::setLineSpacing(float value)
{
	setClipCache(nullptr);
	setFillCache(nullptr);
	setStrokeCache(nullptr);
	PRIVATE()->LineSpacing = value;
}

bool VGText::getLineWrap() const
{
	return PRIVATE()->LineWrap;
}

void VGText::setLineWrap(bool value)
{
	setClipCache(nullptr);
	setFillCache(nullptr);
	setStrokeCache(nullptr);
	PRIVATE()->LineWrap = value;
}

VGTextStyle VGText::getStyle() const
{
	return PRIVATE()->Style;
}

void VGText::setStyle(VGTextStyle value)
{
	setClipCache(nullptr);
	setFillCache(nullptr);
	setStrokeCache(nullptr);
	PRIVATE()->Style = value;
}

VGTextWeight VGText::getWeight() const
{
	return PRIVATE()->Weight;
}

void VGText::setWeight(VGTextWeight value)
{
	setClipCache(nullptr);
	setFillCache(nullptr);
	setStrokeCache(nullptr);
	PRIVATE()->Weight = value;
}

VGTextAlgins VGText::getAlignment() const
{
	return PRIVATE()->Aligns;
}

void VGText::setAlignment(VGTextAlgins value)
{
	setClipCache(nullptr);
	setFillCache(nullptr);
	setStrokeCache(nullptr);
	PRIVATE()->Aligns = value;
}

VGTextDirection VGText::getDirection() const
{
	return PRIVATE()->Direction;
}

void VGText::setDirection(VGTextDirection value)
{
	setClipCache(nullptr);
	setFillCache(nullptr);
	setStrokeCache(nullptr);
	PRIVATE()->Direction = value;
}

VGTextEllipsize VGText::getEllipsize() const
{
	return PRIVATE()->Ellipsize;
}

void VGText::setEllipsize(VGTextEllipsize value)
{
	setClipCache(nullptr);
	setFillCache(nullptr);
	setStrokeCache(nullptr);
	PRIVATE()->Ellipsize = value;
}

VGString VGText::getString() const
{
	return PRIVATE()->Text;
}

void VGText::setText(float x, float y, float width, float height, VGString const& text)
{
	setClipCache(nullptr);
	setFillCache(nullptr);
	setStrokeCache(nullptr);
	PRIVATE()->Text = text;
	PRIVATE()->Client = { x, y, width, height };
}

void VGText::clip()
{
}

void VGText::fill()
{
	if (PRIVATE()->Text.empty()) return;
	if (PRIVATE()->FillStyle == nullptr) return;

	if (getFillCache() == nullptr)
	{
		auto cache = VGNew<VGPrimitive>();
		VGTrueType::Fill(this, PRIVATE()->Client, PRIVATE()->Text, cache.get());
		setFillCache(cache);
	}

	auto cache = getFillCache();
	auto& points = cache->PointList;
	auto& styles = cache->StyleList;
	auto& images = cache->ImageList;
	auto& linears = cache->LinearList; linears.clear();
	auto& radials = cache->RadialList; radials.clear();
	auto& matrixs = cache->MatrixList; matrixs.clear();
	auto& scissors = cache->ScissorList; scissors.clear();

	auto style = PRIVATE()->FillStyle.get();
	auto& matrix = matrixs.emplace_back();
	matrix.Scissor = getScissor();
	matrix.Transform = VGFloat3x3::Transform(getTranslate().X, getTranslate().Y, getRotate(), getScale().X, getScale().Y);

	for (size_t i = 0; i < styles.size(); ++i)
	{
		styles[i].Flags |= (VG_FLAGS_IMAGE_COLOR);
		styles[i].Flags |= (VG_FLAGS_IMAGE_GLYPH);
	}
	for (size_t i = 0; style && i < styles.size(); ++i)
	{
		styles[i].Color = style->Color;
	}
	if (style && VGCast<VGLinearGradient>(style->Gradient))
	{
		for (size_t i = 0; i < styles.size(); ++i)
		{
			styles[i].Flags |= (VG_FLAGS_STYLE_LINEAR);
			styles[i].Linear = (int32_t)linears.size();
		}

		auto gradient = VGCast<VGLinearGradient>(style->Gradient).get();
		auto& linear = linears.emplace_back();
		auto stops = gradient->getColorStop();
		if (stops.size())
		{
			linear.GradStartPos.X = gradient->getStartPos().X;
			linear.GradStartPos.Y = gradient->getStartPos().Y;
			linear.GradEndPos.X = gradient->getEndPos().X;
			linear.GradEndPos.Y = gradient->getEndPos().Y;
			linear.NumStops.X = (uint32_t)stops.size();
			for (size_t i = 0; i < stops.size(); ++i)
			{
				linear.StopColors[i].R = stops[i].R;
				linear.StopColors[i].G = stops[i].G;
				linear.StopColors[i].B = stops[i].B;
				linear.StopColors[i].A = stops[i].A;
				linear.StopPoints[i].X = stops[i].Offset;
			}
		}
	}
	if (style && VGCast<VGRadialGradient>(style->Gradient))
	{
		for (size_t i = 0; i < styles.size(); ++i)
		{
			styles[i].Flags |= (VG_FLAGS_STYLE_RADIAL);
			styles[i].Radial = (int32_t)radials.size();
		}

		auto gradient = VGCast<VGRadialGradient>(style->Gradient).get();
		auto& radial = radials.emplace_back();
		auto stops = gradient->getColorStop();
		if (stops.size())
		{
			radial.Radius.X = gradient->getRadius();
			radial.CenterPos.X = gradient->getCenterPos().X;
			radial.CenterPos.Y = gradient->getCenterPos().Y;
			radial.NumStops.X = (uint32_t)stops.size();
			for (size_t i = 0; i < stops.size(); ++i)
			{
				radial.StopColors[i].R = stops[i].R;
				radial.StopColors[i].G = stops[i].G;
				radial.StopColors[i].B = stops[i].B;
				radial.StopColors[i].A = stops[i].A;
				radial.StopPoints[i].X = stops[i].Offset;
			}
		}
	}
}

void VGText::stroke()
{
}
