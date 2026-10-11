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
#include <nanovg.h>
#include <nanovg_rt.h>
#include "OpenGL.h"

static void presentFrame(rt_module_render_t& module, rt_mesh_t& mesh, int width, int height, rt_texture_t* source)
{
	rt_pass_render_t pass = {};
	pass.screen.color.clear = true;
	pass.screen.color.value = {0.3f, 0.3f, 0.32f, 1.0f};
	pass.screen.color.blend.src = RT_BLEND_ONE;
	pass.screen.color.blend.dst = RT_BLEND_ONE_MINUS_SRC_ALPHA;
	rt_begin_render(pass);
	rt_set_viewport(0.0f, 0.0f, (float)width, (float)height, 0.0f, 1.0f);
	if (source && source->handle && module.handle && mesh.handle)
	{
		rt_bind_module_render(module);
		rt_bind_texture(*source, {.binding = 0});
		rt_draw_mesh(mesh);
	}
	rt_end_render(pass);
	rt_submit();
}

OpenRTDevice::OpenRTDevice()
{
	// Initialize Window Context

	auto W = 1000, H = 600;
	auto window = SDL_CreateWindow("https://github.com/ChivenZhang/OpenUI.git", W, H, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN);
	if (window == nullptr) UI_FATAL("Window could not be created! SDL_Error: %s", SDL_GetError());
	auto context = SDL_GL_CreateContext(window);
	if (context == nullptr)
	{
		SDL_DestroyWindow(window);
		UI_FATAL("GL context could not be created! SDL_Error: %s", SDL_GetError());
	}
	SDL_GL_MakeCurrent(window, context);
	SDL_GL_SetSwapInterval(0);
	m_Window = window;
	m_Context = context;

	// Initialize NanoVG context (OpenRT backend) and default UI fonts

	m_NanoVG = rtCreateRT(RTVG_ANTIALIAS | RTVG_STENCIL_STROKES);
	if (m_NanoVG == nullptr) UI_FATAL("NanoVG context could not be created");
	auto fontSans = nvgCreateFont(m_NanoVG, "sans", "../../Example/OpenRT/Roboto-Regular.ttf");
	auto fontBold = nvgCreateFont(m_NanoVG, "sans-bold", "../../Example/OpenRT/Roboto-Bold.ttf");
	auto fontLight = nvgCreateFont(m_NanoVG, "sans-light", "../../Example/OpenRT/Roboto-Light.ttf");
	auto fontEmoji = nvgCreateFont(m_NanoVG, "emoji", "../../Example/OpenRT/NotoEmoji-Regular.ttf");
	if (fontEmoji != -1)
	{
		if (fontSans != -1) nvgAddFallbackFontId(m_NanoVG, fontSans, fontEmoji);
		if (fontBold != -1) nvgAddFallbackFontId(m_NanoVG, fontBold, fontEmoji);
		if (fontLight != -1) nvgAddFallbackFontId(m_NanoVG, fontLight, fontEmoji);
	}

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
	SDL_GL_MakeCurrent(m_Window, m_Context);
	m_Canvas = nullptr;

	if (m_NanoVG) { rtDeleteRT(m_NanoVG); m_NanoVG = nullptr; }
	if (m_Cursor) { SDL_DestroyCursor(m_Cursor); m_Cursor = nullptr; }

	SDL_GL_DestroyContext(m_Context); m_Context = nullptr;
    SDL_DestroyWindow(m_Window); m_Window = nullptr;
}

UICanvasRaw OpenRTDevice::getCanvas() const
{
    return m_Canvas.get();
}

