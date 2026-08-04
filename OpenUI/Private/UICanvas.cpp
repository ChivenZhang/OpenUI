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
#include "../UICanvas.h"
#include <yoga/Yoga.h>

struct UITopLevelWidget
{
	UIWidgetRef Widget;
	int32_t ZOrder;
};

class UICanvasPrivateData : public UICanvasPrivate
{
public:
	UIConfig Config;
	UIDeviceRaw Device;
	// UIRenderRef Render;
	UIPainterRef Painter;
	UIBuilderRef Builder;
	UIWidgetRaw Focus;
	bool NeedLayout = true, NeedPaint = true;
	UIStringMap<UIRenderRef> RenderMap;
	UIList<UIPrimitive> RenderList;
	UIList<UIWidgetRaw> AnimateList;
	UIList<UIWidgetRef> TopLevelView;
	UIList<UITopLevelWidget> TopLevelList;
};
#define PRIVATE() ((UICanvasPrivateData*) m_Private)

UICanvas::UICanvas(UIDeviceRaw device, UIConfig config)
{
	m_Private = new UICanvasPrivateData;

	PRIVATE()->Device = device;
	PRIVATE()->Config = config;
	PRIVATE()->Builder = UINew<UIBuilder>(this);
}

UICanvas::~UICanvas()
{
	delete m_Private;
	m_Private = nullptr;
}

UIConfig const& UICanvas::getConfig() const
{
	return PRIVATE()->Config;
}

UIDeviceRaw UICanvas::getDevice() const
{
	return PRIVATE()->Device;
}

UIBuilderRaw UICanvas::getBuilder() const
{
	return PRIVATE()->Builder.get();
}

UIImage UICanvas::getTarget() const
{
	return PRIVATE()->Config.RenderTarget;
}

void UICanvas::setTarget(UIImage value)
{
	PRIVATE()->Config.RenderTarget = value;
}

UIPainterRaw UICanvas::getPainter() const
{
	return PRIVATE()->Painter.get();
}

void UICanvas::setPainter(UIPainterRef value)
{
	PRIVATE()->Painter = value;
}

UIRenderRaw UICanvas::getRender(UIString name) const
{
	auto result = PRIVATE()->RenderMap.find(name);
	if (result == PRIVATE()->RenderMap.end()) return nullptr;
	return result->second.get();
}

void UICanvas::setRender(UIRenderRef value)
{
	if (value == nullptr) return;
	PRIVATE()->RenderMap[value->getName()] = value;
}

UIWidgetRaw UICanvas::getFocus() const
{
	return PRIVATE()->Focus;
}

void UICanvas::setFocus(UIWidgetRaw value)
{
	PRIVATE()->Focus = value;
}

void UICanvas::setAnimate(UIWidgetRaw value, bool animate)
{
	if (animate)
	{
		auto result = std::find(PRIVATE()->AnimateList.begin(), PRIVATE()->AnimateList.end(), value);
		if (result == PRIVATE()->AnimateList.end())
			PRIVATE()->AnimateList.push_back(value);
	}
	else
	{
		auto result = std::remove(PRIVATE()->AnimateList.begin(), PRIVATE()->AnimateList.end(), value);
		PRIVATE()->AnimateList.erase(result, PRIVATE()->AnimateList.end());
	}
}

void UICanvas::sendEvent(UIReactorRaw sender, UIEventRaw event)
{
	UILambda<void(UIWidgetRaw)> foreach_func;
	foreach_func = [&](UIWidgetRaw widget)
	{
		if (widget->getVisible() == false) return;
		if (widget->getEventFilter())
		{
			if (widget->getEventFilter()->filter(widget, event)) return;
		}
		else
		{
			if (widget->filter(widget, event)) return;
		}
		auto childList = widget->getWidgets();
		for (size_t i = 0; i < childList.size(); ++i) foreach_func(childList[i].get());
		if (event->Accept == false) widget->handle(sender, event);
	};
	for (size_t i = 0; i < PRIVATE()->TopLevelList.size(); ++i)
	{
		auto widget = PRIVATE()->TopLevelList[PRIVATE()->TopLevelList.size() - 1 - i].Widget.get();
		foreach_func(widget);
		break;
	}
}

