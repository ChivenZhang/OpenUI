/*=================================================
* Copyright @ 2020-2025 ChivenZhang.
* All Rights Reserved.
* =====================Note=========================
*
*
* ====================History=======================
* Created by ChivenZhang at 2025/03/30 15:07:30.
*
* =================================================*/
#ifdef OPENUI_ENABLE_DIRECTX
#include <glslang/Public/resource_limits_c.h>
#include <glslang/Include/glslang_c_interface.h>
#include "SDL3DXDevice.h"
#include "CairoDXRender.h"
#include "../Cairo/CairoUIPainter.h"

static auto vsource = R"(
#version 450
layout (location = 0) in vec2 _point;
layout (location = 1) in uint _index;
layout (location = 0) out vec2 uv;
layout (location = 1) flat out uint index;
void main()
{
	index = _index;
	uv = vec2(_point.x, _point.y);
	gl_Position = vec4(2*_point-1, 0.0, 1.0);
}
)";

static auto fsource = R"(
#version 450
layout (location = 0) in vec2 uv;
layout (location = 1) flat in uint index;
layout (location = 0) out vec4 color;
layout (binding = 0) uniform sampler2D textureList[16];
void main()
{
	color = texture(textureList[index], uv);
}
)";

CairoDXRender::CairoDXRender(uint32_t width, uint32_t height, UIRaw<SDL3DXDevice> _device)
	:
	m_Device(_device)
{

}

CairoDXRender::~CairoDXRender()
{
}

void CairoDXRender::render(UIRect client, UIListView<UIPrimitive> data)
{
}

void CairoDXRender::uploadTexture(int32_t width, int32_t height, uint8_t* pixels)
{
}

ComPtr<ID3DBlob> CairoDXRender::createShaderModule(ComPtr<ID3D12Device> device,  int32_t stage, const char* source)
{
  return nullptr;
}
#endif