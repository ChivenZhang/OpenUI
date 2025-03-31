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

class CairoVKRender : public UIRender
{
public:
	using primitive_t = struct primitive_t
	{
		float X = 0, Y = 0;
		uint32_t Index = 0;
	};

public:
	CairoVKRender(uint32_t width, uint32_t height, VkDevice device, VkPipelineCache pipelineCache, VkDescriptorPool descriptorPool, VkCommandBuffer commandBuffer);
	~CairoVKRender() override;
	void render(UIRect client, UIListView<UIPrimitive> data) override;
	void setOutputView(VkImageView value);

protected:
	VkDevice m_Device;
	VkImageView m_ColorImageView;
	VkCommandBuffer m_CommandBuffer;
	VkPipeline m_Pipeline;
	VkPipelineLayout m_PipelineLayout;
	VkDescriptorPool m_DescriptorPool;
	VkDescriptorSet m_DescriptorSet;
	VkDescriptorSetLayout m_DescriptorSetLayout;
	VkImage m_TempImage; VkImageView m_TempImageView; VkDeviceMemory m_TempImageMemory;
	VkBuffer m_VertexBuffer; VkDeviceMemory m_VertexBufferMemory;
	UIList<primitive_t> m_PrimitiveList;
};
