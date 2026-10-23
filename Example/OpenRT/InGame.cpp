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
#include "OpenUI/UICanvas.h"
#include "OpenUI/UIBuilder.h"
#include "OpenUI/InGame/IG.h"
#include <stb_image.h>
#include <vector>

namespace
{
	struct CampItem
	{
		const char* Name;
		const char* Brief;
		int Count;
		IGRarity Rarity;
	};

	const CampItem Items[] =
	{
		{ "古金长剑", "锋利 · 古老 · 可附魔", 1, IGRarity::Legendary },
		{ "裂脊巨斧", "重击 · 破甲", 1, IGRarity::Epic },
		{ "猎影短弓", "远程 · 静默", 1, IGRarity::Rare },
		{ "余烬法杖", "火焰 · 引导", 1, IGRarity::Epic },
		{ "守望号角", "召唤 · 一次", 1, IGRarity::Rare },
		{ "王城胸甲", "护甲 · 王城制式", 1, IGRarity::Legendary },
		{ "巡礼斗篷", "抗性 · 旅行", 1, IGRarity::Rare },
		{ "铁棘护手", "格挡 · 反击", 1, IGRarity::Uncommon },
		{ "远行靴", "移速 · 已磨损", 1, IGRarity::Common },
		{ "狼牙项链", "暴击 · 狩猎", 1, IGRarity::Epic },
		{ "治疗药膏", "恢复生命", 12, IGRarity::Uncommon },
		{ "魔力药水", "恢复魔力", 8, IGRarity::Rare },
		{ "火把", "照明", 6, IGRarity::Common },
		{ "回城卷轴", "返回灰港", 3, IGRarity::Uncommon },
		{ "旅人干粮", "果腹", 20, IGRarity::Common },
		{ "古金币", "通货", 48, IGRarity::Legendary },
		{ "星铁锭", "锻造材料", 7, IGRarity::Rare },
		{ "空白符文", "可铭刻", 2, IGRarity::Epic },
		{ "青铜钥匙", "开启旧闸", 1, IGRarity::Uncommon },
		{ "灰烬之心", "任务物品", 1, IGRarity::Legendary },
		{ "潮汐珍珠", "祭坛供品", 4, IGRarity::Rare },
		{ "龙骨碎片", "锻造材料", 9, IGRarity::Epic },
		{ "破旧地图", "北脊的路", 1, IGRarity::Common },
		{ "未名遗物", "来源不明", 1, IGRarity::Legendary },
		{ "王城印戒", "身份 · 过期", 1, IGRarity::Rare },
		{ "夜莺笛", "安抚兽群", 1, IGRarity::Uncommon },
		{ "霜纹披巾", "抗寒", 1, IGRarity::Rare },
		{ "黑盐", "防腐 · 交易", 15, IGRarity::Common },
		{ "兽骨箭", "弹药", 36, IGRarity::Common },
		{ "银质罗盘", "不会指北", 1, IGRarity::Epic },
		{ "旧日信笺", "墨迹未干", 1, IGRarity::Uncommon },
		{ "烬火石", "引火", 5, IGRarity::Uncommon },
	};

	const char* Quests[] =
	{
		"余烬之心  ·  前往灰港",
		"守望者的号角仍未吹响",
		"潮汐祭坛缺少四枚珍珠",
		"失落的青铜钥匙",
		"把王城密信交给书记官",
		"狼群越过了北脊",
		"为旅人再收集一份干粮",
		"未名遗物在夜里低语",
		"铁匠愿意修好远行靴",
		"星铁锭还差三块",
	};
}

