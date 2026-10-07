//
// Copyright (c) 2009-2013 Mikko Mononen memon@inside.org
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
#pragma once
#include <GL/glew.h>
#include "nanovg.h"

#ifdef __cplusplus
extern "C" {
#endif

// Create flags

enum NVGcreateFlags {
	// Flag indicating if geometry based anti-aliasing is used (may not be needed when using MSAA).
	NVG_ANTIALIAS 		= 1<<0,
	// Flag indicating if strokes should be drawn using stencil buffer. The rendering will be a little
	// slower, but path overlaps (i.e. self-intersecting or sharp turns) will be drawn just once.
	NVG_STENCIL_STROKES	= 1<<1,
	// Flag indicating that additional debug checks are done.
	NVG_DEBUG 			= 1<<2,
};

#define NANOVG_GL_USE_STATE_FILTER (1)

// Creates NanoVG contexts for different OpenGL (ES) versions.
// Flags should be combination of the create flags above.

NVGcontext* nvgCreateGL3(int flags);
void nvgDeleteGL3(NVGcontext* ctx);

int nvglCreateImageFromHandleGL3(NVGcontext* ctx, GLuint textureId, int w, int h, int flags);
GLuint nvglImageHandleGL3(NVGcontext* ctx, int image);

// These are additional flags on top of NVGimageFlags.
enum NVGimageFlagsGL {
	NVG_IMAGE_NODELETE			= 1<<16,	// Do not delete GL texture handle.
};

#ifdef __cplusplus
}
#endif

#ifdef NANOVG_GL3_IMPLEMENTATION

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "nanovg.h"

enum RTVGuniformLoc {
	RTVG_LOC_VIEWSIZE,
	RTVG_LOC_TEX,
	RTVG_LOC_FRAG,
	RTVG_MAX_LOCS
};

enum RTVGshaderType {
	NSVG_SHADER_FILLGRAD,
	NSVG_SHADER_FILLIMG,
	NSVG_SHADER_SIMPLE,
	NSVG_SHADER_IMG
};

enum RTVGuniformBindings {
	RTVG_FRAG_BINDING = 0,
};

struct RTVGshader {
	GLuint prog;
	GLuint frag;
	GLuint vert;
	GLint loc[RTVG_MAX_LOCS];
};
typedef struct RTVGshader RTVGshader;

struct RTVGtexture {
	int id;
	GLuint tex;
	int width, height;
	int type;
	int flags;
};
typedef struct RTVGtexture RTVGtexture;

struct RTVGblend
{
	GLenum srcRGB;
	GLenum dstRGB;
	GLenum srcAlpha;
	GLenum dstAlpha;
};
typedef struct RTVGblend RTVGblend;

enum RTVGcallType {
	RTVG_NONE = 0,
	RTVG_FILL,
	RTVG_CONVEXFILL,
	RTVG_STROKE,
	RTVG_TRIANGLES,
};

struct RTVGcall {
	int type;
	int image;
	int pathOffset;
	int pathCount;
	int triangleOffset;
	int triangleCount;
	int uniformOffset;
	RTVGblend blendFunc;
};
typedef struct RTVGcall RTVGcall;

struct RTVGpath {
	int fillOffset;
	int fillCount;
	int strokeOffset;
	int strokeCount;
};
typedef struct RTVGpath RTVGpath;

struct RTVGfragUniforms {
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
typedef struct RTVGfragUniforms RTVGfragUniforms;

struct RTVGcontext {
	RTVGshader shader;
	RTVGtexture* textures;
	float view[2];
	int ntextures;
	int ctextures;
	int textureId;
	GLuint vertBuf;
	GLuint vertArr;
	GLuint fragBuf;
	int fragSize;
	int flags;

	// Per frame buffers
	RTVGcall* calls;
	int ccalls;
	int ncalls;
	RTVGpath* paths;
	int cpaths;
	int npaths;
	struct NVGvertex* verts;
	int cverts;
	int nverts;
	unsigned char* uniforms;
	int cuniforms;
	int nuniforms;

	// cached state
	#if NANOVG_GL_USE_STATE_FILTER
	GLuint boundTexture;
	GLuint stencilMask;
	GLenum stencilFunc;
	GLint stencilFuncRef;
	GLuint stencilFuncMask;
	RTVGblend blendFunc;
	#endif

