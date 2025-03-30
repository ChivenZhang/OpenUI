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
#include "CairoVKRender.h"

#include <SDL3/SDL_timer.h>

#include "../Cairo/CairoUIPainter.h"

static auto vsource = R"(
#version 330
layout (location = 0) in vec2 _point;
layout (location = 1) in uint _index;
out vec2 uv;
flat out uint index;
void main()
{
	index = _index;
	uv = vec2(_point.x, 1.0-_point.y);
	gl_Position = vec4(2*_point-1, 0.0, 1.0);
}
)";

static auto fsource = R"(
#version 330
in vec2 uv;
flat in uint index;
layout (location = 0) out vec4 color;
uniform sampler2D textureList[16];
void main()
{
	color = texture(textureList[index], uv);
}
)";

CairoVKRender::CairoVKRender(uint32_t width, uint32_t height, VkDevice device, VkCommandBuffer commandBuffer)
	:
	m_Device(device),
	m_CommandBuffer(commandBuffer)
{
}

CairoVKRender::~CairoVKRender()
{
}

void CairoVKRender::render(UIRect client, UIListView<UIPrimitive> data)
{
}