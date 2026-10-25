#include "PCFrame.h"
#include <algorithm>

struct PCFramePrivate : UIPrivate
{
	UIString Title;
	UIFont Font = PCFont(UIFont::AlignLeft | UIFont::AlignVCenter);
};
#define PRIVATE() ((PCFramePrivate*)m_Private)

PCFrame::PCFrame(UICanvasRaw canvas)
	:
	UIWidget(canvas)
{
	m_Private = new PCFramePrivate;
	setPadding({ 12.0f }, { 36.0f }, { 12.0f }, { 12.0f });
	auto border = PCPen(PCLine());
	auto fill = PCBrush(PCCanvas());
	auto text = PCBrush(PCInk());
	PCPlate(getStyleNormal(), border, fill, text, 6);
	PCPlate(getStyleHover(), border, fill, text, 6);
	PCPlate(getStyleActive(), border, fill, text, 6);
	PCPlate(getStyleDisabled(), PCPen(PCDisabledLine()), PCBrush(PCDisabledFill()), PCBrush(PCDisabledInk()), 6);
	assignFontFamily(getStyleNormal(), PRIVATE()->Font.Family);
}

PCFrame::~PCFrame()
{
	delete m_Private; m_Private = nullptr;
}

void PCFrame::arrange(UIRect client)
{
	(void)client;
	setFlexWrap({ UI_CSS_FLEX_WRAP_NOWRAP });
	setAlignItems({ UI_CSS_ALIGN_ITEMS_STRETCH });
	setFlexDirection({ UI_CSS_FLEX_DIRECTION_COLUMN });
	setJustifyContent({ UI_CSS_JUSTIFY_CONTENT_FLEX_START });
}

void PCFrame::paint(UIRect client, UIPainterRaw painter)
{
	UIWidget::paint(client, painter);
	auto& style = getEnable() ? getStyleNormal() : getStyleDisabled();
	auto face = PCFaceOf(style, client);
	PCDrawFace(painter, client, face);
	UIFont font = PRIVATE()->Font;
	applyFontFamily(style, font);
	PCDrawText(painter, { client.X + 12, client.Y + 4, std::max(0.0f, client.W - 24), 28 }, PRIVATE()->Title, font, face.Text);
}

UIString PCFrame::getTitle() const
{
	return PRIVATE()->Title;
}

void PCFrame::setTitle(UIString const& value)
{
	PRIVATE()->Title = value;
}
#undef PRIVATE
