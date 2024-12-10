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
#include "../UIContext.h"

class UIScrollPrivate : public UIElementPrivate
{
public:
	UIScroll::policy_t HorizontalPolicy = UIScroll::PolicyAsNeeded;
	UIScroll::policy_t VerticalPolicy = UIScroll::PolicyAsNeeded;
	UIScrollBarRef HorizontalScrollBar;
	UIScrollBarRef VerticalScrollBar;
	UIScrollStyle Style;
};
#define PRIVATE() ((UIScrollPrivate*)m_PrivateScroll)

UIScroll::UIScroll()
{
	m_PrivateScroll = new UIScrollPrivate;

	PRIVATE()->HorizontalScrollBar = UINew<UIScrollBar>();
	addElement(PRIVATE()->HorizontalScrollBar);
	PRIVATE()->HorizontalScrollBar->setFixedHeight(12);
	PRIVATE()->HorizontalScrollBar->setMinWidth(12);
	PRIVATE()->HorizontalScrollBar->setOrientation(UI::Horizontal);
	PRIVATE()->HorizontalScrollBar->setSingleStep(80);
	auto style = PRIVATE()->HorizontalScrollBar->getHandle()->getStyle();
	style.Round = { 0, 0 };
	style.Normal.Pen = UIFramePen;
	PRIVATE()->HorizontalScrollBar->getHandle()->setStyle(style);

	PRIVATE()->VerticalScrollBar = UINew<UIScrollBar>();
	addElement(PRIVATE()->VerticalScrollBar);
	PRIVATE()->VerticalScrollBar->setFixedWidth(12);
	PRIVATE()->VerticalScrollBar->setMinHeight(12);
	PRIVATE()->VerticalScrollBar->setOrientation(UI::Vertical);
	PRIVATE()->VerticalScrollBar->setSingleStep(80);
	style = PRIVATE()->VerticalScrollBar->getHandle()->getStyle();
	style.Round = { 0, 0 };
	style.Normal.Pen = UIFramePen;
	PRIVATE()->VerticalScrollBar->getHandle()->setStyle(style);
}

UIScroll::~UIScroll()
{
	delete m_PrivateScroll; m_PrivateScroll = nullptr;
}

void UIScroll::arrange(UIRect client)
{
	this->setJustifyContent(UI::JustifyFlexEnd);

	PRIVATE()->HorizontalScrollBar->setFixedHeight(12 * getContext()->getConfig().ScaleFactor);
	PRIVATE()->HorizontalScrollBar->setMinWidth(12 * getContext()->getConfig().ScaleFactor);

	PRIVATE()->VerticalScrollBar->setFixedWidth(12 * getContext()->getConfig().ScaleFactor);
	PRIVATE()->VerticalScrollBar->setMinHeight(12 * getContext()->getConfig().ScaleFactor);

	if (2 < getChildren().size())
	{
		getChildren()[2]->setFixedPos(0, 0);
		getChildren()[2]->setPositionType(UI::PositionAbsolute);
	}
}

