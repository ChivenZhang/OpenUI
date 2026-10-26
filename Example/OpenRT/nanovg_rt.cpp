//
// nanovg backend on top of the OpenRT rendering interface.
//
// Mirrors nanovg_gl.h: nanovg hands us triangle lists for fills, fringes,
// strokes and text; we batch them per frame and replay them in renderFlush.
//
// Because OpenRT bakes blend / stencil / colour-mask state into render modules
// and only honours that state when rendering offscreen, geometry is drawn into
// the NVGtarget recorded with each draw. colourTarget and stencilTarget are
// rt_texture_t* supplied by the caller through nvgBeginFrame. Flush replays each
// draw into the attachments that were current when it was recorded.
//
#include "nanovg_rt.h"
#include "Shader/Shader.h"
#include <map>
#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stddef.h>

enum RTVGshaderType
{
	NSVG_SHADER_FILLGRAD,
	NSVG_SHADER_FILLIMG,
	NSVG_SHADER_SIMPLE,
	NSVG_SHADER_IMG
};

// Binding slots; must match the shaders in nanovg_rt.h.
enum RTVGbinding
{
	RTVG_FRAG_BINDING = 0,
	RTVG_VIEW_BINDING = 1,
	RTVG_TEX_BINDING = 2,
};

// rt_bind_buffer binds a sub-range of the uniform buffer, so every block must
// start at an offset that satisfies the strictest alignment any backend asks for.
constexpr size_t RTVG_UNIFORM_ALIGN = 256;

struct RTVGtexture
{
	rt_texture_t texture;
	rt_sampler_t sampler;
	int id;
	int width, height;
	int type;
	int flags;
};

struct RTVGblend
{
	rt_blend_factor_t srcRGB;
	rt_blend_factor_t dstRGB;
	rt_blend_factor_t srcAlpha;
	rt_blend_factor_t dstAlpha;
};

struct RTVGstencil
{
	rt_compare_op_t func;
	rt_stencil_op_t sfail;
	rt_stencil_op_t zfail;
	rt_stencil_op_t zpass;
};

// Fixed-function state that is baked into a render module.
struct RTVGpipeline
{
	RTVGblend blend;
	bool colorWrite;
	rt_cull_mode_t cull;
	RTVGstencil front;
	RTVGstencil back;
	rt_texture_format_t colorFormat;
};

// Blend/stencil state plus the colour attachment format. Formats do not fit in
// the 4-bit pipeline key, so they are part of the map key instead.
struct RTVGmoduleKey
{
	uint64_t state = 0;
	uint32_t format = 0;
	bool operator<(RTVGmoduleKey const& other) const
	{
		if (state != other.state) return state < other.state;
		return format < other.format;
	}
};

enum RTVGcallType
{
	RTVG_NONE = 0,
	RTVG_FILL,
	RTVG_CONVEXFILL,
	RTVG_STROKE,
	RTVG_TRIANGLES,
};

struct RTVGcall
{
	int type;
	int image;
	int pathOffset;
	int pathCount;
	int triangleOffset;
	int triangleCount;
	int uniformOffset;
	RTVGblend blendFunc;
	NVGtarget target;
};

struct RTVGpath
{
	int fillOffset;
	int fillCount;
	int strokeOffset;
	int strokeCount;
};

// Layout matches the std140 block in Shader/Fill.slang.
struct RTVGfragUniforms
{
	float scissorMat[12]; // matrices are actually 3 vec4s
	float paintMat[12];
	struct NVGcolor innerCol;
	struct NVGcolor outerCol;
	float scissorExt[2];
	float scissorScale[2];
	float extent[2];
	float radius;
	float feather;
	float strokeMult;
	float strokeThr;
	int texType;
	int type;
};

// Layout matches the std140 view block in Shader/Fill.slang.
struct RTVGviewUniforms
{
	float viewSize[2];
	float pad[2];
};

struct RTVGcontext
{
	NVGcontext* ctx = nullptr;
	int flags = 0;
	float view[2] = {};

	// Colour format and view-uniform offset of the pass currently being replayed.
	rt_texture_format_t passFormat = RT_TEXTURE_RGBA8UNORM;
	size_t viewOffset = 0;

	// GPU buffers
	rt_buffer_t vertBuf;
	rt_buffer_t fragBuf;
	rt_buffer_t viewBuf;

	// Render modules keyed by fixed-function state.
	std::map<RTVGmoduleKey, rt_module_render_t> modules;

	std::map<int, RTVGtexture> textures;
	int dummyTex = 0;

	// Per frame buffers
	size_t fragSize = 0;
	int ncalls = 0;
	int npaths = 0;
	int nverts = 0;
	int nuniforms = 0;
	std::vector<RTVGcall> calls;
	std::vector<RTVGpath> paths;
	std::vector<NVGvertex> verts;
	std::vector<uint8_t> uniforms;
};

// ====================================================================
// Textures

static RTVGtexture* nvg_findTexture(RTVGcontext* rt, int id)
{
	auto result = rt->textures.find(id);
	if (result != rt->textures.end()) return &result->second;
	return nullptr;
}

