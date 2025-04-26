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
#ifdef OPENUI_ENABLE_OPENGL
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
	CairoGLRender(uint32_t width, uint32_t height);
	~CairoGLRender() override;
	void render(UIRect client, UIListView<UIPrimitive> data) override;

protected:
	uint32_t m_NativeProgram;
	uint32_t m_NativeBuffer;
	uint32_t m_NativePrimitive;
	uint32_t m_NativeTexture;
	UIList<primitive_t> m_PrimitiveList;
};
#endif