#include "PCHBox.h"

PCHBox::PCHBox(UICanvasRaw canvas)
	:
	UIWidget(canvas)
{
}

PCHBox::~PCHBox() = default;

void PCHBox::arrange(UIRect client)
{
	(void)client;
	setFlexWrap({ UI_CSS_FLEX_WRAP_NOWRAP });
	setAlignItems({ UI_CSS_ALIGN_ITEMS_STRETCH });
	setFlexDirection({ UI_CSS_FLEX_DIRECTION_ROW });
	setJustifyContent({ UI_CSS_JUSTIFY_CONTENT_FLEX_START });
	for (size_t i = 0; i < getWidgets().size(); ++i)
	{
		auto child = getWidgets()[i];
		if (child->getFixedWidth().Type == UI_CSS_WIDTH_AUTO && child->getMinWidth().Type == UI_CSS_MIN_WIDTH_AUTO)
			child->setFlexGrow({ 1.0f, UI_CSS_FLEX_GROW_NUMBER });
		else child->setFlexGrow({ 0.0f, UI_CSS_FLEX_GROW_NUMBER });
		if (child->getFixedHeight().Type == UI_CSS_HEIGHT_AUTO) child->setAlignSelf({ UI_CSS_ALIGN_SELF_AUTO });
		else child->setAlignSelf({ UI_CSS_ALIGN_SELF_CENTER });
	}
}

void PCHBox::paint(UIRect client, UIPainterRaw painter)
{
	UIWidget::paint(client, painter);
	PCDrawFace(painter, client, PCFaceOf(getStyleNormal(), client));
}
