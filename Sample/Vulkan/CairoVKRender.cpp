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
#include <SDL3/SDL_timer.h>
#include "CairoVKRender.h"
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

CairoVKRender::CairoVKRender(uint32_t width, uint32_t height, VkDevice device, VkPipelineCache pipelineCache, VkDescriptorPool descriptorPool, VkCommandBuffer commandBuffer)
	:
	m_Device(device),
	m_DescriptorPool(descriptorPool),
	m_CommandBuffer(commandBuffer)
{
	// 创建描述符布局
	
	{
		VkDescriptorSetLayoutBinding binding = {};
		binding.binding = 0;
		binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		binding.descriptorCount = 16;
		binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		binding.pImmutableSamplers = nullptr;

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

	// 创建着色器模块

	{
		vkDestroyShaderModule(device, vertShaderModule, nullptr);
		vkDestroyShaderModule(device, fragShaderModule, nullptr);
	}
	
	// 创建管线

	{
		VkPipelineShaderStageCreateInfo shaderStages[2] = {};

		VkShaderModule vertShaderModule = CairoUIPainter::createShaderModule(device, vsource);
		VkPipelineShaderStageCreateInfo& vertShaderStageInfo = shaderStages[0];
		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertShaderStageInfo.module = vertShaderModule;
		vertShaderStageInfo.pName = "main";

		VkShaderModule fragShaderModule = CairoUIPainter::createShaderModule(device, fsource);
		VkPipelineShaderStageCreateInfo& fragShaderStageInfo = shaderStages[1];
		fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageInfo.module = fragShaderModule;
		fragShaderStageInfo.pName = "main";

		UIList<VkVertexInputBindingDescription> vertexInputBindings;
		UIList<VkVertexInputAttributeDescription> vertexInputAttributes;

		auto& vertexInputBindingDescription = vertexInputBindings.emplace_back();
		vertexInputBindingDescription.binding = 0;
		vertexInputBindingDescription.stride = sizeof(primitive_t);
		vertexInputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		{
			auto& vertexInputAttributeDescription = vertexInputAttributes.emplace_back();
			vertexInputAttributeDescription.binding = 0;
			vertexInputAttributeDescription.location = 0;
			vertexInputAttributeDescription.format = VK_FORMAT_R32G32_SFLOAT;
			vertexInputAttributeDescription.offset = 0;
		}
		{
			auto& vertexInputAttributeDescription = vertexInputAttributes.emplace_back();
			vertexInputAttributeDescription.binding = 0;
			vertexInputAttributeDescription.location = 1;
			vertexInputAttributeDescription.format = VK_FORMAT_R32_UINT;
			vertexInputAttributeDescription.offset = sizeof(float) * 2;
		}

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

		VkPipelineViewportStateCreateInfo viewportState = {};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		VkViewport viewportRect = {};
		viewportRect.x = 0.0f;
		viewportRect.y = 0.0f;
		viewportRect.width = 1.0f;
		viewportRect.height = 1.0f;
		viewportRect.minDepth = 0.0f;
		viewportRect.maxDepth = 1.0f;
		VkRect2D scissorRect = {};
		scissorRect.offset.x = 0;
		scissorRect.offset.y = 0;
		scissorRect.extent.width = 1;
		scissorRect.extent.height = 1;
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.pViewports = &viewportRect;
		viewportState.scissorCount = 1;
		viewportState.pScissors = &scissorRect;

		VkPipelineRasterizationStateCreateInfo rasterizer = {};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		rasterizer.depthBiasEnable = VK_FALSE;
		rasterizer.depthBiasConstantFactor = 0.0f;
		rasterizer.depthBiasClamp = 0.0f;
		rasterizer.depthBiasSlopeFactor = 0.0f;
		rasterizer.lineWidth = 1.0f;

		VkPipelineMultisampleStateCreateInfo multisampling = {};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisampling.minSampleShading = 0.0f;
		multisampling.pSampleMask = nullptr;
		multisampling.alphaToCoverageEnable = VK_FALSE;
		multisampling.alphaToOneEnable = VK_FALSE;

		VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
		colorBlendAttachment.blendEnable = VK_FALSE;
		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

		VkPipelineColorBlendStateCreateInfo colorBlending = {};
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = VK_LOGIC_OP_CLEAR;
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;

		VkDynamicState dynamicStates[] = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR,
		};
		VkPipelineDynamicStateCreateInfo dynamicState = {};
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount = 2;
		dynamicState.pDynamicStates = dynamicStates;

		VkFormat colorFormat = VK_FORMAT_R8G8B8A8_UNORM;
		VkPipelineRenderingCreateInfo renderingCreateInfo = {};
		renderingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
		renderingCreateInfo.colorAttachmentCount = 1;
		renderingCreateInfo.pColorAttachmentFormats = &colorFormat;
		renderingCreateInfo.depthAttachmentFormat = VK_FORMAT_UNDEFINED;
		renderingCreateInfo.stencilAttachmentFormat = VK_FORMAT_UNDEFINED;
		pipelineInfo.pNext = &renderingCreateInfo;

		VkGraphicsPipelineCreateInfo pipelineInfo = {};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = shaderStages;
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDepthStencilState = nullptr;
		pipelineInfo.pDynamicState = &dynamicState;
		pipelineInfo.layout = m_PipelineLayout;

		auto result = vkCreateGraphicsPipelines(device, pipelineCache, 1, &pipelineInfo, nullptr, &m_Pipeline);
		if(result != VK_SUCCESS) UI_FATAL("Failed to create graphics pipeline! %d", result);
	
		vkDestroyShaderModule(device, vertShaderModule, nullptr);
		vkDestroyShaderModule(device, fragShaderModule, nullptr);
	}

	// 创建临时纹理视图

	{
		VkImageCreateInfo imageInfo = {};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = width;
		imageInfo.extent.height = height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;
		imageInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		auto result = vkCreateImage(device, &imageInfo, nullptr, &m_TempImage);
		if(result != VK_SUCCESS) UI_FATAL("Failed to create image! %d", result);
		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(device, m_TempImage, &memRequirements);
		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		uint32_t memoryTypeIndex = -1;
		VkPhysicalDeviceMemoryProperties memProperties = {};
		vkGetPhysicalDeviceMemoryProperties(device, &memProperties);
		for(uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
		{
			if((memRequirements.memoryTypeBits & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT))
			{
				memoryTypeIndex = i;
				break;
			}
		}
		if(memoryTypeIndex == -1) UI_FATAL("Failed to find suitable memory type!");
		allocInfo.memoryTypeIndex = memoryTypeIndex;
		result = vkAllocateMemory(device, &allocInfo, nullptr, &m_TempImageMemory);
		if(result != VK_SUCCESS) UI_FATAL("Failed to allocate image memory! %d", result);
		result = vkBindImageMemory(device, m_TempImage, m_TempImageMemory, 0);
		if(result != VK_SUCCESS) UI_FATAL("Failed to bind image memory! %d", result);

		VkImageViewCreateInfo viewInfo = {};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = m_TempImage;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
		viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;
		result = vkCreateImageView(device, &viewInfo, nullptr, &m_TempImageView);
		if(result != VK_SUCCESS) UI_FATAL("Failed to create image view! %d", result);
	}
	
	// 创建描述符

	{
		VkDescriptorSetAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = descriptorPool;
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = &m_DescriptorSetLayout;
		auto result = vkAllocateDescriptorSets(device, &allocInfo, &m_DescriptorSet);
		if(result != VK_SUCCESS) UI_FATAL("Failed to allocate descriptor set! %d", result);

		VkDescriptorImageInfo imageInfo = {};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = m_TempImageView;

		VkWriteDescriptorSet descriptorWrite = {};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = m_DescriptorSet;
		descriptorWrite.dstBinding = 0;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorCount = 16;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
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

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(device, m_VertexBuffer, &memRequirements);
		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		uint32_t memoryTypeIndex = -1;
		VkPhysicalDeviceMemoryProperties memProperties = {};
		vkGetPhysicalDeviceMemoryProperties(device, &memProperties);
		for(uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
		{
			if((memRequirements.memoryTypeBits & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT))
			{
				memoryTypeIndex = i;
				break;
			}
		}
		if(memoryTypeIndex == -1) UI_FATAL("Failed to find suitable memory type!");
		allocInfo.memoryTypeIndex = memoryTypeIndex;
		result = vkAllocateMemory(device, &allocInfo, nullptr, &m_VertexBufferMemory);
		if(result != VK_SUCCESS) UI_FATAL("Failed to allocate vertex buffer memory! %d", result);
		result = vkBindBufferMemory(device, m_VertexBuffer, m_VertexBufferMemory, 0);
		if(result != VK_SUCCESS) UI_FATAL("Failed to bind vertex buffer memory! %d", result);
	}

	// 创建颜色附件

	{
		VkImageCreateInfo imageInfo = {};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = width;
		imageInfo.extent.height = height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;
		imageInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		auto result = vkCreateImage(device, &imageInfo, nullptr, &m_ColorImage);
		if(result != VK_SUCCESS) UI_FATAL("Failed to create image! %d", result);
		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(device, m_ColorImage, &memRequirements);
		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		uint32_t memoryTypeIndex = -1;
		VkPhysicalDeviceMemoryProperties memProperties = {};
		vkGetPhysicalDeviceMemoryProperties(device, &memProperties);
		for(uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
		{
			if((memRequirements.memoryTypeBits & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT))
			{
				memoryTypeIndex = i;
				break;
			}
		}
		if(memoryTypeIndex == -1) UI_FATAL("Failed to find suitable memory type!");
		allocInfo.memoryTypeIndex = memoryTypeIndex;
		result = vkAllocateMemory(device, &allocInfo, nullptr, &m_ColorImageMemory);
		if(result != VK_SUCCESS) UI_FATAL("Failed to allocate image memory! %d", result);
		result = vkBindImageMemory(device, m_ColorpImage, m_ColorImageMemory, 0);
		if(result != VK_SUCCESS) UI_FATAL("Failed to bind image memory! %d", result);

		VkImageViewCreateInfo viewInfo = {};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = m_ColorImage;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
		viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;
		result = vkCreateImageView(device, &viewInfo, nullptr, &m_ColorImageView);
		if(result != VK_SUCCESS) UI_FATAL("Failed to create image view! %d", result);
	}
}

CairoVKRender::~CairoVKRender()
{
	vkDestroyImageView(m_Device, m_ColorImageView, nullptr); m_ColorImageView = nullptr;
	vkDestroyImage(m_Device, m_ColorImage, nullptr); m_ColorImage = nullptr;
	vkFreeMemory(m_Device, m_ColorImageMemory, nullptr); m_ColorImageMemory = nullptr;

	vkDestroyImageView(m_Device, m_TempImageView, nullptr); m_TempImageView = nullptr;
	vkDestroyImage(m_Device, m_TempImage, nullptr);	m_TempImage = nullptr;
	vkFreeMemory(m_Device, m_TempImageMemory, nullptr);	m_TempImageMemory = nullptr;

	vkDestroyBuffer(m_Device, m_VertexBuffer, nullptr); m_VertexBuffer = nullptr;
	vkFreeMemory(m_Device, m_VertexBufferMemory, nullptr); m_VertexBufferMemory = nullptr;

	vkFreeDescriptorSets(m_Device, m_DescriptorPool, 1, &m_DescriptorSet); m_DescriptorSet = nullptr;
	vkDestroyDescriptorSetLayout(m_Device, m_DescriptorSetLayout, nullptr); m_DescriptorSetLayout = nullptr;
	vkDestroyPipelineLayout(m_Device, m_PipelineLayout, nullptr); m_PipelineLayout = nullptr;
	vkDestroyPipeline(m_Device, m_Pipeline, nullptr); m_Pipeline = nullptr;
}

void CairoVKRender::render(UIRect client, UIListView<UIPrimitive> data)
{
	auto cmdBuffer = m_CommandBuffer;

	// 渲染几何图元到临时纹理

	for (size_t i = 0; i < data.size(); ++i)
	{
		auto primitive = data[i].Primitive;
		auto painter = UICast<CairoUIPainter>(data[i].Painter);
		if (primitive.empty() || painter == nullptr) continue;

		auto width = painter->getWidth();
		auto height = painter->getHeight();
		auto pixels = painter->getPixels().data();
		// glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, pixels);

		m_PrimitiveList.clear();
		for (size_t k = 0; k < primitive.size(); ++k)
		{
			m_PrimitiveList.push_back({primitive[k].P0.X, primitive[k].P0.Y, 0});
			m_PrimitiveList.push_back({primitive[k].P1.X, primitive[k].P1.Y, 0});
			m_PrimitiveList.push_back({primitive[k].P2.X, primitive[k].P2.Y, 0});
		}
	}

	VkRect2D renderAreaExtent = {};
	renderAreaExtent.offset.x = client.x;
	renderAreaExtent.offset.y = client.y;
	renderAreaExtent.extent.width = client.width;
	renderAreaExtent.extent.height = client.height;

	VkRenderingAttachmentInfo colorAttachment = {};
	colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
	colorAttachment.imageView = m_ColorImageView;
	colorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.clearValue.color.float32[0] = 0.0f;
	colorAttachment.clearValue.color.float32[1] = 0.0f;
	colorAttachment.clearValue.color.float32[2] = 0.0f;
	colorAttachment.clearValue.color.float32[3] = 1.0f;

	VkRenderingInfo renderingInfo = {};
	renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
	renderingInfo.renderArea = renderAreaExtent;
	renderingInfo.layerCount = 1;
	renderingInfo.colorAttachmentCount = 1;
	renderingInfo.pColorAttachments = &colorAttachment;
	vkCmdBeginRendering(cmdBuffer, &renderingInfo);

	VkViewport viewport = {};
	viewport.x = renderAreaExtent.offset.x;
	viewport.y = renderAreaExtent.offset.y;
	viewport.width = renderAreaExtent.extent.width;
	viewport.height = -renderAreaExtent.extent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(cmdBuffer, 0, 1, &viewport);

	vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline);
	
	vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, 0, 1, &m_DescriptorSet, 0, nullptr);
	
	vkCmdBindVertexBuffers(cmdBuffer, 0, 1, &m_VertexBuffer, nullptr);
	
	vkCmdDraw(cmdBuffer, m_PrimitiveList.size(), 1, 0, 0);
	
	vkCmdEndRendering(cmdBuffer);
}

void CairoVKRender::setOutputView(VkImageView value)
{
	m_ColorImageView = value;
}