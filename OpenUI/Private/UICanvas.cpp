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
#include "../UIButton.h"
#include "../UICombo.h"
#include "../UIInput.h"
#include "../UILine.h"
#include <yoga/Yoga.h>

#include "OpenUI/UICheck.h"
#include "OpenUI/UIRadio.h"

struct UITopLevelWidget
{
	UIWidgetRef Widget;
	int32_t ZOrder;
};

struct UICanvasPrivate : UIPrivate
{
	UIConfig Config;
	UIDeviceRaw Device;
	UIRenderRef Render;
	UIPainterRef Painter;
	UIBuilderRef Builder;
	UIWidgetRaw Focus;
	bool NeedLayout = true, NeedPaint = true;
	UIStringMap<UIRenderRef> EffectorMap;
	UIList<UIPrimitive> RenderList;
	UIList<UIWidgetRaw> AnimateList;
	UIList<UIWidgetRef> TopLevelView;
	UIList<UITopLevelWidget> TopLevelList;
};
#define PRIVATE() ((UICanvasPrivate*) m_Private)

UICanvas::UICanvas(UIDeviceRaw device, UIConfig config)
{
	m_Private = new UICanvasPrivate;
	PRIVATE()->Device = device;
	PRIVATE()->Config = config;
	PRIVATE()->Builder = UINew<UIBuilder>(this);

	// TODO: Embedded Factory from https://developer.mozilla.org/zh-CN/docs/Web/HTML/Reference/Elements

	PRIVATE()->Builder->setFactory("body", UINew<UIWidgetFactory<UIWidget>>());
	PRIVATE()->Builder->setFactory("div", UINew<UIWidgetFactory<UIWidget>>());
	PRIVATE()->Builder->setFactory("hr", UINew<UIWidgetFactory<UIHLine>>());
	PRIVATE()->Builder->setFactory("h1", UINew<UIWidgetFactory<UILabel>>());
	PRIVATE()->Builder->setFactory("h2", UINew<UIWidgetFactory<UILabel>>());
	PRIVATE()->Builder->setFactory("h3", UINew<UIWidgetFactory<UILabel>>());
	PRIVATE()->Builder->setFactory("h4", UINew<UIWidgetFactory<UILabel>>());
	PRIVATE()->Builder->setFactory("h5", UINew<UIWidgetFactory<UILabel>>());
	PRIVATE()->Builder->setFactory("h6", UINew<UIWidgetFactory<UILabel>>());
	PRIVATE()->Builder->setFactory("p", UINew<UIWidgetFactory<UILabel>>());
	PRIVATE()->Builder->setFactory("label", UINew<UIWidgetFactory<UILabel>>());
	PRIVATE()->Builder->setFactory("button", UINew<UIWidgetFactory<UIButton>>());
	PRIVATE()->Builder->setFactory("button:hover", UINew<UIWidgetFactory<UIButton>>());
	PRIVATE()->Builder->setFactory("text", UINew<UIWidgetFactory<UIInput>>());
	PRIVATE()->Builder->setFactory("select", UINew<UIWidgetFactory<UICombo>>());
	PRIVATE()->Builder->setFactory("radio", UINew<UIWidgetFactory<UIRadio>>());
	PRIVATE()->Builder->setFactory("checkbox", UINew<UIWidgetFactory<UICheck>>());
}

