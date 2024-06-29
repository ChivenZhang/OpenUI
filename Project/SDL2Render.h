#pragma once
#include "Widget/IRmGUIRender.h"

class SDL2Render : public IRmGUIRender
{
public:
	SDL2Render();
	~SDL2Render();
	virtual void render(RmArrayView<const RmMeshUV> primitive) override;
};
