//
// Copyright (c) 2013 Mikko Mononen memon@inside.org
//
// This software is provided 'as-is', without any express or implied
// warranty.  In no event will the authors be held liable for any damages
// arising from the use of this software.
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
//
// NanoVG SDL_gpu (SDL3 GPU API) rendering backend.
//
// This is a single-header backend for NanoVG that renders using SDL3's
// modern GPU API (SDL_gpu.h) which abstracts Vulkan / D3D12 / Metal.
//
// Unlike the OpenGL backend (nanovg_gl.h), SDL_gpu requires *pre-compiled*
// shader bytecode (SPIR-V for Vulkan, DXBC/DXIL for D3D12, MSL for Metal).
// The canonical NanoVG shader sources are embedded at the bottom of this
// file (in the implementation section) as `sg_glsl_vertex_shader` and
// `sg_glsl_fragment_shader`. They are authored in GLSL (450 core) with the
// SDL_gpu resource binding conventions already applied, so they can be
// compiled offline to SPIR-V, or cross-compiled with SDL_shadercross /
// glslang / DXC for the other backends.
//
// Usage:
//
//   1. In exactly one .c/.cpp file:
//
//        #define NANOVG_SDLGPU_IMPLEMENTATION
//        #include "nanovg_sdlgpu.h"
//
//   2. Everywhere else, include the header normally (declarations only).
//
//   3. Create a device, compile the shaders for the format the device
//      selected, create the context and render:
//
//        SDL_GPUDevice* dev = SDL_CreateGPUDevice(formatFlags, true, NULL);
//        SDL_ClaimWindowForGPUDevice(dev, window);
//
//        NVGsgShaderBundle shaders = {
//            .format = SDL_GPU_SHADERFORMAT_SPIRV,   // must match device
//            .vertexShader = spirv_vertex, .vertexShaderSize = sizeof(spirv_vertex),
//            .fragmentShader = spirv_fragment, .fragmentShaderSize = sizeof(spirv_fragment),
//        };
//        NVGcontext* vg = nvgCreateSDLGPU(dev, window, NVG_ANTIALIAS, &shaders);
//
//        // per frame:
//        SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(dev);
//        SDL_GPUTexture* swap; Uint32 w, h;
//        SDL_WaitAndAcquireGPUSwapchainTexture(cmd, window, &swap, &w, &h);
//        SDL_GPUTextureFormat fmt = SDL_GetGPUSwapchainTextureFormat(dev, window);
//        nvgsgSetRenderTarget(vg, cmd, swap, fmt, (int)w, (int)h,
//                             SDL_GPU_LOADOP_CLEAR, (SDL_FColor){0,0,0,0});
//        nvgBeginFrame(vg, (float)w, (float)h, 1.0f);
//        ... nvgRect/nvgText ... 
//        nvgEndFrame(vg);            // records into `cmd` (does not submit)
//        SDL_SubmitGPUCommandBuffer(cmd);
//
#ifndef NANOVG_SDLGPU_H
#define NANOVG_SDLGPU_H

#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>
#include "nanovg.h"

#ifdef __cplusplus
extern "C" {
#endif

// Do not delete the SDL_GPUTexture when the image is deleted. Used together
// with nvgsgCreateImageFromTexture() for textures owned by the caller.
#define NVG_IMAGE_NODELETE (1<<16)

// Create flags for nvgCreateSDLGPU.
enum NVGsgCreateFlags {
	NVG_ANTIALIAS       = 1<<0,  // Geometry-based anti-aliasing (may not be needed with MSAA).
	NVG_STENCIL_STROKES = 1<<1,  // Stroke via stencil buffer; avoids overdraw on self-intersections.
	NVG_DEBUG           = 1<<2,  // Additional debug checks.
};

// Pre-compiled shaders required by SDL_gpu. The caller provides the vertex
// and fragment shader bytecode in a format accepted by the device.
typedef struct NVGsgShaderBundle {
	SDL_GPUShaderFormat format;      // e.g. SDL_GPU_SHADERFORMAT_SPIRV
	const Uint8* vertexShader;       // compiled vertex shader bytecode
	Uint32 vertexShaderSize;         // size of `vertexShader` in bytes
	const Uint8* fragmentShader;     // compiled fragment shader bytecode
	Uint32 fragmentShaderSize;       // size of `fragmentShader` in bytes
} NVGsgShaderBundle;

// Creates a NanoVG context that renders with SDL_gpu. `window` is used only
// if you let SDL manage the swapchain format; it may be NULL when rendering
// exclusively to offscreen textures (the color format is then supplied via
// nvgsgSetRenderTarget). Returns NULL on failure.
NVGcontext* nvgCreateSDLGPU(SDL_GPUDevice* device, SDL_Window* window, int flags, const NVGsgShaderBundle* shaders);

// Destroys a context created with nvgCreateSDLGPU.
void nvgDeleteSDLGPU(NVGcontext* ctx);

// Selects where NanoVG will draw for the current frame. Must be called
// before nvgEndFrame() (typically right before nvgBeginFrame()).
//
//   commandBuffer - the command buffer the app will submit after nvgEndFrame.
//                   All render commands for the frame are recorded into it.
//   colorTarget   - color render target (swapchain texture or a texture
//                   created with SDL_GPU_TEXTUREUSAGE_COLOR_TARGET).
//   colorFormat   - pixel format of `colorTarget` (pipelines are compiled
//                   against it). For the swapchain use
//                   SDL_GetGPUSwapchainTextureFormat().
//   width/height  - size of `colorTarget` in pixels.
//   loadOp/clearColor - how the color target is initialized for the pass.
void nvgsgSetRenderTarget(NVGcontext* ctx, SDL_GPUCommandBuffer* commandBuffer,
						  SDL_GPUTexture* colorTarget, SDL_GPUTextureFormat colorFormat,
						  int width, int height, SDL_GPULoadOp loadOp, SDL_FColor clearColor);

// Wraps an externally created SDL_GPUTexture as a NanoVG image. The texture
// is not released when the image is deleted (NVG_IMAGE_NODELETE is implied).
int nvgsgCreateImageFromTexture(NVGcontext* ctx, SDL_GPUTexture* texture, int w, int h, int imageFlags);

// Returns the SDL_GPUTexture backing the given NanoVG image, or NULL.
SDL_GPUTexture* nvgsgImageHandle(NVGcontext* ctx, int image);

#ifdef __cplusplus
}
#endif

#endif /* NANOVG_SDLGPU_H */

#ifdef NANOVG_SDLGPU_IMPLEMENTATION

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stddef.h>

// ---------------------------------------------------------------------------
// Canonical NanoVG shaders (GLSL 450 / Vulkan binding conventions).
//
// The resource layout follows SDL_CreateGPUShader()'s SPIR-V convention:
//   vertex shader   uniforms  -> set = 1
//   fragment shader samplers -> set = 2 (sampler first, binding 0)
//   fragment shader uniforms -> set = 3
//
// For HLSL: vertex uniforms -> b0 space1; fragment samplers -> (t0,s0) space2;
//           fragment uniforms -> b0 space3.
// For MSL: use [[stage_in]] for vertices; samplers/sampler first in the
//          [[texture]]/[[sampler]] tables; uniforms first in [[buffer]].
// ---------------------------------------------------------------------------

static constexpr char sg_glsl_vertex_shader[] =
	"#version 450\n"
	"layout(std140, set = 1, binding = 0) uniform VertexUniforms {\n"
	"	vec4 viewSize;\n"
	"} vtx;\n"
	"layout(location = 0) in vec2 vertex;\n"
	"layout(location = 1) in vec2 tcoord;\n"
	"layout(location = 0) out vec2 ftcoord;\n"
	"layout(location = 1) out vec2 fpos;\n"
	"void main(void) {\n"
	"	ftcoord = tcoord;\n"
	"	fpos = vertex;\n"
	"	gl_Position = vec4(2.0*vertex.x/vtx.viewSize.x - 1.0, 1.0 - 2.0*vertex.y/vtx.viewSize.y, 0.0, 1.0);\n"
	"}\n";