UICanvas::~UICanvas()
{
	delete m_Private; m_Private = nullptr;
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

UIImageRaw UICanvas::getTarget() const
{
	return &PRIVATE()->Config.RenderTarget;
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
	auto result = PRIVATE()->EffectorMap.find(name);
	if (result == PRIVATE()->EffectorMap.end()) return nullptr;
	return result->second.get();
}

void UICanvas::setRender(UIRenderRef value)
{
	if (value == nullptr) return;
	PRIVATE()->EffectorMap[value->getName()] = value;
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
			PRIVATE()->AnimateList.emplace_back(value);
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

	UILambda<void(UIWidgetRaw, bool)> style_func;
	style_func = [&style_func](UIWidgetRaw widget, bool dirty)
	{
		dirty |= widget->getStyles()->getDirty();
		if (dirty == false) return;
		widget->getStyles()->setDirty(false);

		if (widget->getParent() == nullptr) widget->getStyleComputed()->compute(nullptr);
		else widget->getStyleComputed()->compute(widget->getParent()->getStyleComputed());

		for (size_t i = 0; i < widget->getWidgets().size(); ++i)
		{
			style_func(widget->getWidgets()[i].get(), dirty);
		}
	};

	UILambda<void(UIWidgetRaw, UIRect)> arrange_func;
	arrange_func = [&](UIWidgetRaw widget, UIRect client)
	{
		widget->arrange(widget->getBounds());
		for (size_t i = 0; i < widget->getWidgets().size(); ++i)
		{
			arrange_func(widget->getWidgets()[i].get(), widget->getBounds());
		}
	};

	UILambda<YGNodeRef(UIWidgetRaw, UIRect)> foreach_func;
	foreach_func = [&](UIWidgetRaw widget, UIRect client)-> YGNodeRef
	{
		auto node = YGNodeNew();

		switch (widget->getDisplayType().Value)
		{
		default:
		case UI_CSS_DISPLAY_NONE: YGNodeStyleSetDisplay(node, YGDisplayNone);
			break;
		case UI_CSS_DISPLAY_FLEX: YGNodeStyleSetDisplay(node, YGDisplayFlex);
			break;
		}
		switch (widget->getPositionType().Value)
		{
		default:
		case UI_CSS_POSITION_STATIC: YGNodeStyleSetPositionType(node, YGPositionTypeStatic);
			break;
		case UI_CSS_POSITION_RELATIVE: YGNodeStyleSetPositionType(node, YGPositionTypeRelative);
			break;
		case UI_CSS_POSITION_ABSOLUTE: YGNodeStyleSetPositionType(node, YGPositionTypeAbsolute);
			break;
		}
		switch (widget->getFixedPosX().Type)
		{
		default:
		case UI_CSS_LEFT_AUTO: YGNodeStyleSetPosition(node, YGEdge::YGEdgeLeft, UINAN);
			break;
		case UI_CSS_LEFT_LENGTH: YGNodeStyleSetPosition(node, YGEdge::YGEdgeLeft, widget->getFixedPosX().Value);
			break;
		case UI_CSS_LEFT_PERCENTAGE: YGNodeStyleSetPositionPercent(node, YGEdge::YGEdgeLeft, widget->getFixedPosX().Value);
			break;
		}
		switch (widget->getFixedPosY().Type)
		{
		default:
		case UI_CSS_TOP_AUTO: YGNodeStyleSetPosition(node, YGEdge::YGEdgeTop, UINAN);
			break;
		case UI_CSS_TOP_LENGTH: YGNodeStyleSetPosition(node, YGEdge::YGEdgeTop, widget->getFixedPosY().Value);
			break;
		case UI_CSS_TOP_PERCENTAGE: YGNodeStyleSetPositionPercent(node, YGEdge::YGEdgeTop, widget->getFixedPosY().Value);
			break;
		}
		switch (widget->getFixedWidth().Type)
		{
		default: YGNodeStyleSetWidth(node, UINAN);
			break;
		case UI_CSS_WIDTH_AUTO: YGNodeStyleSetWidthAuto(node);
			break;
		case UI_CSS_WIDTH_LENGTH: YGNodeStyleSetWidth(node, widget->getFixedWidth().Value);
			break;
		case UI_CSS_WIDTH_PERCENTAGE: YGNodeStyleSetWidthPercent(node, widget->getFixedWidth().Value);
			break;
		}
		switch (widget->getFixedHeight().Type)
		{
		default: YGNodeStyleSetHeight(node, UINAN);
			break;
		case UI_CSS_HEIGHT_AUTO: YGNodeStyleSetHeightAuto(node);
			break;
		case UI_CSS_HEIGHT_LENGTH: YGNodeStyleSetHeight(node, widget->getFixedHeight().Value);
			break;
		case UI_CSS_HEIGHT_PERCENTAGE: YGNodeStyleSetHeightPercent(node, widget->getFixedHeight().Value);
			break;
		}
		switch (widget->getMinWidth().Type)
		{
		default: YGNodeStyleSetMinWidth(node, UINAN);
			break;
		case UI_CSS_MIN_WIDTH_LENGTH: YGNodeStyleSetMinWidth(node, widget->getMinWidth().Value);
			break;
		case UI_CSS_MIN_WIDTH_PERCENTAGE: YGNodeStyleSetMinWidthPercent(node, widget->getMinWidth().Value);
			break;
		}
		switch (widget->getMinHeight().Type)
		{
		default: YGNodeStyleSetMinHeight(node, UINAN);
			break;
		case UI_CSS_MIN_HEIGHT_LENGTH: YGNodeStyleSetMinHeight(node, widget->getMinHeight().Value);
			break;
		case UI_CSS_MIN_HEIGHT_PERCENTAGE: YGNodeStyleSetMinHeightPercent(node, widget->getMinHeight().Value);
			break;
		}
		switch (widget->getMaxWidth().Type)
		{
		default: YGNodeStyleSetMaxWidth(node, UINAN);
			break;
		case UI_CSS_MAX_WIDTH_LENGTH: YGNodeStyleSetMaxWidth(node, widget->getMaxWidth().Value);
			break;
		case UI_CSS_MAX_WIDTH_PERCENTAGE: YGNodeStyleSetMaxWidthPercent(node, widget->getMaxWidth().Value);
			break;
		}
		switch (widget->getMaxHeight().Type)
		{
		default: YGNodeStyleSetMaxHeight(node, UINAN);
			break;
		case UI_CSS_MAX_HEIGHT_LENGTH: YGNodeStyleSetMaxHeight(node, widget->getMaxHeight().Value);
			break;
		case UI_CSS_MAX_HEIGHT_PERCENTAGE: YGNodeStyleSetMaxHeightPercent(node, widget->getMaxHeight().Value);
			break;
		}
		YGNodeStyleSetBorder(node, YGEdgeLeft, widget->getBorderLeft().Value);
		YGNodeStyleSetBorder(node, YGEdgeTop, widget->getBorderTop().Value);
		YGNodeStyleSetBorder(node, YGEdgeRight, widget->getBorderRight().Value);
		YGNodeStyleSetBorder(node, YGEdgeBottom, widget->getBorderBottom().Value);
		switch (widget->getMarginLeft().Type)
		{
		default: YGNodeStyleSetMargin(node, YGEdgeLeft, UINAN);
			break;
		case UI_CSS_MARGIN_AUTO: YGNodeStyleSetMarginAuto(node, YGEdgeLeft);
			break;
		case UI_CSS_MARGIN_LENGTH: YGNodeStyleSetMargin(node, YGEdgeLeft, widget->getMarginLeft().Value);
			break;
		case UI_CSS_MARGIN_PERCENTAGE: YGNodeStyleSetMarginPercent(node, YGEdgeLeft, widget->getMarginLeft().Value);
			break;
		}
		switch (widget->getMarginTop().Type)
		{
		default: YGNodeStyleSetMargin(node, YGEdgeTop, UINAN);
			break;
		case UI_CSS_MARGIN_AUTO: YGNodeStyleSetMarginAuto(node, YGEdgeTop);
			break;
		case UI_CSS_MARGIN_LENGTH: YGNodeStyleSetMargin(node, YGEdgeTop, widget->getMarginTop().Value);
			break;
		case UI_CSS_MARGIN_PERCENTAGE: YGNodeStyleSetMarginPercent(node, YGEdgeTop, widget->getMarginTop().Value);
			break;
		}
		switch (widget->getMarginRight().Type)
		{
		default: YGNodeStyleSetMargin(node, YGEdgeRight, UINAN);
			break;
		case UI_CSS_MARGIN_AUTO: YGNodeStyleSetMarginAuto(node, YGEdgeRight);
			break;
		case UI_CSS_MARGIN_LENGTH: YGNodeStyleSetMargin(node, YGEdgeRight, widget->getMarginRight().Value);
			break;
		case UI_CSS_MARGIN_PERCENTAGE: YGNodeStyleSetMarginPercent(node, YGEdgeRight, widget->getMarginRight().Value);
			break;
		}
		switch (widget->getMarginBottom().Type)
		{
		default: YGNodeStyleSetMargin(node, YGEdgeBottom, UINAN);
			break;
		case UI_CSS_MARGIN_AUTO: YGNodeStyleSetMarginAuto(node, YGEdgeBottom);
			break;
		case UI_CSS_MARGIN_LENGTH: YGNodeStyleSetMargin(node, YGEdgeBottom, widget->getMarginBottom().Value);
			break;
		case UI_CSS_MARGIN_PERCENTAGE: YGNodeStyleSetMarginPercent(node, YGEdgeBottom, widget->getMarginBottom().Value);
			break;
		}
		switch (widget->getPaddingLeft().Type)
		{
		default: YGNodeStyleSetPadding(node, YGEdgeLeft, UINAN);
			break;
		case UI_CSS_PADDING_LENGTH: YGNodeStyleSetPadding(node, YGEdgeLeft, widget->getPaddingLeft().Value);
			break;
		case UI_CSS_PADDING_PERCENTAGE: YGNodeStyleSetPaddingPercent(node, YGEdgeLeft, widget->getPaddingLeft().Value);
			break;
		}
		switch (widget->getPaddingTop().Type)
		{
		default: YGNodeStyleSetPadding(node, YGEdgeTop, UINAN);
			break;
		case UI_CSS_PADDING_LENGTH: YGNodeStyleSetPadding(node, YGEdgeTop, widget->getPaddingTop().Value);
			break;
		case UI_CSS_PADDING_PERCENTAGE: YGNodeStyleSetPaddingPercent(node, YGEdgeTop, widget->getPaddingTop().Value);
			break;
		}
		switch (widget->getPaddingRight().Type)
		{
		default: YGNodeStyleSetPadding(node, YGEdgeRight, UINAN);
			break;
		case UI_CSS_PADDING_LENGTH: YGNodeStyleSetPadding(node, YGEdgeRight, widget->getPaddingRight().Value);
			break;
		case UI_CSS_PADDING_PERCENTAGE: YGNodeStyleSetPaddingPercent(node, YGEdgeRight, widget->getPaddingRight().Value);
			break;
		}
		switch (widget->getPaddingBottom().Type)
		{
		default: YGNodeStyleSetPadding(node, YGEdgeBottom, UINAN);
			break;
		case UI_CSS_PADDING_LENGTH: YGNodeStyleSetPadding(node, YGEdgeBottom, widget->getPaddingBottom().Value);
			break;
		case UI_CSS_PADDING_PERCENTAGE: YGNodeStyleSetPaddingPercent(node, YGEdgeBottom, widget->getPaddingBottom().Value);
			break;
		}
		// switch (widget->getSpacing().Type)
		// {
		// case UI_CSS_UnitNone: YGNodeStyleSetGap(node, YGGutterRow, 0);
		// 	break;
		// case UI_CSS_UnitPoint: YGNodeStyleSetGap(node, YGGutterRow, widget->getSpacing().Value);
		// 	break;
		// case UI_CSS_UnitPercent: YGNodeStyleSetGapPercent(node, YGGutterRow, widget->getSpacing().Value);
		// 	break;
		// }
		// switch (widget->getSpacing().Type)
		// {
		// case UI_CSS_UnitNone: YGNodeStyleSetGap(node, YGGutterColumn, 0);
		// 	break;
		// case UI_CSS_UnitPoint: YGNodeStyleSetGap(node, YGGutterColumn, widget->getSpacing().Value);
		// 	break;
		// case UI_CSS_UnitPercent: YGNodeStyleSetGapPercent(node, YGGutterColumn, widget->getSpacing().Value);
		// 	break;
		// }
		switch (widget->getFlexDirection().Value)
		{
		default:
		case UI_CSS_FLEX_DIRECTION_COLUMN: YGNodeStyleSetFlexDirection(node, YGFlexDirectionColumn);
			break;
		case UI_CSS_FLEX_DIRECTION_COLUMN_REVERSE: YGNodeStyleSetFlexDirection(node, YGFlexDirectionColumnReverse);
			break;
		case UI_CSS_FLEX_DIRECTION_ROW: YGNodeStyleSetFlexDirection(node, YGFlexDirectionRow);
			break;
		case UI_CSS_FLEX_DIRECTION_ROW_REVERSE: YGNodeStyleSetFlexDirection(node, YGFlexDirectionRowReverse);
			break;
		}
		switch (widget->getFlexWrap().Value)
		{
		default:
		case UI_CSS_FLEX_WRAP_NOWRAP: YGNodeStyleSetFlexWrap(node, YGWrapNoWrap);
			break;
		case UI_CSS_FLEX_WRAP_WRAP: YGNodeStyleSetFlexWrap(node, YGWrapWrap);
			break;
		case UI_CSS_FLEX_WRAP_WRAP_REVERSE: YGNodeStyleSetFlexWrap(node, YGWrapWrapReverse);
			break;
		}
		switch (widget->getAlignContent().Value)
		{
		default: YGNodeStyleSetAlignContent(node, YGAlignAuto);
			break;
		case UI_CSS_ALIGN_CONTENT_FLEX_START: YGNodeStyleSetAlignContent(node, YGAlignFlexStart);
			break;
		case UI_CSS_ALIGN_CONTENT_FLEX_END: YGNodeStyleSetAlignContent(node, YGAlignFlexEnd);
			break;
		case UI_CSS_ALIGN_CONTENT_CENTER: YGNodeStyleSetAlignContent(node, YGAlignCenter);
			break;
		case UI_CSS_ALIGN_CONTENT_STRETCH: YGNodeStyleSetAlignContent(node, YGAlignStretch);
			break;
		case UI_CSS_ALIGN_CONTENT_SPACE_BETWEEN: YGNodeStyleSetAlignContent(node, YGAlignSpaceBetween);
			break;
		case UI_CSS_ALIGN_CONTENT_SPACE_AROUND: YGNodeStyleSetAlignContent(node, YGAlignSpaceAround);
			break;
		case UI_CSS_ALIGN_CONTENT_BASE_LINE: YGNodeStyleSetAlignContent(node, YGAlignBaseline);
			break;
		case UI_CSS_ALIGN_CONTENT_SPACE_EVENLY: YGNodeStyleSetAlignContent(node, YGAlignSpaceEvenly);
			break;
		}
		switch (widget->getAlignItems().Value)
		{
		default: YGNodeStyleSetAlignItems(node, YGAlignAuto);
			break;
		case UI_CSS_ALIGN_ITEMS_FLEX_START: YGNodeStyleSetAlignItems(node, YGAlignFlexStart);
			break;
		case UI_CSS_ALIGN_ITEMS_CENTER: YGNodeStyleSetAlignItems(node, YGAlignCenter);
			break;
		case UI_CSS_ALIGN_ITEMS_FLEX_END: YGNodeStyleSetAlignItems(node, YGAlignFlexEnd);
			break;
		case UI_CSS_ALIGN_ITEMS_STRETCH: YGNodeStyleSetAlignItems(node, YGAlignStretch);
			break;
		case UI_CSS_ALIGN_ITEMS_BASELINE: YGNodeStyleSetAlignItems(node, YGAlignBaseline);
			break;
		case UI_CSS_ALIGN_ITEMS_SPACE_EVENLY: YGNodeStyleSetAlignItems(node, YGAlignSpaceEvenly);
			break;
		case UI_CSS_ALIGN_ITEMS_SPACE_AROUND: YGNodeStyleSetAlignItems(node, YGAlignSpaceAround);
			break;
		}
		switch (widget->getJustifyContent().Value)
		{
		default:
		case UI_CSS_JUSTIFY_CONTENT_FLEX_START: YGNodeStyleSetJustifyContent(node, YGJustifyFlexStart);
			break;
		case UI_CSS_JUSTIFY_CONTENT_CENTER: YGNodeStyleSetJustifyContent(node, YGJustifyCenter);
			break;
		case UI_CSS_JUSTIFY_CONTENT_FLEX_END: YGNodeStyleSetJustifyContent(node, YGJustifyFlexEnd);
			break;
		case UI_CSS_JUSTIFY_CONTENT_SPACE_BETWEEN: YGNodeStyleSetJustifyContent(node, YGJustifySpaceBetween);
			break;
		case UI_CSS_JUSTIFY_CONTENT_SPACE_AROUND: YGNodeStyleSetJustifyContent(node, YGJustifySpaceAround);
			break;
		case UI_CSS_JUSTIFY_CONTENT_SPACE_EVENLY: YGNodeStyleSetJustifyContent(node, YGJustifySpaceEvenly);
			break;
		}
		switch (widget->getFlexBasis().Type)
		{
		default: YGNodeStyleSetFlexBasis(node, UINAN);
			break;
		case UI_CSS_FLEX_BASIS_LENGTH: YGNodeStyleSetFlexBasis(node, widget->getFlexBasis().Value);
			break;
		case UI_CSS_FLEX_BASIS_PERCENTAGE: YGNodeStyleSetFlexBasisPercent(node, widget->getFlexBasis().Value);
			break;
		case UI_CSS_FLEX_BASIS_CONTENT: YGNodeStyleSetFlexBasisAuto(node);
			break;
		}
		switch (widget->getAlignSelf().Value)
		{
		default:
		case UI_CSS_ALIGN_SELF_AUTO: YGNodeStyleSetAlignSelf(node, YGAlignAuto);
			break;
		case UI_CSS_ALIGN_SELF_FLEX_START: YGNodeStyleSetAlignSelf(node, YGAlignFlexStart);
			break;
		case UI_CSS_ALIGN_SELF_CENTER: YGNodeStyleSetAlignSelf(node, YGAlignCenter);
			break;
		case UI_CSS_ALIGN_SELF_FLEX_END: YGNodeStyleSetAlignSelf(node, YGAlignFlexEnd);
			break;
		case UI_CSS_ALIGN_SELF_STRETCH: YGNodeStyleSetAlignSelf(node, YGAlignStretch);
			break;
		case UI_CSS_ALIGN_SELF_BASELINE: YGNodeStyleSetAlignSelf(node, YGAlignBaseline);
			break;
		case UI_CSS_ALIGN_SPACE_EVENLY: YGNodeStyleSetAlignSelf(node, YGAlignSpaceEvenly);
			break;
		case UI_CSS_ALIGN_SPACE_AROUND: YGNodeStyleSetAlignSelf(node, YGAlignSpaceAround);
			break;
		}
		YGNodeStyleSetFlexGrow(node, widget->getFlexGrow().Value);
		YGNodeStyleSetFlexShrink(node, widget->getFlexShrink().Value);

		for (size_t i = 0; i < widget->getWidgets().size(); ++i)
		{
			auto child = foreach_func(widget->getWidgets()[i].get(), client);
			YGNodeInsertChild(node, child, YGNodeGetChildCount(node));
		}
		return node;
	};

	UILambda<void(YGNodeRef, UIWidgetRaw, UIRect)> layout_func;
	layout_func = [&](YGNodeRef node, UIWidgetRaw widget, UIRect client)
	{
		widget->setLocalBounds({YGNodeLayoutGetLeft(node), YGNodeLayoutGetTop(node), YGNodeLayoutGetWidth(node), YGNodeLayoutGetHeight(node)});
		widget->setBounds({client.X + widget->getLocalBounds().X, client.Y + widget->getLocalBounds().Y, widget->getLocalBounds().W, widget->getLocalBounds().H});
		widget->setViewport({UINAN, UINAN, UINAN, UINAN});

		for (size_t i = 0; i < YGNodeGetChildCount(node) && i < widget->getWidgets().size(); ++i)
		{
			layout_func(YGNodeGetChild(node, i), widget->getWidgets()[i].get(), widget->getBounds());
		}
	};

	UILambda<void(UIWidgetRaw, UIRect, UIRect)> relayout_func;
	relayout_func = [&](UIWidgetRaw widget, UIRect client, UIRect viewport)
	{
		if (std::isnan(widget->getViewport().X) ||
			std::isnan(widget->getViewport().Y) ||
			std::isnan(widget->getViewport().W) ||
			std::isnan(widget->getViewport().H))
			widget->setViewport(UIOverlap(viewport, widget->getBounds()));

		widget->layout(widget->getBounds());
		widget->setBounds({client.X + widget->getLocalBounds().X, client.Y + widget->getLocalBounds().Y, widget->getLocalBounds().W, widget->getLocalBounds().H});
		widget->setViewport(UIOverlap(widget->getViewport(), widget->getBounds()));
		for (size_t i = 0; i < widget->getWidgets().size(); ++i)
		{
			auto bounds = widget->getBounds();
			auto childBounds = widget->getWidgets()[i]->getLocalBounds();
			widget->getWidgets()[i]->setBounds({bounds.X + childBounds.X, bounds.Y + childBounds.Y, childBounds.W, childBounds.H});
		}
		for (size_t i = 0; i < widget->getWidgets().size(); ++i)
		{
			relayout_func(widget->getWidgets()[i].get(), widget->getBounds(), widget->getViewport());
		}
	};

	for (auto& widget : PRIVATE()->TopLevelList)
	{
		style_func(widget.Widget.get(), false);

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
	UILambda<void(UIWidgetRaw, UIRect)> foreach_func;
	foreach_func = [&](UIWidgetRaw widget, UIRect client)
	{
		if (widget->getVisible() == false) return;

		auto source = widget->getTarget();
		if (source == nullptr) return;

		auto& filter = widget->getStyle<UIPropFilter>("filter");
		if (auto render = this->getRender(filter.Func); render && filter.Value)
		{
			render->render(client, source, nullptr, widget->getStyleComputed());
		}

		if (auto render = PRIVATE()->Render.get())
		{
			render->render(client, source, this->getTarget(), widget->getStyleComputed());
		}

		auto childList = widget->getWidgets();
		for (size_t i = 0; i < childList.size(); ++i) foreach_func(childList[i].get(), childList[i]->getBounds());
	};

	for (auto& widget : PRIVATE()->TopLevelList)
	{
		foreach_func(widget.Widget.get(), widget.Widget->getBounds());
	}
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