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
#ifdef OPENUI_ENABLE_METAL
#define NS_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#define MTK_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION
#include <Metal/Metal.hpp>
#include <MetalKit/MTKView.hpp>
#include <QuartzCore/CAMetalLayer.hpp>
#include <SDL3/SDL.h>
#include <SDL3/SDL_metal.h>
#include <OpenUI/UIDevice.h>
#include "CairoMTRender.h"
#include "../SDL3InputEnum.h"
#include "../Cairo/CairoUIPainter.h"

/// @brief
class SDL3MTDevice
{
public:
	explicit SDL3MTDevice()
	{
		auto window = SDL_CreateWindow("https://github.com/ChivenZhang/OpenUI.git", 1000, 600, SDL_WINDOW_METAL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN);
		if (window == nullptr)
		{
			SDL_Quit();
			UI_FATAL("Failed to create window");
		}
		m_Window = window;

		int width = 0, height = 0;
		SDL_GetWindowSize(window, &width, &height);

		// 创建设备

		auto devices = MTL::CopyAllDevices();
		for (size_t i = 0; i < devices->count(); ++i)
		{
			auto device = (MTL::Device*)devices->object(i);
			const char* name = device->name()->utf8String();
			bool isLowPower = device->lowPower();
			bool supportsFamily = device->supportsFamily(MTL::GPUFamilyApple1);
			printf("Device %zu: %s (Low Power: %d, Is M-GPU: %d)\n", i, name, isLowPower, supportsFamily);
		}
		devices->release();
		auto device = MTL::CreateSystemDefaultDevice();
		if (device == nullptr)
		{
			SDL_Quit();
			UI_FATAL("Failed to create device");
		}
		m_Device = device;

		// 创建队列

		auto queue = device->newCommandQueue();
		if (queue == nullptr)
		{
			SDL_Quit();
			UI_FATAL("Failed to create command queue");
		}
		m_Queue = queue;

		// 创建信号量

		m_Semaphore = dispatch_semaphore_create(3);

		// 创建交换链

		auto surface = SDL_Metal_CreateView(window);
		if (surface == nullptr)
		{
			SDL_Quit();
			UI_FATAL("Failed to create view");
		}
		m_Surface = surface;

		auto swapchain = (CA::MetalLayer*)SDL_Metal_GetLayer(surface);
		if (swapchain == nullptr)
		{
			SDL_Quit();
			UI_FATAL("Failed to create swapchain");
		}
		m_Swapchain = swapchain;
		m_Swapchain->setDevice(m_Device);
		m_Swapchain->setPixelFormat(MTL::PixelFormatRGBA8Unorm);
		recreateSwapchain();

		// Initialize OpenUI context

		auto scale = SDL_GetWindowDisplayScale(window);
		UIConfig config{.DisplayScale = scale};
		auto openui = UINew<UIContext>(config);
		openui->setPainter(UINew<CairoUIPainter>(width, height));
		openui->setRender(UINew<CairoMTRender>(width, height, this));
		m_UIContext = openui;

		SDL_ShowWindow(window);
	}

	~SDL3MTDevice()
	{
		m_UIContext = nullptr;
		m_Queue->release(); m_Queue = nullptr;
		m_Device->release(); m_Device = nullptr;
		SDL_Metal_DestroyView(m_Surface); m_Surface = nullptr;
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

		int width, height;
		SDL_GetWindowSize(window, &width, &height);
		UIRect client{0, 0, (float)width, (float)height};
		openui->updateElement(::clock() * 0.001f, client);

		// Output frame to screen

		dispatch_semaphore_wait(m_Semaphore, DISPATCH_TIME_FOREVER);

		auto commandBuffer = m_CommandBuffer = m_Queue->commandBuffer();

		auto drawable = m_Swapchain->nextDrawable();
		auto renderPassDescriptor = MTL::RenderPassDescriptor::renderPassDescriptor();
		renderPassDescriptor->colorAttachments()->object(0)->setTexture(drawable->texture());
		renderPassDescriptor->colorAttachments()->object(0)->setLoadAction(MTL::LoadActionClear);
		renderPassDescriptor->colorAttachments()->object(0)->setStoreAction(MTL::StoreActionStore);
		renderPassDescriptor->colorAttachments()->object(0)->setClearColor(MTL::ClearColor(0.3, 0.3, 0.8, 1.0));
        auto renderEncoder = commandBuffer->renderCommandEncoder(renderPassDescriptor);

		auto pixels = UICast<CairoUIPainter>(openui->getPainter())->getPixels();
		UICast<CairoMTRender>(openui->getRender())->uploadTexture(width, height, (uint8_t*)pixels.data());

		openui->renderElement(client);

		renderEncoder->endEncoding();
		commandBuffer->presentDrawable(drawable);
		commandBuffer->addCompletedHandler([=](MTL::CommandBuffer* buffer) {
			dispatch_semaphore_signal(m_Semaphore);
		});
        commandBuffer->commit();
		return true;
	}

	void recreateSwapchain()
	{
		int width = 0, height = 0;
		SDL_GetWindowSize(m_Window, &width, &height);
		m_Swapchain->setDrawableSize({1.0f * width, 1.0f * height});
	}

protected:
	SDL_Window* m_Window;
	SDL_MetalView m_Surface;
	CA::MetalLayer* m_Swapchain;
	MTL::Device* m_Device;
	MTL::CommandQueue* m_Queue;
	MTL::CommandBuffer* m_CommandBuffer;
	dispatch_semaphore_t m_Semaphore;
	int m_CurrentFrame = 0;
	UIContextRef m_UIContext;

	friend class CairoMTRender;
};
#endif