static int rt_renderCreateTexture(void* uptr, int type, int w, int h, int imageFlags, const unsigned char* data)
{
	auto rt = (RTVGcontext*)uptr;

	rt_filter_t minFilter, magFilter;
	if (imageFlags & NVG_IMAGE_GENERATE_MIPMAPS)
		minFilter = (imageFlags & NVG_IMAGE_NEAREST) ? RT_NEAREST_MIPMAP_NEAREST : RT_LINEAR_MIPMAP_LINEAR;
	else
		minFilter = (imageFlags & NVG_IMAGE_NEAREST) ? RT_NEAREST : RT_LINEAR;
	magFilter = (imageFlags & NVG_IMAGE_NEAREST) ? RT_NEAREST : RT_LINEAR;
	rt_address_t addressU = (imageFlags & NVG_IMAGE_REPEATX) ? RT_REPEAT : RT_CLAMP_TO_EDGE;
	rt_address_t addressV = (imageFlags & NVG_IMAGE_REPEATY) ? RT_REPEAT : RT_CLAMP_TO_EDGE;

	rt_texture_info_t info = {};
	info.width = (uint32_t)w;
	info.height = (uint32_t)h;
	info.target = RT_TEXTURE_2D;
	info.format = (type == NVG_TEXTURE_RGBA) ? RT_TEXTURE_RGBA8UNORM : RT_TEXTURE_R8UNORM;
	info.usage = RT_TEXTURE_USAGE_COPY_SRC | RT_TEXTURE_USAGE_COPY_DST | RT_TEXTURE_USAGE_TEXTURE_BINDING;
	info.min_filter = minFilter;
	info.mag_filter = magFilter;
	info.address_u = addressU;
	info.address_v = addressV;
	info.address_w = RT_CLAMP_TO_EDGE;
	info.mipmaps = (imageFlags & NVG_IMAGE_GENERATE_MIPMAPS) ? 0 : 1;
	info.data = data;
	auto texture = rt_create_texture(info);
	if (texture.handle == 0) return 0;

	// Texture views do not carry sampler state, so bind an explicit sampler alongside.
	rt_sampler_info_t sampler = {};
	sampler.min_filter = minFilter;
	sampler.mag_filter = magFilter;
	sampler.address_u = addressU;
	sampler.address_v = addressV;
	sampler.address_w = RT_CLAMP_TO_EDGE;

	auto& tex = rt->textures[(int)texture.handle];
	tex.texture = texture;
	tex.sampler = rt_create_sampler(sampler);
	tex.id = (int)texture.handle;
	tex.width = w;
	tex.height = h;
	tex.type = type;
	tex.flags = imageFlags;
	return tex.id;
}

static int rt_renderDeleteTexture(void* uptr, int image)
{
	auto rt = (RTVGcontext*)uptr;

	if (auto tex = nvg_findTexture(rt, image))
	{
		rt_destroy_sampler(tex->sampler);
		if ((tex->flags & RTVG_IMAGE_NODELETE) == 0) rt_destroy_texture(tex->texture);
		rt->textures.erase(image);
		return 1;
	}
	return 0;
}

static int rt_renderUpdateTexture(void* uptr, int image, int x, int y, int w, int h, const unsigned char* data)
{
	auto rt = (RTVGcontext*)uptr;
	RTVGtexture* tex = nvg_findTexture(rt, image);
	if (tex == nullptr) return 0;

	// nanovg passes a pointer to the whole image and a dirty rectangle inside it.
	uint32_t bpp = (tex->type == NVG_TEXTURE_RGBA) ? 4 : 1;
	rt_texture_data_t source = {};
	source.data = data;
	source.size = (size_t)tex->width * tex->height * bpp;
	source.offset = ((size_t)y * tex->width + x) * bpp;
	source.bytesPerRow = (uint32_t)tex->width * bpp;
	source.rowsPerImage = (uint32_t)h;

	rt_pass_transfer_t pass;
	rt_begin_transfer(pass);
	rt_copy_texture_data(source,
		{ .texture = tex->texture, .mipLevel = 0, .origin = { (uint32_t)x, (uint32_t)y, 0 }, },
		{ (uint32_t)w, (uint32_t)h, 1U });
	rt_end_transfer(pass);
	return 1;
}

static int rt_renderGetTextureSize(void* uptr, int image, int* w, int* h)
{
	auto rt = (RTVGcontext*)uptr;
	RTVGtexture* tex = nvg_findTexture(rt, image);
	if (tex == nullptr) return 0;
	*w = tex->width;
	*h = tex->height;
	return 1;
}

// ====================================================================
// Paint conversion

static void nvg_xformToMat3x4(float* m3, float* t)
{
	m3[0] = t[0];
	m3[1] = t[1];
	m3[2] = 0.0f;
	m3[3] = 0.0f;
	m3[4] = t[2];
	m3[5] = t[3];
	m3[6] = 0.0f;
	m3[7] = 0.0f;
	m3[8] = t[4];
	m3[9] = t[5];
	m3[10] = 1.0f;
	m3[11] = 0.0f;
}

static NVGcolor nvg_premulColor(NVGcolor c)
{
	c.r *= c.a;
	c.g *= c.a;
	c.b *= c.a;
	return c;
}

