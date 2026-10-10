#pragma once
#include <OpenRT.h>
#include "nanovg.h"

// Create flags for rtCreateRT(). Same bit layout as nanovg_gl.h's NVGcreateFlags,
// named differently so both headers can coexist in one translation unit.
enum NVGcreateFlagsRT
{
    // Flag indicating if geometry based anti-aliasing is used (may not be needed when using MSAA).
    RTVG_ANTIALIAS = 1 << 0,
    // Flag indicating if strokes should be drawn using stencil buffer. The rendering will be a little
    // slower, but path overlaps (i.e. self-intersecting or sharp turns) will be drawn just once.
    RTVG_STENCIL_STROKES = 1 << 1,
    // Flag indicating that additional debug checks are done.
    RTVG_DEBUG = 1 << 2,
};

// Creates a NanoVG context that renders through the OpenRT API.
// Geometry is rasterised into an offscreen RGBA8 colour (+ stencil) target at
// nvgEndFrame(). The backend does not present; fetch the target with
// rtGetTargetRT() and composite it yourself (see blitVertShader/blitFragShader).
NVGcontext* rtCreateRT(int flags);

void rtDeleteRT(NVGcontext* ctx);

// Returns the offscreen colour target of the last nvgEndFrame(). Premultiplied
// alpha, sized in device pixels (width * devicePixelRatio). handle == 0 until the
// first nvgBeginFrame(). The texture is recreated when the frame size changes, so
// re-query it every frame.
rt_texture_t& rtGetTargetRT(NVGcontext* ctx);

// ====================================================================
// Shaders. Binding slots:
//   UBO 0 : fragment paint uniforms (one block per draw call)
//   UBO 1 : view size
//   TEX 2 : paint image / font atlas (+ sampler on the same slot)

constexpr auto fillVertShader =
    "#version 460 core\n"
    "layout(std140, binding = 1) uniform view {\n"
    "	vec2 viewSize;\n"
    "};\n"
    "layout(location = 0) in vec2 vertex;\n"
    "layout(location = 1) in vec2 tcoord;\n"
    "layout(location = 0) out vec2 ftcoord;\n"
    "layout(location = 1) out vec2 fpos;\n"
    "void main(void) {\n"
    "	ftcoord = tcoord;\n"
    "	fpos = vertex;\n"
    "	gl_Position = vec4(2.0*vertex.x/viewSize.x - 1.0, 1.0 - 2.0*vertex.y/viewSize.y, 0, 1);\n"
    "}\n";

// Fragment shader body. The backend prepends "#version 460 core" and, when
// RTVG_ANTIALIAS is set, "#define EDGE_AA 1".
constexpr auto fillFragShaderBody =
    "layout(std140, binding = 0) uniform frag {\n"
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
    "layout(binding = 2) uniform sampler2D tex;\n"
    "layout(location = 0) in vec2 ftcoord;\n"
    "layout(location = 1) in vec2 fpos;\n"
    "layout(location = 0) out vec4 outColor;\n"
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
    "		if (texType == 1) color = vec4(color.xyz*color.w,color.w);\n"
    "		if (texType == 2) color = vec4(color.x);\n"
    "		// Apply color tint and alpha.\n"
    "		color *= innerCol;\n"
    "		// Combine alpha\n"
    "		color *= strokeAlpha * scissor;\n"
    "		result = color;\n"
    "	} else if (type == 2) {		// Stencil fill\n"
    "		result = vec4(1,1,1,1);\n"
    "	} else if (type == 3) {		// Textured tris\n"
    "		vec4 color = texture(tex, ftcoord);\n"
    "		if (texType == 1) color = vec4(color.xyz*color.w,color.w);\n"
    "		if (texType == 2) color = vec4(color.x);\n"
    "		color *= scissor;\n"
    "		result = color * innerCol;\n"
    "	}\n"
    "	outColor = result;\n"
    "}\n";
