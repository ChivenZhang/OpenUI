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
#include "UIVBox.h"

/// @brief 
struct UIVBoxPrivate : UIPrivate
{
};
#define PRIVATE() ((UIVBoxPrivate*) m_Private)

UIVBox::UIVBox(UICanvasRaw canvas)
	:
	UIWidget(canvas)
{
	m_Private = new UIVBoxPrivate;
}

UIVBox::~UIVBox()
{
	delete m_Private; m_Private = nullptr;
}

void UIVBox::arrange(UIRect client)
{
	this->setFlexWrap({UI_CSS_FLEX_WRAP_NOWRAP});
	this->setAlignItems({UI_CSS_ALIGN_ITEMS_STRETCH});
	this->setFlexDirection({UI_CSS_FLEX_DIRECTION_COLUMN});
	this->setJustifyContent({UI_CSS_JUSTIFY_CONTENT_SPACE_EVENLY});

	for (size_t i = 0; i < getWidgets().size(); ++i)
	{
		auto child = getWidgets()[i];
		if (child->getFixedHeight().Type == UI_CSS_HEIGHT_AUTO) child->setFlexGrow({1.0f, UI_CSS_FLEX_GROW_NUMBER});
		else child->setFlexGrow({0.0f, UI_CSS_FLEX_GROW_NUMBER});
		if (child->getFixedWidth().Type == UI_CSS_WIDTH_AUTO) child->setAlignSelf({UI_CSS_ALIGN_SELF_AUTO});
		else child->setAlignSelf({UI_CSS_ALIGN_SELF_CENTER});
	}
}

void UIVBox::paint(UIRect client, UIPainterRaw painter)
{
	UIWidget::paint(client, painter);
	UIDrawFace(painter, client, UIFaceOf(getStyleNormal(), client));
}

UIVBoxStyle UIVBox::getStyle() const
{
	UIVBoxStyle result;
	readFrame(getStyleNormal(), result.Pen, result.Brush);
	return result;
}

void UIVBox::setStyle(UIVBoxStyle value)
{
	writeFrame(getStyleNormal(), value.Pen, value.Brush);
}