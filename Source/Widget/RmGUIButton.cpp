#include "RmGUIButton.h"
#include "RmGUILabel.h"
#include <taitank.h>
namespace flex = taitank;

class RmGUIButtonPrivate : public RmGUIWidgetPrivate
{
public:
	RmGUIButtonStyle Style;
	RmGUILabelRef Label;
	RmGUISignalAs<bool> OnClicked;
	RmGUISignalAs<> OnPressed;
	RmGUISignalAs<> OnReleased;
	RmGUISignalAs<> OnHovered;
	RmGUISignalAs<bool> OnToggled;
	bool Pressed = false;
	bool Hovered = false;
	bool Checked = false;
	bool Checkable = false;
};
#define PRIVATE() ((RmGUIButtonPrivate*) m_PrivateButton)

RmGUIButton::RmGUIButton(IRmGUIWidgetRaw parent)
	:
	RmGUIControl(parent),
	m_PrivateButton(nullptr)
{
	m_PrivateButton = new RmGUIButtonPrivate;

	PRIVATE()->Style.Normal.Pen = { .Color = {0 / 255.0f, 120 / 255.0f, 212 / 255.0f, 1.0f} };
	PRIVATE()->Style.Normal.Brush = { .Color = {253 / 255.0f, 253 / 255.0f, 253 / 255.0f, 1.0f} };
	PRIVATE()->Style.Hover.Pen = { .Color = {0 / 255.0f, 120 / 255.0f, 212 / 255.0f, 1.0f} };
	PRIVATE()->Style.Hover.Brush = { .Color = {224 / 255.0f, 238 / 255.0f, 249 / 255.0f, 1.0f} };
	PRIVATE()->Style.Press.Pen = { .Color = {0 / 255.0f, 84 / 255.0f, 153 / 255.0f, 1.0f} };
	PRIVATE()->Style.Press.Brush = { .Color = {204 / 255.0f, 228 / 255.0f, 247 / 255.0f, 1.0f} };
	clicked = &PRIVATE()->OnClicked;
	pressed = &PRIVATE()->OnPressed;
	released = &PRIVATE()->OnReleased;
	hovered = &PRIVATE()->OnHovered;
	toggled = &PRIVATE()->OnToggled;
	PRIVATE()->Label = RmNew<RmGUILabel>();
	PRIVATE()->Label->setAlignment(RmFont::AlignCenter | RmFont::AlignVCenter);
	addWidget(PRIVATE()->Label);
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
	flex::SetAlignItems(root, flex::FlexAlign::FLEX_ALIGN_STRETCH);
	flex::SetFlexDirection(root, flex::FlexDirection::FLEX_DIRECTION_ROW);
	flex::SetJustifyContent(root, flex::FlexAlign::FLEX_ALIGN_SPACE_EVENLY);

	auto childList = getChildren();
	for (size_t i = 0; i < childList.size(); ++i)
	{
		auto node = layout_func(childList[i].get());
		if (std::isnan(childList[i]->getFixedWidth()))
		{
			flex::SetFlexGrow(node, 1.0f);
		}
		if (std::isnan(childList[i]->getFixedHeight()))
		{
			flex::SetAlignSelf(node, flex::FlexAlign::FLEX_ALIGN_STRETCH);
		}
		if (std::isnan(childList[i]->getFixedWidth()) == false && std::isnan(childList[i]->getFixedHeight()) == false)
		{
			flex::SetAlignSelf(node, flex::FlexAlign::FLEX_ALIGN_CENTER);
		}
		root->AddChild(node);
	}
	flex::DoLayout(root, RmNAN, RmNAN);

	for (size_t i = 0; i < childList.size(); ++i)
	{
		auto node = root->GetChild(i);
		auto left = flex::GetLeft(node); auto top = flex::GetTop(node);
		auto width = flex::GetWidth(node); auto height = flex::GetHeight(node);
		childList[i]->setRect({ client->X + left, client->Y + top, width, height });
		auto rect = childList[i]->getRect();
		auto viewport = getViewport();
		childList[i]->setViewport(RmOverlap(viewport, rect));
	}

	flex::NodeFreeRecursive(root);
}

