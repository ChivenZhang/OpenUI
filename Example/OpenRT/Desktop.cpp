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
#include "OpenUI/Desktop/PC.h"
#include <string>

void desktop(UICanvasRaw canvas)
{
	auto builder = canvas->getBuilder();
	auto scale = canvas->getConfig().DisplayScale;
	auto px = [=](float value) { return value * scale; };
	auto field = [&](UIWidgetRef widget, float height)
	{
		widget->setFixedHeight(px(height));
	};

	auto page = builder->newWidget<PCHBox>();
	canvas->addWidget(page);
	page->setFixedWidth({ 100.0f, UI_CSS_WIDTH_PERCENTAGE });
	page->setFixedHeight({ 100.0f, UI_CSS_HEIGHT_PERCENTAGE });
	page->setStyleText("background-color", "#F4F5F7");
	page->setStyleText("padding", "12px");

	auto side = builder->newWidget<PCVBox>();
	page->addWidget(side);
	side->setMinWidth(px(200));
	side->setFlexShrink({ 0.0f });
	side->setStyleText("background-color", "#FFFFFF");
	side->setStyleText("border-color", "#C5CAD3");
	side->setStyleText("border-width", "1px");
	side->setStyleText("border-radius", "8px");
	side->setStyleText("padding", "12px");
	side->setStyleText("gap", "6px");

	auto title = builder->newWidget<PCLabel>();
	side->addWidget(title);
	field(title, 28);
	title->setText("资源库");
	title->setStyleText("font-family", "Segoe UI");
	title->setStyleText("color", "#1C1F24");

	auto search = builder->newWidget<PCLineEdit>();
	side->addWidget(search);
	field(search, 28);
	search->setPlaceholder("搜索名称");

	auto sideRule = builder->newWidget<PCHSeparator>();
	side->addWidget(sideRule);
	field(sideRule, 8);

	auto rangeCaption = builder->newWidget<PCLabel>();
	side->addWidget(rangeCaption);
	field(rangeCaption, 20);
	rangeCaption->setText("范围");
	rangeCaption->setStyleText("color", "#6A717A");

	static PCRadioGroup kinds;
	const char* kindNames[] = { "全部", "文档", "图片" };
	PCRadioButtonRef radios[3];
	for (int index = 0; index < 3; ++index)
	{
		radios[index] = builder->newWidget<PCRadioButton>();
		side->addWidget(radios[index]);
		field(radios[index], 28);
		radios[index]->setText(kindNames[index]);
		radios[index]->setExclusive(&kinds);
	}

	auto filterRule = builder->newWidget<PCHSeparator>();
	side->addWidget(filterRule);
	field(filterRule, 8);

	auto nested = builder->newWidget<PCCheckBox>();
	side->addWidget(nested);
	field(nested, 28);
	nested->setText("包含子目录");
	nested->setChecked(true);

	auto localOnly = builder->newWidget<PCCheckBox>();
	side->addWidget(localOnly);
	field(localOnly, 28);
	localOnly->setText("仅本地文件");

	auto zoomCaption = builder->newWidget<PCLabel>();
	side->addWidget(zoomCaption);
	field(zoomCaption, 20);
	zoomCaption->setText("缩放");
	zoomCaption->setStyleText("color", "#6A717A");

	auto zoom = builder->newWidget<PCLabel>();
	side->addWidget(zoom);
	field(zoom, 20);
	zoom->setText("100%");

	auto slider = builder->newWidget<PCSlider>();
	side->addWidget(slider);
	field(slider, 20);
	slider->setRange(50, 200);
	slider->setValue(100);

	auto progress = builder->newWidget<PCProgressBar>();
	side->addWidget(progress);
	field(progress, 18);
	progress->setRange(50, 200);
	progress->setValue(100);
	progress->setText("100%");

	auto gutter = builder->newWidget<PCVSeparator>();
	page->addWidget(gutter);
	gutter->setFixedWidth(px(12));
	gutter->setFlexShrink({ 0.0f });

	auto tabs = builder->newWidget<PCTabWidget>();
	page->addWidget(tabs);
	tabs->setMinWidth({ 0.0f, UI_CSS_MIN_WIDTH_AUTO });

	UIStringList files = {
		"需求说明.docx", "界面草图.png", "会议记录.txt", "组件清单.xlsx",
		"图标.svg", "发布说明.md", "桌面布局.css", "字体授权.pdf",
	};
	auto browser = builder->newWidget<PCListBox>();
	tabs->addTab("浏览", browser);
	browser->setItems(files);

	auto logScroll = builder->newWidget<PCScroll>();
	tabs->addTab("记录", logScroll);
	auto log = builder->newWidget<PCVBox>();
	logScroll->addWidget(log);
	log->setFixedHeight(px(720));
	log->setStyleText("padding", "8px");
	const char* entries[] = {
		"09:12  同步完成", "09:18  打开 需求说明.docx", "09:26  标记 重要",
		"09:41  另存 界面草图.png", "10:02  筛选 文档", "10:15  缩放 100%",
		"10:21  打开 会议记录.txt", "10:33  固定 组件清单.xlsx", "10:48  取消 只读",
		"11:05  移动到 归档", "11:16  重命名 发布说明.md", "11:22  复制 图标.svg",
		"11:30  份数改为 2", "11:36  排序改为 按时间", "11:44  关闭预览",
		"11:51  显示全部",
	};
	for (auto entry : entries)
	{
		auto line = builder->newWidget<PCLabel>();
		log->addWidget(line);
		field(line, 28);
		line->setText(entry);
	}

	auto form = builder->newWidget<PCVBox>();
	tabs->addTab("调整", form);
	form->setStyleText("padding", "12px");
	form->setStyleText("gap", "6px");

	auto sortCaption = builder->newWidget<PCLabel>();
	form->addWidget(sortCaption);
	field(sortCaption, 20);
	sortCaption->setText("排序");
	sortCaption->setStyleText("color", "#6A717A");

	auto sort = builder->newWidget<PCComboBox>();
	form->addWidget(sort);
	field(sort, 28);
	sort->setItems({ "按名称", "按时间", "按大小", "按类型" });

	auto countCaption = builder->newWidget<PCLabel>();
	form->addWidget(countCaption);
	field(countCaption, 20);
	countCaption->setText("份数");
	countCaption->setStyleText("color", "#6A717A");

	auto count = builder->newWidget<PCSpinBox>();
	form->addWidget(count);
	field(count, 28);
	count->setRange(1, 20);
	count->setValue(1);

	auto markCaption = builder->newWidget<PCLabel>();
	form->addWidget(markCaption);
	field(markCaption, 20);
	markCaption->setText("标记");
	markCaption->setStyleText("color", "#6A717A");

	auto marks = builder->newWidget<PCFlow>();
	form->addWidget(marks);
	field(marks, 68);
	marks->setStyleText("gap", "8px");
	const char* markNames[] = { "重要", "已同步", "只读" };
	PCButtonRef markButtons[3];
	for (int index = 0; index < 3; ++index)
	{
		markButtons[index] = builder->newWidget<PCButton>();
		marks->addWidget(markButtons[index]);
		markButtons[index]->setFixedSize(px(72), px(28));
		markButtons[index]->setText(markNames[index]);
		markButtons[index]->setCheckable(true);
	}

	auto actionCaption = builder->newWidget<PCLabel>();
	form->addWidget(actionCaption);
	field(actionCaption, 20);
	actionCaption->setText("动作");
	actionCaption->setStyleText("color", "#6A717A");

	auto actions = builder->newWidget<PCGrid>();
	form->addWidget(actions);
	field(actions, 88);
	actions->setColumnCount(2);
	actions->setStyleText("gap", "8px");
	const char* actionNames[] = { "复制", "移动", "重命名", "属性" };
	PCButtonRef actionButtons[4];
	for (int index = 0; index < 4; ++index)
	{
		actionButtons[index] = builder->newWidget<PCButton>();
		actions->addWidget(actionButtons[index]);
		actionButtons[index]->setText(actionNames[index]);
	}

	auto commands = builder->newWidget<PCHBox>();
	form->addWidget(commands);
	field(commands, 36);
	commands->setStyleText("gap", "8px");

	auto apply = builder->newWidget<PCButton>();
	commands->addWidget(apply);
	apply->setMinWidth({ 0.0f, UI_CSS_MIN_WIDTH_AUTO });
	apply->setText("应用");
	apply->setStyleText("background-color", "#0F6CBD");
	apply->setStyleText("background-color:hover", "#115EA3");
	apply->setStyleText("background-color:active", "#0C4A7A");
	apply->setStyleText("color", "#FFFFFF");
	apply->setStyleText("color:hover", "#FFFFFF");
	apply->setStyleText("color:active", "#FFFFFF");
	apply->setStyleText("border-color", "#0F6CBD");
	apply->setStyleText("border-color:hover", "#115EA3");
	apply->setStyleText("border-color:active", "#0C4A7A");

	auto reset = builder->newWidget<PCButton>();
	commands->addWidget(reset);
	reset->setMinWidth({ 0.0f, UI_CSS_MIN_WIDTH_AUTO });
	reset->setText("重置");

	auto detailGutter = builder->newWidget<PCVSeparator>();
	page->addWidget(detailGutter);
	detailGutter->setFixedWidth(px(12));
	detailGutter->setFlexShrink({ 0.0f });

	auto detail = builder->newWidget<PCFrame>();
	page->addWidget(detail);
	detail->setMinWidth(px(240));
	detail->setFlexShrink({ 0.0f });
	detail->setTitle("详情");
	detail->setStyleText("gap", "8px");

	auto name = builder->newWidget<PCLabel>();
	detail->addWidget(name);
	field(name, 28);
	name->setText(files.front());

	auto path = builder->newWidget<PCLabel>();
	detail->addWidget(path);
	field(path, 22);
	path->setText("资源库 / " + files.front());
	path->setStyleText("color", "#6A717A");

	auto preview = builder->newWidget<PCStack>();
	detail->addWidget(preview);
	field(preview, 48);
	preview->setStyleText("background-color", "#FFFFFF");
	preview->setStyleText("border-color", "#C5CAD3");
	preview->setStyleText("border-width", "1px");
	preview->setStyleText("border-radius", "4px");
	preview->setStyleText("padding", "8px");
	const char* previewText[] = { "当前范围：全部资源", "当前范围：文档", "当前范围：图片" };
	for (auto text : previewText)
	{
		auto pageLabel = builder->newWidget<PCLabel>();
		preview->addWidget(pageLabel);
		pageLabel->setText(text);
	}

	auto operate = builder->newWidget<PCGroupBox>();
	detail->addWidget(operate);
	operate->setTitle("操作");
	operate->setStyleText("gap", "8px");

	auto open = builder->newWidget<PCButton>();
	operate->addWidget(open);
	field(open, 32);
	open->setText("打开");
	open->setStyleText("background-color", "#0F6CBD");
	open->setStyleText("background-color:hover", "#115EA3");
	open->setStyleText("background-color:active", "#0C4A7A");
	open->setStyleText("color", "#FFFFFF");
	open->setStyleText("color:hover", "#FFFFFF");
	open->setStyleText("color:active", "#FFFFFF");
	open->setStyleText("border-color", "#0F6CBD");
	open->setStyleText("border-color:hover", "#115EA3");
	open->setStyleText("border-color:active", "#0C4A7A");

	auto pin = builder->newWidget<PCButton>();
	operate->addWidget(pin);
	field(pin, 32);
	pin->setText("固定");
	pin->setCheckable(true);

	auto archive = builder->newWidget<PCButton>();
	operate->addWidget(archive);
	field(archive, 32);
	archive->setText("归档");
	archive->setEnable(false);

	auto status = builder->newWidget<PCLabel>();
	detail->addWidget(status);
	field(status, 22);
	status->setText("就绪");
	status->setStyleText("color", "#6A717A");

	auto* browserRaw = browser.get();
	auto* nameRaw = name.get();
	auto* pathRaw = path.get();
	auto* statusRaw = status.get();
	auto* previewRaw = preview.get();
	auto* progressRaw = progress.get();
	auto* zoomRaw = zoom.get();
	auto* searchRaw = search.get();
	auto* sliderRaw = slider.get();
	auto* countRaw = count.get();
	auto* sortRaw = sort.get();

	auto showRow = [browserRaw, nameRaw, pathRaw](int32_t row)
	{
		auto const& items = browserRaw->getItems();
		if (row < 0 || row >= (int32_t)items.size())
		{
			nameRaw->setText("未选择");
			pathRaw->setText("没有匹配的文件");
			return;
		}
		nameRaw->setText(items[row]);
		pathRaw->setText("资源库 / " + items[row]);
	};

	browser->currentRowChanged->connect(nullptr, [showRow, statusRaw](int32_t row)
	{
		showRow(row);
		if (row >= 0) statusRaw->setText("已选中");
	});
	browser->itemActivated->connect(nullptr, [browserRaw, statusRaw](int32_t row)
	{
		auto const& items = browserRaw->getItems();
		if (row < 0 || row >= (int32_t)items.size()) return;
		statusRaw->setText("已打开 " + items[row]);
	});
	browser->setCurrentRow(0);

	search->textChanged->connect(nullptr, [browserRaw, statusRaw, files, showRow](UIString const& text)
	{
		UIStringList shown;
		for (auto const& item : files)
		{
			if (text.empty() || item.find(text) != UIString::npos) shown.push_back(item);
		}
		browserRaw->setItems(shown);
		showRow(shown.empty() ? -1 : 0);
		if (shown.empty() == false) browserRaw->setCurrentRow(0);
		statusRaw->setText(text.empty() ? UIString("显示全部") : "筛选：" + text);
	});
	search->returnPressed->connect(nullptr, [searchRaw, statusRaw]()
	{
		statusRaw->setText(searchRaw->getText().empty() ? UIString("显示全部") : "确认筛选：" + searchRaw->getText());
	});

	for (int index = 0; index < 3; ++index)
	{
		radios[index]->toggled->connect(nullptr, [previewRaw, statusRaw, index](bool on)
		{
			if (on == false) return;
			previewRaw->setCurrentIndex(index);
			const char* names[] = { "全部", "文档", "图片" };
			statusRaw->setText(UIString("范围：") + names[index]);
		});
	}
	radios[0]->setChecked(true);

	nested->toggled->connect(nullptr, [statusRaw](bool on)
	{
		statusRaw->setText(on ? "包含子目录" : "仅当前目录");
	});
	localOnly->toggled->connect(nullptr, [statusRaw](bool on)
	{
		statusRaw->setText(on ? "仅本地文件" : "包含远程文件");
	});

	slider->valueChanged->connect(nullptr, [progressRaw, zoomRaw](int32_t value)
	{
		progressRaw->setValue(value);
		progressRaw->setText(std::to_string(value) + "%");
		zoomRaw->setText(std::to_string(value) + "%");
	});

	tabs->currentChanged->connect(nullptr, [statusRaw](int32_t index)
	{
		const char* names[] = { "浏览", "记录", "调整" };
		if (index < 0 || index > 2) return;
		statusRaw->setText(UIString("页签：") + names[index]);
	});

	sort->setCurrentIndex(0);
	sort->currentTextChanged->connect(nullptr, [statusRaw](UIString const& text)
	{
		statusRaw->setText("排序：" + text);
	});

	count->valueChanged->connect(nullptr, [statusRaw](int32_t value)
	{
		statusRaw->setText("份数：" + std::to_string(value));
	});

	for (int index = 0; index < 3; ++index)
	{
		markButtons[index]->toggled->connect(nullptr, [statusRaw, index](bool on)
		{
			const char* names[] = { "重要", "已同步", "只读" };
			statusRaw->setText(UIString(names[index]) + (on ? " 已标记" : " 已取消"));
		});
	}
	for (int index = 0; index < 4; ++index)
	{
		actionButtons[index]->clicked->connect(nullptr, [statusRaw, browserRaw, index]()
		{
			const char* names[] = { "复制", "移动", "重命名", "属性" };
			auto const& items = browserRaw->getItems();
			auto row = browserRaw->getCurrentRow();
			UIString target = row >= 0 && row < (int32_t)items.size() ? items[row] : UIString("当前项");
			statusRaw->setText(UIString(names[index]) + " " + target);
		});
	}

	apply->clicked->connect(nullptr, [statusRaw, sortRaw, countRaw]()
	{
		statusRaw->setText("已应用 " + sortRaw->getCurrentText() + " / " + std::to_string(countRaw->getValue()) + " 份");
	});
	reset->clicked->connect(nullptr, [statusRaw, searchRaw, sliderRaw, countRaw, sortRaw, browserRaw, files, showRow]()
	{
		searchRaw->setText("");
		sliderRaw->setValue(100);
		countRaw->setValue(1);
		sortRaw->setCurrentIndex(0);
		browserRaw->setItems(files);
		browserRaw->setCurrentRow(0);
		showRow(0);
		statusRaw->setText("已重置");
	});
	open->clicked->connect(nullptr, [statusRaw, browserRaw]()
	{
		auto const& items = browserRaw->getItems();
		auto row = browserRaw->getCurrentRow();
		if (row < 0 || row >= (int32_t)items.size()) return;
		statusRaw->setText("已打开 " + items[row]);
	});
	pin->toggled->connect(nullptr, [statusRaw, browserRaw](bool on)
	{
		auto const& items = browserRaw->getItems();
		auto row = browserRaw->getCurrentRow();
		UIString target = row >= 0 && row < (int32_t)items.size() ? items[row] : UIString("当前项");
		statusRaw->setText(on ? "已固定 " + target : "已取消固定 " + target);
	});

	statusRaw->setText("就绪");
}
