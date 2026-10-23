#include "IGHBox.h"

struct IGHBoxPrivate : UIPrivate
{
	IGFrameStyle Style;
};
#define PRIVATE() ((IGHBoxPrivate*)m_Private)

IGHBox::IGHBox(UICanvasRaw canvas)
	:
	UIWidget(canvas)
{
	m_Private = new IGHBoxPrivate;
}

IGHBox::~IGHBox()
{
	delete m_Private; m_Private = nullptr;
}

void IGHBox::arrange(UIRect client)
{
	(void)client;
	this->setFlexWrap({UI_CSS_FLEX_WRAP_NOWRAP});
	this->setAlignItems({UI_CSS_ALIGN_ITEMS_STRETCH});
	this->setFlexDirection({UI_CSS_FLEX_DIRECTION_ROW});
	this->setJustifyContent({UI_CSS_JUSTIFY_CONTENT_FLEX_START});

	for (size_t i = 0; i < getWidgets().size(); ++i)
	{
		auto child = getWidgets()[i];
		if (child->getFixedWidth().Type == UI_CSS_WIDTH_AUTO && child->getMinWidth().Type == UI_CSS_MIN_WIDTH_AUTO)
			child->setFlexGrow({1.0f, UI_CSS_FLEX_GROW_NUMBER});
		else child->setFlexGrow({0.0f, UI_CSS_FLEX_GROW_NUMBER});
		if (child->getFixedHeight().Type == UI_CSS_HEIGHT_AUTO) child->setAlignSelf({UI_CSS_ALIGN_SELF_AUTO});
		else child->setAlignSelf({UI_CSS_ALIGN_SELF_CENTER});
	}
}

void IGHBox::paint(UIRect client, UIPainterRaw painter)
{
	UIWidget::paint(client, painter);
	IGDrawBox(painter, client, PRIVATE()->Style.Pen, PRIVATE()->Style.Brush, 0);
}

IGFrameStyle IGHBox::getStyle() const
{
	return PRIVATE()->Style;
}

void IGHBox::setStyle(IGFrameStyle value)
{
	PRIVATE()->Style = value;
}
#undef PRIVATE