static constexpr char sg_glsl_fragment_shader[] =
	"#version 450\n"
	"layout(std140, set = 3, binding = 0) uniform FragUniforms {\n"
	"	vec4 scissorMat[3];\n"
	"	vec4 paintMat[3];\n"
	"	vec4 innerCol;\n"
	"	vec4 outerCol;\n"
	"	vec4 scissorExtScale;\n"     // xy = extent, zw = scale
	"	vec4 extentRadiusFeather;\n" // xy = extent, z = radius, w = feather
	"	vec4 strokeData;\n"          // x = strokeMult, y = strokeThr, z = texType, w = type
	"} frag;\n"
	"layout(set = 2, binding = 0) uniform sampler2D tex;\n"
	"layout(location = 0) in vec2 ftcoord;\n"
	"layout(location = 1) in vec2 fpos;\n"
	"layout(location = 0) out vec4 outColor;\n"
	"\n"
	"vec2 sgMul3x3(vec4 m[3], vec2 p) {\n"
	"	return vec2(m[0].x*p.x + m[1].x*p.y + m[2].x,\n"
	"	            m[0].y*p.x + m[1].y*p.y + m[2].y);\n"
	"}\n"
	"\n"
	"float sgRoundRect(vec2 pt, vec2 ext, float rad) {\n"
	"	vec2 ext2 = ext - vec2(rad, rad);\n"
	"	vec2 d = abs(pt) - ext2;\n"
	"	return min(max(d.x, d.y), 0.0) + length(max(d, 0.0)) - rad;\n"
	"}\n"
	"\n"
	"float sgScissorMask(vec2 p) {\n"
	"	vec2 sc = abs(sgMul3x3(frag.scissorMat, p)) - frag.scissorExtScale.xy;\n"
	"	sc = vec2(0.5, 0.5) - sc * frag.scissorExtScale.zw;\n"
	"	return clamp(sc.x, 0.0, 1.0) * clamp(sc.y, 0.0, 1.0);\n"
	"}\n"
	"\n"
	"float sgStrokeMask(void) {\n"
	"	return min(1.0, (1.0 - abs(ftcoord.x*2.0 - 1.0)) * frag.strokeData.x)\n"
	"	     * min(1.0, ftcoord.y);\n"
	"}\n"
	"\n"
	"void main(void) {\n"
	"	vec4 result;\n"
	"	float scissor = sgScissorMask(fpos);\n"
	"	float strokeAlpha = sgStrokeMask();\n"
	"	if (strokeAlpha < frag.strokeData.y) discard;\n"
	"	if (frag.strokeData.w == 0.0) {\n"          // gradient
	"		vec2 pt = sgMul3x3(frag.paintMat, fpos);\n"
	"		float d = clamp((sgRoundRect(pt, frag.extentRadiusFeather.xy, frag.extentRadiusFeather.z)\n"
	"		              + frag.extentRadiusFeather.w*0.5) / frag.extentRadiusFeather.w, 0.0, 1.0);\n"
	"		vec4 color = mix(frag.innerCol, frag.outerCol, d);\n"
	"		color *= strokeAlpha * scissor;\n"
	"		result = color;\n"
	"	} else if (frag.strokeData.w == 1.0) {\n"    // image fill
	"		vec2 pt = sgMul3x3(frag.paintMat, fpos) / frag.extentRadiusFeather.xy;\n"
	"		vec4 color = texture(tex, pt);\n"
	"		if (frag.strokeData.z == 1.0) color = vec4(color.xyz*color.w, color.w);\n"
	"		if (frag.strokeData.z == 2.0) color = vec4(color.x);\n"
	"		color *= frag.innerCol;\n"
	"		color *= strokeAlpha * scissor;\n"
	"		result = color;\n"
	"	} else if (frag.strokeData.w == 2.0) {\n"    // stencil fill (simple)
	"		result = vec4(1.0);\n"
	"	} else {\n"                                  // textured triangles
	"		vec4 color = texture(tex, ftcoord);\n"
	"		if (frag.strokeData.z == 1.0) color = vec4(color.xyz*color.w, color.w);\n"
	"		if (frag.strokeData.z == 2.0) color = vec4(color.x);\n"
	"		color *= scissor;\n"
	"		result = color * frag.innerCol;\n"
	"	}\n"
	"	outColor = result;\n"
	"}\n";

// ---------------------------------------------------------------------------
// Internal types
// ---------------------------------------------------------------------------

typedef struct SGtexture {
	int id;
	SDL_GPUTexture* tex;
	SDL_GPUSampler* sampler;
	int type;   // NVG_TEXTURE_ALPHA / NVG_TEXTURE_RGBA
	int flags;  // NVG_IMAGE_*
	int width;
	int height;
} SGtexture;

typedef struct SGblend {
	SDL_GPUBlendFactor srcRGB;
	SDL_GPUBlendFactor dstRGB;
	SDL_GPUBlendFactor srcAlpha;
	SDL_GPUBlendFactor dstAlpha;
} SGblend;

enum SGcallType {
	SG_NONE = 0,
	SG_FILL,
	SG_CONVEXFILL,
	SG_STROKE,
	SG_TRIANGLES,
};

typedef struct SGpath {
	int triOffset;      // triangulated fan (TRIANGLELIST)
	int triCount;
	int strokeOffset;   // fringe strip (TRIANGLESTRIP)
	int strokeCount;
} SGpath;

typedef struct SGcall {
	int type;
	int image;
	SGblend blend;
	int pathOffset;
	int pathCount;
	int quadOffset;     // FILL: bounding-box strip (4 verts)
	int triOffset;      // TRIANGLES
	int triCount;
	int uniformOffset;  // in SGfragUniforms units
	int uniformCount;   // 1 or 2
} SGcall;

// std140 layout; must exactly match the FragUniforms block in
// sg_glsl_fragment_shader above (176 bytes total).
typedef struct SGfragUniforms {
	union {
		float data[44];
		struct {
			float scissorMat[12];   // 48 bytes (3 x vec4)
			float paintMat[12];     // 48 bytes (3 x vec4)
			float innerCol[4];      // 16 bytes
			float outerCol[4];      // 16 bytes
			float scissorExt[2];    // } 16 bytes
			float scissorScale[2];  // }
			float extent[2];        // } 16 bytes
			float radius;           // }
			float feather;          // }
			float strokeMult;       // } 16 bytes
			float strokeThr;        // }
			float texType;          // }
			float type;             // }
		};
	};
} SGfragUniforms;

enum SGPipeKind {
	SG_PIPE_FILL_STENCIL = 0,      // TRIANGLELIST, stencil INCR_WRAP/DECR_WRAP, color off
	SG_PIPE_FILL_FRINGE,           // TRIANGLESTRIP, stencil EQUAL 0 keep (also stroke AA)
	SG_PIPE_FILL_FINAL,            // TRIANGLESTRIP, stencil NOTEQUAL 0 zero
	SG_PIPE_CONVEX,                // TRIANGLELIST, no stencil (also triangles)
	SG_PIPE_STRIP,                 // TRIANGLESTRIP, no stencil (plain stroke / convex fringe)
	SG_PIPE_STROKE_STENCIL_FILL,   // TRIANGLESTRIP, stencil EQUAL 0 incr
	SG_PIPE_STROKE_STENCIL_CLEAR,  // TRIANGLESTRIP, stencil ALWAYS zero, color off
	SG_PIPE_COUNT
};

typedef struct SGpipelineKey {
	int kind;
	SDL_GPUTextureFormat colorFormat;
	SGblend blend;
} SGpipelineKey;

typedef struct SGpipelineEntry {
	SGpipelineKey key;
	SDL_GPUGraphicsPipeline* pipeline;
} SGpipelineEntry;

typedef struct SGNVGcontext {
	SDL_GPUDevice* dev;
	int flags;
	NVGsgShaderBundle shaders;

	// shaders
	SDL_GPUShader* vs;
	SDL_GPUShader* fs;

	// dummy 1x1 white texture + sampler
	SDL_GPUTexture* dummyTex;
	SDL_GPUSampler* dummySampler;

	// dynamic vertex buffer + upload staging buffer
	SDL_GPUBuffer* vertBuf;
	SDL_GPUTransferBuffer* uploadBuf;
	Uint32 vertBufSize;

	// render target state (set per frame via nvgsgSetRenderTarget)
	SDL_GPUCommandBuffer* commandBuffer;
	SDL_GPUTexture* colorTarget;
	SDL_GPUTextureFormat colorFormat;
	SDL_GPULoadOp loadOp;
	SDL_FColor clearColor;
	int rtW, rtH;

	// depth-stencil target (owned; recreated on resize)
	SDL_GPUTexture* dsTex;
	int dsW, dsH;
	SDL_GPUTextureFormat dsFormat;
	int hasStencil;

	// textures
	SGtexture* textures;
	int ntextures, ctextures, textureId;

	// per-frame CPU buffers
	SGcall* calls;
	int ncalls, ccalls;
	SGpath* paths;
	int npaths, cpaths;
	NVGvertex* verts;
	int nverts, cverts;
	unsigned char* uniforms;
	int nuniforms, cuniforms;
	int fragSize;

	// pipeline cache
	SGpipelineEntry* pipeCache;
	int npipeCache, cpipeCache;

	float view[2];
} SGNVGcontext;

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

static int sg__maxi(int a, int b) { return a > b ? a : b; }

static void sg__vset(NVGvertex* vtx, float x, float y, float u, float v)
{
	vtx->x = x; vtx->y = y; vtx->u = u; vtx->v = v;
}

static void sg__xformToMat3x4(float* m3, const float* t)
{
	m3[0] = t[0]; m3[1] = t[1]; m3[2] = 0.0f; m3[3] = 0.0f;
	m3[4] = t[2]; m3[5] = t[3]; m3[6] = 0.0f; m3[7] = 0.0f;
	m3[8] = t[4]; m3[9] = t[5]; m3[10] = 1.0f; m3[11] = 0.0f;
}