static int nvg_convertPaint(RTVGcontext* rt, RTVGfragUniforms* frag, NVGpaint* paint, NVGscissor* scissor, float width, float fringe, float strokeThr)
{
	RTVGtexture* tex = nullptr;
	float invxform[6];

	memset(frag, 0, sizeof(*frag));

	frag->innerCol = nvg_premulColor(paint->innerColor);
	frag->outerCol = nvg_premulColor(paint->outerColor);

	if (scissor->extent[0] < -0.5f || scissor->extent[1] < -0.5f)
	{
		memset(frag->scissorMat, 0, sizeof(frag->scissorMat));
		frag->scissorExt[0] = 1.0f;
		frag->scissorExt[1] = 1.0f;
		frag->scissorScale[0] = 1.0f;
		frag->scissorScale[1] = 1.0f;
	}
	else
	{
		nvgTransformInverse(invxform, scissor->xform);
		nvg_xformToMat3x4(frag->scissorMat, invxform);
		frag->scissorExt[0] = scissor->extent[0];
		frag->scissorExt[1] = scissor->extent[1];
		frag->scissorScale[0] = sqrtf(scissor->xform[0] * scissor->xform[0] + scissor->xform[2] * scissor->xform[2]) / fringe;
		frag->scissorScale[1] = sqrtf(scissor->xform[1] * scissor->xform[1] + scissor->xform[3] * scissor->xform[3]) / fringe;
	}

	memcpy(frag->extent, paint->extent, sizeof(frag->extent));
	frag->strokeMult = (width * 0.5f + fringe * 0.5f) / fringe;
	frag->strokeThr = strokeThr;

	if (paint->image != 0)
	{
		tex = nvg_findTexture(rt, paint->image);
		if (tex == nullptr) return 0;
		if ((tex->flags & NVG_IMAGE_FLIPY) != 0)
		{
			float m1[6], m2[6];
			nvgTransformTranslate(m1, 0.0f, frag->extent[1] * 0.5f);
			nvgTransformMultiply(m1, paint->xform);
			nvgTransformScale(m2, 1.0f, -1.0f);
			nvgTransformMultiply(m2, m1);
			nvgTransformTranslate(m1, 0.0f, -frag->extent[1] * 0.5f);
			nvgTransformMultiply(m1, m2);
			nvgTransformInverse(invxform, m1);
		}
		else
		{
			nvgTransformInverse(invxform, paint->xform);
		}
		frag->type = NSVG_SHADER_FILLIMG;

		if (tex->type == NVG_TEXTURE_RGBA)
			frag->texType = (tex->flags & NVG_IMAGE_PREMULTIPLIED) ? 0 : 1;
		else
			frag->texType = 2;
	}
	else
	{
		frag->type = NSVG_SHADER_FILLGRAD;
		frag->radius = paint->radius;
		frag->feather = paint->feather;
		nvgTransformInverse(invxform, paint->xform);
	}

	nvg_xformToMat3x4(frag->paintMat, invxform);

	return 1;
}

// ====================================================================
// Render modules

static uint64_t nvg_pipelineKey(const RTVGpipeline& p)
{
	// 14 fields, each fits in 4 bits.
	uint64_t key = 0;
	auto push = [&key](uint32_t v) { key = (key << 4) | (v & 0xF); };
	push(p.blend.srcRGB);
	push(p.blend.dstRGB);
	push(p.blend.srcAlpha);
	push(p.blend.dstAlpha);
	push(p.colorWrite ? 1 : 0);
	push(p.cull);
	push(p.front.func);
	push(p.front.sfail);
	push(p.front.zfail);
	push(p.front.zpass);
	push(p.back.func);
	push(p.back.sfail);
	push(p.back.zfail);
	push(p.back.zpass);
	return key;
}

static void nvg_setVertexLayout(rt_module_render_info_t& info)
{
	info.vertex[0].stride = sizeof(NVGvertex);
	info.vertex[0].instance = false;
	info.vertex[0].attrib[0] = { .location = 0, .offset = (uint32_t)offsetof(NVGvertex, x), .format = RT_VERTEX_FLOAT32X2 };
	info.vertex[0].attrib[1] = { .location = 1, .offset = (uint32_t)offsetof(NVGvertex, u), .format = RT_VERTEX_FLOAT32X2 };
}

static rt_module_render_t& nvg_getModule(RTVGcontext* rt, const RTVGpipeline& p)
{
	rt_texture_format_t format = p.colorFormat != RT_TEXTURE_NONE ? p.colorFormat : RT_TEXTURE_RGBA8UNORM;
	RTVGmoduleKey key = { nvg_pipelineKey(p), (uint32_t)format };
	auto found = rt->modules.find(key);
	if (found != rt->modules.end()) return found->second;

	rt_module_render_info_t info = {};
	auto backend = rt_get_backend();
	auto vs = shader::fill_vs(backend);
	auto fs = shader::fill_fs(backend);
	info.vshader = { vs.code, vs.size, vs.entry };
	info.fshader = { fs.code, fs.size, fs.entry };

	info.colors[0].format = format;
	info.colors[0].write.r = p.colorWrite;
	info.colors[0].write.g = p.colorWrite;
	info.colors[0].write.b = p.colorWrite;
	info.colors[0].write.a = p.colorWrite;
	info.colors[0].color.func = RT_FUNC_ADD;
	info.colors[0].color.src = p.blend.srcRGB;
	info.colors[0].color.dst = p.blend.dstRGB;
	info.colors[0].alpha.func = RT_FUNC_ADD;
	info.colors[0].alpha.src = p.blend.srcAlpha;
	info.colors[0].alpha.dst = p.blend.dstAlpha;

	info.depth.write = false;
	info.depth.func = RT_ALWAYS;

	info.stencil.read = 0xFF;
	info.stencil.write = 0xFF;
	info.stencil.front.func = p.front.func;
	info.stencil.front.sfail = p.front.sfail;
	info.stencil.front.zfail = p.front.zfail;
	info.stencil.front.zpass = p.front.zpass;
	info.stencil.back.func = p.back.func;
	info.stencil.back.sfail = p.back.sfail;
	info.stencil.back.zfail = p.back.zfail;
	info.stencil.back.zpass = p.back.zpass;

	nvg_setVertexLayout(info);
	info.binding[0] = { .binding = RTVG_FRAG_BINDING, .type = RT_BINDING_UNIFORM_BUFFER };
	info.binding[1] = { .binding = RTVG_VIEW_BINDING, .type = RT_BINDING_UNIFORM_BUFFER };
	info.binding[2] = { .binding = RTVG_TEX_BINDING, .type = RT_BINDING_TEXTURE };
	info.binding[3] = { .binding = shader::fill_sampler_binding(backend), .type = RT_BINDING_SAMPLER };

	info.cull_mode = p.cull;
	info.wind_mode = RT_CCW;
	info.fill_mode = RT_FILL;
	info.primitive = RT_TRIANGLES;

	return rt->modules.emplace(key, rt_create_module_render(info)).first->second;
}

static constexpr RTVGstencil RTVG_STENCIL_OFF = { RT_ALWAYS, RT_STENCIL_KEEP, RT_STENCIL_KEEP, RT_STENCIL_KEEP };

