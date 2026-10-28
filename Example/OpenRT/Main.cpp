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
#define OPENRTX_IMPLEMENTATION
#include <OpenRTX.h>
#include <OpenGL.h>
#include <SDL3/SDL.h>
#include "OpenRTDevice.h"

void sample(UICanvasRaw context, SDL_Window* window);
void ingame(UICanvasRaw context);
void desktop(UICanvasRaw canvas);

int main()
{
	// Render preedit text in UIInput/UIText; keep the native candidate list.
	// Windows only emits SDL_EVENT_TEXT_EDITING when composition is enabled.
	SDL_SetHint(SDL_HINT_IME_IMPLEMENTED_UI, "composition");
	SDL_Init(SDL_INIT_VIDEO);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	auto window = SDL_CreateWindow("OpenGL Context", 1, 1, SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);
	auto context = SDL_GL_CreateContext(window);
	SDL_GL_MakeCurrent(window, context);
	SDL_GL_SetSwapInterval(0);
	rt_load_library();

	auto device = UINew<OpenRTDevice>();
	auto canvas = device->getCanvas();
	device->setCursor("pointer");

	// canvas->addWidget(canvas->getBuilder()->buildWidget(R"(
	// 	<style>
	// 		div {
	// 			width: 100%;
	// 			height: 100%;
	// 		}
	// 		button {
	// 			width: 120px;
	// 			height: 40px;
	// 			border-radius: 4px;
	// 		}
	// 		button:hover {
	// 			background-color: #ff0000;
	// 		}
	// 		button:active {
	// 			background-color: #00ff00;
	// 		}
	// 		button:disabled {
	// 			background-color: #0000ff;
	// 		}
	// 	</style>
	// 	<div>
	// 		<button class="a">Button</button>
	// 	</div>
	// )"));

	sample(canvas, device->getWindow());

	while (device->update()) continue;
	device = nullptr;

	SDL_GL_MakeCurrent(window, context);
	rt_unload_library();

	SDL_Quit();
	return 0;
}

#include "OpenUI/UICanvas.h"
#include "OpenUI/UIBuilder.h"
#include "OpenUI/Default/UIHBox.h"
#include "OpenUI/Default/UIVBox.h"
#include "OpenUI/Default/UIFlow.h"
#include "OpenUI/Default/UIScroll.h"
#include "OpenUI/Default/UIGrid.h"
#include "OpenUI/Default/UILabel.h"
#include "OpenUI/Default/UIButton.h"
#include "OpenUI/Default/UISlider.h"
#include "OpenUI/Default/UIProgress.h"
#include "OpenUI/Default/UIRadio.h"
#include "OpenUI/Default/UICheck.h"
#include "OpenUI/Default/UICombo.h"
#include "OpenUI/Default/UILine.h"
#include "OpenUI/Default/UIInput.h"
#include "OpenUI/Default/UIText.h"

