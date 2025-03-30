#include <SDL3/SDL.h>
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
#include <stb_image.h>
#ifdef OPENUI_ENABLE_OPENGL
#include "OpenGL/SDL3GLDevice.h"
#endif
#ifdef OPENUI_ENABLE_VULKAN
#include <vulkan/vulkan.h>
#include <SDL3/SDL_vulkan.h>
#include "Vulkan/SDL3VKDevice.h"
#endif

void business(UIContextRaw context, SDL_Window* window);

int main()
{
	SDL_Init(SDL_INIT_VIDEO);
	SDL_SetHint(SDL_HINT_IME_IMPLEMENTED_UI, "1");

#ifdef OPENUI_ENABLE_OPENGL
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	{
		auto device = UINew<SDL3GLDevice>();
		auto openui = device->getContext();
		auto window = device->getWindow();
		business(openui, window);
		while (device->update());
		device = nullptr;
	}
#endif

#ifdef OPENUI_ENABLE_VULKAN
	VkApplicationInfo appInfo = {};
	VkInstanceCreateInfo instanceCreateInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.apiVersion = VK_API_VERSION_1_3;
	instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceCreateInfo.pApplicationInfo = &appInfo;
#ifdef __APPLE__
	instanceCreateInfo.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif
	instanceCreateInfo.ppEnabledExtensionNames = SDL_Vulkan_GetInstanceExtensions(&instanceCreateInfo.enabledExtensionCount);
	VkInstance instance = VK_NULL_HANDLE;
	vkCreateInstance(&instanceCreateInfo, nullptr, &instance);
	{
		auto device = UINew<SDL3VKDevice>(instance);
		auto openui = device->getContext();
		auto window = device->getWindow();
		business(openui, window);
		while (device->update());
		device = nullptr;
	}
	vkDestroyInstance(instance, nullptr);
#endif

	SDL_Quit();
	return 0;
}

