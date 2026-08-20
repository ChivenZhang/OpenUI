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

struct VGState {
	SDL_Window* window;
	SDL_GPUDevice* device;
	/* CPU-side data mirrors */
	int width = 0, height = 0;
	SDL_GPUTexture* emptyImg;
	SDL_GPUSampler* sampler;
	SDL_GPURenderPass* pass;
	SDL_GPUCommandBuffer* cmd;
	std::queue<SDL_GPUTransferBuffer*> rq;

	SDL_GPUTexture* msaaColor = nullptr;
	SDL_GPUTexture* msaaDepth = nullptr;

	int msaaWidth = 0;
	int msaaHeight = 0;

	SDL_GPUSampleCount msaaSampleCount = SDL_GPU_SAMPLECOUNT_4;
	ovg_draw_data_t* data;
};

// FBO 结构体 - SDL3 GPU 版本
struct vg_fbo_t {
	uint32_t width, height;
	glm::ivec2 display_size;
	ovg_ctx_t* ctx = 0;
	SDL_Window* window;
	SDL_GPUTexture* color;				// 窗口主颜色纹理
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

	gem_info_t              state = {};
};

bool VG_Init(VGState* g, int width, int height);
// 设备创建与销毁
ovg_device_t* new_sdl3gpu_device(SDL_GPUDevice* gpuDevice);
void          free_sdl3gpu_device(ovg_device_t* dev);

// 渲染上下文创建与销毁
ovg_ctx_t* new_ovgctx_sdl3(ovg_device_t* dev, SDL_GPUTextureFormat colorFormat, SDL_GPUTextureFormat depthFormat, SDL_GPUSampleCount samples);
void        free_ovgctx_sdl3(ovg_ctx_t* ctx);

// FBO 管理
vg_fbo_t    new_vgfbo_sdl3(ovg_ctx_t* ctx, int width, int height, SDL_Window* window = nullptr);
void        free_vgfbo_sdl3(vg_fbo_t* fbo);

void* ovg_get_window_swapchain(ovg_ctx_t* ctx, vg_fbo_t* fbo);
// 绘制入口 
void ovg_draw_data(ovg_ctx_t* ctx, vg_fbo_t* fbo, ovg_draw_data_t* data);

// 获取内部命令缓冲区（用于录制渲染命令）
SDL_GPUCommandBuffer* ovg_get_command_buffer(ovg_ctx_t* ctx);

// 显式 MSAA 解析
void ovg_resolve_msaa_sdl3(ovg_ctx_t* ctx, SDL_GPUCommandBuffer* cmdBuf, vg_fbo_t* fbo);

// ─── 帧生命周期 ───────────────────────────────
SDL_GPUCommandBuffer* ovg_begin_frame(ovg_ctx_t* ctx, vg_fbo_t* fbo, bool clearAll);
void                     ovg_end_frame(ovg_ctx_t* ctx, vg_fbo_t* fbo);

// ─── 获取当前活跃 RenderPass ──────────────────
SDL_GPURenderPass* ovg_get_current_render_pass(ovg_ctx_t* ctx);

// ─── 纹理绑定（替代 Vulkan 描述符集）───
void ovg_bind_texture(ovg_ctx_t* ctx, SDL_GPUCommandBuffer* cmdBuf, SDL_GPURenderPass* pass, sdl3gpu_texture* tex);

// ─── UBO 绑定（渐变数据）─────────────────
void ovg_bind_ubo(ovg_ctx_t* ctx, SDL_GPUCommandBuffer* cmdBuf, SDL_GPURenderPass* pass, uint32_t offset);

// ─── 管道绑定 ───────────────────────────────
void ovg_bind_vg_pipeline(ovg_ctx_t* ctx, SDL_GPUCommandBuffer* cmdBuf, SDL_GPURenderPass* pass, int operatorType);
void ovg_bind_geom_pipeline(ovg_ctx_t* ctx, SDL_GPUCommandBuffer* cmdBuf, SDL_GPURenderPass* pass, const gem_info_t* info);

// ─── 几何缓冲区绑定 ──────────────────────────
void ovg_bind_geom_buffers(ovg_ctx_t* ctx, SDL_GPUCommandBuffer* cmdBuf, SDL_GPURenderPass* pass,
	uint32_t vboOffset, uint32_t iboOffset);

// ─── 视口与裁剪 ─────────────────────────────
void ovg_set_viewport(ovg_ctx_t* ctx, SDL_GPUCommandBuffer* cmdBuf, SDL_GPURenderPass* pass,
	float x, float y, float w, float h);
void ovg_set_scissor(ovg_ctx_t* ctx, SDL_GPUCommandBuffer* cmdBuf, SDL_GPURenderPass* pass,
	int x, int y, int w, int h);
void ovg_reset_scissor(ovg_ctx_t* ctx, SDL_GPUCommandBuffer* cmdBuf, SDL_GPURenderPass* pass);

// ─── 模板辅助 ──────────────────────────────────
// 动态设置 stencil reference（在 RenderPass 内随时可调用）
//   绑定 VG 管道后会自动设置正确的 ref，此函数用于特殊场景：
//   - 多级裁剪（bit2=0x4, bit3=0x8...）
//   - 调试（把 stencil 值画成颜色）
void ovg_set_stencil_reference(ovg_ctx_t* ctx, SDL_GPURenderPass* pass, uint8_t ref);

// ─── 命令提交与同步 ──────────────────────────
void ovg_submit_command_buffer(SDL_GPUCommandBuffer* cmdBuf);
void ovg_wait_idle(ovg_ctx_t* ctx);