static void sg__premulColor(float* dst, NVGcolor c)
{
	dst[0] = c.r * c.a;
	dst[1] = c.g * c.a;
	dst[2] = c.b * c.a;
	dst[3] = c.a;
}

static SDL_GPUBlendFactor sg__blendFactor(int f)
{
	switch (f) {
		case NVG_ZERO: return SDL_GPU_BLENDFACTOR_ZERO;
		case NVG_ONE: return SDL_GPU_BLENDFACTOR_ONE;
		case NVG_SRC_COLOR: return SDL_GPU_BLENDFACTOR_SRC_COLOR;
		case NVG_ONE_MINUS_SRC_COLOR: return SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_COLOR;
		case NVG_DST_COLOR: return SDL_GPU_BLENDFACTOR_DST_COLOR;
		case NVG_ONE_MINUS_DST_COLOR: return SDL_GPU_BLENDFACTOR_ONE_MINUS_DST_COLOR;
		case NVG_SRC_ALPHA: return SDL_GPU_BLENDFACTOR_SRC_ALPHA;
		case NVG_ONE_MINUS_SRC_ALPHA: return SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
		case NVG_DST_ALPHA: return SDL_GPU_BLENDFACTOR_DST_ALPHA;
		case NVG_ONE_MINUS_DST_ALPHA: return SDL_GPU_BLENDFACTOR_ONE_MINUS_DST_ALPHA;
		case NVG_SRC_ALPHA_SATURATE: return SDL_GPU_BLENDFACTOR_SRC_ALPHA_SATURATE;
		default: return SDL_GPU_BLENDFACTOR_INVALID;
	}
}

static SGblend sg__blendFromComposite(NVGcompositeOperationState op)
{
	SGblend blend;
	blend.srcRGB = sg__blendFactor(op.srcRGB);
	blend.dstRGB = sg__blendFactor(op.dstRGB);
	blend.srcAlpha = sg__blendFactor(op.srcAlpha);
	blend.dstAlpha = sg__blendFactor(op.dstAlpha);
	if (blend.srcRGB == SDL_GPU_BLENDFACTOR_INVALID ||
		blend.dstRGB == SDL_GPU_BLENDFACTOR_INVALID ||
		blend.srcAlpha == SDL_GPU_BLENDFACTOR_INVALID ||
		blend.dstAlpha == SDL_GPU_BLENDFACTOR_INVALID) {
		// Fall back to premultiplied alpha source-over.
		blend.srcRGB = SDL_GPU_BLENDFACTOR_ONE;
		blend.dstRGB = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
		blend.srcAlpha = SDL_GPU_BLENDFACTOR_ONE;
		blend.dstAlpha = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
	}
	return blend;
}

// ---------------------------------------------------------------------------
// Texture management
// ---------------------------------------------------------------------------

static SGtexture* sg__allocTexture(SGNVGcontext* sg)
{
	SGtexture* tex = NULL;
	int i;
	for (i = 0; i < sg->ntextures; i++) {
		if (sg->textures[i].id == 0) { tex = &sg->textures[i]; break; }
	}
	if (tex == NULL) {
		if (sg->ntextures + 1 > sg->ctextures) {
			SGtexture* textures;
			int ctextures = sg__maxi(sg->ntextures + 1, 4) + sg->ctextures / 2;
			textures = (SGtexture*)realloc(sg->textures, sizeof(SGtexture) * ctextures);
			if (textures == NULL) return NULL;
			sg->textures = textures;
			sg->ctextures = ctextures;
		}
		tex = &sg->textures[sg->ntextures++];
	}
	memset(tex, 0, sizeof(*tex));
	tex->id = ++sg->textureId;
	return tex;
}

static SGtexture* sg__findTexture(SGNVGcontext* sg, int id)
{
	int i;
	for (i = 0; i < sg->ntextures; i++)
		if (sg->textures[i].id == id)
			return &sg->textures[i];
	return NULL;
}

static int sg__deleteTexture(SGNVGcontext* sg, int id)
{
	int i;
	for (i = 0; i < sg->ntextures; i++) {
		if (sg->textures[i].id == id) {
			SGtexture* t = &sg->textures[i];
			if (t->tex != NULL && (t->flags & NVG_IMAGE_NODELETE) == 0)
				SDL_ReleaseGPUTexture(sg->dev, t->tex);
			if (t->sampler != NULL)
				SDL_ReleaseGPUSampler(sg->dev, t->sampler);
			memset(t, 0, sizeof(*t));
			return 1;
		}
	}
	return 0;
}

static SDL_GPUSampler* sg__createSampler(SGNVGcontext* sg, int imageFlags)
{
	int nearest = (imageFlags & NVG_IMAGE_NEAREST) != 0;
	SDL_GPUSamplerCreateInfo ci;
	memset(&ci, 0, sizeof(ci));
	ci.min_filter = nearest ? SDL_GPU_FILTER_NEAREST : SDL_GPU_FILTER_LINEAR;
	ci.mag_filter = nearest ? SDL_GPU_FILTER_NEAREST : SDL_GPU_FILTER_LINEAR;
	ci.mipmap_mode = nearest ? SDL_GPU_SAMPLERMIPMAPMODE_NEAREST : SDL_GPU_SAMPLERMIPMAPMODE_LINEAR;
	ci.address_mode_u = (imageFlags & NVG_IMAGE_REPEATX) ? SDL_GPU_SAMPLERADDRESSMODE_REPEAT : SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
	ci.address_mode_v = (imageFlags & NVG_IMAGE_REPEATY) ? SDL_GPU_SAMPLERADDRESSMODE_REPEAT : SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
	ci.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
	return SDL_CreateGPUSampler(sg->dev, &ci);
}

static Uint32 sg__mipLevels(int w, int h)
{
	Uint32 n = 1;
	while (w > 1 || h > 1) { w >>= 1; h >>= 1; n++; }
	return n;
}

// Uploads a (possibly sub-)region of `data` into `tex`. `data` points to a
// tightly-packed buffer of the *full* texture (fullW = full texture width),
// matching the semantics of NanoVG's renderUpdateTexture() (the font atlas).
static int sg__uploadTextureData(SGNVGcontext* sg, SDL_GPUTexture* tex,
								 int x, int y, int w, int h, int fullW,
								 const unsigned char* data, int bpp)
{
	size_t rowBytes = (size_t)w * bpp;
	size_t total = rowBytes * (size_t)h;
	const unsigned char* srcBase = data + ((size_t)y * fullW + (size_t)x) * bpp;
	SDL_GPUTransferBuffer* tb;
	unsigned char* mapped;
	SDL_GPUCommandBuffer* cmd;
	SDL_GPUCopyPass* cpass;
	int row;

	if (total == 0) return 1;

	SDL_GPUTransferBufferCreateInfo tbInfo
	{
		.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD, .size = (Uint32)total,
	};
	tb = SDL_CreateGPUTransferBuffer(sg->dev, &tbInfo);
	if (tb == NULL) return 0;

	mapped = (unsigned char*)SDL_MapGPUTransferBuffer(sg->dev, tb, false);
	if (mapped == NULL) { SDL_ReleaseGPUTransferBuffer(sg->dev, tb); return 0; }
	for (row = 0; row < h; row++) {
		memcpy(mapped + row * rowBytes, srcBase + (size_t)row * fullW * bpp, rowBytes);
	}
	SDL_UnmapGPUTransferBuffer(sg->dev, tb);

	cmd = SDL_AcquireGPUCommandBuffer(sg->dev);
	if (cmd == NULL) { SDL_ReleaseGPUTransferBuffer(sg->dev, tb); return 0; }

	cpass = SDL_BeginGPUCopyPass(cmd);
	SDL_GPUTextureTransferInfo srcRange
	{
		.transfer_buffer = tb,
		.offset = 0,
		.pixels_per_row = (Uint32)w,
		.rows_per_layer = (Uint32)h
	};
	SDL_GPUTextureRegion dstRange
	{
		.texture = tex, .mip_level = 0, .layer = 0,
		.x = (Uint32)x, .y = (Uint32)y, .z = 0,
		.w = (Uint32)w, .h = (Uint32)h, .d = 1
	};
	SDL_UploadToGPUTexture(cpass, &srcRange, &dstRange, false);
	SDL_EndGPUCopyPass(cpass);
	SDL_SubmitGPUCommandBuffer(cmd);
	SDL_ReleaseGPUTransferBuffer(sg->dev, tb);
	return 1;
}

// ---------------------------------------------------------------------------
// Per-frame CPU buffers
// ---------------------------------------------------------------------------

static int sg__allocCall(SGNVGcontext* sg)
{
	SGcall* calls;
	int ccalls;
	if (sg->ncalls + 1 > sg->ccalls) {
		ccalls = sg__maxi(sg->ncalls + 1, 128) + sg->ccalls / 2;
		calls = (SGcall*)realloc(sg->calls, sizeof(SGcall) * ccalls);
		if (calls == NULL) return -1;
		sg->calls = calls; sg->ccalls = ccalls;
	}
	memset(&sg->calls[sg->ncalls], 0, sizeof(SGcall));
	return sg->ncalls++;
}

