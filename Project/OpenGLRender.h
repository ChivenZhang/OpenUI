#pragma once
#include "Widget/IRmGUIRender.h"

class OpenGLRender : public IRmGUIRender
{
public:
	using primitive_t = struct primitive_t
	{
		float X = 0, Y = 0, U = 0, V = 0;
		uint32_t Index;
	};

public:
	OpenGLRender();
	~OpenGLRender();
	virtual void render(RmRect client, RmArrayView<RmPrimitive> data) override;

protected:
	uint32_t m_NativeProgram;
	uint32_t m_NativeBuffer;
	uint32_t m_NativePrimitive;
	RmVector<primitive_t> m_PrimitiveList;
};