static RTVGpipeline nvg_pipelinePlain(RTVGcontext* rt, const RTVGcall* call)
{
	RTVGpipeline p = {};
	p.blend = call->blendFunc;
	p.colorWrite = true;
	p.cull = RT_CULL_BACK;
	p.front = RTVG_STENCIL_OFF;
	p.back = RTVG_STENCIL_OFF;
	p.colorFormat = rt->passFormat;
	return p;
}

// ====================================================================
// Draw calls (all run inside the offscreen render pass opened by renderFlush)

static void nvg_bindPipeline(RTVGcontext* rt, const RTVGpipeline& p, int uniformOffset, int image)
{
	rt_bind_module_render(nvg_getModule(rt, p));
	rt_bind_buffer(rt->viewBuf, { .binding = RTVG_VIEW_BINDING, .offset = rt->viewOffset, .size = sizeof(RTVGviewUniforms) });
	rt_bind_buffer(rt->fragBuf, { .binding = RTVG_FRAG_BINDING, .offset = (size_t)uniformOffset, .size = rt->fragSize });

	RTVGtexture* tex = nullptr;
	if (image != 0) tex = nvg_findTexture(rt, image);
	// If no image is set, use empty texture
	if (tex == nullptr) tex = nvg_findTexture(rt, rt->dummyTex);
	if (tex != nullptr)
	{
		rt_bind_texture(tex->texture, { .binding = RTVG_TEX_BINDING });
		rt_bind_sampler(tex->sampler, { .binding = shader::fill_sampler_binding(rt_get_backend()) });
	}
}

static void nvg_fill(RTVGcontext* rt, RTVGcall* call)
{
	RTVGpath* paths = &rt->paths[call->pathOffset];
	int i, npaths = call->pathCount;

	// Draw shapes: stencil only, non-zero winding via INCR on front / DECR on back faces.
	RTVGpipeline p = nvg_pipelinePlain(rt, call);
	p.colorWrite = false;
	p.cull = RT_CULL_NONE;
	p.front = { RT_ALWAYS, RT_STENCIL_KEEP, RT_STENCIL_KEEP, RT_STENCIL_INCR_WRAP };
	p.back = { RT_ALWAYS, RT_STENCIL_KEEP, RT_STENCIL_KEEP, RT_STENCIL_DECR_WRAP };
	nvg_bindPipeline(rt, p, call->uniformOffset, 0);
	for (i = 0; i < npaths; i++)
		rt_draw_array(&rt->vertBuf, 1, paths[i].fillCount, 1, paths[i].fillOffset, 0);

	// Draw anti-aliased pixels where the stencil is still zero.
	if (rt->flags & RTVG_ANTIALIAS)
	{
		p = nvg_pipelinePlain(rt, call);
		p.front = p.back = { RT_EQUAL, RT_STENCIL_KEEP, RT_STENCIL_KEEP, RT_STENCIL_KEEP };
		nvg_bindPipeline(rt, p, call->uniformOffset + (int)rt->fragSize, call->image);
		for (i = 0; i < npaths; i++)
			rt_draw_array(&rt->vertBuf, 1, paths[i].strokeCount, 1, paths[i].strokeOffset, 0);
	}

	// Draw fill through the bounding box where the stencil is non-zero, resetting it.
	p = nvg_pipelinePlain(rt, call);
	p.front = p.back = { RT_NOTEQUAL, RT_STENCIL_ZERO, RT_STENCIL_ZERO, RT_STENCIL_ZERO };
	nvg_bindPipeline(rt, p, call->uniformOffset + (int)rt->fragSize, call->image);
	rt_draw_array(&rt->vertBuf, 1, call->triangleCount, 1, call->triangleOffset, 0);
}

static void nvg_convexFill(RTVGcontext* rt, RTVGcall* call)
{
	RTVGpath* paths = &rt->paths[call->pathOffset];
	int i, npaths = call->pathCount;

	nvg_bindPipeline(rt, nvg_pipelinePlain(rt, call), call->uniformOffset, call->image);
	for (i = 0; i < npaths; i++)
	{
		rt_draw_array(&rt->vertBuf, 1, paths[i].fillCount, 1, paths[i].fillOffset, 0);
		// Draw fringes
		if (paths[i].strokeCount > 0)
			rt_draw_array(&rt->vertBuf, 1, paths[i].strokeCount, 1, paths[i].strokeOffset, 0);
	}
}

static void nvg_stroke(RTVGcontext* rt, RTVGcall* call)
{
	RTVGpath* paths = &rt->paths[call->pathOffset];
	int i, npaths = call->pathCount;

	if (rt->flags & RTVG_STENCIL_STROKES)
	{
		// Fill the stroke base without overlap
		RTVGpipeline p = nvg_pipelinePlain(rt, call);
		p.front = p.back = { RT_EQUAL, RT_STENCIL_KEEP, RT_STENCIL_KEEP, RT_STENCIL_INCR };
		nvg_bindPipeline(rt, p, call->uniformOffset + (int)rt->fragSize, call->image);
		for (i = 0; i < npaths; i++)
			rt_draw_array(&rt->vertBuf, 1, paths[i].strokeCount, 1, paths[i].strokeOffset, 0);

		// Draw anti-aliased pixels.
		p = nvg_pipelinePlain(rt, call);
		p.front = p.back = { RT_EQUAL, RT_STENCIL_KEEP, RT_STENCIL_KEEP, RT_STENCIL_KEEP };
		nvg_bindPipeline(rt, p, call->uniformOffset, call->image);
		for (i = 0; i < npaths; i++)
			rt_draw_array(&rt->vertBuf, 1, paths[i].strokeCount, 1, paths[i].strokeOffset, 0);

		// Clear stencil buffer.
		p = nvg_pipelinePlain(rt, call);
		p.colorWrite = false;
		p.front = p.back = { RT_ALWAYS, RT_STENCIL_ZERO, RT_STENCIL_ZERO, RT_STENCIL_ZERO };
		nvg_bindPipeline(rt, p, call->uniformOffset, call->image);
		for (i = 0; i < npaths; i++)
			rt_draw_array(&rt->vertBuf, 1, paths[i].strokeCount, 1, paths[i].strokeOffset, 0);
	}
	else
	{
		// Draw Strokes
		nvg_bindPipeline(rt, nvg_pipelinePlain(rt, call), call->uniformOffset, call->image);
		for (i = 0; i < npaths; i++)
			rt_draw_array(&rt->vertBuf, 1, paths[i].strokeCount, 1, paths[i].strokeOffset, 0);
	}
}

