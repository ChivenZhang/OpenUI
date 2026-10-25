#include "PCStack.h"
#include "OpenUI/UICanvas.h"
#include <algorithm>

struct PCStackPrivate : UIPrivate
{
	int32_t Index = 0;
	UISignalAs<int32_t> OnCurrentChanged;
};
#define PRIVATE() ((PCStackPrivate*)m_Private)

PCStack::PCStack(UICanvasRaw canvas)
	:
	UIWidget(canvas)
{
	m_Private = new PCStackPrivate;
	currentChanged = &PRIVATE()->OnCurrentChanged;
}

PCStack::~PCStack()
{
	delete m_Private; m_Private = nullptr;
}

void PCStack::arrange(UIRect client)
{
	(void)client;
	auto count = (int32_t)getWidgets().size();
	if (count == 0) return;
	if (PRIVATE()->Index < 0 || PRIVATE()->Index >= count) PRIVATE()->Index = 0;
	for (int32_t i = 0; i < count; ++i)
	{
		auto child = getWidgets()[i];
		child->setPositionType({ UI_CSS_POSITION_ABSOLUTE });
		child->setFixedPos({ 0.0f, UI_CSS_LEFT_PERCENTAGE }, { 0.0f, UI_CSS_TOP_PERCENTAGE });
		child->setFixedSize({ 100.0f, UI_CSS_WIDTH_PERCENTAGE }, { 100.0f, UI_CSS_HEIGHT_PERCENTAGE });
		child->setDisplayType(i == PRIVATE()->Index ? UIPropDisplay{ UI_CSS_DISPLAY_FLEX } : UIPropDisplay{ UI_CSS_DISPLAY_NONE });
	}
}

void PCStack::paint(UIRect client, UIPainterRaw painter)
{
	UIWidget::paint(client, painter);
	PCDrawFace(painter, client, PCFaceOf(getStyleNormal(), client));
}

int32_t PCStack::getCurrentIndex() const
{
	return PRIVATE()->Index;
}

void PCStack::setCurrentIndex(int32_t value)
{
	auto count = (int32_t)getWidgets().size();
	if (count == 0) value = 0;
	else value = std::clamp(value, 0, count - 1);
	if (PRIVATE()->Index == value) return;
	PRIVATE()->Index = value;
	PRIVATE()->OnCurrentChanged.signal(value);
	if (getCanvas()) getCanvas()->layoutWidget();
}
#undef PRIVATE
