#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>  
#include <SDL2/SDL_surface.h>
#include <GL/glew.h>
#include <iostream>  
#include <cairo/cairo.h>
#include "SDL2Painter.h"
#include "SDL2Render.h"
#include "Widget/Private/RmGUIContext.h"
#include "Widget/Private/RmGUIHBox.h"
#include "Widget/Private/RmGUIVBox.h"
#include "Widget/Private/RmGUIFlow.h"
#include "Widget/Private/RmGUIScroll.h"
#include "Widget/Private/RmGUIPanel.h"
#include "Widget/Private/RmGUILabel.h"
#include "Widget/Private/RmGUIButton.h"
#include "Widget/Private/RmGUIScroll.h"
#include "Widget/Private/RmGUICombo.h"
#include "Widget/Private/RmGUIText.h"
#include "Widget/Private/RmGUIEdit.h"

static int count = 0;

int main(int argc, char* argv[]) {
	SDL_Window* window;
	SDL_GLContext glContext;

	// 初始化SDL  
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
		return -1;
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	// 创建SDL窗口  
	window = SDL_CreateWindow("https://github.com/ChivenZhang/OpenUI.git", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1000, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);
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
	auto context = RmNew<RmGUIContext>();
	auto painter = RmNew<SDL2Painter>(w, h);
	auto render = RmNew<SDL2Render>();
	context->setPainter(painter);
	context->setRender(render);
	auto texture = SDL_CreateTexture(renderer, SDL_PixelFormatEnum::SDL_PIXELFORMAT_BGRA32, SDL_TextureAccess::SDL_TEXTUREACCESS_STREAMING, painter->getWidth(), painter->getHeight());

	auto top = RmNew<RmGUIFlow>();
	context->addWidget(top);

#if 0
	{
		auto root = RmNew<RmGUIVBox>();
		top->addWidget(root);
		root->setBorder({ 5,5,5,5 });

		auto child0 = RmNew<RmGUIHBox>();
		root->addWidget(child0);
		child0->setBorder({ 5,5,5,5 });

		auto child1 = RmNew<RmGUIPanel>();
		child0->addWidget(child1);
		child1->setFixedWidth(100); child1->setFixedHeight(50);
		child1->setBorder({ 5,5,5,5 });

		auto child11 = RmNew<RmGUIPanel>();
		child0->addWidget(child11);
		child11->setFixedWidth(100); child11->setFixedHeight(50);
		child11->setBorder({ 5,5,5,5 });

		auto child2 = RmNew<RmGUIHBox>();
		root->addWidget(child2);
		child2->setBorder({ 5,5,5,5 });

		auto child3 = RmNew<RmGUIPanel>();
		child2->addWidget(child3);
		child3->setFixedWidth(100); child3->setFixedHeight(50);
		child3->setBorder({ 5,5,5,5 });

		auto child33 = RmNew<RmGUIPanel>();
		child2->addWidget(child33);
		child33->setFixedWidth(100); child33->setFixedHeight(50);
		child33->setBorder({ 5,5,5,5 });

		auto child333 = RmNew<RmGUIPanel>();
		child2->addWidget(child333);
		child333->setFixedWidth(100); child333->setFixedHeight(50);
		child333->setBorder({ 5,5,5,5 });
	}
#endif

#if 0
	{
		auto root = RmNew<RmGUIHBox>();
		top->addWidget(root);
		root->setBorder({ 5,5,5,5 });

		auto child0 = RmNew<RmGUIPanel>();
		root->addWidget(child0);
		child0->setBorder({ 5,5,5,5 });

		auto child1 = RmNew<RmGUIPanel>();
		child0->addWidget(child1);
		child1->setFixedWidth(100); child1->setFixedHeight(50);
		child1->setBorder({ 5,5,5,5 });

		auto child11 = RmNew<RmGUIPanel>();
		child0->addWidget(child11);
		child11->setFixedWidth(100); child11->setFixedHeight(50);
		child11->setBorder({ 5,5,5,5 });

		auto child111 = RmNew<RmGUIPanel>();
		child0->addWidget(child111);
		child111->setFixedWidth(100); child111->setFixedHeight(50);
		child111->setBorder({ 5,5,5,5 });

		auto child2 = RmNew<RmGUIVBox>();
		root->addWidget(child2);
		child2->setBorder({ 5,5,5,5 });

		auto child3 = RmNew<RmGUIPanel>();
		child2->addWidget(child3);
		child3->setFixedWidth(100); child3->setFixedHeight(50);
		child3->setBorder({ 5,5,5,5 });

		auto child33 = RmNew<RmGUIPanel>();
		child2->addWidget(child33);
		child33->setFixedWidth(100); child33->setFixedHeight(50);
		child33->setBorder({ 5,5,5,5 });
	}
#endif

#if 0
	{
		auto root = RmNew<RmGUIFlow>();
		top->addWidget(root);
		// root->setFixedWidth(400); root->setFixedHeight(200);
		root->setBorder({ 5,5,5,5 });

		auto label = RmNew<RmGUILabel>();
		label->setText("计数:0");
		label->setFixedWidth(100); label->setFixedHeight(35);
		label->setAlignment(RmFont::AlignCenter | RmFont::AlignVCenter);
		label->setMargin({ 5,5,5,5 });
		auto style = label->getStyle();
		style.Brush.Color = { 0,0,0,1 };
		style.Font.Style = RmFont::StyleItalic;
		style.Font.Weight = RmFont::WeightBold;
		label->setStyle(style);

		auto button1 = RmNew<RmGUIButton>();
		root->addWidget(button1);
		button1->setText("自减");
		button1->setFixedWidth(100); button1->setFixedHeight(35);
		button1->setMargin({ 5,5,5,5 });
		// button1->setCheckable(true);
		style = button1->getTextStyle();
		style.Brush.Color = { 0.5,0,1,1 };
		button1->setTextStyle(style);
		button1->clicked->connect(nullptr, [=](bool checked) {
			label->setText("计数:" + std::to_string(count = std::clamp(--count, 0, 10)));
			});

		root->addWidget(label);

		auto button2 = RmNew<RmGUIButton>();
		root->addWidget(button2);
		button2->setText("自增 ");
		button2->setFixedWidth(100); button2->setFixedHeight(35);
		button2->setMargin({ 5,5,5,5 });
		style = button2->getTextStyle();
		style.Brush.Color = { 0.5,0,1,1 };
		button2->setTextStyle(style);
		button2->clicked->connect(nullptr, [=](bool checked) {
			label->setText("计数:" + std::to_string(count = std::clamp(++count, 0, 10)));
			});

		auto button3 = RmNew<RmGUIButton>();
		root->addWidget(button3);
		button3->setText("重置");
		button3->setFixedWidth(100); button3->setFixedHeight(35);
		button3->setMargin({ 5,5,5,5 });
		style = button3->getTextStyle();
		style.Brush.Color = { 0.5,0,1,1 };
		button3->setTextStyle(style);
		button3->clicked->connect(nullptr, [=](bool checked) {
			label->setText("计数:" + std::to_string(count = 0));
			});

		auto scroll = RmNew<RmGUIScroll>();
		root->addWidget(scroll);
		scroll->setMinWidth(50); scroll->setMinHeight(50);
		scroll->setMargin({ 5,5,5,5 });

		auto panel = RmNew<RmGUIPanel>();
		scroll->addWidget(panel);
		panel->setRect({ 0, 0, 2000, 2000 });
	}
#endif

#if 0
	if (true)
	{
		auto button = RmNew<RmGUIButton>();
		button->setFixedHeight(100);
		button->setText("Button");
		auto style = button->getTextStyle();
		style.Font.Size = 30;
		button->setTextStyle(style);
		auto style2 = button->getStyle();
		style2.Normal.Brush.Color = { 1,1,1,0.2 };
		style2.Hover.Brush.Color = { 1,1,1,0.5 };
		style2.Press.Brush.Color = { 1,1,1,0.8 };
		button->setStyle(style2);
		context->addWidget(button);
	}
#endif

#if 1
	if (true)
	{
		auto combo = RmNew<RmGUICombo>();
		top->addWidget(combo);
		combo->setPosition(100, 100);
		combo->setFixedSize(100, 35);
		combo->setMaxCount(4);
		combo->setItems({ "White", "Red", "Green", "Blue", "Black" });
		combo->setCurrentIndex(0);
		combo->currentTextChanged->connect(nullptr, [](RmString text) {
			printf("combo %s\n", text.c_str());
			});

		auto combo2 = RmNew<RmGUICombo>();
		top->addWidget(combo2);
		combo2->setPosition(200, 100);
		combo2->setFixedSize(100, 35);
		combo2->setItems({ "111", "222", "333", "444", "555", "666" });
		combo2->setCurrentText("666");
		combo2->currentTextChanged->connect(nullptr, [](RmString text) {
			printf("combo2 %s\n", text.c_str());
			});
	}
#endif

#if 0
	if (true)
	{
		auto text = RmNew<RmGUIText>();
		context->addWidget(text);
	}
#endif

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
				// 可以使用 event.button1.button1 来获取被点击的鼠标按钮的编号  
			{
				int x, y;
				SDL_GetWindowPosition(window, &x, &y);
				if (event.button.clicks == 1)
				{
					IRmGUIMouseDownEvent event2(event.motion.x, event.motion.y, x + event.motion.x, y + event.motion.y, event.button.button, event.button.button, event.key.keysym.mod, event.button.clicks);
					context->sendEvent(nullptr, &event2);
				}
				else
				{
					IRmGUIMouseDblClickEvent event2(event.motion.x, event.motion.y, x + event.motion.x, y + event.motion.y, event.button.button, event.button.button, event.key.keysym.mod, event.button.clicks);
					context->sendEvent(nullptr, &event2);
				}
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
				IRmGUIMouseWheelEvent event2(event.wheel.x, event.wheel.y, event.wheel.x, event.wheel.y, event.wheel.mouseX, event.wheel.mouseY, x + event.wheel.mouseX, y + event.wheel.mouseY, event.button.button, event.button.button, event.key.keysym.mod);
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
					painter->resize(event.window.data1, event.window.data2);
					SDL_DestroyTexture(texture);
					texture = SDL_CreateTexture(renderer, SDL_PixelFormatEnum::SDL_PIXELFORMAT_BGRA32, SDL_TextureAccess::SDL_TEXTUREACCESS_STREAMING, painter->getWidth(), painter->getHeight());

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
		SDL_SetRenderDrawColor(renderer, 238, 238, 242, 255);
		SDL_RenderClear(renderer);
		int w, h;
		SDL_GetWindowSize(window, &w, &h);
		context->layoutWidget(RmRect{ 0, 0, (float)w, (float)h });
		context->paintWidget(RmRect{ 0, 0, (float)w, (float)h });
		context->renderWidget(RmRect{ 0, 0, (float)w, (float)h });

		// 更新屏幕内容  
		SDL_UpdateTexture(texture, nullptr, painter->getPixelData().data(), painter->getStride());
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