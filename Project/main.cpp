#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>  
#include <SDL2/SDL_surface.h>
#include <GL/glew.h>
#include <iostream>  
#include <cairo/cairo.h>
#include "Widget/IRmGUIContext.h"
#include "SDL2Surface.h"
#include "Widget/Private/RmGUIPanel.h"
#include "Widget/Private/RmGUILabel.h"

int main(int argc, char* argv[]) {
	SDL_Window* window;
	SDL_GLContext glContext;

	// 初始化SDL  
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
		return -1;
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	// 创建SDL窗口  
	window = SDL_CreateWindow("SDL2, OpenGL and GLEW", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1000, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);
	if (window == nullptr) {
		std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return -1;
	}

	// 创建OpenGL上下文  
	glContext = SDL_GL_CreateContext(window);
	if (glContext == nullptr) {
		std::cerr << "OpenGL context could not be created! SDL_Error: " << SDL_GetError() << std::endl;
		SDL_DestroyWindow(window);
		SDL_Quit();
		return -1;
	}

	// 初始化GLEW  
	GLenum glewInitResult = glewInit();
	if (GLEW_OK != glewInitResult) {
		std::cerr << "GLEW could not be initialized! " << glewGetErrorString(glewInitResult) << std::endl;
		SDL_GL_DeleteContext(glContext);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return -1;
	}

	//// 检查OpenGL版本信息  
	std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == NULL) {
		// 处理错误  
		SDL_Log("Could not create renderer: %s", SDL_GetError());
		exit(1);
	}

	// 这里可以添加OpenGL的初始化和渲染代码  

	int w, h;
	SDL_GetWindowSize(window, &w, &h);
	auto surface = RmNew<SDL2Surface>(w, h);
	auto context = IRmGUIContext::GetInstance();
	context->addWidget(RmNew<RmGUIPanel>());
	context->setSurface(surface);

	auto texture = SDL_CreateTextureFromSurface(renderer, surface->getNativeSurface());

	// 主循环  
	bool quit = false;
	SDL_Event event;
	while (!quit)
	{
		// ... 处理事件、渲染等 ...  
		while (SDL_PollEvent(&event))
		{
			// 根据事件类型进行处理  
			switch (event.type)
			{
			case SDL_QUIT: // 用户请求退出（如点击了窗口的关闭按钮）  
				// 执行退出操作（如关闭窗口、清理资源、结束程序）...  
				SDL_Quit();
				return 0;
			case SDL_KEYDOWN: // 键盘按键按下  
				// 处理键盘按键事件...  
				// 可以使用 event.key.keysym.sym 来获取按下的键的标识符
			{
				IRmGUIKeyDownEvent event2(event.key.keysym.sym, event.key.keysym.mod, event.key.keysym.scancode, event.key.keysym.sym, event.key.keysym.mod, RmString(), event.key.repeat);
				context->sendEvent(nullptr, &event2);
			}
			break;
			case SDL_KEYUP: // 键盘按键释放  
				// 处理键盘按键释放事件...  
			{
				IRmGUIKeyUpEvent event2(event.key.keysym.sym, event.key.keysym.mod, event.key.keysym.scancode, event.key.keysym.sym, event.key.keysym.mod, RmString(), event.key.repeat);
				context->sendEvent(nullptr, &event2);
			}
			break;
			case SDL_TEXTINPUT:
			{
				IRmGUIKeyInputEvent event2(event.key.keysym.sym, event.key.keysym.mod, event.key.keysym.scancode, event.key.keysym.sym, event.key.keysym.mod, event.edit.text, event.key.repeat);
				context->sendEvent(nullptr, &event2);
			}
			break;
			case SDL_MOUSEMOTION: // 鼠标移动  
				// 处理鼠标移动事件...  
				// 可以使用 event.motion.x 和 event.motion.y 来获取鼠标的当前位置  
			{
				int x, y;
				SDL_GetWindowPosition(window, &x, &y);
				IRmGUIMouseMoveEvent event2(event.motion.x, event.motion.y, x + event.motion.x, y + event.motion.y, event.button.button, event.button.button, event.key.keysym.mod);
				context->sendEvent(nullptr, &event2);
			}
			break;
			case SDL_MOUSEBUTTONDOWN: // 鼠标点击  
				// 处理鼠标点击事件...  
				// 可以使用 event.button.button 来获取被点击的鼠标按钮的编号  
			{
				int x, y;
				SDL_GetWindowPosition(window, &x, &y);
				IRmGUIMouseDownEvent event2(event.motion.x, event.motion.y, x + event.motion.x, y + event.motion.y, event.button.button, event.button.button, event.key.keysym.mod);
				context->sendEvent(nullptr, &event2);
			}
			break;
			case SDL_MOUSEBUTTONUP: // 鼠标按钮释放  
				// 处理鼠标按钮释放事件...  
			{
				int x, y;
				SDL_GetWindowPosition(window, &x, &y);
				IRmGUIMouseUpEvent event2(event.motion.x, event.motion.y, x + event.motion.x, y + event.motion.y, event.button.button, event.button.button, event.key.keysym.mod);
				context->sendEvent(nullptr, &event2);
			}
			break;
			case SDL_MOUSEWHEEL: // 鼠标滚轮
			{
				int x, y;
				SDL_GetWindowPosition(window, &x, &y);
				IRmGUIMouseWheelEvent event2(event.wheel.x, event.wheel.y, event.wheel.x, event.wheel.y, event.motion.x, event.motion.y, x + event.motion.x, y + event.motion.y, event.button.button, event.button.button, event.key.keysym.mod);
				context->sendEvent(nullptr, &event2);
			}
			break;
			case SDL_WINDOWEVENT:
				// 窗体事件  
				switch (event.window.event)
				{
				case SDL_WINDOWEVENT_SHOWN:
				{
					IRmGUIShowEvent event2;
					context->sendEvent(nullptr, &event2);
				} break;
				case SDL_WINDOWEVENT_HIDDEN:
				{
					IRmGUIHideEvent event2;
					context->sendEvent(nullptr, &event2);
				} break;
				case SDL_WINDOWEVENT_CLOSE:
				{
					IRmGUICloseEvent event2;
					context->sendEvent(nullptr, &event2);
				} break;
				case SDL_WINDOWEVENT_MOVED:
				{
					IRmGUIMoveEvent event2(event.window.data1, event.window.data2);
					context->sendEvent(nullptr, &event2);
				} break;
				case SDL_WINDOWEVENT_RESIZED:
					// 窗口大小改变  
					// event.window.data1 是新的宽度  
					// event.window.data2 是新的高度  
					// printf("Window resized to %dx%d\n", event.window.data1, event.window.data2);
				{
					surface->resize(event.window.data1, event.window.data2);
					SDL_DestroyTexture(texture);
					texture = SDL_CreateTextureFromSurface(renderer, surface->getNativeSurface());

					IRmGUIResizeEvent event2(event.window.data1, event.window.data2);
					context->sendEvent(nullptr, &event2);
				}
				break;
				case SDL_WINDOWEVENT_MINIMIZED:
					// 窗口最小化  
					// printf("Window minimized\n");
					break;
				case SDL_WINDOWEVENT_MAXIMIZED:
					// 窗口最大化  
					// printf("Window maximized\n");
					break;
				case SDL_WINDOWEVENT_RESTORED:
					// 窗口从最小化或最大化状态恢复  
					// printf("Window restored\n");
					break;
				case SDL_WINDOWEVENT_ENTER:
				{
					int x, y;
					SDL_GetWindowPosition(window, &x, &y);
					IRmGUIMouseEnterEvent event2(event.motion.x, event.motion.y, x + event.motion.x, y + event.motion.y, event.button.button, event.button.button, event.key.keysym.mod);
					context->sendEvent(nullptr, &event2);
				} break;
				case SDL_WINDOWEVENT_LEAVE:
				{
					int x, y;
					SDL_GetWindowPosition(window, &x, &y);
					IRmGUIMouseLeaveEvent event2(event.motion.x, event.motion.y, x + event.motion.x, y + event.motion.y, event.button.button, event.button.button, event.key.keysym.mod);
					context->sendEvent(nullptr, &event2);
				} break;
				case SDL_WINDOWEVENT_FOCUS_GAINED:
				{
					IRmGUIFocusEvent event2(true);
					context->sendEvent(nullptr, &event2);
				} break;
				case SDL_WINDOWEVENT_FOCUS_LOST:
				{
					IRmGUIFocusEvent event2(false);
					context->sendEvent(nullptr, &event2);
				} break;
				// ... 其他窗体事件  
				default:
					break;
				}
				break;
			default:
				// 忽略未处理的事件  
				break;
			}
		}

		// 渲染所有控件
		SDL_SetRenderDrawColor(renderer, 104, 33, 122, 255);
		SDL_RenderClear(renderer);
		int x, y, w, h;
		SDL_GetWindowSize(window, &w, &h);
		SDL_GetWindowPosition(window, &x, &y);
		context->renderWidget(RmRect{ (float)x, (float)y, (float)w, (float)h });

		// 更新屏幕内容  
		SDL_UpdateTexture(texture, nullptr, surface->getPixelData().data(), surface->getStride());
		SDL_RenderCopy(renderer, texture, nullptr, nullptr);
		SDL_RenderPresent(renderer);
	}

	// 清理并退出SDL  
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_GL_DeleteContext(glContext);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}