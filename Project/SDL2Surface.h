#pragma once
#include "Widget/IRmGUISurface.h"
#include <SDL2/SDL.h>
#include <cairo/cairo.h>

class SDL2Surface : public IRmGUISurface
{
public:
	SDL2Surface(uint32_t width, uint32_t height);
	~SDL2Surface();
	virtual uint32_t getWidth() const override;
	virtual uint32_t getHeight() const override;
	virtual uint32_t getStride() const override;
	virtual RmArrayView<const uint8_t> getPixelData() const override;
	virtual RmRaw<IRmGUIPainter> getPainter() const override;
	virtual void resize(uint32_t width, uint32_t height) override;
	SDL_Surface* getNativeSurface() const;

protected:
	SDL_Surface* m_WindowSurface;
	cairo_surface_t* m_NativeSurface;
	cairo_t* m_NativeContext;
	RmRef<IRmGUIPainter> m_Painter;
};