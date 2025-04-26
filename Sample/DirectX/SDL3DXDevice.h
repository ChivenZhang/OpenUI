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
#ifdef OPENUI_ENABLE_DIRECTX
#include <SDL3/SDL.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
using Microsoft::WRL::ComPtr;
#include <OpenUI/UIContext.h>
#include "CairoDXRender.h"
#include "../SDL3InputEnum.h"
#include "../Cairo/CairoUIPainter.h"

/// @brief
class SDL3DXDevice
{
public:
	explicit SDL3DXDevice(ComPtr<IDXGIFactory4> instance)
		:
		m_Instance(instance)
	{
		auto window = SDL_CreateWindow("https://github.com/ChivenZhang/OpenUI.git", 1000, 600, SDL_WINDOW_RESIZABLE);
		if (window == nullptr)
		{
			SDL_Quit();
			UI_FATAL("Failed to create window");
		}
		m_Window = window;

		int width = 0, height = 0;
		SDL_GetWindowSize(window, &width, &height);
		auto hwnd = (HWND)SDL_GetPointerProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr);

		ComPtr<IDXGIFactory6> factory6;
		if (instance.As(&factory6) != S_OK) UI_FATAL("Could not query DXGIFactory6!");
		ComPtr<IDXGIAdapter> adapter;
		auto result = factory6->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter));
		if (result != S_OK) UI_FATAL("EnumAdapterByGpuPreference failed");
		m_PhysicalDevice = adapter;

		result = D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&m_Device));
		if (result != S_OK) UI_FATAL("D3D12CreateDevice failed");

		D3D12_COMMAND_QUEUE_DESC commandQueueDesc = {};
		commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		commandQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		result = m_Device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&m_Queue));
		if (result != S_OK) UI_FATAL("Could not create command queue!");

		DXGI_SWAP_CHAIN_DESC1 swapchainDesc = {};
		swapchainDesc.BufferCount = 3;
		swapchainDesc.Width = width;
		swapchainDesc.Height = height;
		swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapchainDesc.SampleDesc.Count = 1;
		swapchainDesc.SampleDesc.Quality = 0;
		swapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		result = factory6->CreateSwapChainForHwnd(m_Queue.Get(), hwnd, &swapchainDesc, nullptr, nullptr, &m_Swapchain);
		if (result != S_OK) UI_FATAL("Could not create swapchain!");

		result = factory6->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER);
		if (result != S_OK) UI_FATAL("Could not make swapchain!");

		D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
		heapDesc.NumDescriptors = 3;
		heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		result = m_Device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_DescriptorPool1));
		if (result != S_OK) UI_FATAL("Could not create descriptor heap!");

		heapDesc = {};
		heapDesc.NumDescriptors = 3;
		heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		result = m_Device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_DescriptorPool2));
		if (result != S_OK) UI_FATAL("Could not create descriptor heap!");

		result = m_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_CommandPool));
		if (result != S_OK) UI_FATAL("Could not create command allocator!");

		result = m_Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_CommandPool.Get(), nullptr, IID_PPV_ARGS(&m_CommandBuffer));
		if (result != S_OK) UI_FATAL("Could not create command list!");

		// Initialize OpenUI context

		auto scale = SDL_GetWindowDisplayScale(window);
		UIConfig config{.DisplayScale = scale};
		auto openui = UINew<UIContext>(config);
		openui->setPainter(UINew<CairoUIPainter>(width, height));
		openui->setRender(UINew<CairoDXRender>(width, height, this));
		m_UIContext = openui;
	}

	~SDL3DXDevice()
	{
		m_UIContext = nullptr;

		// for (size_t i=0; i<m_SwapchainFences.size(); ++i) vkDestroyFence(m_Device, m_SwapchainFences[i], nullptr);
		// for (size_t i=0; i<m_SwapchainViews.size(); ++i) vkDestroyImageView(m_Device, m_SwapchainViews[i], nullptr);
		// m_SwapchainImages.clear();
		// m_SwapchainFences.clear();
		// m_SwapchainViews.clear();

		m_DescriptorPool1 = nullptr;
		m_DescriptorPool2 = nullptr;
		m_CommandBuffer = nullptr;
		m_CommandPool = nullptr;
		m_Swapchain = nullptr;
		m_Device = nullptr;
		m_Instance = nullptr;
		SDL_DestroyWindow(m_Window); m_Window = nullptr;
	}

	SDL_Window* getWindow() const
	{
		return m_Window;
	}

	UIContextRaw getContext() const
	{
		return m_UIContext.get();
	}

	bool update()
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

		// auto result = vkWaitForFences(m_Device, 1, &m_SwapchainFences[m_CurrentFrame], VK_TRUE, UINT64_MAX);
		// if (result != VK_SUCCESS) UI_FATAL("Failed to wait for swap chain image!");
		// result = vkResetFences(m_Device, 1, &m_SwapchainFences[m_CurrentFrame]);
		// if (result != VK_SUCCESS) UI_FATAL("Failed to reset swap chain image!");
		//
		// uint32_t imageIndex = -1;
		// result = vkAcquireNextImageKHR(m_Device, m_Swapchain, UINT64_MAX, m_SemaphoreImages[m_CurrentFrame], nullptr, &imageIndex);
		// if (result == VK_SUBOPTIMAL_KHR || result == VK_ERROR_OUT_OF_DATE_KHR)
		// {
		// 	recreateSwapchain();
		// 	return true;
		// }
		// if (result != VK_SUBOPTIMAL_KHR && result != VK_SUCCESS) UI_FATAL("Failed to acquire swap chain image!");
		//
		// auto commandBuffer = m_CommandBuffers[m_CurrentFrame];
		// result = vkResetCommandBuffer(commandBuffer, 0);
		// if (result != VK_SUCCESS) UI_FATAL("Failed to reset command buffer!");
		//
		// VkCommandBufferBeginInfo beginInfo = {};
		// beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		// beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
		// result = vkBeginCommandBuffer(commandBuffer, &beginInfo);
		// if (result != VK_SUCCESS) UI_FATAL("Failed to begin command buffer!");
		//
		// auto pixels = UICast<CairoUIPainter>(openui->getPainter())->getPixels();
		// UICast<CairoVKRender>(openui->getRender())->uploadTexture(w, h, (uint8_t*)pixels.data());
		//
		// VkRect2D renderAreaExtent = {};
		// renderAreaExtent.offset.x = client.X;
		// renderAreaExtent.offset.y = client.Y;
		// renderAreaExtent.extent.width = client.W;
		// renderAreaExtent.extent.height = client.H;
		//
		// VkRenderingInfo renderingInfo = {};
		// VkRenderingAttachmentInfo colorAttachment = {};
		// colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
		// colorAttachment.imageView = m_SwapchainViews[imageIndex];
		// colorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		// colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		// colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		// colorAttachment.clearValue.color.float32[0] = 0.0f;
		// colorAttachment.clearValue.color.float32[1] = 0.0f;
		// colorAttachment.clearValue.color.float32[2] = 0.0f;
		// colorAttachment.clearValue.color.float32[3] = 1.0f;
		// renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
		// renderingInfo.renderArea = renderAreaExtent;
		// renderingInfo.layerCount = 1;
		// renderingInfo.colorAttachmentCount = 1;
		// renderingInfo.pColorAttachments = &colorAttachment;
		// vkCmdBeginRendering(commandBuffer, &renderingInfo);
		//
		// openui->renderElement(client);
		//
		// vkCmdEndRendering(commandBuffer);
		//
		// VkImageMemoryBarrier barrier = {};
		// barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		// barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		// barrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		// barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		// barrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		// barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		// barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		// barrier.image = m_SwapchainImages[imageIndex];
		// barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		// barrier.subresourceRange.baseMipLevel = 0;
		// barrier.subresourceRange.levelCount = 1;
		// barrier.subresourceRange.baseArrayLayer = 0;
		// barrier.subresourceRange.layerCount = 1;
		// vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);
		//
		// result = vkEndCommandBuffer(commandBuffer);
		// if (result != VK_SUCCESS) UI_FATAL("Failed to end command buffer!");
		//
		// VkSubmitInfo submitInfo = {};
		// submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		// submitInfo.waitSemaphoreCount = 1;
		// submitInfo.pWaitSemaphores = &m_SemaphoreImages[m_CurrentFrame];
		// VkPipelineStageFlags waitDestStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		// submitInfo.pWaitDstStageMask = &waitDestStageMask;
		// submitInfo.commandBufferCount = 1;
		// submitInfo.pCommandBuffers = &commandBuffer;
		// submitInfo.signalSemaphoreCount = 1;
		// submitInfo.pSignalSemaphores = &m_SemaphorePaints[m_CurrentFrame];
		// result = vkQueueSubmit(m_Queue, 1, &submitInfo, m_SwapchainFences[m_CurrentFrame]);
		// if (result != VK_SUCCESS) UI_FATAL("Failed to submit draw command buffer!");
		//
		// VkPresentInfoKHR presentInfo = {};
		// presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		// presentInfo.waitSemaphoreCount = 1;
		// presentInfo.pWaitSemaphores = &m_SemaphorePaints[m_CurrentFrame];
		// presentInfo.swapchainCount = 1;
		// presentInfo.pSwapchains = &m_Swapchain;
		// presentInfo.pImageIndices = &imageIndex;
		// result = vkQueuePresentKHR(m_Queue, &presentInfo);
		// if (result == VK_SUBOPTIMAL_KHR || result == VK_ERROR_OUT_OF_DATE_KHR)
		// {
		// 	recreateSwapchain();
		// 	return true;
		// }
		// if (result != VK_SUCCESS) UI_FATAL("Failed to present swap chain image!");
		//
		// m_CurrentFrame = (m_CurrentFrame + 1) % (uint32_t)m_SwapchainImages.size();
		return true;
	}

	void recreateSwapchain()
	{
		// VkSurfaceCapabilitiesKHR surfaceCapabilities = {};
		// auto result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_PhysicalDevice, m_Surface, &surfaceCapabilities);
		// if (result != VK_SUCCESS) UI_FATAL("vkGetPhysicalDeviceSurfaceCapabilitiesKHR failed");
		//
		// int width = 0, height = 0;
		// SDL_GetWindowSizeInPixels(m_Window, &width, &height);
		// auto flags = SDL_GetWindowFlags(m_Window);
		//
		// vkDeviceWaitIdle(m_Device);
		// uint32_t surfaceFormatsCount = 0;
		// vkGetPhysicalDeviceSurfaceFormatsKHR(m_PhysicalDevice, m_Surface, &surfaceFormatsCount, nullptr);
		// UIList<VkSurfaceFormatKHR> surfaceFormats(surfaceFormatsCount);
		// vkGetPhysicalDeviceSurfaceFormatsKHR(m_PhysicalDevice, m_Surface, &surfaceFormatsCount, surfaceFormats.data());
		//
		// for (size_t i=0; i<m_SwapchainFences.size(); ++i) vkDestroyFence(m_Device, m_SwapchainFences[i], nullptr);
		// for (size_t i=0; i<m_SemaphoreImages.size(); ++i) vkDestroySemaphore(m_Device, m_SemaphoreImages[i], nullptr);
		// for (size_t i=0; i<m_SemaphorePaints.size(); ++i) vkDestroySemaphore(m_Device, m_SemaphorePaints[i], nullptr);
		// for (size_t i=0; i<m_SwapchainViews.size(); ++i) vkDestroyImageView(m_Device, m_SwapchainViews[i], nullptr);
		// for (size_t i=0; i<m_CommandBuffers.size(); ++i) vkFreeCommandBuffers(m_Device, m_CommandPool, 1, &m_CommandBuffers[i]);
		// m_SwapchainImages.clear();
		// m_SemaphoreImages.clear();
		// m_SemaphorePaints.clear();
		// m_SwapchainFences.clear();
		// m_SwapchainViews.clear();
		// m_CommandBuffers.clear();
		//
		// VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
		// swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		// swapchainCreateInfo.surface = m_Surface;
		// swapchainCreateInfo.minImageCount = 3;
		// swapchainCreateInfo.imageFormat = VK_FORMAT_R8G8B8A8_UNORM;
		// swapchainCreateInfo.imageColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
		// swapchainCreateInfo.imageExtent.width = SDL_clamp((uint32_t)width, surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width);
		// swapchainCreateInfo.imageExtent.height = SDL_clamp((uint32_t)height, surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height);
		// swapchainCreateInfo.imageArrayLayers = 1;
		// swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		// swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		// swapchainCreateInfo.preTransform = surfaceCapabilities.currentTransform;
		// if (flags & SDL_WINDOW_TRANSPARENT) swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR;
		// else swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		// swapchainCreateInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
		// swapchainCreateInfo.clipped = VK_TRUE;
		// swapchainCreateInfo.oldSwapchain = m_Swapchain;
		// for (auto i = 0; i < surfaceFormatsCount; i++)
		// {
		// 	if (surfaceFormats[i].format == VK_FORMAT_R8G8B8A8_UNORM)
		// 	{
		// 		swapchainCreateInfo.imageFormat = surfaceFormats[i].format;
		// 		swapchainCreateInfo.imageColorSpace = surfaceFormats[i].colorSpace;
		// 		break;
		// 	}
		// }
		// VkSwapchainKHR swapchain = VK_NULL_HANDLE;
		// result = vkCreateSwapchainKHR(m_Device, &swapchainCreateInfo, nullptr, &swapchain);
		// if (result != VK_SUCCESS) UI_FATAL("vkCreateSwapchainKHR failed");
		// m_Swapchain = swapchain;
		// if (swapchainCreateInfo.oldSwapchain) vkDestroySwapchainKHR(m_Device, swapchainCreateInfo.oldSwapchain, nullptr);
		//
		// uint32_t swapchainImageCount = 0;
		// vkGetSwapchainImagesKHR(m_Device, m_Swapchain, &swapchainImageCount, nullptr);
		// UIList<VkImage> swapchainImages(swapchainImageCount);
		// vkGetSwapchainImagesKHR(m_Device, swapchain, &swapchainImageCount, swapchainImages.data());
		// m_SwapchainImages = std::move(swapchainImages);
		// m_SwapchainFormat = swapchainCreateInfo.imageFormat;
		//
		// m_SwapchainViews.resize(m_SwapchainImages.size());
		// for (size_t i=0; i<m_SwapchainViews.size(); ++i)
		// {
		// 	VkImageViewCreateInfo viewInfo = {};
		// 	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		// 	viewInfo.image = m_SwapchainImages[i];
		// 	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		// 	viewInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
		// 	viewInfo.components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
		// 	viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		// 	viewInfo.subresourceRange.baseMipLevel = 0;
		// 	viewInfo.subresourceRange.levelCount = 1;
		// 	viewInfo.subresourceRange.baseArrayLayer = 0;
		// 	viewInfo.subresourceRange.layerCount = 1;
		// 	result = vkCreateImageView(m_Device, &viewInfo, nullptr, &m_SwapchainViews[i]);
		// 	if (result != VK_SUCCESS) UI_FATAL("vkCreateImageView failed");
		// }
		//
		// m_SwapchainFences.resize(m_SwapchainImages.size());
		// for (size_t i=0; i<m_SwapchainFences.size(); ++i)
		// {
		// 	VkFenceCreateInfo fenceCreateInfo = {};
		// 	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		// 	fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
		// 	result = vkCreateFence(m_Device, &fenceCreateInfo, nullptr, &m_SwapchainFences[i]);
		// 	if (result != VK_SUCCESS) UI_FATAL("vkCreateFence failed");
		// }
		//
		// m_SemaphoreImages.resize(m_SwapchainImages.size());
		// for (size_t i=0; i<m_SemaphoreImages.size(); ++i)
		// {
		// 	VkSemaphoreCreateInfo semaphoreCreateInfo = {};
		// 	semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		// 	result = vkCreateSemaphore(m_Device, &semaphoreCreateInfo, nullptr, &m_SemaphoreImages[i]);
		// 	if (result != VK_SUCCESS) UI_FATAL("Could not create semaphore!");
		// }
		//
		// m_SemaphorePaints.resize(m_SwapchainImages.size());
		// for (size_t i=0; i<m_SemaphorePaints.size(); ++i)
		// {
		// 	VkSemaphoreCreateInfo semaphoreCreateInfo = {};
		// 	semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		// 	result = vkCreateSemaphore(m_Device, &semaphoreCreateInfo, nullptr, &m_SemaphorePaints[i]);
		// 	if (result != VK_SUCCESS) UI_FATAL("Could not create semaphore!");
		// }
		//
		// m_CommandBuffers.resize(m_SwapchainImages.size());
		// for (size_t i=0; i<m_CommandBuffers.size(); ++i)
		// {
		// 	VkCommandBufferAllocateInfo commandBufferCreateInfo = {};
		// 	commandBufferCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		// 	commandBufferCreateInfo.commandPool = m_CommandPool;
		// 	commandBufferCreateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		// 	commandBufferCreateInfo.commandBufferCount = 1;
		// 	result = vkAllocateCommandBuffers(m_Device, &commandBufferCreateInfo, &m_CommandBuffers[i]);
		// 	if (result != VK_SUCCESS) UI_FATAL("Could not allocate command buffers!");
		// }
		//
		// result = vkQueueWaitIdle(m_Queue);
		// if (result != VK_SUCCESS) UI_FATAL("Failed to acquire swapchain image!");
	}

protected:
	SDL_Window* m_Window;
	ComPtr<IDXGIFactory4> m_Instance;
	ComPtr<IDXGISwapChain1> m_Swapchain;
	ComPtr<IDXGIAdapter1> m_PhysicalDevice;
	ComPtr<ID3D12Device> m_Device;
	ComPtr<ID3D12CommandQueue> m_Queue;
	ComPtr<ID3D12CommandAllocator> m_CommandPool;
	ComPtr<ID3D12GraphicsCommandList> m_CommandBuffer;
	ComPtr<ID3D12DescriptorHeap> m_DescriptorPool1;	// for render target
	ComPtr<ID3D12DescriptorHeap> m_DescriptorPool2;	// for shader resource
	uint32_t m_CurrentFrame = 0;
	UIList<ComPtr<ID3D12Resource>> m_SwapchainImages;
	UIList<ComPtr<ID3D12Resource>> m_SwapchainViews;
	ComPtr<ID3D12Fence> m_SwapchainFence;
	UIContextRef m_UIContext;

	friend class CairoDXRender;
};
#endif