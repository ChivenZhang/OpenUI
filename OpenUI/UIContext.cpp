#include "UIContext.h"
#include <yoga/Yoga.h>

struct UIContextElement
{
	UIElementRef Element;
	int32_t ZOrder;
};

class UIContextPrivateData : public UIContextPrivate
{
public:
	UIPainterRef Painter;
	UIRenderRef Render;
	UIElementRaw Focus;
	UIVector<UIContextElement> TopLevelList;
};
#define PRIVATE() ((UIContextPrivateData*) m_Private)

UIContext::UIContext()
{
	m_Private = new UIContextPrivateData;
}

UIContext::~UIContext()
{
	delete m_Private; m_Private = nullptr;
}

UIPainterRaw UIContext::getPainter() const
{
	return PRIVATE()->Painter.get();
}

void UIContext::setPainter(UIPainterRef value)
{
	PRIVATE()->Painter = value;
}

UIRenderRaw UIContext::getRender() const
{
	return PRIVATE()->Render.get();
}

void UIContext::setRender(UIRenderRef value)
{
	PRIVATE()->Render = value;
}

UIElementRaw UIContext::getFocus() const
{
	return PRIVATE()->Focus;
}

void UIContext::setFocus(UIElementRaw value)
{
	PRIVATE()->Focus = value;
}

void UIContext::sendEvent(UIReactorRaw sender, UIEventRaw event)
{
	UILambda<void(UIElementRaw)> foreach_func;
	foreach_func = [&](UIElementRaw element) {
		if (element->getVisible() == false) return;
		if (element->getEventFilter() && element->getEventFilter()->filter(sender, event) == true) return;
		if (element->filter(sender, event) == true) return;
		auto childList = element->getChildren();
		for (size_t i = 0; i < childList.size(); ++i) foreach_func(childList[i].get());
		if (event->Accept == false) element->handle(sender, event);
		};
	for (size_t i = 0; i < PRIVATE()->TopLevelList.size(); ++i)
	{
		auto element = PRIVATE()->TopLevelList[PRIVATE()->TopLevelList.size() - 1 - i].Element.get();
		if (element->getVisible() == false) continue;
		foreach_func(element);
		break;
	}
}

void UIContext::postEvent(UIReactorRef sender, UIEventRef event)
{
}

bool UIContext::addElement(UIElementRef value, int32_t zorder)
{
	if (value == nullptr) return false;
	auto result = std::find_if(PRIVATE()->TopLevelList.begin(), PRIVATE()->TopLevelList.end(), [=](UIContextElement const& e)->bool { return e.Element == value; });
	if (result == PRIVATE()->TopLevelList.end()) PRIVATE()->TopLevelList.emplace_back(value, zorder);
	else result->ZOrder = zorder;
	value->setContext(this);
	value->setParent(nullptr);
	std::sort(PRIVATE()->TopLevelList.begin(), PRIVATE()->TopLevelList.end(), [](UIContextElement const& a, UIContextElement const& b) { return a.ZOrder < b.ZOrder; });
	return true;
}

bool UIContext::removeElement(UIElementRef value)
{
	auto result = std::remove_if(PRIVATE()->TopLevelList.begin(), PRIVATE()->TopLevelList.end(), [=](UIContextElement const& e)->bool { return e.Element == value; });
	if (result == PRIVATE()->TopLevelList.end()) return false;
	PRIVATE()->TopLevelList.erase(result, PRIVATE()->TopLevelList.end());
	value->setContext(nullptr);
	value->setParent(nullptr);
	return true;
}

void UIContext::removeElement()
{
	for (size_t i = 0; i < PRIVATE()->TopLevelList.size(); ++i)
	{
		PRIVATE()->TopLevelList[i].Element->setContext(nullptr);
		PRIVATE()->TopLevelList[i].Element->setParent(nullptr);
	}
	PRIVATE()->TopLevelList.clear();
}