static int sg__allocPaths(SGNVGcontext* sg, int n)
{
	SGpath* paths;
	int cpaths;
	if (sg->npaths + n > sg->cpaths) {
		cpaths = sg__maxi(sg->npaths + n, 128) + sg->cpaths / 2;
		paths = (SGpath*)realloc(sg->paths, sizeof(SGpath) * cpaths);
		if (paths == NULL) return -1;
		sg->paths = paths; sg->cpaths = cpaths;
	}
	{
		int ret = sg->npaths;
		sg->npaths += n;
		return ret;
	}
}

static int sg__allocVerts(SGNVGcontext* sg, int n)
{
	NVGvertex* verts;
	int cverts;
	if (sg->nverts + n > sg->cverts) {
		cverts = sg__maxi(sg->nverts + n, 4096) + sg->cverts / 2;
		verts = (NVGvertex*)realloc(sg->verts, sizeof(NVGvertex) * cverts);
		if (verts == NULL) return -1;
		sg->verts = verts; sg->cverts = cverts;
	}
	{
		int ret = sg->nverts;
		sg->nverts += n;
		return ret;
	}
}

static int sg__allocFragUniforms(SGNVGcontext* sg, int n)
{
	unsigned char* uniforms;
	int cuniforms;
	if (sg->nuniforms + n > sg->cuniforms) {
		cuniforms = sg__maxi(sg->nuniforms + n, 128) + sg->cuniforms / 2;
		uniforms = (unsigned char*)realloc(sg->uniforms, (size_t)sg->fragSize * cuniforms);
		if (uniforms == NULL) return -1;
		sg->uniforms = uniforms; sg->cuniforms = cuniforms;
	}
	{
		int ret = sg->nuniforms;
		sg->nuniforms += n;
		return ret;
	}
}

static SGfragUniforms* sg__fragUniformPtr(SGNVGcontext* sg, int i)
{
	return (SGfragUniforms*)(sg->uniforms + (size_t)i * sg->fragSize);
}

static int sg__maxVertCount(const NVGpath* paths, int npaths)
{
	int i, count = 0;
	for (i = 0; i < npaths; i++) {
		if (paths[i].nfill > 2) count += (paths[i].nfill - 2) * 3;  // triangulated fan
		count += paths[i].nstroke;
	}
	return count;
}

// ---------------------------------------------------------------------------
// Paint conversion
// ---------------------------------------------------------------------------

static int sg__convertPaint(SGNVGcontext* sg, SGfragUniforms* frag, NVGpaint* paint,
							NVGscissor* scissor, float width, float fringe, float strokeThr)
{
	SGtexture* tex = NULL;
	float invxform[6];

	memset(frag, 0, sizeof(*frag));
	sg__premulColor(frag->innerCol, paint->innerColor);
	sg__premulColor(frag->outerCol, paint->outerColor);

	if (scissor->extent[0] < -0.5f || scissor->extent[1] < -0.5f) {
		memset(frag->scissorMat, 0, sizeof(frag->scissorMat));
		frag->scissorExt[0] = 1.0f; frag->scissorExt[1] = 1.0f;
		frag->scissorScale[0] = 1.0f; frag->scissorScale[1] = 1.0f;
	} else {
		nvgTransformInverse(invxform, scissor->xform);
		sg__xformToMat3x4(frag->scissorMat, invxform);
		frag->scissorExt[0] = scissor->extent[0];
		frag->scissorExt[1] = scissor->extent[1];
		frag->scissorScale[0] = sqrtf(scissor->xform[0]*scissor->xform[0] + scissor->xform[2]*scissor->xform[2]) / fringe;
		frag->scissorScale[1] = sqrtf(scissor->xform[1]*scissor->xform[1] + scissor->xform[3]*scissor->xform[3]) / fringe;
	}

	memcpy(frag->extent, paint->extent, sizeof(frag->extent));
	frag->strokeMult = (width*0.5f + fringe*0.5f) / fringe;
	frag->strokeThr = strokeThr;

	if (paint->image != 0) {
		tex = sg__findTexture(sg, paint->image);
		if (tex == NULL) return 0;
		if ((tex->flags & NVG_IMAGE_FLIPY) != 0) {
			float m1[6], m2[6];
			nvgTransformTranslate(m1, 0.0f, frag->extent[1] * 0.5f);
			nvgTransformMultiply(m1, paint->xform);
			nvgTransformScale(m2, 1.0f, -1.0f);
			nvgTransformMultiply(m2, m1);
			nvgTransformTranslate(m1, 0.0f, -frag->extent[1] * 0.5f);
			nvgTransformMultiply(m1, m2);
			nvgTransformInverse(invxform, m1);
		} else {
			nvgTransformInverse(invxform, paint->xform);
		}
		frag->type = 1.0f;
		if (tex->type == NVG_TEXTURE_RGBA)
			frag->texType = (tex->flags & NVG_IMAGE_PREMULTIPLIED) ? 0.0f : 1.0f;
		else
			frag->texType = 2.0f;
	} else {
		frag->type = 0.0f;
		frag->radius = paint->radius;
		frag->feather = paint->feather;
		nvgTransformInverse(invxform, paint->xform);
	}

	sg__xformToMat3x4(frag->paintMat, invxform);
	return 1;
}

// ---------------------------------------------------------------------------
// Pipeline creation / cache
// ---------------------------------------------------------------------------

static SDL_GPUGraphicsPipeline* sg__createPipeline(SGNVGcontext* sg, const SGpipelineKey* key)
{
	SDL_GPUVertexBufferDescription vbd;
	SDL_GPUVertexAttribute attrs[2];
	SDL_GPUColorTargetDescription colorDesc;
	SDL_GPUDepthStencilState dsState;
	SDL_GPUGraphicsPipelineCreateInfo ci;

	memset(&dsState, 0, sizeof(dsState));
	SDL_GPUGraphicsPipelineTargetInfo targetInfo;
	memset(&colorDesc, 0, sizeof(colorDesc));
	memset(&targetInfo, 0, sizeof(targetInfo));
	SDL_GPUStencilOpState keep = { SDL_GPU_STENCILOP_KEEP, SDL_GPU_STENCILOP_KEEP, SDL_GPU_STENCILOP_KEEP, SDL_GPU_COMPAREOP_ALWAYS };
	SDL_GPUPrimitiveType prim = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
	SDL_GPUCullMode cull = SDL_GPU_CULLMODE_BACK;
	Uint8 stencilWriteMask = 0;
	int stencilTest = 0;
	int colorMaskOff = 0;

	switch (key->kind) {
		case SG_PIPE_FILL_STENCIL:
			prim = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
			cull = SDL_GPU_CULLMODE_NONE;
			stencilTest = 1; stencilWriteMask = 0xff; colorMaskOff = 1;
			dsState.front_stencil_state = {
				SDL_GPU_STENCILOP_KEEP, SDL_GPU_STENCILOP_INCREMENT_AND_WRAP,
				SDL_GPU_STENCILOP_KEEP, SDL_GPU_COMPAREOP_ALWAYS };
			dsState.back_stencil_state = {
				SDL_GPU_STENCILOP_KEEP, SDL_GPU_STENCILOP_DECREMENT_AND_WRAP,
				SDL_GPU_STENCILOP_KEEP, SDL_GPU_COMPAREOP_ALWAYS };
			break;
		case SG_PIPE_FILL_FRINGE:
			prim = SDL_GPU_PRIMITIVETYPE_TRIANGLESTRIP;
			stencilTest = 1;
			dsState.front_stencil_state = keep;
			dsState.back_stencil_state = keep;
			dsState.front_stencil_state.compare_op = SDL_GPU_COMPAREOP_EQUAL;
			dsState.back_stencil_state.compare_op = SDL_GPU_COMPAREOP_EQUAL;
			break;
		case SG_PIPE_FILL_FINAL:
			prim = SDL_GPU_PRIMITIVETYPE_TRIANGLESTRIP;
			stencilTest = 1; stencilWriteMask = 0xff;
			dsState.front_stencil_state = {
				SDL_GPU_STENCILOP_ZERO, SDL_GPU_STENCILOP_ZERO,
				SDL_GPU_STENCILOP_ZERO, SDL_GPU_COMPAREOP_NOT_EQUAL };
			dsState.back_stencil_state = dsState.front_stencil_state;
			break;
		case SG_PIPE_CONVEX:
			prim = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
			break;
		case SG_PIPE_STRIP:
			prim = SDL_GPU_PRIMITIVETYPE_TRIANGLESTRIP;
			break;
		case SG_PIPE_STROKE_STENCIL_FILL:
			prim = SDL_GPU_PRIMITIVETYPE_TRIANGLESTRIP;
			stencilTest = 1; stencilWriteMask = 0xff;
			dsState.front_stencil_state = {
				SDL_GPU_STENCILOP_KEEP, SDL_GPU_STENCILOP_INCREMENT_AND_CLAMP,
				SDL_GPU_STENCILOP_KEEP, SDL_GPU_COMPAREOP_EQUAL };
			dsState.back_stencil_state = dsState.front_stencil_state;
			break;
		case SG_PIPE_STROKE_STENCIL_CLEAR:
			prim = SDL_GPU_PRIMITIVETYPE_TRIANGLESTRIP;
			stencilTest = 1; stencilWriteMask = 0xff; colorMaskOff = 1;
			dsState.front_stencil_state = {
				SDL_GPU_STENCILOP_ZERO, SDL_GPU_STENCILOP_ZERO,
				SDL_GPU_STENCILOP_ZERO, SDL_GPU_COMPAREOP_ALWAYS };
			dsState.back_stencil_state = dsState.front_stencil_state;
			break;
		default:
			return NULL;
	}

	vbd.slot = 0;
	vbd.pitch = sizeof(NVGvertex);
	vbd.input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;
	vbd.instance_step_rate = 0;

	attrs[0].location = 0;
	attrs[0].buffer_slot = 0;
	attrs[0].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2;
	attrs[0].offset = offsetof(NVGvertex, x);

	attrs[1].location = 1;
	attrs[1].buffer_slot = 0;
	attrs[1].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2;
	attrs[1].offset = offsetof(NVGvertex, u);

	colorDesc.format = key->colorFormat;
	colorDesc.blend_state.src_color_blendfactor = key->blend.srcRGB;
	colorDesc.blend_state.dst_color_blendfactor = key->blend.dstRGB;
	colorDesc.blend_state.color_blend_op = SDL_GPU_BLENDOP_ADD;
	colorDesc.blend_state.src_alpha_blendfactor = key->blend.srcAlpha;
	colorDesc.blend_state.dst_alpha_blendfactor = key->blend.dstAlpha;
	colorDesc.blend_state.alpha_blend_op = SDL_GPU_BLENDOP_ADD;
	colorDesc.blend_state.color_write_mask = 0;
	colorDesc.blend_state.enable_blend = 1;
	colorDesc.blend_state.enable_color_write_mask = colorMaskOff;

	dsState.compare_op = SDL_GPU_COMPAREOP_ALWAYS;
	dsState.compare_mask = 0xff;
	dsState.write_mask = stencilWriteMask;
	dsState.enable_depth_test = 0;
	dsState.enable_depth_write = 0;
	dsState.enable_stencil_test = stencilTest;

	targetInfo.color_target_descriptions = &colorDesc;
	targetInfo.num_color_targets = 1;
	targetInfo.depth_stencil_format = sg->dsFormat;
	targetInfo.has_depth_stencil_target = sg->hasStencil;

	memset(&ci, 0, sizeof(ci));
	ci.vertex_shader = sg->vs;
	ci.fragment_shader = sg->fs;
	ci.vertex_input_state.vertex_buffer_descriptions = &vbd;
	ci.vertex_input_state.num_vertex_buffers = 1;
	ci.vertex_input_state.vertex_attributes = attrs;
	ci.vertex_input_state.num_vertex_attributes = 2;
	ci.primitive_type = prim;
	ci.rasterizer_state.fill_mode = SDL_GPU_FILLMODE_FILL;
	ci.rasterizer_state.cull_mode = cull;
	ci.rasterizer_state.front_face = SDL_GPU_FRONTFACE_COUNTER_CLOCKWISE;
	ci.rasterizer_state.enable_depth_clip = 1;
	ci.multisample_state.sample_count = SDL_GPU_SAMPLECOUNT_1;
	ci.depth_stencil_state = dsState;
	ci.target_info = targetInfo;

	return SDL_CreateGPUGraphicsPipeline(sg->dev, &ci);
}

