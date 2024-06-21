#include "RmGUIHBox.h"
#include <taitank.h>

RmGUIHBox::RmGUIHBox(IRmGUIWidgetRaw parent)
	:
	RmGUILayout(parent)
{
}

void RmGUIHBox::layout(rmrect client)
{
	auto layout_func = [=](RmRaw<IRmGUIWidget> widget)->taitank::TaitankNodeRef {
		auto flex = taitank::NodeCreate();
		taitank::SetWidth(flex, widget->getFixedWidth());
		taitank::SetHeight(flex, widget->getFixedHeight());
		taitank::SetMinWidth(flex, widget->getMinWidth());
		taitank::SetMinHeight(flex, widget->getMinHeight());
		taitank::SetMaxWidth(flex, widget->getMaxWidth());
		taitank::SetMaxHeight(flex, widget->getMaxHeight());
		taitank::SetBorder(flex, taitank::CSSDirection::CSS_LEFT, widget->getBorder().X);
		taitank::SetBorder(flex, taitank::CSSDirection::CSS_TOP, widget->getBorder().Y);
		taitank::SetBorder(flex, taitank::CSSDirection::CSS_RIGHT, widget->getBorder().Z);
		taitank::SetBorder(flex, taitank::CSSDirection::CSS_BOTTOM, widget->getBorder().W);
		taitank::SetMargin(flex, taitank::CSSDirection::CSS_LEFT, widget->getMargin().X);
		taitank::SetMargin(flex, taitank::CSSDirection::CSS_TOP, widget->getMargin().Y);
		taitank::SetMargin(flex, taitank::CSSDirection::CSS_RIGHT, widget->getMargin().Z);
		taitank::SetMargin(flex, taitank::CSSDirection::CSS_BOTTOM, widget->getMargin().W);
		taitank::SetPadding(flex, taitank::CSSDirection::CSS_LEFT, widget->getPadding().X);
		taitank::SetPadding(flex, taitank::CSSDirection::CSS_TOP, widget->getPadding().Y);
		taitank::SetPadding(flex, taitank::CSSDirection::CSS_RIGHT, widget->getPadding().Z);
		taitank::SetPadding(flex, taitank::CSSDirection::CSS_BOTTOM, widget->getPadding().W);
		taitank::SetAlignItems(flex, taitank::FlexAlign::FLEX_ALIGN_CENTER);
		taitank::SetFlexDirection(flex, taitank::FlexDirection::FLEX_DIRECTION_ROW);
		taitank::SetJustifyContent(flex, taitank::FlexAlign::FLEX_ALIGN_SPACE_EVENLY);
		return flex;
		};

	auto root = layout_func(this);
	taitank::SetWidth(root, client->W);
	taitank::SetHeight(root, client->H);
	taitank::SetPosition(root, taitank::CSSDirection::CSS_LEFT, client->X);
	taitank::SetPosition(root, taitank::CSSDirection::CSS_TOP, client->Y);
	auto childList = getChildren();
	for (size_t i = 0; i < childList.size(); ++i) root->AddChild(layout_func(childList[i].get()));
	taitank::DoLayout(root, VALUE_UNDEFINED, VALUE_UNDEFINED);

	for (size_t i = 0; i < childList.size(); ++i)
	{
		auto flex = root->GetChild(i);
		auto left = taitank::GetLeft(flex); auto top = taitank::GetTop(flex);
		auto width = taitank::GetWidth(flex); auto height = taitank::GetHeight(flex);
		childList[i]->setWidth(width);
		childList[i]->setHeight(height);
		childList[i]->setRect({ left, top, width, height });
	}
	auto left = taitank::GetLeft(root);
	auto top = taitank::GetTop(root);
	auto width = taitank::GetWidth(root);
	auto height = taitank::GetHeight(root);
	setWidth(width);
	setHeight(height);
	setRect({ left, top, width, height });
	taitank::NodeFreeRecursive(root);
}