void UIContext::layoutElement(UIRect client)
{
	UILambda<YGNodeRef(UIElementRaw, UIRect)> foreach_func;
	foreach_func = [&](UIElementRaw element, UIRect client)->YGNodeRef {

		auto node = YGNodeNew();

		switch (element->getFixedPosX().Unit)
		{
		case UI::UnitNone: YGNodeStyleSetPosition(node, YGEdge::YGEdgeLeft, UINAN); break;
		case UI::UnitPoint: YGNodeStyleSetPosition(node, YGEdge::YGEdgeLeft, element->getFixedPosX()); break;
		case UI::UnitPercent: YGNodeStyleSetPositionPercent(node, YGEdge::YGEdgeLeft, element->getFixedPosX()); break;
		}
		switch (element->getFixedPosY().Unit)
		{
		case UI::UnitNone: YGNodeStyleSetPosition(node, YGEdge::YGEdgeRight, UINAN); break;
		case UI::UnitPoint: YGNodeStyleSetPosition(node, YGEdge::YGEdgeRight, element->getFixedPosY()); break;
		case UI::UnitPercent: YGNodeStyleSetPositionPercent(node, YGEdge::YGEdgeRight, element->getFixedPosY()); break;
		}
		switch (element->getFixedWidth().Unit)
		{
		case UI::UnitNone: YGNodeStyleSetWidth(node, UINAN); break;
		case UI::UnitPoint: YGNodeStyleSetWidth(node, element->getFixedWidth()); break;
		case UI::UnitPercent: YGNodeStyleSetWidthPercent(node, element->getFixedWidth()); break;
		case UI::UnitAuto: YGNodeStyleSetWidthAuto(node); break;
		}
		switch (element->getFixedHeight().Unit)
		{
		case UI::UnitNone: YGNodeStyleSetHeight(node, UINAN); break;
		case UI::UnitPoint: YGNodeStyleSetHeight(node, element->getFixedHeight()); break;
		case UI::UnitPercent: YGNodeStyleSetHeightPercent(node, element->getFixedHeight()); break;
		case UI::UnitAuto: YGNodeStyleSetHeightAuto(node); break;
		}
		switch (element->getMinWidth().Unit)
		{
		case UI::UnitNone: YGNodeStyleSetMinWidth(node, UINAN); break;
		case UI::UnitPoint: YGNodeStyleSetMinWidth(node, element->getMinWidth()); break;
		case UI::UnitPercent: YGNodeStyleSetMinWidthPercent(node, element->getMinWidth()); break;
		}
		switch (element->getMinHeight().Unit)
		{
		case UI::UnitNone: YGNodeStyleSetMinHeight(node, UINAN); break;
		case UI::UnitPoint: YGNodeStyleSetMinHeight(node, element->getMinHeight()); break;
		case UI::UnitPercent: YGNodeStyleSetMinHeightPercent(node, element->getMinHeight()); break;
		}
		switch (element->getMaxWidth().Unit)
		{
		case UI::UnitNone: YGNodeStyleSetMaxWidth(node, UINAN); break;
		case UI::UnitPoint: YGNodeStyleSetMaxWidth(node, element->getMaxWidth()); break;
		case UI::UnitPercent: YGNodeStyleSetMaxWidthPercent(node, element->getMaxWidth()); break;
		}
		switch (element->getMaxHeight().Unit)
		{
		case UI::UnitNone: YGNodeStyleSetMaxHeight(node, UINAN); break;
		case UI::UnitPoint: YGNodeStyleSetMaxHeight(node, element->getMaxHeight()); break;
		case UI::UnitPercent: YGNodeStyleSetMaxHeightPercent(node, element->getMaxHeight()); break;
		}
		YGNodeStyleSetBorder(node, YGEdgeLeft, element->getBorder()[0]);
		YGNodeStyleSetBorder(node, YGEdgeTop, element->getBorder()[1]);
		YGNodeStyleSetBorder(node, YGEdgeRight, element->getBorder()[2]);
		YGNodeStyleSetBorder(node, YGEdgeBottom, element->getBorder()[3]);
		switch (element->getMargin()[0].Unit)
		{
		case UI::UnitNone: YGNodeStyleSetMargin(node, YGEdgeLeft, UINAN); break;
		case UI::UnitPoint: YGNodeStyleSetMargin(node, YGEdgeLeft, element->getMargin()[0]); break;
		case UI::UnitPercent: YGNodeStyleSetMarginPercent(node, YGEdgeLeft, element->getMargin()[0]); break;
		case UI::UnitAuto: YGNodeStyleSetMarginAuto(node, YGEdgeLeft); break;
		}
		switch (element->getMargin()[1].Unit)
		{
		case UI::UnitNone: YGNodeStyleSetMargin(node, YGEdgeTop, UINAN); break;
		case UI::UnitPoint: YGNodeStyleSetMargin(node, YGEdgeTop, element->getMargin()[1]); break;
		case UI::UnitPercent: YGNodeStyleSetMarginPercent(node, YGEdgeTop, element->getMargin()[1]); break;
		case UI::UnitAuto: YGNodeStyleSetMarginAuto(node, YGEdgeTop); break;
		}
		switch (element->getMargin()[2].Unit)
		{
		case UI::UnitNone: YGNodeStyleSetMargin(node, YGEdgeRight, UINAN); break;
		case UI::UnitPoint: YGNodeStyleSetMargin(node, YGEdgeRight, element->getMargin()[2]); break;
		case UI::UnitPercent: YGNodeStyleSetMarginPercent(node, YGEdgeRight, element->getMargin()[2]); break;
		case UI::UnitAuto: YGNodeStyleSetMarginAuto(node, YGEdgeRight); break;
		}
		switch (element->getMargin()[3].Unit)
		{
		case UI::UnitNone: YGNodeStyleSetMargin(node, YGEdgeBottom, UINAN); break;
		case UI::UnitPoint: YGNodeStyleSetMargin(node, YGEdgeBottom, element->getMargin()[3]); break;
		case UI::UnitPercent: YGNodeStyleSetMarginPercent(node, YGEdgeBottom, element->getMargin()[3]); break;
		case UI::UnitAuto: YGNodeStyleSetMarginAuto(node, YGEdgeBottom); break;
		}
		switch (element->getPadding()[0].Unit)
		{
		case UI::UnitNone: YGNodeStyleSetPadding(node, YGEdgeLeft, UINAN); break;
		case UI::UnitPoint: YGNodeStyleSetPadding(node, YGEdgeLeft, element->getPadding()[0]); break;
		case UI::UnitPercent: YGNodeStyleSetPaddingPercent(node, YGEdgeLeft, element->getPadding()[0]); break;
		}
		switch (element->getPadding()[1].Unit)
		{
		case UI::UnitNone: YGNodeStyleSetPadding(node, YGEdgeTop, UINAN); break;
		case UI::UnitPoint: YGNodeStyleSetPadding(node, YGEdgeTop, element->getPadding()[1]); break;
		case UI::UnitPercent: YGNodeStyleSetPaddingPercent(node, YGEdgeTop, element->getPadding()[1]); break;
		}
		switch (element->getPadding()[2].Unit)
		{
		case UI::UnitNone: YGNodeStyleSetPadding(node, YGEdgeRight, UINAN); break;
		case UI::UnitPoint: YGNodeStyleSetPadding(node, YGEdgeRight, element->getPadding()[2]); break;
		case UI::UnitPercent: YGNodeStyleSetPaddingPercent(node, YGEdgeRight, element->getPadding()[2]); break;
		}
		switch (element->getPadding()[3].Unit)
		{
		case UI::UnitNone: YGNodeStyleSetPadding(node, YGEdgeBottom, UINAN); break;
		case UI::UnitPoint: YGNodeStyleSetPadding(node, YGEdgeBottom, element->getPadding()[3]); break;
		case UI::UnitPercent: YGNodeStyleSetPaddingPercent(node, YGEdgeBottom, element->getPadding()[3]); break;
		}
		switch (element->getFlexDirection())
		{
		case UI::FlexDirectionColumn: YGNodeStyleSetFlexDirection(node, YGFlexDirectionColumn); break;
		case UI::FlexDirectionColumnReverse: YGNodeStyleSetFlexDirection(node, YGFlexDirectionColumnReverse); break;
		case UI::FlexDirectionRow: YGNodeStyleSetFlexDirection(node, YGFlexDirectionRow); break;
		case UI::FlexDirectionRowReverse: YGNodeStyleSetFlexDirection(node, YGFlexDirectionRowReverse); break;
		}
		switch (element->getFlexWrap())
		{
		case UI::FlexNoWrap: YGNodeStyleSetFlexWrap(node, YGWrapNoWrap); break;
		case UI::FlexDoWrap: YGNodeStyleSetFlexWrap(node, YGWrapWrap); break;
		case UI::FlexWrapReverse: YGNodeStyleSetFlexWrap(node, YGWrapWrapReverse); break;
		}
		switch (element->getAlignContent())
		{
		case UI::AlignAuto: YGNodeStyleSetAlignContent(node, YGAlignAuto); break;
		case UI::AlignFlexStart: YGNodeStyleSetAlignContent(node, YGAlignFlexStart); break;
		case UI::AlignCenter: YGNodeStyleSetAlignContent(node, YGAlignCenter); break;
		case UI::AlignFlexEnd: YGNodeStyleSetAlignContent(node, YGAlignFlexEnd); break;
		case UI::AlignStretch: YGNodeStyleSetAlignContent(node, YGAlignStretch); break;
		case UI::AlignBaseline: YGNodeStyleSetAlignContent(node, YGAlignBaseline); break;
		case UI::AlignSpaceBetween: YGNodeStyleSetAlignContent(node, YGAlignSpaceEvenly); break;
		case UI::AlignSpaceAround: YGNodeStyleSetAlignContent(node, YGAlignSpaceAround); break;
		case UI::AlignSpaceEvenly: YGNodeStyleSetAlignContent(node, YGAlignSpaceEvenly); break;
		}
		switch (element->getAlignItems())
		{
		case UI::AlignAuto: YGNodeStyleSetAlignItems(node, YGAlignAuto); break;
		case UI::AlignFlexStart: YGNodeStyleSetAlignItems(node, YGAlignFlexStart); break;
		case UI::AlignCenter: YGNodeStyleSetAlignItems(node, YGAlignCenter); break;
		case UI::AlignFlexEnd: YGNodeStyleSetAlignItems(node, YGAlignFlexEnd); break;
		case UI::AlignStretch: YGNodeStyleSetAlignItems(node, YGAlignStretch); break;
		case UI::AlignBaseline: YGNodeStyleSetAlignItems(node, YGAlignBaseline); break;
		case UI::AlignSpaceBetween: YGNodeStyleSetAlignItems(node, YGAlignSpaceEvenly); break;
		case UI::AlignSpaceAround: YGNodeStyleSetAlignItems(node, YGAlignSpaceAround); break;
		case UI::AlignSpaceEvenly: YGNodeStyleSetAlignItems(node, YGAlignSpaceEvenly); break;
		}
		switch (element->getJustifyContent())
		{
		case UI::JustifyFlexStart: YGNodeStyleSetJustifyContent(node, YGJustifyFlexStart); break;
		case UI::JustifyCenter: YGNodeStyleSetJustifyContent(node, YGJustifyCenter); break;
		case UI::JustifyFlexEnd: YGNodeStyleSetJustifyContent(node, YGJustifyFlexEnd); break;
		case UI::JustifySpaceBetween: YGNodeStyleSetJustifyContent(node, YGJustifySpaceBetween); break;
		case UI::JustifySpaceAround: YGNodeStyleSetJustifyContent(node, YGJustifySpaceAround); break;
		case UI::JustifySpaceEvenly: YGNodeStyleSetJustifyContent(node, YGJustifySpaceEvenly); break;
		}
		switch (element->getFlexBasis().Unit)
		{
		case UI::UnitNone: YGNodeStyleSetFlexBasis(node, UINAN); break;
		case UI::UnitPoint: YGNodeStyleSetFlexBasis(node, element->getFlexBasis()); break;
		case UI::UnitPercent: YGNodeStyleSetFlexBasisPercent(node, element->getFlexBasis()); break;
		case UI::UnitAuto: YGNodeStyleSetFlexBasisAuto(node); break;
		}
		switch (element->getAlignSelf())
		{
		case UI::AlignAuto: YGNodeStyleSetAlignSelf(node, YGAlignAuto); break;
		case UI::AlignFlexStart: YGNodeStyleSetAlignSelf(node, YGAlignFlexStart); break;
		case UI::AlignCenter: YGNodeStyleSetAlignSelf(node, YGAlignCenter); break;
		case UI::AlignFlexEnd: YGNodeStyleSetAlignSelf(node, YGAlignFlexEnd); break;
		case UI::AlignStretch: YGNodeStyleSetAlignSelf(node, YGAlignStretch); break;
		case UI::AlignBaseline: YGNodeStyleSetAlignSelf(node, YGAlignBaseline); break;
		case UI::AlignSpaceBetween: YGNodeStyleSetAlignSelf(node, YGAlignSpaceEvenly); break;
		case UI::AlignSpaceAround: YGNodeStyleSetAlignSelf(node, YGAlignSpaceAround); break;
		case UI::AlignSpaceEvenly: YGNodeStyleSetAlignSelf(node, YGAlignSpaceEvenly); break;
		}
		YGNodeStyleSetFlexGrow(node, element->getFlexGrow());
		YGNodeStyleSetFlexShrink(node, element->getFlexShrink());

		for (size_t i = 0; i < element->getChildren().size(); ++i)
		{
			auto child = foreach_func(element->getChildren()[i].get(), client);
			YGNodeInsertChild(node, child, YGNodeGetChildCount(node));
		}
		return node;
		};

	UILambda<void(YGNodeRef, UIElementRaw, UIRect)> layout_func;
	layout_func = [&](YGNodeRef node, UIElementRaw element, UIRect client) {

		element->setBounds({ YGNodeLayoutGetLeft(node), YGNodeLayoutGetTop(node), YGNodeLayoutGetWidth(node), YGNodeLayoutGetHeight(node) });
		element->setViewport(UIOverlap(element->getBounds(), client));

		for (size_t i = 0; i < YGNodeGetChildCount(node) && i < element->getChildren().size(); ++i)
		{
			layout_func(YGNodeGetChild(node, i), element->getChildren()[i].get(), element->getChildren()[i]->getBounds());
		}
		};

	for (size_t i = 0; i < PRIVATE()->TopLevelList.size(); ++i)
	{
		PRIVATE()->TopLevelList[i].Element->setFixedPos(client.X, client.Y);
		PRIVATE()->TopLevelList[i].Element->setFixedSize(client.W, client.H);
		PRIVATE()->TopLevelList[i].Element->setBounds(client);
		PRIVATE()->TopLevelList[i].Element->setViewport(client);
		auto root = foreach_func(PRIVATE()->TopLevelList[i].Element.get(), PRIVATE()->TopLevelList[i].Element->getBounds());
		YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);
		layout_func(root, PRIVATE()->TopLevelList[i].Element.get(), PRIVATE()->TopLevelList[i].Element->getBounds());
		YGNodeFreeRecursive(root);
	}
}