void business(UIContextRaw context, SDL_Window* window)
{
	auto builder = context->getBuilder();
	auto scale = context->getConfig().DisplayScale;

	auto layout = builder->create<UIFlow>();
	context->addElement(layout);

	//if (false)
	{
		auto scroll = builder->create<UIScroll>();
		layout->addElement(scroll);
		scroll->setFixedSize(300 * scale, 200 * scale);
		scroll->setHorizontalValue(150);
		scroll->setVerticallValue(150);
		//if (false)
		{
			auto label = builder->create<UILabel>();
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
		auto vbox = builder->create<UIVBox>();
		layout->addElement(vbox);
		vbox->setFixedSize(200 * scale, 200 * scale);
		{
			auto button = builder->create<UIButton>();
			vbox->addElement(button);
			button->setText("Button0");
		}
		{
			auto button = builder->create<UIButton>();
			vbox->addElement(button);
			button->setText("Button1");
		}
		{
			auto button = builder->create<UIButton>();
			vbox->addElement(button);
			button->setText("Button2");
		}
		{
			auto button = builder->create<UIButton>();
			vbox->addElement(button);
			button->setText("Button3");
		}
	}
	//if (false)
	{
		auto hbox = builder->create<UIHBox>();
		layout->addElement(hbox);
		hbox->setFixedSize(250 * scale, 200 * scale);
		{
			auto button = builder->create<UIButton>();
			hbox->addElement(button);
			button->setText("Button0");
		}
		{
			auto button = builder->create<UIButton>();
			hbox->addElement(button);
			button->setText("Button1");
		}
		{
			auto button = builder->create<UIButton>();
			hbox->addElement(button);
			button->setText("Button2");
		}
		{
			auto button = builder->create<UIButton>();
			hbox->addElement(button);
			button->setText("Button3");
		}
	}
	//if (false)
	{
		auto grid = builder->create<UIGrid>();
		layout->addElement(grid);
		grid->setRowStretch({ 1,1,1 });
		grid->setColumnStretch({ 1,1,1 });
		grid->setFixedSize(200 * scale, 200 * scale);
		{
			auto button = builder->create<UIButton>();
			grid->addElement(button, 0, 0, 2, 2);
			button->setText("Button0");
		}
		{
			auto button = builder->create<UIButton>();
			grid->addElement(button, 0, 2, 3, 1);
			button->setText("Button1");
		}
		{
			auto button = builder->create<UIButton>();
			grid->addElement(button, 2, 0, 1, 1);
			button->setText("Button2");
		}
		{
			auto button = builder->create<UIButton>();
			grid->addElement(button, 2, 1, 1, 1);
			button->setText("Button3");
		}
	}
	//if (false)
	{
		auto group = builder->create<UIVBox>();
		layout->addElement(group);
		group->setFixedSize(200 * scale, 200 * scale);
		{
			auto radio = builder->create<UIRadio>();
			group->addElement(radio);
			radio->setFixedSize(100 * scale, 30 * scale);
			radio->setText("Radio");
			radio->setChecked(true);

			auto radio0 = builder->create<UIRadio>();
			group->addElement(radio0);
			radio0->setFixedSize(100 * scale, 30 * scale);
			radio0->setText("Radio");
			radio0->setExclusive(radio->getExclusive());

			auto radio1 = builder->create<UIRadio>();
			group->addElement(radio1);
			radio1->setFixedSize(100 * scale, 30 * scale);
			radio1->setText("Radio");
			radio1->setExclusive(radio->getExclusive());
		}
		{
			auto check = builder->create<UICheck>();
			group->addElement(check);
			check->setFixedSize(100 * scale, 30 * scale);
			check->setText("Check");
			check->setChecked(true);
		}
		{
			auto check = builder->create<UICheck>();
			group->addElement(check);
			check->setFixedSize(100 * scale, 30 * scale);
			check->setText("Check");
		}
	}
	//if (false)
	{
		auto label = builder->create<UILabel>();
		layout->addElement(label);
		label->setFixedSize(100 * scale, 30 * scale);
		label->setText("Label");
	}
	//if (false)
	{
		auto button = builder->create<UIButton>();
		layout->addElement(button);
		button->setFixedSize(100 * scale, 30 * scale);
		button->setText("Button");
	}
	//if (false)
	{
		auto slider = builder->create<UISlider>();
		layout->addElement(slider);
		slider->setFixedSize(100 * scale, 30 * scale);
		slider->setRange(0, 100 * scale);
		slider->setValue(25 * scale);
	}
	//if (false)
	{
		auto slider = builder->create<UISlider>();
		layout->addElement(slider);
		slider->setOrientation(UI::Vertical);
		slider->setFixedSize(30 * scale, 100 * scale);
		slider->setRange(0, 100 * scale);
		slider->setValue(25 * scale);
	}
	//if(false)
	{
		auto hline = builder->create<UIHLine>();
		layout->addElement(hline);
		hline->setFixedSize(100 * scale, 30 * scale);
	}
	//if(false)
	{
		auto vline = builder->create<UIVLine>();
		layout->addElement(vline);
		vline->setFixedSize(30 * scale, 100 * scale);
	}
	//if (false)
	{
		auto combo = builder->create<UICombo>();
		layout->addElement(combo);
		combo->setFixedSize(100 * scale, 30 * scale);
		combo->setMaxCount(4);
		combo->setItems({ "黄金糕狮子头螺蛳粉", "黄金糕", "狮子头", "螺蛳粉", "蚵仔煎", "双皮奶", "龙须面" });
		combo->setCurrentIndex(0);
		combo->currentTextChanged->connect(nullptr, [](UIString text) {
			printf("combo1 %s\n", text.c_str());
			});

		auto combo2 = builder->create<UICombo>();
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
		auto input = builder->create<UIInput>();
		layout->addElement(input);
		input->setFixedSize(100 * scale, 30 * scale);
		input->setText("Hello,OpenUI");
		input->editingStarted->connect(nullptr, [=](UIRect rect) {
			SDL_StartTextInput(window);
			SDL_Rect sdlRect{ (int)rect.X, (int)rect.Y, (int)rect.W, (int)rect.H };
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