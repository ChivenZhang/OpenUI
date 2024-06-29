#pragma once
#include "Widget/IRmGUIRender.h"

class OpenGLRender : public IRmGUIRender
{
public:
	OpenGLRender();
	~OpenGLRender();
	virtual void render(RmArrayView<RmPrimitive> data) override;

protected:
	uint32_t m_NativeProgram;
};