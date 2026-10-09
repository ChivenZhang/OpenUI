#pragma once
#include <OpenRT.h>
#include "nanovg.h"
#include <GL/glew.h>

enum NVGcreateFlags
{
    NVG_DEBUG = 1 << 2,
};

NVGcontext* nvgCreateGL3(int flags);

void nvgDeleteGL3(NVGcontext* ctx);

int nvglCreateImageFromHandleGL3(NVGcontext* ctx, GLuint textureId, int w, int h, int flags);

GLuint nvglImageHandleGL3(NVGcontext* ctx, int image);

enum NVGimageFlagsGL
{
    NVG_IMAGE_NODELETE = 1 << 16, // Do not delete GL texture handle.
};

constexpr auto fillVertShader =
    "#version 150 core\n"
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

constexpr auto fillFragShader =
    "#version 150 core\n"
    "#define EDGE_AA 1\n"
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
    "layout(binding = 0) uniform sampler2D tex;\n"
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