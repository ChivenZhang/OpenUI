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
class OPENVG_API VGText : public VGElement
{
public:
	VGText();
	~VGText();

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

	VGString getFamily() const;
	void setFamily(VGString value);

	uint32_t getSize() const;
	void setSize(uint32_t value);

	uint32_t getSpacing() const;
	void setSpacing(uint32_t value);

	float getLineSpacing() const;
	void setLineSpacing(float value);

	bool getLineWrap() const;
	void setLineWrap(bool value);

	VGTextStyle getStyle() const;
	void setStyle(VGTextStyle value);

	VGTextWeight getWeight() const;
	void setWeight(VGTextWeight value);

	VGTextAlgins getAlignment() const;
	void setAlignment(VGTextAlgins value);

	VGTextDirection getDirection() const;
	void setDirection(VGTextDirection value);

	VGTextEllipsize getEllipsize() const;
	void setEllipsize(VGTextEllipsize value);

	VGString getString() const;
	void setText(float x, float y, float width, float height, VGString const& text);
	void setText(VGRect client, VGString const& text) { setText(client.X, client.Y, client.W, client.H, text); }

protected:
	virtual void clip() override;
	virtual void fill() override;
	virtual void stroke() override;

private:
	VGPrivateRaw m_Private;
};
using VGTextRef = VGRef<VGText>;
using VGTextRaw = VGRaw<VGText>;