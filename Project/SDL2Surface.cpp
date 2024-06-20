#include "SDL2Surface.h"
#include "SDL2Painter.h"

SDL2Surface::SDL2Surface(uint32_t width, uint32_t height)
{
	m_WindowSurface = SDL_CreateRGBSurfaceWithFormat(0, width, height, 32, SDL_PixelFormatEnum::SDL_PIXELFORMAT_ARGB32);
	m_NativeSurface = cairo_image_surface_create_for_data((unsigned char*)m_WindowSurface->pixels, CAIRO_FORMAT_ARGB32, m_WindowSurface->w, m_WindowSurface->h, m_WindowSurface->pitch);
	m_NativeContext = cairo_create(m_NativeSurface);
	m_Painter = RmNew<SDL2Painter>(m_NativeContext);
}

SDL2Surface::~SDL2Surface()
{
	cairo_destroy(m_NativeContext);
	cairo_surface_destroy(m_NativeSurface);
	SDL_FreeSurface(m_WindowSurface);
}

uint32_t SDL2Surface::getWidth() const
{
	return m_WindowSurface->w;
}

uint32_t SDL2Surface::getHeight() const
{
	return m_WindowSurface->h;
}

uint32_t SDL2Surface::getStride() const
{
	return m_WindowSurface->pitch;
}

RmArrayView<const uint8_t> SDL2Surface::getPixelData() const
{
	return RmArrayView<const uint8_t>((uint8_t*)m_WindowSurface->pixels, m_WindowSurface->h * m_WindowSurface->w * m_WindowSurface->pitch);
}

RmRaw<IRmGUIPainter> SDL2Surface::getPainter() const
{
	return m_Painter.get();
}

RmRaw<IRmGUIRender> SDL2Surface::getRender() const
{
	return m_Render.get();
}

void SDL2Surface::resize(uint32_t width, uint32_t height)
{
	cairo_destroy(m_NativeContext);
	cairo_surface_destroy(m_NativeSurface);
	SDL_FreeSurface(m_WindowSurface);
	m_WindowSurface = SDL_CreateRGBSurfaceWithFormat(0, width, height, 32, SDL_PixelFormatEnum::SDL_PIXELFORMAT_ARGB32);
	m_NativeSurface = cairo_image_surface_create_for_data((unsigned char*)m_WindowSurface->pixels, CAIRO_FORMAT_ARGB32, m_WindowSurface->w, m_WindowSurface->h, m_WindowSurface->pitch);
	m_NativeContext = cairo_create(m_NativeSurface);
	m_Painter = RmNew<SDL2Painter>(m_NativeContext);
}

SDL_Surface* SDL2Surface::getNativeSurface() const
{
	return m_WindowSurface;
}
