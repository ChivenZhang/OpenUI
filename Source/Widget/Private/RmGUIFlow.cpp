#include "RmGUIFlow.h"
#include <taitank.h>
namespace flex = taitank;

RmGUIFlow::RmGUIFlow(IRmGUIWidgetRaw parent)
	:
	RmGUILayout(parent)
{
}

void RmGUIFlow::layout(RmRectRaw client)
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
	flex::SetFlexWrap(root, flex::FlexWrapMode::FLEX_WRAP);
	flex::SetFlexDirection(root, flex::FlexDirection::FLEX_DIRECTION_ROW);
	auto childList = getChildren();
	for (size_t i = 0; i < childList.size(); ++i)
	{
		auto node = layout_func(childList[i].get());
		if (std::isnan(childList[i]->getFixedWidth()) == false && std::isnan(childList[i]->getFixedHeight()) == false)
		{
			flex::SetAlignSelf(node, flex::FlexAlign::FLEX_ALIGN_START);
		}
		if (std::isnan(childList[i]->getFixedWidth()))
		{
			flex::SetFlexGrow(node, 1.0f);
		}
		if (std::isnan(childList[i]->getFixedHeight()))
		{
			flex::SetAlignSelf(node, flex::FlexAlign::FLEX_ALIGN_STRETCH);
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
		childList[i]->setViewport(RmRect{ std::max(rect.X, viewport.X), std::max(rect.Y, viewport.Y), std::min(rect.X+rect.W, viewport.X+viewport.W), std::min(rect.Y+rect.H, viewport.Y+viewport.H) });
	}

	flex::NodeFreeRecursive(root);
}

void RmGUIFlow::paint(IRmGUIPainterRaw painter, RmRectRaw client)
{
	RmGUIWidget::paint(painter, client);
	painter->setPen({ .Color = { 108 / 255.0f, 110 / 255.0f, 111 / 255.0f, 1.0f }, });
	painter->setBrush({ .Color = { 238 / 255.0f, 238 / 255.0f, 242 / 255.0f, 1.0f }, });
	painter->drawRect(client->X + 1, client->Y + 1, client->W - 2, client->H - 2);
}