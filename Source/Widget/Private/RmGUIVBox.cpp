#include "RmGUIVBox.h"
#include <taitank.h>

RmGUIVBox::RmGUIVBox(IRmGUIWidgetRaw parent)
	:
	RmGUILayout(parent)
{
}

void RmGUIVBox::layout(RmRectRaw client)
{
	auto layout_func = [](RmRaw<IRmGUIWidget> widget)->taitank::TaitankNodeRef {
		auto node = taitank::NodeCreate();
		taitank::SetWidth(node, widget->getFixedWidth());
		taitank::SetHeight(node, widget->getFixedHeight());
		taitank::SetMinWidth(node, widget->getMinWidth());
		taitank::SetMinHeight(node, widget->getMinHeight());
		taitank::SetMaxWidth(node, widget->getMaxWidth());
		taitank::SetMaxHeight(node, widget->getMaxHeight());
		taitank::SetBorder(node, taitank::CSSDirection::CSS_LEFT, widget->getBorder().X);
		taitank::SetBorder(node, taitank::CSSDirection::CSS_TOP, widget->getBorder().Y);
		taitank::SetBorder(node, taitank::CSSDirection::CSS_RIGHT, widget->getBorder().Z);
		taitank::SetBorder(node, taitank::CSSDirection::CSS_BOTTOM, widget->getBorder().W);
		taitank::SetMargin(node, taitank::CSSDirection::CSS_LEFT, widget->getMargin().X);
		taitank::SetMargin(node, taitank::CSSDirection::CSS_TOP, widget->getMargin().Y);
		taitank::SetMargin(node, taitank::CSSDirection::CSS_RIGHT, widget->getMargin().Z);
		taitank::SetMargin(node, taitank::CSSDirection::CSS_BOTTOM, widget->getMargin().W);
		taitank::SetPadding(node, taitank::CSSDirection::CSS_LEFT, widget->getPadding().X);
		taitank::SetPadding(node, taitank::CSSDirection::CSS_TOP, widget->getPadding().Y);
		taitank::SetPadding(node, taitank::CSSDirection::CSS_RIGHT, widget->getPadding().Z);
		taitank::SetPadding(node, taitank::CSSDirection::CSS_BOTTOM, widget->getPadding().W);
		return node;
		};

	auto root = layout_func(this);
	taitank::SetWidth(root, client->W);
	taitank::SetHeight(root, client->H);
	taitank::SetAlignItems(root, taitank::FlexAlign::FLEX_ALIGN_CENTER);
	taitank::SetFlexDirection(root, taitank::FlexDirection::FLEX_DIRECTION_COLUMN);
	taitank::SetJustifyContent(root, taitank::FlexAlign::FLEX_ALIGN_SPACE_EVENLY);
	auto childList = getChildren();
	for (size_t i = 0; i < childList.size(); ++i)
	{
		auto node = layout_func(childList[i].get());
		if (RmCast<RmGUILayout>(childList[i]))
		{
			taitank::SetFlexGrow(node, 1.0f);
			taitank::SetAlignSelf(node, taitank::FlexAlign::FLEX_ALIGN_STRETCH);
		}
		else
		{
			taitank::SetAlignSelf(node, taitank::FlexAlign::FLEX_ALIGN_CENTER);
		}
		root->AddChild(node);
	}
	taitank::DoLayout(root, VALUE_UNDEFINED, VALUE_UNDEFINED);

	auto left = taitank::GetLeft(root); auto top = taitank::GetTop(root);
	auto width = taitank::GetWidth(root); auto height = taitank::GetHeight(root);
	setRect({ client->X + left, client->Y + top, width, height });
	for (size_t i = 0; i < childList.size(); ++i)
	{
		auto node = root->GetChild(i);
		auto left = taitank::GetLeft(node); auto top = taitank::GetTop(node);
		auto width = taitank::GetWidth(node); auto height = taitank::GetHeight(node);
		childList[i]->setRect({ client->X + left, client->Y + top, width, height });
	}

	taitank::NodeFreeRecursive(root);
}

void RmGUIVBox::paint(IRmGUIPainterRaw painter, RmRectRaw client)
{
	painter->setPen({ 194 / 255.0f, 195 / 255.0f, 201 / 255.0f, 1.0f });
	painter->drawRect(client->X + 1, client->Y + 1, client->W - 2, client->H - 2);

	RmGUIWidget::paint(painter, client);
}