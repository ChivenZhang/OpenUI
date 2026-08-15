#pragma once
/*
 * vg_renderer.c
 *
 * SDL3 GPU vector-graphics renderer built around vg.slang.h
 *
 *  Supports: SOLID / SURFACE / LINEAR / RADIAL / SWEEP paints
 *  Uses:    SDL_PushGPUVertexUniformData  (push constants = UBO + PushConsts)
 *
 *  Build (Linux, Vulkan backend):
 *      glslangValidator -V vg.vert -o vg.vert.spv
 *      glslangValidator -V vg.frag -o vg.frag.spv
 *      gcc vg_renderer.c -o vg_renderer $(sdl3-config --cflags --libs) -lvulkan
 *
 *  Run:  place vg.vert.spv / vg.frag.spv next to the executable.
 */

#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <queue>
#include <vector>
 /* ═══════════════════════════════════════════════════════════════
  *  SDL3 GPU resource-binding quick reference (SPIR-V)
  *
  *   Vertex shader
  *       set 0 : sampled textures, storage textures, storage buffers
  *       set 1 : uniform buffers   ← uboGrad (vertex) + PushConsts
  *
  *   Fragment shader
  *       set 2 : sampled textures, storage textures, storage buffers
  *       set 3 : uniform buffers   ← uboGrad (fragment) + PushConsts
  *
  *   Push constants are uploaded with
  *       SDL_PushGPUVertexUniformData  → vertex   stage (set 1)
  *       SDL_PushGPUFragmentUniformData → fragment stage (set 3)
  *
  * ═══════════════════════════════════════════════════════════════ */

  /* ── 1.  C-side mirror structs (std140 / 16-byte alignment) ───── */

  /* Push constants — must match the GLSL `PushConsts` block.
   *     source   : vec4   (16 B)
   *     size     : vec2   ( 8 B, padded to 16)
   *     fsq_type : int    ( 4 B)
   *     opacity  : float  ( 4 B)
   *     mat      : mat3x2 (24 B, padded to 32)
   *     matInv   : mat3x2 (24 B, padded to 32)
   *   Total = 16 + 16 + 8 + 24 + 24 = 88 B  (rounded to 96 in GLSL)
   *
   * We keep it simple: a single 80-byte block that matches the
   * GLSL layout when packed correctly.  Verify with the static
   * assertion below.
   */
typedef struct PushConsts {
	float source[4];      /* xy = box size, zw = surface tex size */
	float size[2];        /* viewport size */
	int   fullScreenQuad_srcType;
	float opacity;
	float mat[6];         /* 3x2 matrix stored row-major */
	float matInv[6];
} PushConsts;

/* Gradient uniform buffer — must match the GLSL `uboGrad_` block.
 *   colors  : vec4  x32 = 512 B  (16 B aligned)
 *   stops   : float x32 = 128 B
 *   cp      : vec4  x2  =  32 B
 *   m       : ivec4      =  16 B
 *   scale   : vec2       =   8 B
 *   count   : uint       =   4 B
 *   extend  : int        =   4 B
 *   Total = 704 B
 */
#define MAX_STOPS 32
typedef struct UboGrad {
	float    colors[MAX_STOPS][4];
	float    stops[MAX_STOPS];
	float    cp[2][4];
	int      m[4];
	float    scale[2];
	uint32_t count;
	int      extend;
} UboGrad;

struct push_constants_std140_t {
	glm::mat2x3 mat;
	glm::mat2x3 matInv;
	float source[4];              // vec4
	float size[2];                // vec2
	uint32_t fsq_patternType;
	float opacity;
};
struct gpu_cmd_it
{
	push_constants_std140_t u;
	vg_gradient_t grad;
	Uint32 num_vertices;
	Uint32 num_instances;
	Uint32 first_vertex;
	Uint32 first_instance;
};
/* ── 3.  Global renderer state ────────────────────────────────── */
typedef struct VGState {
	SDL_Window* window;
	SDL_GPUDevice* device;
	SDL_GPUGraphicsPipeline* pipeline;
	SDL_GPUBuffer* vertexBuffer;
	SDL_GPUSampler* linearSampler;
	uint32_t cap_v = 0;
	/* CPU-side data mirrors */
	UboGrad                ubo;
	int width = 0, height = 0;
	SDL_GPUTexture* emptyImg;
	SDL_GPUSampler* sampler;
	SDL_GPURenderPass* pass;
	SDL_GPUCommandBuffer* cmd;
	std::queue<SDL_GPUTransferBuffer*> rq;
	std::vector<ovgVertex> vs;
	std::vector<gpu_cmd_it> cs;
	ovg_draw_data* data;
} VGState;


/* ── 4.  Load a pre-compiled SPIR-V file ─────────────────────── */
void* LoadSPIRV(const char* path, size_t* outSize);
/* ── 5.  Create an SDL_GPUShader from a .spv file ────────────── */
SDL_GPUShader* CreateShader(VGState* g, const char* spvPath, SDL_GPUShaderStage stage,
	Uint32 numSamplers, Uint32 numStorageTex, Uint32 numStorageBuf, Uint32 numUniformBuf);

/* ── 6.  Init ─────────────────────────────────────────────────── */
bool VG_Init(VGState* g, int width, int height);

/* ── 7.  Push uniforms for the current draw ─────────────────────── */
/*
 *  Binding map (matches the GLSL shaders):
 *
 *  Vertex stage
 *    set=1, binding=0 : uboGrad  → SDL_PushGPUVertexUniformData, slot 0
 *    push constants    : (auto)    → SDL_PushGPUVertexUniformData, slot 0
 *                                  BUT SDL3 GPU treats push constants
 *                                  separately, so we use the same call
 *                                  with the PushConsts struct.
 *
 *  Fragment stage
 *    set=3, binding=0 : uboGrad  → SDL_PushGPUFragmentUniformData, slot 0
 *    set=2, binding=0 : sourceTex → SDL_BindGPUFragmentSamplers
 *    push constants    : (auto)    → SDL_PushGPUFragmentUniformData
 *
 *  Since both push constants and uboGrad live in the same uniform
 *  slot range, we push them sequentially.  SDL3 GPU internally
 *  allocates separate buffer regions, so this is safe.
 */
void VG_PushDrawUniforms(VGState* g, SDL_GPUCommandBuffer* cmd, float vpW, float vpH, int patType, float opacity);
/* ── 8.  Draw a filled rectangle (2 triangles, 6 vertices) ──────── */
#define PAT_SOLID  0
#define PAT_SURFACE 1
#define PAT_LINEAR 2
#define PAT_RADIAL 3
#define PAT_MESH   4
#define PAT_RASTER 5
#define PAT_SWEEP  6

#define STENCIL_FILL_BIT              0x1
#define STENCIL_CLIP_BIT              0x2
#define STENCIL_ALL_BIT               0x3

void VG_DrawFilledRect(VGState* g, float x, float y, float w, float h, int patType, float opacity, uint32_t c);
void VG_RenderFrame(VGState* g, ovg_draw_data* data);
