#pragma once
#include <SDL2/SDL.h>
#include "Widget/IRmGUISurface.h"

class SDL2Surface : public IRmGUISurface
{
public:
	SDL2Surface(SDL_Window* native);
	virtual RmRaw<IRmGUIPainter> getPainter() const override;
	virtual RmRaw<IRmGUIContext> getContext() const override;
	virtual void setContext(RmRef<IRmGUIContext> value) override;

protected:
	SDL_Window* m_NativeWindow;
};