void UICanvas::postEvent(UIReactorRef sender, UIEventRef event)
{
}

bool UICanvas::addWidget(UIWidgetRef value, int32_t zorder)
{
	if (value == nullptr) return false;
	auto result = std::find_if(PRIVATE()->TopLevelList.begin(), PRIVATE()->TopLevelList.end(), [=](UITopLevelWidget const& e)-> bool { return e.Widget == value; });
	if (result == PRIVATE()->TopLevelList.end())
		PRIVATE()->TopLevelList.push_back({value, zorder});
	else result->ZOrder = zorder;
	value->setContext(this);
	value->setParent(nullptr);
	std::sort(PRIVATE()->TopLevelList.begin(), PRIVATE()->TopLevelList.end(), [](UITopLevelWidget const& a, UITopLevelWidget const& b) { return a.ZOrder < b.ZOrder; });
	PRIVATE()->TopLevelView.resize(PRIVATE()->TopLevelList.size());
	for (size_t i = 0; i < PRIVATE()->TopLevelList.size(); ++i)
		PRIVATE()->TopLevelView[i] = PRIVATE()->TopLevelList[i].Widget;
	layoutWidget();
	return true;
}

bool UICanvas::removeWidget(UIWidgetRef value)
{
	auto result = std::remove_if(PRIVATE()->TopLevelList.begin(), PRIVATE()->TopLevelList.end(), [=](UITopLevelWidget const& e)-> bool { return e.Widget == value; });
	if (result == PRIVATE()->TopLevelList.end()) return false;
	PRIVATE()->TopLevelList.erase(result, PRIVATE()->TopLevelList.end());
	PRIVATE()->TopLevelView.resize(PRIVATE()->TopLevelList.size());
	for (size_t i = 0; i < PRIVATE()->TopLevelList.size(); ++i)
		PRIVATE()->TopLevelView[i] = PRIVATE()->TopLevelList[i].Widget;
	value->setContext(nullptr);
	value->setParent(nullptr);
	layoutWidget();
	return true;
}

void UICanvas::removeWidget()
{
	for (size_t i = 0; i < PRIVATE()->TopLevelList.size(); ++i)
	{
		PRIVATE()->TopLevelList[i].Widget->setContext(nullptr);
		PRIVATE()->TopLevelList[i].Widget->setParent(nullptr);
	}
	PRIVATE()->TopLevelList.clear();
	PRIVATE()->TopLevelView.clear();
	layoutWidget();
}

bool UICanvas::existWidget(UIWidgetRef value) const
{
	auto result = std::find(PRIVATE()->TopLevelView.begin(), PRIVATE()->TopLevelView.end(), value);
	return result != PRIVATE()->TopLevelView.end();
}

UIListView<const UIWidgetRef> UICanvas::getWidget() const
{
	return PRIVATE()->TopLevelView;
}

void UICanvas::layoutWidget()
{
	PRIVATE()->NeedLayout = true;
	paintWidget();
}

