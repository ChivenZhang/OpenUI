#include "IGSlot.h"
#include "IGStyle.h"
#include <algorithm>

struct IGSlotPrivate : UIPrivate
{
	UIString Text;
	UIImage Image;
	UIList<uint8_t> Pixels;
	int32_t Count = 0;
	IGRarity Rarity = IGRarity::Common;
	UISignalAs<> OnClicked;
	bool Hovered = false;
	UIFont Font{ .Align = UIFont::AlignCenter | UIFont::AlignVCenter, };
	UIFont CountFont{ .Align = UIFont::AlignRight | UIFont::AlignBottom, };

	UIPen pen() const
	{
		switch (Rarity)
		{
		case IGRarity::Uncommon: return { .Color { 90 / 255.0f, 140 / 255.0f, 78 / 255.0f, 1.0f }, .Width = 1.0f, };
		case IGRarity::Rare: return { .Color { 70 / 255.0f, 120 / 255.0f, 180 / 255.0f, 1.0f }, .Width = 1.0f, };
		case IGRarity::Epic: return { .Color { 150 / 255.0f, 90 / 255.0f, 180 / 255.0f, 1.0f }, .Width = 1.0f, };
		case IGRarity::Legendary: return UIAccentPen;
		default: return UIFramePen;
		}
	}
};
#define PRIVATE() ((IGSlotPrivate*)m_Private)

IGSlot::IGSlot(UICanvasRaw canvas)
	:
	UIWidget(canvas)
{
	m_Private = new IGSlotPrivate;
	clicked = &PRIVATE()->OnClicked;
}

IGSlot::~IGSlot()
{
	delete m_Private; m_Private = nullptr;
}

void IGSlot::paint(UIRect client, UIPainterRaw painter)
{
	UIWidget::paint(client, painter);
	UIPen pen = PRIVATE()->pen();
	if (PRIVATE()->Hovered) pen = UIAccentHoverPen;
	IGDrawBox(painter, client, pen, UIInsetBrush, 2);
	if (PRIVATE()->Image.Pixels)
	{
		float x = client.X + (client.W - PRIVATE()->Image.Width) * 0.5f;
		float y = client.Y + (client.H - PRIVATE()->Image.Height) * 0.5f;
		painter->drawImage(x, y, PRIVATE()->Image);
	}
	else IGDrawText(painter, client, PRIVATE()->Text, PRIVATE()->Font, UITextBrush);
	if (PRIVATE()->Count > 1)
		IGDrawText(painter, client, std::to_string(PRIVATE()->Count), PRIVATE()->CountFont, UIAccentBrush);
}

void IGSlot::setImage(UIImage image)
{
	if (image.Pixels && image.Format == UIImage::Byte && image.Width * 4 == image.Stride)
	{
		auto bytes = image.Width * image.Height * 4;
		PRIVATE()->Pixels.assign((uint8_t*)image.Pixels, (uint8_t*)image.Pixels + bytes);
		PRIVATE()->Image = image;
		PRIVATE()->Image.Pixels = PRIVATE()->Pixels.data();
	}
	else
	{
		PRIVATE()->Pixels.clear();
		PRIVATE()->Image = image;
	}
}

void IGSlot::setCount(int32_t count)
{
	PRIVATE()->Count = std::max(0, count);
}

void IGSlot::setRarity(IGRarity rarity)
{
	PRIVATE()->Rarity = rarity;
}

void IGSlot::setText(UIString const& text)
{
	PRIVATE()->Text = text;
}

void IGSlot::mousePressEvent(UIMouseEventRaw event)
{
	if (getEnable() == false || event == nullptr || event->Button != UIInputEnum::MOUSE_BUTTON_LEFT) return;
	if (inBounds(event->X, event->Y) == false) return;
	PRIVATE()->OnClicked.signal();
	event->Accept = true;
}

void IGSlot::mouseMoveEvent(UIMouseEventRaw event)
{
	if (event == nullptr) return;
	PRIVATE()->Hovered = inBounds(event->X, event->Y);
}

void IGSlot::leaveEvent(UIMouseEventRaw event)
{
	(void)event;
	PRIVATE()->Hovered = false;
}
#undef PRIVATE
