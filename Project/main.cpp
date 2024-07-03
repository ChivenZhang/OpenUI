#define SDL_MAIN_HANDLED
#include <GL/glew.h>
#include <SDL3/SDL.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "OpenGLPainter.h"
#include "OpenGLRender.h"
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
	SDL_GLContext context;

	// 初始化SDL  
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
		return -1;
	}
	SDL_SetHint(SDL_HINT_IME_IMPLEMENTED_UI, "1");
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	// 创建SDL窗口  
	window = SDL_CreateWindow("https://github.com/ChivenZhang/OpenUI.git", 1000, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if (window == nullptr) {
		std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return -1;
	}

	// 创建OpenGL上下文  
	context = SDL_GL_CreateContext(window);
	if (context == nullptr) {
		std::cerr << "OpenGL context could not be created! SDL_Error: " << SDL_GetError() << std::endl;
		SDL_DestroyWindow(window);
		SDL_Quit();
		return -1;
	}
	SDL_GL_SetSwapInterval(1);

	// 初始化GLEW  
	GLenum glewInitResult = glewInit();
	if (GLEW_OK != glewInitResult) {
		std::cerr << "GLEW could not be initialized! " << glewGetErrorString(glewInitResult) << std::endl;
		SDL_GL_DeleteContext(context);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return -1;
	}

	//// 检查OpenGL版本信息  
	std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;

	int w, h;
	SDL_GetWindowSize(window, &w, &h);
	auto openui = RmNew<RmGUIContext>();
	auto painter = RmNew<OpenGLPainter>(w, h);
	auto render = RmNew<OpenGLRender>();
	openui->setPainter(painter);
	openui->setRender(render);

	auto top = RmNew<RmGUIHBox>();
	openui->addWidget(top);
	// top->setPainter(painter);

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

#if 1
	{
		auto child0 = RmNew<RmGUIHBox>();
		top->addWidget(child0);
		child0->setBorder({ 5,5,5,5 });

		auto child1 = RmNew<RmGUILabel>();
		child0->addWidget(child1);
		child1->setBorder({ 5,5,5,5 });
		child1->setText("Label");
		child1->setScaledContents(RmGUILabel::ScaleNoRatio);

		int img_width, img_height, channels;
		auto image_data = stbi_load("../../../OpenGL.png", &img_width, &img_height, &channels, 4);
		if (image_data) child1->setPixmap({
			(uint32_t)img_width, (uint32_t)img_height, (uint32_t)(img_width * channels),
			RmArrayView<const uint8_t>(image_data, img_height * img_width * channels) });

		auto child11 = RmNew<RmGUILabel>();
		child0->addWidget(child11);
		child11->setBorder({ 5,5,5,5 });
		child11->setText("标签");
		child11->setScaledContents(RmGUILabel::ScaleKeepRatio);
		if (image_data) child11->setPixmap({
			(uint32_t)img_width, (uint32_t)img_height, (uint32_t)(img_width * channels),
			RmArrayView<const uint8_t>(image_data, img_height * img_width * channels) });
		stbi_image_free(image_data);

		auto child2 = RmNew<RmGUIVBox>();
		top->addWidget(child2);
		child2->setBorder({ 5,5,5,5 });

		auto child3 = RmNew<RmGUIText>();
		child2->addWidget(child3);
		child3->setBorder({ 5,5,5,5 });
		child3->setFixedSize(200, 35);
		child3->setText("Hello, World!");
		child3->setSelection(2, 5);
		child3->editingStarted->connect(nullptr, [=](RmRect rect) {
			SDL_StartTextInput(window);
			SDL_Rect sdlRect{ rect.X, rect.Y, rect.W, rect.H };
			SDL_SetTextInputArea(window, &sdlRect, 0);
			});
		child3->textPasted->connect(nullptr, [=](RmString& value) {
			if (SDL_HasClipboardText())
			{
				auto result = SDL_GetClipboardText();
				value = RmString(result);
				SDL_free(result);
			}
			});
		child3->textCopied->connect(nullptr, [=](RmString const& value) {
			SDL_ClearClipboardData();
			SDL_SetClipboardText(value.c_str());
			});

		auto child33 = RmNew<RmGUICombo>();
		child2->addWidget(child33);
		child33->setBorder({ 5,5,5,5 });
		child33->setFixedSize(100, 40);
		child33->setItems({ "一月", "二月", "三月", "四月", "五月","六月", "七月", "八月", "九月", "十月", "十一月", "十二月", });
		child33->setMaxCount(5);
		child33->setCurrentIndex(6);
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
		openui->addWidget(button);
	}
#endif

#if 0
	if (true)
	{
		auto combo = RmNew<RmGUICombo>();
		top->addWidget(combo);
		combo->setPosition(100, 100);
		combo->setFixedSize(100, 35);
		combo->setMaxCount(3);
		combo->setItems({ "White", "Red", "Green", "Blue", "Black" });
		combo->setCurrentIndex(0);
		combo->currentTextChanged->connect(nullptr, [](RmString text) {
			printf("combo %s\n", text.c_str());
			});

		auto combo2 = RmNew<RmGUICombo>();
		top->addWidget(combo2);
		combo2->setPosition(200, 100);
		combo2->setFixedSize(100, 35);
		combo2->setMaxCount(5);
		combo2->setItems({ "111", "222", "333", "444", "555", "666" });
		combo2->setCurrentText("666");
		combo2->currentTextChanged->connect(nullptr, [](RmString text) {
			printf("combo2 %s\n", text.c_str());
			});

		auto combo3 = RmNew<RmGUICombo>();
		top->addWidget(combo3);
		combo3->setPosition(200, 100);
		combo3->setFixedSize(100, 35);
		combo3->setItems({ "壹个", "贰个", "叁个", "肆个", "伍个", "陆个" });
		combo3->setCurrentIndex(0);
		combo3->currentTextChanged->connect(nullptr, [](RmString text) {
			printf("combo3 %s\n", text.c_str());
			});
	}
#endif

#if 0
	if (true)
	{
		auto text = RmNew<RmGUIText>();
		openui->addWidget(text);
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
			case SDL_EVENT_QUIT: // 用户请求退出（如点击了窗口的关闭按钮）  
				// 执行退出操作（如关闭窗口、清理资源、结束程序）...  
				SDL_Quit();
				return 0;
			case SDL_EVENT_KEY_DOWN: // 键盘按键按下  
				// 处理键盘按键事件...  
				// 可以使用 event.key.sym 来获取按下的键的标识符
			{
				IRmGUIKeyDownEvent event2(event.key.key, event.key.mod, event.key.scancode, event.key.key, event.key.mod, RmString(), event.key.repeat);
				openui->sendEvent(nullptr, &event2);
			} break;
			case SDL_EVENT_KEY_UP: // 键盘按键释放  
				// 处理键盘按键释放事件...  
			{
				IRmGUIKeyUpEvent event2(event.key.key, event.key.mod, event.key.scancode, event.key.key, event.key.mod, RmString(), event.key.repeat);
				openui->sendEvent(nullptr, &event2);
			} break;
			case SDL_EVENT_TEXT_EDITING:
			{
				IRmGUITextInputEvent event2(event.key.key, event.key.mod, event.key.scancode, event.key.key, event.key.mod, event.edit.text, event.key.repeat, false, event.edit.start, event.edit.length);
				openui->sendEvent(nullptr, &event2);
			} break;
			case SDL_EVENT_TEXT_INPUT:
			{
				IRmGUITextInputEvent event2(event.key.key, event.key.mod, event.key.scancode, event.key.key, event.key.mod, event.edit.text, event.key.repeat, true);
				openui->sendEvent(nullptr, &event2);
			} break;
			case SDL_EVENT_MOUSE_MOTION: // 鼠标移动  
				// 处理鼠标移动事件...  
				// 可以使用 event.motion.x 和 event.motion.y 来获取鼠标的当前位置  
			{
				int x, y;
				SDL_GetWindowPosition(window, &x, &y);
				IRmGUIMouseMoveEvent event2(event.motion.x, event.motion.y, x + event.motion.x, y + event.motion.y, event.button.button, event.button.button, event.key.mod);
				openui->sendEvent(nullptr, &event2);
			} break;
			case SDL_EVENT_MOUSE_BUTTON_DOWN: // 鼠标点击  
				// 处理鼠标点击事件...  
				// 可以使用 event.button1.button1 来获取被点击的鼠标按钮的编号  
			{
				int x, y;
				SDL_GetWindowPosition(window, &x, &y);
				if (event.button.clicks == 1)
				{
					IRmGUIMouseDownEvent event2(event.motion.x, event.motion.y, x + event.motion.x, y + event.motion.y, event.button.button, event.button.button, event.key.mod, event.button.clicks);
					openui->sendEvent(nullptr, &event2);
				}
				else
				{
					IRmGUIMouseDblClickEvent event2(event.motion.x, event.motion.y, x + event.motion.x, y + event.motion.y, event.button.button, event.button.button, event.key.mod, event.button.clicks);
					openui->sendEvent(nullptr, &event2);
				}
			} break;
			case SDL_EVENT_MOUSE_BUTTON_UP: // 鼠标按钮释放  
				// 处理鼠标按钮释放事件...  
			{
				int x, y;
				SDL_GetWindowPosition(window, &x, &y);
				IRmGUIMouseUpEvent event2(event.motion.x, event.motion.y, x + event.motion.x, y + event.motion.y, event.button.button, event.button.button, event.key.mod);
				openui->sendEvent(nullptr, &event2);
			} break;
			case SDL_EVENT_MOUSE_WHEEL: // 鼠标滚轮
			{
				int x, y;
				SDL_GetWindowPosition(window, &x, &y);
				IRmGUIMouseWheelEvent event2(event.wheel.x, event.wheel.y, event.wheel.x, event.wheel.y, event.wheel.mouse_x, event.wheel.mouse_y, x + event.wheel.mouse_x, y + event.wheel.mouse_y, event.button.button, event.button.button, event.key.mod);
				openui->sendEvent(nullptr, &event2);
			} break;
			case SDL_EVENT_WINDOW_MOUSE_ENTER:
			{
				int x, y;
				SDL_GetWindowPosition(window, &x, &y);
				IRmGUIMouseEnterEvent event2(event.motion.x, event.motion.y, x + event.motion.x, y + event.motion.y, event.button.button, event.button.button, event.key.mod);
				openui->sendEvent(nullptr, &event2);
			} break;
			case SDL_EVENT_WINDOW_MOUSE_LEAVE:
			{
				int x, y;
				SDL_GetWindowPosition(window, &x, &y);
				IRmGUIMouseLeaveEvent event2(event.motion.x, event.motion.y, x + event.motion.x, y + event.motion.y, event.button.button, event.button.button, event.key.mod);
				openui->sendEvent(nullptr, &event2);
			} break;
			case SDL_EVENT_WINDOW_SHOWN:
			{
				IRmGUIShowEvent event2;
				openui->sendEvent(nullptr, &event2);
			} break;
			case SDL_EVENT_WINDOW_HIDDEN:
			{
				IRmGUIHideEvent event2;
				openui->sendEvent(nullptr, &event2);
			} break;
			case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
			{
				IRmGUICloseEvent event2;
				openui->sendEvent(nullptr, &event2);
			} break;
			case SDL_EVENT_WINDOW_MOVED:
			{
				IRmGUIMoveEvent event2(event.window.data1, event.window.data2);
				openui->sendEvent(nullptr, &event2);
			} break;
			case SDL_EVENT_WINDOW_RESIZED:
				// 窗口大小改变  
				// event.window.data1 是新的宽度  
				// event.window.data2 是新的高度  
				// printf("Window resized to %dx%d\n", event.window.data1, event.window.data2);
			{
				painter->resize(event.window.data1, event.window.data2);

				IRmGUIResizeEvent event2(event.window.data1, event.window.data2);
				openui->sendEvent(nullptr, &event2);
			} break;
			case SDL_EVENT_WINDOW_MINIMIZED:
				// 窗口最小化  
				// printf("Window minimized\n");
				break;
			case SDL_EVENT_WINDOW_MAXIMIZED:
				// 窗口最大化  
				// printf("Window maximized\n");
				break;
			case SDL_EVENT_WINDOW_RESTORED:
				// 窗口从最小化或最大化状态恢复  
				// printf("Window restored\n");
				break;
			case SDL_EVENT_WINDOW_FOCUS_GAINED:
			{
				IRmGUIFocusEvent event2(true);
				openui->sendEvent(nullptr, &event2);
			} break;
			case SDL_EVENT_WINDOW_FOCUS_LOST:
			{
				IRmGUIFocusEvent event2(false);
				openui->sendEvent(nullptr, &event2);
			} break;
			default:
				// 忽略未处理的事件  
				break;
			}
		}

		// 渲染所有控件

		int w, h;
		SDL_GetWindowSize(window, &w, &h);
		RmRect client{ 0, 0, (float)w, (float)h };
		openui->layoutWidget(client);
		openui->paintWidget(client);

		// 更新屏幕内容  
		SDL_GL_MakeCurrent(window, context);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_SCISSOR_TEST);
		glClearColor(0.8, 0.8, 0.8, 1);
		glClear(GL_COLOR_BUFFER_BIT);
		glViewport((int32_t)client.X, (int32_t)client.Y, (int32_t)client.W, (int32_t)client.H);
		openui->renderWidget(client);
		SDL_GL_SwapWindow(window);
	}

	render = nullptr;
	painter = nullptr;
	context = nullptr;

	// 清理并退出SDL  
	//SDL_DestroyTexture(texture);
	//SDL_DestroyRenderer(renderer);
	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}