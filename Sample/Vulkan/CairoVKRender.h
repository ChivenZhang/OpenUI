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
#include <vulkan/vulkan_core.h>
class SDL3VKDevice;

class CairoVKRender : public UIRender
{
public:
	using primitive_t = struct primitive_t
	{
		float X = 0, Y = 0;
		uint32_t Index = 0;
	};

public:
	CairoVKRender(uint32_t width, uint32_t height, UIRaw<SDL3VKDevice> device);
	~CairoVKRender() override;
	void render(UIRect client, UIListView<UIPrimitive> data) override;

protected:
	VkShaderModule createShaderModule(VkDevice device, int32_t stage, const char* source);

protected:
	UIRaw<SDL3VKDevice> m_Device;
	VkPipeline m_Pipeline;
	VkPipelineLayout m_PipelineLayout;
	VkDescriptorSet m_DescriptorSet;
	VkDescriptorSetLayout m_DescriptorSetLayout;
	VkImage m_Texture; VkImageView m_TextureView; VkDeviceMemory m_TextureMemory;
	VkBuffer m_VertexBuffer; VkDeviceMemory m_VertexBufferMemory;
	UIList<primitive_t> m_PrimitiveList;
};
