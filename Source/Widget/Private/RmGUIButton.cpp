#include "RmGUIButton.h"
#include "RmGUILabel.h"
#include <taitank.h>
namespace flex = taitank;

static struct RmGUIButtonStyle
{
	RmPen Pen;
	RmBrush Brush;
	RmFloat2 Round = { 4, 4 };
};

static struct RmGUIButtonTextStyle
{
	RmPen Pen;
	RmFont Font;
	RmBrush Brush;
};

class RmGUIButtonPrivate : public RmGUIWidgetPrivate
{
public:
	RmGUIButtonStyle Normal, Hover, Press, Disable;
	RmGUILabelRef LabelWidget;
	RmGUISignalAs<> OnClicked;
	bool Pressed = false;
	bool Hovered = false;
};
#define PRIVATE() ((RmGUIButtonPrivate*) m_PrivateButton)

RmGUIButton::RmGUIButton(IRmGUIWidgetRaw parent)
	:
	RmGUIControl(parent),
	m_PrivateButton(nullptr)
{
	m_PrivateButton = new RmGUIButtonPrivate;

	PRIVATE()->Normal.Pen = { .Color = {0 / 255.0f, 120 / 255.0f, 212 / 255.0f, 1.0f} };
	PRIVATE()->Normal.Brush = { .Color = {253 / 255.0f, 253 / 255.0f, 253 / 255.0f, 1.0f} };
	PRIVATE()->Hover.Pen = { .Color = {0 / 255.0f, 120 / 255.0f, 212 / 255.0f, 1.0f} };
	PRIVATE()->Hover.Brush = { .Color = {224 / 255.0f, 238 / 255.0f, 249 / 255.0f, 1.0f} };
	PRIVATE()->Press.Pen = { .Color = {0 / 255.0f, 84 / 255.0f, 153 / 255.0f, 1.0f} };
	PRIVATE()->Press.Brush = { .Color = {204 / 255.0f, 228 / 255.0f, 247 / 255.0f, 1.0f} };
	onClicked = &PRIVATE()->OnClicked;
	PRIVATE()->LabelWidget = RmNew<RmGUILabel>();
	PRIVATE()->LabelWidget->setTextAlignment(RmFont::AlignCenter | RmFont::AlignVCenter);
	addWidget(PRIVATE()->LabelWidget);
}

RmGUIButton::~RmGUIButton()
{
	delete m_PrivateButton; m_PrivateButton = nullptr;
}

void RmGUIButton::layout(RmRectRaw client)
{
	auto layout_func = [](RmRaw<IRmGUIWidget> widget)->flex::TaitankNodeRef {
		auto node = flex::NodeCreate();
		flex::SetWidth(node, widget->getFixedWidth());
		flex::SetHeight(node, widget->getFixedHeight());
		flex::SetMinWidth(node, widget->getMinWidth());
		flex::SetMinHeight(node, widget->getMinHeight());
		flex::SetMaxWidth(node, widget->getMaxWidth());
		flex::SetMaxHeight(node, widget->getMaxHeight());
		flex::SetBorder(node, flex::CSSDirection::CSS_LEFT, widget->getBorder().X);
		flex::SetBorder(node, flex::CSSDirection::CSS_TOP, widget->getBorder().Y);
		flex::SetBorder(node, flex::CSSDirection::CSS_RIGHT, widget->getBorder().Z);
		flex::SetBorder(node, flex::CSSDirection::CSS_BOTTOM, widget->getBorder().W);
		flex::SetMargin(node, flex::CSSDirection::CSS_LEFT, widget->getMargin().X);
		flex::SetMargin(node, flex::CSSDirection::CSS_TOP, widget->getMargin().Y);
		flex::SetMargin(node, flex::CSSDirection::CSS_RIGHT, widget->getMargin().Z);
		flex::SetMargin(node, flex::CSSDirection::CSS_BOTTOM, widget->getMargin().W);
		flex::SetPadding(node, flex::CSSDirection::CSS_LEFT, widget->getPadding().X);
		flex::SetPadding(node, flex::CSSDirection::CSS_TOP, widget->getPadding().Y);
		flex::SetPadding(node, flex::CSSDirection::CSS_RIGHT, widget->getPadding().Z);
		flex::SetPadding(node, flex::CSSDirection::CSS_BOTTOM, widget->getPadding().W);
		return node;
		};

	auto root = layout_func(this);
	flex::SetWidth(root, client->W);
	flex::SetHeight(root, client->H);
	flex::SetAlignItems(root, flex::FlexAlign::FLEX_ALIGN_CENTER);
	flex::SetFlexDirection(root, flex::FlexDirection::FLEX_DIRECTION_ROW);
	flex::SetJustifyContent(root, flex::FlexAlign::FLEX_ALIGN_SPACE_EVENLY);

	auto childList = getChildren();
	for (size_t i = 0; i < childList.size(); ++i)
	{
		auto node = layout_func(childList[i].get());
		if (std::isnan(childList[i]->getFixedWidth()) || std::isnan(childList[i]->getFixedHeight()))
		{
			flex::SetFlexGrow(node, 1.0f);
			flex::SetAlignSelf(node, flex::FlexAlign::FLEX_ALIGN_STRETCH);
		}
		else
		{
			flex::SetAlignSelf(node, flex::FlexAlign::FLEX_ALIGN_CENTER);
		}
		root->AddChild(node);
	}
	flex::DoLayout(root, RmNAN, RmNAN);

	auto left = flex::GetLeft(root); auto top = flex::GetTop(root);
	auto width = flex::GetWidth(root); auto height = flex::GetHeight(root);
	setRect({ client->X + left, client->Y + top, width, height });
	for (size_t i = 0; i < childList.size(); ++i)
	{
		auto node = root->GetChild(i);
		auto left = flex::GetLeft(node); auto top = flex::GetTop(node);
		auto width = flex::GetWidth(node); auto height = flex::GetHeight(node);
		childList[i]->setRect({ client->X + left, client->Y + top, width, height });
	}

	flex::NodeFreeRecursive(root);
}

