/*=================================================
* Copyright © 2020-2026 ChivenZhang.
* All Rights Reserved.
* =====================Note=========================
*
*
* ====================History=======================
* Created by chivenzhang@gmail.com.
*
* =================================================*/
#ifdef OPENUI_ENABLE_OPENRT
#include "OpenRTDevice.h"
#include "OpenRTRender.h"
#include "OpenRTPainter.h"
#include "../SDL3InputEnum.h"
#define NANOVG_GL3_IMPLEMENTATION
#include <GL/glew.h>
#include <nanovg.h>
#include <nanovg_rt.h>
#include "demo.h"
#include "OpenGL.h"
#include "perf.h"

OpenRTDevice::OpenRTDevice()
{
	// Initialize Window Context

	auto W = 1000, H = 600;
    auto window = SDL_CreateWindow("https://github.com/ChivenZhang/OpenUI.git", W, H,  SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN);
	auto context = SDL_GL_CreateContext(window);
	SDL_GL_MakeCurrent(window, context);
	SDL_GL_SetSwapInterval(0);
	m_Window = window;
	m_Context = context;

	// Initialize OpenGL Context

	// Initialize OpenUI context

	auto scale = SDL_GetWindowDisplayScale(window);
	UIConfig config{.DisplayScale = scale};
	auto canvas = UINew<UICanvas>(this, config);
    canvas->setRender(UINew<OpenRTRender>(canvas.get(), W, H));
    canvas->setPainter(UINew<OpenRTPainter>(canvas.get(), W, H));
    m_Canvas = canvas;

    SDL_ShowWindow(window);
}

OpenRTDevice::~OpenRTDevice()
{
    m_Canvas = nullptr;

	SDL_GL_DestroyContext(m_Context); m_Context = nullptr;
    SDL_DestroyWindow(m_Window); m_Window = nullptr;
}

UICanvasRaw OpenRTDevice::getCanvas() const
{
    return m_Canvas.get();
}

void OpenRTDevice::setCursor(UIString type)
{
}

UIString OpenRTDevice::getClipText() const
{
	return SDL_GetClipboardText();
}

void OpenRTDevice::setClipText(UIString text)
{
	SDL_SetClipboardText(text.c_str());
}

void OpenRTDevice::setKeyboard(bool value)
{
}

bool OpenRTDevice::translateText(UIString text, UIString& result) const
{
	return false;
}

void OpenRTDevice::logMessage(uint8_t type, UIString text) const
{
}

