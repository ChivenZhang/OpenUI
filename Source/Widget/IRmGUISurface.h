#pragma once
#include "IRmGUIPainter.h"
class IRmGUIContext;

/// @brief Base interface of native window.
class RMGUI_API IRmGUISurface
{
public:
	virtual ~IRmGUISurface() = default;

	virtual uint32_t getWidth() const = 0;

	virtual uint32_t getHeight() const = 0;

	virtual uint32_t getStride() const = 0;

	virtual RmArrayView<const uint8_t> getPixelData() const = 0;

	virtual RmRaw<IRmGUIPainter> getPainter() const = 0;

	virtual void resize(uint32_t width, uint32_t height) = 0;
};