static SDL_GPUGraphicsPipeline* sg__getPipeline(SGNVGcontext* sg, int kind, SGblend blend)
{
	SGpipelineKey key;
	SDL_GPUGraphicsPipeline* pipeline;
	SGpipelineEntry* entries;
	int i;

	key.kind = kind;
	key.colorFormat = sg->colorFormat;
	key.blend = blend;

	for (i = 0; i < sg->npipeCache; i++) {
		SGpipelineEntry* e = &sg->pipeCache[i];
		if (e->key.kind == key.kind &&
			e->key.colorFormat == key.colorFormat &&
			e->key.blend.srcRGB == blend.srcRGB &&
			e->key.blend.dstRGB == blend.dstRGB &&
			e->key.blend.srcAlpha == blend.srcAlpha &&
			e->key.blend.dstAlpha == blend.dstAlpha)
			return e->pipeline;
	}

	pipeline = sg__createPipeline(sg, &key);
	if (pipeline == NULL) return NULL;

	if (sg->npipeCache + 1 > sg->cpipeCache) {
		int c = sg__maxi(sg->npipeCache + 1, 8) + sg->cpipeCache / 2;
		entries = (SGpipelineEntry*)realloc(sg->pipeCache, sizeof(SGpipelineEntry) * c);
		if (entries == NULL) { SDL_ReleaseGPUGraphicsPipeline(sg->dev, pipeline); return NULL; }
		sg->pipeCache = entries; sg->cpipeCache = c;
	}
	sg->pipeCache[sg->npipeCache].key = key;
	sg->pipeCache[sg->npipeCache].pipeline = pipeline;
	sg->npipeCache++;
	return pipeline;
}

// ---------------------------------------------------------------------------
// Vertex buffer management
// ---------------------------------------------------------------------------

static int sg__ensureVertexBuffers(SGNVGcontext* sg, int nverts)
{
	Uint32 need = (Uint32)nverts * sizeof(NVGvertex);
	Uint32 size;
	if (need == 0) return 1;
	if (sg->vertBufSize >= need) return 1;

	size = need * 3 / 2 + sizeof(NVGvertex);
	if (sg->vertBuf != NULL) SDL_ReleaseGPUBuffer(sg->dev, sg->vertBuf);
	if (sg->uploadBuf != NULL) SDL_ReleaseGPUTransferBuffer(sg->dev, sg->uploadBuf);

	SDL_GPUBufferCreateInfo info
	{
		.usage = SDL_GPU_BUFFERUSAGE_VERTEX,
		.size = size,
	};
	sg->vertBuf = SDL_CreateGPUBuffer(sg->dev, &info);

	SDL_GPUTransferBufferCreateInfo tbInfo
	{
		.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
		.size = size,
	};
	sg->uploadBuf = SDL_CreateGPUTransferBuffer(sg->dev, &tbInfo);
	if (sg->vertBuf == NULL || sg->uploadBuf == NULL) return 0;

	sg->vertBufSize = size;
	return 1;
}

// ---------------------------------------------------------------------------
// Draw state
// ---------------------------------------------------------------------------

static void sg__setUniformsAndTexture(SGNVGcontext* sg, SDL_GPUCommandBuffer* cmd,
									  SDL_GPURenderPass* rp, int image, int uniformOffset)
{
	SGfragUniforms* frag = sg__fragUniformPtr(sg, uniformOffset);
	SGtexture* tex = sg__findTexture(sg, image);
	SDL_GPUTextureSamplerBinding binding;

	SDL_PushGPUFragmentUniformData(cmd, 0, frag, sizeof(SGfragUniforms));

	if (tex == NULL) {
		binding.texture = sg->dummyTex;
		binding.sampler = sg->dummySampler;
	} else {
		binding.texture = tex->tex;
		binding.sampler = tex->sampler;
	}
	SDL_BindGPUFragmentSamplers(rp, 0, &binding, 1);
}

static void sg__bindPipeline(SGNVGcontext* sg, SDL_GPURenderPass* rp, int kind, SGblend blend)
{
	SDL_GPUGraphicsPipeline* pipe = sg__getPipeline(sg, kind, blend);
	if (pipe != NULL)
		SDL_BindGPUGraphicsPipeline(rp, pipe);
}

// ---------------------------------------------------------------------------
// NVGparams callbacks
// ---------------------------------------------------------------------------

