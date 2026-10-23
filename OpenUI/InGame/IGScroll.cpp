#include "IGScroll.h"
#include "OpenUI/UICanvas.h"
#include <algorithm>

struct IGScrollPrivate : UIPrivate
{
	IGFrameStyle Style;
	float Offset = 0;
};
#define PRIVATE() ((IGScrollPrivate*)m_Private)

IGScroll::IGScroll(UICanvasRaw canvas)
	:
	UIWidget(canvas)
{
	m_Private = new IGScrollPrivate;
	PRIVATE()->Style.Pen = UIFramePen;
	PRIVATE()->Style.Brush = UIInsetBrush;
}

IGScroll::~IGScroll()
{
	delete m_Private; m_Private = nullptr;
}

void IGScroll::arrange(UIRect client)
{
	(void)client;
	if (getWidgets().empty()) return;
	auto content = getWidgets()[0];
	float limit = std::max(0.0f, content->getHeight() - getHeight());
	PRIVATE()->Offset = std::clamp(PRIVATE()->Offset, 0.0f, limit);
	content->setPositionType({UI_CSS_POSITION_ABSOLUTE});
	content->setFixedPos({0.0f, UI_CSS_LEFT_LENGTH}, {-PRIVATE()->Offset, UI_CSS_TOP_LENGTH});
	content->setFixedWidth({100.0f, UI_CSS_WIDTH_PERCENTAGE});
}

void IGScroll::paint(UIRect client, UIPainterRaw painter)
{
	UIWidget::paint(client, painter);
	IGDrawBox(painter, client, PRIVATE()->Style.Pen, PRIVATE()->Style.Brush, 2);
}

float IGScroll::getOffset() const
{
	return PRIVATE()->Offset;
}

void IGScroll::setOffset(float value)
{
	PRIVATE()->Offset = value;
	if (getCanvas()) getCanvas()->layoutWidget();
}

IGFrameStyle IGScroll::getStyle() const
{
	return PRIVATE()->Style;
}

void IGScroll::setStyle(IGFrameStyle value)
{
	PRIVATE()->Style = value;
}

void IGScroll::wheelEvent(UIMouseWheelEventRaw event)
{
	if (event == nullptr || inBounds(event->X, event->Y) == false) return;
	float delta = event->PixelY != 0 ? (float)event->PixelY : (float)event->AngleY;
	PRIVATE()->Offset -= delta;
	event->Accept = true;
	if (getCanvas()) getCanvas()->layoutWidget();
}
#undef PRIVATE
