#include <GL/glew.h>
#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "SDL3InputEnum.h"
#include "Cairo/CairoGLPainter.h"
#include "Cairo/CairoGLRender.h"
#include "OpenUI/UIContext.h"
#include "OpenUI/UIBuilder.h"
#include "OpenUI/UIHBox.h"
#include "OpenUI/UIVBox.h"
#include "OpenUI/UIFlow.h"
#include "OpenUI/UIScroll.h"
#include "OpenUI/UIGrid.h"
#include "OpenUI/UILabel.h"
#include "OpenUI/UIButton.h"
#include "OpenUI/UISlider.h"
#include "OpenUI/UIRadio.h"
#include "OpenUI/UICheck.h"
#include "OpenUI/UICombo.h"
#include "OpenUI/UILine.h"
#include "OpenUI/UIInput.h"

void sample(UIContextRef context, SDL_Window* window);

int main()
{
	// Create SDL window and OpenGL context

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		UI_ERROR("SDL could not initialize! SDL_Error: %s", SDL_GetError());
		return -1;
	}

	SDL_SetHint(SDL_HINT_IME_IMPLEMENTED_UI, "1");
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	auto window = SDL_CreateWindow("https://github.com/ChivenZhang/OpenUI.git", 1000, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if (window == nullptr)
	{
		UI_ERROR("Window could not be created! SDL_Error: %s", SDL_GetError());
		SDL_Quit();
		return -1;
	}
	auto context = SDL_GL_CreateContext(window);
	if (context == nullptr)
	{
		UI_ERROR("OpenGL context could not be created! SDL_Error: %s", SDL_GetError());
		SDL_DestroyWindow(window);
		SDL_Quit();
		return -1;
	}
	SDL_GL_SetSwapInterval(1);
	auto glewInitResult = glewInit();
	if (GLEW_OK != glewInitResult)
	{
		UI_ERROR("GLEW could not be initialized! %s", glewGetErrorString(glewInitResult));
		SDL_GL_DeleteContext(context);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return -1;
	}
	int w, h;
	SDL_GetWindowSize(window, &w, &h);
	auto scale = SDL_GetWindowDisplayScale(window);
	UI_INFO("%f", scale);

	// Initialize OpenUI context

	UIConfig config{ .DisplayScale = scale };
	auto openui = UINew<UIContext>(config);
	openui->setRender(UINew<CairoGLRender>());
	openui->setPainter(UINew<CairoGLPainter>(w, h));

	// Run sample in event loop

	sample(openui, window);

	bool running = true;
	while (running)
	{
		// Send events to OpenUI

		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_EVENT_QUIT:
			{
				running = false;
			} break;
			case SDL_EVENT_KEY_DOWN:
			{
				UIKeyDownEvent event2(SDL3InputEnum::GetKeyboardEnum(event.key.key), SDL3InputEnum::GetModifierEnum(event.key.mod), event.key.scancode, event.key.key, event.key.mod, UIString(), event.key.repeat);
				openui->sendEvent(nullptr, &event2);
			} break;
			case SDL_EVENT_KEY_UP:
			{
				UIKeyUpEvent event2(SDL3InputEnum::GetKeyboardEnum(event.key.key), SDL3InputEnum::GetModifierEnum(event.key.mod), event.key.scancode, event.key.key, event.key.mod, UIString(), event.key.repeat);
				openui->sendEvent(nullptr, &event2);
			} break;
			case SDL_EVENT_TEXT_EDITING:
			{
				UITextInputEvent event2(SDL3InputEnum::GetKeyboardEnum(event.key.key), SDL3InputEnum::GetModifierEnum(event.key.mod), event.key.scancode, event.key.key, event.key.mod, event.edit.text, event.key.repeat, false, event.edit.start, event.edit.length);
				openui->sendEvent(nullptr, &event2);
			} break;
			case SDL_EVENT_TEXT_INPUT:
			{
				UITextInputEvent event2(SDL3InputEnum::GetKeyboardEnum(event.key.key), SDL3InputEnum::GetModifierEnum(event.key.mod), event.key.scancode, event.key.key, event.key.mod, event.edit.text, event.key.repeat, true);
				openui->sendEvent(nullptr, &event2);
			} break;
			case SDL_EVENT_MOUSE_MOTION:
			{
				int x, y;
				SDL_GetWindowPosition(window, &x, &y);
				UIMouseMoveEvent event2(event.motion.x, event.motion.y, x + event.motion.x, y + event.motion.y, SDL3InputEnum::GetMouseEnum(event.button.button), SDL3InputEnum::GetMouseEnum(event.button.button), SDL3InputEnum::GetModifierEnum(event.key.mod));
				openui->sendEvent(nullptr, &event2);
			} break;
			case SDL_EVENT_MOUSE_BUTTON_DOWN:
			{
				int x, y;
				SDL_GetWindowPosition(window, &x, &y);
				if (event.button.clicks == 1)
				{
					UIMouseDownEvent event2(event.motion.x, event.motion.y, x + event.motion.x, y + event.motion.y, SDL3InputEnum::GetMouseEnum(event.button.button), SDL3InputEnum::GetMouseEnum(event.button.button), SDL3InputEnum::GetModifierEnum(event.key.mod), event.button.clicks);
					openui->sendEvent(nullptr, &event2);
				}
				else
				{
					UIMouseDblClickEvent event2(event.motion.x, event.motion.y, x + event.motion.x, y + event.motion.y, SDL3InputEnum::GetMouseEnum(event.button.button), SDL3InputEnum::GetMouseEnum(event.button.button), SDL3InputEnum::GetModifierEnum(event.key.mod), event.button.clicks);
					openui->sendEvent(nullptr, &event2);
				}
			} break;
			case SDL_EVENT_MOUSE_BUTTON_UP:
			{
				int x, y;
				SDL_GetWindowPosition(window, &x, &y);
				UIMouseUpEvent event2(event.motion.x, event.motion.y, x + event.motion.x, y + event.motion.y, SDL3InputEnum::GetMouseEnum(event.button.button), SDL3InputEnum::GetMouseEnum(event.button.button), SDL3InputEnum::GetModifierEnum(event.key.mod));
				openui->sendEvent(nullptr, &event2);
			} break;
			case SDL_EVENT_MOUSE_WHEEL:
			{
				int x, y;
				SDL_GetWindowPosition(window, &x, &y);
				UIMouseWheelEvent event2(event.wheel.x, event.wheel.y, event.wheel.x, event.wheel.y, event.wheel.mouse_x, event.wheel.mouse_y, x + event.wheel.mouse_x, y + event.wheel.mouse_y, SDL3InputEnum::GetMouseEnum(event.button.button), SDL3InputEnum::GetMouseEnum(event.button.button), SDL3InputEnum::GetModifierEnum(event.key.mod));
				openui->sendEvent(nullptr, &event2);
			} break;
			case SDL_EVENT_WINDOW_MOUSE_ENTER:
			{
				int x, y;
				SDL_GetWindowPosition(window, &x, &y);
				UIMouseEnterEvent event2(event.motion.x, event.motion.y, x + event.motion.x, y + event.motion.y, SDL3InputEnum::GetMouseEnum(event.button.button), SDL3InputEnum::GetMouseEnum(event.button.button), SDL3InputEnum::GetModifierEnum(event.key.mod));
				openui->sendEvent(nullptr, &event2);
			} break;
			case SDL_EVENT_WINDOW_MOUSE_LEAVE:
			{
				int x, y;
				SDL_GetWindowPosition(window, &x, &y);
				UIMouseLeaveEvent event2(event.motion.x, event.motion.y, x + event.motion.x, y + event.motion.y, SDL3InputEnum::GetMouseEnum(event.button.button), SDL3InputEnum::GetMouseEnum(event.button.button), SDL3InputEnum::GetModifierEnum(event.key.mod));
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
			case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
			{
				openui->layoutElement();
				openui->getPainter()->resize(event.window.data1, event.window.data2);
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

		// Update layout and paint

		int w, h;
		SDL_GetWindowSize(window, &w, &h);
		UIRect client{ 0, 0, (float)w, (float)h };
		openui->animateElement(::clock() * 0.001f);
		openui->layoutElement(client);
		openui->paintElement(client);

		// Output frame to screen

		SDL_GL_MakeCurrent(window, context);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_SCISSOR_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glClearColor(0.8, 0.8, 0.8, 1);
		glClear(GL_COLOR_BUFFER_BIT);
		glViewport((int32_t)client.X, (int32_t)client.Y, (int32_t)client.W, (int32_t)client.H);
		openui->renderElement(client);
		SDL_GL_SwapWindow(window);
	}

	// Clean up OpenUI and SDL

	openui = nullptr;
	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}

void sample(UIContextRef context, SDL_Window* window)
{
	auto scale = context->getConfig().DisplayScale;

	auto layout = UINew<UIFlow>();
	context->addElement(layout);

	//if (false)
	{
		auto scroll = UINew<UIScroll>();
		layout->addElement(scroll);
		scroll->setFixedSize(300 * scale, 200 * scale);
		scroll->setHorizontalValue(150);
		scroll->setVerticallValue(150);
		//if (false)
		{
			auto label = UINew<UILabel>();
			scroll->addElement(label);
			label->setFixedSize(300 * scale, 300 * scale);
			label->setScaledContents(UILabel::ScaleKeepRatio);
			int img_width, img_height, channels;
			auto image_data = stbi_load("../../OpenUI.png", &img_width, &img_height, &channels, 4);
			if (image_data) label->setPixmap(UIImage{
							(uint32_t)img_width, (uint32_t)img_height, (uint32_t)(img_width * 4), 0, image_data });
			stbi_image_free(image_data);
		}
	}
	//if (false)
	{
		auto vbox = UINew<UIVBox>();
		layout->addElement(vbox);
		vbox->setFixedSize(200 * scale, 200 * scale);
		{
			auto button = UINew<UIButton>();
			vbox->addElement(button);
			button->setText("Button0");
		}
		{
			auto button = UINew<UIButton>();
			vbox->addElement(button);
			button->setText("Button1");
		}
		{
			auto button = UINew<UIButton>();
			vbox->addElement(button);
			button->setText("Button2");
		}
		{
			auto button = UINew<UIButton>();
			vbox->addElement(button);
			button->setText("Button3");
		}
	}
	//if (false)
	{
		auto hbox = UINew<UIHBox>();
		layout->addElement(hbox);
		hbox->setFixedSize(250 * scale, 200 * scale);
		{
			auto button = UINew<UIButton>();
			hbox->addElement(button);
			button->setText("Button0");
		}
		{
			auto button = UINew<UIButton>();
			hbox->addElement(button);
			button->setText("Button1");
		}
		{
			auto button = UINew<UIButton>();
			hbox->addElement(button);
			button->setText("Button2");
		}
		{
			auto button = UINew<UIButton>();
			hbox->addElement(button);
			button->setText("Button3");
		}
	}
	//if (false)
	{
		auto grid = UINew<UIGrid>();
		layout->addElement(grid);
		grid->setRowStretch({ 1,1,1 });
		grid->setColumnStretch({ 1,1,1 });
		grid->setFixedSize(200 * scale, 200 * scale);
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
		group->setFixedSize(200 * scale, 200 * scale);
		{
			auto radio = UINew<UIRadio>();
			group->addElement(radio);
			radio->setFixedSize(100 * scale, 30 * scale);
			radio->setText("Radio");
			radio->setChecked(true);

			auto radio0 = UINew<UIRadio>();
			group->addElement(radio0);
			radio0->setFixedSize(100 * scale, 30 * scale);
			radio0->setText("Radio");
			radio0->setExclusive(radio->getExclusive());

			auto radio1 = UINew<UIRadio>();
			group->addElement(radio1);
			radio1->setFixedSize(100 * scale, 30 * scale);
			radio1->setText("Radio");
			radio1->setExclusive(radio->getExclusive());
		}
		{
			auto check = UINew<UICheck>();
			group->addElement(check);
			check->setFixedSize(100 * scale, 30 * scale);
			check->setText("Check");
			check->setChecked(true);
		}
		{
			auto check = UINew<UICheck>();
			group->addElement(check);
			check->setFixedSize(100 * scale, 30 * scale);
			check->setText("Check");
		}
	}
	//if (false)
	{
		auto label = UINew<UILabel>();
		layout->addElement(label);
		label->setFixedSize(100 * scale, 30 * scale);
		label->setText("Label");
	}
	//if (false)
	{
		auto button = UINew<UIButton>();
		layout->addElement(button);
		button->setFixedSize(100 * scale, 30 * scale);
		button->setText("Button");
	}
	//if (false)
	{
		auto slider = UINew<UISlider>();
		layout->addElement(slider);
		slider->setFixedSize(100 * scale, 30 * scale);
		slider->setRange(0, 100 * scale);
		slider->setValue(25 * scale);
	}
	//if (false)
	{
		auto slider = UINew<UISlider>();
		layout->addElement(slider);
		slider->setOrientation(UI::Vertical);
		slider->setFixedSize(30 * scale, 100 * scale);
		slider->setRange(0, 100 * scale);
		slider->setValue(25 * scale);
	}
	//if(false)
	{
		auto hline = UINew<UIHLine>();
		layout->addElement(hline);
		hline->setFixedSize(100 * scale, 30 * scale);
	}
	//if(false)
	{
		auto vline = UINew<UIVLine>();
		layout->addElement(vline);
		vline->setFixedSize(30 * scale, 100 * scale);
	}
	//if (false)
	{
		auto combo = UINew<UICombo>();
		layout->addElement(combo);
		combo->setFixedSize(100 * scale, 30 * scale);
		combo->setMaxCount(4);
		combo->setItems({ "黄金糕狮子头螺蛳粉", "黄金糕", "狮子头", "螺蛳粉", "蚵仔煎", "双皮奶", "龙须面" });
		combo->setCurrentIndex(0);
		combo->currentTextChanged->connect(nullptr, [](UIString text) {
			printf("combo1 %s\n", text.c_str());
			});

		auto combo2 = UINew<UICombo>();
		layout->addElement(combo2);
		combo2->setFixedSize(100 * scale, 30 * scale);
		combo2->setItems({ "黄金糕狮子头螺蛳粉", "黄金糕", "狮子头", "螺蛳粉", "蚵仔煎", "双皮奶", "龙须面" });
		combo2->setCurrentText("黄金糕");
		combo2->currentTextChanged->connect(nullptr, [](UIString text) {
			printf("combo2 %s\n", text.c_str());
			});
	}
	// if(false)
	{
		auto input = UINew<UIInput>();
		layout->addElement(input);
		input->setFixedSize(100 * scale, 30 * scale);
		input->setText("Hello,OpenUI");
		input->editingStarted->connect(nullptr, [=](UIRect rect) {
			SDL_StartTextInput(window);
			SDL_Rect sdlRect{ rect.X, rect.Y, rect.W, rect.H };
			SDL_SetTextInputArea(window, &sdlRect, 0);
			});
		input->textPasted->connect(nullptr, [=](UIString& value) {
			if (SDL_HasClipboardText())
			{
				auto result = SDL_GetClipboardText();
				value = UIString(result);
				SDL_free(result);
			}
			});
		input->textCopied->connect(nullptr, [=](UIString const& value) {
			SDL_ClearClipboardData();
			SDL_SetClipboardText(value.c_str());
			});
	}
}