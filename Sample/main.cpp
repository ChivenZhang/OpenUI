#include <GL/glew.h>
#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "OpenGLPainter.h"
#include "OpenGLRender.h"
#include "OpenUI/UIContext.h"
#include "OpenUI/UIBuilder.h"
#include "OpenUI/Element/UIHBox.h"
#include "OpenUI/Element/UIVBox.h"
#include "OpenUI/Element/UIFlow.h"
#include "OpenUI/Element/UIScroll.h"
#include "OpenUI/Element/UIGrid.h"
#include "OpenUI/Element/UILabel.h"
#include "OpenUI/Element/UIButton.h"
#include "OpenUI/Element/UISlider.h"
#include "OpenUI/Element/UIRadio.h"
#include "OpenUI/Element/UICheck.h"
#include "OpenUI/Element/UICombo.h"
static int count = 0;

void sample(UIContextRef context, SDL_Window* window);

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
	auto openui = UINew<UIContext>();
	auto painter = UINew<OpenGLPainter>(w, h);
	auto render = UINew<OpenGLRender>();
	openui->setPainter(painter);
	openui->setRender(render);

	sample(openui, window);

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
				UIKeyDownEvent event2(event.key.key, event.key.mod, event.key.scancode, event.key.key, event.key.mod, UIString(), event.key.repeat);
				openui->sendEvent(nullptr, &event2);
			} break;
			case SDL_EVENT_KEY_UP:
			{
				UIKeyUpEvent event2(event.key.key, event.key.mod, event.key.scancode, event.key.key, event.key.mod, UIString(), event.key.repeat);
				openui->sendEvent(nullptr, &event2);
			} break;
			case SDL_EVENT_TEXT_EDITING:
			{
				UITextInputEvent event2(event.key.key, event.key.mod, event.key.scancode, event.key.key, event.key.mod, event.edit.text, event.key.repeat, false, event.edit.start, event.edit.length);
				openui->sendEvent(nullptr, &event2);
			} break;
			case SDL_EVENT_TEXT_INPUT:
			{
				UITextInputEvent event2(event.key.key, event.key.mod, event.key.scancode, event.key.key, event.key.mod, event.edit.text, event.key.repeat, true);
				openui->sendEvent(nullptr, &event2);
			} break;
			case SDL_EVENT_MOUSE_MOTION:
			{
				int x, y;
				SDL_GetWindowPosition(window, &x, &y);
				UIMouseMoveEvent event2(event.motion.x, event.motion.y, x + event.motion.x, y + event.motion.y, event.button.button, event.button.button, event.key.mod);
				openui->sendEvent(nullptr, &event2);
			} break;
			case SDL_EVENT_MOUSE_BUTTON_DOWN:
			{
				int x, y;
				SDL_GetWindowPosition(window, &x, &y);
				if (event.button.clicks == 1)
				{
					UIMouseDownEvent event2(event.motion.x, event.motion.y, x + event.motion.x, y + event.motion.y, event.button.button, event.button.button, event.key.mod, event.button.clicks);
					openui->sendEvent(nullptr, &event2);
				}
				else
				{
					UIMouseDblClickEvent event2(event.motion.x, event.motion.y, x + event.motion.x, y + event.motion.y, event.button.button, event.button.button, event.key.mod, event.button.clicks);
					openui->sendEvent(nullptr, &event2);
				}
			} break;
			case SDL_EVENT_MOUSE_BUTTON_UP:
			{
				int x, y;
				SDL_GetWindowPosition(window, &x, &y);
				UIMouseUpEvent event2(event.motion.x, event.motion.y, x + event.motion.x, y + event.motion.y, event.button.button, event.button.button, event.key.mod);
				openui->sendEvent(nullptr, &event2);
			} break;
			case SDL_EVENT_MOUSE_WHEEL:
			{
				int x, y;
				SDL_GetWindowPosition(window, &x, &y);
				UIMouseWheelEvent event2(event.wheel.x, event.wheel.y, event.wheel.x, event.wheel.y, event.wheel.mouse_x, event.wheel.mouse_y, x + event.wheel.mouse_x, y + event.wheel.mouse_y, event.button.button, event.button.button, event.key.mod);
				openui->sendEvent(nullptr, &event2);
			} break;
			case SDL_EVENT_WINDOW_MOUSE_ENTER:
			{
				int x, y;
				SDL_GetWindowPosition(window, &x, &y);
				UIMouseEnterEvent event2(event.motion.x, event.motion.y, x + event.motion.x, y + event.motion.y, event.button.button, event.button.button, event.key.mod);
				openui->sendEvent(nullptr, &event2);
			} break;
			case SDL_EVENT_WINDOW_MOUSE_LEAVE:
			{
				int x, y;
				SDL_GetWindowPosition(window, &x, &y);
				UIMouseLeaveEvent event2(event.motion.x, event.motion.y, x + event.motion.x, y + event.motion.y, event.button.button, event.button.button, event.key.mod);
				openui->sendEvent(nullptr, &event2);
			} break;
			case SDL_EVENT_WINDOW_SHOWN:
			{
				UIShowEvent event2;
				openui->sendEvent(nullptr, &event2);
			} break;
			case SDL_EVENT_WINDOW_HIDDEN:
			{
				UIHideEvent event2;
				openui->sendEvent(nullptr, &event2);
			} break;
			case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
			{
				UICloseEvent event2;
				openui->sendEvent(nullptr, &event2);
			} break;
			case SDL_EVENT_WINDOW_MOVED:
			{
				UIMoveEvent event2(event.window.data1, event.window.data2);
				openui->sendEvent(nullptr, &event2);
			} break;
			case SDL_EVENT_WINDOW_RESIZED:
			{
				painter->resize(event.window.data1, event.window.data2);
				UIResizeEvent event2(event.window.data1, event.window.data2);
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
				UIFocusEvent event2(true);
				openui->sendEvent(nullptr, &event2);
			} break;
			case SDL_EVENT_WINDOW_FOCUS_LOST:
			{
				UIFocusEvent event2(false);
				openui->sendEvent(nullptr, &event2);
			} break;
			default:
				break;
			}
		}

		int w, h;
		SDL_GetWindowSize(window, &w, &h);
		UIRect client{ 0, 0, (float)w, (float)h };
		openui->layoutElement(client);
		openui->paintElement(client);
		openui->animateElement(::clock() * 0.001f);

		SDL_GL_MakeCurrent(window, context);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_SCISSOR_TEST);
		glClearColor(0.8, 0.8, 0.8, 1);
		glClear(GL_COLOR_BUFFER_BIT);
		glViewport((int32_t)client.X, (int32_t)client.Y, (int32_t)client.W, (int32_t)client.H);
		openui->renderElement(client);
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

