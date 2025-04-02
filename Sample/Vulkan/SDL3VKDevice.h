#pragma once
/*=================================================
* Copyright @ 2020-2025 ChivenZhang.
* All Rights Reserved.
* =====================Note=========================
*
*
* ====================History=======================
* Created by ChivenZhang at 2025/03/29 14:16:58.
*
* =================================================*/
#include <SDL3/SDL.h>
#include <vulkan/vulkan.h>
#include <SDL3/SDL_vulkan.h>
#include <OpenUI/UIDevice.h>
#include "CairoVKRender.h"
#include "../SDL3InputEnum.h"
#include "../Cairo/CairoUIPainter.h"

/// @brief
class SDL3VKDevice : public UIDevice
{
public:
	SDL3VKDevice(VkInstance instance)
		:
		m_Instance(instance)
	{
		auto window = SDL_CreateWindow("https://github.com/ChivenZhang/OpenUI.git", 1000, 600, SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);
		if (window == nullptr)
		{
			UI_ERROR("Window could not be created! SDL_Error: %s", SDL_GetError());
			SDL_Quit();
			UI_FATAL("GLEW could not be initialized!");
		}
		m_Window = window;

		VkSurfaceKHR surface = VK_NULL_HANDLE;
		if (SDL_Vulkan_CreateSurface(window, instance, nullptr, &surface) == false)
		{
			SDL_Quit();
			UI_FATAL("Could not create Vulkan surface!");
		}
		m_Surface = surface;

		size_t graphicsFamilyIndex = -1;
		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
		uint32_t physicalDeviceCount = 0;
		vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);
		UIList<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
		vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices.data());

		for (size_t i=0; i<physicalDevices.size(); ++i)
		{
			VkPhysicalDeviceProperties physicalDeviceProperties = {};
			vkGetPhysicalDeviceProperties(physicalDevices[i], &physicalDeviceProperties);
			size_t familyQueueIndex = -1;
			uint32_t queueFamilyCount = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(physicalDevices[i], &queueFamilyCount, nullptr);
			UIList<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
			vkGetPhysicalDeviceQueueFamilyProperties(physicalDevices[i], &queueFamilyCount, queueFamilies.data());
			for (size_t k=0; k<queueFamilies.size(); ++k)
			{
				if (queueFamilies[k].queueFlags & VK_QUEUE_GRAPHICS_BIT)
				{
					VkBool32 supported = 0;
					auto result = vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevices[i], k, surface, &supported);
					if (result != VK_SUCCESS) UI_FATAL("Could not get surface support!");
					if (supported && SDL_Vulkan_GetPresentationSupport(instance, physicalDevices[i], k))
					{
						if (familyQueueIndex == -1 || physicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
						{
							familyQueueIndex = k;
						}
					}
				}
			}
			if (familyQueueIndex == -1) continue;

			auto supported = false;
			uint32_t extensionPropertiesCount = 0;
			vkEnumerateDeviceExtensionProperties(physicalDevices[i], nullptr, &extensionPropertiesCount, nullptr);
			UIList<VkExtensionProperties> extensions(extensionPropertiesCount);
			vkEnumerateDeviceExtensionProperties(physicalDevices[i], nullptr, &extensionPropertiesCount, extensions.data());
			for (size_t k=0; k<extensions.size(); ++k)
			{
				if (SDL_strcmp(extensions[k].extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME) == 0) {
					supported = true;
					break;
				}
			}
			if (supported == false) continue;
			physicalDevice = physicalDevices[i];
			graphicsFamilyIndex = familyQueueIndex;
		}
		if (physicalDevice == VK_NULL_HANDLE) UI_FATAL("Could not find a suitable physical device!");
		m_PhysicalDevice = physicalDevice;
		const char* requiredExtensions[]
		{
			VK_KHR_SWAPCHAIN_EXTENSION_NAME,
			VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
#ifdef __APPLE__
	        "VK_KHR_portability_subset"
#endif
		};

		constexpr VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamic_rendering_feature
		{
			.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR,
			.dynamicRendering = VK_TRUE,
		};
		float queuePriority = 1.0f;
		VkDeviceQueueCreateInfo deviceQueueCreateInfo = {};
		deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		deviceQueueCreateInfo.queueFamilyIndex = graphicsFamilyIndex;
		deviceQueueCreateInfo.queueCount = 1;
		deviceQueueCreateInfo.pQueuePriorities = &queuePriority;
		VkDeviceCreateInfo deviceCreateInfo = {};
		deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceCreateInfo.pNext = &dynamic_rendering_feature;
		deviceCreateInfo.queueCreateInfoCount = 1;
		deviceCreateInfo.pQueueCreateInfos = &deviceQueueCreateInfo;
		deviceCreateInfo.pEnabledFeatures = nullptr;
		deviceCreateInfo.enabledExtensionCount = (uint32_t)SDL_arraysize(requiredExtensions);
		deviceCreateInfo.ppEnabledExtensionNames = requiredExtensions;
		VkDevice device = VK_NULL_HANDLE;
		auto result = vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device);
		if (result != VK_SUCCESS) UI_FATAL("Could not create logical device!");
		m_Device = device;

		VkQueue queue = VK_NULL_HANDLE;
		vkGetDeviceQueue(m_Device, graphicsFamilyIndex, 0, &queue);
		m_Queue = queue;

		VkCommandPoolCreateInfo commandPoolCreateInfo = {};
		commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT | VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
		commandPoolCreateInfo.queueFamilyIndex = graphicsFamilyIndex;
		VkCommandPool commandPool = VK_NULL_HANDLE;
		result = vkCreateCommandPool(device, &commandPoolCreateInfo, nullptr, &commandPool);
		if (result != VK_SUCCESS) UI_FATAL("Could not create command pool!");
		m_CommandPool = commandPool;

		VkCommandBufferAllocateInfo commandBufferCreateInfo = {};
		commandBufferCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferCreateInfo.commandPool = commandPool;
		commandBufferCreateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		commandBufferCreateInfo.commandBufferCount = 1;
		VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
		result = vkAllocateCommandBuffers(device, &commandBufferCreateInfo, &commandBuffer);
		if (result != VK_SUCCESS) UI_FATAL("Could not allocate command buffers!");
		m_CommandBuffer = commandBuffer;

		VkDescriptorPoolSize poolSizes[] =
		{
			{VK_DESCRIPTOR_TYPE_SAMPLER, 8},
			{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 8},
			{VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 8},
			{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 8},
			{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 8},
		};
		VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
		descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptorPoolCreateInfo.maxSets = 512;
		descriptorPoolCreateInfo.poolSizeCount = sizeof(poolSizes) / sizeof(VkDescriptorPoolSize);
		descriptorPoolCreateInfo.pPoolSizes = poolSizes;
		descriptorPoolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
		result = vkCreateDescriptorPool(device, &descriptorPoolCreateInfo, nullptr, &descriptorPool);
		if (result != VK_SUCCESS) UI_FATAL("Could not create descriptor pool!");
		m_DescriptorPool = descriptorPool;

		VkPipelineCacheCreateInfo pipelineCacheCreateInfo = {};
		pipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
		pipelineCacheCreateInfo.initialDataSize = 0;
		pipelineCacheCreateInfo.pInitialData = nullptr;
		pipelineCacheCreateInfo.flags = 0;
		VkPipelineCache pipelineCache = VK_NULL_HANDLE;
		result = vkCreatePipelineCache(device, &pipelineCacheCreateInfo, nullptr, &pipelineCache);
		if (result != VK_SUCCESS) UI_FATAL("Could not create pipeline cache!");
		m_PipelineCache = pipelineCache;

		VkSemaphoreCreateInfo semaphoreCreateInfo = {};
		semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		VkSemaphore semaphore = VK_NULL_HANDLE;
		result = vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &semaphore);
		if (result != VK_SUCCESS) UI_FATAL("Could not create semaphore!");
		m_SemaphoreImage = semaphore;
		semaphoreCreateInfo = {};
		semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		semaphore = VK_NULL_HANDLE;
		result = vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &semaphore);
		if (result != VK_SUCCESS) UI_FATAL("Could not create semaphore!");
		m_SemaphorePaint = semaphore;

		m_Swapchain = VK_NULL_HANDLE;
		recreateSwapchain();

		// Initialize OpenUI context

		int width = 0, height = 0;
		SDL_GetWindowSize(window, &width, &height);
		auto scale = SDL_GetWindowDisplayScale(window);
		UIConfig config{.DisplayScale = scale};
		auto openui = UINew<UIContext>(config);
		openui->setPainter(UINew<CairoUIPainter>(width, height));
		openui->setRender(UINew<CairoVKRender>(width, height, this));
		m_UIContext = openui;
	}

	~SDL3VKDevice() override
	{
		vkDeviceWaitIdle(m_Device);
		m_UIContext = nullptr;
		for (size_t i=0; i<m_SwapchainFences.size(); ++i) vkDestroyFence(m_Device, m_SwapchainFences[i], nullptr);
		for (size_t i=0; i<m_SwapchainViews.size(); ++i) vkDestroyImageView(m_Device, m_SwapchainViews[i], nullptr);
		m_SwapchainImages.clear();
		m_SwapchainFences.clear();
		m_SwapchainViews.clear();
		vkDestroySwapchainKHR(m_Device, m_Swapchain, nullptr); m_Swapchain = VK_NULL_HANDLE;
		vkDestroySemaphore(m_Device, m_SemaphoreImage, nullptr); m_SemaphoreImage = VK_NULL_HANDLE;
		vkDestroySemaphore(m_Device, m_SemaphorePaint, nullptr); m_SemaphorePaint = VK_NULL_HANDLE;
		vkDestroyPipelineCache(m_Device, m_PipelineCache, nullptr); m_PipelineCache = VK_NULL_HANDLE;
		vkDestroyDescriptorPool(m_Device, m_DescriptorPool, nullptr); m_DescriptorPool = VK_NULL_HANDLE;
		vkFreeCommandBuffers(m_Device, m_CommandPool, 1, &m_CommandBuffer); m_CommandBuffer = VK_NULL_HANDLE;
		vkDestroyCommandPool(m_Device, m_CommandPool, nullptr); m_CommandPool = VK_NULL_HANDLE;
		vkDestroyDevice(m_Device, nullptr); m_Device = VK_NULL_HANDLE;
		vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr); m_Surface = VK_NULL_HANDLE;
		m_Instance = VK_NULL_HANDLE;
		SDL_DestroyWindow(m_Window); m_Window = VK_NULL_HANDLE;
	}

	SDL_Window* getWindow() const
	{
		return m_Window;
	}

	UIContextRaw getContext() const override
	{
		return m_UIContext.get();
	}

	bool update() override
	{
		auto window = getWindow();
		auto openui = getContext();

		// Send events to OpenUI

		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_EVENT_QUIT:
				{
					return false;
				}
				break;
			case SDL_EVENT_KEY_DOWN:
				{
					UIKeyDownEvent event2(SDL3InputEnum::GetKeyboardEnum(event.key.key), SDL3InputEnum::GetModifierEnum(event.key.mod), event.key.scancode, event.key.key, event.key.mod, UIString(),
										event.key.repeat);
					openui->sendEvent(nullptr, &event2);
				}
				break;
			case SDL_EVENT_KEY_UP:
				{
					UIKeyUpEvent event2(SDL3InputEnum::GetKeyboardEnum(event.key.key), SDL3InputEnum::GetModifierEnum(event.key.mod), event.key.scancode, event.key.key, event.key.mod, UIString(),
										event.key.repeat);
					openui->sendEvent(nullptr, &event2);
				}
				break;
			case SDL_EVENT_TEXT_EDITING:
				{
					UITextInputEvent event2(SDL3InputEnum::GetKeyboardEnum(event.key.key), SDL3InputEnum::GetModifierEnum(event.key.mod), event.key.scancode, event.key.key, event.key.mod,
											event.edit.text, event.key.repeat, false, event.edit.start, event.edit.length);
					openui->sendEvent(nullptr, &event2);
				}
				break;
			case SDL_EVENT_TEXT_INPUT:
				{
					UITextInputEvent event2(SDL3InputEnum::GetKeyboardEnum(event.key.key), SDL3InputEnum::GetModifierEnum(event.key.mod), event.key.scancode, event.key.key, event.key.mod,
											event.edit.text, event.key.repeat, true);
					openui->sendEvent(nullptr, &event2);
				}
				break;
			case SDL_EVENT_MOUSE_MOTION:
				{
					int x, y;
					SDL_GetWindowPosition(window, &x, &y);
					UIMouseMoveEvent event2(event.motion.x, event.motion.y, x + event.motion.x, y + event.motion.y, SDL3InputEnum::GetMouseEnum(event.button.button),
											SDL3InputEnum::GetMouseEnum(event.button.button), SDL3InputEnum::GetModifierEnum(event.key.mod));
					openui->sendEvent(nullptr, &event2);
				}
				break;
			case SDL_EVENT_MOUSE_BUTTON_DOWN:
				{
					int x, y;
					SDL_GetWindowPosition(window, &x, &y);
					if (event.button.clicks == 1)
					{
						UIMouseDownEvent event2(event.motion.x, event.motion.y, x + event.motion.x, y + event.motion.y, SDL3InputEnum::GetMouseEnum(event.button.button),
												SDL3InputEnum::GetMouseEnum(event.button.button), SDL3InputEnum::GetModifierEnum(event.key.mod), event.button.clicks);
						openui->sendEvent(nullptr, &event2);
					}
					else
					{
						UIMouseDblClickEvent event2(event.motion.x, event.motion.y, x + event.motion.x, y + event.motion.y, SDL3InputEnum::GetMouseEnum(event.button.button),
													SDL3InputEnum::GetMouseEnum(event.button.button), SDL3InputEnum::GetModifierEnum(event.key.mod), event.button.clicks);
						openui->sendEvent(nullptr, &event2);
					}
				}
				break;
			case SDL_EVENT_MOUSE_BUTTON_UP:
				{
					int x, y;
					SDL_GetWindowPosition(window, &x, &y);
					UIMouseUpEvent event2(event.motion.x, event.motion.y, x + event.motion.x, y + event.motion.y, SDL3InputEnum::GetMouseEnum(event.button.button),
										SDL3InputEnum::GetMouseEnum(event.button.button), SDL3InputEnum::GetModifierEnum(event.key.mod));
					openui->sendEvent(nullptr, &event2);
				}
				break;
			case SDL_EVENT_MOUSE_WHEEL:
				{
					int x, y;
					SDL_GetWindowPosition(window, &x, &y);
					UIMouseWheelEvent event2(event.wheel.x, event.wheel.y, event.wheel.x, event.wheel.y, event.wheel.mouse_x, event.wheel.mouse_y, x + event.wheel.mouse_x, y + event.wheel.mouse_y,
											SDL3InputEnum::GetMouseEnum(event.button.button), SDL3InputEnum::GetMouseEnum(event.button.button), SDL3InputEnum::GetModifierEnum(event.key.mod));
					openui->sendEvent(nullptr, &event2);
				}
				break;
			case SDL_EVENT_WINDOW_MOUSE_ENTER:
				{
					int x, y;
					SDL_GetWindowPosition(window, &x, &y);
					UIMouseEnterEvent event2(event.motion.x, event.motion.y, x + event.motion.x, y + event.motion.y, SDL3InputEnum::GetMouseEnum(event.button.button),
											SDL3InputEnum::GetMouseEnum(event.button.button), SDL3InputEnum::GetModifierEnum(event.key.mod));
					openui->sendEvent(nullptr, &event2);
				}
				break;
			case SDL_EVENT_WINDOW_MOUSE_LEAVE:
				{
					int x, y;
					SDL_GetWindowPosition(window, &x, &y);
					UIMouseLeaveEvent event2(event.motion.x, event.motion.y, x + event.motion.x, y + event.motion.y, SDL3InputEnum::GetMouseEnum(event.button.button),
											SDL3InputEnum::GetMouseEnum(event.button.button), SDL3InputEnum::GetModifierEnum(event.key.mod));
					openui->sendEvent(nullptr, &event2);
				}
				break;
			case SDL_EVENT_WINDOW_SHOWN:
				{
					UIShowEvent event2;
					openui->sendEvent(nullptr, &event2);
				}
				break;
			case SDL_EVENT_WINDOW_HIDDEN:
				{
					UIHideEvent event2;
					openui->sendEvent(nullptr, &event2);
				}
				break;
			case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
				{
					UICloseEvent event2;
					openui->sendEvent(nullptr, &event2);
				}
				break;
			case SDL_EVENT_WINDOW_MOVED:
				{
					UIMoveEvent event2(event.window.data1, event.window.data2);
					openui->sendEvent(nullptr, &event2);
				}
				break;
			case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
				{
					recreateSwapchain();
					openui->layoutElement();
					UICast<CairoUIPainter>(openui->getPainter())->resize(event.window.data1, event.window.data2);
					UIResizeEvent event2(event.window.data1, event.window.data2);
					openui->sendEvent(nullptr, &event2);
				}
				break;
			case SDL_EVENT_WINDOW_MINIMIZED:
				break;
			case SDL_EVENT_WINDOW_MAXIMIZED:
				break;
			case SDL_EVENT_WINDOW_RESTORED:
				break;
			case SDL_EVENT_WINDOW_FOCUS_GAINED:
				{
					UIFocusEvent event2(true);
					openui->sendEvent(nullptr, &event2);
				}
				break;
			case SDL_EVENT_WINDOW_FOCUS_LOST:
				{
					UIFocusEvent event2(false);
					openui->sendEvent(nullptr, &event2);
				}
				break;
			default:
				break;
			}
		}

		// Update layout and paint

		int w, h;
		SDL_GetWindowSize(window, &w, &h);
		UIRect client{0, 0, (float)w, (float)h};
		openui->updateElement(::clock() * 0.001f, client);

		// Output frame to screen

		uint32_t frameIndex = -1;
		auto result = vkAcquireNextImageKHR(m_Device, m_Swapchain, UINT64_MAX, m_SemaphoreImage, nullptr, &frameIndex);
		if (result == VK_SUBOPTIMAL_KHR || result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			recreateSwapchain();
			return true;
		}
		if (result != VK_SUBOPTIMAL_KHR && result != VK_SUCCESS) UI_FATAL("Failed to acquire swap chain image!");
		result = vkWaitForFences(m_Device, 1, &m_SwapchainFences[frameIndex], VK_TRUE, UINT64_MAX);
		if (result != VK_SUCCESS) UI_FATAL("Failed to wait for swap chain image!");
		result = vkResetFences(m_Device, 1, &m_SwapchainFences[frameIndex]);
		if (result != VK_SUCCESS) UI_FATAL("Failed to reset swap chain image!");

		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
		result = vkBeginCommandBuffer(m_CommandBuffer, &beginInfo);
		if (result != VK_SUCCESS) UI_FATAL("Failed to begin command buffer!");

		auto pixels = UICast<CairoUIPainter>(openui->getPainter())->getPixels();
		UICast<CairoVKRender>(openui->getRender())->uploadTexture(w, h, (uint8_t*)pixels.data());

		VkRect2D renderAreaExtent = {};
		renderAreaExtent.offset.x = client.X;
		renderAreaExtent.offset.y = client.Y;
		renderAreaExtent.extent.width = client.W;
		renderAreaExtent.extent.height = client.H;

		VkRenderingInfo renderingInfo = {};
		VkRenderingAttachmentInfo colorAttachment = {};
		colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
		colorAttachment.imageView = m_SwapchainViews[frameIndex];
		colorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.clearValue.color.float32[0] = 0.0f;
		colorAttachment.clearValue.color.float32[1] = 0.0f;
		colorAttachment.clearValue.color.float32[2] = 0.0f;
		colorAttachment.clearValue.color.float32[3] = 1.0f;
		renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
		renderingInfo.renderArea = renderAreaExtent;
		renderingInfo.layerCount = 1;
		renderingInfo.colorAttachmentCount = 1;
		renderingInfo.pColorAttachments = &colorAttachment;
		vkCmdBeginRendering(m_CommandBuffer, &renderingInfo);

		openui->renderElement(client);

		vkCmdEndRendering(m_CommandBuffer);

		VkImageMemoryBarrier barrier = {};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		barrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = m_SwapchainImages[frameIndex];
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;
		vkCmdPipelineBarrier(m_CommandBuffer, VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

		result = vkEndCommandBuffer(m_CommandBuffer);
		if (result != VK_SUCCESS) UI_FATAL("Failed to end command buffer!");

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &m_SemaphoreImage;
		VkPipelineStageFlags waitDestStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		submitInfo.pWaitDstStageMask = &waitDestStageMask;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &m_CommandBuffer;
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &m_SemaphorePaint;
		result = vkQueueSubmit(m_Queue, 1, &submitInfo, m_SwapchainFences[frameIndex]);
		if (result != VK_SUCCESS) UI_FATAL("Failed to submit draw command buffer!");
		
		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &m_SemaphorePaint;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &m_Swapchain;
		presentInfo.pImageIndices = &frameIndex;
		result = vkQueuePresentKHR(m_Queue, &presentInfo);
		if (result == VK_SUBOPTIMAL_KHR || result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			recreateSwapchain();
			return true;
		}
		if (result != VK_SUCCESS) UI_FATAL("Failed to present swap chain image!");

		result = vkQueueWaitIdle(m_Queue);
		if (result != VK_SUCCESS) UI_FATAL("Failed to acquire swapchain image!");

		return true;
	}

	void recreateSwapchain()
	{
		VkSurfaceCapabilitiesKHR surfaceCapabilities = {};
		auto result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_PhysicalDevice, m_Surface, &surfaceCapabilities);
		if (result != VK_SUCCESS) UI_FATAL("vkGetPhysicalDeviceSurfaceCapabilitiesKHR failed");

		int width = 0, height = 0;
		SDL_GetWindowSizeInPixels(m_Window, &width, &height);
		auto flags = SDL_GetWindowFlags(m_Window);

		vkDeviceWaitIdle(m_Device);
		uint32_t surfaceFormatsCount = 0;
		vkGetPhysicalDeviceSurfaceFormatsKHR(m_PhysicalDevice, m_Surface, &surfaceFormatsCount, nullptr);
		UIList<VkSurfaceFormatKHR> surfaceFormats(surfaceFormatsCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(m_PhysicalDevice, m_Surface, &surfaceFormatsCount, surfaceFormats.data());

		for (size_t i=0; i<m_SwapchainFences.size(); ++i) vkDestroyFence(m_Device, m_SwapchainFences[i], nullptr);
		for (size_t i=0; i<m_SwapchainViews.size(); ++i) vkDestroyImageView(m_Device, m_SwapchainViews[i], nullptr);
		m_SwapchainImages.clear();
		m_SwapchainFences.clear();
		m_SwapchainViews.clear();

		VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
		swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		swapchainCreateInfo.surface = m_Surface;
		swapchainCreateInfo.minImageCount = 1;
		swapchainCreateInfo.imageFormat = VK_FORMAT_R8G8B8A8_UNORM;
		swapchainCreateInfo.imageColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
		swapchainCreateInfo.imageExtent.width = SDL_clamp((uint32_t)width, surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width);
		swapchainCreateInfo.imageExtent.height = SDL_clamp((uint32_t)height, surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height);
		swapchainCreateInfo.imageArrayLayers = 1;
		swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		swapchainCreateInfo.preTransform = surfaceCapabilities.currentTransform;
		if (flags & SDL_WINDOW_TRANSPARENT) swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR;
		else swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		swapchainCreateInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
		swapchainCreateInfo.clipped = VK_TRUE;
		swapchainCreateInfo.oldSwapchain = m_Swapchain;
		for (auto i = 0; i < surfaceFormatsCount; i++)
		{
			if (surfaceFormats[i].format == VK_FORMAT_R8G8B8A8_UNORM)
			{
				swapchainCreateInfo.imageFormat = surfaceFormats[i].format;
				swapchainCreateInfo.imageColorSpace = surfaceFormats[i].colorSpace;
				break;
			}
		}
		VkSwapchainKHR swapchain = VK_NULL_HANDLE;
		result = vkCreateSwapchainKHR(m_Device, &swapchainCreateInfo, nullptr, &swapchain);
		if (result != VK_SUCCESS) UI_FATAL("vkCreateSwapchainKHR failed");
		m_Swapchain = swapchain;
		if (swapchainCreateInfo.oldSwapchain) vkDestroySwapchainKHR(m_Device, swapchainCreateInfo.oldSwapchain, nullptr);

		uint32_t swapchainImageCount = 0;
		vkGetSwapchainImagesKHR(m_Device, m_Swapchain, &swapchainImageCount, nullptr);
		UIList<VkImage> swapchainImages(swapchainImageCount);
		vkGetSwapchainImagesKHR(m_Device, swapchain, &swapchainImageCount, swapchainImages.data());
		m_SwapchainImages = std::move(swapchainImages);

		m_SwapchainViews.resize(m_SwapchainImages.size());
		for (size_t i=0; i<m_SwapchainViews.size(); ++i)
		{
			VkImageViewCreateInfo viewInfo = {};
			viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			viewInfo.image = m_SwapchainImages[i];
			viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			viewInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
			viewInfo.components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
			viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			viewInfo.subresourceRange.baseMipLevel = 0;
			viewInfo.subresourceRange.levelCount = 1;
			viewInfo.subresourceRange.baseArrayLayer = 0;
			viewInfo.subresourceRange.layerCount = 1;
			result = vkCreateImageView(m_Device, &viewInfo, nullptr, &m_SwapchainViews[i]);
			if (result != VK_SUCCESS) UI_FATAL("vkCreateImageView failed");
		}

		m_SwapchainFences.resize(m_SwapchainImages.size());
		for (size_t i=0; i<m_SwapchainFences.size(); ++i)
		{
			VkFenceCreateInfo fenceCreateInfo = {};
			fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
			result = vkCreateFence(m_Device, &fenceCreateInfo, nullptr, &m_SwapchainFences[i]);
			if (result != VK_SUCCESS) UI_FATAL("vkCreateFence failed");
		}
		m_SwapchainFormat = swapchainCreateInfo.imageFormat;

		result = vkQueueWaitIdle(m_Queue);
		if (result != VK_SUCCESS) UI_FATAL("Failed to acquire swapchain image!");
	}

protected:
	SDL_Window* m_Window;
	VkInstance m_Instance;
	VkSurfaceKHR m_Surface;
	VkPhysicalDevice m_PhysicalDevice;
	VkDevice m_Device;
	VkQueue m_Queue;
	VkCommandPool m_CommandPool;
	VkCommandBuffer m_CommandBuffer;
	VkDescriptorPool m_DescriptorPool;
	VkPipelineCache m_PipelineCache;
	VkSemaphore m_SemaphoreImage, m_SemaphorePaint;
	VkSwapchainKHR m_Swapchain;
	VkFormat m_SwapchainFormat;
	UIList<VkImage> m_SwapchainImages;
	UIList<VkImageView> m_SwapchainViews;
	UIList<VkFence> m_SwapchainFences;
	UIContextRef m_UIContext;

	friend class CairoVKRender;
};