	int dummyTex;
};
typedef struct RTVGcontext RTVGcontext;

[[deprecated]]
static int nvg_maxi(int a, int b) { return a > b ? a : b; }

[[deprecated]]
static void nvg_bindTexture(RTVGcontext* gl, GLuint tex)
{
#if NANOVG_GL_USE_STATE_FILTER
	if (gl->boundTexture != tex) {
		gl->boundTexture = tex;
		glBindTexture(GL_TEXTURE_2D, tex);
	}
#else
	glBindTexture(GL_TEXTURE_2D, tex);
#endif
}

[[deprecated]]
static void nvg_stencilMask(RTVGcontext* gl, GLuint mask)
{
#if NANOVG_GL_USE_STATE_FILTER
	if (gl->stencilMask != mask) {
		gl->stencilMask = mask;
		glStencilMask(mask);
	}
#else
	glStencilMask(mask);
#endif
}

[[deprecated]]
static void nvg_stencilFunc(RTVGcontext* gl, GLenum func, GLint ref, GLuint mask)
{
#if NANOVG_GL_USE_STATE_FILTER
	if ((gl->stencilFunc != func) ||
		(gl->stencilFuncRef != ref) ||
		(gl->stencilFuncMask != mask)) {

		gl->stencilFunc = func;
		gl->stencilFuncRef = ref;
		gl->stencilFuncMask = mask;
		glStencilFunc(func, ref, mask);
	}
#else
	glStencilFunc(func, ref, mask);
#endif
}

[[deprecated]]
static void nvg_blendFuncSeparate(RTVGcontext* gl, const RTVGblend* blend)
{
#if NANOVG_GL_USE_STATE_FILTER
	if ((gl->blendFunc.srcRGB != blend->srcRGB) ||
		(gl->blendFunc.dstRGB != blend->dstRGB) ||
		(gl->blendFunc.srcAlpha != blend->srcAlpha) ||
		(gl->blendFunc.dstAlpha != blend->dstAlpha)) {

		gl->blendFunc = *blend;
		glBlendFuncSeparate(blend->srcRGB, blend->dstRGB, blend->srcAlpha,blend->dstAlpha);
	}
#else
	glBlendFuncSeparate(blend->srcRGB, blend->dstRGB, blend->srcAlpha,blend->dstAlpha);
#endif
}

static RTVGtexture* nvg_allocTexture(RTVGcontext* gl)
{
	RTVGtexture* tex = NULL;
	int i;

	for (i = 0; i < gl->ntextures; i++) {
		if (gl->textures[i].id == 0) {
			tex = &gl->textures[i];
			break;
		}
	}
	if (tex == NULL) {
		if (gl->ntextures+1 > gl->ctextures) {
			RTVGtexture* textures;
			int ctextures = nvg_maxi(gl->ntextures+1, 4) +  gl->ctextures/2; // 1.5x Overallocate
			textures = (RTVGtexture*)realloc(gl->textures, sizeof(RTVGtexture)*ctextures);
			if (textures == NULL) return NULL;
			gl->textures = textures;
			gl->ctextures = ctextures;
		}
		tex = &gl->textures[gl->ntextures++];
	}

	memset(tex, 0, sizeof(*tex));
	tex->id = ++gl->textureId;

	return tex;
}

[[deprecated]]
static RTVGtexture* nvg_findTexture(RTVGcontext* gl, int id)
{
	int i;
	for (i = 0; i < gl->ntextures; i++)
		if (gl->textures[i].id == id)
			return &gl->textures[i];
	return NULL;
}

[[deprecated]]
static int nvg_deleteTexture(RTVGcontext* gl, int id)
{
	int i;
	for (i = 0; i < gl->ntextures; i++) {
		if (gl->textures[i].id == id) {
			if (gl->textures[i].tex != 0 && (gl->textures[i].flags & NVG_IMAGE_NODELETE) == 0)
				glDeleteTextures(1, &gl->textures[i].tex);
			memset(&gl->textures[i], 0, sizeof(gl->textures[i]));
			return 1;
		}
	}
	return 0;
}

[[deprecated]]
static void nvg_dumpShaderError(GLuint shader, const char* name, const char* type)
{
	GLchar str[1024];
	GLsizei len = 0;
	glGetShaderInfoLog(shader, sizeof(str), &len, str);
	str[len] = '\0';
	printf("Shader %s/%s error:\n%s\n", name, type, str);
}

[[deprecated]]
static void nvg_dumpProgramError(GLuint prog, const char* name)
{
	GLchar str[512+1];
	GLsizei len = 0;
	glGetProgramInfoLog(prog, 512, &len, str);
	if (len > 512) len = 512;
	str[len] = '\0';
	printf("Program %s error:\n%s\n", name, str);
}

[[deprecated]]
static void nvg_checkError(RTVGcontext* gl, const char* str)
{
	GLenum err;
	if ((gl->flags & NVG_DEBUG) == 0) return;
	err = glGetError();
	if (err != GL_NO_ERROR) {
		printf("Error %08x after %s\n", err, str);
		return;
	}
}

[[deprecated]]
static int nvg_createShader(RTVGshader* shader, const char* name, const char* header, const char* opts, const char* vshader, const char* fshader)
{
	GLint status;
	GLuint prog, vert, frag;
	const char* str[3];
	str[0] = header;
	str[1] = opts != NULL ? opts : "";

	memset(shader, 0, sizeof(*shader));

	prog = glCreateProgram();
	vert = glCreateShader(GL_VERTEX_SHADER);
	frag = glCreateShader(GL_FRAGMENT_SHADER);
	str[2] = vshader;
	glShaderSource(vert, 3, str, 0);
	str[2] = fshader;
	glShaderSource(frag, 3, str, 0);

	glCompileShader(vert);
	glGetShaderiv(vert, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE) {
		nvg_dumpShaderError(vert, name, "vert");
		return 0;
	}

	glCompileShader(frag);
	glGetShaderiv(frag, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE) {
		nvg_dumpShaderError(frag, name, "frag");
		return 0;
	}

	glAttachShader(prog, vert);
	glAttachShader(prog, frag);

	glBindAttribLocation(prog, 0, "vertex");
	glBindAttribLocation(prog, 1, "tcoord");

	glLinkProgram(prog);
	glGetProgramiv(prog, GL_LINK_STATUS, &status);
	if (status != GL_TRUE) {
		nvg_dumpProgramError(prog, name);
		return 0;
	}

	shader->prog = prog;
	shader->vert = vert;
	shader->frag = frag;

	return 1;
}

[[deprecated]]
static void nvg_deleteShader(RTVGshader* shader)
{
	if (shader->prog != 0)
		glDeleteProgram(shader->prog);
	if (shader->vert != 0)
		glDeleteShader(shader->vert);
	if (shader->frag != 0)
		glDeleteShader(shader->frag);
}

[[deprecated]]
static void nvg_getUniforms(RTVGshader* shader)
{
	shader->loc[RTVG_LOC_VIEWSIZE] = glGetUniformLocation(shader->prog, "viewSize");
	shader->loc[RTVG_LOC_TEX] = glGetUniformLocation(shader->prog, "tex");
	shader->loc[RTVG_LOC_FRAG] = glGetUniformBlockIndex(shader->prog, "frag");
}

static int rt_renderCreateTexture(void* uptr, int type, int w, int h, int imageFlags, const unsigned char* data);

static int rt_renderCreate(void* uptr)
{
	RTVGcontext* gl = (RTVGcontext*)uptr;
	int align = 4;

	// TODO: mediump float may not be enough for GLES2 in iOS.
	// see the following discussion: https://github.com/memononen/nanovg/issues/46
	static const char* shaderHeader =
		"#version 150 core\n"
		"\n";

	static const char* fillVertShader =
		"	uniform vec2 viewSize;\n"
		"	in vec2 vertex;\n"
		"	in vec2 tcoord;\n"
		"	out vec2 ftcoord;\n"
		"	out vec2 fpos;\n"
		"void main(void) {\n"
		"	ftcoord = tcoord;\n"
		"	fpos = vertex;\n"
		"	gl_Position = vec4(2.0*vertex.x/viewSize.x - 1.0, 1.0 - 2.0*vertex.y/viewSize.y, 0, 1);\n"
		"}\n";

	static const char* fillFragShader =
		"layout(std140) uniform frag {\n"
		"	mat3 scissorMat;\n"
		"	mat3 paintMat;\n"
		"	vec4 innerCol;\n"
		"	vec4 outerCol;\n"
		"	vec2 scissorExt;\n"
		"	vec2 scissorScale;\n"
		"	vec2 extent;\n"
		"	float radius;\n"
		"	float feather;\n"
		"	float strokeMult;\n"
		"	float strokeThr;\n"
		"	int texType;\n"
		"	int type;\n"
		"};\n"
		"uniform sampler2D tex;\n"
		"in vec2 ftcoord;\n"
		"in vec2 fpos;\n"
		"out vec4 outColor;\n"
		"\n"
		"float sdroundrect(vec2 pt, vec2 ext, float rad) {\n"
		"	vec2 ext2 = ext - vec2(rad,rad);\n"
		"	vec2 d = abs(pt) - ext2;\n"
		"	return min(max(d.x,d.y),0.0) + length(max(d,0.0)) - rad;\n"
		"}\n"
		"\n"
		"// Scissoring\n"
		"float scissorMask(vec2 p) {\n"
		"	vec2 sc = (abs((scissorMat * vec3(p,1.0)).xy) - scissorExt);\n"
		"	sc = vec2(0.5,0.5) - sc * scissorScale;\n"
		"	return clamp(sc.x,0.0,1.0) * clamp(sc.y,0.0,1.0);\n"
		"}\n"
		"#ifdef EDGE_AA\n"
		"// Stroke - from [0..1] to clipped pyramid, where the slope is 1px.\n"
		"float strokeMask() {\n"
		"	return min(1.0, (1.0-abs(ftcoord.x*2.0-1.0))*strokeMult) * min(1.0, ftcoord.y);\n"
		"}\n"
		"#endif\n"
		"\n"
		"void main(void) {\n"
		"   vec4 result;\n"
		"	float scissor = scissorMask(fpos);\n"
		"#ifdef EDGE_AA\n"
		"	float strokeAlpha = strokeMask();\n"
		"	if (strokeAlpha < strokeThr) discard;\n"
		"#else\n"
		"	float strokeAlpha = 1.0;\n"
		"#endif\n"
		"	if (type == 0) {			// Gradient\n"
		"		// Calculate gradient color using box gradient\n"
		"		vec2 pt = (paintMat * vec3(fpos,1.0)).xy;\n"
		"		float d = clamp((sdroundrect(pt, extent, radius) + feather*0.5) / feather, 0.0, 1.0);\n"
		"		vec4 color = mix(innerCol,outerCol,d);\n"
		"		// Combine alpha\n"
		"		color *= strokeAlpha * scissor;\n"
		"		result = color;\n"
		"	} else if (type == 1) {		// Image\n"
		"		// Calculate color fron texture\n"
		"		vec2 pt = (paintMat * vec3(fpos,1.0)).xy / extent;\n"
		"		vec4 color = texture(tex, pt);\n"
		"		if (texType == 1) color = vec4(color.xyz*color.w,color.w);"
		"		if (texType == 2) color = vec4(color.x);"
		"		// Apply color tint and alpha.\n"
		"		color *= innerCol;\n"
		"		// Combine alpha\n"
		"		color *= strokeAlpha * scissor;\n"
		"		result = color;\n"
		"	} else if (type == 2) {		// Stencil fill\n"
		"		result = vec4(1,1,1,1);\n"
		"	} else if (type == 3) {		// Textured tris\n"
		"		vec4 color = texture(tex, ftcoord);\n"
		"		if (texType == 1) color = vec4(color.xyz*color.w,color.w);"
		"		if (texType == 2) color = vec4(color.x);"
		"		color *= scissor;\n"
		"		result = color * innerCol;\n"
		"	}\n"
		"	outColor = result;\n"
		"}\n";

	nvg_checkError(gl, "init");

	if (gl->flags & NVG_ANTIALIAS) {
		if (nvg_createShader(&gl->shader, "shader", shaderHeader, "#define EDGE_AA 1\n", fillVertShader, fillFragShader) == 0)
			return 0;
	} else {
		if (nvg_createShader(&gl->shader, "shader", shaderHeader, NULL, fillVertShader, fillFragShader) == 0)
			return 0;
	}

	nvg_checkError(gl, "uniform locations");
	nvg_getUniforms(&gl->shader);

	// Create dynamic vertex array
	glGenVertexArrays(1, &gl->vertArr);
	glGenBuffers(1, &gl->vertBuf);

	// Create UBOs
	glUniformBlockBinding(gl->shader.prog, gl->shader.loc[RTVG_LOC_FRAG], RTVG_FRAG_BINDING);
	glGenBuffers(1, &gl->fragBuf);
	glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &align);

