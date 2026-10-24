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
#include "../UIScroll.h"
#include "../UICanvas.h"

struct UIScrollPrivate : UIPrivate
{
	UIScroll::policy_t HorizontalPolicy = UIScroll::PolicyAsNeeded;
	UIScroll::policy_t VerticalPolicy = UIScroll::PolicyAsNeeded;
	UIScrollBarRef HorizonScrollBar;
	UIScrollBarRef VerticalScrollBar;
	UIScrollStyle Style;
};
#define PRIVATE() ((UIScrollPrivate*)m_Private)

UIScroll::UIScroll(UICanvasRaw canvas)
	:
	UIWidget(canvas)
{
	m_Private = new UIScrollPrivate;

	PRIVATE()->HorizonScrollBar = UINew<UIScrollBar>(canvas);
	addWidget(PRIVATE()->HorizonScrollBar);
	PRIVATE()->HorizonScrollBar->setFixedHeight({12, UI_CSS_HEIGHT_LENGTH});
	PRIVATE()->HorizonScrollBar->setMinWidth({12, UI_CSS_MIN_WIDTH_LENGTH});
	PRIVATE()->HorizonScrollBar->setOrientation(UI::Horizontal);
	PRIVATE()->HorizonScrollBar->setSingleStep(80);
	auto css = [](UIColor color)
	{
		return "rgba("
			+ std::to_string((int)(color.R * 255.0f + 0.5f)) + ","
			+ std::to_string((int)(color.G * 255.0f + 0.5f)) + ","
			+ std::to_string((int)(color.B * 255.0f + 0.5f)) + ","
			+ std::to_string((int)(color.A * 255.0f + 0.5f)) + ")";
	};
	auto bindHandle = [&](UIButtonRaw handle)
	{
		handle->setStyleText("border-radius", "6px");
		handle->setStyleText("border-color", "rgba(0,0,0,0)");
		handle->setStyleText("border-color:hover", "rgba(0,0,0,0)");
		handle->setStyleText("border-color:active", "rgba(0,0,0,0)");
		handle->setStyleText("border-width", "0px");
		handle->setStyleText("background-color", css(UIThumbBrush.Color));
		handle->setStyleText("background-color:hover", css(UIAccentBrush.Color));
		handle->setStyleText("background-color:active", css(UIAccentBrush.Color));
	};
	bindHandle(PRIVATE()->HorizonScrollBar->getHandle());

	PRIVATE()->VerticalScrollBar = UINew<UIScrollBar>(canvas);
	addWidget(PRIVATE()->VerticalScrollBar);
	PRIVATE()->VerticalScrollBar->setFixedWidth({12, UI_CSS_WIDTH_LENGTH});
	PRIVATE()->VerticalScrollBar->setMinHeight({12, UI_CSS_MIN_HEIGHT_LENGTH});
	PRIVATE()->VerticalScrollBar->setOrientation(UI::Vertical);
	PRIVATE()->VerticalScrollBar->setSingleStep(80);
	bindHandle(PRIVATE()->VerticalScrollBar->getHandle());
}

UIScroll::~UIScroll()
{
	delete m_Private; m_Private = nullptr;
}

void UIScroll::arrange(UIRect client)
{
	this->setJustifyContent({UI_CSS_JUSTIFY_CONTENT_FLEX_END});

	PRIVATE()->HorizonScrollBar->setFixedHeight({12 * getCanvas()->getConfig().DisplayScale, UI_CSS_HEIGHT_LENGTH});
	PRIVATE()->HorizonScrollBar->setMinWidth({12 * getCanvas()->getConfig().DisplayScale, UI_CSS_MIN_WIDTH_LENGTH});

	PRIVATE()->VerticalScrollBar->setFixedWidth({12 * getCanvas()->getConfig().DisplayScale, UI_CSS_WIDTH_LENGTH});
	PRIVATE()->VerticalScrollBar->setMinHeight({12 * getCanvas()->getConfig().DisplayScale, UI_CSS_MIN_HEIGHT_LENGTH});

	if (2 < getWidgets().size())
	{
		getWidgets()[2]->setFixedPos({0, UI_CSS_WIDTH_LENGTH}, {0, UI_CSS_HEIGHT_LENGTH});
		getWidgets()[2]->setPositionType({UI_CSS_POSITION_ABSOLUTE});
	}
}

