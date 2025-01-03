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
#include <OpenUI/UIRender.h>

class CairoGLRender : public UIRender
{
public:
	using primitive_t = struct primitive_t
	{
		float X = 0, Y = 0;
		uint32_t Index = 0;
	};

public:
	CairoGLRender();
	~CairoGLRender();
	virtual void render(UIRect client, UIArrayView<UIPrimitive> data) override;

protected:
	uint32_t m_NativeProgram;
	uint32_t m_NativeBuffer;
	uint32_t m_NativePrimitive;
	UIList<primitive_t> m_PrimitiveList;
};