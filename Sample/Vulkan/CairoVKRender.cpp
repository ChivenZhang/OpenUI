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
#ifdef OPENUI_ENABLE_VULKAN
#include <glslang/Public/resource_limits_c.h>
#include <glslang/Include/glslang_c_interface.h>
#include "SDL3VKDevice.h"
#include "CairoVKRender.h"
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

CairoVKRender::CairoVKRender(uint32_t width, uint32_t height, UIRaw<SDL3VKDevice> _device)
	:
	m_Device(_device),
	m_Width(width),
	m_Height(height)
{
	auto device = _device->m_Device;
	auto pipelineCache = _device->m_PipelineCache;
	auto descriptorPool = _device->m_DescriptorPool;
	auto physicalDevice = _device->m_PhysicalDevice;

	// 创建描述符集布局
	
	{
		VkDescriptorSetLayoutBinding binding = {};
		binding.binding = 0;
		binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		binding.descriptorCount = 16;
		binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		VkDescriptorSetLayoutCreateInfo layoutInfo = {};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = 1;
		layoutInfo.pBindings = &binding;
		auto result = vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &m_DescriptorSetLayout);
		if(result != VK_SUCCESS) UI_FATAL("Failed to create descriptor set layout! %d", result);
	}

	// 创建图形管线布局

	{
		VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 1;
		pipelineLayoutInfo.pSetLayouts = &m_DescriptorSetLayout;
		auto result = vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &m_PipelineLayout);
		if(result != VK_SUCCESS) UI_FATAL("Failed to create pipeline layout! %d", result);
	}
	
	// 创建管线

	{
		VkPipelineShaderStageCreateInfo shaderStages[2] = {};

		VkShaderModule vertShaderModule = createShaderModule(device, GLSLANG_STAGE_VERTEX, vsource);
		VkPipelineShaderStageCreateInfo& vertShaderStageInfo = shaderStages[0];
		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertShaderStageInfo.module = vertShaderModule;
		vertShaderStageInfo.pName = "main";

		VkShaderModule fragShaderModule = createShaderModule(device, GLSLANG_STAGE_FRAGMENT, fsource);
		VkPipelineShaderStageCreateInfo& fragShaderStageInfo = shaderStages[1];
		fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageInfo.module = fragShaderModule;
		fragShaderStageInfo.pName = "main";

		UIList<VkVertexInputBindingDescription> vertexInputBindings
		{
			{0, (uint32_t)sizeof(primitive_t), VK_VERTEX_INPUT_RATE_VERTEX},
		};
		UIList<VkVertexInputAttributeDescription> vertexInputAttributes
		{
			{0, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(primitive_t, X)},
			{1, 0, VK_FORMAT_R32_UINT, offsetof(primitive_t, Index)},
		};
		VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = vertexInputBindings.size();
		vertexInputInfo.pVertexBindingDescriptions = vertexInputBindings.data();
		vertexInputInfo.vertexAttributeDescriptionCount = vertexInputAttributes.size();
		vertexInputInfo.pVertexAttributeDescriptions = vertexInputAttributes.data();

		VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssembly.primitiveRestartEnable = VK_FALSE;

		VkPipelineRasterizationStateCreateInfo rasterizer = {};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizer.depthBiasEnable = VK_FALSE;
		rasterizer.lineWidth = 1.0f;

		VkPipelineColorBlendStateCreateInfo colorBlending = {};
		VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;

		VkPipelineViewportStateCreateInfo viewportState = {};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		VkViewport viewportRect = {};
		viewportRect.x = 0.0f;
		viewportRect.y = 0.0f;
		viewportRect.width = width;
		viewportRect.height = height;
		viewportRect.minDepth = 0.0f;
		viewportRect.maxDepth = 1.0f;
		VkRect2D scissorRect = {};
		scissorRect.offset.x = 0;
		scissorRect.offset.y = 0;
		scissorRect.extent.width = width;
		scissorRect.extent.height = height;
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.pViewports = &viewportRect;
		viewportState.scissorCount = 1;
		viewportState.pScissors = &scissorRect;

		VkPipelineMultisampleStateCreateInfo multisampling = {};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

		VkPipelineDynamicStateCreateInfo dynamicState = {};
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		UIList<VkDynamicState> dynamicStates
		{
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR,
			VK_DYNAMIC_STATE_CULL_MODE,
			VK_DYNAMIC_STATE_FRONT_FACE,
			VK_DYNAMIC_STATE_PRIMITIVE_TOPOLOGY
		};
		dynamicState.dynamicStateCount = dynamicStates.size();
		dynamicState.pDynamicStates = dynamicStates.data();

		VkPipelineRenderingCreateInfo renderingCreateInfo = {};
		renderingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
		renderingCreateInfo.colorAttachmentCount = 1;
		renderingCreateInfo.pColorAttachmentFormats = &m_Device->m_SwapchainFormat;
		renderingCreateInfo.depthAttachmentFormat = VK_FORMAT_UNDEFINED;
		renderingCreateInfo.stencilAttachmentFormat = VK_FORMAT_UNDEFINED;

		VkGraphicsPipelineCreateInfo pipelineInfo = {};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.pNext = &renderingCreateInfo;
		pipelineInfo.layout = m_PipelineLayout;
		pipelineInfo.stageCount = sizeof(shaderStages) / sizeof(VkPipelineShaderStageCreateInfo);
		pipelineInfo.pStages = shaderStages;
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDynamicState = &dynamicState;
		auto result = vkCreateGraphicsPipelines(device, pipelineCache, 1, &pipelineInfo, nullptr, &m_Pipeline);
		if(result != VK_SUCCESS) UI_FATAL("Failed to create graphics pipeline! %d", result);

		vkDestroyShaderModule(device, vertShaderModule, nullptr);
		vkDestroyShaderModule(device, fragShaderModule, nullptr);
	}

	// 创建纹理视图

	{
		VkImageCreateInfo imageInfo = {};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = width;
		imageInfo.extent.height = height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;
		imageInfo.format = VK_FORMAT_B8G8R8A8_UNORM;
		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		auto result = vkCreateImage(device, &imageInfo, nullptr, &m_Texture);
		if(result != VK_SUCCESS) UI_FATAL("Failed to create image! %d", result);

		VkMemoryRequirements memoryRequirements;
		vkGetImageMemoryRequirements(device, m_Texture, &memoryRequirements);
		VkPhysicalDeviceMemoryProperties physicalMemoryProperties;
		vkGetPhysicalDeviceMemoryProperties(physicalDevice, &physicalMemoryProperties);
		uint32_t memoryTypeIndex = -1;
		VkPhysicalDeviceMemoryProperties deviceMemoryProperties = {};
		vkGetPhysicalDeviceMemoryProperties(physicalDevice, &deviceMemoryProperties);
		for(uint32_t i = 0; i < deviceMemoryProperties.memoryTypeCount; ++i)
		{
			// Resource must support this memory type
			if ((memoryRequirements.memoryTypeBits & (1 << i)) == 0) continue;
			// Mappable resource must be host visible
			if ((physicalMemoryProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) == 0) continue;
			// Mappable must also be host coherent.
			if ((physicalMemoryProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == 0) continue;
			// Found the first candidate memory type
			if (memoryTypeIndex == -1)
			{
				memoryTypeIndex = i;
				continue;
			}
			// All things equal favor the memory in the biggest heap
			auto bestTypeHeapSize = physicalMemoryProperties.memoryHeaps[physicalMemoryProperties.memoryTypes[memoryTypeIndex].heapIndex].size;
			auto candidateHeapSize = physicalMemoryProperties.memoryHeaps[physicalMemoryProperties.memoryTypes[i].heapIndex].size;
			if (bestTypeHeapSize < candidateHeapSize)
			{
				memoryTypeIndex = i;
				continue;
			}
		}
		if(memoryTypeIndex == -1) UI_FATAL("Failed to find suitable memory type!");
		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memoryRequirements.size;
		allocInfo.memoryTypeIndex = memoryTypeIndex;
		result = vkAllocateMemory(device, &allocInfo, nullptr, &m_TextureMemory);
		if(result != VK_SUCCESS) UI_FATAL("Failed to allocate image memory! %d", result);
		result = vkBindImageMemory(device, m_Texture, m_TextureMemory, 0);
		if(result != VK_SUCCESS) UI_FATAL("Failed to bind image memory! %d", result);

		VkImageViewCreateInfo viewInfo = {};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = m_Texture;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = VK_FORMAT_B8G8R8A8_UNORM;
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;
		result = vkCreateImageView(device, &viewInfo, nullptr, &m_TextureView);
		if(result != VK_SUCCESS) UI_FATAL("Failed to create image view! %d", result);

		VkSamplerCreateInfo samplerInfo = {};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.anisotropyEnable = VK_FALSE;
		samplerInfo.maxAnisotropy = 16.0f;
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		result = vkCreateSampler(device, &samplerInfo, nullptr, &m_Sampler);
		if (result != VK_SUCCESS) UI_FATAL("Failed to create texture sampler! %d", result);
	}
	
	// 创建描述符集

	{
		VkDescriptorSetAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = descriptorPool;
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = &m_DescriptorSetLayout;
		auto result = vkAllocateDescriptorSets(device, &allocInfo, &m_DescriptorSet);
		if(result != VK_SUCCESS) UI_FATAL("Failed to allocate descriptor set! %d", result);

		VkDescriptorImageInfo imageInfo = {};
		imageInfo.sampler = m_Sampler;
		imageInfo.imageView = m_TextureView;
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		VkWriteDescriptorSet descriptorWrite = {};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = m_DescriptorSet;
		descriptorWrite.dstBinding = 0;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pImageInfo = &imageInfo;
		vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
	}

	// 创建顶点缓冲

	{
		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = sizeof(primitive_t) * 1024; // TODO: Dynamic size
		bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		auto result = vkCreateBuffer(device, &bufferInfo, nullptr, &m_VertexBuffer);
		if(result != VK_SUCCESS) UI_FATAL("Failed to create vertex buffer! %d", result);

		VkMemoryRequirements memoryRequirements;
		vkGetBufferMemoryRequirements(device, m_VertexBuffer, &memoryRequirements);
		VkPhysicalDeviceMemoryProperties physicalMemoryProperties;
		vkGetPhysicalDeviceMemoryProperties(physicalDevice, &physicalMemoryProperties);
		uint32_t memoryTypeIndex = -1;
		VkPhysicalDeviceMemoryProperties deviceMemoryProperties = {};
		vkGetPhysicalDeviceMemoryProperties(physicalDevice, &deviceMemoryProperties);
		for(uint32_t i = 0; i < deviceMemoryProperties.memoryTypeCount; ++i)
		{
			// Resource must support this memory type
			if ((memoryRequirements.memoryTypeBits & (1 << i)) == 0) continue;
			// Mappable resource must be host visible
			if ((physicalMemoryProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) == 0) continue;
			// Mappable must also be host coherent.
			if ((physicalMemoryProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == 0) continue;
			// Found the first candidate memory type
			if (memoryTypeIndex == -1)
			{
				memoryTypeIndex = i;
				continue;
			}
			// All things equal favor the memory in the biggest heap
			auto bestTypeHeapSize = physicalMemoryProperties.memoryHeaps[physicalMemoryProperties.memoryTypes[memoryTypeIndex].heapIndex].size;
			auto candidateHeapSize = physicalMemoryProperties.memoryHeaps[physicalMemoryProperties.memoryTypes[i].heapIndex].size;
			if (bestTypeHeapSize < candidateHeapSize)
			{
				memoryTypeIndex = i;
				continue;
			}
		}
		if(memoryTypeIndex == -1) UI_FATAL("Failed to find suitable memory type!");
		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memoryRequirements.size;
		allocInfo.memoryTypeIndex = memoryTypeIndex;
		result = vkAllocateMemory(device, &allocInfo, nullptr, &m_VertexBufferMemory);
		if(result != VK_SUCCESS) UI_FATAL("Failed to allocate vertex buffer memory! %d", result);
		result = vkBindBufferMemory(device, m_VertexBuffer, m_VertexBufferMemory, 0);
		if(result != VK_SUCCESS) UI_FATAL("Failed to bind vertex buffer memory! %d", result);
	}

	// 创建上传缓冲区

	{
		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = 1920 * 1920 * 4;
		bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		auto result = vkCreateBuffer(device, &bufferInfo, nullptr, &m_StageBuffer);
		if(result != VK_SUCCESS) UI_FATAL("Failed to create vertex buffer! %d", result);

		VkMemoryRequirements memoryRequirements;
		vkGetBufferMemoryRequirements(device, m_StageBuffer, &memoryRequirements);
		VkPhysicalDeviceMemoryProperties physicalMemoryProperties;
		vkGetPhysicalDeviceMemoryProperties(physicalDevice, &physicalMemoryProperties);
		uint32_t memoryTypeIndex = -1;
		VkPhysicalDeviceMemoryProperties deviceMemoryProperties = {};
		vkGetPhysicalDeviceMemoryProperties(physicalDevice, &deviceMemoryProperties);
		for(uint32_t i = 0; i < deviceMemoryProperties.memoryTypeCount; ++i)
		{
			// Resource must support this memory type
			if ((memoryRequirements.memoryTypeBits & (1 << i)) == 0) continue;
			// Mappable resource must be host visible
			if ((physicalMemoryProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) == 0) continue;
			// Mappable must also be host coherent.
			if ((physicalMemoryProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == 0) continue;
			// Found the first candidate memory type
			if (memoryTypeIndex == -1)
			{
				memoryTypeIndex = i;
				continue;
			}
			// All things equal favor the memory in the biggest heap
			auto bestTypeHeapSize = physicalMemoryProperties.memoryHeaps[physicalMemoryProperties.memoryTypes[memoryTypeIndex].heapIndex].size;
			auto candidateHeapSize = physicalMemoryProperties.memoryHeaps[physicalMemoryProperties.memoryTypes[i].heapIndex].size;
			if (bestTypeHeapSize < candidateHeapSize)
			{
				memoryTypeIndex = i;
				continue;
			}
		}
		if(memoryTypeIndex == -1) UI_FATAL("Failed to find suitable memory type!");
		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memoryRequirements.size;
		allocInfo.memoryTypeIndex = memoryTypeIndex;
		result = vkAllocateMemory(device, &allocInfo, nullptr, &m_StageBufferMemory);
		if(result != VK_SUCCESS) UI_FATAL("Failed to allocate vertex buffer memory! %d", result);
		result = vkBindBufferMemory(device, m_StageBuffer, m_StageBufferMemory, 0);
		if(result != VK_SUCCESS) UI_FATAL("Failed to bind vertex buffer memory! %d", result);
	}
}

CairoVKRender::~CairoVKRender()
{
	auto device = m_Device->m_Device;
	auto pipelineCache = m_Device->m_PipelineCache;
	auto descriptorPool = m_Device->m_DescriptorPool;
	auto physicalDevice = m_Device->m_PhysicalDevice;

	vkDestroySampler(device, m_Sampler, nullptr); m_Sampler = VK_NULL_HANDLE;
	vkDestroyImageView(device, m_TextureView, nullptr); m_TextureView = VK_NULL_HANDLE;
	vkDestroyImage(device, m_Texture, nullptr);	m_Texture = VK_NULL_HANDLE;
	vkFreeMemory(device, m_TextureMemory, nullptr);	m_TextureMemory = VK_NULL_HANDLE;

	vkDestroyBuffer(device, m_StageBuffer, nullptr); m_StageBuffer = VK_NULL_HANDLE;
	vkFreeMemory(device, m_StageBufferMemory, nullptr); m_StageBufferMemory = VK_NULL_HANDLE;

	vkDestroyBuffer(device, m_VertexBuffer, nullptr); m_VertexBuffer = VK_NULL_HANDLE;
	vkFreeMemory(device, m_VertexBufferMemory, nullptr); m_VertexBufferMemory = VK_NULL_HANDLE;

	vkFreeDescriptorSets(device, descriptorPool, 1, &m_DescriptorSet); m_DescriptorSet = VK_NULL_HANDLE;
	vkDestroyDescriptorSetLayout(device, m_DescriptorSetLayout, nullptr); m_DescriptorSetLayout = VK_NULL_HANDLE;
	vkDestroyPipelineLayout(device, m_PipelineLayout, nullptr); m_PipelineLayout = VK_NULL_HANDLE;
	vkDestroyPipeline(device, m_Pipeline, nullptr); m_Pipeline = VK_NULL_HANDLE;
}

void CairoVKRender::render(UIRect client, UIListView<UIPrimitive> data)
{
	auto device = m_Device->m_Device;
	auto queue = m_Device->m_Queue;
	auto physicalDevice = m_Device->m_PhysicalDevice;
	auto vertexBuffer = m_VertexBuffer;
	auto vertexMemory = m_VertexBufferMemory;
	auto descriptorSet = m_DescriptorSet;
	auto pipeline = m_Pipeline;
	auto pipelineLayout = m_PipelineLayout;
	auto commandBuffer = m_Device->m_CommandBuffers[m_Device->m_CurrentFrame];
	for (size_t i = 0; i < data.size(); ++i)
	{
		auto primitive = data[i].Primitive;
		auto painter = UICast<CairoUIPainter>(data[i].Painter);
		if (primitive.empty() || painter == nullptr) continue;

		auto width = painter->getWidth();
		auto height = painter->getHeight();

		if(m_Width < width || m_Height < height)
		{
			UI_ERROR("CairoVKRender: Too large image size %d %d", width, height);
			return;
		}

		m_PrimitiveList.clear();
		for (size_t k = 0; k < primitive.size(); ++k)
		{
			m_PrimitiveList.push_back({primitive[k].P0.X, primitive[k].P0.Y, 0});
			m_PrimitiveList.push_back({primitive[k].P1.X, primitive[k].P1.Y, 0});
			m_PrimitiveList.push_back({primitive[k].P2.X, primitive[k].P2.Y, 0});
		}

		uint8_t* memory = nullptr;
		auto result = vkMapMemory(device, vertexMemory, 0, m_PrimitiveList.size() * sizeof(primitive_t), 0, (void**)&memory);
		if (result != VK_SUCCESS) UI_FATAL("Failed to map vertex buffer! %d", result);
		if (memory) ::memcpy(memory, m_PrimitiveList.data(), m_PrimitiveList.size() * sizeof(primitive_t));
		vkUnmapMemory(device, vertexMemory);

		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);

		VkDeviceSize offsets = 0;
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertexBuffer, &offsets);

		VkViewport vp
		{
			.width = (float)width,
			.height = (float)height,
			.minDepth = 0.0f,
			.maxDepth = 1.0f
		};
		vkCmdSetViewport(commandBuffer, 0, 1, &vp);

		VkRect2D scissor
		{
			.extent = {
				.width = (uint32_t)width,
				.height = (uint32_t)height,
			}
		};
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

		vkCmdSetCullMode(commandBuffer, VK_CULL_MODE_NONE);

		vkCmdSetFrontFace(commandBuffer, VK_FRONT_FACE_CLOCKWISE);

		vkCmdSetPrimitiveTopology(commandBuffer, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);

		vkCmdDraw(commandBuffer, m_PrimitiveList.size(), 1, 0, 0);

		break;
	}
}

