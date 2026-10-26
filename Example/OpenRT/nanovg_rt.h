#pragma once
#include "nanovg.h"
#include "OpenRT.h"

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

enum NVGimageFlagsRT {
    RTVG_IMAGE_NODELETE			= 1<<16,	// Do not delete GL texture handle.
};

// Creates a nanovg context that renders through the OpenRT API.
// Geometry is rasterised at nvgEndFrame() into the NVGtarget passed to
// nvgBeginFrame. Colour and stencil pointers are rt_texture_t*. The backend
// does not allocate them or present.
NVGcontext* rtCreateRT(int flags);

void rtDeleteRT(NVGcontext* ctx);

int rtCreateTextureNative(NVGcontext* ctx, int imageFlags, rt_texture_t const& texture);

rt_texture_t rtGetTextureNative(NVGcontext* ctx, int imageHandle);

// Fill shaders: Shader/Fill.slang, compiled under Shader/<backend>/Fill.h.
// Binding slots:
//   UBO 0 : fragment paint uniforms (one block per draw call)
//   UBO 1 : view size
//   TEX 2 : paint image / font atlas (+ sampler on the same slot; WebGPU uses slot 3)