static void nvg_triangles(RTVGcontext* rt, RTVGcall* call)
{
	nvg_bindPipeline(rt, nvg_pipelinePlain(rt, call), call->uniformOffset, call->image);
	rt_draw_array(&rt->vertBuf, 1, call->triangleCount, 1, call->triangleOffset, 0);
}

// ====================================================================
// Frame

static void nvg_ensureBuffer(rt_buffer_t& buffer, size_t needed, rt_buffer_usages_t usage)
{
	if (needed <= buffer.size) return;
	size_t size = buffer.size ? buffer.size : 1024;
	while (size < needed) size *= 2;
	rt_destroy_buffer(buffer);
	buffer = rt_create_buffer({ .size = size, .usage = usage, });
}

static void rt_renderViewport(void* uptr, float width, float height, float devicePixelRatio)
{
	auto rt = (RTVGcontext*)uptr;
	(void)devicePixelRatio;
	rt->view[0] = width;
	rt->view[1] = height;
}

static void rt_renderCancel(void* uptr)
{
	auto rt = (RTVGcontext*)uptr;
	rt->nverts = 0;
	rt->npaths = 0;
	rt->ncalls = 0;
	rt->nuniforms = 0;
}

static rt_blend_factor_t nvg_convertBlendFuncFactor(int factor, bool& valid)
{
	switch (factor)
	{
	case NVG_ZERO: return RT_BLEND_ZERO;
	case NVG_ONE: return RT_BLEND_ONE;
	case NVG_SRC_COLOR: return RT_BLEND_SRC_COLOR;
	case NVG_ONE_MINUS_SRC_COLOR: return RT_BLEND_ONE_MINUS_SRC_COLOR;
	case NVG_DST_COLOR: return RT_BLEND_DST_COLOR;
	case NVG_ONE_MINUS_DST_COLOR: return RT_BLEND_ONE_MINUS_DST_COLOR;
	case NVG_SRC_ALPHA: return RT_BLEND_SRC_ALPHA;
	case NVG_ONE_MINUS_SRC_ALPHA: return RT_BLEND_ONE_MINUS_SRC_ALPHA;
	case NVG_DST_ALPHA: return RT_BLEND_DST_ALPHA;
	case NVG_ONE_MINUS_DST_ALPHA: return RT_BLEND_ONE_MINUS_DST_ALPHA;
	case NVG_SRC_ALPHA_SATURATE: return RT_BLEND_SRC_ALPHA_SATURATE;
	default:
		valid = false;
		return RT_BLEND_ONE;
	}
}

static RTVGblend nvg_blendCompositeOperation(NVGcompositeOperationState op)
{
	bool valid = true;
	RTVGblend blend;
	blend.srcRGB = nvg_convertBlendFuncFactor(op.srcRGB, valid);
	blend.dstRGB = nvg_convertBlendFuncFactor(op.dstRGB, valid);
	blend.srcAlpha = nvg_convertBlendFuncFactor(op.srcAlpha, valid);
	blend.dstAlpha = nvg_convertBlendFuncFactor(op.dstAlpha, valid);
	if (!valid)
	{
		blend.srcRGB = RT_BLEND_ONE;
		blend.dstRGB = RT_BLEND_ONE_MINUS_SRC_ALPHA;
		blend.srcAlpha = RT_BLEND_ONE;
		blend.dstAlpha = RT_BLEND_ONE_MINUS_SRC_ALPHA;
	}
	return blend;
}

struct RTVGpass
{
	int begin = 0;
	int end = 0;
	rt_texture_view_t color = {};
	rt_texture_view_t stencil = {};
	// Colour texture texel size, falling back to the nvgBeginFrame view.
	float viewW = 0;
	float viewH = 0;
};