bool UICanvas::layoutWidget(UIRect client)
{
	if (PRIVATE()->NeedLayout == false) return false;
	PRIVATE()->NeedLayout = false;

	UILambda<void(UIWidgetRaw, UIRect)> arrange_func;
	arrange_func = [&](UIWidgetRaw element, UIRect client)
	{
		element->arrange(element->getBounds());
		for (size_t i = 0; i < element->getWidgets().size(); ++i)
		{
			arrange_func(element->getWidgets()[i].get(), element->getBounds());
		}
	};

	UILambda<YGNodeRef(UIWidgetRaw, UIRect)> foreach_func;
	foreach_func = [&](UIWidgetRaw element, UIRect client)-> YGNodeRef
	{
		auto node = YGNodeNew();

		switch (element->getDisplayType())
		{
		case UI::DisplayFlex: YGNodeStyleSetDisplay(node, YGDisplayFlex);
			break;
		case UI::DisplayNone: YGNodeStyleSetDisplay(node, YGDisplayNone);
			break;
		}
		switch (element->getPositionType())
		{
		case UI::PositionStatic: YGNodeStyleSetPositionType(node, YGPositionTypeStatic);
			break;
		case UI::PositionRelative: YGNodeStyleSetPositionType(node, YGPositionTypeRelative);
			break;
		case UI::PositionAbsolute: YGNodeStyleSetPositionType(node, YGPositionTypeAbsolute);
			break;
		}
		switch (element->getFixedPosX().Unit)
		{
		case UI::UnitNone: YGNodeStyleSetPosition(node, YGEdge::YGEdgeLeft, UINAN);
			break;
		case UI::UnitPoint: YGNodeStyleSetPosition(node, YGEdge::YGEdgeLeft, element->getFixedPosX());
			break;
		case UI::UnitPercent: YGNodeStyleSetPositionPercent(node, YGEdge::YGEdgeLeft, element->getFixedPosX());
			break;
		}
		switch (element->getFixedPosY().Unit)
		{
		case UI::UnitNone: YGNodeStyleSetPosition(node, YGEdge::YGEdgeTop, UINAN);
			break;
		case UI::UnitPoint: YGNodeStyleSetPosition(node, YGEdge::YGEdgeTop, element->getFixedPosY());
			break;
		case UI::UnitPercent: YGNodeStyleSetPositionPercent(node, YGEdge::YGEdgeTop, element->getFixedPosY());
			break;
		}
		switch (element->getFixedWidth().Unit)
		{
		case UI::UnitNone: YGNodeStyleSetWidth(node, UINAN);
			break;
		case UI::UnitPoint: YGNodeStyleSetWidth(node, element->getFixedWidth());
			break;
		case UI::UnitPercent: YGNodeStyleSetWidthPercent(node, element->getFixedWidth());
			break;
		case UI::UnitAuto: YGNodeStyleSetWidthAuto(node);
			break;
		}
		switch (element->getFixedHeight().Unit)
		{
		case UI::UnitNone: YGNodeStyleSetHeight(node, UINAN);
			break;
		case UI::UnitPoint: YGNodeStyleSetHeight(node, element->getFixedHeight());
			break;
		case UI::UnitPercent: YGNodeStyleSetHeightPercent(node, element->getFixedHeight());
			break;
		case UI::UnitAuto: YGNodeStyleSetHeightAuto(node);
			break;
		}
		switch (element->getMinWidth().Unit)
		{
		case UI::UnitNone: YGNodeStyleSetMinWidth(node, UINAN);
			break;
		case UI::UnitPoint: YGNodeStyleSetMinWidth(node, element->getMinWidth());
			break;
		case UI::UnitPercent: YGNodeStyleSetMinWidthPercent(node, element->getMinWidth());
			break;
		}
		switch (element->getMinHeight().Unit)
		{
		case UI::UnitNone: YGNodeStyleSetMinHeight(node, UINAN);
			break;
		case UI::UnitPoint: YGNodeStyleSetMinHeight(node, element->getMinHeight());
			break;
		case UI::UnitPercent: YGNodeStyleSetMinHeightPercent(node, element->getMinHeight());
			break;
		}
		switch (element->getMaxWidth().Unit)
		{
		case UI::UnitNone: YGNodeStyleSetMaxWidth(node, UINAN);
			break;
		case UI::UnitPoint: YGNodeStyleSetMaxWidth(node, element->getMaxWidth());
			break;
		case UI::UnitPercent: YGNodeStyleSetMaxWidthPercent(node, element->getMaxWidth());
			break;
		}
		switch (element->getMaxHeight().Unit)
		{
		case UI::UnitNone: YGNodeStyleSetMaxHeight(node, UINAN);
			break;
		case UI::UnitPoint: YGNodeStyleSetMaxHeight(node, element->getMaxHeight());
			break;
		case UI::UnitPercent: YGNodeStyleSetMaxHeightPercent(node, element->getMaxHeight());
			break;
		}
		YGNodeStyleSetBorder(node, YGEdgeLeft, element->getBorder()[0]);
		YGNodeStyleSetBorder(node, YGEdgeTop, element->getBorder()[1]);
		YGNodeStyleSetBorder(node, YGEdgeRight, element->getBorder()[2]);
		YGNodeStyleSetBorder(node, YGEdgeBottom, element->getBorder()[3]);
		switch (element->getMargin()[0].Unit)
		{
		case UI::UnitNone: YGNodeStyleSetMargin(node, YGEdgeLeft, UINAN);
			break;
		case UI::UnitPoint: YGNodeStyleSetMargin(node, YGEdgeLeft, element->getMargin()[0]);
			break;
		case UI::UnitPercent: YGNodeStyleSetMarginPercent(node, YGEdgeLeft, element->getMargin()[0]);
			break;
		case UI::UnitAuto: YGNodeStyleSetMarginAuto(node, YGEdgeLeft);
			break;
		}
		switch (element->getMargin()[1].Unit)
		{
		case UI::UnitNone: YGNodeStyleSetMargin(node, YGEdgeTop, UINAN);
			break;
		case UI::UnitPoint: YGNodeStyleSetMargin(node, YGEdgeTop, element->getMargin()[1]);
			break;
		case UI::UnitPercent: YGNodeStyleSetMarginPercent(node, YGEdgeTop, element->getMargin()[1]);
			break;
		case UI::UnitAuto: YGNodeStyleSetMarginAuto(node, YGEdgeTop);
			break;
		}
		switch (element->getMargin()[2].Unit)
		{
		case UI::UnitNone: YGNodeStyleSetMargin(node, YGEdgeRight, UINAN);
			break;
		case UI::UnitPoint: YGNodeStyleSetMargin(node, YGEdgeRight, element->getMargin()[2]);
			break;
		case UI::UnitPercent: YGNodeStyleSetMarginPercent(node, YGEdgeRight, element->getMargin()[2]);
			break;
		case UI::UnitAuto: YGNodeStyleSetMarginAuto(node, YGEdgeRight);
			break;
		}
		switch (element->getMargin()[3].Unit)
		{
		case UI::UnitNone: YGNodeStyleSetMargin(node, YGEdgeBottom, UINAN);
			break;
		case UI::UnitPoint: YGNodeStyleSetMargin(node, YGEdgeBottom, element->getMargin()[3]);
			break;
		case UI::UnitPercent: YGNodeStyleSetMarginPercent(node, YGEdgeBottom, element->getMargin()[3]);
			break;
		case UI::UnitAuto: YGNodeStyleSetMarginAuto(node, YGEdgeBottom);
			break;
		}
		switch (element->getPadding()[0].Unit)
		{
		case UI::UnitNone: YGNodeStyleSetPadding(node, YGEdgeLeft, UINAN);
			break;
		case UI::UnitPoint: YGNodeStyleSetPadding(node, YGEdgeLeft, element->getPadding()[0]);
			break;
		case UI::UnitPercent: YGNodeStyleSetPaddingPercent(node, YGEdgeLeft, element->getPadding()[0]);
			break;
		}
		switch (element->getPadding()[1].Unit)
		{
		case UI::UnitNone: YGNodeStyleSetPadding(node, YGEdgeTop, UINAN);
			break;
		case UI::UnitPoint: YGNodeStyleSetPadding(node, YGEdgeTop, element->getPadding()[1]);
			break;
		case UI::UnitPercent: YGNodeStyleSetPaddingPercent(node, YGEdgeTop, element->getPadding()[1]);
			break;
		}
		switch (element->getPadding()[2].Unit)
		{
		case UI::UnitNone: YGNodeStyleSetPadding(node, YGEdgeRight, UINAN);
			break;
		case UI::UnitPoint: YGNodeStyleSetPadding(node, YGEdgeRight, element->getPadding()[2]);
			break;
		case UI::UnitPercent: YGNodeStyleSetPaddingPercent(node, YGEdgeRight, element->getPadding()[2]);
			break;
		}
		switch (element->getPadding()[3].Unit)
		{
		case UI::UnitNone: YGNodeStyleSetPadding(node, YGEdgeBottom, UINAN);
			break;
		case UI::UnitPoint: YGNodeStyleSetPadding(node, YGEdgeBottom, element->getPadding()[3]);
			break;
		case UI::UnitPercent: YGNodeStyleSetPaddingPercent(node, YGEdgeBottom, element->getPadding()[3]);
			break;
		}
		switch (element->getSpacing()[0].Unit)
		{
		case UI::UnitNone: YGNodeStyleSetGap(node, YGGutterRow, 0);
			break;
		case UI::UnitPoint: YGNodeStyleSetGap(node, YGGutterRow, element->getSpacing()[0]);
			break;
		case UI::UnitPercent: YGNodeStyleSetGapPercent(node, YGGutterRow, element->getSpacing()[0]);
			break;
		}
		switch (element->getSpacing()[1].Unit)
		{
		case UI::UnitNone: YGNodeStyleSetGap(node, YGGutterColumn, 0);
			break;
		case UI::UnitPoint: YGNodeStyleSetGap(node, YGGutterColumn, element->getSpacing()[1]);
			break;
		case UI::UnitPercent: YGNodeStyleSetGapPercent(node, YGGutterColumn, element->getSpacing()[1]);
			break;
		}
		switch (element->getFlexDirection())
		{
		case UI::FlexDirectionColumn: YGNodeStyleSetFlexDirection(node, YGFlexDirectionColumn);
			break;
		case UI::FlexDirectionColumnReverse: YGNodeStyleSetFlexDirection(node, YGFlexDirectionColumnReverse);
			break;
		case UI::FlexDirectionRow: YGNodeStyleSetFlexDirection(node, YGFlexDirectionRow);
			break;
		case UI::FlexDirectionRowReverse: YGNodeStyleSetFlexDirection(node, YGFlexDirectionRowReverse);
			break;
		}
		switch (element->getFlexWrap())
		{
		case UI::FlexNoWrap: YGNodeStyleSetFlexWrap(node, YGWrapNoWrap);
			break;
		case UI::FlexDoWrap: YGNodeStyleSetFlexWrap(node, YGWrapWrap);
			break;
		case UI::FlexWrapReverse: YGNodeStyleSetFlexWrap(node, YGWrapWrapReverse);
			break;
		}
		switch (element->getAlignContent())
		{
		case UI::AlignAuto: YGNodeStyleSetAlignContent(node, YGAlignAuto);
			break;
		case UI::AlignFlexStart: YGNodeStyleSetAlignContent(node, YGAlignFlexStart);
			break;
		case UI::AlignCenter: YGNodeStyleSetAlignContent(node, YGAlignCenter);
			break;
		case UI::AlignFlexEnd: YGNodeStyleSetAlignContent(node, YGAlignFlexEnd);
			break;
		case UI::AlignStretch: YGNodeStyleSetAlignContent(node, YGAlignStretch);
			break;
		case UI::AlignBaseline: YGNodeStyleSetAlignContent(node, YGAlignBaseline);
			break;
		case UI::AlignSpaceBetween: YGNodeStyleSetAlignContent(node, YGAlignSpaceEvenly);
			break;
		case UI::AlignSpaceAround: YGNodeStyleSetAlignContent(node, YGAlignSpaceAround);
			break;
		case UI::AlignSpaceEvenly: YGNodeStyleSetAlignContent(node, YGAlignSpaceEvenly);
			break;
		}
		switch (element->getAlignItems())
		{
		case UI::AlignAuto: YGNodeStyleSetAlignItems(node, YGAlignAuto);
			break;
		case UI::AlignFlexStart: YGNodeStyleSetAlignItems(node, YGAlignFlexStart);
			break;
		case UI::AlignCenter: YGNodeStyleSetAlignItems(node, YGAlignCenter);
			break;
		case UI::AlignFlexEnd: YGNodeStyleSetAlignItems(node, YGAlignFlexEnd);
			break;
		case UI::AlignStretch: YGNodeStyleSetAlignItems(node, YGAlignStretch);
			break;
		case UI::AlignBaseline: YGNodeStyleSetAlignItems(node, YGAlignBaseline);
			break;
		case UI::AlignSpaceBetween: YGNodeStyleSetAlignItems(node, YGAlignSpaceEvenly);
			break;
		case UI::AlignSpaceAround: YGNodeStyleSetAlignItems(node, YGAlignSpaceAround);
			break;
		case UI::AlignSpaceEvenly: YGNodeStyleSetAlignItems(node, YGAlignSpaceEvenly);
			break;
		}
		switch (element->getJustifyContent())
		{
		case UI::JustifyFlexStart: YGNodeStyleSetJustifyContent(node, YGJustifyFlexStart);
			break;
		case UI::JustifyCenter: YGNodeStyleSetJustifyContent(node, YGJustifyCenter);
			break;
		case UI::JustifyFlexEnd: YGNodeStyleSetJustifyContent(node, YGJustifyFlexEnd);
			break;
		case UI::JustifySpaceBetween: YGNodeStyleSetJustifyContent(node, YGJustifySpaceBetween);
			break;
		case UI::JustifySpaceAround: YGNodeStyleSetJustifyContent(node, YGJustifySpaceAround);
			break;
		case UI::JustifySpaceEvenly: YGNodeStyleSetJustifyContent(node, YGJustifySpaceEvenly);
			break;
		}
		switch (element->getFlexBasis().Unit)
		{
		case UI::UnitNone: YGNodeStyleSetFlexBasis(node, UINAN);
			break;
		case UI::UnitPoint: YGNodeStyleSetFlexBasis(node, element->getFlexBasis());
			break;
		case UI::UnitPercent: YGNodeStyleSetFlexBasisPercent(node, element->getFlexBasis());
			break;
		case UI::UnitAuto: YGNodeStyleSetFlexBasisAuto(node);
			break;
		}
		switch (element->getAlignSelf())
		{
		case UI::AlignAuto: YGNodeStyleSetAlignSelf(node, YGAlignAuto);
			break;
		case UI::AlignFlexStart: YGNodeStyleSetAlignSelf(node, YGAlignFlexStart);
			break;
		case UI::AlignCenter: YGNodeStyleSetAlignSelf(node, YGAlignCenter);
			break;
		case UI::AlignFlexEnd: YGNodeStyleSetAlignSelf(node, YGAlignFlexEnd);
			break;
		case UI::AlignStretch: YGNodeStyleSetAlignSelf(node, YGAlignStretch);
			break;
		case UI::AlignBaseline: YGNodeStyleSetAlignSelf(node, YGAlignBaseline);
			break;
		case UI::AlignSpaceBetween: YGNodeStyleSetAlignSelf(node, YGAlignSpaceEvenly);
			break;
		case UI::AlignSpaceAround: YGNodeStyleSetAlignSelf(node, YGAlignSpaceAround);
			break;
		case UI::AlignSpaceEvenly: YGNodeStyleSetAlignSelf(node, YGAlignSpaceEvenly);
			break;
		}
		YGNodeStyleSetFlexGrow(node, element->getFlexGrow());
		YGNodeStyleSetFlexShrink(node, element->getFlexShrink());

		for (size_t i = 0; i < element->getWidgets().size(); ++i)
		{
			auto child = foreach_func(element->getWidgets()[i].get(), client);
			YGNodeInsertChild(node, child, YGNodeGetChildCount(node));
		}
		return node;
	};

	UILambda<void(YGNodeRef, UIWidgetRaw, UIRect)> layout_func;
	layout_func = [&](YGNodeRef node, UIWidgetRaw element, UIRect client)
	{
		element->setLocalBounds({YGNodeLayoutGetLeft(node), YGNodeLayoutGetTop(node), YGNodeLayoutGetWidth(node), YGNodeLayoutGetHeight(node)});
		element->setBounds({client.X + element->getLocalBounds().X, client.Y + element->getLocalBounds().Y, element->getLocalBounds().W, element->getLocalBounds().H});
		element->setViewport({UINAN, UINAN, UINAN, UINAN});

		for (size_t i = 0; i < YGNodeGetChildCount(node) && i < element->getWidgets().size(); ++i)
		{
			layout_func(YGNodeGetChild(node, i), element->getWidgets()[i].get(), element->getBounds());
		}
	};

	UILambda<void(UIWidgetRaw, UIRect, UIRect)> relayout_func;
	relayout_func = [&](UIWidgetRaw element, UIRect client, UIRect viewport)
	{
		if (std::isnan(element->getViewport().X) ||
			std::isnan(element->getViewport().Y) ||
			std::isnan(element->getViewport().W) ||
			std::isnan(element->getViewport().H))
			element->setViewport(UIOverlap(viewport, element->getBounds()));

		element->layout(element->getBounds());
		element->setBounds({client.X + element->getLocalBounds().X, client.Y + element->getLocalBounds().Y, element->getLocalBounds().W, element->getLocalBounds().H});
		element->setViewport(UIOverlap(element->getViewport(), element->getBounds()));
		for (size_t i = 0; i < element->getWidgets().size(); ++i)
		{
			auto bounds = element->getBounds();
			auto childBounds = element->getWidgets()[i]->getLocalBounds();
			element->getWidgets()[i]->setBounds({bounds.X + childBounds.X, bounds.Y + childBounds.Y, childBounds.W, childBounds.H});
		}
		for (size_t i = 0; i < element->getWidgets().size(); ++i)
		{
			relayout_func(element->getWidgets()[i].get(), element->getBounds(), element->getViewport());
		}
	};

	for (auto& widget : PRIVATE()->TopLevelList)
	{
		arrange_func(widget.Widget.get(), client);

		auto root = foreach_func(widget.Widget.get(), client);
		YGNodeCalculateLayout(root, client.W, client.H, YGDirectionLTR);
		layout_func(root, widget.Widget.get(), client);
		YGNodeFreeRecursive(root);

		relayout_func(widget.Widget.get(), client, client);
	}
	return true;
}