void sample(UIContextRef context, SDL_Window* window)
{
	auto layout = UINew<UIFlow>();
	context->addElement(layout);

	//if (false)
	{
		auto scroll = UINew<UIScroll>();
		layout->addElement(scroll);
		scroll->setFixedSize(200, 200);
		scroll->setHorizontalValue(150);
		scroll->setVerticallValue(150);
		//if (false)
		{
			auto label = UINew<UILabel>();
			scroll->addElement(label);
			label->setFixedSize(300, 300);
			label->setScaledContents(UILabel::ScaleKeepRatio);
			int img_width, img_height, channels;
			auto image_data = stbi_load("../../../OpenUI.png", &img_width, &img_height, &channels, 4);
			if (image_data) label->setPixmap({
							(uint32_t)img_width, (uint32_t)img_height, (uint32_t)(img_width * 4),
							UIArrayView<const uint8_t>(image_data, img_height * img_width * 4) });
			stbi_image_free(image_data);
		}
	}
	//if (false)
	{
		auto grid = UINew<UIGrid>();
		layout->addElement(grid);
		grid->setRowStretch({ 1,1,1 });
		grid->setColumnStretch({ 1,1,1 });
		grid->setFixedSize(200, 200);
		{
			auto button = UINew<UIButton>();
			grid->addElement(button, 0, 0, 2, 2);
			button->setText("Button0");
		}
		{
			auto button = UINew<UIButton>();
			grid->addElement(button, 0, 2, 3, 1);
			button->setText("Button1");
		}
		{
			auto button = UINew<UIButton>();
			grid->addElement(button, 2, 0, 1, 1);
			button->setText("Button2");
		}
		{
			auto button = UINew<UIButton>();
			grid->addElement(button, 2, 1, 1, 1);
			button->setText("Button3");
		}
	}
	//if (false)
	{
		auto group = UINew<UIVBox>();
		layout->addElement(group);
		group->setFixedSize(200, 200);
		{
			auto radio = UINew<UIRadio>();
			group->addElement(radio);
			radio->setFixedSize(100, 30);
			radio->setText("Radio");

			auto radio0 = UINew<UIRadio>();
			group->addElement(radio0);
			radio0->setFixedSize(100, 30);
			radio0->setText("Radio");
			radio0->setExclusive(radio->getExclusive());

			auto radio1 = UINew<UIRadio>();
			group->addElement(radio1);
			radio1->setFixedSize(100, 30);
			radio1->setText("Radio");
			radio1->setChecked(true);
			radio1->setExclusive(radio->getExclusive());

		}
		{
			auto check = UINew<UICheck>();
			group->addElement(check);
			check->setFixedSize(100, 30);
			check->setText("Check");
		}
		{
			auto check = UINew<UICheck>();
			group->addElement(check);
			check->setFixedSize(100, 30);
			check->setText("Check");
			check->setChecked(true);
		}
	}
	//if (false)
	{
		auto label = UINew<UILabel>();
		layout->addElement(label);
		label->setFixedSize(100, 30);
		label->setText("Label");
	}
	//if (false)
	{
		auto button = UINew<UIButton>();
		layout->addElement(button);
		button->setFixedSize(100, 30);
		button->setText("Button");
	}
	//if (false)
	{
		auto slider = UINew<UISlider>();
		layout->addElement(slider);
		slider->setFixedSize(100, 30);
		slider->setRange(0, 100);
		slider->setValue(25);
	}
	//if (false)
	{
		auto slider = UINew<UISlider>();
		layout->addElement(slider);
		slider->setOrientation(UI::Vertical);
		slider->setFixedSize(30, 100);
		slider->setRange(0, 100);
		slider->setValue(25);
	}
	if (false)
	{
		auto combo = UINew<UICombo>();
		layout->addElement(combo);
		combo->setFixedSize(100, 30);
	}
}