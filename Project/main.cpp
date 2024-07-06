#include <GL/glew.h>
#define SDL_MAIN_HANDLED
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

void sample0(RmGUIContextRef openui, SDL_Window* window);
void sample1(RmGUIContextRef openui, SDL_Window* window);
void sample2(RmGUIContextRef openui, SDL_Window* window);
void sample3(RmGUIContextRef openui, SDL_Window* window);

int main(int argc, char* argv[])
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
		return -1;
	}
	SDL_SetHint(SDL_HINT_IME_IMPLEMENTED_UI, "1");
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	auto window = SDL_CreateWindow("https://github.com/ChivenZhang/OpenUI.git", 1000, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if (window == nullptr)
	{
		std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return -1;
	}
	auto context = SDL_GL_CreateContext(window);
	if (context == nullptr)
	{
		std::cerr << "OpenGL context could not be created! SDL_Error: " << SDL_GetError() << std::endl;
		SDL_DestroyWindow(window);
		SDL_Quit();
		return -1;
	}
	SDL_GL_SetSwapInterval(1);
	auto glewInitResult = glewInit();
	if (GLEW_OK != glewInitResult)
	{
		std::cerr << "GLEW could not be initialized! " << glewGetErrorString(glewInitResult) << std::endl;
		SDL_GL_DeleteContext(context);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return -1;
	}

	int w, h;
	SDL_GetWindowSize(window, &w, &h);
	auto openui = RmNew<RmGUIContext>();
	auto painter = RmNew<OpenGLPainter>(w, h);
	auto render = RmNew<OpenGLRender>();
	openui->setPainter(painter);
	openui->setRender(render);

	//sample0(openui, window);
	//sample1(openui, window);
	sample2(openui, window);
	//sample3(openui, window);

	SDL_Event event;
	bool quit = false;
	while (!quit)
	{
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_EVENT_QUIT:
				SDL_Quit();
				return 0;
			case SDL_EVENT_KEY_DOWN:
			{
				IRmGUIKeyDownEvent event2(event.key.key, event.key.mod, event.key.scancode, event.key.key, event.key.mod, RmString(), event.key.repeat);
				openui->sendEvent(nullptr, &event2);
			} break;
			case SDL_EVENT_KEY_UP:
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
			{
				int x, y;
				SDL_GetWindowPosition(window, &x, &y);
				IRmGUIMouseMoveEvent event2(event.motion.x, event.motion.y, x + event.motion.x, y + event.motion.y, event.button.button, event.button.button, event.key.mod);
				openui->sendEvent(nullptr, &event2);
			} break;
			case SDL_EVENT_MOUSE_BUTTON_DOWN: // 鼠标点击  
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
			case SDL_EVENT_MOUSE_BUTTON_UP:
			{
				int x, y;
				SDL_GetWindowPosition(window, &x, &y);
				IRmGUIMouseUpEvent event2(event.motion.x, event.motion.y, x + event.motion.x, y + event.motion.y, event.button.button, event.button.button, event.key.mod);
				openui->sendEvent(nullptr, &event2);
			} break;
			case SDL_EVENT_MOUSE_WHEEL:
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
			{
				painter->resize(event.window.data1, event.window.data2);
				IRmGUIResizeEvent event2(event.window.data1, event.window.data2);
				openui->sendEvent(nullptr, &event2);
			} break;
			case SDL_EVENT_WINDOW_MINIMIZED:
				break;
			case SDL_EVENT_WINDOW_MAXIMIZED:
				break;
			case SDL_EVENT_WINDOW_RESTORED:
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
				break;
			}
		}

		int w, h;
		SDL_GetWindowSize(window, &w, &h);
		RmRect client{ 0, 0, (float)w, (float)h };
		openui->layoutWidget(client);
		openui->paintWidget(client);

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
	openui = nullptr;
	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}