void UIContext::paintElement(UIRect client)
{
	UILambda<void(UIElementRaw, UIRect, UIPainterRaw)> foreach_func;
	foreach_func = [&](UIElementRaw element, UIRect client, UIPainterRaw painter) {
		if (element->getVisible() == false) return;
		if (element->getPainter()) painter = element->getPainter();
		if (painter == nullptr) return;
		element->paint(client, painter);
		auto childList = element->getChildren();
		for (size_t i = 0; i < childList.size(); ++i) foreach_func(childList[i].get(), childList[i]->getBounds(), painter);
		};
	for (size_t i = 0; i < PRIVATE()->TopLevelList.size(); ++i)
	{
		foreach_func(PRIVATE()->TopLevelList[i].Element.get(), PRIVATE()->TopLevelList[i].Element->getBounds(), getPainter());
	}
}

void UIContext::renderElement(UIRect client)
{
	if (getRender() == nullptr) return;

	UIVector<UIPrimitive> renderList;
	UILambda<void(UIElementRaw, UIRect client)> foreach_func;
	foreach_func = [&](UIElementRaw element, UIRect client) {
		if (element->getVisible() == false) return;
		if (element->getPainter()) renderList.emplace_back(element->getPainter(), element->getPrimitive());
		auto childList = element->getChildren();
		for (size_t i = 0; i < childList.size(); ++i) foreach_func(childList[i].get(), childList[i]->getBounds());
		};

	if (getPainter())
	{
		auto viewport = client;
		UIPointUV3 primitive[2];
		primitive[0].P0 = { viewport.X, viewport.Y };
		primitive[0].P1 = { viewport.X + viewport.W, viewport.Y };
		primitive[0].P2 = { viewport.X + viewport.W, viewport.Y + viewport.H };
		primitive[1].P0 = { viewport.X, viewport.Y };
		primitive[1].P1 = { viewport.X + viewport.W, viewport.Y + viewport.H };
		primitive[1].P2 = { viewport.X, viewport.Y + viewport.H };
		renderList.emplace_back(getPainter(), primitive);
	}

	for (size_t i = 0; i < PRIVATE()->TopLevelList.size(); ++i)
	{
		foreach_func(PRIVATE()->TopLevelList[i].Element.get(), PRIVATE()->TopLevelList[i].Element->getBounds());
	}
	getRender()->render(client, renderList);
}