static void rt_renderFlush(void* uptr)
{
	auto rt = (RTVGcontext*)uptr;

	auto resolve = [&](NVGtarget target, RTVGpass& pass) -> bool
	{
		auto* color = (rt_texture_t*)target.colorTarget;
		auto* stencil = (rt_texture_t*)target.stencilTarget;
		if (color == nullptr || color->handle == 0 || color->default_view.handle == 0) return false;
		if (stencil == nullptr || stencil->handle == 0 || stencil->default_view.handle == 0) return false;

		pass.color = color->default_view;
		pass.stencil = stencil->default_view;
		if (color->width != 0 && color->height != 0)
		{
			pass.viewW = (float)color->width;
			pass.viewH = (float)color->height;
		}
		else
		{
			pass.viewW = rt->view[0];
			pass.viewH = rt->view[1];
		}
		return true;
	};

	std::vector<RTVGpass> passes;
	for (int i = 0; i < rt->ncalls; )
	{
		int begin = i;
		NVGtarget key = rt->calls[i].target;
		++i;
		while (i < rt->ncalls && rt->calls[i].target.colorTarget == key.colorTarget && rt->calls[i].target.stencilTarget == key.stencilTarget)
			++i;

		RTVGpass batch;
		batch.begin = begin;
		batch.end = i;
		if (!resolve(key, batch)) continue;
		passes.push_back(batch);
	}

	if (!passes.empty() && rt->ncalls > 0)
	{
		size_t vertBytes = (size_t)rt->nverts * sizeof(NVGvertex);
		size_t fragBytes = (size_t)rt->nuniforms * rt->fragSize;
		size_t viewBytes = passes.size() * RTVG_UNIFORM_ALIGN;
		nvg_ensureBuffer(rt->vertBuf, vertBytes, RT_BUFFER_USAGE_VERTEX | RT_BUFFER_USAGE_COPY_DST);
		nvg_ensureBuffer(rt->fragBuf, fragBytes, RT_BUFFER_USAGE_UNIFORM | RT_BUFFER_USAGE_COPY_DST);
		nvg_ensureBuffer(rt->viewBuf, viewBytes, RT_BUFFER_USAGE_UNIFORM | RT_BUFFER_USAGE_COPY_DST);

		std::vector<uint8_t> views(viewBytes);
		for (size_t p = 0; p < passes.size(); ++p)
		{
			RTVGviewUniforms view = { { passes[p].viewW, passes[p].viewH }, { 0.0f, 0.0f } };
			memcpy(views.data() + p * RTVG_UNIFORM_ALIGN, &view, sizeof(view));
		}

		rt_pass_transfer_t transfer;
		rt_begin_transfer(transfer);
		if (vertBytes)
			rt_copy_buffer_data({ .data = (const uint8_t*)rt->verts.data(), .size = vertBytes, }, { .buffer = rt->vertBuf, }, vertBytes);
		if (fragBytes)
			rt_copy_buffer_data({ .data = rt->uniforms.data(), .size = fragBytes, }, { .buffer = rt->fragBuf, }, fragBytes);
		rt_copy_buffer_data({ .data = views.data(), .size = viewBytes, }, { .buffer = rt->viewBuf, }, viewBytes);
		rt_end_transfer(transfer);
	}

	for (size_t p = 0; p < passes.size(); ++p)
	{
		RTVGpass const& batch = passes[p];
		rt->passFormat = batch.color.format != RT_TEXTURE_NONE ? batch.color.format : RT_TEXTURE_RGBA8UNORM;
		rt->viewOffset = p * RTVG_UNIFORM_ALIGN;

		// Stencil is cleared on every pass so fill winding starts from zero.
		rt_pass_render_t pass = {
			.colors = { { .texture_view = batch.color, .clear = false, } },
			.depth = { .texture_view = batch.stencil, .clear = true, },
			.stencil = { .clear = true, .value = 0, .refer = 0, },
		};
		rt_begin_render(pass);
		for (int i = batch.begin; i < batch.end; ++i)
		{
			RTVGcall* call = &rt->calls[i];
			if (call->type == RTVG_FILL)
				nvg_fill(rt, call);
			else if (call->type == RTVG_CONVEXFILL)
				nvg_convexFill(rt, call);
			else if (call->type == RTVG_STROKE)
				nvg_stroke(rt, call);
			else if (call->type == RTVG_TRIANGLES)
				nvg_triangles(rt, call);
		}
		rt_end_render(pass);
	}

	rt->nverts = 0;
	rt->npaths = 0;
	rt->ncalls = 0;
	rt->nuniforms = 0;
}

// ====================================================================
// Call recording

static int nvg_maxVertCount(const NVGpath* paths, int npaths)
{
	int i, count = 0;
	for (i = 0; i < npaths; i++)
	{
		count += paths[i].nfill;
		count += paths[i].nstroke;
	}
	return count;
}

static RTVGcall* nvg_allocCall(RTVGcontext* rt)
{
	if ((size_t)rt->ncalls + 1 > rt->calls.size()) rt->calls.resize(rt->calls.size() * 2 + 1);
	RTVGcall* ret = &rt->calls[rt->ncalls++];
	memset(ret, 0, sizeof(RTVGcall));
	if (rt->ctx) ret->target = nvgTarget(rt->ctx);
	return ret;
}

static int nvg_allocPaths(RTVGcontext* rt, int n)
{
	size_t needed = (size_t)rt->npaths + n;
	if (needed > rt->paths.size()) rt->paths.resize(needed * 2);
	int ret = rt->npaths;
	rt->npaths += n;
	return ret;
}

static int nvg_allocVerts(RTVGcontext* rt, int n)
{
	size_t needed = (size_t)rt->nverts + n;
	if (needed > rt->verts.size()) rt->verts.resize(needed * 2);
	int ret = rt->nverts;
	rt->nverts += n;
	return ret;
}

// Returns a byte offset into the uniform buffer; consecutive blocks are fragSize apart.
static int nvg_allocFragUniforms(RTVGcontext* rt, int n)
{
	size_t needed = (size_t)(rt->nuniforms + n) * rt->fragSize;
	if (needed > rt->uniforms.size()) rt->uniforms.resize(needed * 2);
	int ret = (int)((size_t)rt->nuniforms * rt->fragSize);
	rt->nuniforms += n;
	return ret;
}

static RTVGfragUniforms* nvg_fragUniformPtr(RTVGcontext* rt, int i)
{
	return (RTVGfragUniforms*)&rt->uniforms[(size_t)i];
}

static void nvg_vset(NVGvertex* vtx, float x, float y, float u, float v)
{
	vtx->x = x;
	vtx->y = y;
	vtx->u = u;
	vtx->v = v;
}

