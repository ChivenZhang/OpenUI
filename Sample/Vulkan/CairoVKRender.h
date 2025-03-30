#pragma once
/*=================================================
* Copyright @ 2020-2025 ChivenZhang.
* All Rights Reserved.
* =====================Note=========================
*
*
* ====================History=======================
* Created by ChivenZhang at 2025/03/30 15:07:17.
*
* =================================================*/
#include <OpenUI/UIRender.h>

class CairoVKRender : public UIRender
{
public:
	using primitive_t = struct primitive_t
	{
		float X = 0, Y = 0;
		uint32_t Index = 0;
	};

public:
	CairoVKRender(uint32_t width, uint32_t height);
	~CairoVKRender() override;
	void render(UIRect client, UIListView<UIPrimitive> data) override;

protected:
	UIList<primitive_t> m_PrimitiveList;
};