	gl->fragSize = sizeof(RTVGfragUniforms) + align - sizeof(RTVGfragUniforms) % align;

	// Some platforms does not allow to have samples to unset textures.
	// Create empty one which is bound when there's no texture specified.
	gl->dummyTex = rt_renderCreateTexture(gl, NVG_TEXTURE_ALPHA, 1, 1, 0, NULL);

	nvg_checkError(gl, "create done");

	glFinish();

	return 1;
}

static int rt_renderCreateTexture(void* uptr, int type, int w, int h, int imageFlags, const unsigned char* data)
{
	RTVGcontext* gl = (RTVGcontext*)uptr;
	RTVGtexture* tex = nvg_allocTexture(gl);

	if (tex == NULL) return 0;

	glGenTextures(1, &tex->tex);
	tex->width = w;
	tex->height = h;
	tex->type = type;
	tex->flags = imageFlags;
	nvg_bindTexture(gl, tex->tex);

	glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, tex->width);
	glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
	glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);

	if (type == NVG_TEXTURE_RGBA)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	else
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, w, h, 0, GL_RED, GL_UNSIGNED_BYTE, data);

	if (imageFlags & NVG_IMAGE_GENERATE_MIPMAPS) {
		if (imageFlags & NVG_IMAGE_NEAREST) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		} else {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		}
	} else {
		if (imageFlags & NVG_IMAGE_NEAREST) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		} else {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		}
	}

	if (imageFlags & NVG_IMAGE_NEAREST) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	} else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	if (imageFlags & NVG_IMAGE_REPEATX)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	else
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

	if (imageFlags & NVG_IMAGE_REPEATY)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	else
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
	glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);

	// The new way to build mipmaps on GLES and GL3
