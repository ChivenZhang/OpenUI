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
#include <directx/d3d12.h>
#include <directx/d3dx12.h>
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
		auto window = SDL_CreateWindow("https://github.com/ChivenZhang/OpenUI.git", 1000, 600, SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN);
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

		auto result = factory6->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&m_PhysicalDevice));
		if (result != S_OK) UI_FATAL("EnumAdapterByGpuPreference failed");

		result = D3D12CreateDevice(m_PhysicalDevice.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&m_Device));
		if (result != S_OK) UI_FATAL("D3D12CreateDevice failed");

		result = m_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_SwapchainFence));
		if (result != S_OK) UI_FATAL("CreateFence failed");

		if (instance->GetCreationFlags() & DXGI_CREATE_FACTORY_DEBUG)
		{
			ComPtr<ID3D12InfoQueue1> infoQueue;
			result = m_Device->QueryInterface(IID_PPV_ARGS(&infoQueue));
			if (result != S_OK) UI_FATAL("QueryInterface failed");
			result = infoQueue->RegisterMessageCallback([](D3D12_MESSAGE_CATEGORY category, D3D12_MESSAGE_SEVERITY severity, D3D12_MESSAGE_ID id, const char* message, void *context)
			{
				if (severity == D3D12_MESSAGE_SEVERITY_WARNING) UI_WARN("%s", message);
				else if (severity == D3D12_MESSAGE_SEVERITY_ERROR) UI_ERROR("%s", message);
				else UI_INFO("%s", message);
			}, D3D12_MESSAGE_CALLBACK_FLAG_NONE, this, &m_MessageCookie);
			if (result != S_OK) UI_FATAL("RegisterMessageCallback failed");
		}

		m_SwapchainEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		if (m_SwapchainEvent == nullptr) UI_FATAL("CreateEvent failed");

		D3D12_COMMAND_QUEUE_DESC commandQueueDesc = {};
		commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		commandQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		result = m_Device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&m_Queue));
		if (result != S_OK) UI_FATAL("Could not create command queue!");

		result = m_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_CommandPool));
		if (result != S_OK) UI_FATAL("Could not create command allocator!");

		result = m_Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_CommandPool.Get(), nullptr, IID_PPV_ARGS(&m_CommandBuffer));
		if (result != S_OK) UI_FATAL("Could not create command list!");

		DXGI_SWAP_CHAIN_DESC1 swapchainDesc = {};
		swapchainDesc.BufferCount = 3;
		swapchainDesc.Width = width;
		swapchainDesc.Height = height;
		swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapchainDesc.SampleDesc.Count = 1;
		swapchainDesc.SampleDesc.Quality = 0;
		swapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		ComPtr<IDXGISwapChain1> swapchain1;
		result = factory6->CreateSwapChainForHwnd(m_Queue.Get(), hwnd, &swapchainDesc, nullptr, nullptr, &swapchain1);
		if (result != S_OK) UI_FATAL("Could not create swapchain!");
		result = swapchain1.As(&m_Swapchain);
		if (result != S_OK) UI_FATAL("Could not create swapchain!");

		result = factory6->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER);
		if (result != S_OK) UI_FATAL("Could not make swapchain!");

		recreateSwapchain();

		// Initialize OpenUI context

		auto scale = SDL_GetWindowDisplayScale(window);
		UIConfig config{.DisplayScale = scale};
		auto openui = UINew<UIContext>(config);
		openui->setPainter(UINew<CairoUIPainter>(width, height));
		openui->setRender(UINew<CairoDXRender>(width, height, this));
		m_UIContext = openui;

		SDL_ShowWindow(window);
	}

	~SDL3DXDevice()
	{
		m_CurrentFrame += 1;
		auto result = m_Queue->Signal(m_SwapchainFence.Get(), m_CurrentFrame);
		if (result != S_OK) UI_FATAL("Failed to signal swap chain fence!");
		if (m_SwapchainFence->GetCompletedValue() < m_CurrentFrame)
		{
			m_SwapchainFence->SetEventOnCompletion(m_CurrentFrame, m_SwapchainEvent);
			WaitForSingleObject(m_SwapchainEvent, INFINITE);
		}

		if (m_Instance->GetCreationFlags() & DXGI_CREATE_FACTORY_DEBUG)
		{
			ComPtr<ID3D12InfoQueue1> infoQueue;
			result = m_Device->QueryInterface(IID_PPV_ARGS(&infoQueue));
			if (result != S_OK) UI_FATAL("QueryInterface failed");
			result = infoQueue->UnregisterMessageCallback(m_MessageCookie);
			if (result != S_OK) UI_FATAL("UnregisterMessageCallback failed");
		}

		m_UIContext = nullptr;
		m_SwapchainImages.clear();
		m_SwapchainFence = nullptr;
		m_DescriptorPool = nullptr;
		m_CommandBuffer = nullptr;
		m_CommandPool = nullptr;
		m_Swapchain = nullptr;
		m_Device = nullptr;
		m_Instance = nullptr;
		SDL_DestroyWindow(m_Window); m_Window = nullptr;
		CloseHandle(m_SwapchainEvent); m_SwapchainEvent = nullptr;
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

		auto imageIndex = m_Swapchain->GetCurrentBackBufferIndex();

		auto pixels = UICast<CairoUIPainter>(openui->getPainter())->getPixels();
		UICast<CairoDXRender>(openui->getRender())->uploadTexture(w, h, (uint8_t*)pixels.data());

		auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_SwapchainImages[imageIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		m_CommandBuffer->ResourceBarrier(1, &barrier);

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_DescriptorPool->GetCPUDescriptorHandleForHeapStart(), imageIndex, m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));

		m_CommandBuffer->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

		float clearColor[] = {0.3f, 0.3f, 0.8f, 1.0f};
		m_CommandBuffer->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

		openui->renderElement(client);

		barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_SwapchainImages[imageIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		m_CommandBuffer->ResourceBarrier(1, &barrier);

		auto result = m_CommandBuffer->Close();
		if (result != S_OK) UI_FATAL("Failed to end command buffer!");

		ID3D12CommandList* commandLists[] = { m_CommandBuffer.Get() };
		m_Queue->ExecuteCommandLists(1, commandLists);

		result = m_Swapchain->Present(1, 0);
		if (result == DXGI_ERROR_DEVICE_REMOVED || result == DXGI_ERROR_DEVICE_RESET)
		{
			recreateSwapchain();
			return true;
		}
		m_CurrentFrame += 1;
		result = m_Queue->Signal(m_SwapchainFence.Get(), m_CurrentFrame);
		if (result != S_OK) UI_FATAL("Failed to signal swap chain fence!");
		if (m_SwapchainFence->GetCompletedValue() < m_CurrentFrame)
		{
			m_SwapchainFence->SetEventOnCompletion(m_CurrentFrame, m_SwapchainEvent);
			WaitForSingleObject(m_SwapchainEvent, INFINITE);
		}

		result = m_CommandPool->Reset();
		if (result != S_OK) UI_FATAL("Failed to reset command pool!");

		result = m_CommandBuffer->Reset(m_CommandPool.Get(), nullptr);
		if (result != S_OK) UI_FATAL("Failed to reset command buffer!");
		return true;
	}

	void recreateSwapchain()
	{
		int width = 0, height = 0;
		SDL_GetWindowSize(m_Window, &width, &height);

		m_CurrentFrame += 1;
		auto result = m_Queue->Signal(m_SwapchainFence.Get(), m_CurrentFrame);
		if (result != S_OK) UI_FATAL("Failed to signal swap chain fence!");
		if (m_SwapchainFence->GetCompletedValue() < m_CurrentFrame)
		{
			m_SwapchainFence->SetEventOnCompletion(m_CurrentFrame, m_SwapchainEvent);
			WaitForSingleObject(m_SwapchainEvent, INFINITE);
		}
		m_SwapchainImages.clear();

		DXGI_SWAP_CHAIN_DESC swapchainDesc;
		result = m_Swapchain->GetDesc(&swapchainDesc);
		if (result != S_OK) UI_FATAL("Could not get swapchain desc!");

		result = m_Swapchain->ResizeBuffers(swapchainDesc.BufferCount, width, height, DXGI_FORMAT_UNKNOWN, 0);
		if (result != S_OK) UI_FATAL("Could not resize swapchain!");

		D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
		heapDesc.NumDescriptors = swapchainDesc.BufferCount;
		heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		result = m_Device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_DescriptorPool));
		if (result != S_OK) UI_FATAL("Could not create descriptor heap!");

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_DescriptorPool->GetCPUDescriptorHandleForHeapStart());
		m_SwapchainImages.resize(swapchainDesc.BufferCount);
		for (size_t i=0; i<swapchainDesc.BufferCount; ++i)
		{
			result = m_Swapchain->GetBuffer(i, IID_PPV_ARGS(&m_SwapchainImages[i]));
			if (result != S_OK) UI_FATAL("Could not get swapchain image!");
			m_Device->CreateRenderTargetView(m_SwapchainImages[i].Get(), nullptr, rtvHandle);
			rtvHandle.Offset(1, m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
		}
	}

protected:
	SDL_Window* m_Window;
	ComPtr<IDXGIFactory4> m_Instance;
	ComPtr<IDXGISwapChain3> m_Swapchain;
	ComPtr<IDXGIAdapter1> m_PhysicalDevice;
	ComPtr<ID3D12Device> m_Device;
	ComPtr<ID3D12CommandQueue> m_Queue;
	ComPtr<ID3D12CommandAllocator> m_CommandPool;
	ComPtr<ID3D12GraphicsCommandList> m_CommandBuffer;
	ComPtr<ID3D12DescriptorHeap> m_DescriptorPool;
	uint32_t m_CurrentFrame = 0;
	UIList<ComPtr<ID3D12Resource>> m_SwapchainImages;
	ComPtr<ID3D12Fence> m_SwapchainFence;
	HANDLE m_SwapchainEvent;
	DWORD m_MessageCookie;
	UIContextRef m_UIContext;

	friend class CairoDXRender;
};
#endif