void sample0(RmGUIContextRef openui, SDL_Window* window)
{
	auto toplevel = RmNew<RmGUIHBox>();
	openui->addWidget(toplevel);

	auto child0 = RmNew<RmGUIVBox>();
	toplevel->addWidget(child0);
	child0->setBorder({ 5,5,5,5 });

	{
		auto combo = RmNew<RmGUICombo>();
		child0->addWidget(combo);
		combo->setPosition(100, 100);
		combo->setFixedSize(100, 35);
		combo->setMaxCount(3);
		combo->setItems({ "White", "Red", "Green", "Blue", "Black" });
		combo->setCurrentIndex(0);
		combo->currentTextChanged->connect(nullptr, [](RmString text) { printf("combo0 %s\n", text.c_str()); });

		auto combo2 = RmNew<RmGUICombo>();
		child0->addWidget(combo2);
		combo2->setPosition(200, 100);
		combo2->setFixedSize(100, 35);
		combo2->setItems({ "壹个", "贰个", "叁个", "肆个", "伍个", "陆个" });
		combo2->setCurrentIndex(0);
		combo2->currentTextChanged->connect(nullptr, [](RmString text) { printf("combo2 %s\n", text.c_str()); });

		auto combo3 = RmNew<RmGUICombo>();
		child0->addWidget(combo3);
		combo3->setPosition(200, 100);
		combo3->setFixedSize(100, 35);
		combo3->setItems({ "一月", "二月", "三月", "四月", "五月","六月", "七月", "八月", "九月", "十月", "十一月", "十二月", });
		combo3->setCurrentIndex(0);
		combo3->currentTextChanged->connect(nullptr, [](RmString text) { printf("combo3 %s\n", text.c_str()); });
	}

	auto child1 = RmNew<RmGUIHBox>();
	toplevel->addWidget(child1);
	child1->setBorder({ 5,5,5,5 });

	{
		auto combo = RmNew<RmGUICombo>();
		child1->addWidget(combo);
		combo->setPosition(100, 100);
		combo->setFixedSize(100, 35);
		combo->setMaxCount(3);
		combo->setItems({ "White", "Red", "Green", "Blue", "Black" });
		combo->setCurrentIndex(0);
		combo->currentTextChanged->connect(nullptr, [](RmString text) { printf("combo0 %s\n", text.c_str()); });

		auto combo2 = RmNew<RmGUICombo>();
		child1->addWidget(combo2);
		combo2->setPosition(200, 100);
		combo2->setFixedSize(100, 35);
		combo2->setItems({ "壹个", "贰个", "叁个", "肆个", "伍个", "陆个" });
		combo2->setCurrentIndex(0);
		combo2->currentTextChanged->connect(nullptr, [](RmString text) { printf("combo2 %s\n", text.c_str()); });

		auto combo3 = RmNew<RmGUICombo>();
		child1->addWidget(combo3);
		combo3->setPosition(200, 100);
		combo3->setFixedSize(100, 35);
		combo3->setItems({ "一月", "二月", "三月", "四月", "五月","六月", "七月", "八月", "九月", "十月", "十一月", "十二月", });
		combo3->setCurrentIndex(0);
		combo3->currentTextChanged->connect(nullptr, [](RmString text) { printf("combo3 %s\n", text.c_str()); });
	}
}

void sample1(RmGUIContextRef openui, SDL_Window* window)
{
	auto toplevel = RmNew<RmGUIVBox>();
	openui->addWidget(toplevel);

	auto layout = RmNew<RmGUIFlow>();
	toplevel->addWidget(layout);
	layout->setBorder({ 5,5,5,5 });

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
	layout->addWidget(button1);
	button1->setText("自减");
	button1->setFixedWidth(100); button1->setFixedHeight(35);
	button1->setMargin({ 5,5,5,5 });
	style = button1->getLabel()->getStyle();
	style.Brush.Color = { 0.5,0,1,1 };
	button1->getLabel()->setStyle(style);
	button1->clicked->connect(nullptr, [=](bool checked) {
		label->setText("计数:" + std::to_string(count = std::clamp(--count, 0, 10)));
		});

	layout->addWidget(label);

	auto button2 = RmNew<RmGUIButton>();
	layout->addWidget(button2);
	button2->setText("自增 ");
	button2->setFixedWidth(100); button2->setFixedHeight(35);
	button2->setMargin({ 5,5,5,5 });
	style = button2->getLabel()->getStyle();
	style.Brush.Color = { 0.5,0,1,1 };
	button2->getLabel()->setStyle(style);
	button2->clicked->connect(nullptr, [=](bool checked) {
		label->setText("计数:" + std::to_string(count = std::clamp(++count, 0, 10)));
		});

	auto button3 = RmNew<RmGUIButton>();
	layout->addWidget(button3);
	button3->setText("重置");
	button3->setFixedWidth(100); button3->setFixedHeight(35);
	button3->setMargin({ 5,5,5,5 });
	style = button3->getLabel()->getStyle();
	style.Brush.Color = { 0.5,0,1,1 };
	button3->getLabel()->setStyle(style);
	button3->clicked->connect(nullptr, [=](bool checked) {
		label->setText("计数:" + std::to_string(count = 0));
		});

	auto scroll = RmNew<RmGUIScroll>();
	toplevel->addWidget(scroll);
	scroll->setMinWidth(50); scroll->setMinHeight(50);
	scroll->setMargin({ 5,5,5,5 });

	auto panel = RmNew<RmGUIPanel>();
	scroll->addWidget(panel);
	panel->setFixedSize(2000, 2000);
}