bool OpenRTDevice::update()
{
	auto canvas = getCanvas();
	auto window = getWindow();
	auto render = canvas->getRender({});

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
				canvas->sendEvent(nullptr, &event2);
			}
			break;
		case SDL_EVENT_KEY_UP:
			{
				UIKeyUpEvent event2(SDL3InputEnum::GetKeyboardEnum(event.key.key), SDL3InputEnum::GetModifierEnum(event.key.mod), event.key.scancode, event.key.key, event.key.mod, UIString(),
									event.key.repeat);
				canvas->sendEvent(nullptr, &event2);
			}
			break;
		case SDL_EVENT_TEXT_EDITING:
			{
				UITextInputEvent event2(SDL3InputEnum::GetKeyboardEnum(event.key.key), SDL3InputEnum::GetModifierEnum(event.key.mod), event.key.scancode, event.key.key, event.key.mod,
										event.edit.text, event.key.repeat, false, event.edit.start, event.edit.length);
				canvas->sendEvent(nullptr, &event2);
			}
			break;
		case SDL_EVENT_TEXT_INPUT:
			{
				UITextInputEvent event2(SDL3InputEnum::GetKeyboardEnum(event.key.key), SDL3InputEnum::GetModifierEnum(event.key.mod), event.key.scancode, event.key.key, event.key.mod,
										event.edit.text, event.key.repeat, true);
				canvas->sendEvent(nullptr, &event2);
			}
			break;
		case SDL_EVENT_MOUSE_MOTION:
			{
				int x, y;
				SDL_GetWindowPosition(window, &x, &y);
				UIMouseMoveEvent event2(event.motion.x, event.motion.y, x + event.motion.x, y + event.motion.y, SDL3InputEnum::GetMouseEnum(event.button.button),
										SDL3InputEnum::GetMouseEnum(event.button.button), SDL3InputEnum::GetModifierEnum(event.key.mod));
				canvas->sendEvent(nullptr, &event2);
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
					canvas->sendEvent(nullptr, &event2);
				}
				else
				{
					UIMouseDblClickEvent event2(event.motion.x, event.motion.y, x + event.motion.x, y + event.motion.y, SDL3InputEnum::GetMouseEnum(event.button.button),
												SDL3InputEnum::GetMouseEnum(event.button.button), SDL3InputEnum::GetModifierEnum(event.key.mod), event.button.clicks);
					canvas->sendEvent(nullptr, &event2);
				}
			}
			break;
		case SDL_EVENT_MOUSE_BUTTON_UP:
			{
				int x, y;
				SDL_GetWindowPosition(window, &x, &y);
				UIMouseUpEvent event2(event.motion.x, event.motion.y, x + event.motion.x, y + event.motion.y, SDL3InputEnum::GetMouseEnum(event.button.button),
									SDL3InputEnum::GetMouseEnum(event.button.button), SDL3InputEnum::GetModifierEnum(event.key.mod));
				canvas->sendEvent(nullptr, &event2);
			}
			break;
		case SDL_EVENT_MOUSE_WHEEL:
			{
				int x, y;
				SDL_GetWindowPosition(window, &x, &y);
				UIMouseWheelEvent event2(event.wheel.x, event.wheel.y, event.wheel.x, event.wheel.y, event.wheel.mouse_x, event.wheel.mouse_y, x + event.wheel.mouse_x, y + event.wheel.mouse_y,
										SDL3InputEnum::GetMouseEnum(event.button.button), SDL3InputEnum::GetMouseEnum(event.button.button), SDL3InputEnum::GetModifierEnum(event.key.mod));
				canvas->sendEvent(nullptr, &event2);
			}
			break;
		case SDL_EVENT_WINDOW_MOUSE_ENTER:
			{
				int x, y;
				SDL_GetWindowPosition(window, &x, &y);
				UIMouseEnterEvent event2(event.motion.x, event.motion.y, x + event.motion.x, y + event.motion.y, SDL3InputEnum::GetMouseEnum(event.button.button),
										SDL3InputEnum::GetMouseEnum(event.button.button), SDL3InputEnum::GetModifierEnum(event.key.mod));
				canvas->sendEvent(nullptr, &event2);
			}
			break;
		case SDL_EVENT_WINDOW_MOUSE_LEAVE:
			{
				int x, y;
				SDL_GetWindowPosition(window, &x, &y);
				UIMouseLeaveEvent event2(event.motion.x, event.motion.y, x + event.motion.x, y + event.motion.y, SDL3InputEnum::GetMouseEnum(event.button.button),
										SDL3InputEnum::GetMouseEnum(event.button.button), SDL3InputEnum::GetModifierEnum(event.key.mod));
				canvas->sendEvent(nullptr, &event2);
			}
			break;
		case SDL_EVENT_WINDOW_SHOWN:
			{
				UIShowEvent event2;
				canvas->sendEvent(nullptr, &event2);
			}
			break;
		case SDL_EVENT_WINDOW_HIDDEN:
			{
				UIHideEvent event2;
				canvas->sendEvent(nullptr, &event2);
			}
			break;
		case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
			{
				UICloseEvent event2;
				canvas->sendEvent(nullptr, &event2);
			}
			break;
		case SDL_EVENT_WINDOW_MOVED:
			{
				UIMoveEvent event2(event.window.data1, event.window.data2);
				canvas->sendEvent(nullptr, &event2);
			}
			break;
		case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
			{
				canvas->layoutWidget();
				// UICast<OPENRTPainter>(canvas->getPainter())->resize(event.window.data1, event.window.data2);
				UIResizeEvent event2(event.window.data1, event.window.data2);
				canvas->sendEvent(nullptr, &event2);
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
				canvas->sendEvent(nullptr, &event2);
			}
			break;
		case SDL_EVENT_WINDOW_FOCUS_LOST:
			{
				UIFocusEvent event2(false);
				canvas->sendEvent(nullptr, &event2);
			}
			break;
		default:
			break;
		}
	}

	// Output frame to screen

	int32_t width = 0, height = 0;
	SDL_GetWindowSizeInPixels(window, &width, &height);

	auto source = render->newImage(width, height);
	canvas->setTarget(source);
	canvas->updateWidget(::clock() * 0.001f, UIRect{0, 0, (float)width, (float)height});

	// Copy frame to screen

	auto t0 = SDL_GetTicks() * 0.001f;

	SDL_GL_MakeCurrent(m_Window, m_Context);
	static auto vg = rtCreateRT(RTVG_ANTIALIAS | RTVG_STENCIL_STROKES);
	static DemoData data;
	static PerfGraph fps;
	static auto loaded = []()
	{
		loadDemoData(vg, &data);
		return true;
	}();

	glViewport(0, 0, width, height);
	glClearColor(0.3f, 0.3f, 0.32f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);

	// width/height are already in device pixels, so the pixel ratio is 1.
	nvgBeginFrame(vg, width, height, 1.0f);
	renderDemo(vg, 0, 0, width, height, ::clock() * 0.001f, false, &data);
	renderGraph(vg, 5,5, &fps);
	nvgEndFrame(vg);

	gl_draw_screen(width, height, {}, rtGetTargetRT(vg));

	auto t1 = SDL_GetTicks() * 0.001f;
	updateGraph(&fps, t1 - t0);

	SDL_GL_SwapWindow(m_Window);

	render->delImage(source);
	return true;
}

SDL_Window* OpenRTDevice::getWindow() const
{
    return m_Window;
}

#endif