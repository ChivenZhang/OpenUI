#pragma once
#include "Render/IRmGUIRender.h"

class RmGUIRender : public IRmGUIRender
{
public:
	virtual void render(RmArrayView<data_t> data) override;
};