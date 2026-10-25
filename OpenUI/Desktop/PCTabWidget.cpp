#include "PCTabWidget.h"
#include "OpenUI/UICanvas.h"
#include <algorithm>

struct PCTabWidgetPrivate : UIPrivate
{
	UIStringList Titles;
	UIFont Font = PCFont(UIFont::AlignCenter | UIFont::AlignVCenter);
	UISignalAs<int32_t> OnCurrentChanged;
	int32_t Index = 0;
	int32_t Hover = -1;
};
#define PRIVATE() ((PCTabWidgetPrivate*)m_Private)

static float PCTabBar = 32.0f;

PCTabWidget::PCTabWidget(UICanvasRaw canvas)
	:
	UIWidget(canvas)
{
	m_Private = new PCTabWidgetPrivate;
	currentChanged = &PRIVATE()->OnCurrentChanged;
	setMinSize(160, 120);
	PCPlate(getStyleNormal(), PCPen(PCLine()), PCBrush(PCCanvas()), PCBrush(PCInk()), 0);
	PCPlate(getStyleHover(), PCPen(PCLine()), PCBrush(PCHover()), PCBrush(PCInk()), 0);
	PCPlate(getStyleChecked(), PCPen(PCLine()), PCBrush(PCPaper()), PCBrush(PCAccent()), 0);
	PCPlate(getStyleDisabled(), PCPen(PCDisabledLine()), PCBrush(PCDisabledFill()), PCBrush(PCDisabledInk()), 0);
	assignFontFamily(getStyleNormal(), PRIVATE()->Font.Family);
}

PCTabWidget::~PCTabWidget()
{
	delete m_Private; m_Private = nullptr;
}

int32_t PCTabWidget::addTab(UIString const& title, UIWidgetRef page)
{
	PRIVATE()->Titles.push_back(title);
	if (page) addWidget(page);
	if ((int32_t)PRIVATE()->Titles.size() == 1) PRIVATE()->Index = 0;
	return (int32_t)PRIVATE()->Titles.size() - 1;
}

void PCTabWidget::arrange(UIRect client)
{
	(void)client;
	auto count = (int32_t)getWidgets().size();
	if (PRIVATE()->Index >= count) PRIVATE()->Index = std::max(0, count - 1);
	for (int32_t i = 0; i < count; ++i)
	{
		auto child = getWidgets()[i];
		child->setPositionType({ UI_CSS_POSITION_ABSOLUTE });
		child->setFixedPos({ 0.0f, UI_CSS_LEFT_LENGTH }, { PCTabBar, UI_CSS_TOP_LENGTH });
		child->setFixedSize({ 100.0f, UI_CSS_WIDTH_PERCENTAGE }, { 0.0f, UI_CSS_HEIGHT_AUTO });
		child->setFixedHeight(i == PRIVATE()->Index ? UIPropHeight{ std::max(0.0f, getHeight() - PCTabBar), UI_CSS_HEIGHT_LENGTH } : UIPropHeight{ 0.0f, UI_CSS_HEIGHT_LENGTH });
		child->setDisplayType(i == PRIVATE()->Index ? UIPropDisplay{ UI_CSS_DISPLAY_FLEX } : UIPropDisplay{ UI_CSS_DISPLAY_NONE });
	}
}

void PCTabWidget::paint(UIRect client, UIPainterRaw painter)
{
	UIWidget::paint(client, painter);
	auto plate = PCFaceOf(getEnable() ? getStyleNormal() : getStyleDisabled(), client);
	PCDrawFace(painter, client, plate);
	int32_t count = (int32_t)PRIVATE()->Titles.size();
	if (count <= 0) return;
	float width = client.W / (float)count;
	UIFont font = PRIVATE()->Font;
	applyFontFamily(getStyleNormal(), font);
	for (int32_t i = 0; i < count; ++i)
	{
		UIRect tab{ client.X + i * width, client.Y, width, PCTabBar };
		UIWidgetStyle& style = getEnable() == false ? getStyleDisabled()
			: i == PRIVATE()->Index ? getStyleChecked()
			: i == PRIVATE()->Hover ? getStyleHover()
			: getStyleNormal();
		auto face = PCFaceOf(style, tab);
		PCDrawFace(painter, tab, face);
		PCDrawText(painter, tab, PRIVATE()->Titles[i], font, face.Text);
	}
}

int32_t PCTabWidget::getCurrentIndex() const
{
	return PRIVATE()->Index;
}

void PCTabWidget::setCurrentIndex(int32_t value)
{
	auto count = (int32_t)PRIVATE()->Titles.size();
	if (count == 0) return;
	value = std::clamp(value, 0, count - 1);
	if (PRIVATE()->Index == value) return;
	PRIVATE()->Index = value;
	PRIVATE()->OnCurrentChanged.signal(value);
	if (getCanvas()) getCanvas()->layoutWidget();
}

UIString PCTabWidget::getTabText(int32_t index) const
{
	if (index < 0 || index >= (int32_t)PRIVATE()->Titles.size()) return {};
	return PRIVATE()->Titles[index];
}

void PCTabWidget::setTabText(int32_t index, UIString const& value)
{
	if (index < 0 || index >= (int32_t)PRIVATE()->Titles.size()) return;
	PRIVATE()->Titles[index] = value;
}

void PCTabWidget::mousePressEvent(UIMouseEventRaw event)
{
	if (getEnable() == false || event == nullptr || event->Button != UIInputEnum::MOUSE_BUTTON_LEFT) return;
	auto bounds = getBounds();
	if (event->Y < bounds.Y || event->Y > bounds.Y + PCTabBar) return;
	int32_t count = (int32_t)PRIVATE()->Titles.size();
	if (count <= 0 || bounds.W <= 0) return;
	int32_t index = (int32_t)((event->X - bounds.X) / (bounds.W / (float)count));
	setCurrentIndex(index);
	event->Accept = true;
}

void PCTabWidget::mouseMoveEvent(UIMouseEventRaw event)
{
	if (event == nullptr) return;
	auto bounds = getBounds();
	int32_t count = (int32_t)PRIVATE()->Titles.size();
	if (count <= 0 || event->Y < bounds.Y || event->Y > bounds.Y + PCTabBar || bounds.W <= 0) PRIVATE()->Hover = -1;
	else PRIVATE()->Hover = (int32_t)((event->X - bounds.X) / (bounds.W / (float)count));
}

void PCTabWidget::leaveEvent(UIMouseEventRaw event)
{
	(void)event;
	PRIVATE()->Hover = -1;
}
#undef PRIVATE
