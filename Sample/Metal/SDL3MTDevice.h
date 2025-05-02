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
#include <AppKit/AppKit.hpp>
#include <MetalKit/MetalKit.hpp>
#include <SDL3/SDL.h>
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

		// TODO: Output frame to screen

		auto pixels = UICast<CairoUIPainter>(openui->getPainter())->getPixels();
		UICast<CairoMTRender>(openui->getRender())->uploadTexture(w, h, (uint8_t*)pixels.data());

		openui->renderElement(client);
		return true;
	}

	void recreateSwapchain()
	{
	}

protected:
	SDL_Window* m_Window;
	UIContextRef m_UIContext;

	friend class CairoMTRender;
};
#endif