#if !defined(NANOVG_GL2)
	if (imageFlags & NVG_IMAGE_GENERATE_MIPMAPS) {
		glGenerateMipmap(GL_TEXTURE_2D);
	}
#endif

	nvg_checkError(gl, "create tex");
	nvg_bindTexture(gl, 0);

	return tex->id;
}

static int rt_renderDeleteTexture(void* uptr, int image)
{
	RTVGcontext* gl = (RTVGcontext*)uptr;
	return nvg_deleteTexture(gl, image);
}

static int rt_renderUpdateTexture(void* uptr, int image, int x, int y, int w, int h, const unsigned char* data)
{
	RTVGcontext* gl = (RTVGcontext*)uptr;
	RTVGtexture* tex = nvg_findTexture(gl, image);

	if (tex == NULL) return 0;
	nvg_bindTexture(gl, tex->tex);

	glPixelStorei(GL_UNPACK_ALIGNMENT,1);

	glPixelStorei(GL_UNPACK_ROW_LENGTH, tex->width);
	glPixelStorei(GL_UNPACK_SKIP_PIXELS, x);
	glPixelStorei(GL_UNPACK_SKIP_ROWS, y);

	if (tex->type == NVG_TEXTURE_RGBA)
		glTexSubImage2D(GL_TEXTURE_2D, 0, x,y, w,h, GL_RGBA, GL_UNSIGNED_BYTE, data);
	else
		glTexSubImage2D(GL_TEXTURE_2D, 0, x,y, w,h, GL_RED, GL_UNSIGNED_BYTE, data);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
	glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);

	nvg_bindTexture(gl, 0);

	return 1;
}

static int rt_renderGetTextureSize(void* uptr, int image, int* w, int* h)
{
	RTVGcontext* gl = (RTVGcontext*)uptr;
	RTVGtexture* tex = nvg_findTexture(gl, image);
	if (tex == NULL) return 0;
	*w = tex->width;
	*h = tex->height;
	return 1;
}

[[deprecated]]
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

[[deprecated]]
static NVGcolor nvg_premulColor(NVGcolor c)
{
	c.r *= c.a;
	c.g *= c.a;
	c.b *= c.a;
	return c;
}

static int nvg_convertPaint(RTVGcontext* gl, RTVGfragUniforms* frag, NVGpaint* paint, NVGscissor* scissor, float width, float fringe, float strokeThr)
{
	RTVGtexture* tex = NULL;
	float invxform[6];

	memset(frag, 0, sizeof(*frag));

	frag->innerCol = nvg_premulColor(paint->innerColor);
	frag->outerCol = nvg_premulColor(paint->outerColor);

	if (scissor->extent[0] < -0.5f || scissor->extent[1] < -0.5f) {
		memset(frag->scissorMat, 0, sizeof(frag->scissorMat));
		frag->scissorExt[0] = 1.0f;
		frag->scissorExt[1] = 1.0f;
		frag->scissorScale[0] = 1.0f;
		frag->scissorScale[1] = 1.0f;
	} else {
		nvgTransformInverse(invxform, scissor->xform);
		nvg_xformToMat3x4(frag->scissorMat, invxform);
		frag->scissorExt[0] = scissor->extent[0];
		frag->scissorExt[1] = scissor->extent[1];
		frag->scissorScale[0] = sqrtf(scissor->xform[0]*scissor->xform[0] + scissor->xform[2]*scissor->xform[2]) / fringe;
		frag->scissorScale[1] = sqrtf(scissor->xform[1]*scissor->xform[1] + scissor->xform[3]*scissor->xform[3]) / fringe;
	}

	memcpy(frag->extent, paint->extent, sizeof(frag->extent));
	frag->strokeMult = (width*0.5f + fringe*0.5f) / fringe;
	frag->strokeThr = strokeThr;

	if (paint->image != 0) {
		tex = nvg_findTexture(gl, paint->image);
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
		frag->type = NSVG_SHADER_FILLIMG;

		if (tex->type == NVG_TEXTURE_RGBA)
			frag->texType = (tex->flags & NVG_IMAGE_PREMULTIPLIED) ? 0 : 1;
		else
			frag->texType = 2;

//		printf("frag->texType = %d\n", frag->texType);
	} else {
		frag->type = NSVG_SHADER_FILLGRAD;
		frag->radius = paint->radius;
		frag->feather = paint->feather;
		nvgTransformInverse(invxform, paint->xform);
	}

	nvg_xformToMat3x4(frag->paintMat, invxform);

	return 1;
}

[[deprecated]]
static RTVGfragUniforms* nvg_fragUniformPtr(RTVGcontext* gl, int i);

static void nvg_setUniforms(RTVGcontext* gl, int uniformOffset, int image)
{
	RTVGtexture* tex = NULL;
	glBindBufferRange(GL_UNIFORM_BUFFER, RTVG_FRAG_BINDING, gl->fragBuf, uniformOffset, sizeof(RTVGfragUniforms));

	if (image != 0) {
		tex = nvg_findTexture(gl, image);
	}
	// If no image is set, use empty texture
	if (tex == NULL) {
		tex = nvg_findTexture(gl, gl->dummyTex);
	}
	nvg_bindTexture(gl, tex != NULL ? tex->tex : 0);
	nvg_checkError(gl, "tex paint tex");
}

