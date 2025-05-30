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
#ifdef OPENUI_ENABLE_VULKAN
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
	void uploadTexture(int32_t width, int32_t height, uint8_t* pixels);

protected:
	VkShaderModule createShaderModule(VkDevice device, int32_t stage, const char* source);

protected:
	UIRaw<SDL3VKDevice> m_Device;
	VkPipeline m_Pipeline;
	VkPipelineLayout m_PipelineLayout;
	VkDescriptorSet m_DescriptorSet;
	VkDescriptorSetLayout m_DescriptorSetLayout;
	VkBuffer m_StageBuffer; VkDeviceMemory m_StageBufferMemory;
	VkBuffer m_VertexBuffer; VkDeviceMemory m_VertexBufferMemory;
	VkImage m_Texture; VkImageView m_TextureView; VkDeviceMemory m_TextureMemory; VkSampler m_Sampler;
	uint32_t m_Width, m_Height;
	UIList<primitive_t> m_PrimitiveList;
};
#endif