void ingame(UICanvasRaw canvas)
{
	auto builder = canvas->getBuilder();
	auto scale = canvas->getConfig().DisplayScale;
	auto px = [=](float value) { return value * scale; };
	const int itemCount = (int)(sizeof(Items) / sizeof(Items[0]));

	auto page = builder->newWidget<IGVBox>();
	canvas->addWidget(page);
	page->setFixedWidth({ 100.0f, UI_CSS_WIDTH_PERCENTAGE });
	page->setFixedHeight({ 100.0f, UI_CSS_HEIGHT_PERCENTAGE });
	page->setStyle({ .Pen = UINoPen, .Brush = UIFrameBrush });

	auto bar = builder->newWidget<IGHBox>();
	page->addWidget(bar);
	bar->setFixedHeight(px(48));
	bar->setPadding({ px(12) }, { px(8) }, { px(12) }, { px(8) });
	bar->setStyle({ .Pen = UIFramePen, .Brush = UIInsetBrush });
	{
		auto title = builder->newWidget<IGLabel>();
		bar->addWidget(title);
		title->setFixedSize(px(180), px(32));
		title->setText("灰烬边境");
		UIFont font;
		font.Size = 18;
		font.Align = UIFont::AlignLeft | UIFont::AlignVCenter;
		title->setFont(font);
		title->setColor(UIAccentBrush);

		auto spring = builder->newWidget<IGLabel>();
		bar->addWidget(spring);

		auto gold = builder->newWidget<IGLabel>();
		bar->addWidget(gold);
		gold->setFixedSize(px(140), px(32));
		gold->setText("金币  1,280");
		gold->setFont({ .Align = UIFont::AlignRight | UIFont::AlignVCenter });
		gold->setColor(UIAccentBrush);

		auto map = builder->newWidget<IGButton>();
		bar->addWidget(map);
		map->setFixedSize(px(72), px(32));
		map->setMarginLeft({ px(8) });
		map->setText("地图");

		auto system = builder->newWidget<IGButton>();
		bar->addWidget(system);
		system->setFixedSize(px(72), px(32));
		system->setMarginLeft({ px(8) });
		system->setText("系统");
	}

	auto body = builder->newWidget<IGHBox>();
	page->addWidget(body);

	auto left = builder->newWidget<IGVBox>();
	body->addWidget(left);
	left->setMinWidth(px(228));
	left->setMarginLeft({ px(8) });
	left->setMarginTop({ px(8) });
	left->setMarginBottom({ px(8) });

	auto portrait = builder->newWidget<IGStack>();
	left->addWidget(portrait);
	portrait->setFixedHeight(px(128));
	portrait->setMarginBottom({ px(8) });
	portrait->setStyle({ .Pen = UIFramePen, .Brush = UIInsetBrush });
	{
		auto crest = builder->newWidget<IGImage>();
		portrait->addWidget(crest);
		int imgWidth = 0, imgHeight = 0, channels = 0;
		auto imageData = stbi_load("../../OpenUI.png", &imgWidth, &imgHeight, &channels, 4);
		bool loaded = imageData != nullptr;
		if (loaded)
		{
			crest->setImage({ (uint32_t)imgWidth, (uint32_t)imgHeight, (uint32_t)(imgWidth * 4), 4, imageData });
			stbi_image_free(imageData);
		}

		auto level = builder->newWidget<IGLabel>();
		portrait->addWidget(level);
		level->setText(loaded ? "Lv.27  守望者" : "纹章");
		UIFont font;
		font.Align = loaded ? (UIFont::AlignCenter | UIFont::AlignBottom) : (UIFont::AlignCenter | UIFont::AlignVCenter);
		font.Size = 16;
		level->setFont(font);
		level->setColor(UIAccentBrush);
	}

	auto status = builder->newWidget<IGPanel>();
	left->addWidget(status);
	status->setTitle("状态");
	status->setFixedHeight(px(132));
	status->setMarginBottom({ px(8) });
	{
		auto health = builder->newWidget<IGProgress>();
		status->addWidget(health);
		health->setFixedHeight(px(22));
		health->setMarginBottom({ px(6) });
		health->setValue(860.0f / 1200.0f);
		health->setText("生命  860 / 1200");

		auto mana = builder->newWidget<IGProgress>();
		status->addWidget(mana);
		mana->setFixedHeight(px(22));
		mana->setMarginBottom({ px(6) });
		mana->setValue(240.0f / 400.0f);
		mana->setText("魔力  240 / 400");

		auto experience = builder->newWidget<IGProgress>();
		status->addWidget(experience);
		experience->setFixedHeight(px(22));
		experience->setValue(0.37f);
		experience->setText("经验  37%");
	}

	auto kit = builder->newWidget<IGPanel>();
	left->addWidget(kit);
	kit->setTitle("行装");
	{
		auto helm = builder->newWidget<IGCheck>();
		kit->addWidget(helm);
		helm->setFixedHeight(px(28));
		helm->setText("显示头盔");
		helm->setChecked(true);

		auto cloak = builder->newWidget<IGCheck>();
		kit->addWidget(cloak);
		cloak->setFixedHeight(px(28));
		cloak->setText("显示披风");
		cloak->setChecked(true);

		auto tradable = builder->newWidget<IGCheck>();
		kit->addWidget(tradable);
		tradable->setFixedHeight(px(28));
		tradable->setMarginBottom({ px(6) });
		tradable->setText("只看可交易");

		auto weightCaption = builder->newWidget<IGLabel>();
		kit->addWidget(weightCaption);
		weightCaption->setFixedHeight(px(22));
		weightCaption->setText("负重");
		weightCaption->setColor(UIMutedBrush);

		auto burden = builder->newWidget<IGSlider>();
		kit->addWidget(burden);
		burden->setFixedHeight(px(28));
		burden->setRange(0, 100);

		auto burdenBar = builder->newWidget<IGProgress>();
		kit->addWidget(burdenBar);
		burdenBar->setFixedHeight(px(18));
		burdenBar->setMarginTop({ px(4) });
		burden->valueChanged->connect(nullptr, [=](int32_t value) {
			burdenBar->setValue(value / 100.0f);
			burdenBar->setText(std::to_string(value) + " / 100");
			});
		burden->setValue(62);
	}

	auto gutter = builder->newWidget<IGVLine>();
	body->addWidget(gutter);
	gutter->setFixedWidth(px(8));
	gutter->setFlexShrink({ 0.0f });

	auto bag = builder->newWidget<IGPanel>();
	body->addWidget(bag);
	bag->setTitle("背包");
	bag->setMarginTop({ px(8) });
	bag->setMarginBottom({ px(8) });
	{
		auto filter = builder->newWidget<IGLabel>();
		bag->addWidget(filter);
		filter->setFixedHeight(px(22));
		filter->setText("分类  ·  全部");
		filter->setColor(UIMutedBrush);

		auto categories = builder->newWidget<IGFlow>();
		bag->addWidget(categories);
		categories->setFixedHeight(px(40));
		const char* names[] = { "全部", "武器", "防具", "消耗", "材料" };
		for (auto name : names)
		{
			auto button = builder->newWidget<IGButton>();
			categories->addWidget(button);
			button->setFixedSize(px(72), px(28));
			button->setMarginRight({ px(6) });
			button->setText(name);
			button->clicked->connect(nullptr, [=]() {
				filter->setText(UIString("分类  ·  ") + name);
				});
		}

		auto rule = builder->newWidget<IGHLine>();
		bag->addWidget(rule);
		rule->setFixedHeight(px(8));

		auto scroller = builder->newWidget<IGScroll>();
		bag->addWidget(scroller);

		auto cells = builder->newWidget<IGFlow>();
		scroller->addWidget(cells);
		cells->setFixedHeight(px(580));

		auto hold = std::make_shared<std::vector<int>>(itemCount);
		auto slots = std::make_shared<std::vector<IGSlotRef>>(itemCount);
		auto picked = std::make_shared<int>(0);

		auto detail = builder->newWidget<IGPanel>();
		auto name = builder->newWidget<IGLabel>();
		auto summary = builder->newWidget<IGLabel>();
		auto owned = builder->newWidget<IGLabel>();
		auto action = builder->newWidget<IGLabel>();
		auto takeLabel = builder->newWidget<IGLabel>();
		auto take = builder->newWidget<IGSlider>();

		auto show = [=](int index) {
			*picked = index;
			name->setText(Items[index].Name);
			summary->setText(Items[index].Brief);
			int count = (*hold)[index];
			owned->setText(count > 0 ? "持有  " + std::to_string(count) : "已经丢弃");
			take->setRange(1, count > 1 ? count : 1);
			take->setValue(1);
			};

		for (int index = 0; index < itemCount; ++index)
		{
			auto slot = builder->newWidget<IGSlot>();
			cells->addWidget(slot);
			slot->setFixedSize(px(76), px(76));
			slot->setMarginRight({ px(6) });
			slot->setMarginBottom({ px(6) });
			slot->setText(Items[index].Name);
			slot->setRarity(Items[index].Rarity);
			slot->setCount(Items[index].Count);
			(*hold)[index] = Items[index].Count;
			(*slots)[index] = slot;
			slot->clicked->connect(nullptr, [=]() { show(index); });
		}

		auto detailGutter = builder->newWidget<IGVLine>();
		body->addWidget(detailGutter);
		detailGutter->setFixedWidth(px(8));
		detailGutter->setFlexShrink({ 0.0f });

		auto right = builder->newWidget<IGVBox>();
		body->addWidget(right);
		right->setMinWidth(px(276));
		right->setMarginTop({ px(8) });
		right->setMarginRight({ px(8) });
		right->setMarginBottom({ px(8) });

		right->addWidget(detail);
		detail->setTitle("详情");
		detail->setFixedHeight(px(292));
		detail->setMarginBottom({ px(8) });
		{
			detail->addWidget(name);
			name->setFixedHeight(px(28));
			UIFont nameFont;
			nameFont.Size = 18;
			nameFont.Align = UIFont::AlignLeft | UIFont::AlignVCenter;
			name->setFont(nameFont);
			name->setColor(UIAccentBrush);

			detail->addWidget(summary);
			summary->setFixedHeight(px(36));
			UIFont summaryFont;
			summaryFont.Align = UIFont::AlignLeft | UIFont::AlignTop;
			summaryFont.LineWrap = true;
			summary->setFont(summaryFont);

			detail->addWidget(owned);
			owned->setFixedHeight(px(22));
			owned->setColor(UIMutedBrush);

			auto traits = builder->newWidget<IGFlow>();
			detail->addWidget(traits);
			traits->setFixedHeight(px(36));
			const char* traitNames[] = { "锋利", "古老", "可附魔" };
			for (auto traitName : traitNames)
			{
				auto trait = builder->newWidget<IGLabel>();
				traits->addWidget(trait);
				trait->setFixedSize(px(72), px(24));
				trait->setMarginRight({ px(6) });
				trait->setText(traitName);
				trait->setFont({ .Align = UIFont::AlignCenter | UIFont::AlignVCenter });
				trait->setColor(UIAccentBrush);
			}

			auto detailRule = builder->newWidget<IGHLine>();
			detail->addWidget(detailRule);
			detailRule->setFixedHeight(px(8));

			detail->addWidget(takeLabel);
			takeLabel->setFixedHeight(px(22));
			takeLabel->setText("取出  1");
			takeLabel->setColor(UIMutedBrush);

			detail->addWidget(take);
			take->setFixedHeight(px(28));
			take->valueChanged->connect(nullptr, [=](int32_t value) {
				takeLabel->setText("取出  " + std::to_string(value));
				});

			auto actions = builder->newWidget<IGHBox>();
			detail->addWidget(actions);
			actions->setFixedHeight(px(36));
			actions->setMarginTop({ px(6) });
			{
				auto equip = builder->newWidget<IGButton>();
				actions->addWidget(equip);
				equip->setMarginRight({ px(8) });
				equip->setText("装备");
				equip->clicked->connect(nullptr, [=]() {
					int index = *picked;
					if ((*hold)[index] <= 0) return;
					action->setText(UIString("已装备  ") + Items[index].Name);
					});

				auto drop = builder->newWidget<IGButton>();
				actions->addWidget(drop);
				drop->setText("丢弃");
				drop->clicked->connect(nullptr, [=]() {
					int index = *picked;
					if ((*hold)[index] <= 0) return;
					(*hold)[index] -= 1;
					(*slots)[index]->setCount((*hold)[index]);
					show(index);
					});
			}

			detail->addWidget(action);
			action->setFixedHeight(px(22));
			action->setMarginTop({ px(4) });
			action->setText("尚未装备");
			action->setColor(UIMutedBrush);
		}

		auto attributes = builder->newWidget<IGPanel>();
		right->addWidget(attributes);
		attributes->setTitle("属性");
		attributes->setFixedHeight(px(148));
		attributes->setMarginBottom({ px(8) });
		{
			auto grid = builder->newWidget<IGGrid>();
			attributes->addWidget(grid);
			grid->setFixedHeight(px(88));
			const char* stats[] = { "攻击  128", "护甲  46", "负重  62", "幸运  15" };
			for (int index = 0; index < 4; ++index)
			{
				auto stat = builder->newWidget<IGLabel>();
				grid->addWidget(stat, index / 2, index % 2);
				stat->setText(stats[index]);
				stat->setFont({ .Align = UIFont::AlignCenter | UIFont::AlignVCenter });
			}
		}

		auto journal = builder->newWidget<IGPanel>();
		right->addWidget(journal);
		journal->setTitle("任务");
		{
			auto questScroll = builder->newWidget<IGScroll>();
			journal->addWidget(questScroll);

			auto quests = builder->newWidget<IGVBox>();
			questScroll->addWidget(quests);
			quests->setFixedHeight(px(360));
			for (int index = 0; index < (int)(sizeof(Quests) / sizeof(Quests[0])); ++index)
			{
				auto quest = builder->newWidget<IGLabel>();
				quests->addWidget(quest);
				quest->setFixedHeight(px(32));
				quest->setMarginBottom({ px(4) });
				quest->setText(Quests[index]);
				quest->setColor(index == 0 ? UIAccentBrush : UITextBrush);
			}
		}

		for (int index = 0; index < itemCount; ++index) (*hold)[index] = Items[index].Count;
		show(0);
	}
}