static void rt_renderViewport(void* uptr, float width, float height, float devicePixelRatio)
{
	NVG_NOTUSED(devicePixelRatio);
	RTVGcontext* gl = (RTVGcontext*)uptr;
	gl->view[0] = width;
	gl->view[1] = height;
}

static void nvg_fill(RTVGcontext* gl, RTVGcall* call)
{
	RTVGpath* paths = &gl->paths[call->pathOffset];
	int i, npaths = call->pathCount;

	// Draw shapes
	glEnable(GL_STENCIL_TEST);
	nvg_stencilMask(gl, 0xff);
	nvg_stencilFunc(gl, GL_ALWAYS, 0, 0xff);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	// set bindpoint for solid loc
	nvg_setUniforms(gl, call->uniformOffset, 0);
	nvg_checkError(gl, "fill simple");

	glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_KEEP, GL_INCR_WRAP);
	glStencilOpSeparate(GL_BACK, GL_KEEP, GL_KEEP, GL_DECR_WRAP);
	glDisable(GL_CULL_FACE);
	for (i = 0; i < npaths; i++)
		glDrawArrays(GL_TRIANGLE_FAN, paths[i].fillOffset, paths[i].fillCount);
	glEnable(GL_CULL_FACE);

	// Draw anti-aliased pixels
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

	nvg_setUniforms(gl, call->uniformOffset + gl->fragSize, call->image);
	nvg_checkError(gl, "fill fill");

	if (gl->flags & NVG_ANTIALIAS) {
		nvg_stencilFunc(gl, GL_EQUAL, 0x00, 0xff);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		// Draw fringes
		for (i = 0; i < npaths; i++)
			glDrawArrays(GL_TRIANGLE_STRIP, paths[i].strokeOffset, paths[i].strokeCount);
	}

	// Draw fill
	nvg_stencilFunc(gl, GL_NOTEQUAL, 0x0, 0xff);
	glStencilOp(GL_ZERO, GL_ZERO, GL_ZERO);
	glDrawArrays(GL_TRIANGLE_STRIP, call->triangleOffset, call->triangleCount);

	glDisable(GL_STENCIL_TEST);
}

static void nvg_convexFill(RTVGcontext* gl, RTVGcall* call)
{
	RTVGpath* paths = &gl->paths[call->pathOffset];
	int i, npaths = call->pathCount;

	nvg_setUniforms(gl, call->uniformOffset, call->image);
	nvg_checkError(gl, "convex fill");

	for (i = 0; i < npaths; i++) {
		glDrawArrays(GL_TRIANGLE_FAN, paths[i].fillOffset, paths[i].fillCount);
		// Draw fringes
		if (paths[i].strokeCount > 0) {
			glDrawArrays(GL_TRIANGLE_STRIP, paths[i].strokeOffset, paths[i].strokeCount);
		}
	}
}

static void nvg_stroke(RTVGcontext* gl, RTVGcall* call)
{
	RTVGpath* paths = &gl->paths[call->pathOffset];
	int npaths = call->pathCount, i;

	if (gl->flags & NVG_STENCIL_STROKES) {

		glEnable(GL_STENCIL_TEST);
		nvg_stencilMask(gl, 0xff);

		// Fill the stroke base without overlap
		nvg_stencilFunc(gl, GL_EQUAL, 0x0, 0xff);
		glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
		nvg_setUniforms(gl, call->uniformOffset + gl->fragSize, call->image);
		nvg_checkError(gl, "stroke fill 0");
		for (i = 0; i < npaths; i++)
			glDrawArrays(GL_TRIANGLE_STRIP, paths[i].strokeOffset, paths[i].strokeCount);

		// Draw anti-aliased pixels.
		nvg_setUniforms(gl, call->uniformOffset, call->image);
		nvg_stencilFunc(gl, GL_EQUAL, 0x00, 0xff);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		for (i = 0; i < npaths; i++)
			glDrawArrays(GL_TRIANGLE_STRIP, paths[i].strokeOffset, paths[i].strokeCount);

		// Clear stencil buffer.
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		nvg_stencilFunc(gl, GL_ALWAYS, 0x0, 0xff);
		glStencilOp(GL_ZERO, GL_ZERO, GL_ZERO);
		nvg_checkError(gl, "stroke fill 1");
		for (i = 0; i < npaths; i++)
			glDrawArrays(GL_TRIANGLE_STRIP, paths[i].strokeOffset, paths[i].strokeCount);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

		glDisable(GL_STENCIL_TEST);

//		nvg_convertPaint(gl, nvg_fragUniformPtr(gl, call->uniformOffset + gl->fragSize), paint, scissor, strokeWidth, fringe, 1.0f - 0.5f/255.0f);

	} else {
		nvg_setUniforms(gl, call->uniformOffset, call->image);
		nvg_checkError(gl, "stroke fill");
		// Draw Strokes
		for (i = 0; i < npaths; i++)
			glDrawArrays(GL_TRIANGLE_STRIP, paths[i].strokeOffset, paths[i].strokeCount);
	}
}

[[deprecated]]
static void nvg_triangles(RTVGcontext* gl, RTVGcall* call)
{
	nvg_setUniforms(gl, call->uniformOffset, call->image);
	nvg_checkError(gl, "triangles fill");

	glDrawArrays(GL_TRIANGLES, call->triangleOffset, call->triangleCount);
}

static void rt_renderCancel(void* uptr) {
	RTVGcontext* gl = (RTVGcontext*)uptr;
	gl->nverts = 0;
	gl->npaths = 0;
	gl->ncalls = 0;
	gl->nuniforms = 0;
}

