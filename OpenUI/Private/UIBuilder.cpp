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

bool UIBuilder::addFactory(UIString tag, UIFactoryRef value)
{
	if (value == nullptr) return false;
	PRIVATE()->FactoryMap[tag] = value;
	return true;
}

bool UIBuilder::removeFactory(UIString tag)
{
	return PRIVATE()->FactoryMap.erase(tag);
}

void UIBuilder::removeFactory()
{
	PRIVATE()->FactoryMap.clear();
}

UIWidgetRef UIBuilder::buildWidget(UIString html) const
{
	UIParser parser;
	UIParser::widget_t document;
	if (parser.parse(html, document))
	{
		// Process the parsed widget

		UILambda<UIWidgetRef(UIParser::widget_t&, uint32_t)> process_func;
		process_func = [&process_func, this](UIParser::widget_t& w, uint32_t depth)->UIWidgetRef
		{
			auto factory = PRIVATE()->FactoryMap.find(w.Type);
			if (factory == PRIVATE()->FactoryMap.end() || factory->second == nullptr)
			{
				UI_ERROR("cannot find '%s' factory", w.Type.c_str());
				return {};
			}

			auto widget = factory->second->newWidget();
			if (widget == nullptr) return {};
			widget->setAttribute("textContent", w.Text);

			// Process each widget

			std::cout << UIString(depth * 4, ' ');
			std::cout << "<" << w.Type << ">" << std::endl;

			for (auto& a : w.Attrib)
			{
				widget->setAttribute(a.Name, a.Value);

				std::cout << UIString((depth + 1) * 4, ' ');
				std::cout << "-attr :" << a.Name << " = " << a.Value << std::endl;
			}
			for (auto& s : w.Style)
			{
				widget->setStyleText(s.Name, s.Value);

				std::cout << UIString((depth + 1) * 4, ' ');
				std::cout << "-style:" << s.Name << " = " << s.Value << " -priority " << s.Priority << std::endl;
			}

			for (auto& c : w.Children)
			{
				if (auto child = process_func(c, depth + 1))
				{
					widget->addWidget(child);
				}
			}

			std::cout << UIString(depth * 4, ' ');
			std::cout << "</" << w.Type << ">" << std::endl;

			return widget;
		};

		return process_func(document.Children.front(), 0);
	}
	return {};
}