void UICanvas::paintWidget()
{
	PRIVATE()->NeedPaint = true;
}

bool UICanvas::paintWidget(UIRect client)
{
	if (PRIVATE()->NeedPaint == false) return false;
	PRIVATE()->NeedPaint = false;

	UILambda<void(UIWidgetRaw, UIRect, UIPainterRaw)> foreach_func;
	foreach_func = [&](UIWidgetRaw widget, UIRect client, UIPainterRaw painter)
	{
		if (widget->getVisible() == false || painter == nullptr) return;
		widget->paint(client, painter);
		auto childList = widget->getWidgets();
		for (size_t i = 0; i < childList.size(); ++i) foreach_func(childList[i].get(), childList[i]->getBounds(), painter);
		widget->repaint(client, painter);
	};

	for (auto& widget : PRIVATE()->TopLevelList)
	{
		foreach_func(widget.Widget.get(), widget.Widget->getBounds(), getPainter());
	}
	return true;
}

void UICanvas::renderWidget(UIRect client)
{
	if (getPainter() == nullptr) return;

	for (auto& geometry : getPainter()->getGeometry())
	{
		for (auto& primitive : geometry.Primitives)
		{
			if (primitive.Style == nullptr) continue;
			auto renderName = primitive.Style->getStyle<UIString>("render");
			if (auto render = getRender(renderName))
			{
				render->render(geometry.Client, {&primitive, 1});
			}
		}
	}

	getPainter()->getGeometry().clear();
}

void UICanvas::animateWidget(float time)
{
	for (size_t i = 0; i < PRIVATE()->AnimateList.size(); ++i)
	{
		if (PRIVATE()->AnimateList[i] == nullptr) continue;
		UITimerEvent event(time);
		PRIVATE()->AnimateList[i]->timerEvent(&event);
	}
}

void UICanvas::updateWidget(float time, UIRect client)
{
	layoutWidget(client);
	animateWidget(time);
	paintWidget(client);
}