static GLenum RTVG_convertBlendFuncFactor(int factor)
{
	if (factor == NVG_ZERO)
		return GL_ZERO;
	if (factor == NVG_ONE)
		return GL_ONE;
	if (factor == NVG_SRC_COLOR)
		return GL_SRC_COLOR;
	if (factor == NVG_ONE_MINUS_SRC_COLOR)
		return GL_ONE_MINUS_SRC_COLOR;
	if (factor == NVG_DST_COLOR)
		return GL_DST_COLOR;
	if (factor == NVG_ONE_MINUS_DST_COLOR)
		return GL_ONE_MINUS_DST_COLOR;
	if (factor == NVG_SRC_ALPHA)
		return GL_SRC_ALPHA;
	if (factor == NVG_ONE_MINUS_SRC_ALPHA)
		return GL_ONE_MINUS_SRC_ALPHA;
	if (factor == NVG_DST_ALPHA)
		return GL_DST_ALPHA;
	if (factor == NVG_ONE_MINUS_DST_ALPHA)
		return GL_ONE_MINUS_DST_ALPHA;
	if (factor == NVG_SRC_ALPHA_SATURATE)
		return GL_SRC_ALPHA_SATURATE;
	return GL_INVALID_ENUM;
}

static RTVGblend nvg_blendCompositeOperation(NVGcompositeOperationState op)
{
	RTVGblend blend;
	blend.srcRGB = RTVG_convertBlendFuncFactor(op.srcRGB);
	blend.dstRGB = RTVG_convertBlendFuncFactor(op.dstRGB);
	blend.srcAlpha = RTVG_convertBlendFuncFactor(op.srcAlpha);
	blend.dstAlpha = RTVG_convertBlendFuncFactor(op.dstAlpha);
	if (blend.srcRGB == GL_INVALID_ENUM || blend.dstRGB == GL_INVALID_ENUM || blend.srcAlpha == GL_INVALID_ENUM || blend.dstAlpha == GL_INVALID_ENUM)
	{
		blend.srcRGB = GL_ONE;
		blend.dstRGB = GL_ONE_MINUS_SRC_ALPHA;
		blend.srcAlpha = GL_ONE;
		blend.dstAlpha = GL_ONE_MINUS_SRC_ALPHA;
	}
	return blend;
}

static void rt_renderFlush(void* uptr)
{
	RTVGcontext* gl = (RTVGcontext*)uptr;
	int i;

	if (gl->ncalls > 0) {

		// Setup require GL state.
		glUseProgram(gl->shader.prog);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CCW);
		glEnable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_SCISSOR_TEST);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glStencilMask(0xffffffff);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		glStencilFunc(GL_ALWAYS, 0, 0xffffffff);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
		#if NANOVG_GL_USE_STATE_FILTER
		gl->boundTexture = 0;
		gl->stencilMask = 0xffffffff;
		gl->stencilFunc = GL_ALWAYS;
		gl->stencilFuncRef = 0;
		gl->stencilFuncMask = 0xffffffff;
		gl->blendFunc.srcRGB = GL_INVALID_ENUM;
		gl->blendFunc.srcAlpha = GL_INVALID_ENUM;
		gl->blendFunc.dstRGB = GL_INVALID_ENUM;
		gl->blendFunc.dstAlpha = GL_INVALID_ENUM;
		#endif

		// Upload ubo for frag shaders
		glBindBuffer(GL_UNIFORM_BUFFER, gl->fragBuf);
		glBufferData(GL_UNIFORM_BUFFER, gl->nuniforms * gl->fragSize, gl->uniforms, GL_STREAM_DRAW);

		// Upload vertex data
		glBindVertexArray(gl->vertArr);
		glBindBuffer(GL_ARRAY_BUFFER, gl->vertBuf);
		glBufferData(GL_ARRAY_BUFFER, gl->nverts * sizeof(NVGvertex), gl->verts, GL_STREAM_DRAW);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(NVGvertex), (const GLvoid*)(size_t)0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(NVGvertex), (const GLvoid*)(0 + 2*sizeof(float)));

		// Set view and texture just once per frame.
		glUniform1i(gl->shader.loc[RTVG_LOC_TEX], 0);
		glUniform2fv(gl->shader.loc[RTVG_LOC_VIEWSIZE], 1, gl->view);

		glBindBuffer(GL_UNIFORM_BUFFER, gl->fragBuf);

		for (i = 0; i < gl->ncalls; i++) {
			RTVGcall* call = &gl->calls[i];
			nvg_blendFuncSeparate(gl,&call->blendFunc);
			if (call->type == RTVG_FILL)
				nvg_fill(gl, call);
			else if (call->type == RTVG_CONVEXFILL)
				nvg_convexFill(gl, call);
			else if (call->type == RTVG_STROKE)
				nvg_stroke(gl, call);
			else if (call->type == RTVG_TRIANGLES)
				nvg_triangles(gl, call);
		}

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glBindVertexArray(0);
		glDisable(GL_CULL_FACE);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		glUseProgram(0);
		nvg_bindTexture(gl, 0);
	}

	// Reset calls
	gl->nverts = 0;
	gl->npaths = 0;
	gl->ncalls = 0;
	gl->nuniforms = 0;
}

static int nvg_maxVertCount(const NVGpath* paths, int npaths)
{
	int i, count = 0;
	for (i = 0; i < npaths; i++) {
		count += paths[i].nfill;
		count += paths[i].nstroke;
	}
	return count;
}

static RTVGcall* nvg_allocCall(RTVGcontext* gl)
{
	RTVGcall* ret = NULL;
	if (gl->ncalls+1 > gl->ccalls) {
		RTVGcall* calls;
		int ccalls = nvg_maxi(gl->ncalls+1, 128) + gl->ccalls/2; // 1.5x Overallocate
		calls = (RTVGcall*)realloc(gl->calls, sizeof(RTVGcall) * ccalls);
		if (calls == NULL) return NULL;
		gl->calls = calls;
		gl->ccalls = ccalls;
	}
	ret = &gl->calls[gl->ncalls++];
	memset(ret, 0, sizeof(RTVGcall));
	return ret;
}