static void rt_renderFill(void* uptr, NVGpaint* paint, NVGcompositeOperationState compositeOperation,
	NVGscissor* scissor, float fringe,
	const float* bounds, const NVGpath* paths, int npaths)
{
	auto rt = (RTVGcontext*)uptr;
	RTVGcall* call = nvg_allocCall(rt);
	NVGvertex* quad;
	RTVGfragUniforms* frag;
	int i, maxverts, offset;

	if (call == nullptr) return;

	call->type = RTVG_FILL;
	call->triangleCount = 6; // Bounding box as two triangles
	call->pathOffset = nvg_allocPaths(rt, npaths);
	call->pathCount = npaths;
	call->image = paint->image;
	call->blendFunc = nvg_blendCompositeOperation(compositeOperation);

	if (npaths == 1 && paths[0].convex)
	{
		call->type = RTVG_CONVEXFILL;
		call->triangleCount = 0; // Bounding box fill quad not needed for convex fill
	}

	// Allocate vertices for all the paths.
	maxverts = nvg_maxVertCount(paths, npaths) + call->triangleCount;
	offset = nvg_allocVerts(rt, maxverts);

	for (i = 0; i < npaths; i++)
	{
		RTVGpath* copy = &rt->paths[call->pathOffset + i];
		const NVGpath* path = &paths[i];
		memset(copy, 0, sizeof(RTVGpath));
		if (path->nfill > 0)
		{
			copy->fillOffset = offset;
			copy->fillCount = path->nfill;
			memcpy(&rt->verts[offset], path->fill, sizeof(NVGvertex) * path->nfill);
			offset += path->nfill;
		}
		if (path->nstroke > 0)
		{
			copy->strokeOffset = offset;
			copy->strokeCount = path->nstroke;
			memcpy(&rt->verts[offset], path->stroke, sizeof(NVGvertex) * path->nstroke);
			offset += path->nstroke;
		}
	}

	// Setup uniforms for draw calls
	if (call->type == RTVG_FILL)
	{
		// Quad as two triangles (same winding as nanovg_gl.h)
		call->triangleOffset = offset;
		quad = &rt->verts[call->triangleOffset];
		nvg_vset(&quad[0], bounds[2], bounds[3], 0.5f, 1.0f);
		nvg_vset(&quad[1], bounds[2], bounds[1], 0.5f, 1.0f);
		nvg_vset(&quad[2], bounds[0], bounds[3], 0.5f, 1.0f);
		nvg_vset(&quad[3], bounds[0], bounds[3], 0.5f, 1.0f);
		nvg_vset(&quad[4], bounds[2], bounds[1], 0.5f, 1.0f);
		nvg_vset(&quad[5], bounds[0], bounds[1], 0.5f, 1.0f);

		call->uniformOffset = nvg_allocFragUniforms(rt, 2);
		// Simple shader for stencil
		frag = nvg_fragUniformPtr(rt, call->uniformOffset);
		memset(frag, 0, sizeof(*frag));
		frag->strokeThr = -1.0f;
		frag->type = NSVG_SHADER_SIMPLE;
		// Fill shader
		nvg_convertPaint(rt, nvg_fragUniformPtr(rt, call->uniformOffset + (int)rt->fragSize), paint, scissor, fringe, fringe, -1.0f);
	}
	else
	{
		call->uniformOffset = nvg_allocFragUniforms(rt, 1);
		// Fill shader
		nvg_convertPaint(rt, nvg_fragUniformPtr(rt, call->uniformOffset), paint, scissor, fringe, fringe, -1.0f);
	}
}

static void rt_renderStroke(void* uptr, NVGpaint* paint, NVGcompositeOperationState compositeOperation,
	NVGscissor* scissor, float fringe,
	float strokeWidth, const NVGpath* paths, int npaths)
{
	auto rt = (RTVGcontext*)uptr;
	RTVGcall* call = nvg_allocCall(rt);
	int i, maxverts, offset;

	if (call == nullptr) return;

	call->type = RTVG_STROKE;
	call->pathOffset = nvg_allocPaths(rt, npaths);
	call->pathCount = npaths;
	call->image = paint->image;
	call->blendFunc = nvg_blendCompositeOperation(compositeOperation);

	// Allocate vertices for all the paths.
	maxverts = nvg_maxVertCount(paths, npaths);
	offset = nvg_allocVerts(rt, maxverts);

	for (i = 0; i < npaths; i++)
	{
		RTVGpath* copy = &rt->paths[call->pathOffset + i];
		const NVGpath* path = &paths[i];
		memset(copy, 0, sizeof(RTVGpath));
		if (path->nstroke)
		{
			copy->strokeOffset = offset;
			copy->strokeCount = path->nstroke;
			memcpy(&rt->verts[offset], path->stroke, sizeof(NVGvertex) * path->nstroke);
			offset += path->nstroke;
		}
	}

	if (rt->flags & RTVG_STENCIL_STROKES)
	{
		// Fill shader
		call->uniformOffset = nvg_allocFragUniforms(rt, 2);
		nvg_convertPaint(rt, nvg_fragUniformPtr(rt, call->uniformOffset), paint, scissor, strokeWidth, fringe, -1.0f);
		nvg_convertPaint(rt, nvg_fragUniformPtr(rt, call->uniformOffset + (int)rt->fragSize), paint, scissor, strokeWidth, fringe, 1.0f - 0.5f / 255.0f);
	}
	else
	{
		// Fill shader
		call->uniformOffset = nvg_allocFragUniforms(rt, 1);
		nvg_convertPaint(rt, nvg_fragUniformPtr(rt, call->uniformOffset), paint, scissor, strokeWidth, fringe, -1.0f);
	}
}

static void rt_renderTriangles(void* uptr, NVGpaint* paint, NVGcompositeOperationState compositeOperation,
	NVGscissor* scissor,
	const NVGvertex* verts, int nverts, float fringe)
{
	auto rt = (RTVGcontext*)uptr;
	RTVGcall* call = nvg_allocCall(rt);
	RTVGfragUniforms* frag;

	if (call == nullptr) return;

	call->type = RTVG_TRIANGLES;
	call->image = paint->image;
	call->blendFunc = nvg_blendCompositeOperation(compositeOperation);

	// Allocate vertices for all the paths.
	call->triangleOffset = nvg_allocVerts(rt, nverts);
	call->triangleCount = nverts;

	memcpy(&rt->verts[call->triangleOffset], verts, sizeof(NVGvertex) * nverts);

	// Fill shader
	call->uniformOffset = nvg_allocFragUniforms(rt, 1);
	frag = nvg_fragUniformPtr(rt, call->uniformOffset);
	nvg_convertPaint(rt, frag, paint, scissor, 1.0f, fringe, -1.0f);
	frag->type = NSVG_SHADER_IMG;
}

