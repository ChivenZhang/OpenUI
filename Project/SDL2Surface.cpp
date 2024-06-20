#include "SDL2Surface.h"

SDL2Surface::SDL2Surface(SDL_Window* native)
	:
	m_NativeWindow(native)
{
}

RmRaw<IRmGUIPainter> SDL2Surface::getPainter() const
{
	return RmRaw<IRmGUIPainter>();
}

RmRaw<IRmGUIContext> SDL2Surface::getContext() const
{
	return RmRaw<IRmGUIContext>();
}

void SDL2Surface::setContext(RmRef<IRmGUIContext> value)
{
}