void RmGUIButton::paint(IRmGUIPainterRaw painter, RmRectRaw client)
{
	RmGUIWidget::paint(painter, client);

	if (getEnable() == false)
	{
		painter->setPen(PRIVATE()->Style.Disable.Pen);
		painter->setBrush(PRIVATE()->Style.Disable.Brush);
	}
	else if (PRIVATE()->Checkable && PRIVATE()->Checked || PRIVATE()->Checkable == false && PRIVATE()->Pressed)
	{
		painter->setPen(PRIVATE()->Style.Press.Pen);
		painter->setBrush(PRIVATE()->Style.Press.Brush);
	}
	else if (PRIVATE()->Hovered)
	{
		painter->setPen(PRIVATE()->Style.Hover.Pen);
		painter->setBrush(PRIVATE()->Style.Hover.Brush);
	}
	else
	{
		painter->setPen(PRIVATE()->Style.Normal.Pen);
		painter->setBrush(PRIVATE()->Style.Normal.Brush);
	}

	auto round = PRIVATE()->Style.Round;
	if (round.X < 0.5f || round.Y < 0.5f)
		painter->drawRect(client->X + 1, client->Y + 1, client->W - 2, client->H - 2);
	else
		painter->drawRoundedRect(client->X + 1, client->Y + 1, client->W - 2, client->H - 2, round.X, round.Y);
}

RmString RmGUIButton::getText() const
{
	return PRIVATE()->Label->getText();
}

void RmGUIButton::setText(RmString const& text)
{
	PRIVATE()->Label->setText(text);
}

RmGUIButtonStyle RmGUIButton::getStyle() const
{
	return PRIVATE()->Style;
}

void RmGUIButton::setStyle(RmGUIButtonStyle const& style)
{
	PRIVATE()->Style = style;
}

bool RmGUIButton::getChecked() const
{
	return PRIVATE()->Checked;
}

void RmGUIButton::setChecked(bool value)
{
	PRIVATE()->Checked = value;
}

bool RmGUIButton::getCheckable() const
{
	return PRIVATE()->Checkable;
}

void RmGUIButton::setCheckable(bool value)
{
	if (PRIVATE()->Checkable != value) PRIVATE()->Checked = false;
	PRIVATE()->Checkable = value;
}

bool RmGUIButton::getDown() const
{
	return PRIVATE()->Pressed;
}

RmGUILabelRaw RmGUIButton::getLabel() const
{
	return PRIVATE()->Label.get();
}

void RmGUIButton::mouseDoubleEvent(IRmGUIMouseEventRaw event)
{
	auto viewport = RmOverlap(getViewport(), getRect());
	if (viewport.X <= event->X && event->X <= viewport.X + viewport.W
		&& viewport.Y <= event->Y && event->Y <= viewport.Y + viewport.H)
	{
		if (event->Button == 1)
		{
			PRIVATE()->Pressed = true;
			if (PRIVATE()->Checkable) PRIVATE()->Checked = !PRIVATE()->Checked;
			PRIVATE()->OnPressed.signal();
			PRIVATE()->OnClicked.signal(PRIVATE()->Checked);
		}
	}
}

void RmGUIButton::mousePressEvent(IRmGUIMouseEventRaw event)
{
	auto viewport = RmOverlap(getViewport(), getRect());
	if (viewport.X <= event->X && event->X <= viewport.X + viewport.W
		&& viewport.Y <= event->Y && event->Y <= viewport.Y + viewport.H)
	{
		if (event->Button == 1)
		{
			PRIVATE()->Pressed = true;
			if (PRIVATE()->Checkable) PRIVATE()->Checked = !PRIVATE()->Checked;
			PRIVATE()->OnPressed.signal();
			PRIVATE()->OnClicked.signal(PRIVATE()->Checked);
		}
	}
}

void RmGUIButton::mouseReleaseEvent(IRmGUIMouseEventRaw event)
{
	if (event->Button == 1)
	{
		if (PRIVATE()->Pressed)
		{
			PRIVATE()->Pressed = false;
			PRIVATE()->OnReleased.signal();
		}
	}
}

void RmGUIButton::mouseMoveEvent(IRmGUIMouseEventRaw event)
{
	auto viewport = RmOverlap(getViewport(), getRect());
	if (viewport.X <= event->X && event->X <= viewport.X + viewport.W
		&& viewport.Y <= event->Y && event->Y <= viewport.Y + viewport.H)
	{
		PRIVATE()->Hovered = true;

		PRIVATE()->OnHovered.signal();
	}
	else
	{
		PRIVATE()->Hovered = false;
	}
}

void RmGUIButton::enterEvent(IRmGUIMouseEventRaw event)
{
}

void RmGUIButton::leaveEvent(IRmGUIMouseEventRaw event)
{
	PRIVATE()->Hovered = false;
}