// ====================================================================
// Lifetime

static int rt_renderCreate(void* uptr)
{
	auto rt = (RTVGcontext*)uptr;

	rt->fragSize = (sizeof(RTVGfragUniforms) + RTVG_UNIFORM_ALIGN - 1) & ~(RTVG_UNIFORM_ALIGN - 1);

	// Dynamic per-frame buffers; grown on demand in renderFlush.
	rt->vertBuf = rt_create_buffer({ .size = sizeof(NVGvertex) * 4096, .usage = RT_BUFFER_USAGE_VERTEX | RT_BUFFER_USAGE_COPY_DST, });
	rt->fragBuf = rt_create_buffer({ .size = rt->fragSize * 128, .usage = RT_BUFFER_USAGE_UNIFORM | RT_BUFFER_USAGE_COPY_DST, });
	rt->viewBuf = rt_create_buffer({ .size = RTVG_UNIFORM_ALIGN, .usage = RT_BUFFER_USAGE_UNIFORM | RT_BUFFER_USAGE_COPY_DST, });

	// Some platforms does not allow to have samples to unset textures.
	// Create empty one which is bound when there's no texture specified.
	rt->dummyTex = rt_renderCreateTexture(rt, NVG_TEXTURE_ALPHA, 1, 1, 0, nullptr);

	rt_submit();
	return 1;
}

static void rt_renderDelete(void* uptr)
{
	auto rt = (RTVGcontext*)uptr;
	if (rt == nullptr) return;

	for (auto& module : rt->modules)
		rt_destroy_module_render(module.second);
	rt->modules.clear();

	for (auto& texture : rt->textures)
	{
		rt_destroy_sampler(texture.second.sampler);
		if ((texture.second.flags & RTVG_IMAGE_NODELETE) == 0)  rt_destroy_texture(texture.second.texture);
	}
	rt->textures.clear();

	rt_destroy_buffer(rt->vertBuf);
	rt_destroy_buffer(rt->fragBuf);
	rt_destroy_buffer(rt->viewBuf);

	delete rt;
}

NVGcontext* rtCreateRT(int flags)
{
	NVGparams params;
	NVGcontext* ctx = nullptr;
	auto rt = new RTVGcontext;

	memset(&params, 0, sizeof(params));
	params.renderCreate = rt_renderCreate;
	params.renderCreateTexture = rt_renderCreateTexture;
	params.renderDeleteTexture = rt_renderDeleteTexture;
	params.renderUpdateTexture = rt_renderUpdateTexture;
	params.renderGetTextureSize = rt_renderGetTextureSize;
	params.renderViewport = rt_renderViewport;
	params.renderCancel = rt_renderCancel;
	params.renderFlush = rt_renderFlush;
	params.renderFill = rt_renderFill;
	params.renderStroke = rt_renderStroke;
	params.renderTriangles = rt_renderTriangles;
	params.renderDelete = rt_renderDelete;
	params.userPtr = rt;
	params.edgeAntiAlias = (flags & RTVG_ANTIALIAS) ? 1 : 0;

	rt->flags = flags;

	ctx = nvgCreateInternal(&params);
	if (ctx == nullptr) goto error;
	rt->ctx = ctx;

	return ctx;

error:
	// 'gl' is freed by nvgDeleteInternal.
	if (ctx != nullptr) nvgDeleteInternal(ctx);
	return nullptr;
}

void rtDeleteRT(NVGcontext* ctx)
{
	nvgDeleteInternal(ctx);
}

int rtCreateTextureNative(NVGcontext* ctx, int imageFlags, rt_texture_t const& texture)
{
	auto rt = (RTVGcontext*)nvgInternalParams(ctx)->userPtr;

	rt_filter_t minFilter, magFilter;
	if (imageFlags & NVG_IMAGE_GENERATE_MIPMAPS)
		minFilter = (imageFlags & NVG_IMAGE_NEAREST) ? RT_NEAREST_MIPMAP_NEAREST : RT_LINEAR_MIPMAP_LINEAR;
	else
		minFilter = (imageFlags & NVG_IMAGE_NEAREST) ? RT_NEAREST : RT_LINEAR;
	magFilter = (imageFlags & NVG_IMAGE_NEAREST) ? RT_NEAREST : RT_LINEAR;
	rt_address_t addressU = (imageFlags & NVG_IMAGE_REPEATX) ? RT_REPEAT : RT_CLAMP_TO_EDGE;
	rt_address_t addressV = (imageFlags & NVG_IMAGE_REPEATY) ? RT_REPEAT : RT_CLAMP_TO_EDGE;

	// Texture views do not carry sampler state, so bind an explicit sampler alongside.
	rt_sampler_info_t sampler = {};
	sampler.min_filter = minFilter;
	sampler.mag_filter = magFilter;
	sampler.address_u = addressU;
	sampler.address_v = addressV;
	sampler.address_w = RT_CLAMP_TO_EDGE;

	auto& tex = rt->textures[(int)texture.handle];
	tex.texture = texture;
	tex.sampler = rt_create_sampler(sampler);
	tex.id = (int)texture.handle;
	tex.width = texture.width;
	tex.height = texture.height;
	tex.type = (texture.format == RT_TEXTURE_RGBA8UNORM) ? NVG_TEXTURE_RGBA : NVG_TEXTURE_ALPHA;
	tex.flags = imageFlags | RTVG_IMAGE_NODELETE;
	return tex.id;
}

rt_texture_t rtGetTextureNative(NVGcontext* ctx, int imageHandle)
{
	auto rt = (RTVGcontext*)nvgInternalParams(ctx)->userPtr;

	return nvg_findTexture(rt, imageHandle)->texture;
}
