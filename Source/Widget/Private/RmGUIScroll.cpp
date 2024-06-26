#include "RmGUIScroll.h"

class RmGUIScrollPrivate : public RmGUIWidgetPrivate
{
public:
	RmGUIScroll::policy_t HorizontalPolicy = RmGUIScroll::ScrollBarAsNeeded;
	RmGUIScroll::policy_t VerticalPolicy = RmGUIScroll::ScrollBarAsNeeded;
	RmGUIScrollBarRef HorizontalScrollBar;
	RmGUIScrollBarRef VerticalScrollBar;
};
#define PRIVATE() ((RmGUIScrollPrivate*)m_PrivateScroll)

RmGUIScroll::RmGUIScroll(IRmGUIWidgetRaw parent)
	:
	RmGUILayout(parent),
	m_PrivateScroll(nullptr)
{
	m_PrivateScroll = new RmGUIScrollPrivate;
	PRIVATE()->HorizontalScrollBar = RmNew<RmGUIScrollBar>();
	addWidget(PRIVATE()->HorizontalScrollBar);
	PRIVATE()->HorizontalScrollBar->setFixedHeight(16);
	PRIVATE()->HorizontalScrollBar->setMinWidth(16);
	PRIVATE()->HorizontalScrollBar->setOrientation(true);
	PRIVATE()->HorizontalScrollBar->setSingleStep(80);

	PRIVATE()->VerticalScrollBar = RmNew<RmGUIScrollBar>();
	addWidget(PRIVATE()->VerticalScrollBar);
	PRIVATE()->VerticalScrollBar->setFixedWidth(16);
	PRIVATE()->VerticalScrollBar->setMinHeight(16);
	PRIVATE()->VerticalScrollBar->setOrientation(false);
	PRIVATE()->VerticalScrollBar->setSingleStep(80);
}

RmGUIScroll::~RmGUIScroll()
{
	delete m_PrivateScroll; m_PrivateScroll = nullptr;
}

void RmGUIScroll::layout(RmRectRaw client)
{
	auto scrollBarX = client->X;
	auto scrollBarY = client->Y + client->H - PRIVATE()->HorizontalScrollBar->getFixedHeight();
	auto scrollBarW = client->W - PRIVATE()->VerticalScrollBar->getFixedWidth();
	auto scrollBarH = PRIVATE()->HorizontalScrollBar->getFixedHeight();
	PRIVATE()->HorizontalScrollBar->setRect({ scrollBarX, scrollBarY, scrollBarW, scrollBarH });
	PRIVATE()->HorizontalScrollBar->setViewport(PRIVATE()->HorizontalScrollBar->getRect());

	scrollBarX = client->X + client->W - PRIVATE()->VerticalScrollBar->getFixedWidth();
	scrollBarY = client->Y;
	scrollBarW = PRIVATE()->VerticalScrollBar->getFixedWidth();
	scrollBarH = client->H - PRIVATE()->HorizontalScrollBar->getFixedHeight();
	PRIVATE()->VerticalScrollBar->setRect({ scrollBarX, scrollBarY, scrollBarW, scrollBarH });
	PRIVATE()->VerticalScrollBar->setViewport(PRIVATE()->VerticalScrollBar->getRect());

	if (2 < getChildren().size())
	{
		auto contentWidget = getChildren()[2];

		auto contentX = client->X - PRIVATE()->HorizontalScrollBar->getValue();
		auto contentY = client->Y - PRIVATE()->VerticalScrollBar->getValue();
		auto contentW = contentWidget->getRect().W;
		auto contentH = contentWidget->getRect().H;
		contentWidget->setRect({ contentX, contentY, contentW, contentH });
		contentX = client->X;
		contentY = client->Y;
		contentW = client->W - PRIVATE()->VerticalScrollBar->getFixedWidth();
		contentH = client->H - PRIVATE()->HorizontalScrollBar->getFixedHeight();
		contentWidget->setViewport({ contentX, contentY, contentW, contentH });

		PRIVATE()->HorizontalScrollBar->setRange(0, contentWidget->getRect().W - contentWidget->getViewport().W);
		PRIVATE()->VerticalScrollBar->setRange(0, contentWidget->getRect().H - contentWidget->getViewport().H);
	}
}

void RmGUIScroll::paint(IRmGUIPainterRaw painter, RmRectRaw client)
{
	RmGUIWidget::paint(painter, client);
	painter->setPen({ .Color = { 108 / 255.0f, 110 / 255.0f, 111 / 255.0f, 1.0f }, });
	painter->setBrush({ .Color = { 238 / 255.0f, 238 / 255.0f, 242 / 255.0f, 1.0f }, });
	painter->drawRect(client->X + 1, client->Y + 1, client->W - 2, client->H - 2);
}

RmGUIScroll::policy_t RmGUIScroll::getHorizontalPolicy() const
{
	return PRIVATE()->HorizontalPolicy;
}

void RmGUIScroll::setHorizontalPolicy(policy_t value)
{
	PRIVATE()->HorizontalPolicy = value;
}

RmGUIScrollBarRaw RmGUIScroll::getHorizontalBar() const
{
	return PRIVATE()->HorizontalScrollBar.get();
}

RmGUIScroll::policy_t RmGUIScroll::getVerticalPolicy() const
{
	return PRIVATE()->VerticalPolicy;
}

void RmGUIScroll::setVerticalPolicy(policy_t value)
{
	PRIVATE()->VerticalPolicy = value;
}

RmGUIScrollBarRaw RmGUIScroll::getVerticalBar() const
{
	return PRIVATE()->VerticalScrollBar.get();
}

void RmGUIScroll::wheelEvent(IRmGUIMouseWheelEventRaw event)
{
	auto client = getRect();
	auto viewport = getViewport();
	if (client.X <= event->X && event->X <= client.X + client.W
		&& client.Y <= event->Y && event->Y <= client.Y + client.H
		&& viewport.X <= event->X && event->X <= viewport.X + viewport.W
		&& viewport.Y <= event->Y && event->Y <= viewport.Y + viewport.H)
	{
		getHorizontalBar()->setValue(getHorizontalBar()->getValue() + event->AngleX * getHorizontalBar()->getSingleStep());
		getVerticalBar()->setValue(getVerticalBar()->getValue() - event->AngleY * getVerticalBar()->getSingleStep());
		event->Accept = true;
	}
}
