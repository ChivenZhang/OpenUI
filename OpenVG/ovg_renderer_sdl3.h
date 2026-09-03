#pragma once

#include <cstdint>
#include <queue>
#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <queue>
#include <vector>
#include "ovg.h"
// Forward declarations
struct ovg_device_t;
struct ovg_ctx_t;
struct sdl3gpu_texture;  // 内部纹理包装器

struct ovg_sdl3_ctx {
	SDL_Window* window;
	SDL_GPUDevice* device; 
};

// FBO 结构体 - SDL3 GPU 版本
struct vg_fbo_t {
	uint32_t width, height;
	glm::ivec2 display_size;
	ovg_ctx_t* ctx = 0;
	SDL_Window* window;
	SDL_GPUTexture* swapchain;			// 窗口主颜色纹理
	sdl3gpu_texture* colorTex;			// 主颜色纹理
	sdl3gpu_texture* colorTexMS;		// MSAA 解析前纹理（可选）
	sdl3gpu_texture* depthStencilTex;	// 深度+模板纹理
	SDL_GPUCommandBuffer* cmd;
	bool hasStencil;// 是否有模板附件
};


// 几何信息结构体
struct gem_info_t0 {
	uint32_t topology : 5;   // VK_PRIMITIVE_TOPOLOGY_*
	uint32_t polygon : 2;   // VK_POLYGON_MODE_*
	uint32_t frontFace : 1;   // 0=CW, 1=CCW
	uint32_t shader : 3;   // shader 索引
	uint32_t blendMode : 4;   // blendMode_e
	uint32_t flags : 8;   // depth_stencil_State 标志
	float    lineWidth;
};

// 管道状态（SDL3 GPU 版本）
struct pipelinestate_p {
	SDL_GPUGraphicsPipeline* pipeline;
	SDL_GPUSampler* sampler; 
	gem_info_t state = {};
};

bool vg_sdl3_init(ovg_sdl3_ctx* g, int width, int height,bool is_vulkan);
// 设备创建与销毁
ovg_device_t* new_sdl3gpu_device(SDL_GPUDevice* gpuDevice);
void          free_sdl3gpu_device(ovg_device_t* dev);
// 渲染上下文创建与销毁
ovg_ctx_t* new_ovgctx_sdl3(ovg_device_t* dev, SDL_GPUTextureFormat colorFormat, SDL_GPUTextureFormat depthFormat, SDL_GPUSampleCount samples);
void        free_ovgctx_sdl3(ovg_ctx_t* ctx);

// FBO 管理
vg_fbo_t    new_vgfbo_sdl3(ovg_ctx_t* ctx, int width, int height, SDL_Window* window = nullptr);
void        free_vgfbo_sdl3(vg_fbo_t* fbo);

SDL_GPUCommandBuffer* ovg_get_window_swapchain(ovg_ctx_t* ctx, vg_fbo_t* fbo);
// 绘制入口 
void ovg_draw_data(ovg_ctx_t* ctx, vg_fbo_t* fbo, ovg_draw_data_t* data, size_t count);

void ovg_wait_idle(ovg_ctx_t* ctx);
sdl3gpu_texture* new_texture_def(ovg_ctx_t* ctx, int w, int h, vg_format_t format);
