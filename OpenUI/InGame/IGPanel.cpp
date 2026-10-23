#include "IGPanel.h"

struct IGPanelPrivate : UIPrivate
{
	IGFrameStyle Style{ .Pen = UIFramePen, .Brush = UIInsetBrush, };
	UIString Title;
	UIFont Font{ .Align = UIFont::AlignLeft | UIFont::AlignVCenter, };
};
#define PRIVATE() ((IGPanelPrivate*)m_Private)

IGPanel::IGPanel(UICanvasRaw canvas)
	:
	UIWidget(canvas)
{
	m_Private = new IGPanelPrivate;
}

IGPanel::~IGPanel()
{
	delete m_Private; m_Private = nullptr;
}

void IGPanel::arrange(UIRect client)
{
	(void)client;
	float top = PRIVATE()->Title.empty() ? 8.0f : 32.0f;
	this->setPadding({8.0f}, {top}, {8.0f}, {8.0f});
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

void IGPanel::paint(UIRect client, UIPainterRaw painter)
{
	UIWidget::paint(client, painter);
	IGDrawBox(painter, client, PRIVATE()->Style.Pen, PRIVATE()->Style.Brush, 2);
	if (PRIVATE()->Title.empty()) return;
	UIRect title{ client.X + 12, client.Y + 4, std::max(0.0f, client.W - 24), 24 };
	IGDrawText(painter, title, PRIVATE()->Title, PRIVATE()->Font, UIAccentBrush);
}

UIString IGPanel::getTitle() const
{
	return PRIVATE()->Title;
}

void IGPanel::setTitle(UIString const& text)
{
	PRIVATE()->Title = text;
}

IGFrameStyle IGPanel::getStyle() const
{
	return PRIVATE()->Style;
}

void IGPanel::setStyle(IGFrameStyle value)
{
	PRIVATE()->Style = value;
}
#undef PRIVATE