static int sg__renderCreate(void* uptr)
{
	SGNVGcontext* sg = (SGNVGcontext*)uptr;
	SDL_GPUShaderCreateInfo sci;
	SDL_GPUTextureCreateInfo tci;
	unsigned char white = 0xff;

	sg->fragSize = sizeof(SGfragUniforms);

	// Determine a depth-stencil format that supports stencil.
	{
		static const SDL_GPUTextureFormat candidates[] = {
			SDL_GPU_TEXTUREFORMAT_D32_FLOAT_S8_UINT,
			SDL_GPU_TEXTUREFORMAT_D24_UNORM_S8_UINT,
		};
		int i;
		sg->hasStencil = 0;
		for (i = 0; i < (int)(sizeof(candidates)/sizeof(candidates[0])); i++) {
			if (SDL_GPUTextureSupportsFormat(sg->dev, candidates[i],
					SDL_GPU_TEXTURETYPE_2D, SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET)) {
				sg->dsFormat = candidates[i];
				sg->hasStencil = 1;
				break;
			}
		}
		if (!sg->hasStencil)
			printf("nanovg_sdlgpu: no depth-stencil format with stencil support; fills will not render.\n");
	}

	if (sg->flags & NVG_DEBUG)
		printf("nanovg_sdlgpu: renderCreate (shader format 0x%x)\n", (unsigned)sg->shaders.format);

	// Shaders are created from the caller-provided bundle.
	auto shaders = sg->shaders;
	if (shaders.vertexShader == NULL || shaders.fragmentShader == NULL)
		return 0;

	memset(&sci, 0, sizeof(sci));
	sci.format = shaders.format;
	sci.entrypoint = "main";
	sci.num_samplers = 0;
	sci.num_uniform_buffers = 1;
	sci.code_size = shaders.vertexShaderSize;
	sci.code = shaders.vertexShader;
	sci.stage = SDL_GPU_SHADERSTAGE_VERTEX;
	sg->vs = SDL_CreateGPUShader(sg->dev, &sci);
	if (sg->vs == NULL) return 0;

	memset(&sci, 0, sizeof(sci));
	sci.format = shaders.format;
	sci.entrypoint = "main";
	sci.num_samplers = 1;
	sci.num_uniform_buffers = 1;
	sci.code_size = shaders.fragmentShaderSize;
	sci.code = shaders.fragmentShader;
	sci.stage = SDL_GPU_SHADERSTAGE_FRAGMENT;
	sg->fs = SDL_CreateGPUShader(sg->dev, &sci);
	if (sg->fs == NULL) return 0;

	// Dummy 1x1 white texture + sampler (bound when no image is set).
	memset(&tci, 0, sizeof(tci));
	tci.type = SDL_GPU_TEXTURETYPE_2D;
	tci.format = SDL_GPU_TEXTUREFORMAT_R8_UNORM;
	tci.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER;
	tci.width = 1; tci.height = 1; tci.layer_count_or_depth = 1;
	tci.num_levels = 1; tci.sample_count = SDL_GPU_SAMPLECOUNT_1;
	sg->dummyTex = SDL_CreateGPUTexture(sg->dev, &tci);
	if (sg->dummyTex == NULL) return 0;
	sg->dummySampler = sg__createSampler(sg, 0);
	sg__uploadTextureData(sg, sg->dummyTex, 0, 0, 1, 1, 1, &white, 1);

	return 1;
}

static int sg__renderCreateTexture(void* uptr, int type, int w, int h, int imageFlags, const unsigned char* data)
{
	SGNVGcontext* sg = (SGNVGcontext*)uptr;
	SGtexture* tex = sg__allocTexture(sg);
	SDL_GPUTextureCreateInfo tci;
	SDL_GPUTextureFormat fmt;
	int mips, bpp;

	if (tex == NULL) return 0;

	tex->type = type;
	tex->flags = imageFlags;
	tex->width = w;
	tex->height = h;

	fmt = (type == NVG_TEXTURE_RGBA) ? SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM
									 : SDL_GPU_TEXTUREFORMAT_R8_UNORM;
	bpp = (type == NVG_TEXTURE_RGBA) ? 4 : 1;
	mips = (imageFlags & NVG_IMAGE_GENERATE_MIPMAPS) != 0;

	memset(&tci, 0, sizeof(tci));
	tci.type = SDL_GPU_TEXTURETYPE_2D;
	tci.format = fmt;
	tci.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER;
	tci.width = (Uint32)w;
	tci.height = (Uint32)h;
	tci.layer_count_or_depth = 1;
	tci.num_levels = mips ? sg__mipLevels(w, h) : 1;
	tci.sample_count = SDL_GPU_SAMPLECOUNT_1;

	tex->tex = SDL_CreateGPUTexture(sg->dev, &tci);
	if (tex->tex == NULL) { memset(tex, 0, sizeof(*tex)); return 0; }

	tex->sampler = sg__createSampler(sg, imageFlags);

	if (data != NULL) {
		if (!sg__uploadTextureData(sg, tex->tex, 0, 0, w, h, w, data, bpp)) {
			sg__deleteTexture(sg, tex->id);
			return 0;
		}
		if (mips) {
			SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(sg->dev);
			if (cmd != NULL) {
				SDL_GenerateMipmapsForGPUTexture(cmd, tex->tex);
				SDL_SubmitGPUCommandBuffer(cmd);
			}
		}
	}

	return tex->id;
}

static int sg__renderDeleteTexture(void* uptr, int image)
{
	SGNVGcontext* sg = (SGNVGcontext*)uptr;
	return sg__deleteTexture(sg, image);
}

static int sg__renderUpdateTexture(void* uptr, int image, int x, int y, int w, int h, const unsigned char* data)
{
	SGNVGcontext* sg = (SGNVGcontext*)uptr;
	SGtexture* tex = sg__findTexture(sg, image);
	int bpp;
	if (tex == NULL || tex->tex == NULL) return 0;
	bpp = (tex->type == NVG_TEXTURE_RGBA) ? 4 : 1;
	return sg__uploadTextureData(sg, tex->tex, x, y, w, h, tex->width, data, bpp);
}

static int sg__renderGetTextureSize(void* uptr, int image, int* w, int* h)
{
	SGNVGcontext* sg = (SGNVGcontext*)uptr;
	SGtexture* tex = sg__findTexture(sg, image);
	if (tex == NULL) return 0;
	*w = tex->width;
	*h = tex->height;
	return 1;
}

static void sg__renderViewport(void* uptr, float width, float height, float devicePixelRatio)
{
	SGNVGcontext* sg = (SGNVGcontext*)uptr;
	(void)devicePixelRatio;
	sg->view[0] = width;
	sg->view[1] = height;
}

static void sg__renderCancel(void* uptr)
{
	SGNVGcontext* sg = (SGNVGcontext*)uptr;
	sg->nverts = 0;
	sg->npaths = 0;
	sg->ncalls = 0;
	sg->nuniforms = 0;
}