void sample(UICanvasRaw context, SDL_Window* window)
{
	auto builder = context->getBuilder();
	auto scale = context->getConfig().DisplayScale;
	auto px = [=](float value) { return value * scale; };

	auto page = builder->newWidget<UIHBox>();
	context->addWidget(page);
	page->setFixedWidth({100.0f, UI_CSS_WIDTH_PERCENTAGE});
	page->setFixedHeight({100.0f, UI_CSS_HEIGHT_PERCENTAGE});
	page->setStyleText("background-color", "#F4F5F7");

	auto filters = builder->newWidget<UIVBox>();
	page->addWidget(filters);
	filters->setMinWidth(px(180));
	filters->setStyleText("background-color", "#FFFFFF");
	{
		auto title = builder->newWidget<UILabel>();
		filters->addWidget(title);
		title->setFixedSize(px(180), px(36));
		title->setText("道具仓库");

		auto caption = builder->newWidget<UILabel>();
		filters->addWidget(caption);
		caption->setFixedSize(px(180), px(28));
		caption->setText("分类");

		auto all = builder->newWidget<UIRadio>();
		filters->addWidget(all);
		all->setFixedSize(px(180), px(32));
		all->setText("全部");
		all->setChecked(true);

		const char* categories[] = { "武器", "防具", "消耗" };
		for (auto name : categories)
		{
			auto radio = builder->newWidget<UIRadio>();
			filters->addWidget(radio);
			radio->setFixedSize(px(180), px(32));
			radio->setText(name);
			radio->setExclusive(all->getExclusive());
		}

		auto split = builder->newWidget<UIHLine>();
		filters->addWidget(split);
		split->setFixedSize(px(180), px(8));

		auto owned = builder->newWidget<UICheck>();
		filters->addWidget(owned);
		owned->setFixedSize(px(180), px(32));
		owned->setText("仅未装备");
		owned->setChecked(true);

		auto tradable = builder->newWidget<UICheck>();
		filters->addWidget(tradable);
		tradable->setFixedSize(px(180), px(32));
		tradable->setText("可交易");

		auto weightLabel = builder->newWidget<UILabel>();
		filters->addWidget(weightLabel);
		weightLabel->setFixedSize(px(180), px(28));
		weightLabel->setText("负重");

		auto weight = builder->newWidget<UISlider>();
		filters->addWidget(weight);
		weight->setFixedSize(px(180), px(32));
		weight->setRange(0, 100);
		weight->setValue(62);

		auto load = builder->newWidget<UIProgress>();
		filters->addWidget(load);
		load->setFixedSize(px(180), px(24));
		load->setRange(0, 100);
		load->setValue(62);
		load->setText("62%");
		auto loadRaw = load.get();
		weight->sliderMoved->connect(nullptr, [loadRaw](int32_t value) {
			loadRaw->setValue(value);
			loadRaw->setText(std::to_string(value) + "%");
			});
	}

	auto gutter = builder->newWidget<UIVLine>();
	page->addWidget(gutter);
	gutter->setFixedWidth(px(8));
	gutter->setFlexShrink({0.0f});

	auto itemsFlow = builder->newWidget<UIFlow>();
	page->addWidget(itemsFlow);
	itemsFlow->setStyleText("background-color", "#FFFFFF");
	{
		const char* items[] = {
			"古金长剑", "裂脊巨斧", "猎影短弓", "余烬法杖",
			"王城胸甲", "巡礼斗篷", "铁棘护手", "远行靴",
			"治疗药膏", "魔力药水", "火把", "回城卷轴",
			"古金币", "狼牙项链", "星铁锭", "空白符文",
			"破旧地图", "旅人干粮", "青铜钥匙", "守望号角",
			"灰烬之心", "潮汐珍珠", "龙骨碎片", "未名遗物",
		};

		auto selected = builder->newWidget<UILabel>();
		auto summary = builder->newWidget<UILabel>();
		auto note = builder->newWidget<UIText>();

		for (int index = 0; index < 24; ++index)
		{
			auto item = builder->newWidget<UIButton>();
			itemsFlow->addWidget(item);
			item->setFixedSize(px(120), px(40));
			item->setText(items[index]);
			item->clicked->connect(nullptr, [=](bool) {
				selected->setText(items[index]);
				summary->setText(UIString(items[index]) + "  x1    可交易");
				note->setText(UIString(items[index]) + "\n王城旧藏。\n可交易，取出后记入行囊。");
				});
			if (index == 0)
			{
				selected->setText(items[index]);
				summary->setText(UIString(items[index]) + "  x1    可交易");
				note->setText(UIString(items[index]) + "\n王城旧藏。\n可交易，取出后记入行囊。");
			}
		}

		auto detailGutter = builder->newWidget<UIVLine>();
		page->addWidget(detailGutter);
		detailGutter->setFixedWidth(px(8));
		detailGutter->setFlexShrink({0.0f});

		auto detail = builder->newWidget<UIVBox>();
		page->addWidget(detail);
		detail->setMinWidth(px(274));
		detail->setStyleText("background-color", "#FFFFFF");

		auto search = builder->newWidget<UIInput>();
		detail->addWidget(search);
		search->setFixedSize(px(260), px(36));
		search->setPlaceholderText("搜索道具");
		search->editingStarted->connect(nullptr, [=](UIRect rect) {
			SDL_StartTextInput(window);
			SDL_Rect sdlRect{ (int)rect.X, (int)rect.Y, (int)rect.W, (int)rect.H };
			SDL_SetTextInputArea(window, &sdlRect, 0);
			});
		search->textPasted->connect(nullptr, [=](UIString& value) {
			if (SDL_HasClipboardText())
			{
				auto result = SDL_GetClipboardText();
				value = UIString(result);
				SDL_free(result);
			}
			});
		search->textCopied->connect(nullptr, [=](UIString const& value) {
			SDL_ClearClipboardData();
			SDL_SetClipboardText(value.c_str());
			});

		auto sort = builder->newWidget<UICombo>();
		detail->addWidget(sort);
		sort->setFixedSize(px(260), px(36));
		sort->setMaxCount(4);
		sort->setItems({ "按稀有度", "按名称", "按重量", "按获得时间" });
		sort->setCurrentIndex(0);

		auto tidy = builder->newWidget<UIButton>();
		detail->addWidget(tidy);
		tidy->setFixedSize(px(260), px(36));
		tidy->setText("整理");

		selected->setFixedSize(px(260), px(32));
		detail->addWidget(selected);

		auto portraitRow = builder->newWidget<UIHBox>();
		detail->addWidget(portraitRow);
		portraitRow->setFixedSize(px(260), px(120));
		{
			auto portrait = builder->newWidget<UILabel>();
			portraitRow->addWidget(portrait);
			portrait->setFixedSize(px(200), px(120));
			portrait->setScaledContents(UILabel::ScaleKeepRatio);
			portrait->setText("纹章");
			int imgWidth = 0, imgHeight = 0, channels = 0;
			auto imageData = stbi_load("../../OpenUI.png", &imgWidth, &imgHeight, &channels, 4);
			if (imageData)
			{
				portrait->setPixmap({ (uint32_t)imgWidth, (uint32_t)imgHeight, (uint32_t)(imgWidth * 4), 0, imageData });
				stbi_image_free(imageData);
			}

			auto durability = builder->newWidget<UISlider>();
			portraitRow->addWidget(durability);
			durability->setOrientation(UI::Vertical);
			durability->setFixedSize(px(36), px(120));
			durability->setRange(0, 100);
			durability->setValue(80);
		}

		summary->setFixedSize(px(260), px(28));
		detail->addWidget(summary);

		detail->addWidget(note);
		note->setFixedSize(px(260), px(72));
		note->setPlaceholderText("备注");
		note->editingStarted->connect(nullptr, [=](UIRect rect) {
			SDL_StartTextInput(window);
			SDL_Rect sdlRect{ (int)rect.X, (int)rect.Y, (int)rect.W, (int)rect.H };
			SDL_SetTextInputArea(window, &sdlRect, 0);
			});
		note->textPasted->connect(nullptr, [=](UIString& value) {
			if (SDL_HasClipboardText())
			{
				auto result = SDL_GetClipboardText();
				value = UIString(result);
				SDL_free(result);
			}
			});
		note->textCopied->connect(nullptr, [=](UIString const& value) {
			SDL_ClearClipboardData();
			SDL_SetClipboardText(value.c_str());
			});

		auto traits = builder->newWidget<UIFlow>();
		detail->addWidget(traits);
		traits->setFixedSize(px(260), px(72));
		const char* traitNames[] = { "锋利", "古老", "可附魔" };
		for (auto traitName : traitNames)
		{
			auto trait = builder->newWidget<UILabel>();
			traits->addWidget(trait);
			trait->setFixedSize(px(76), px(28));
			trait->setText(traitName);
		}

		auto detailRule = builder->newWidget<UIHLine>();
		detail->addWidget(detailRule);
		detailRule->setFixedSize(px(260), px(8));

		auto countLabel = builder->newWidget<UILabel>();
		detail->addWidget(countLabel);
		countLabel->setFixedSize(px(260), px(28));
		countLabel->setText("取出数量");

		auto count = builder->newWidget<UISlider>();
		detail->addWidget(count);
		count->setFixedSize(px(260), px(32));
		count->setRange(1, 20);
		count->setValue(1);

		auto actions = builder->newWidget<UIHBox>();
		detail->addWidget(actions);
		actions->setFixedSize(px(260), px(40));
		{
			auto equip = builder->newWidget<UIButton>();
			actions->addWidget(equip);
			equip->setFixedSize(px(120), px(36));
			equip->setText("装备");

			auto drop = builder->newWidget<UIButton>();
			actions->addWidget(drop);
			drop->setFixedSize(px(120), px(36));
			drop->setText("丢弃");
		}
	}
}