void OpenRTDevice::setCursor(UIString type)
{
	SDL_SystemCursor id = SDL_SYSTEM_CURSOR_DEFAULT;
	if (type == "pointer" || type == "hand") id = SDL_SYSTEM_CURSOR_POINTER;
	else if (type == "text" || type == "ibeam") id = SDL_SYSTEM_CURSOR_TEXT;
	else if (type == "crosshair") id = SDL_SYSTEM_CURSOR_CROSSHAIR;
	else if (type == "move" || type == "all-scroll") id = SDL_SYSTEM_CURSOR_MOVE;
	else if (type == "wait") id = SDL_SYSTEM_CURSOR_WAIT;
	else if (type == "progress") id = SDL_SYSTEM_CURSOR_PROGRESS;
	else if (type == "not-allowed" || type == "no-drop") id = SDL_SYSTEM_CURSOR_NOT_ALLOWED;
	else if (type == "ew-resize" || type == "col-resize") id = SDL_SYSTEM_CURSOR_EW_RESIZE;
	else if (type == "ns-resize" || type == "row-resize") id = SDL_SYSTEM_CURSOR_NS_RESIZE;
	else if (type == "nesw-resize") id = SDL_SYSTEM_CURSOR_NESW_RESIZE;
	else if (type == "nwse-resize") id = SDL_SYSTEM_CURSOR_NWSE_RESIZE;
	else if (type == "n-resize") id = SDL_SYSTEM_CURSOR_N_RESIZE;
	else if (type == "s-resize") id = SDL_SYSTEM_CURSOR_S_RESIZE;
	else if (type == "e-resize") id = SDL_SYSTEM_CURSOR_E_RESIZE;
	else if (type == "w-resize") id = SDL_SYSTEM_CURSOR_W_RESIZE;
	else if (type == "ne-resize") id = SDL_SYSTEM_CURSOR_NE_RESIZE;
	else if (type == "nw-resize") id = SDL_SYSTEM_CURSOR_NW_RESIZE;
	else if (type == "se-resize") id = SDL_SYSTEM_CURSOR_SE_RESIZE;
	else if (type == "sw-resize") id = SDL_SYSTEM_CURSOR_SW_RESIZE;

	if (m_Cursor)
	{
		SDL_DestroyCursor(m_Cursor);
		m_Cursor = nullptr;
	}
	m_Cursor = SDL_CreateSystemCursor(id);
	if (m_Cursor) SDL_SetCursor(m_Cursor);
}

UIString OpenRTDevice::getClipText() const
{
	char* text = SDL_GetClipboardText();
	UIString result = text ? text : "";
	SDL_free(text);
	return result;
}

void OpenRTDevice::setClipText(UIString text)
{
	SDL_SetClipboardText(text.c_str());
}

void OpenRTDevice::setKeyboard(bool value)
{
	if (m_Window == nullptr) return;
	if (value) SDL_StartTextInput(m_Window);
	else SDL_StopTextInput(m_Window);
}

bool OpenRTDevice::translateText(UIString text, UIString& result) const
{
	if (text.empty()) return false;
	result = std::move(text);
	return true;
}

void OpenRTDevice::logMessage(uint8_t type, UIString text) const
{
	switch (type)
	{
	case 0: UI_DEBUG("%s", text.c_str()); break;
	case 1: UI_INFO("%s", text.c_str()); break;
	case 2: UI_WARN("%s", text.c_str()); break;
	default: UI_ERROR("%s", text.c_str()); break;
	}
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

	// Paint the canvas, then present the premultiplied target over the window.

	int32_t width = 0, height = 0;
	SDL_GetWindowSizeInPixels(window, &width, &height);
	if (width <= 0 || height <= 0) return true;


	SDL_GL_MakeCurrent(m_Window, m_Context);

	auto t0 = ::clock();
	if (render)
	{
		auto source = render->newImage((uint32_t)width, (uint32_t)height);
		if (source.Handle)
		{
			canvas->setTarget(source);
			canvas->updateWidget(::clock() * 0.001f, UIRect{0, 0, (float)width, (float)height});
		}
		auto* target = source.Handle ? (rt_texture_t*)(uintptr_t)source.Handle : nullptr;
		gl_draw_screen(width, height, {}, *target);
		SDL_GL_SwapWindow(m_Window);

		if (source.Handle) render->delImage(source);
	}
	else
	{
		rt_texture_t empty;
		gl_draw_screen(width, height, {}, empty);
		SDL_GL_SwapWindow(m_Window);
	}

	auto t1 = ::clock();

	std::cout << (t1 - t0) << " ms" << std::endl;
	return true;
}

SDL_Window* OpenRTDevice::getWindow() const
{
    return m_Window;
}

NVGcontext* OpenRTDevice::getNanoVG() const
{
    return m_NanoVG;
}

#endif