void UIScroll::layout(UIRect client)
{
	if (2 < getWidgets().size())
	{
		auto content = getWidgets()[2];

		auto showH = false, showV = false;
		switch (PRIVATE()->HorizontalPolicy)
		{
		case UIScroll::PolicyAsNeeded: showH = client.W < content->getWidth(); break;
		case UIScroll::PolicyAlwaysOff: showH = false; break;
		case UIScroll::PolicyAlwaysOn: showH = true; break;
		}
		switch (PRIVATE()->VerticalPolicy)
		{
		case UIScroll::PolicyAsNeeded: showV = client.H < content->getHeight(); break;
		case UIScroll::PolicyAlwaysOff: showV = false; break;
		case UIScroll::PolicyAlwaysOn: showV = true; break;
		}
		PRIVATE()->HorizonScrollBar->setVisible(showH);
		PRIVATE()->VerticalScrollBar->setVisible(showV);

		auto width = PRIVATE()->VerticalScrollBar->getFixedWidth().Value;
		auto height = PRIVATE()->HorizonScrollBar->getFixedHeight().Value;

		if (showH)
		{
			if (showV)
			{
				PRIVATE()->HorizonScrollBar->setLocalBounds({ 0, client.H - height, client.W - width, height });
				PRIVATE()->VerticalScrollBar->setLocalBounds({ client.W - width, 0, width, client.H - height });
			}
			else
			{
				PRIVATE()->HorizonScrollBar->setLocalBounds({ 0, client.H - height, client.W, height });
				PRIVATE()->VerticalScrollBar->setLocalBounds({ 0, 0, 0, client.H - height });
			}
		}
		else
		{
			if (showV)
			{
				PRIVATE()->HorizonScrollBar->setLocalBounds({ 0, 0, client.W - width, 0 });
				PRIVATE()->VerticalScrollBar->setLocalBounds({ client.W - width, 0, width, client.H });
			}
			else
			{
				PRIVATE()->HorizonScrollBar->setLocalBounds({ 0, 0, client.W, 0 });
				PRIVATE()->VerticalScrollBar->setLocalBounds({ 0, 0, 0, client.H });
			}
		}

		auto bounds = PRIVATE()->HorizonScrollBar->getLocalBounds();
		PRIVATE()->HorizonScrollBar->setViewport({ client.X + bounds.X, client.Y + bounds.Y, bounds.W, bounds.H });
		PRIVATE()->HorizonScrollBar->setRange(0, std::max<int32_t>(0, content->getWidth()));

		bounds = PRIVATE()->VerticalScrollBar->getLocalBounds();
		PRIVATE()->VerticalScrollBar->setViewport({ client.X + bounds.X, client.Y + bounds.Y, bounds.W, bounds.H });
		PRIVATE()->VerticalScrollBar->setRange(0, std::max<int32_t>(0, content->getHeight()));

		content->setLocalBounds({
			showH ? -(PRIVATE()->HorizonScrollBar->getValue() / content->getWidth() * (content->getWidth() - client.W)) : 0,
			showV ? -(PRIVATE()->VerticalScrollBar->getValue() / content->getHeight() * (content->getHeight() - client.H)) : 0,
			content->getWidth(),
			content->getHeight(),
			});
		content->setViewport({
			client.X + 1,
			client.Y + 1,
			PRIVATE()->HorizonScrollBar->getLocalBounds().W - 2,
			PRIVATE()->VerticalScrollBar->getLocalBounds().H - 2
			});

	}
	else
	{
		PRIVATE()->HorizonScrollBar->setRange(0, 0);
		PRIVATE()->VerticalScrollBar->setRange(0, 0);

		auto showH = false, showV = false;
		switch (PRIVATE()->HorizontalPolicy)
		{
		case UIScroll::PolicyAsNeeded: showH = false; break;
		case UIScroll::PolicyAlwaysOff: showH = false; break;
		case UIScroll::PolicyAlwaysOn: showH = true; break;
		}
		switch (PRIVATE()->VerticalPolicy)
		{
		case UIScroll::PolicyAsNeeded: showV = false; break;
		case UIScroll::PolicyAlwaysOff: showV = false; break;
		case UIScroll::PolicyAlwaysOn: showV = true; break;
		}
		PRIVATE()->HorizonScrollBar->setVisible(showH);
		PRIVATE()->VerticalScrollBar->setVisible(showV);
	}
}

