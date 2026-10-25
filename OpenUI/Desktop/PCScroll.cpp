#include "PCScroll.h"
#include "OpenUI/UICanvas.h"
#include <algorithm>

struct PCScrollPrivate : UIPrivate
{
	float Offset = 0;
	UISignalAs<float> OnScrolled;
};
#define PRIVATE() ((PCScrollPrivate*)m_Private)

PCScroll::PCScroll(UICanvasRaw canvas)
	:
	UIWidget(canvas)
{
	m_Private = new PCScrollPrivate;
	scrolled = &PRIVATE()->OnScrolled;
	PCPlate(getStyleNormal(), PCPen(PCLine()), PCBrush(PCPaper()), PCBrush(PCInk()), 4);
	PCPlate(getStyleDisabled(), PCPen(PCDisabledLine()), PCBrush(PCDisabledFill()), PCBrush(PCDisabledInk()), 4);
}

PCScroll::~PCScroll()
{
	delete m_Private; m_Private = nullptr;
}

void PCScroll::arrange(UIRect client)
{
	(void)client;
	if (getWidgets().empty()) return;
	auto content = getWidgets()[0];
	float limit = std::max(0.0f, content->getHeight() - getHeight());
	PRIVATE()->Offset = std::clamp(PRIVATE()->Offset, 0.0f, limit);
	content->setPositionType({ UI_CSS_POSITION_ABSOLUTE });
	content->setFixedPos({ 0.0f, UI_CSS_LEFT_LENGTH }, { -PRIVATE()->Offset, UI_CSS_TOP_LENGTH });
	content->setFixedWidth({ 100.0f, UI_CSS_WIDTH_PERCENTAGE });
}

void PCScroll::paint(UIRect client, UIPainterRaw painter)
{
	UIWidget::paint(client, painter);
	auto& style = getEnable() ? getStyleNormal() : getStyleDisabled();
	PCDrawFace(painter, client, PCFaceOf(style, client));
}

float PCScroll::getOffset() const
{
	return PRIVATE()->Offset;
}

void PCScroll::setOffset(float value)
{
	if (PRIVATE()->Offset == value) return;
	PRIVATE()->Offset = value;
	PRIVATE()->OnScrolled.signal(value);
	if (getCanvas()) getCanvas()->layoutWidget();
}

void PCScroll::wheelEvent(UIMouseWheelEventRaw event)
{
	if (event == nullptr || getEnable() == false || inBounds(event->X, event->Y) == false) return;
	float delta = event->PixelY != 0 ? (float)event->PixelY : (float)event->AngleY;
	setOffset(PRIVATE()->Offset - delta);
	event->Accept = true;
}
#undef PRIVATE