void RmGUIButton::paint(IRmGUIPainterRaw painter, RmRectRaw client)
{
	RmGUIWidget::paint(painter, client);
	painter->setPen({ .Color = { 108 / 255.0f, 110 / 255.0f, 111 / 255.0f, 1.0f }, });
	painter->setBrush({ .Color = { 238 / 255.0f, 238 / 255.0f, 242 / 255.0f, 1.0f }, });
	painter->drawRect(client->X + 1, client->Y + 1, client->W - 2, client->H - 2);

	RmFloat2 round;
	if (getEnable() == false)
	{
		round = PRIVATE()->Disable.Round;
		painter->setPen(PRIVATE()->Disable.Pen);
		painter->setBrush(PRIVATE()->Disable.Brush);
	}
	else if (PRIVATE()->Pressed)
	{
		round = PRIVATE()->Press.Round;
		painter->setPen(PRIVATE()->Press.Pen);
		painter->setBrush(PRIVATE()->Press.Brush);
	}
	else if (PRIVATE()->Hovered)
	{
		round = PRIVATE()->Hover.Round;
		painter->setPen(PRIVATE()->Hover.Pen);
		painter->setBrush(PRIVATE()->Hover.Brush);
	}
	else
	{
		round = PRIVATE()->Normal.Round;
		painter->setPen(PRIVATE()->Normal.Pen);
		painter->setBrush(PRIVATE()->Normal.Brush);
	}

	painter->drawRoundedRect(client->X, client->Y, client->W, client->H, round.X, round.Y);
}

RmString RmGUIButton::getText() const
{
	return PRIVATE()->LabelWidget->getText();
}

void RmGUIButton::setText(RmString const& text)
{
	PRIVATE()->LabelWidget->setText(text);
}

RmGUILabelTextStyle RmGUIButton::getStyle() const
{
	return PRIVATE()->LabelWidget->getStyle();
}

void RmGUIButton::setStyle(RmGUILabelTextStyle const& style)
{
	PRIVATE()->LabelWidget->setStyle(style);
}

RmFontAligns RmGUIButton::getTextAlignment() const
{
	return PRIVATE()->LabelWidget->getTextAlignment();
}

void RmGUIButton::setTextAlignment(RmFontAligns value)
{
	PRIVATE()->LabelWidget->setTextAlignment(value);
}

void RmGUIButton::mousePressEvent(IRmGUIMouseEventRaw event)
{
	auto client = getRect();
	if (client.X <= event->X && event->X <= client.X + client.W
		&& client.Y <= event->Y && event->Y <= client.Y + client.H)
	{
		if (event->Button == 1)
		{
			PRIVATE()->Pressed = true;
			PRIVATE()->OnClicked.emit();
		}
	}
}

void RmGUIButton::mouseReleaseEvent(IRmGUIMouseEventRaw event)
{
	if (event->Button == 1) PRIVATE()->Pressed = false;
}

void RmGUIButton::mouseMoveEvent(IRmGUIMouseEventRaw event)
{
	auto client = getRect();
	if (client.X <= event->X && event->X <= client.X + client.W
		&& client.Y <= event->Y && event->Y <= client.Y + client.H)
	{
		PRIVATE()->Hovered = true;
	}
	else
	{
		PRIVATE()->Hovered = false;
	}
}