void UIScroll::layout(UIRect client)
{
	if (2 < getChildren().size())
	{
		auto content = getChildren()[2];

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
		PRIVATE()->HorizontalScrollBar->setVisible(showH);
		PRIVATE()->VerticalScrollBar->setVisible(showV);

		auto width = PRIVATE()->VerticalScrollBar->getFixedWidth();
		auto height = PRIVATE()->HorizontalScrollBar->getFixedHeight();

		if (showH)
		{
			if (showV)
			{
				PRIVATE()->HorizontalScrollBar->setLocalBounds({ 0, client.H - height, client.W - width, height });
				PRIVATE()->VerticalScrollBar->setLocalBounds({ client.W - width, 0, width, client.H - height });
			}
			else
			{
				PRIVATE()->HorizontalScrollBar->setLocalBounds({ 0, client.H - height, client.W, height });
				PRIVATE()->VerticalScrollBar->setLocalBounds({ 0, 0, 0, client.H - height });
			}
		}
		else
		{
			if (showV)
			{
				PRIVATE()->HorizontalScrollBar->setLocalBounds({ 0, 0, client.W - width, 0 });
				PRIVATE()->VerticalScrollBar->setLocalBounds({ client.W - width, 0, width, client.H });
			}
			else
			{
				PRIVATE()->HorizontalScrollBar->setLocalBounds({ 0, 0, client.W, 0 });
				PRIVATE()->VerticalScrollBar->setLocalBounds({ 0, 0, 0, client.H });
			}
		}

		auto bounds = PRIVATE()->HorizontalScrollBar->getLocalBounds();
		PRIVATE()->HorizontalScrollBar->setViewport({ client.X + bounds.X, client.Y + bounds.Y, bounds.W, bounds.H });
		PRIVATE()->HorizontalScrollBar->setRange(0, std::max<int32_t>(0, content->getWidth()));

		bounds = PRIVATE()->VerticalScrollBar->getLocalBounds();
		PRIVATE()->VerticalScrollBar->setViewport({ client.X + bounds.X, client.Y + bounds.Y, bounds.W, bounds.H });
		PRIVATE()->VerticalScrollBar->setRange(0, std::max<int32_t>(0, content->getHeight()));

		content->setLocalBounds({
			showH ? -(PRIVATE()->HorizontalScrollBar->getValue() / content->getWidth() * (content->getWidth() - client.W)) : 0,
			showV ? -(PRIVATE()->VerticalScrollBar->getValue() / content->getHeight() * (content->getHeight() - client.H)) : 0,
			content->getWidth(),
			content->getHeight(),
			});
		content->setViewport({
			client.X + 1,
			client.Y + 1,
			PRIVATE()->HorizontalScrollBar->getLocalBounds().W - 2,
			PRIVATE()->VerticalScrollBar->getLocalBounds().H - 2
			});

	}
	else
	{
		PRIVATE()->HorizontalScrollBar->setRange(0, 0);
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
		PRIVATE()->HorizontalScrollBar->setVisible(showH);
		PRIVATE()->VerticalScrollBar->setVisible(showV);
	}
}

void UIScroll::paint(UIRect client, UIPainterRaw painter)
{
	UIElement::paint(client, painter);
	painter->setPen(UINoPen);
	painter->setBrush(PRIVATE()->Style.Brush);
	painter->drawRect(client.X, client.Y, client.W, client.H);
}

void UIScroll::repaint(UIRect client, UIPainterRaw painter)
{
	UIElement::repaint(client, painter);
	painter->setPen(PRIVATE()->Style.Pen);
	painter->setBrush(UINoBrush);
	painter->drawRect(client.X, client.Y, client.W, client.H);
}

bool UIScroll::addElement(UIElementRef value)
{
	removeElement();
	return UIElement::addElement(value);
}

bool UIScroll::removeElement(UIElementRef value)
{
	if (getChildren().size() <= 2 || getChildren()[2] != value) return false;
	return UIElement::removeElement(value);
}

void UIScroll::removeElement()
{
	UIList<UIElementRef> result;
	for (size_t i = 2; i < getChildren().size(); ++i) result.push_back(getChildren()[i]);
	for (size_t i = 0; i < result.size(); ++i) removeElement(result[i]);
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

void UIScroll::setVerticallValue(float value)
{
	getVerticalBar()->setValue(value);
}

UIScrollBarRaw UIScroll::getHorizontalBar() const
{
	return PRIVATE()->HorizontalScrollBar.get();
}

UIScrollBarRaw UIScroll::getVerticalBar() const
{
	return PRIVATE()->VerticalScrollBar.get();
}

UIElementRaw UIScroll::getContentView() const
{
	return (2 < getChildren().size()) ? getChildren()[2].get() : nullptr;
}

void UIScroll::wheelEvent(UIMouseWheelEventRaw event)
{
	if (inBounds(event->X, event->Y))
	{
		getHorizontalBar()->setValue(getHorizontalBar()->getValue() + event->AngleX * getHorizontalBar()->getSingleStep());
		getVerticalBar()->setValue(getVerticalBar()->getValue() - event->AngleY * getVerticalBar()->getSingleStep());
		event->Accept = true;

		if (getContext()) getContext()->layoutElement();
	}
}

UIString UIScrollFactory::getTagName() const
{
	return "scroll";
}

UIElementRef UIScrollFactory::getElement(UIString style) const
{
	auto result = UINew<UIScroll>();
	result->setStyleText(style);
	return result;
}