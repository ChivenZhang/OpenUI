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

class OpenVGTextures
{
public:
	struct texture_t
	{
		uint32_t Handle;
		VGRect Bounds;
	};
	struct context_t
	{
		uint32_t Texture = 0;
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

class SDLGPURenderVG : public VGRender
{
public:
	SDLGPURenderVG();
	~SDLGPURenderVG() override;
	VGImageRaw getTarget() const override;
	void setTarget(VGImageRaw value) override;
	void render(VGRect client, VGListView<const VGPrimitive> data) override;

protected:
	uint32_t m_NativeProgram;
	uint32_t m_NativeVertexBuffer;
	uint32_t m_NativeStyleBuffer;
	uint32_t m_NativeLinearBuffer;
	uint32_t m_NativeRadialBuffer;
	uint32_t m_NativeMatrixBuffer;
	uint32_t m_NativePrimitive;
	size_t m_PrimitiveIndex = 0;

	VGList<VGPrimitive::point_t> m_PointList;
	VGList<VGPrimitive::style_t> m_StyleList;
	VGList<VGPrimitive::linear_t> m_LinearList;
	VGList<VGPrimitive::radial_t> m_RadialList;
	VGList<VGPrimitive::matrix_t> m_MatrixList;
	VGList<OpenVGTextures::texture_t> m_TextureList;
};