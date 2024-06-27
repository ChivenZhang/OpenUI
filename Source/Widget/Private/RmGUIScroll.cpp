#include "RmGUIScroll.h"
#include "RmGUIScroll.h"
#include "RmGUIScroll.h"

class RmGUIScrollPrivate : public RmGUIWidgetPrivate
{
public:
	RmGUIScroll::policy_t HorizontalPolicy = RmGUIScroll::PolicyAsNeeded;
	RmGUIScroll::policy_t VerticalPolicy = RmGUIScroll::PolicyAsNeeded;
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
	auto viewport = getViewport();

	if (2 < getChildren().size())
	{
		auto contentWidget = getChildren()[2];
		auto contentFixedW = contentWidget->getFixedWidth();
		auto contentFixedH = contentWidget->getFixedHeight();
		auto scrollHFixedH = PRIVATE()->HorizontalScrollBar->getFixedHeight();
		auto scrollVFixedW = PRIVATE()->VerticalScrollBar->getFixedWidth();

		auto ShowHShowV = [=]()->bool {
			auto HScrollX = client->X;
			auto HScrollY = client->Y + client->H - scrollHFixedH;
			auto HScrollW = client->W - scrollVFixedW;
			auto HScrollH = scrollHFixedH;

			auto VScrollX = client->X + client->W - scrollVFixedW;
			auto VScrollY = client->Y;
			auto VScrollW = scrollVFixedW;
			auto VScrollH = client->H - scrollHFixedH;

			auto ContentX = client->X;
			auto ContentY = client->Y;
			auto ContentW = HScrollW;
			auto ContentH = VScrollH;

			auto RealContentW = std::isnan(contentFixedW) ? ContentW : contentFixedW;
			auto RealContentH = std::isnan(contentFixedH) ? ContentH : contentFixedH;
			auto RealContentX = client->X - (PRIVATE()->HorizontalScrollBar->getValue() / RealContentW * (RealContentH - ContentH));
			auto RealContentY = client->Y - (PRIVATE()->VerticalScrollBar->getValue() / RealContentH * (RealContentH - ContentH));

			auto result = ContentW < RealContentW && ContentH < RealContentH;
			if (result)
			{
				PRIVATE()->HorizontalScrollBar->setVisible(true);
				PRIVATE()->VerticalScrollBar->setVisible(true);

				PRIVATE()->HorizontalScrollBar->setRect({ HScrollX, HScrollY, HScrollW, HScrollH });
				PRIVATE()->HorizontalScrollBar->setViewport(RmOverlap(viewport, PRIVATE()->HorizontalScrollBar->getRect()));
				PRIVATE()->HorizontalScrollBar->setRange(0, RealContentW);

				PRIVATE()->VerticalScrollBar->setRect({ VScrollX, VScrollY, VScrollW, VScrollH });
				PRIVATE()->VerticalScrollBar->setViewport(RmOverlap(viewport, PRIVATE()->VerticalScrollBar->getRect()));
				PRIVATE()->VerticalScrollBar->setRange(0, RealContentH);

				contentWidget->setRect({ RealContentX, RealContentY, RealContentW, RealContentH });
				contentWidget->setViewport(RmOverlap(viewport, contentWidget->getRect()));
			}
			return result;
			};
		auto ShowHHideV = [=]()->bool {
			auto HScrollX = client->X;
			auto HScrollY = client->Y + client->H - scrollHFixedH;
			auto HScrollW = client->W;
			auto HScrollH = scrollHFixedH;

			auto VScrollX = client->X + client->W;
			auto VScrollY = client->Y;
			auto VScrollW = 0.0f;
			auto VScrollH = client->H - scrollHFixedH;

			auto ContentX = client->X;
			auto ContentY = client->Y;
			auto ContentW = HScrollW;
			auto ContentH = VScrollH;

			auto RealContentW = std::isnan(contentFixedW) ? ContentW : contentFixedW;
			auto RealContentH = std::isnan(contentFixedH) ? ContentH : contentFixedH;
			auto RealContentX = client->X - (PRIVATE()->HorizontalScrollBar->getValue() / RealContentW * (RealContentH - ContentH));
			auto RealContentY = client->Y - (PRIVATE()->VerticalScrollBar->getValue() / RealContentH * (RealContentH - ContentH));

			auto result = ContentW < RealContentW && ContentH >= RealContentH;
			if (result)
			{
				PRIVATE()->HorizontalScrollBar->setVisible(true);
				PRIVATE()->VerticalScrollBar->setVisible(false);

				PRIVATE()->HorizontalScrollBar->setRect({ HScrollX, HScrollY, HScrollW, HScrollH });
				PRIVATE()->HorizontalScrollBar->setViewport(RmOverlap(viewport, PRIVATE()->HorizontalScrollBar->getRect()));
				PRIVATE()->HorizontalScrollBar->setRange(0, RealContentW);

				PRIVATE()->VerticalScrollBar->setRect({ VScrollX, VScrollY, VScrollW, VScrollH });
				PRIVATE()->VerticalScrollBar->setViewport(RmOverlap(viewport, PRIVATE()->VerticalScrollBar->getRect()));
				PRIVATE()->VerticalScrollBar->setRange(0, RealContentH);

				contentWidget->setRect({ RealContentX, RealContentY, RealContentW, RealContentH });
				contentWidget->setViewport(RmOverlap(viewport, contentWidget->getRect()));
			}
			return result;
			};
		auto HideHShowV = [=]()->bool {
			auto HScrollX = client->X;
			auto HScrollY = client->Y + client->H;
			auto HScrollW = client->W - scrollVFixedW;
			auto HScrollH = 0.0f;

			auto VScrollX = client->X + client->W - scrollVFixedW;
			auto VScrollY = client->Y;
			auto VScrollW = scrollVFixedW;
			auto VScrollH = client->H;

			auto ContentX = client->X;
			auto ContentY = client->Y;
			auto ContentW = HScrollW;
			auto ContentH = VScrollH;

			auto RealContentW = std::isnan(contentFixedW) ? ContentW : contentFixedW;
			auto RealContentH = std::isnan(contentFixedH) ? ContentH : contentFixedH;
			auto RealContentX = client->X - (PRIVATE()->HorizontalScrollBar->getValue() / RealContentW * (RealContentH - ContentH));
			auto RealContentY = client->Y - (PRIVATE()->VerticalScrollBar->getValue() / RealContentH * (RealContentH - ContentH));

			auto result = ContentW >= RealContentW && ContentH < RealContentH;
			if (result)
			{
				PRIVATE()->HorizontalScrollBar->setVisible(false);
				PRIVATE()->VerticalScrollBar->setVisible(true);

				PRIVATE()->HorizontalScrollBar->setRect({ HScrollX, HScrollY, HScrollW, HScrollH });
				PRIVATE()->HorizontalScrollBar->setViewport(RmOverlap(viewport, PRIVATE()->HorizontalScrollBar->getRect()));
				PRIVATE()->HorizontalScrollBar->setRange(0, RealContentW);

				PRIVATE()->VerticalScrollBar->setRect({ VScrollX, VScrollY, VScrollW, VScrollH });
				PRIVATE()->VerticalScrollBar->setViewport(RmOverlap(viewport, PRIVATE()->VerticalScrollBar->getRect()));
				PRIVATE()->VerticalScrollBar->setRange(0, RealContentH);

				contentWidget->setRect({ RealContentX, RealContentY, RealContentW, RealContentH });
				contentWidget->setViewport(RmOverlap(viewport, contentWidget->getRect()));
			}
			return result;
			};
		auto HideHHideV = [=]()->bool {
			auto HScrollX = client->X;
			auto HScrollY = client->Y + client->H;
			auto HScrollW = client->W;
			auto HScrollH = 0.0f;

			auto VScrollX = client->X + client->W;
			auto VScrollY = client->Y;
			auto VScrollW = 0.0f;
			auto VScrollH = client->H;

			auto ContentX = client->X;
			auto ContentY = client->Y;
			auto ContentW = HScrollW;
			auto ContentH = VScrollH;

			auto RealContentW = std::isnan(contentFixedW) ? ContentW : contentFixedW;
			auto RealContentH = std::isnan(contentFixedH) ? ContentH : contentFixedH;
			auto RealContentX = client->X - (PRIVATE()->HorizontalScrollBar->getValue() / RealContentW * (RealContentH - ContentH));
			auto RealContentY = client->Y - (PRIVATE()->VerticalScrollBar->getValue() / RealContentH * (RealContentH - ContentH));

			auto result = ContentW >= RealContentW && ContentH >= RealContentH;
			if (result)
			{
				PRIVATE()->HorizontalScrollBar->setVisible(false);
				PRIVATE()->VerticalScrollBar->setVisible(false);

				PRIVATE()->HorizontalScrollBar->setRect({ HScrollX, HScrollY, HScrollW, HScrollH });
				PRIVATE()->HorizontalScrollBar->setViewport(RmOverlap(viewport, PRIVATE()->HorizontalScrollBar->getRect()));
				PRIVATE()->HorizontalScrollBar->setRange(0, RealContentW);

				PRIVATE()->VerticalScrollBar->setRect({ VScrollX, VScrollY, VScrollW, VScrollH });
				PRIVATE()->VerticalScrollBar->setViewport(RmOverlap(viewport, PRIVATE()->VerticalScrollBar->getRect()));
				PRIVATE()->VerticalScrollBar->setRange(0, RealContentH);

				contentWidget->setRect({ RealContentX, RealContentY, RealContentW, RealContentH });
				contentWidget->setViewport(RmOverlap(viewport, contentWidget->getRect()));
			}
			return result;
			};

		if (ShowHShowV());
		else if (ShowHHideV());
		else if (HideHShowV());
		else if (HideHHideV());
	}
	else
	{
		PRIVATE()->HorizontalScrollBar->setVisible(false);
		PRIVATE()->VerticalScrollBar->setVisible(false);
	}

}

void RmGUIScroll::paint(IRmGUIPainterRaw painter, RmRectRaw client)
{
	RmGUIWidget::paint(painter, client);
	painter->setPen({ .Color = { 108 / 255.0f, 110 / 255.0f, 111 / 255.0f, 1.0f }, });
	painter->setBrush({ .Color = { 238 / 255.0f, 238 / 255.0f, 242 / 255.0f, 1.0f }, });
	painter->drawRect(client->X + 1, client->Y + 1, client->W - 2, client->H - 2);
}

void RmGUIScroll::removeWidget()
{
	RmVector<IRmGUIWidgetRef> result;
	for (size_t i = 2; i < getChildren().size(); ++i) result.push_back(getChildren()[i]);
	for (size_t i = 0; i < result.size(); ++i) removeWidget(result[i]);
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

void RmGUIScroll::mouseMoveEvent(IRmGUIMouseEventRaw event)
{
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
