#include "PCVBox.h"

PCVBox::PCVBox(UICanvasRaw canvas)
	:
	UIWidget(canvas)
{
}

PCVBox::~PCVBox() = default;

void PCVBox::arrange(UIRect client)
{
	(void)client;
	setFlexWrap({ UI_CSS_FLEX_WRAP_NOWRAP });
	setAlignItems({ UI_CSS_ALIGN_ITEMS_STRETCH });
	setFlexDirection({ UI_CSS_FLEX_DIRECTION_COLUMN });
	setJustifyContent({ UI_CSS_JUSTIFY_CONTENT_FLEX_START });
	for (size_t i = 0; i < getWidgets().size(); ++i)
	{
		auto child = getWidgets()[i];
		if (child->getFixedHeight().Type == UI_CSS_HEIGHT_AUTO && child->getMinHeight().Type == UI_CSS_MIN_HEIGHT_AUTO)
			child->setFlexGrow({ 1.0f, UI_CSS_FLEX_GROW_NUMBER });
		else child->setFlexGrow({ 0.0f, UI_CSS_FLEX_GROW_NUMBER });
		if (child->getFixedWidth().Type == UI_CSS_WIDTH_AUTO) child->setAlignSelf({ UI_CSS_ALIGN_SELF_AUTO });
		else child->setAlignSelf({ UI_CSS_ALIGN_SELF_CENTER });
	}
}

void PCVBox::paint(UIRect client, UIPainterRaw painter)
{
	UIWidget::paint(client, painter);
	PCDrawFace(painter, client, PCFaceOf(getStyleNormal(), client));
}
