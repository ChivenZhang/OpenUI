/*=================================================
* Copyright © 2020-2025 ChivenZhang.
* All Rights Reserved.
* =====================Note=========================
*
*
* ====================History=======================
* Created by ChivenZhang@gmail.com.
*
* =================================================*/
#include "../UIBuilder.h"
#include "../UIParser.h"

class UIBuilderPrivate : public UIWidgetPrivate
{
public:
	UICanvasRaw Canvas;
	UIStringMap<UIFactoryRef> FactoryMap;
};
#define PRIVATE() ((UIBuilderPrivate*) m_Private)

UIBuilder::UIBuilder(UICanvasRaw canvas)
{
	m_Private = new UIBuilderPrivate;

	PRIVATE()->Canvas = canvas;
}

UIBuilder::~UIBuilder()
{
	delete m_Private;
	m_Private = nullptr;
}

UICanvasRaw UIBuilder::getCanvas() const
{
	return PRIVATE()->Canvas;
}

bool UIBuilder::addFactory(UIFactoryRef value)
{
	if (value == nullptr) return false;
	PRIVATE()->FactoryMap[value->getTagName()] = value;
	return true;
}

bool UIBuilder::removeFactory(UIString name)
{
	return PRIVATE()->FactoryMap.erase(name);
}

void UIBuilder::removeFactory()
{
	PRIVATE()->FactoryMap.clear();
}

UIWidgetRef UIBuilder::buildWidget(UIString html) const
{
	UIParser parser;
	UIParser::widget_t widget;
	if (parser.parse(html, widget))
	{
		// Process the parsed widget

		UILambda<void(UIParser::widget_t&, uint32_t)> process_func;
		process_func = [&process_func](UIParser::widget_t& w, uint32_t depth)
		{
			if (w.Type.empty()) return;

			// Process each widget

			std::cout << UIString(depth * 4, ' ');
			std::cout << "<" << w.Type << ">" << std::endl;

			for (auto& a : w.Attrib)
			{
				std::cout << UIString((depth + 1) * 4, ' ');
				std::cout << "-attr :" << a.Name << " = " << a.Value << std::endl;
			}
			for (auto& s : w.Style)
			{
				std::cout << UIString((depth + 1) * 4, ' ');
				std::cout << "-style:" << s.Name << " = " << s.Value << " -priority " << s.Priority << std::endl;
			}

			for (auto& c : w.Children) process_func(c, depth + 1);

			std::cout << UIString(depth * 4, ' ');
			std::cout << "</" << w.Type << ">" << std::endl;
		};
		process_func(widget, 0);
	}
	return {};
}
