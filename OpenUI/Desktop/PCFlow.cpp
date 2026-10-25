#include "PCFlow.h"

PCFlow::PCFlow(UICanvasRaw canvas)
	:
	UIWidget(canvas)
{
}

PCFlow::~PCFlow() = default;

void PCFlow::arrange(UIRect client)
{
	(void)client;
	setFlexDirection({ UI_CSS_FLEX_DIRECTION_ROW });
	setFlexWrap({ UI_CSS_FLEX_WRAP_WRAP });
	setAlignItems({ UI_CSS_ALIGN_ITEMS_FLEX_START });
	setJustifyContent({ UI_CSS_JUSTIFY_CONTENT_FLEX_START });
	for (size_t i = 0; i < getWidgets().size(); ++i)
		getWidgets()[i]->setFlexGrow({ 0.0f, UI_CSS_FLEX_GROW_NUMBER });
}

void PCFlow::paint(UIRect client, UIPainterRaw painter)
{
	UIWidget::paint(client, painter);
	PCDrawFace(painter, client, PCFaceOf(getStyleNormal(), client));
}