static int nvg_allocPaths(RTVGcontext* gl, int n)
{
	int ret = 0;
	if (gl->npaths+n > gl->cpaths) {
		RTVGpath* paths;
		int cpaths = nvg_maxi(gl->npaths + n, 128) + gl->cpaths/2; // 1.5x Overallocate
		paths = (RTVGpath*)realloc(gl->paths, sizeof(RTVGpath) * cpaths);
		if (paths == NULL) return -1;
		gl->paths = paths;
		gl->cpaths = cpaths;
	}
	ret = gl->npaths;
	gl->npaths += n;
	return ret;
}

static int nvg_allocVerts(RTVGcontext* gl, int n)
{
	int ret = 0;
	if (gl->nverts+n > gl->cverts) {
		NVGvertex* verts;
		int cverts = nvg_maxi(gl->nverts + n, 4096) + gl->cverts/2; // 1.5x Overallocate
		verts = (NVGvertex*)realloc(gl->verts, sizeof(NVGvertex) * cverts);
		if (verts == NULL) return -1;
		gl->verts = verts;
		gl->cverts = cverts;
	}
	ret = gl->nverts;
	gl->nverts += n;
	return ret;
}

static int nvg_allocFragUniforms(RTVGcontext* gl, int n)
{
	int ret = 0, structSize = gl->fragSize;
	if (gl->nuniforms+n > gl->cuniforms) {
		unsigned char* uniforms;
		int cuniforms = nvg_maxi(gl->nuniforms+n, 128) + gl->cuniforms/2; // 1.5x Overallocate
		uniforms = (unsigned char*)realloc(gl->uniforms, structSize * cuniforms);
		if (uniforms == NULL) return -1;
		gl->uniforms = uniforms;
		gl->cuniforms = cuniforms;
	}
	ret = gl->nuniforms * structSize;
	gl->nuniforms += n;
	return ret;
}

static RTVGfragUniforms* nvg_fragUniformPtr(RTVGcontext* gl, int i)
{
	return (RTVGfragUniforms*)&gl->uniforms[i];
}

static void nvg_vset(NVGvertex* vtx, float x, float y, float u, float v)
{
	vtx->x = x;
	vtx->y = y;
	vtx->u = u;
	vtx->v = v;
}

static void rt_renderFill(void* uptr, NVGpaint* paint, NVGcompositeOperationState compositeOperation, NVGscissor* scissor, float fringe,
							  const float* bounds, const NVGpath* paths, int npaths)
{
	RTVGcontext* gl = (RTVGcontext*)uptr;
	RTVGcall* call = nvg_allocCall(gl);
	NVGvertex* quad;
	RTVGfragUniforms* frag;
	int i, maxverts, offset;

	if (call == NULL) return;

	call->type = RTVG_FILL;
	call->triangleCount = 4;
	call->pathOffset = nvg_allocPaths(gl, npaths);
	if (call->pathOffset == -1) goto error;
	call->pathCount = npaths;
	call->image = paint->image;
	call->blendFunc = nvg_blendCompositeOperation(compositeOperation);

	if (npaths == 1 && paths[0].convex)
	{
		call->type = RTVG_CONVEXFILL;
		call->triangleCount = 0;	// Bounding box fill quad not needed for convex fill
	}

	// Allocate vertices for all the paths.
	maxverts = nvg_maxVertCount(paths, npaths) + call->triangleCount;
	offset = nvg_allocVerts(gl, maxverts);
	if (offset == -1) goto error;

	for (i = 0; i < npaths; i++) {
		RTVGpath* copy = &gl->paths[call->pathOffset + i];
		const NVGpath* path = &paths[i];
		memset(copy, 0, sizeof(RTVGpath));
		if (path->nfill > 0) {
			copy->fillOffset = offset;
			copy->fillCount = path->nfill;
			memcpy(&gl->verts[offset], path->fill, sizeof(NVGvertex) * path->nfill);
			offset += path->nfill;
		}
		if (path->nstroke > 0) {
			copy->strokeOffset = offset;
			copy->strokeCount = path->nstroke;
			memcpy(&gl->verts[offset], path->stroke, sizeof(NVGvertex) * path->nstroke);
			offset += path->nstroke;
		}
	}

	// Setup uniforms for draw calls
	if (call->type == RTVG_FILL) {
		// Quad
		call->triangleOffset = offset;
		quad = &gl->verts[call->triangleOffset];
		nvg_vset(&quad[0], bounds[2], bounds[3], 0.5f, 1.0f);
		nvg_vset(&quad[1], bounds[2], bounds[1], 0.5f, 1.0f);
		nvg_vset(&quad[2], bounds[0], bounds[3], 0.5f, 1.0f);
		nvg_vset(&quad[3], bounds[0], bounds[1], 0.5f, 1.0f);

		call->uniformOffset = nvg_allocFragUniforms(gl, 2);
		if (call->uniformOffset == -1) goto error;
		// Simple shader for stencil
		frag = nvg_fragUniformPtr(gl, call->uniformOffset);
		memset(frag, 0, sizeof(*frag));
		frag->strokeThr = -1.0f;
		frag->type = NSVG_SHADER_SIMPLE;
		// Fill shader
		nvg_convertPaint(gl, nvg_fragUniformPtr(gl, call->uniformOffset + gl->fragSize), paint, scissor, fringe, fringe, -1.0f);
	} else {
		call->uniformOffset = nvg_allocFragUniforms(gl, 1);
		if (call->uniformOffset == -1) goto error;
		// Fill shader
		nvg_convertPaint(gl, nvg_fragUniformPtr(gl, call->uniformOffset), paint, scissor, fringe, fringe, -1.0f);
	}

	return;

error:
	// We get here if call alloc was ok, but something else is not.
	// Roll back the last call to prevent drawing it.
	if (gl->ncalls > 0) gl->ncalls--;
}