void sample2(RmGUIContextRef openui, SDL_Window* window)
{
	auto toplevel = RmNew<RmGUIVBox>();
	openui->addWidget(toplevel);

	auto image = RmNew<RmGUILabel>();
	toplevel->addWidget(image);
	image->setBorder({ 5,5,5,5 });
	image->setScaledContents(RmGUILabel::ScaleKeepRatio);

	int img_width, img_height, channels;
	auto image_data = stbi_load("../../../Content/0.jpg", &img_width, &img_height, &channels, 4);
	if (image_data) image->setPixmap({
					(uint32_t)img_width, (uint32_t)img_height, (uint32_t)(img_width * 4),
					RmArrayView<const uint8_t>(image_data, img_height * img_width * 4) });
	stbi_image_free(image_data);

	auto menu = RmNew<RmGUIHBox>();
	openui->addWidget(menu);
	auto style = menu->getStyle();
	style.Brush = { .Style = RmBrush::NoBrush };
	menu->setStyle(style);

	RmArray<RmString, 5> paths
	{
		"../../../Content/0.jpg",
		"../../../Content/1.jpg",
		"../../../Content/2.jpg",
		"../../../Content/3.jpg",
		"../../../Content/4.jpg",
	};
	static int32_t index = 0;
	{
		auto left = RmNew<RmGUIButton>();
		menu->addWidget(left);
		left->setMargin({ 10,10,10,10 });
		left->setFixedSize(64, 100);
		auto style = left->getStyle();
		style.Round = { 8, 8 };
		style.Normal.Pen = style.Disable.Pen = { .Style = RmPen::NoPen };
		style.Normal.Brush = style.Disable.Brush = { .Style = RmBrush::NoBrush };
		left->setStyle(style);
		left->getLabel()->setScaledContents(RmGUILabel::ScaleKeepRatio);
		image_data = stbi_load("../../../Content/sort-left.png", &img_width, &img_height, &channels, 4);
		if (image_data) left->getLabel()->setPixmap({
						(uint32_t)img_width, (uint32_t)img_height, (uint32_t)(img_width * 4),
						RmArrayView<const uint8_t>(image_data, img_height * img_width * 4) });
		stbi_image_free(image_data);

		left->clicked->connect(nullptr, [=](bool) {
			index = (index + paths.size() - 1) % paths.size();
			int img_width, img_height, channels;
			auto image_data = stbi_load(paths[index].c_str(), &img_width, &img_height, &channels, 4);
			if (image_data) image->setPixmap({
							(uint32_t)img_width, (uint32_t)img_height, (uint32_t)(img_width * 4),
							RmArrayView<const uint8_t>(image_data, img_height * img_width * 4) });
			stbi_image_free(image_data);
			});
	}

	auto middle = RmNew<RmGUIPanel>();
	menu->addWidget(middle);

	{
		auto right = RmNew<RmGUIButton>();
		menu->addWidget(right);
		right->setMargin({ 10,10,10,10 });
		right->setFixedSize(64, 100);
		auto style = right->getStyle();
		style.Round = { 8, 8 };
		style.Normal.Pen = style.Disable.Pen = { .Style = RmPen::NoPen };
		style.Normal.Brush = style.Disable.Brush = { .Style = RmBrush::NoBrush };
		right->setStyle(style);
		right->getLabel()->setScaledContents(RmGUILabel::ScaleKeepRatio);
		image_data = stbi_load("../../../Content/sort-right.png", &img_width, &img_height, &channels, 4);
		if (image_data) right->getLabel()->setPixmap({
						(uint32_t)img_width, (uint32_t)img_height, (uint32_t)(img_width * 4),
						RmArrayView<const uint8_t>(image_data, img_height * img_width * 4) });
		stbi_image_free(image_data);

		right->clicked->connect(nullptr, [=](bool) {
			index = (index + 1) % paths.size();
			int img_width, img_height, channels;
			auto image_data = stbi_load(paths[index].c_str(), &img_width, &img_height, &channels, 4);
			if (image_data) image->setPixmap({
							(uint32_t)img_width, (uint32_t)img_height, (uint32_t)(img_width * 4),
							RmArrayView<const uint8_t>(image_data, img_height * img_width * 4) });
			stbi_image_free(image_data);
			});
	}
}

void sample3(RmGUIContextRef openui, SDL_Window* window)
{
	// TODO
}