void UIScroll::paint(UIRect client, UIPainterRaw painter)
{
	UIWidget::paint(client, painter);

	painter->setPen(PRIVATE()->Style.Pen);
	painter->setBrush(PRIVATE()->Style.Brush);
	painter->drawRect(client.X, client.Y, client.W, client.H);
}

bool UIScroll::addWidget(UIWidgetRef value)
{
	removeWidget();
	return UIWidget::addWidget(value);
}

bool UIScroll::removeWidget(UIWidgetRef value)
{
	if (getWidgets().size() <= 2 || getWidgets()[2] != value) return false;
	return UIWidget::removeWidget(value);
}

void UIScroll::removeWidget()
{
	UIList<UIWidgetRef> result;
	for (size_t i = 2; i < getWidgets().size(); ++i) result.push_back(getWidgets()[i]);
	for (size_t i = 0; i < result.size(); ++i) removeWidget(result[i]);
}

UIScrollStyle UIScroll::getStyle() const
{
	return PRIVATE()->Style;
}

void UIScroll::setStyle(UIScrollStyle value)
{
	PRIVATE()->Style = value;
}

UIScroll::policy_t UIScroll::getHorizontalPolicy() const
{
	return PRIVATE()->HorizontalPolicy;
}

void UIScroll::setHorizontalPolicy(policy_t value)
{
	PRIVATE()->HorizontalPolicy = value;
}

UIScroll::policy_t UIScroll::getVerticalPolicy() const
{
	return PRIVATE()->VerticalPolicy;
}

void UIScroll::setVerticalPolicy(policy_t value)
{
	PRIVATE()->VerticalPolicy = value;
}

void UIScroll::setScrollPolicy(policy_t horizontal, policy_t vertical)
{
	setHorizontalPolicy(horizontal);
	setVerticalPolicy(vertical);
}

float UIScroll::getHorizontalValue() const
{
	return getHorizontalBar()->getValue();
}

void UIScroll::setHorizontalValue(float value)
{
	getHorizontalBar()->setValue(value);
}

float UIScroll::getVerticalValue() const
{
	return getVerticalBar()->getValue();
}

void UIScroll::setVerticalValue(float value)
{
	getVerticalBar()->setValue(value);
}

UIScrollBarRaw UIScroll::getHorizontalBar() const
{
	return PRIVATE()->HorizonScrollBar.get();
}

UIScrollBarRaw UIScroll::getVerticalBar() const
{
	return PRIVATE()->VerticalScrollBar.get();
}

UIWidgetRaw UIScroll::getContentView() const
{
	return (2 < getWidgets().size()) ? getWidgets()[2].get() : nullptr;
}

void UIScroll::wheelEvent(UIMouseWheelEventRaw event)
{
	if (inBounds(event->X, event->Y))
	{
		getHorizontalBar()->setValue(getHorizontalBar()->getValue() + event->AngleX * getHorizontalBar()->getSingleStep());
		getVerticalBar()->setValue(getVerticalBar()->getValue() - event->AngleY * getVerticalBar()->getSingleStep());
		event->Accept = true;

		if (getCanvas()) getCanvas()->layoutWidget();
	}
}