void CairoVKRender::uploadTexture(int32_t width, int32_t height, uint8_t* pixels)
{
	auto device = m_Device->m_Device;
	auto queue = m_Device->m_Queue;
	auto physicalDevice = m_Device->m_PhysicalDevice;
	auto vertexBuffer = m_VertexBuffer;
	auto vertexMemory = m_VertexBufferMemory;
	auto descriptorSet = m_DescriptorSet;
	auto pipeline = m_Pipeline;
	auto pipelineLayout = m_PipelineLayout;
	auto commandBuffer = m_Device->m_CommandBuffers[m_Device->m_CurrentFrame];

	if (width != m_Width || height != m_Height)
	{
		auto result = vkQueueWaitIdle(queue);
		if (result != VK_SUCCESS) UI_FATAL("Failed to wait on queue! %d", result);

		vkDestroyImageView(device, m_TextureView, nullptr); m_TextureView = VK_NULL_HANDLE;
		vkDestroyImage(device, m_Texture, nullptr);	m_Texture = VK_NULL_HANDLE;
		vkFreeMemory(device, m_TextureMemory, nullptr);	m_TextureMemory = VK_NULL_HANDLE;
		vkDestroyBuffer(device, m_StageBuffer, nullptr); m_StageBuffer = VK_NULL_HANDLE;
		vkFreeMemory(device, m_StageBufferMemory, nullptr); m_StageBufferMemory = VK_NULL_HANDLE;

		{
			VkImageCreateInfo imageInfo = {};
			imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			imageInfo.imageType = VK_IMAGE_TYPE_2D;
			imageInfo.extent.width = m_Width = width;
			imageInfo.extent.height = m_Height = height;
			imageInfo.extent.depth = 1;
			imageInfo.mipLevels = 1;
			imageInfo.arrayLayers = 1;
			imageInfo.format = VK_FORMAT_B8G8R8A8_UNORM;
			imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
			imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			imageInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
			imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
			imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			result = vkCreateImage(device, &imageInfo, nullptr, &m_Texture);
			if(result != VK_SUCCESS) UI_FATAL("Failed to create image! %d", result);

			VkMemoryRequirements memoryRequirements;
			vkGetImageMemoryRequirements(device, m_Texture, &memoryRequirements);
			VkPhysicalDeviceMemoryProperties physicalMemoryProperties;
			vkGetPhysicalDeviceMemoryProperties(physicalDevice, &physicalMemoryProperties);
			uint32_t memoryTypeIndex = -1;
			VkPhysicalDeviceMemoryProperties deviceMemoryProperties = {};
			vkGetPhysicalDeviceMemoryProperties(physicalDevice, &deviceMemoryProperties);
			for(uint32_t i = 0; i < deviceMemoryProperties.memoryTypeCount; ++i)
			{
				// Resource must support this memory type
				if ((memoryRequirements.memoryTypeBits & (1 << i)) == 0) continue;
				// Mappable resource must be host visible
				if ((physicalMemoryProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) == 0) continue;
				// Mappable must also be host coherent.
				if ((physicalMemoryProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == 0) continue;
				// Found the first candidate memory type
				if (memoryTypeIndex == -1)
				{
					memoryTypeIndex = i;
					continue;
				}
				// All things equal favor the memory in the biggest heap
				auto bestTypeHeapSize = physicalMemoryProperties.memoryHeaps[physicalMemoryProperties.memoryTypes[memoryTypeIndex].heapIndex].size;
				auto candidateHeapSize = physicalMemoryProperties.memoryHeaps[physicalMemoryProperties.memoryTypes[i].heapIndex].size;
				if (bestTypeHeapSize < candidateHeapSize)
				{
					memoryTypeIndex = i;
					continue;
				}
			}
			if(memoryTypeIndex == -1) UI_FATAL("Failed to find suitable memory type!");
			VkMemoryAllocateInfo allocInfo = {};
			allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			allocInfo.allocationSize = memoryRequirements.size;
			allocInfo.memoryTypeIndex = memoryTypeIndex;
			result = vkAllocateMemory(device, &allocInfo, nullptr, &m_TextureMemory);
			if(result != VK_SUCCESS) UI_FATAL("Failed to allocate image memory! %d", result);
			result = vkBindImageMemory(device, m_Texture, m_TextureMemory, 0);
			if(result != VK_SUCCESS) UI_FATAL("Failed to bind image memory! %d", result);
		}

		{
			VkImageViewCreateInfo viewInfo = {};
			viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			viewInfo.image = m_Texture;
			viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			viewInfo.format = VK_FORMAT_B8G8R8A8_UNORM;
			viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			viewInfo.subresourceRange.baseMipLevel = 0;
			viewInfo.subresourceRange.levelCount = 1;
			viewInfo.subresourceRange.baseArrayLayer = 0;
			viewInfo.subresourceRange.layerCount = 1;
			result = vkCreateImageView(device, &viewInfo, nullptr, &m_TextureView);
			if(result != VK_SUCCESS) UI_FATAL("Failed to create image view! %d", result);
		}

		{
			VkDescriptorImageInfo imageInfo = {};
			imageInfo.sampler = m_Sampler;
			imageInfo.imageView = m_TextureView;
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			VkWriteDescriptorSet descriptorWrite = {};
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = m_DescriptorSet;
			descriptorWrite.dstBinding = 0;
			descriptorWrite.dstArrayElement = 0;
			descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.pImageInfo = &imageInfo;
			vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
		}

		{
			VkBufferCreateInfo bufferInfo = {};
			bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			bufferInfo.size = 1920 * 1920 * 4;
			bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
			bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			result = vkCreateBuffer(device, &bufferInfo, nullptr, &m_StageBuffer);
			if(result != VK_SUCCESS) UI_FATAL("Failed to create vertex buffer! %d", result);

			VkMemoryRequirements memoryRequirements;
			vkGetBufferMemoryRequirements(device, m_StageBuffer, &memoryRequirements);
			VkPhysicalDeviceMemoryProperties physicalMemoryProperties;
			vkGetPhysicalDeviceMemoryProperties(physicalDevice, &physicalMemoryProperties);
			uint32_t memoryTypeIndex = -1;
			VkPhysicalDeviceMemoryProperties deviceMemoryProperties = {};
			vkGetPhysicalDeviceMemoryProperties(physicalDevice, &deviceMemoryProperties);
			for(uint32_t i = 0; i < deviceMemoryProperties.memoryTypeCount; ++i)
			{
				// Resource must support this memory type
				if ((memoryRequirements.memoryTypeBits & (1 << i)) == 0) continue;
				// Mappable resource must be host visible
				if ((physicalMemoryProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) == 0) continue;
				// Mappable must also be host coherent.
				if ((physicalMemoryProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == 0) continue;
				// Found the first candidate memory type
				if (memoryTypeIndex == -1)
				{
					memoryTypeIndex = i;
					continue;
				}
				// All things equal favor the memory in the biggest heap
				auto bestTypeHeapSize = physicalMemoryProperties.memoryHeaps[physicalMemoryProperties.memoryTypes[memoryTypeIndex].heapIndex].size;
				auto candidateHeapSize = physicalMemoryProperties.memoryHeaps[physicalMemoryProperties.memoryTypes[i].heapIndex].size;
				if (bestTypeHeapSize < candidateHeapSize)
				{
					memoryTypeIndex = i;
					continue;
				}
			}
			if(memoryTypeIndex == -1) UI_FATAL("Failed to find suitable memory type!");
			VkMemoryAllocateInfo allocInfo = {};
			allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			allocInfo.allocationSize = memoryRequirements.size;
			allocInfo.memoryTypeIndex = memoryTypeIndex;
			result = vkAllocateMemory(device, &allocInfo, nullptr, &m_StageBufferMemory);
			if(result != VK_SUCCESS) UI_FATAL("Failed to allocate vertex buffer memory! %d", result);
			result = vkBindBufferMemory(device, m_StageBuffer, m_StageBufferMemory, 0);
			if(result != VK_SUCCESS) UI_FATAL("Failed to bind vertex buffer memory! %d", result);
		}
	}

	uint8_t* buffer = nullptr;
	auto result = vkMapMemory(device, m_StageBufferMemory, 0, width * height * 4, 0, (void**)&buffer);
	if (result != VK_SUCCESS) UI_FATAL("Failed to map vertex buffer! %d", result);
	if (buffer) ::memcpy(buffer, pixels, width * height * 4);
	vkUnmapMemory(device, m_StageBufferMemory);

	VkImageMemoryBarrier imageBarrier = {};
	imageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	imageBarrier.image = m_Texture;
	imageBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imageBarrier.subresourceRange.baseMipLevel = 0;
	imageBarrier.subresourceRange.levelCount = 1;
	imageBarrier.subresourceRange.baseArrayLayer = 0;
	imageBarrier.subresourceRange.layerCount = 1;
	imageBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	imageBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
	imageBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	imageBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageBarrier.image = m_Texture;
	vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageBarrier);

	VkBufferImageCopy region{};
	region.bufferOffset = 0;
	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.layerCount = 1;
	region.imageExtent = { (uint32_t)width, (uint32_t)height, 1 };
	vkCmdCopyBufferToImage(commandBuffer, m_StageBuffer, m_Texture, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

	imageBarrier = {};
	imageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	imageBarrier.image = m_Texture;
	imageBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imageBarrier.subresourceRange.baseMipLevel = 0;
	imageBarrier.subresourceRange.levelCount = 1;
	imageBarrier.subresourceRange.baseArrayLayer = 0;
	imageBarrier.subresourceRange.layerCount = 1;
	imageBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	imageBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imageBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	imageBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	imageBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageBarrier.image = m_Texture;
	vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageBarrier);
}

VkShaderModule CairoVKRender::createShaderModule(VkDevice device,  int32_t stage, const char* source)
{
	glslang_input_t input = {};
	input.language = GLSLANG_SOURCE_GLSL;
	input.stage = (glslang_stage_t)stage;
	input.client = GLSLANG_CLIENT_VULKAN;
	input.client_version = GLSLANG_TARGET_VULKAN_1_3;
	input.target_language = GLSLANG_TARGET_SPV;
	input.target_language_version = GLSLANG_TARGET_SPV_1_3;
	input.code = source;
	input.default_version = 100;
	input.default_profile = GLSLANG_NO_PROFILE;
	input.force_default_version_and_profile = false;
	input.forward_compatible = false;
	input.messages = GLSLANG_MSG_DEFAULT_BIT;
	input.resource = glslang_default_resource();
	auto shader = glslang_shader_create(&input);
	if (!glslang_shader_preprocess(shader, &input))
	{
		UI_ERROR("%s", glslang_shader_get_info_log(shader));
		glslang_shader_delete(shader);
		UI_FATAL("failed to preprocess shader");
	}
	if (!glslang_shader_parse(shader, &input))
	{
		UI_ERROR("%s", glslang_shader_get_info_log(shader));
		glslang_shader_delete(shader);
		UI_FATAL("failed to parse shader");
	}
	auto program = glslang_program_create();
	glslang_program_add_shader(program, shader);
	if (!glslang_program_link(program, GLSLANG_MSG_SPV_RULES_BIT | GLSLANG_MSG_VULKAN_RULES_BIT))
	{
		glslang_program_delete(program);
		glslang_shader_delete(shader);
		UI_FATAL("cannot link program: %s", glslang_shader_get_info_log(shader));
	}
	glslang_program_SPIRV_generate(program, input.stage);
	UIList<uint32_t> binary;
	binary.resize(glslang_program_SPIRV_get_size(program));
	glslang_program_SPIRV_get(program, binary.data());
	glslang_program_delete(program);
	glslang_shader_delete(shader);

	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = binary.size() * sizeof(uint32_t);
	createInfo.pCode = binary.data();
	VkShaderModule shaderModule = VK_NULL_HANDLE;
	auto result = vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule);
	if (result != VK_SUCCESS) UI_FATAL("cannot create shader module");
	return shaderModule;
}
#endif