static void sg__renderFlush(void* uptr)
{
	SGNVGcontext* sg = (SGNVGcontext*)uptr;
	SDL_GPUColorTargetInfo color;
	SDL_GPUDepthStencilTargetInfo ds;
	SDL_GPURenderPass* rp;
	SDL_GPUCopyPass* cpass;
	Uint32 vbytes;
	float view[4];
	int i;

	if (sg->ncalls == 0) return;

	if (sg->commandBuffer == NULL || sg->colorTarget == NULL || !sg->hasStencil) {
		sg->nverts = sg->npaths = sg->ncalls = sg->nuniforms = 0;
		return;
	}

	// Recreate the depth-stencil target if the render target resized.
	if (sg->dsTex == NULL || sg->dsW != sg->rtW || sg->dsH != sg->rtH) {
		if (sg->dsTex != NULL) SDL_ReleaseGPUTexture(sg->dev, sg->dsTex);
		SDL_GPUTextureCreateInfo info
		{
			.type = SDL_GPU_TEXTURETYPE_2D,
			.format = sg->dsFormat,
			.usage = SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET,
			.width = (Uint32)sg->rtW,
			.height = (Uint32)sg->rtH,
			.layer_count_or_depth = 1,
			.num_levels = 1,
			.sample_count = SDL_GPU_SAMPLECOUNT_1
		};
		sg->dsTex = SDL_CreateGPUTexture(sg->dev, &info);
		if (sg->dsTex == NULL) {
			sg->nverts = sg->npaths = sg->ncalls = sg->nuniforms = 0;
			return;
		}
		sg->dsW = sg->rtW;
		sg->dsH = sg->rtH;
	}

	// Upload vertex data through a transfer buffer (copy pass before render).
	if (!sg__ensureVertexBuffers(sg, sg->nverts)) {
		sg->nverts = sg->npaths = sg->ncalls = sg->nuniforms = 0;
		return;
	}
	vbytes = (Uint32)sg->nverts * sizeof(NVGvertex);
	{
		void* mapped = SDL_MapGPUTransferBuffer(sg->dev, sg->uploadBuf, true);
		if (mapped != NULL) {
			memcpy(mapped, sg->verts, vbytes);
			SDL_UnmapGPUTransferBuffer(sg->dev, sg->uploadBuf);
		}
	}
	cpass = SDL_BeginGPUCopyPass(sg->commandBuffer);
	SDL_GPUTransferBufferLocation srcRange
	{
		.transfer_buffer = sg->uploadBuf,
		.offset = 0,
	};
	SDL_GPUBufferRegion dstRange
	{
		.buffer = sg->vertBuf,
		.offset = 0,
		.size = vbytes,
	};
	SDL_UploadToGPUBuffer(cpass,
		&srcRange,
		&dstRange,
		true);
	SDL_EndGPUCopyPass(cpass);

	// Begin the render pass.
	memset(&color, 0, sizeof(color));
	color.texture = sg->colorTarget;
	color.clear_color = sg->clearColor;
	color.load_op = sg->loadOp;
	color.store_op = SDL_GPU_STOREOP_STORE;
	color.cycle = false;

	memset(&ds, 0, sizeof(ds));
	ds.texture = sg->dsTex;
	ds.clear_depth = 1.0f;
	ds.load_op = SDL_GPU_LOADOP_CLEAR;
	ds.store_op = SDL_GPU_STOREOP_DONT_CARE;
	ds.stencil_load_op = SDL_GPU_LOADOP_CLEAR;
	ds.stencil_store_op = SDL_GPU_STOREOP_DONT_CARE;
	ds.clear_stencil = 0;

	rp = SDL_BeginGPURenderPass(sg->commandBuffer, &color, 1, &ds);
	if (rp == NULL) {
		sg->nverts = sg->npaths = sg->ncalls = sg->nuniforms = 0;
		return;
	}

	SDL_GPUViewport viewport
	{
		0.0f, 0.0f,
		(float)sg->rtW, (float)sg->rtH,
		0.0f, 1.0f
	};
	SDL_SetGPUViewport(rp, &viewport);
	SDL_SetGPUStencilReference(rp, 0);
	SDL_GPUBufferBinding binding
	{
		.buffer = sg->vertBuf,
		.offset = 0
	};
	SDL_BindGPUVertexBuffers(rp, 0, &binding, 1);

	view[0] = sg->view[0]; view[1] = sg->view[1]; view[2] = 0.0f; view[3] = 0.0f;
	SDL_PushGPUVertexUniformData(sg->commandBuffer, 0, view, sizeof(view));

	for (i = 0; i < sg->ncalls; i++) {
		SGcall* call = &sg->calls[i];
		SGpath* paths = &sg->paths[call->pathOffset];
		int npaths = call->pathCount;
		int j;

		if (call->type == SG_FILL) {
			sg__bindPipeline(sg, rp, SG_PIPE_FILL_STENCIL, call->blend);
			sg__setUniformsAndTexture(sg, sg->commandBuffer, rp, 0, call->uniformOffset);
			for (j = 0; j < npaths; j++)
				if (paths[j].triCount > 0)
					SDL_DrawGPUPrimitives(rp, paths[j].triCount, 1, paths[j].triOffset, 0);

			if (sg->flags & NVG_ANTIALIAS) {
				sg__bindPipeline(sg, rp, SG_PIPE_FILL_FRINGE, call->blend);
				sg__setUniformsAndTexture(sg, sg->commandBuffer, rp, call->image, call->uniformOffset + 1);
				for (j = 0; j < npaths; j++)
					if (paths[j].strokeCount > 0)
						SDL_DrawGPUPrimitives(rp, paths[j].strokeCount, 1, paths[j].strokeOffset, 0);
			}

			sg__bindPipeline(sg, rp, SG_PIPE_FILL_FINAL, call->blend);
			sg__setUniformsAndTexture(sg, sg->commandBuffer, rp, call->image, call->uniformOffset + 1);
			SDL_DrawGPUPrimitives(rp, 4, 1, call->quadOffset, 0);

		} else if (call->type == SG_CONVEXFILL) {
			sg__bindPipeline(sg, rp, SG_PIPE_CONVEX, call->blend);
			sg__setUniformsAndTexture(sg, sg->commandBuffer, rp, call->image, call->uniformOffset);
			for (j = 0; j < npaths; j++)
				if (paths[j].triCount > 0)
					SDL_DrawGPUPrimitives(rp, paths[j].triCount, 1, paths[j].triOffset, 0);

			if (sg->flags & NVG_ANTIALIAS) {
				sg__bindPipeline(sg, rp, SG_PIPE_STRIP, call->blend);
				for (j = 0; j < npaths; j++)
					if (paths[j].strokeCount > 0)
						SDL_DrawGPUPrimitives(rp, paths[j].strokeCount, 1, paths[j].strokeOffset, 0);
			}

		} else if (call->type == SG_STROKE) {
			if (sg->flags & NVG_STENCIL_STROKES) {
				sg__bindPipeline(sg, rp, SG_PIPE_STROKE_STENCIL_FILL, call->blend);
				sg__setUniformsAndTexture(sg, sg->commandBuffer, rp, call->image, call->uniformOffset + 1);
				for (j = 0; j < npaths; j++)
					if (paths[j].strokeCount > 0)
						SDL_DrawGPUPrimitives(rp, paths[j].strokeCount, 1, paths[j].strokeOffset, 0);

				sg__bindPipeline(sg, rp, SG_PIPE_FILL_FRINGE, call->blend);
				sg__setUniformsAndTexture(sg, sg->commandBuffer, rp, call->image, call->uniformOffset);
				for (j = 0; j < npaths; j++)
					if (paths[j].strokeCount > 0)
						SDL_DrawGPUPrimitives(rp, paths[j].strokeCount, 1, paths[j].strokeOffset, 0);

				sg__bindPipeline(sg, rp, SG_PIPE_STROKE_STENCIL_CLEAR, call->blend);
				sg__setUniformsAndTexture(sg, sg->commandBuffer, rp, call->image, call->uniformOffset);
				for (j = 0; j < npaths; j++)
					if (paths[j].strokeCount > 0)
						SDL_DrawGPUPrimitives(rp, paths[j].strokeCount, 1, paths[j].strokeOffset, 0);

			} else {
				sg__bindPipeline(sg, rp, SG_PIPE_STRIP, call->blend);
				sg__setUniformsAndTexture(sg, sg->commandBuffer, rp, call->image, call->uniformOffset);
				for (j = 0; j < npaths; j++)
					if (paths[j].strokeCount > 0)
						SDL_DrawGPUPrimitives(rp, paths[j].strokeCount, 1, paths[j].strokeOffset, 0);
			}

		} else if (call->type == SG_TRIANGLES) {
			sg__bindPipeline(sg, rp, SG_PIPE_CONVEX, call->blend);
			sg__setUniformsAndTexture(sg, sg->commandBuffer, rp, call->image, call->uniformOffset);
			SDL_DrawGPUPrimitives(rp, call->triCount, 1, call->triOffset, 0);
		}
	}

	SDL_EndGPURenderPass(rp);

	sg->nverts = 0;
	sg->npaths = 0;
	sg->ncalls = 0;
	sg->nuniforms = 0;
}

static void sg__renderFill(void* uptr, NVGpaint* paint, NVGcompositeOperationState compositeOperation,
						   NVGscissor* scissor, float fringe, const float* bounds,
						   const NVGpath* paths, int npaths)
{
	SGNVGcontext* sg = (SGNVGcontext*)uptr;
	SGcall* call = NULL;
	int callIndex = sg__allocCall(sg);
	int maxverts, offset;
	int i, j;

	if (callIndex == -1) return;
	call = &sg->calls[callIndex];
	call->type = SG_FILL;
	call->pathOffset = sg__allocPaths(sg, npaths);
	if (call->pathOffset == -1) goto error;
	call->pathCount = npaths;
	call->image = paint->image;
	call->blend = sg__blendFromComposite(compositeOperation);
	call->quadOffset = -1;

	if (npaths == 1 && paths[0].convex)
		call->type = SG_CONVEXFILL;

	maxverts = sg__maxVertCount(paths, npaths) + 4;
	offset = sg__allocVerts(sg, maxverts);
	if (offset == -1) goto error;

	for (i = 0; i < npaths; i++) {
		SGpath* copy = &sg->paths[call->pathOffset + i];
		const NVGpath* path = &paths[i];
		memset(copy, 0, sizeof(*copy));
		if (path->nfill > 2) {
			copy->triOffset = offset;
			copy->triCount = (path->nfill - 2) * 3;
			for (j = 0; j < path->nfill - 2; j++) {
				sg->verts[offset++] = path->fill[0];
				sg->verts[offset++] = path->fill[j + 1];
				sg->verts[offset++] = path->fill[j + 2];
			}
		}
		if (path->nstroke > 0) {
			copy->strokeOffset = offset;
			copy->strokeCount = path->nstroke;
			memcpy(&sg->verts[offset], path->stroke, sizeof(NVGvertex) * path->nstroke);
			offset += path->nstroke;
		}
	}

	if (call->type == SG_FILL) {
		NVGvertex* quad;
		SGfragUniforms* frag;
		call->quadOffset = offset;
		quad = &sg->verts[call->quadOffset];
		sg__vset(&quad[0], bounds[2], bounds[3], 0.5f, 1.0f);
		sg__vset(&quad[1], bounds[2], bounds[1], 0.5f, 1.0f);
		sg__vset(&quad[2], bounds[0], bounds[3], 0.5f, 1.0f);
		sg__vset(&quad[3], bounds[0], bounds[1], 0.5f, 1.0f);

		call->uniformOffset = sg__allocFragUniforms(sg, 2);
		if (call->uniformOffset == -1) goto error;
		call->uniformCount = 2;

		frag = sg__fragUniformPtr(sg, call->uniformOffset);
		memset(frag, 0, sizeof(*frag));
		frag->strokeThr = -1.0f;
		frag->type = 2.0f;  // simple

		sg__convertPaint(sg, sg__fragUniformPtr(sg, call->uniformOffset + 1), paint, scissor, fringe, fringe, -1.0f);
	} else {
		call->uniformOffset = sg__allocFragUniforms(sg, 1);
		if (call->uniformOffset == -1) goto error;
		call->uniformCount = 1;
		sg__convertPaint(sg, sg__fragUniformPtr(sg, call->uniformOffset), paint, scissor, fringe, fringe, -1.0f);
	}

	return;

error:
	if (sg->ncalls > 0) sg->ncalls--;
}

