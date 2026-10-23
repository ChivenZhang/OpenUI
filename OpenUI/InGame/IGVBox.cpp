#include "IGVBox.h"

struct IGVBoxPrivate : UIPrivate
{
	IGFrameStyle Style;
};
#define PRIVATE() ((IGVBoxPrivate*)m_Private)

IGVBox::IGVBox(UICanvasRaw canvas)
	:
	UIWidget(canvas)
{
	m_Private = new IGVBoxPrivate;
}

IGVBox::~IGVBox()
{
	delete m_Private; m_Private = nullptr;
}

void IGVBox::arrange(UIRect client)
{
	(void)client;
	this->setFlexWrap({UI_CSS_FLEX_WRAP_NOWRAP});
	this->setAlignItems({UI_CSS_ALIGN_ITEMS_STRETCH});
	this->setFlexDirection({UI_CSS_FLEX_DIRECTION_COLUMN});
	this->setJustifyContent({UI_CSS_JUSTIFY_CONTENT_FLEX_START});

	for (size_t i = 0; i < getWidgets().size(); ++i)
	{
		auto child = getWidgets()[i];
		if (child->getFixedHeight().Type == UI_CSS_HEIGHT_AUTO && child->getMinHeight().Type == UI_CSS_MIN_HEIGHT_AUTO)
			child->setFlexGrow({1.0f, UI_CSS_FLEX_GROW_NUMBER});
		else child->setFlexGrow({0.0f, UI_CSS_FLEX_GROW_NUMBER});
		if (child->getFixedWidth().Type == UI_CSS_WIDTH_AUTO) child->setAlignSelf({UI_CSS_ALIGN_SELF_AUTO});
		else child->setAlignSelf({UI_CSS_ALIGN_SELF_CENTER});
	}
}

void IGVBox::paint(UIRect client, UIPainterRaw painter)
{
	UIWidget::paint(client, painter);
	IGDrawBox(painter, client, PRIVATE()->Style.Pen, PRIVATE()->Style.Brush, 0);
}

IGFrameStyle IGVBox::getStyle() const
{
	return PRIVATE()->Style;
}

void IGVBox::setStyle(IGFrameStyle value)
{
	PRIVATE()->Style = value;
}
#undef PRIVATE
