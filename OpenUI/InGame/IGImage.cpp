#include "IGImage.h"

struct IGImagePrivate : UIPrivate
{
	UIImage Image;
	UIList<uint8_t> Pixels;
};
#define PRIVATE() ((IGImagePrivate*)m_Private)

IGImage::IGImage(UICanvasRaw canvas)
	:
	UIWidget(canvas)
{
	m_Private = new IGImagePrivate;
}

IGImage::~IGImage()
{
	delete m_Private; m_Private = nullptr;
}

void IGImage::paint(UIRect client, UIPainterRaw painter)
{
	UIWidget::paint(client, painter);
	if (PRIVATE()->Image.Pixels == nullptr) return;
	float x = client.X + (client.W - PRIVATE()->Image.Width) * 0.5f;
	float y = client.Y + (client.H - PRIVATE()->Image.Height) * 0.5f;
	painter->drawImage(x, y, PRIVATE()->Image);
}

void IGImage::setImage(UIImage image)
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
#undef PRIVATE