static void sg__renderStroke(void* uptr, NVGpaint* paint, NVGcompositeOperationState compositeOperation,
							 NVGscissor* scissor, float fringe, float strokeWidth,
							 const NVGpath* paths, int npaths)
{
	SGNVGcontext* sg = (SGNVGcontext*)uptr;
	SGcall* call;
	int callIndex = sg__allocCall(sg);
	int maxverts, offset;
	int i;

	if (callIndex == -1) return;
	call = &sg->calls[callIndex];
	call->type = SG_STROKE;
	call->pathOffset = sg__allocPaths(sg, npaths);
	if (call->pathOffset == -1) goto error;
	call->pathCount = npaths;
	call->image = paint->image;
	call->blend = sg__blendFromComposite(compositeOperation);

	maxverts = sg__maxVertCount(paths, npaths);
	offset = sg__allocVerts(sg, maxverts);
	if (offset == -1) goto error;

	for (i = 0; i < npaths; i++) {
		SGpath* copy = &sg->paths[call->pathOffset + i];
		const NVGpath* path = &paths[i];
		memset(copy, 0, sizeof(*copy));
		if (path->nstroke > 0) {
			copy->strokeOffset = offset;
			copy->strokeCount = path->nstroke;
			memcpy(&sg->verts[offset], path->stroke, sizeof(NVGvertex) * path->nstroke);
			offset += path->nstroke;
		}
	}

	if (sg->flags & NVG_STENCIL_STROKES) {
		call->uniformOffset = sg__allocFragUniforms(sg, 2);
		if (call->uniformOffset == -1) goto error;
		call->uniformCount = 2;
		sg__convertPaint(sg, sg__fragUniformPtr(sg, call->uniformOffset), paint, scissor, strokeWidth, fringe, -1.0f);
		sg__convertPaint(sg, sg__fragUniformPtr(sg, call->uniformOffset + 1), paint, scissor, strokeWidth, fringe, 1.0f - 0.5f/255.0f);
	} else {
		call->uniformOffset = sg__allocFragUniforms(sg, 1);
		if (call->uniformOffset == -1) goto error;
		call->uniformCount = 1;
		sg__convertPaint(sg, sg__fragUniformPtr(sg, call->uniformOffset), paint, scissor, strokeWidth, fringe, -1.0f);
	}

	return;

error:
	if (sg->ncalls > 0) sg->ncalls--;
}

static void sg__renderTriangles(void* uptr, NVGpaint* paint, NVGcompositeOperationState compositeOperation,
								NVGscissor* scissor, const NVGvertex* verts, int nverts, float fringe)
{
	SGNVGcontext* sg = (SGNVGcontext*)uptr;
	SGcall* call;
	SGfragUniforms* frag;
	int callIndex = sg__allocCall(sg);

	if (callIndex == -1) return;
	call = &sg->calls[callIndex];
	call->type = SG_TRIANGLES;
	call->image = paint->image;
	call->blend = sg__blendFromComposite(compositeOperation);

	call->triOffset = sg__allocVerts(sg, nverts);
	if (call->triOffset == -1) goto error;
	call->triCount = nverts;
	memcpy(&sg->verts[call->triOffset], verts, sizeof(NVGvertex) * nverts);

	call->uniformOffset = sg__allocFragUniforms(sg, 1);
	if (call->uniformOffset == -1) goto error;
	call->uniformCount = 1;

	frag = sg__fragUniformPtr(sg, call->uniformOffset);
	sg__convertPaint(sg, frag, paint, scissor, 1.0f, fringe, -1.0f);
	frag->type = 3.0f;  // textured triangles

	return;

error:
	if (sg->ncalls > 0) sg->ncalls--;
}

static void sg__renderDelete(void* uptr)
{
	SGNVGcontext* sg = (SGNVGcontext*)uptr;
	int i;
	if (sg == NULL) return;

	if (sg->dev != NULL)
		SDL_WaitForGPUIdle(sg->dev);

	for (i = 0; i < sg->npipeCache; i++)
		if (sg->pipeCache[i].pipeline != NULL)
			SDL_ReleaseGPUGraphicsPipeline(sg->dev, sg->pipeCache[i].pipeline);
	free(sg->pipeCache);

	for (i = 0; i < sg->ntextures; i++) {
		SGtexture* t = &sg->textures[i];
		if (t->tex != NULL && (t->flags & NVG_IMAGE_NODELETE) == 0)
			SDL_ReleaseGPUTexture(sg->dev, t->tex);
		if (t->sampler != NULL)
			SDL_ReleaseGPUSampler(sg->dev, t->sampler);
	}
	free(sg->textures);

	if (sg->dummyTex != NULL) SDL_ReleaseGPUTexture(sg->dev, sg->dummyTex);
	if (sg->dummySampler != NULL) SDL_ReleaseGPUSampler(sg->dev, sg->dummySampler);
	if (sg->dsTex != NULL) SDL_ReleaseGPUTexture(sg->dev, sg->dsTex);
	if (sg->vertBuf != NULL) SDL_ReleaseGPUBuffer(sg->dev, sg->vertBuf);
	if (sg->uploadBuf != NULL) SDL_ReleaseGPUTransferBuffer(sg->dev, sg->uploadBuf);
	if (sg->vs != NULL) SDL_ReleaseGPUShader(sg->dev, sg->vs);
	if (sg->fs != NULL) SDL_ReleaseGPUShader(sg->dev, sg->fs);

	free(sg->calls);
	free(sg->paths);
	free(sg->verts);
	free(sg->uniforms);
	free(sg);
}

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

NVGcontext* nvgCreateSDLGPU(SDL_GPUDevice* device, int flags, const NVGsgShaderBundle shaders)
{
	NVGparams params;
	SGNVGcontext* sg;
	NVGcontext* ctx;

	if (device == NULL) return NULL;
	if (shaders.vertexShader == NULL || shaders.fragmentShader == NULL) return NULL;

	sg = (SGNVGcontext*)calloc(1, sizeof(SGNVGcontext));
	if (sg == NULL) return NULL;

	sg->dev = device;
	sg->flags = flags;
	sg->shaders = shaders;

	memset(&params, 0, sizeof(params));
	params.userPtr = sg;
	params.edgeAntiAlias = flags & NVG_ANTIALIAS ? 1 : 0;
	params.renderCreate = sg__renderCreate;
	params.renderCreateTexture = sg__renderCreateTexture;
	params.renderDeleteTexture = sg__renderDeleteTexture;
	params.renderUpdateTexture = sg__renderUpdateTexture;
	params.renderGetTextureSize = sg__renderGetTextureSize;
	params.renderViewport = sg__renderViewport;
	params.renderCancel = sg__renderCancel;
	params.renderFlush = sg__renderFlush;
	params.renderFill = sg__renderFill;
	params.renderStroke = sg__renderStroke;
	params.renderTriangles = sg__renderTriangles;
	params.renderDelete = sg__renderDelete;

	ctx = nvgCreateInternal(&params);
	if (ctx == NULL)
		return NULL;  // 'sg' is freed by nvgDeleteInternal -> renderDelete

	return ctx;
}

void nvgDeleteSDLGPU(NVGcontext* ctx)
{
	nvgDeleteInternal(ctx);
}

void nvgsgSetRenderTarget(NVGcontext* ctx, SDL_GPUCommandBuffer* commandBuffer,
						  SDL_GPUTexture* colorTarget, SDL_GPUTextureFormat colorFormat,
						  int width, int height, SDL_GPULoadOp loadOp, SDL_FColor clearColor)
{
	SGNVGcontext* sg = (SGNVGcontext*)nvgInternalParams(ctx)->userPtr;
	sg->commandBuffer = commandBuffer;
	sg->colorTarget = colorTarget;
	sg->colorFormat = colorFormat;
	sg->rtW = width;
	sg->rtH = height;
	sg->loadOp = loadOp;
	sg->clearColor = clearColor;
}

int nvgsgCreateImageFromTexture(NVGcontext* ctx, SDL_GPUTexture* texture, int w, int h, int imageFlags)
{
	SGNVGcontext* sg = (SGNVGcontext*)nvgInternalParams(ctx)->userPtr;
	SGtexture* tex = sg__allocTexture(sg);
	if (tex == NULL) return 0;

	tex->tex = texture;
	tex->sampler = sg__createSampler(sg, imageFlags);
	tex->type = NVG_TEXTURE_RGBA;
	tex->flags = imageFlags | NVG_IMAGE_NODELETE;
	tex->width = w;
	tex->height = h;
	return tex->id;
}

SDL_GPUTexture* nvgsgImageHandle(NVGcontext* ctx, int image)
{
	SGNVGcontext* sg = (SGNVGcontext*)nvgInternalParams(ctx)->userPtr;
	SGtexture* tex = sg__findTexture(sg, image);
	return tex != NULL ? tex->tex : NULL;
}

#endif /* NANOVG_SDLGPU_IMPLEMENTATION */