static void rt_renderStroke(void* uptr, NVGpaint* paint, NVGcompositeOperationState compositeOperation, NVGscissor* scissor, float fringe,
								float strokeWidth, const NVGpath* paths, int npaths)
{
	RTVGcontext* gl = (RTVGcontext*)uptr;
	RTVGcall* call = nvg_allocCall(gl);
	int i, maxverts, offset;

	if (call == NULL) return;

	call->type = RTVG_STROKE;
	call->pathOffset = nvg_allocPaths(gl, npaths);
	if (call->pathOffset == -1) goto error;
	call->pathCount = npaths;
	call->image = paint->image;
	call->blendFunc = nvg_blendCompositeOperation(compositeOperation);

	// Allocate vertices for all the paths.
	maxverts = nvg_maxVertCount(paths, npaths);
	offset = nvg_allocVerts(gl, maxverts);
	if (offset == -1) goto error;

	for (i = 0; i < npaths; i++) {
		RTVGpath* copy = &gl->paths[call->pathOffset + i];
		const NVGpath* path = &paths[i];
		memset(copy, 0, sizeof(RTVGpath));
		if (path->nstroke) {
			copy->strokeOffset = offset;
			copy->strokeCount = path->nstroke;
			memcpy(&gl->verts[offset], path->stroke, sizeof(NVGvertex) * path->nstroke);
			offset += path->nstroke;
		}
	}

	if (gl->flags & NVG_STENCIL_STROKES) {
		// Fill shader
		call->uniformOffset = nvg_allocFragUniforms(gl, 2);
		if (call->uniformOffset == -1) goto error;

		nvg_convertPaint(gl, nvg_fragUniformPtr(gl, call->uniformOffset), paint, scissor, strokeWidth, fringe, -1.0f);
		nvg_convertPaint(gl, nvg_fragUniformPtr(gl, call->uniformOffset + gl->fragSize), paint, scissor, strokeWidth, fringe, 1.0f - 0.5f/255.0f);

	} else {
		// Fill shader
		call->uniformOffset = nvg_allocFragUniforms(gl, 1);
		if (call->uniformOffset == -1) goto error;
		nvg_convertPaint(gl, nvg_fragUniformPtr(gl, call->uniformOffset), paint, scissor, strokeWidth, fringe, -1.0f);
	}

	return;

error:
	// We get here if call alloc was ok, but something else is not.
	// Roll back the last call to prevent drawing it.
	if (gl->ncalls > 0) gl->ncalls--;
}

static void rt_renderTriangles(void* uptr, NVGpaint* paint, NVGcompositeOperationState compositeOperation, NVGscissor* scissor,
								   const NVGvertex* verts, int nverts, float fringe)
{
	RTVGcontext* gl = (RTVGcontext*)uptr;
	RTVGcall* call = nvg_allocCall(gl);
	RTVGfragUniforms* frag;

	if (call == NULL) return;

	call->type = RTVG_TRIANGLES;
	call->image = paint->image;
	call->blendFunc = nvg_blendCompositeOperation(compositeOperation);

	// Allocate vertices for all the paths.
	call->triangleOffset = nvg_allocVerts(gl, nverts);
	if (call->triangleOffset == -1) goto error;
	call->triangleCount = nverts;

	memcpy(&gl->verts[call->triangleOffset], verts, sizeof(NVGvertex) * nverts);

	// Fill shader
	call->uniformOffset = nvg_allocFragUniforms(gl, 1);
	if (call->uniformOffset == -1) goto error;
	frag = nvg_fragUniformPtr(gl, call->uniformOffset);
	nvg_convertPaint(gl, frag, paint, scissor, 1.0f, fringe, -1.0f);
	frag->type = NSVG_SHADER_IMG;

	return;

error:
	// We get here if call alloc was ok, but something else is not.
	// Roll back the last call to prevent drawing it.
	if (gl->ncalls > 0) gl->ncalls--;
}

static void rt_renderDelete(void* uptr)
{
	RTVGcontext* gl = (RTVGcontext*)uptr;
	int i;
	if (gl == NULL) return;

	nvg_deleteShader(&gl->shader);

	if (gl->fragBuf != 0)
		glDeleteBuffers(1, &gl->fragBuf);
	if (gl->vertArr != 0)
		glDeleteVertexArrays(1, &gl->vertArr);
	if (gl->vertBuf != 0)
		glDeleteBuffers(1, &gl->vertBuf);

	for (i = 0; i < gl->ntextures; i++) {
		if (gl->textures[i].tex != 0 && (gl->textures[i].flags & NVG_IMAGE_NODELETE) == 0)
			glDeleteTextures(1, &gl->textures[i].tex);
	}
	free(gl->textures);

	free(gl->paths);
	free(gl->verts);
	free(gl->uniforms);
	free(gl->calls);

	free(gl);
}

NVGcontext* nvgCreateGL3(int flags)
{
	NVGparams params;
	NVGcontext* ctx = NULL;
	RTVGcontext* gl = (RTVGcontext*)malloc(sizeof(RTVGcontext));
	if (gl == NULL) goto error;
	memset(gl, 0, sizeof(RTVGcontext));

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
	params.userPtr = gl;
	params.edgeAntiAlias = flags & NVG_ANTIALIAS ? 1 : 0;

	gl->flags = flags;

	ctx = nvgCreateInternal(&params);
	if (ctx == NULL) goto error;

	return ctx;

error:
	// 'gl' is freed by nvgDeleteInternal.
	if (ctx != NULL) nvgDeleteInternal(ctx);
	return NULL;
}

void nvgDeleteGL3(NVGcontext* ctx)
{
	nvgDeleteInternal(ctx);
}

int nvglCreateImageFromHandleGL3(NVGcontext* ctx, GLuint textureId, int w, int h, int imageFlags)
{
	RTVGcontext* gl = (RTVGcontext*)nvgInternalParams(ctx)->userPtr;
	RTVGtexture* tex = nvg_allocTexture(gl);

	if (tex == NULL) return 0;

	tex->type = NVG_TEXTURE_RGBA;
	tex->tex = textureId;
	tex->flags = imageFlags;
	tex->width = w;
	tex->height = h;

	return tex->id;
}

GLuint nvglImageHandleGL3(NVGcontext* ctx, int image)
{
	RTVGcontext* gl = (RTVGcontext*)nvgInternalParams(ctx)->userPtr;
	RTVGtexture* tex = nvg_findTexture(gl, image);
	return tex->tex;
}

#endif /* NANOVG_GL_IMPLEMENTATION */
