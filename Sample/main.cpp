#include <SDL3/SDL.h>
#include "OpenUI/UICanvas.h"
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
#ifdef OPENUI_ENABLE_SDLGPU
#include "SDLGPU/SDLGPUDevice.h"
#endif

void sample(UICanvasRaw context, SDL_Window* window);

int main()
{
#ifdef OPENUI_ENABLE_SDLGPU
	SDL_Init(SDL_INIT_VIDEO);
	{
		auto device = UINew<SDLGPUDevice>();
		auto canvas = device->getCanvas();

		canvas->addWidget(canvas->getBuilder()->buildWidget(R"(
			<style>
				div {
					height: 30px;
				}
				button {
					width: auto;
				}
			</style>
			<div>
				<button style='filter:invert()'>My Button</button>
			</div>
	    )"));
		
		while (device->update()) continue;
		device = nullptr;
	}
	SDL_Quit();
#endif
	return 0;
}

void sample(UICanvasRaw context, SDL_Window* window)
{
	auto builder = context->getBuilder();
	auto scale = context->getConfig().DisplayScale;

	auto layout = builder->newWidget<UIFlow>();
	context->addWidget(layout);

	//if (false)
	{
		auto scroll = builder->newWidget<UIScroll>();
		layout->addWidget(scroll);
		scroll->setFixedSize(300 * scale, 200 * scale);
		scroll->setHorizontalValue(150);
		scroll->setVerticalValue(150);
		//if (false)
		{
			auto label = builder->newWidget<UILabel>();
			scroll->addWidget(label);
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
		auto vbox = builder->newWidget<UIVBox>();
		layout->addWidget(vbox);
		vbox->setFixedSize(200 * scale, 200 * scale);
		{
			auto button = builder->newWidget<UIButton>();
			vbox->addWidget(button);
			button->setText("Button0");
		}
		{
			auto button = builder->newWidget<UIButton>();
			vbox->addWidget(button);
			button->setText("Button1");
		}
		{
			auto button = builder->newWidget<UIButton>();
			vbox->addWidget(button);
			button->setText("Button2");
		}
		{
			auto button = builder->newWidget<UIButton>();
			vbox->addWidget(button);
			button->setText("Button3");
		}
	}
	//if (false)
	{
		auto hbox = builder->newWidget<UIHBox>();
		layout->addWidget(hbox);
		hbox->setFixedSize(250 * scale, 200 * scale);
		{
			auto button = builder->newWidget<UIButton>();
			hbox->addWidget(button);
			button->setText("Button0");
		}
		{
			auto button = builder->newWidget<UIButton>();
			hbox->addWidget(button);
			button->setText("Button1");
		}
		{
			auto button = builder->newWidget<UIButton>();
			hbox->addWidget(button);
			button->setText("Button2");
		}
		{
			auto button = builder->newWidget<UIButton>();
			hbox->addWidget(button);
			button->setText("Button3");
		}
	}
	//if (false)
	{
		auto grid = builder->newWidget<UIGrid>();
		layout->addWidget(grid);
		grid->setRowStretch({ 1,1,1 });
		grid->setColumnStretch({ 1,1,1 });
		grid->setFixedSize(200 * scale, 200 * scale);
		{
			auto button = builder->newWidget<UIButton>();
			grid->addWidget(button, 0, 0, 2, 2);
			button->setText("Button0");
		}
		{
			auto button = builder->newWidget<UIButton>();
			grid->addWidget(button, 0, 2, 3, 1);
			button->setText("Button1");
		}
		{
			auto button = builder->newWidget<UIButton>();
			grid->addWidget(button, 2, 0, 1, 1);
			button->setText("Button2");
		}
		{
			auto button = builder->newWidget<UIButton>();
			grid->addWidget(button, 2, 1, 1, 1);
			button->setText("Button3");
		}
	}
	//if (false)
	{
		auto group = builder->newWidget<UIVBox>();
		layout->addWidget(group);
		group->setFixedSize(200 * scale, 200 * scale);
		{
			auto radio = builder->newWidget<UIRadio>();
			group->addWidget(radio);
			radio->setFixedSize(100 * scale, 30 * scale);
			radio->setText("Radio");
			radio->setChecked(true);

			auto radio0 = builder->newWidget<UIRadio>();
			group->addWidget(radio0);
			radio0->setFixedSize(100 * scale, 30 * scale);
			radio0->setText("Radio");
			radio0->setExclusive(radio->getExclusive());

			auto radio1 = builder->newWidget<UIRadio>();
			group->addWidget(radio1);
			radio1->setFixedSize(100 * scale, 30 * scale);
			radio1->setText("Radio");
			radio1->setExclusive(radio->getExclusive());
		}
		{
			auto check = builder->newWidget<UICheck>();
			group->addWidget(check);
			check->setFixedSize(100 * scale, 30 * scale);
			check->setText("Check");
			check->setChecked(true);
		}
		{
			auto check = builder->newWidget<UICheck>();
			group->addWidget(check);
			check->setFixedSize(100 * scale, 30 * scale);
			check->setText("Check");
		}
	}
	//if (false)
	{
		auto label = builder->newWidget<UILabel>();
		layout->addWidget(label);
		label->setFixedSize(100 * scale, 30 * scale);
		label->setText("Label");
	}
	//if (false)
	{
		auto button = builder->newWidget<UIButton>();
		layout->addWidget(button);
		button->setFixedSize(100 * scale, 30 * scale);
		button->setText("Button");
	}
	//if (false)
	{
		auto slider = builder->newWidget<UISlider>();
		layout->addWidget(slider);
		slider->setFixedSize(100 * scale, 30 * scale);
		slider->setRange(0, 100 * scale);
		slider->setValue(25 * scale);
	}
	//if (false)
	{
		auto slider = builder->newWidget<UISlider>();
		layout->addWidget(slider);
		slider->setOrientation(UI::Vertical);
		slider->setFixedSize(30 * scale, 100 * scale);
		slider->setRange(0, 100 * scale);
		slider->setValue(25 * scale);
	}
	//if(false)
	{
		auto hline = builder->newWidget<UIHLine>();
		layout->addWidget(hline);
		hline->setFixedSize(100 * scale, 30 * scale);
	}
	//if(false)
	{
		auto vline = builder->newWidget<UIVLine>();
		layout->addWidget(vline);
		vline->setFixedSize(30 * scale, 100 * scale);
	}
	//if (false)
	{
		auto combo = builder->newWidget<UICombo>();
		layout->addWidget(combo);
		combo->setFixedSize(100 * scale, 30 * scale);
		combo->setMaxCount(4);
		combo->setItems({ (char*)u8"黄金糕狮子头螺蛳粉", (char*)u8"黄金糕", (char*)u8"狮子头", (char*)u8"螺蛳粉", (char*)u8"蚵仔煎", (char*)u8"双皮奶", (char*)u8"龙须面" });
		combo->setCurrentIndex(0);
		combo->currentTextChanged->connect(nullptr, [](UIString text) {
			printf("combo1 %s\n", text.c_str());
			});

		auto combo2 = builder->newWidget<UICombo>();
		layout->addWidget(combo2);
		combo2->setFixedSize(100 * scale, 30 * scale);
		combo2->setItems({ (char*)u8"黄金糕狮子头螺蛳粉", (char*)u8"黄金糕", (char*)u8"狮子头", (char*)u8"螺蛳粉", (char*)u8"蚵仔煎", (char*)u8"双皮奶", (char*)u8"龙须面" });
		combo2->setCurrentText((char*)u8"黄金糕");
		combo2->currentTextChanged->connect(nullptr, [](UIString text) {
			printf("combo2 %s\n", text.c_str());
			});
	}
	// if(false)
	{
		auto input = builder->newWidget<UIInput>();
		layout->addWidget(input);
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