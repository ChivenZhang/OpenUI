#pragma once
/*=================================================
* Copyright © 2020-2025 ChivenZhang.
* All Rights Reserved.
* =====================Note=========================
*
*
* ====================History=======================
* Created by ChivenZhang@gmail.com.
*
* =================================================*/
#include <OpenVG/VGRender.h>
#include <SDL3/SDL_gpu.h>

#include "OpenUI/UI.h"

class OpenVGTextures
{
public:
	struct texture_t
	{
		uint64_t Handle;
		VGRect Bounds;
	};
	struct context_t
	{
		uint64_t Texture = 0;
		uint32_t Format = 0;	// 0: byte , 1: float
		uint32_t X = 0, Y = 0, W = 0, H = 0;
	};

public:
	int renderCreate(context_t& context, int format, int width, int height);
	int renderResize(context_t& context, int format, int width, int height);
	void renderUpdate(context_t& context, int format, int* rect, const unsigned char* data);
	void renderDelete(context_t& context);

public:
	static VGRaw<OpenVGTextures> Instance();
	OpenVGTextures();
	~OpenVGTextures();
	texture_t getTexture(VGImage image, bool glyph = false);

protected:
	context_t m_FontStashTexture;
	VGMap<VGImage, context_t> m_ImageTextureMap;
	VGMap<VGImage, context_t> m_GlyphTextureMap;
};

#include "OpenVG/VGPublic.h"

class SDLGPURenderVG : public VGRender
{
public:
	SDLGPURenderVG(SDL_GPUDevice* device);
	~SDLGPURenderVG() override;
	VGImageRaw getTarget() const override;
	void setTarget(VGImageRaw value) override;
	void render(VGRect client, VGListView<const VGPrimitive> data) override;

protected:
	VGPrivateRaw m_Private;
};