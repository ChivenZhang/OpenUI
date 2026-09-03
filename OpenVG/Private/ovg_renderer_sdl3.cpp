/*
 * ovg_renderer_sdl3.cpp
 * SDL3 GPU 矢量渲染后端





2026/8/31 后端支持普通三角形渲染


shader规则：
每set的binding排列
 Category 1: read resources
	  samplerCount,
	  storageTextureCount,
	  storageBufferCount,
 Category 2: write resources
	  writeStorageTextureCount,
	  writeStorageBufferCount,
 Category 3: uniform buffers
	  uniformBufferCount

顶点	set=0[num_samplers, num_storageTextures, num_storageBuffers]
	set=1[num_uniformBuffers]
像素	set=2[num_samplers, num_storageTextures, num_storageBuffers]
	set=3[numUniformBuffers]
计算	set=0[num_samplers, num_readonly_storage_textures, num_readonly_storage_buffers]
	set=1[num_readwrite_storage_textures, num_readwrite_storage_buffers]
	set=2[num_uniform_buffers]


 */


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>

#include <array>
#include <map>
#include <unordered_map>
#include <set>
#include <vector>
#include <cstring>
#include <cassert>
#include <cmath>
#include <vulkan/vulkan.h>
#include "ovg_renderer_sdl3.h"
 // ========================================================================
 // 着色器 SPIR-V 数据（extern 声明，链接时从 spv 数组引入）
 // ========================================================================

#include "shaders/spv_c/a_vg.vert.h"
#include "shaders/spv_c/a_vg.frag.h"

#include "shaders/spv_c/a_base3d.vert.h"
#include "shaders/spv_c/a_base3d.frag.h"

#include "shaders/spv_c/a_base3d_mask.vert.h"
#include "shaders/spv_c/a_base3d_mask.frag.h"

#include "shaders/spv_c/a_base3d_dsc.vert.h"
#include "shaders/spv_c/a_base3d_dsc.frag.h"

#include "shaders/spv_c/a_base3d_inst.vert.h"
#include "shaders/spv_c/a_base3d_inst.frag.h"

#include "shaders/spv_c/a_base3d_dsc_inst.vert.h"
#include "shaders/spv_c/a_base3d_dsc_inst.frag.h"


#define FULLSCREEN_BIT         0x10000000
#define SRCTYPE_MASK           0x000000FF
// ========================================================================
// 内部数据结构
// ========================================================================


struct sdl3gpu_texture {
	SDL_GPUTexture* texture = nullptr;
	SDL_GPUSampler* sampler = nullptr;
	SDL_GPUDevice* device = nullptr;
	SDL_GPUTextureFormat format = SDL_GPU_TEXTUREFORMAT_INVALID;
	int                  width = 0;
	int                  height = 0;
	uint32_t             references = 0;
	void* user_ptr = 0;
	uint32_t id = 0;
	bool                 hasStencil = false;
};

struct shadermodule_vf {
	SDL_GPUShader* vert = nullptr;
	SDL_GPUShader* frag = nullptr;
};

struct pipelinestate_p_internal {
	SDL_GPUGraphicsPipeline* pipeline = nullptr;
	SDL_GPUSampler* defaultSampler = nullptr;
	gem_info_t               state = {};
};

// 混合参数
struct blend_params {
	SDL_GPUBlendFactor srcColor;
	SDL_GPUBlendFactor dstColor;
	SDL_GPUBlendOp     colorOp;
	SDL_GPUBlendFactor srcAlpha;
	SDL_GPUBlendFactor dstAlpha;
	SDL_GPUBlendOp     alphaOp;
	bool               blendEnable;
};

// 设备上下文
struct ovg_device_t {
	SDL_GPUDevice* gpuDevice = nullptr;

	shadermodule_vf    shaderModules[5] = {};

	sdl3gpu_texture* emptyTexture = nullptr;
	SDL_GPUShaderFormat supportedFormats = SDL_GPU_SHADERFORMAT_INVALID;

};
class gpu_buffer_cx
{
public:
	SDL_GPUBuffer* buf = 0;
	SDL_GPUDevice* dev = 0;
	SDL_GPUBufferCreateInfo info = {};	// 保存了分配大小
	size_t size = 0;					// 正在用的大小
public:
	gpu_buffer_cx();
	~gpu_buffer_cx();
	void init(SDL_GPUDevice* d, const SDL_GPUBufferCreateInfo* createinfo);
	void init(SDL_GPUDevice* d, uint32_t usage, size_t capacity);
	void resize(size_t newsize);

private:

};

class OvgGpuBuffers {
public:
	SDL_GPUDevice* _device = nullptr;

	gpu_buffer_cx _vbo = {};
	gpu_buffer_cx _ibo = {};
	gpu_buffer_cx _ssbo = {};

	SDL_GPUTransferBuffer* _staging = nullptr;
	size_t _stagingSize = 0;
	size_t vbo_ps = 0;
	size_t ibo_ps = 0;
	size_t ssbo_ps = 0;
	char* mapdt = 0;
	size_t _offset = 0;
public:
	OvgGpuBuffers() = default;
	~OvgGpuBuffers();

	// 初始化（只调用一次）
	void init(SDL_GPUDevice* device);
	// 总顶点大小、索引大小、ssbo大小
	void begin(size_t vcs, size_t ics, size_t ssbocs);
	// 分片添加数据，返回offset,
	uint32_t add_vbo(const void* data, uint32_t size);
	uint32_t add_ibo(const void* data, uint32_t size);
	uint32_t add_ssbo(const void* data, uint32_t size);

	void end(SDL_GPUCommandBuffer* cmd);


	// 绑定接口（RenderPass 内）
	void bindVBO(SDL_GPURenderPass* pass, uint32_t offset = 0);
	void bindIBO(SDL_GPURenderPass* pass, uint32_t offset = 0);
	// 偏移需要自行在ubo指定
	void bindSSBO(SDL_GPURenderPass* pass, bool is_vertex);
	void bind_v_ssbo(SDL_GPURenderPass* pass, SDL_GPUBuffer** storage_buffers, uint32_t num_bindings);
	void bind_f_ssbo(SDL_GPURenderPass* pass, SDL_GPUBuffer** storage_buffers, uint32_t num_bindings);

	// getter（用于创建 pipeline / bind）
	SDL_GPUBuffer* vbo() const { return _vbo.buf; }
	SDL_GPUBuffer* ibo() const { return _ibo.buf; }
private:
};

// 渲染上下文
struct ovg_ctx_t {
	ovg_device_t* device = nullptr;

	SDL_GPUTextureFormat colorFormat = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
	SDL_GPUTextureFormat depthFormat = SDL_GPU_TEXTUREFORMAT_D24_UNORM_S8_UINT;
	SDL_GPUSampleCount   samples = SDL_GPU_SAMPLECOUNT_1;

	// ─── 5 条 VG 专用管道 ───────────────────────
	SDL_GPUGraphicsPipeline* pipeOVER = nullptr;  // 正常绘制 (blend = OVER)
	SDL_GPUGraphicsPipeline* pipeSUB = nullptr;  // 差值绘制 (blend = SUB)
	SDL_GPUGraphicsPipeline* pipeCLEAR = nullptr;  // 清除操作 (logic op CLEAR) 
	SDL_GPUGraphicsPipeline* pipeClipping = nullptr;  // 裁剪掩码写入 (stencil REPLACE)

	SDL_GPUGraphicsPipeline* pipeStencilClear = nullptr; // 清 0
	SDL_GPUGraphicsPipeline* pipeStencilFill = nullptr;  // 非零填充
	SDL_GPUGraphicsPipeline* pipeClipInc = nullptr;      // push
	SDL_GPUGraphicsPipeline* pipeClipDec = nullptr;      // pop
	SDL_GPUGraphicsPipeline* pipeStencilDraw = nullptr;  // 在裁剪区内绘制 
	std::set<SDL_GPUGraphicsPipeline*> freepipe;
	int clipDepth = 0;   // 当前裁剪深度（0 = 无裁剪）
	// ─── 几何管线缓存 ─────────────────────────────
	std::map<uint64_t, pipelinestate_p_internal> geomPipelines;
	pipelinestate_p_internal* currentPipeline = nullptr;
	uint64_t                                     currentState = ~0ull;

	// ─── 缓冲区 ────────────────────────────────────
	OvgGpuBuffers* gpubuf = 0;
	// 纹理管理
	std::pmr::unordered_map<uint32_t, sdl3gpu_texture*> textures;
	// 释放列队
	std::queue<sdl3gpu_texture*> texq;
	uint32_t next_image_id = 0;
	sdl3gpu_texture* currentTexture = nullptr;
	uint32_t         gradientOffset = 0;

	// ─── 视口 ──────────────────────────────────────
	int viewportW = 0;
	int viewportH = 0;

	// ─── 状态 ──────────────────────────────────────
	bool cmdStarted = false;
	int  status = 0;

	// ─── 当前活跃 RenderPass ──────────────────────
	SDL_GPURenderPass* pass = nullptr;
	SDL_GPUCommandBuffer* currentCmdBuf = nullptr;

	// ─── 当前绑定的 VG 管道索引（用于自动设 ref）──
	int currentVgPipeIndex = -1;  // 0=OVER,1=CLEAR,2=SUB,3=POLYFILL,4=CLIPPING
};

// ========================================================================
// 常量
// ========================================================================
#define VG_PTS_SIZE         1024
#define VG_VBO_SIZE         (VG_PTS_SIZE * 4)
#define VG_IBO_SIZE         (VG_VBO_SIZE * 6)


// 顶点布局
#define OVG_VERTEX_SIZE     20   // pos.xy(8) + uv.xy(8) + color(4)
#define GEOM_VERTEX_SIZE    28   // pos.xyz(12) + uv.xy(8) + col0(4) + col1(4)

// ========================================================================
// 工具函数
// ========================================================================

inline size_t align_up(size_t val, size_t align)
{
	return (val + align - 1) / align * align;
}
//inline uint32_t align_up(uint32_t v, uint32_t a) {
//	return (v + a - 1) & ~(a - 1);
//}
/*
	VG_FORMAT_RGBA8 = 0,
	VG_FORMAT_BGRA8,
	VG_FORMAT_RGBA8_SRGB, //SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM_SRGB
	VG_FORMAT_BGRA8_SRGB,
	VG_FORMAT_RGBA16F,
	VG_FORMAT_RGBA32F,
*/
inline SDL_GPUTextureFormat vg_to_sdl_format(vg_format_t fmt) {
	switch (fmt) {
	case VG_FORMAT_RGBA8:   return SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
	case VG_FORMAT_BGRA8:   return SDL_GPU_TEXTUREFORMAT_B8G8R8A8_UNORM;
	case VG_FORMAT_RGBA8_SRGB:   return SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM_SRGB;
	case VG_FORMAT_BGRA8_SRGB:   return SDL_GPU_TEXTUREFORMAT_B8G8R8A8_UNORM_SRGB;
	case VG_FORMAT_RGBA16F: return SDL_GPU_TEXTUREFORMAT_R16G16B16A16_FLOAT;
	case VG_FORMAT_RGBA32F: return SDL_GPU_TEXTUREFORMAT_R32G32B32A32_FLOAT;
	default:                return SDL_GPU_TEXTUREFORMAT_INVALID;
	}
}
static SDL_GPUShaderFormat detect_supported_shader_format(SDL_GPUDevice* dev) {
	SDL_GPUShaderFormat fmt = SDL_GetGPUShaderFormats(dev);
	if (fmt & SDL_GPU_SHADERFORMAT_SPIRV)  return SDL_GPU_SHADERFORMAT_SPIRV;
	if (fmt & SDL_GPU_SHADERFORMAT_DXIL)   return SDL_GPU_SHADERFORMAT_DXIL;
	if (fmt & SDL_GPU_SHADERFORMAT_MSL)    return SDL_GPU_SHADERFORMAT_MSL;
	return SDL_GPU_SHADERFORMAT_INVALID;
}

// ========================================================================
// 着色器编译
// ========================================================================
static SDL_GPUShader* compile_shader(SDL_GPUDevice* device, SDL_GPUShaderStage stage, const uint32_t* code, size_t codeSize, const glm::ivec4& num, const char* entryPoint)
{
	SDL_GPUShaderFormat fmt = detect_supported_shader_format(device);
	if (fmt == SDL_GPU_SHADERFORMAT_INVALID) {
		SDL_LogError(SDL_LOG_CATEGORY_GPU, "No supported shader format!");
		return nullptr;
	}

	SDL_GPUShaderCreateInfo sci = {};
	sci.stage = stage;
	sci.format = fmt;
	sci.code_size = codeSize;
	sci.code = (const Uint8*)code;
	sci.entrypoint = entryPoint;
	sci.num_samplers = num.x;
	sci.num_uniform_buffers = num.y;
	sci.num_storage_buffers = num.z;
	sci.num_storage_textures = num.w;

	SDL_GPUShader* shader = SDL_CreateGPUShader(device, &sci);
	if (!shader) {
		SDL_LogError(SDL_LOG_CATEGORY_GPU, "Failed to create %s shader: %s",
			stage == SDL_GPU_SHADERSTAGE_VERTEX ? "vertex" : "fragment",
			SDL_GetError());
	}
	return shader;
}

// ========================================================================
// 缓冲区管理
// ========================================================================



gpu_buffer_cx::gpu_buffer_cx()
{}

gpu_buffer_cx::~gpu_buffer_cx()
{
	if (buf)
		SDL_ReleaseGPUBuffer(dev, buf);
	buf = 0;
}
void gpu_buffer_cx::init(SDL_GPUDevice* d, const SDL_GPUBufferCreateInfo* createinfo)
{
	if (d && createinfo) {
		dev = d;
		buf = SDL_CreateGPUBuffer(dev, createinfo);
		info = *createinfo;
	}
}
void gpu_buffer_cx::init(SDL_GPUDevice* d, uint32_t usage, size_t capacity)
{
	SDL_GPUBufferCreateInfo c = {};
	if ((!d && !dev) || !usage)return;
	c.usage = usage;
	c.size = align_up(capacity, 256);
	if (d)
		dev = d;
	buf = SDL_CreateGPUBuffer(dev, &c);
	info = c;
	size = c.size;
}
void gpu_buffer_cx::resize(size_t newsize)
{
	size = newsize;
	if (info.size < newsize && info.usage) {
		newsize = align_up(0.5 * info.size + newsize, 256);
		if (buf)
			SDL_ReleaseGPUBuffer(dev, buf);
		info.size = newsize;
		buf = SDL_CreateGPUBuffer(dev, &info);
	}
}

OvgGpuBuffers::~OvgGpuBuffers()
{
	if (_staging)
		SDL_ReleaseGPUTransferBuffer(_device, _staging);

}

void OvgGpuBuffers::init(SDL_GPUDevice* device)
{
	_device = device;
	_vbo.init(_device, SDL_GPU_BUFFERUSAGE_VERTEX, 1024);
	_ibo.init(_device, SDL_GPU_BUFFERUSAGE_INDEX, 1024);
	_ssbo.init(_device, SDL_GPU_BUFFERUSAGE_GRAPHICS_STORAGE_READ, 1024);
}

void OvgGpuBuffers::begin(size_t vcs, size_t ics, size_t ssbocs)
{
	vcs = align_up(vcs, 64);
	ics = align_up(ics, 64);
	ssbocs = align_up(ssbocs, 64);
	size_t ss = vcs + ics + ssbocs;
	if (_device && ss > _stagingSize)
	{
		ss = align_up(ss, 256);
		SDL_GPUTransferBufferCreateInfo tbi = {};
		tbi.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
		tbi.size = ss;
		auto nb = SDL_CreateGPUTransferBuffer(_device, &tbi);
		if (nb) {
			if (_staging)
			{
				SDL_ReleaseGPUTransferBuffer(_device, _staging);
			}
			_stagingSize = ss;
			_staging = nb;
		}
	}
	_vbo.resize(vcs);
	_ibo.resize(ics);
	_ssbo.resize(ssbocs);
	mapdt = (char*)SDL_MapGPUTransferBuffer(_device, _staging, true);
	vbo_ps = ibo_ps = ssbo_ps = 0;
	if (!mapdt)return;
}

uint32_t OvgGpuBuffers::add_vbo(const void* data, uint32_t size)
{
	assert(mapdt);
	if (!mapdt || !size || !data)return 0;
	auto dst = mapdt + vbo_ps;
	memcpy(dst, data, size);
	auto ret = vbo_ps;
	vbo_ps += size;
	return ret;
}

uint32_t OvgGpuBuffers::add_ibo(const void* data, uint32_t size)
{
	assert(mapdt);
	if (!mapdt || !size || !data)return 0;
	auto dst = mapdt + ibo_ps + _vbo.size;
	memcpy(dst, data, size);
	auto ret = ibo_ps;
	ibo_ps += size;
	return ret;
}

uint32_t OvgGpuBuffers::add_ssbo(const void* data, uint32_t size)
{
	assert(mapdt);
	if (!mapdt || !size || !data)return 0;
	auto dst = mapdt + ssbo_ps + _vbo.size + _ibo.size;
	memcpy(dst, data, size);
	auto ret = ssbo_ps;
	ssbo_ps += size;
	return ret;
}

void OvgGpuBuffers::end(SDL_GPUCommandBuffer* cmd)
{
	SDL_UnmapGPUTransferBuffer(_device, _staging);
	if (!(_vbo.size + _ibo.size > 0))return;
	auto copyPass = SDL_BeginGPUCopyPass(cmd);
	SDL_GPUTransferBufferLocation tbl = { .transfer_buffer = _staging, .offset = 0 };
	SDL_GPUBufferRegion vbr = { .buffer = _vbo.buf, .offset = 0, .size = (uint32_t)_vbo.size };
	if (vbr.size)
	{
		SDL_UploadToGPUBuffer(copyPass, &tbl, &vbr, true);
	}
	SDL_GPUBufferRegion ibr = { .buffer = _ibo.buf, .offset = 0, .size = (uint32_t)_ibo.size };
	if (ibr.size) {
		tbl.offset = vbr.size;
		SDL_UploadToGPUBuffer(copyPass, &tbl, &ibr, true);
	}
	SDL_GPUBufferRegion ubr = { .buffer = _ssbo.buf, .offset = 0, .size = (uint32_t)_ssbo.size };
	if (ubr.size) {
		tbl.offset += ibr.size;
		SDL_UploadToGPUBuffer(copyPass, &tbl, &ubr, true);
	}
	SDL_EndGPUCopyPass(copyPass);
}

void OvgGpuBuffers::bindVBO(SDL_GPURenderPass* pass, uint32_t offset)
{
	SDL_GPUBufferBinding binding = {};
	binding.buffer = _vbo.buf;
	binding.offset = offset;
	SDL_BindGPUVertexBuffers(pass, 0, &binding, 1);
}

void OvgGpuBuffers::bindIBO(SDL_GPURenderPass* pass, uint32_t offset)
{
	SDL_GPUBufferBinding binding = {};
	binding.buffer = _ibo.buf;
	binding.offset = offset;
	SDL_BindGPUIndexBuffer(pass, &binding, SDL_GPU_INDEXELEMENTSIZE_32BIT);
}

void OvgGpuBuffers::bindSSBO(SDL_GPURenderPass* pass, bool is_vertex)
{
	if (is_vertex)
		SDL_BindGPUVertexStorageBuffers(pass, 0, &_ssbo.buf, 1);
	else
		SDL_BindGPUFragmentStorageBuffers(pass, 0, &_ssbo.buf, 1);
}

void OvgGpuBuffers::bind_v_ssbo(SDL_GPURenderPass* pass, SDL_GPUBuffer** storage_buffers, uint32_t num_bindings)
{
	SDL_BindGPUVertexStorageBuffers(pass, 0, storage_buffers, num_bindings);
}

void OvgGpuBuffers::bind_f_ssbo(SDL_GPURenderPass* pass, SDL_GPUBuffer** storage_buffers, uint32_t num_bindings)
{
	SDL_BindGPUFragmentStorageBuffers(pass, 0, storage_buffers, num_bindings);
}


struct gpu_ssbo_t {
	SDL_GPUDevice* device;
	SDL_GPUBuffer* buf;
	Uint32          capacity;
	Uint32          size;
};
bool matrix_buffer_create(SDL_GPUDevice* device, Uint32 max_count, gpu_ssbo_t* out)
{
	if (max_count == 0) return false;
	Uint32 size = align_up(max_count * 64, 256);

	SDL_GPUBufferCreateInfo bi = {
		.usage = SDL_GPU_BUFFERUSAGE_GRAPHICS_STORAGE_READ,
		.size = size,
	};
	SDL_GPUBuffer* buf = SDL_CreateGPUBuffer(device, &bi);
	if (!buf) return false;

	out->device = device;
	out->buf = buf;
	out->capacity = max_count;
	out->size = size;
	return true;
}

bool matrix_buffer_update(gpu_ssbo_t* mb, const glm::mat4* matrices, Uint32 count)
{
	if (count > mb->capacity) return false;

	SDL_GPUDevice* device = mb->device;
	Uint32 raw = count * 64;

	SDL_GPUTransferBufferCreateInfo ti = {
		.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
		.size = raw,
	};
	SDL_GPUTransferBuffer* tb = SDL_CreateGPUTransferBuffer(device, &ti);
	if (!tb) return false;

	void* ptr = SDL_MapGPUTransferBuffer(device, tb, false);
	if (!ptr) { SDL_ReleaseGPUTransferBuffer(device, tb); return false; }
	SDL_memcpy(ptr, matrices, raw);
	SDL_UnmapGPUTransferBuffer(device, tb);

	SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(device);
	SDL_GPUCopyPass* cp = SDL_BeginGPUCopyPass(cmd);

	SDL_GPUTransferBufferLocation src = { .transfer_buffer = tb, .offset = 0 };
	SDL_GPUBufferRegion dst = { .buffer = mb->buf, .offset = 0, .size = raw };
	SDL_UploadToGPUBuffer(cp, &src, &dst, true);

	SDL_EndGPUCopyPass(cp);
	SDL_SubmitGPUCommandBuffer(cmd);

	SDL_ReleaseGPUTransferBuffer(device, tb);
	return true;
}

void matrix_buffer_bind_vertex(gpu_ssbo_t* mb, SDL_GPURenderPass* pass, Uint32 slot)
{
	SDL_BindGPUVertexStorageBuffers(pass, slot, &mb->buf, 1);
}

void matrix_buffer_destroy(gpu_ssbo_t* mb) {
	if (mb && mb->buf) {
		SDL_WaitForGPUIdle(mb->device);
		SDL_ReleaseGPUBuffer(mb->device, mb->buf);
		mb->buf = NULL;
	}
}
// ========================================================================
// 纹理管理
// ========================================================================
static sdl3gpu_texture* new_texture(sdl3gpu_texture* tex, ovg_device_t* dev, SDL_GPUTextureFormat  format, int width, int height, SDL_GPUTextureUsageFlags extraUsage = 0)
{
	tex->format = format;
	tex->width = width;
	tex->height = height;
	tex->references = 1;

	SDL_GPUTextureCreateInfo info = {};
	info.format = format;
	info.type = SDL_GPU_TEXTURETYPE_2D;
	info.width = (Uint32)width;
	info.height = (Uint32)height;
	info.layer_count_or_depth = 1;
	info.num_levels = 1;
	info.sample_count = SDL_GPU_SAMPLECOUNT_1;
	info.usage = extraUsage ? extraUsage : SDL_GPU_TEXTUREUSAGE_SAMPLER | SDL_GPU_TEXTUREUSAGE_COLOR_TARGET;

	tex->device = dev->gpuDevice;
	tex->texture = SDL_CreateGPUTexture(dev->gpuDevice, &info);
	assert(tex->texture && "Failed to create texture");

	return tex;
}

static sdl3gpu_texture* new_msaa_texture(sdl3gpu_texture* tex, ovg_device_t* dev, SDL_GPUTextureFormat format, int width, int height, SDL_GPUSampleCount samples)
{
	tex->format = format;
	tex->width = width;
	tex->height = height;
	tex->references = 1;

	SDL_GPUTextureCreateInfo info = {};
	info.format = format;
	info.type = SDL_GPU_TEXTURETYPE_2D;
	info.width = (Uint32)width;
	info.height = (Uint32)height;
	info.layer_count_or_depth = 1;
	info.num_levels = 1;
	info.sample_count = samples;
	info.usage = SDL_GPU_TEXTUREUSAGE_COLOR_TARGET;

	tex->device = dev->gpuDevice;
	tex->texture = SDL_CreateGPUTexture(dev->gpuDevice, &info);
	assert(tex->texture && "Failed to create MSAA texture");

	return tex;
}

static sdl3gpu_texture* new_depth_stencil_texture(sdl3gpu_texture* tex, ovg_device_t* dev, SDL_GPUTextureFormat format, int width, int height, SDL_GPUSampleCount samples)
{
	tex->format = format;
	tex->width = width;
	tex->height = height;
	tex->hasStencil = true;
	tex->references = 1;

	SDL_GPUTextureCreateInfo info = {};
	info.format = format;
	info.type = SDL_GPU_TEXTURETYPE_2D;
	info.width = (Uint32)width;
	info.height = (Uint32)height;
	info.layer_count_or_depth = 1;
	info.num_levels = 1;
	info.sample_count = samples;
	info.usage = SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET;
	tex->device = dev->gpuDevice;
	tex->texture = SDL_CreateGPUTexture(dev->gpuDevice, &info);
	assert(tex->texture && "Failed to create depth/stencil texture");

	return tex;
}
sdl3gpu_texture* new_texture(ovg_device_t* dev, SDL_GPUTextureFormat  format, int width, int height, SDL_GPUTextureUsageFlags extraUsage = 0)
{
	auto p = new sdl3gpu_texture();
	if (p)
	{
		new_texture(p, dev, format, width, height, extraUsage);
	}
	return p;
}
static sdl3gpu_texture* new_msaa_texture(ovg_device_t* dev, SDL_GPUTextureFormat format, int width, int height, SDL_GPUSampleCount samples)
{
	auto p = new sdl3gpu_texture();
	if (p)
	{
		new_msaa_texture(p, dev, format, width, height, samples);
	}
	return p;
}
static sdl3gpu_texture* new_depth_stencil_texture(ovg_device_t* dev, SDL_GPUTextureFormat format, int width, int height, SDL_GPUSampleCount samples)
{
	auto p = new sdl3gpu_texture();
	if (p)
	{
		new_depth_stencil_texture(p, dev, format, width, height, samples);
	}
	return p;
}

static void create_sampler(
	sdl3gpu_texture* tex,
	SDL_GPUFilter         minFilter,
	SDL_GPUFilter         magFilter,
	SDL_GPUSamplerMipmapMode mipmapMode,
	SDL_GPUSamplerAddressMode addressMode)
{
	if (tex->sampler) {
		SDL_ReleaseGPUSampler(tex->device, tex->sampler);
	}

	SDL_GPUSamplerCreateInfo sci = {};
	sci.min_filter = minFilter;
	sci.mag_filter = magFilter;
	sci.mipmap_mode = mipmapMode;
	sci.address_mode_u = addressMode;
	sci.address_mode_v = addressMode;
	sci.address_mode_w = addressMode;
	sci.max_anisotropy = 1.0f;

	tex->sampler = SDL_CreateGPUSampler(tex->device, &sci);
}

static void destroy_texture(sdl3gpu_texture* tex) {
	if (!tex) return;
	tex->references--;
	if (tex->references > 0) return;

	if (tex->sampler) {
		SDL_ReleaseGPUSampler(tex->device, tex->sampler);
		tex->sampler = nullptr;
	}
	if (tex->texture) {
		SDL_ReleaseGPUTexture(tex->device, tex->texture);
		tex->texture = nullptr;
	}
	delete tex;
}

// ========================================================================
// 混合模式转换
// ========================================================================

#if 1
#define SDL_BLENDMODE_NONE_FULL(blend) \
    do { \
        (blend).enable_blend = false; \
        (blend).src_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE; \
        (blend).dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ZERO; \
        (blend).color_blend_op = SDL_GPU_BLENDOP_ADD; \
        (blend).src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE; \
        (blend).dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ZERO; \
        (blend).alpha_blend_op = SDL_GPU_BLENDOP_ADD; \
    } while(0)

#define SDL_BLENDMODE_BLEND_FULL(blend) \
    do { \
        (blend).enable_blend = true; \
        (blend).src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA; \
        (blend).dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA; \
        (blend).color_blend_op = SDL_GPU_BLENDOP_ADD; \
        (blend).src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE; \
        (blend).dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA; \
        (blend).alpha_blend_op = SDL_GPU_BLENDOP_ADD; \
    } while(0)

#define SDL_BLENDMODE_BLEND_PREMULTIPLIED_FULL(blend) \
    do { \
        (blend).enable_blend = true; \
        (blend).src_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE; \
        (blend).dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA; \
        (blend).color_blend_op = SDL_GPU_BLENDOP_ADD; \
        (blend).src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE; \
        (blend).dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA; \
        (blend).alpha_blend_op = SDL_GPU_BLENDOP_ADD; \
    } while(0)

#define SDL_BLENDMODE_ADD_FULL(blend) \
    do { \
        (blend).enable_blend = true; \
        (blend).src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA; \
        (blend).dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE; \
        (blend).color_blend_op = SDL_GPU_BLENDOP_ADD; \
        (blend).src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ZERO; \
        (blend).dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE; \
        (blend).alpha_blend_op = SDL_GPU_BLENDOP_ADD; \
    } while(0)

#define SDL_BLENDMODE_ADD_PREMULTIPLIED_FULL(blend) \
    do { \
        (blend).enable_blend = true; \
        (blend).src_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE; \
        (blend).dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE; \
        (blend).color_blend_op = SDL_GPU_BLENDOP_ADD; \
        (blend).src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ZERO; \
        (blend).dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE; \
        (blend).alpha_blend_op = SDL_GPU_BLENDOP_ADD; \
    } while(0)

#define SDL_BLENDMODE_MOD_FULL(blend) \
    do { \
        (blend).enable_blend = true; \
        (blend).src_color_blendfactor = SDL_GPU_BLENDFACTOR_ZERO; \
        (blend).dst_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_COLOR; \
        (blend).color_blend_op = SDL_GPU_BLENDOP_ADD; \
        (blend).src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ZERO; \
        (blend).dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE; \
        (blend).alpha_blend_op = SDL_GPU_BLENDOP_ADD; \
    } while(0)

#define SDL_BLENDMODE_MUL_FULL(blend) \
    do { \
        (blend).enable_blend = true; \
        (blend).src_color_blendfactor = SDL_GPU_BLENDFACTOR_DST_COLOR; \
        (blend).dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA; \
        (blend).color_blend_op = SDL_GPU_BLENDOP_ADD; \
        (blend).src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ZERO; \
        (blend).dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE; \
        (blend).alpha_blend_op = SDL_GPU_BLENDOP_ADD; \
    } while(0)

#define SDL_BLENDMODE_SCREEN_FULL(blend) \
    do { \
        (blend).enable_blend = true; \
        (blend).src_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE; \
        (blend).dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_COLOR; \
        (blend).color_blend_op = SDL_GPU_BLENDOP_ADD; \
        (blend).src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE; \
        (blend).dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_COLOR; \
        (blend).alpha_blend_op = SDL_GPU_BLENDOP_ADD; \
    } while(0)

void gpu_set_blend(SDL_GPUColorTargetBlendState& blend, blendMode_e bm)
{
	blend = {};
	switch (bm)
	{
	case blendMode_e::none:
		SDL_BLENDMODE_NONE_FULL(blend);
		break;
	case blendMode_e::normal:
		SDL_BLENDMODE_BLEND_FULL(blend);
		break;
	case blendMode_e::additive:
		SDL_BLENDMODE_ADD_FULL(blend);
		break;
	case blendMode_e::normal_prem:
		SDL_BLENDMODE_BLEND_PREMULTIPLIED_FULL(blend);
		break;
	case blendMode_e::additive_prem:
		SDL_BLENDMODE_ADD_PREMULTIPLIED_FULL(blend);
		break;
	case blendMode_e::multiply:
		SDL_BLENDMODE_MUL_FULL(blend);
		break;
	case blendMode_e::modulate:
		SDL_BLENDMODE_MOD_FULL(blend);
		break;
	case blendMode_e::screen:
		SDL_BLENDMODE_SCREEN_FULL(blend);
		break;
	default:
		SDL_BLENDMODE_NONE_FULL(blend);
		break;
	}

	blend.color_write_mask =
		SDL_GPU_COLORCOMPONENT_R |
		SDL_GPU_COLORCOMPONENT_G |
		SDL_GPU_COLORCOMPONENT_B |
		SDL_GPU_COLORCOMPONENT_A;
}
#endif // 1

// ========================================================================
// 着色器模块管理
// ========================================================================
static void init_shader_modules(ovg_device_t* dev) {
	struct { const uint32_t* v; size_t vlen; const uint32_t* f; size_t flen; } shaders[5] = {
		{a_base3d_vert,       sizeof(a_base3d_vert),         a_base3d_frag,          sizeof(a_base3d_frag)},
		{a_base3d_mask_vert,    sizeof(a_base3d_mask_vert),    a_base3d_mask_frag,     sizeof(a_base3d_mask_frag)},
		{a_base3d_dsc_vert,     sizeof(a_base3d_dsc_vert),     a_base3d_dsc_frag,     sizeof(a_base3d_dsc_frag)},
		{a_base3d_inst_vert,    sizeof(a_base3d_inst_vert),    a_base3d_inst_frag,    sizeof(a_base3d_inst_frag)},
		{a_base3d_dsc_inst_vert, sizeof(a_base3d_dsc_inst_vert),a_base3d_dsc_inst_frag, sizeof(a_base3d_dsc_inst_frag)},
	};
	struct num_sm {
		glm::ivec4 vnum = {};	//num_samplers = num.x;	num_uniform_buffers = num.y;	num_storage_buffers = num.z;	num_storage_textures = num.w;
		glm::ivec4 fnum = {};
	};
	num_sm nums[5] = { {glm::ivec4(0,1,0,0),glm::ivec4(1,0,0,0)}
		,{glm::ivec4(0,1,0,0),glm::ivec4(2,1,0,0)}
		,{glm::ivec4(0,1,0,0),glm::ivec4(1,0,0,0)}
		,{glm::ivec4(0,1,1,0),glm::ivec4(1,0,0,0)}
		,{glm::ivec4(0,1,1,0),glm::ivec4(1,0,0,0)}
	};
	for (int i = 0; i < 5; i++) {
		dev->shaderModules[i].vert = compile_shader(dev->gpuDevice, SDL_GPU_SHADERSTAGE_VERTEX, shaders[i].v, shaders[i].vlen, nums[i].vnum, "main");
		dev->shaderModules[i].frag = compile_shader(dev->gpuDevice, SDL_GPU_SHADERSTAGE_FRAGMENT, shaders[i].f, shaders[i].flen, nums[i].fnum, "main");
	}
}

static void destroy_shader_modules(ovg_device_t* dev) {
	for (int i = 0; i < 5; i++) {
		if (dev->shaderModules[i].vert) SDL_ReleaseGPUShader(dev->gpuDevice, dev->shaderModules[i].vert);
		if (dev->shaderModules[i].frag) SDL_ReleaseGPUShader(dev->gpuDevice, dev->shaderModules[i].frag);
		dev->shaderModules[i] = {};
	}
}

// ========================================================================
// 管道创建辅助
// ========================================================================
struct vg_pipeline_inputs {
	SDL_GPUShader* vertShader;
	SDL_GPUShader* fragShader;
	SDL_GPUTextureFormat colorFormat;
	SDL_GPUTextureFormat depthFormat;
	SDL_GPUSampleCount   samples;
	SDL_GPUPrimitiveType topology;
	SDL_GPUColorComponentFlags color_write_mask;
	bool                 depthTestEnable;
	bool                 depthWriteEnable;
	bool                 stencilTestEnable;
	bool                 enable_color_write_mask;
	bool                 logicOpEnable;
	SDL_GPUBlendOp       logicOp;
	blendMode_e          blendMode;
	float                lineWidth;
	uint32_t             vertexStride;
	uint32_t             numAttributes;
	SDL_GPUVertexAttribute attributes[4];
	// 模板状态
	SDL_GPUDepthStencilState ds;
	//SDL_GPUDepthStencilState stencilBack;
	SDL_GPUStencilOpState stencilFront;
	SDL_GPUStencilOpState stencilBack;
};

static SDL_GPUGraphicsPipeline* create_graphics_pipeline(ovg_device_t* dev, const vg_pipeline_inputs* inputs, ovg_ctx_t* ctx)
{
	blend_params bp = {};
	SDL_GPUColorTargetDescription colorTarget = {};
	colorTarget.format = inputs->colorFormat;
	colorTarget.blend_state.enable_blend = bp.blendEnable;
	colorTarget.blend_state.src_color_blendfactor = bp.srcColor;
	colorTarget.blend_state.dst_color_blendfactor = bp.dstColor;
	colorTarget.blend_state.color_blend_op = bp.colorOp;
	colorTarget.blend_state.src_alpha_blendfactor = bp.srcAlpha;
	colorTarget.blend_state.dst_alpha_blendfactor = bp.dstAlpha;
	colorTarget.blend_state.alpha_blend_op = bp.alphaOp;
	colorTarget.blend_state.enable_color_write_mask = true;
	colorTarget.blend_state.color_write_mask = SDL_GPU_COLORCOMPONENT_R |
		SDL_GPU_COLORCOMPONENT_G |
		SDL_GPU_COLORCOMPONENT_B |
		SDL_GPU_COLORCOMPONENT_A;

	gpu_set_blend(colorTarget.blend_state, inputs->blendMode);
	if (inputs->logicOpEnable)
		colorTarget.blend_state.alpha_blend_op = colorTarget.blend_state.color_blend_op = inputs->logicOp;
	SDL_GPUDepthStencilState dsState = {};
	dsState.enable_depth_test = inputs->depthTestEnable;
	dsState.enable_depth_write = inputs->depthWriteEnable;
	dsState.compare_op = SDL_GPU_COMPAREOP_LESS_OR_EQUAL;
	dsState.enable_stencil_test = inputs->stencilTestEnable;
	dsState.front_stencil_state = inputs->stencilFront;
	dsState.back_stencil_state = inputs->stencilBack;
	dsState.compare_mask = inputs->ds.compare_mask;
	dsState.write_mask = inputs->ds.write_mask;
	SDL_GPURasterizerState rasterState = {};
	rasterState.fill_mode = SDL_GPU_FILLMODE_FILL;
	rasterState.cull_mode = SDL_GPU_CULLMODE_NONE;
	rasterState.front_face = SDL_GPU_FRONTFACE_COUNTER_CLOCKWISE;
	rasterState.enable_depth_bias = false;
	rasterState.enable_depth_clip = false;
	colorTarget.blend_state.enable_color_write_mask = inputs->enable_color_write_mask;
	if (inputs->enable_color_write_mask)
		colorTarget.blend_state.color_write_mask = inputs->color_write_mask;
	SDL_GPUMultisampleState msState = {};
	msState.sample_count = inputs->samples;
	msState.sample_mask = 0;// 0xFFFFFFFF;

	SDL_GPUVertexBufferDescription vertexBufferDesc = {};
	vertexBufferDesc.slot = 0;
	vertexBufferDesc.input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;
	vertexBufferDesc.pitch = inputs->vertexStride;

	SDL_GPUVertexInputState vertexInput = {};
	vertexInput.num_vertex_buffers = 1;
	vertexInput.vertex_buffer_descriptions = &vertexBufferDesc;
	vertexInput.num_vertex_attributes = inputs->numAttributes;
	vertexInput.vertex_attributes = (SDL_GPUVertexAttribute*)inputs->attributes;

	SDL_GPUGraphicsPipelineCreateInfo pci = {};
	pci.vertex_shader = inputs->vertShader;
	pci.fragment_shader = inputs->fragShader;
	pci.vertex_input_state = vertexInput;
	pci.primitive_type = inputs->topology;
	pci.rasterizer_state = rasterState;
	pci.multisample_state = msState;
	pci.depth_stencil_state = dsState;
	//pci.num_color_targets = 1;
	//pci.color_target_descriptions = &colorTarget;
	//pci.enable_primitive_restart = false;
	pci.target_info.has_depth_stencil_target = (inputs->depthFormat != SDL_GPU_TEXTUREFORMAT_INVALID);
	pci.target_info.depth_stencil_format = inputs->depthFormat;
	pci.target_info.num_color_targets = 1;
	pci.target_info.color_target_descriptions = &colorTarget;

	SDL_GPUGraphicsPipeline* pipeline = SDL_CreateGPUGraphicsPipeline(dev->gpuDevice, &pci);
	if (!pipeline) {
		SDL_LogError(SDL_LOG_CATEGORY_GPU, "Failed to create graphics pipeline: %s", SDL_GetError());
	}
	if (ctx)
		ctx->freepipe.insert(pipeline);
	return pipeline;
}

// ========================================================================
// ★ 核心：5 条 VG 管道初始化（ref 动态切方案）
// ========================================================================
//
//   bit1 (STENCIL_CLIP_BIT = 0x1) → 裁剪掩码，由 pipeClipping 的 REPLACE 写入
//
// 动态 ref 切换：
//   - 填充阶段：ref 无关（INVERT 不依赖 ref）
//   - 裁剪阶段：ref = STENCIL_CLIP_BIT，写入掩码
//   - 绘制阶段：ref = STENCIL_CLIP_BIT，通过测试
//
// ========================================================================
static void init_vg_pipelines(ovg_ctx_t* ctx) {
	ovg_device_t* dev = ctx->device;

	glm::ivec4 vnum = { 0,1,0,0 };	//num_samplers = num.x;	num_uniform_buffers = num.y;	num_storage_buffers = num.z;	num_storage_textures = num.w;
	glm::ivec4 fnum = { 1,1,0,0 };
	// 编译 VG 着色器
	SDL_GPUShader* vgVert = compile_shader(dev->gpuDevice, SDL_GPU_SHADERSTAGE_VERTEX, vg_vert, sizeof(vg_vert), vnum, "main");
	SDL_GPUShader* vgFrag = compile_shader(dev->gpuDevice, SDL_GPU_SHADERSTAGE_FRAGMENT, vg_frag, sizeof(vg_frag), fnum, "main");

	// 公共顶点属性: pos(2) + uv(2) + color(1) = 20 bytes
	SDL_GPUVertexAttribute vgAttrs[3] = {
		{0, 0, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2,        0},   // pos
		{1, 0, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2,        8},   // uv
		{2, 0, SDL_GPU_VERTEXELEMENTFORMAT_UBYTE4_NORM, 16},	// color (RGBA8 UNORM)
	};

	// ═══════════════════════════════════════════════════════════════
	// ② pipeClipping — 裁剪掩码写入
	//   职责：把通过的像素 bit1 写成 ref（= STENCIL_CLIP_BIT = 0x1）
	//   使用：TRIANGLE_LIST，compareOp=ALWAYS
	//   绘制前需：SDL_SetGPUStencilReference(pass, STENCIL_CLIP_BIT)
	// ═══════════════════════════════════════════════════════════════
	{
		vg_pipeline_inputs inputs = {};
		inputs.vertShader = vgVert;
		inputs.fragShader = vgFrag;
		inputs.colorFormat = ctx->colorFormat;
		inputs.depthFormat = ctx->depthFormat;
		inputs.samples = ctx->samples;
		inputs.topology = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
		inputs.depthTestEnable = false;
		inputs.depthWriteEnable = false;
		inputs.stencilTestEnable = true;
		inputs.blendMode = blendMode_e::none;
		inputs.vertexStride = sizeof(ovgVertex);
		inputs.numAttributes = 3;
		inputs.enable_color_write_mask = true;// 只写 stencil
		inputs.color_write_mask = 0;
		memcpy(inputs.attributes, vgAttrs, sizeof(vgAttrs));

		// ★ 核心 stencil 状态 
		inputs.ds.compare_mask = STENCIL_CLIP_BIT;
		inputs.ds.write_mask = STENCIL_CLIP_BIT;
		inputs.stencilFront.compare_op = SDL_GPU_COMPAREOP_ALWAYS;
		inputs.stencilFront.pass_op = SDL_GPU_STENCILOP_REPLACE;
		inputs.stencilFront.fail_op = SDL_GPU_STENCILOP_KEEP;
		inputs.stencilFront.depth_fail_op = SDL_GPU_STENCILOP_KEEP;
		inputs.stencilBack = inputs.stencilFront;
		ctx->pipeClipping = create_graphics_pipeline(dev, &inputs, ctx);
	}

	// ═══════════════════════════════════════════════════════════════
	// ③ pipeOVER — 正常绘制（受裁剪影响）
	//   职责：只画 stencil & CLIP_BIT == CLIP_BIT 的像素
	//   使用：TRIANGLE_LIST，compareOp=EQUAL
	//   绘制前需：SDL_SetGPUStencilReference(pass, STENCIL_CLIP_BIT)
	// ═══════════════════════════════════════════════════════════════
	{
		vg_pipeline_inputs inputs = {};
		inputs.vertShader = vgVert;
		inputs.fragShader = vgFrag;
		inputs.colorFormat = ctx->colorFormat;
		inputs.depthFormat = ctx->depthFormat;
		inputs.samples = ctx->samples;
		inputs.topology = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
		inputs.depthTestEnable = false;
		inputs.depthWriteEnable = false;
		inputs.stencilTestEnable = true;
		inputs.blendMode = blendMode_e::normal_prem;
		inputs.vertexStride = sizeof(ovgVertex);
		inputs.numAttributes = 3;
		memcpy(inputs.attributes, vgAttrs, sizeof(vgAttrs));

		// ★ 核心 stencil 状态
		//   compareOp = EQUAL → (stencil & compare_mask) == (ref & compare_mask)
		//   compare_mask = CLIP_BIT → 只比 bit1
		//   writeMask = 0 → 不改 stencil
		//   ref = STENCIL_CLIP_BIT (0x1) → 动态设置
		inputs.ds.compare_mask = STENCIL_CLIP_BIT;
		inputs.ds.write_mask = 0x0;                   // 不改 stencil
		inputs.stencilFront.compare_op = SDL_GPU_COMPAREOP_EQUAL;
		inputs.stencilFront.pass_op = SDL_GPU_STENCILOP_KEEP;
		inputs.stencilFront.fail_op = SDL_GPU_STENCILOP_KEEP;
		inputs.stencilFront.depth_fail_op = SDL_GPU_STENCILOP_KEEP;
		//inputs.stencilFront.reference = 0;  // 动态覆盖
		inputs.stencilBack = inputs.stencilFront;

		ctx->pipeOVER = create_graphics_pipeline(dev, &inputs, ctx);
	}

	// ═══════════════════════════════════════════════════════════════
	// ④ pipeSUB — 差值绘制（受裁剪影响，blend 不同）
	//   stencil 状态与 pipeOVER 完全相同
	// ═══════════════════════════════════════════════════════════════
	{
		vg_pipeline_inputs inputs = {};
		inputs.vertShader = vgVert;
		inputs.fragShader = vgFrag;
		inputs.colorFormat = ctx->colorFormat;
		inputs.depthFormat = ctx->depthFormat;
		inputs.samples = ctx->samples;
		inputs.topology = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
		inputs.depthTestEnable = false;
		inputs.depthWriteEnable = false;
		inputs.stencilTestEnable = true;
		inputs.blendMode = blendMode_e::normal_prem;  // 用逻辑操作 SUBTRACT 模拟
		inputs.vertexStride = sizeof(ovgVertex);
		inputs.numAttributes = 3;
		memcpy(inputs.attributes, vgAttrs, sizeof(vgAttrs));

		// stencil 与 OVER 完全一致
		inputs.ds.compare_mask = STENCIL_CLIP_BIT;
		inputs.ds.write_mask = 0x0;
		inputs.stencilFront.compare_op = SDL_GPU_COMPAREOP_EQUAL;
		inputs.stencilFront.pass_op = SDL_GPU_STENCILOP_KEEP;
		inputs.stencilFront.fail_op = SDL_GPU_STENCILOP_KEEP;
		inputs.stencilFront.depth_fail_op = SDL_GPU_STENCILOP_KEEP;
		//inputs.stencilFront.reference = 0;
		inputs.stencilBack = inputs.stencilFront;
		inputs.logicOp = SDL_GPU_BLENDOP_SUBTRACT;
		inputs.logicOpEnable = true;
		ctx->pipeSUB = create_graphics_pipeline(dev, &inputs, ctx);
	}

	// ═══════════════════════════════════════════════════════════════
	// ⑤ pipeCLEAR — 清除操作
	//   职责：无视 stencil，直接写颜色（logic op CLEAR）
	// ═══════════════════════════════════════════════════════════════
	{
		vg_pipeline_inputs inputs = {};
		inputs.vertShader = vgVert;
		inputs.fragShader = vgFrag;
		inputs.colorFormat = ctx->colorFormat;
		inputs.depthFormat = ctx->depthFormat;
		inputs.samples = ctx->samples;
		inputs.topology = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
		inputs.depthTestEnable = false;
		inputs.depthWriteEnable = false;
		inputs.stencilTestEnable = false;  // ★ 关 stencil
		inputs.blendMode = blendMode_e::none;
		inputs.vertexStride = sizeof(ovgVertex);
		inputs.numAttributes = 3;
		memcpy(inputs.attributes, vgAttrs, sizeof(vgAttrs));

		// stencil 状态无关（已禁用）
		//inputs.stencilFront.compare_mask = 0xFF;
		//inputs.stencilFront.write_mask = 0x0;
		inputs.stencilFront.compare_op = SDL_GPU_COMPAREOP_ALWAYS;
		inputs.stencilFront.pass_op = SDL_GPU_STENCILOP_KEEP;
		inputs.stencilFront.fail_op = SDL_GPU_STENCILOP_KEEP;
		inputs.stencilFront.depth_fail_op = SDL_GPU_STENCILOP_KEEP;
		inputs.stencilBack = inputs.stencilFront;

		inputs.enable_color_write_mask = true;
		ctx->pipeCLEAR = create_graphics_pipeline(dev, &inputs, ctx);
	}
	// todo 管线
	{
		vg_pipeline_inputs inputs = {};
		inputs.vertShader = vgVert;
		inputs.fragShader = vgFrag;
		inputs.colorFormat = ctx->colorFormat;
		inputs.depthFormat = ctx->depthFormat;
		inputs.samples = ctx->samples;
		inputs.topology = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
		inputs.depthTestEnable = false;
		inputs.depthWriteEnable = false;
		inputs.stencilTestEnable = true;
		inputs.blendMode = blendMode_e::none;
		inputs.vertexStride = sizeof(ovgVertex);
		inputs.numAttributes = 3;
		memcpy(inputs.attributes, vgAttrs, sizeof(vgAttrs));
		inputs.enable_color_write_mask = true;
		inputs.color_write_mask = 0;

		// ★ 清 stencil 核心
		inputs.ds.compare_mask = 0xFF;
		inputs.ds.write_mask = 0xFF;

		inputs.stencilFront.compare_op = SDL_GPU_COMPAREOP_ALWAYS;
		inputs.stencilFront.pass_op = SDL_GPU_STENCILOP_REPLACE;
		inputs.stencilFront.fail_op = SDL_GPU_STENCILOP_REPLACE;
		inputs.stencilFront.depth_fail_op = SDL_GPU_STENCILOP_REPLACE;

		inputs.stencilBack = inputs.stencilFront;

		ctx->pipeStencilClear = create_graphics_pipeline(dev, &inputs, ctx);
	}
	{
		vg_pipeline_inputs inputs = {};
		inputs.vertShader = vgVert;      // fullscreen tri
		inputs.fragShader = vgFrag;      // empty FS
		inputs.colorFormat = ctx->colorFormat;
		inputs.depthFormat = ctx->depthFormat;
		inputs.samples = ctx->samples;
		inputs.topology = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
		inputs.depthTestEnable = false;
		inputs.depthWriteEnable = false;
		inputs.stencilTestEnable = true;
		inputs.blendMode = blendMode_e::none;
		inputs.vertexStride = sizeof(ovgVertex);
		inputs.numAttributes = 3;
		memcpy(inputs.attributes, vgAttrs, sizeof(vgAttrs));
		inputs.enable_color_write_mask = true;
		inputs.color_write_mask = 0;

		// ★ push：EQUAL + INCR
		inputs.ds.compare_mask = 0xFF;
		inputs.ds.write_mask = 0xFF;

		inputs.stencilFront.compare_op = SDL_GPU_COMPAREOP_EQUAL;
		inputs.stencilFront.pass_op = SDL_GPU_STENCILOP_INCREMENT_AND_CLAMP;
		inputs.stencilFront.fail_op = SDL_GPU_STENCILOP_KEEP;
		inputs.stencilFront.depth_fail_op = SDL_GPU_STENCILOP_KEEP;

		inputs.stencilBack = inputs.stencilFront;

		ctx->pipeClipInc = create_graphics_pipeline(dev, &inputs, ctx);
	}
	{
		vg_pipeline_inputs inputs = {};
		inputs.vertShader = vgVert;
		inputs.fragShader = vgFrag;
		inputs.colorFormat = ctx->colorFormat;
		inputs.depthFormat = ctx->depthFormat;
		inputs.samples = ctx->samples;
		inputs.topology = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
		inputs.depthTestEnable = false;
		inputs.depthWriteEnable = false;
		inputs.stencilTestEnable = true;
		inputs.blendMode = blendMode_e::none;
		inputs.vertexStride = sizeof(ovgVertex);
		inputs.numAttributes = 3;
		memcpy(inputs.attributes, vgAttrs, sizeof(vgAttrs));
		inputs.enable_color_write_mask = true;
		inputs.color_write_mask = 0;

		// ★ pop：EQUAL + DECR
		inputs.ds.compare_mask = 0xFF;
		inputs.ds.write_mask = 0xFF;

		inputs.stencilFront.compare_op = SDL_GPU_COMPAREOP_EQUAL;
		inputs.stencilFront.pass_op = SDL_GPU_STENCILOP_DECREMENT_AND_CLAMP;
		inputs.stencilFront.fail_op = SDL_GPU_STENCILOP_KEEP;
		inputs.stencilFront.depth_fail_op = SDL_GPU_STENCILOP_KEEP;

		inputs.stencilBack = inputs.stencilFront;

		ctx->pipeClipDec = create_graphics_pipeline(dev, &inputs, ctx);
	}
	{
		vg_pipeline_inputs inputs = {};
		inputs.vertShader = vgVert;
		inputs.fragShader = vgFrag;
		inputs.colorFormat = ctx->colorFormat;
		inputs.depthFormat = ctx->depthFormat;
		inputs.samples = ctx->samples;
		inputs.topology = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
		inputs.depthTestEnable = false;
		inputs.depthWriteEnable = false;
		inputs.stencilTestEnable = true;
		inputs.blendMode = blendMode_e::normal_prem; // 或你需要的混合
		inputs.vertexStride = sizeof(ovgVertex);
		inputs.numAttributes = 3;
		memcpy(inputs.attributes, vgAttrs, sizeof(vgAttrs));
		inputs.enable_color_write_mask = true;
		inputs.color_write_mask = SDL_GPU_COLORCOMPONENT_R |
			SDL_GPU_COLORCOMPONENT_G |
			SDL_GPU_COLORCOMPONENT_B |
			SDL_GPU_COLORCOMPONENT_A;

		// ★ 只在 CLIP 区内画
		inputs.ds.compare_mask = STENCIL_CLIP_BIT;
		inputs.ds.write_mask = 0;

		inputs.stencilFront.compare_op = SDL_GPU_COMPAREOP_EQUAL;
		inputs.stencilFront.pass_op = SDL_GPU_STENCILOP_KEEP;
		inputs.stencilFront.fail_op = SDL_GPU_STENCILOP_KEEP;
		inputs.stencilFront.depth_fail_op = SDL_GPU_STENCILOP_KEEP;

		inputs.stencilBack = inputs.stencilFront;

		ctx->pipeStencilDraw = create_graphics_pipeline(dev, &inputs, ctx);
	}
	// 释放 VG 着色器（管道已持有引用）
	SDL_ReleaseGPUShader(dev->gpuDevice, vgVert);
	SDL_ReleaseGPUShader(dev->gpuDevice, vgFrag);
}

// ========================================================================
// 几何管道创建
// ========================================================================
static pipelinestate_p_internal create_geom_pipeline(ovg_ctx_t* ctx, const gem_info_t* info)
{
	pipelinestate_p_internal result = {};
	auto dev = ctx->device;
	int shaderIdx = info->shader;
	if (shaderIdx < 0 || shaderIdx >= 5) shaderIdx = 0;
	bool doublesided = info->shader == ST_INSTANCE_DOUBLESIDED || info->shader == ST_DOUBLESIDED;
	SDL_GPUShader* vert = dev->shaderModules[shaderIdx].vert;
	SDL_GPUShader* frag = dev->shaderModules[shaderIdx].frag;

	SDL_GPUVertexAttribute geomAttrs[4] = {
		{0, 0, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3,        0},
		{1, 0, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2,       12},
		{2, 0, SDL_GPU_VERTEXELEMENTFORMAT_UBYTE4_NORM,  20},
		{3, 0, SDL_GPU_VERTEXELEMENTFORMAT_UBYTE4_NORM,  24},
	};

	SDL_GPUColorTargetDescription colorTarget = {};
	colorTarget.format = ctx->colorFormat;
	colorTarget.blend_state.enable_color_write_mask = true;
	colorTarget.blend_state.color_write_mask = SDL_GPU_COLORCOMPONENT_R |
		SDL_GPU_COLORCOMPONENT_G |
		SDL_GPU_COLORCOMPONENT_B |
		SDL_GPU_COLORCOMPONENT_A;

	blend_params bp = {};
	colorTarget.blend_state.enable_blend = bp.blendEnable;
	colorTarget.blend_state.src_color_blendfactor = bp.srcColor;
	colorTarget.blend_state.dst_color_blendfactor = bp.dstColor;
	colorTarget.blend_state.color_blend_op = bp.colorOp;
	colorTarget.blend_state.src_alpha_blendfactor = bp.srcAlpha;
	colorTarget.blend_state.dst_alpha_blendfactor = bp.dstAlpha;
	colorTarget.blend_state.alpha_blend_op = bp.alphaOp;
	gpu_set_blend(colorTarget.blend_state, (blendMode_e)info->blendMode);

	SDL_GPUDepthStencilState dsState = {};
	dsState.enable_depth_test = (info->flags & (uint8_t)depth_stencil_State::d_depthtest_enable) != 0;
	dsState.enable_depth_write = (info->flags & (uint8_t)depth_stencil_State::d_depthwrite_enable) != 0;
	dsState.compare_op = SDL_GPU_COMPAREOP_LESS_OR_EQUAL;
	dsState.enable_stencil_test = (info->flags & (uint8_t)depth_stencil_State::d_stenciltest_enable) != 0;
	dsState.compare_mask = 0xFF;
	dsState.write_mask = 0;
	dsState.front_stencil_state.compare_op = SDL_GPU_COMPAREOP_ALWAYS;
	dsState.front_stencil_state.pass_op = SDL_GPU_STENCILOP_KEEP;
	dsState.front_stencil_state.fail_op = SDL_GPU_STENCILOP_KEEP;
	dsState.front_stencil_state.depth_fail_op = SDL_GPU_STENCILOP_KEEP;
	dsState.back_stencil_state = dsState.front_stencil_state;

	SDL_GPURasterizerState rasterState = {};
	rasterState.fill_mode = (SDL_GPUFillMode)info->polygon;
	rasterState.cull_mode = SDL_GPU_CULLMODE_NONE;
	rasterState.front_face = info->frontFace ? SDL_GPU_FRONTFACE_CLOCKWISE : SDL_GPU_FRONTFACE_COUNTER_CLOCKWISE;
	rasterState.enable_depth_clip = false;

	SDL_GPUMultisampleState msState = {};
	msState.sample_count = ctx->samples;
	msState.sample_mask = 0;// 0xFFFFFFFF;

	SDL_GPUVertexBufferDescription vbDesc = {};
	vbDesc.slot = 0;
	vbDesc.input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;
	vbDesc.pitch = doublesided ? sizeof(geomVertex2) : sizeof(geomVertex1);

	SDL_GPUVertexInputState vertexInput = {};
	vertexInput.num_vertex_buffers = 1;
	vertexInput.vertex_buffer_descriptions = &vbDesc;
	vertexInput.num_vertex_attributes = 3 + (doublesided ? 1 : 0);
	vertexInput.vertex_attributes = geomAttrs;

	SDL_GPUGraphicsPipelineCreateInfo pci = {};
	pci.vertex_shader = vert;
	pci.fragment_shader = frag;
	pci.vertex_input_state = vertexInput;
	pci.primitive_type = (SDL_GPUPrimitiveType)info->topology;
	pci.rasterizer_state = rasterState;
	pci.multisample_state = msState;
	pci.depth_stencil_state = dsState;
	//pci.num_color_targets = 1;
	//pci.color_target_descriptions = &colorTarget;
	//pci.enable_primitive_restart = false;
	pci.target_info.has_depth_stencil_target = true;
	pci.target_info.depth_stencil_format = ctx->depthFormat;
	pci.target_info.num_color_targets = 1;
	pci.target_info.color_target_descriptions = &colorTarget;

	result.pipeline = SDL_CreateGPUGraphicsPipeline(dev->gpuDevice, &pci);
	result.state = *info;

	SDL_GPUSamplerCreateInfo sci = {};
	sci.min_filter = SDL_GPU_FILTER_LINEAR;
	sci.mag_filter = SDL_GPU_FILTER_LINEAR;
	sci.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_NEAREST;
	sci.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
	sci.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
	sci.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
	sci.max_anisotropy = 1.0f;
	result.defaultSampler = SDL_CreateGPUSampler(dev->gpuDevice, &sci);

	return result;
}

// ========================================================================
// 管道状态管理
// ========================================================================
static pipelinestate_p_internal* get_geom_pipeline(ovg_ctx_t* ctx, const gem_info_t* info) {
	uint64_t key = *(const uint64_t*)info;

	if (ctx->currentPipeline && ctx->currentState == key) {
		return ctx->currentPipeline;
	}

	ctx->currentState = key;
	auto it = ctx->geomPipelines.find(key);
	if (it == ctx->geomPipelines.end()) {
		pipelinestate_p_internal p = create_geom_pipeline(ctx, info);
		ctx->geomPipelines[key] = p;
		ctx->currentPipeline = &ctx->geomPipelines[key];
	}
	else {
		ctx->currentPipeline = &it->second;
	}
	return ctx->currentPipeline;
}

// ========================================================================
// ★ 核心：VG 管道绑定 + 自动设置 stencil ref
// ========================================================================
//
// 每次 bind pipeline 后立刻设置正确的 stencil reference：
//   pipePolyFill → ref 无关（ALWAYS），设 0 即可
//   pipeClipping → ref = STENCIL_CLIP_BIT，REPLACE 写入
//   pipeOVER     → ref = STENCIL_CLIP_BIT，EQUAL 比较
//   pipeSUB      → ref = STENCIL_CLIP_BIT，EQUAL 比较
//   pipeCLEAR    → stencil 已禁用，ref 无关
//
// ========================================================================
void ovg_bind_vg_pipeline(ovg_ctx_t* ctx, SDL_GPUCommandBuffer* cmdBuf, SDL_GPURenderPass* pass, int operatorType) {
	if (!ctx || !pass) return;

	SDL_GPUGraphicsPipeline* pipe = ctx->pipeOVER;
	int pipeIndex = 0;
	auto ot = (vg_operator_t)operatorType;
	switch (ot) {
	case vg_operator_t::VG_OPERATOR_OVER:  pipe = ctx->pipeOVER;     pipeIndex = VG_PIPE_OVER;      break;  // VG_OPERATOR_OVER
	case vg_operator_t::VG_OPERATOR_CLEAR:  pipe = ctx->pipeCLEAR;    pipeIndex = VG_PIPE_CLEAR;     break;  // VG_OPERATOR_CLEAR
	case vg_operator_t::VG_OPERATOR_DIFFERENCE:  pipe = ctx->pipeSUB;      pipeIndex = VG_PIPE_SUB;       break;  // VG_OPERATOR_DIFFERENCE
	default:  break;
	}

	SDL_BindGPUGraphicsPipeline(pass, pipe);
	ctx->currentVgPipeIndex = pipeIndex;

	// ★ 根据管道类型自动设置正确的 stencil reference
	switch (pipeIndex) {
	case VG_PIPE_CLIPPING:
		// REPLACE 写入：ref = STENCIL_CLIP_BIT → 通过的像素 bit1 = 1
		SDL_SetGPUStencilReference(pass, STENCIL_CLIP_BIT);
		break;

	case VG_PIPE_OVER:
	case VG_PIPE_SUB:
		// EQUAL 比较：只画 bit1 == 1 的像素
		SDL_SetGPUStencilReference(pass, STENCIL_CLIP_BIT);
		break;
	case VG_PIPE_CLEAR:
		// stencil 已禁用，ref 无关
		SDL_SetGPUStencilReference(pass, 0);
		break;
	}
}

// ========================================================================
// ★ 显式设置 stencil ref（高级用法）
// ========================================================================
//
// 允许调用者在绑定管道后手动覆盖 ref 值。
// 典型场景：多级裁剪（bit2=0x4, bit3=0x8...）
//
// ⚠️ 注意：SDL3 GPU 不能动态改 compareMask，
// 所以多级裁剪的 compare_mask 必须提前在管道中设好。
// 这里只提供 ref 切换能力。
//
// ========================================================================
void ovg_set_stencil_reference(ovg_ctx_t* ctx, SDL_GPURenderPass* pass, uint8_t ref) {
	if (!ctx || !pass) return;
	SDL_SetGPUStencilReference(pass, ref);
}

// ========================================================================
// 设备创建与销毁
// ========================================================================
ovg_device_t* new_sdl3gpu_device(SDL_GPUDevice* gpuDevice) {
	if (!gpuDevice) return nullptr;

	ovg_device_t* dev = new ovg_device_t();
	dev->gpuDevice = gpuDevice;

	dev->supportedFormats = detect_supported_shader_format(gpuDevice);

	init_shader_modules(dev);

	// 创建默认空纹理
	dev->emptyTexture = new_texture(dev, SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM, 16, 16,
		SDL_GPU_TEXTUREUSAGE_SAMPLER |
		SDL_GPU_TEXTUREUSAGE_COLOR_TARGET);
	create_sampler(dev->emptyTexture,
		SDL_GPU_FILTER_NEAREST, SDL_GPU_FILTER_NEAREST,
		SDL_GPU_SAMPLERMIPMAPMODE_NEAREST,
		SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE);

	// 用白色填充空纹理
	SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(gpuDevice);
	if (cmd) {
		SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(cmd);
		if (copyPass) {
			uint32_t white = 0xFFFFFFFF;
			SDL_GPUTransferBufferCreateInfo tbi = {};
			tbi.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
			tbi.size = sizeof(uint32_t) * 16 * 16;
			SDL_GPUTransferBuffer* staging = SDL_CreateGPUTransferBuffer(gpuDevice, &tbi);
			if (staging) {
				void* mapped = SDL_MapGPUTransferBuffer(gpuDevice, staging, false);
				if (mapped) {
					auto dd = (char*)mapped;
					for (int i = 0; i < 16 * 16; i++)
						memcpy(dd + i * sizeof(int), &white, sizeof(white));
					SDL_UnmapGPUTransferBuffer(gpuDevice, staging);
				}
				SDL_GPUTextureTransferInfo tex_src = {};
				tex_src.transfer_buffer = staging;
				tex_src.rows_per_layer = dev->emptyTexture->height;
				tex_src.pixels_per_row = dev->emptyTexture->width;

				SDL_GPUTextureRegion tex_dst = {};
				tex_dst.texture = dev->emptyTexture->texture;
				tex_dst.x = 0;
				tex_dst.y = 0;
				tex_dst.w = dev->emptyTexture->width;
				tex_dst.h = dev->emptyTexture->height;
				tex_dst.d = 1;
				SDL_UploadToGPUTexture(copyPass, &tex_src, &tex_dst, false);
				SDL_EndGPUCopyPass(copyPass);
				SDL_SubmitGPUCommandBuffer(cmd);
				SDL_WaitForGPUIdle(gpuDevice);
				SDL_ReleaseGPUTransferBuffer(gpuDevice, staging);
			}
			else {
				SDL_EndGPUCopyPass(copyPass);
				SDL_SubmitGPUCommandBuffer(cmd);
			}
		}
		else {
			SDL_SubmitGPUCommandBuffer(cmd);
		}
	}

	return dev;
}

void free_sdl3gpu_device(ovg_device_t* dev) {
	if (!dev) return;

	if (dev->emptyTexture) {
		destroy_texture(dev->emptyTexture);
		dev->emptyTexture = nullptr;
	}
	destroy_shader_modules(dev);
	delete dev;
}

// ========================================================================
// 渲染上下文创建与销毁
// ========================================================================
ovg_ctx_t* new_ovgctx_sdl3(ovg_device_t* dev, SDL_GPUTextureFormat colorFormat, SDL_GPUTextureFormat depthFormat, SDL_GPUSampleCount samples) {
	if (!dev || !dev->gpuDevice) return nullptr;

	ovg_ctx_t* ctx = new ovg_ctx_t();
	ctx->device = dev;
	ctx->colorFormat = colorFormat;
	ctx->depthFormat = depthFormat;
	ctx->samples = samples;
	ctx->gpubuf = new OvgGpuBuffers();
	ctx->gpubuf->init(dev->gpuDevice);
	ctx->textures[0] = dev->emptyTexture;
	ctx->next_image_id++;
	init_vg_pipelines(ctx);

	return ctx;
}

void free_ovgctx_sdl3(ovg_ctx_t* ctx) {
	if (!ctx) return;
	SDL_WaitForGPUIdle(ctx->device->gpuDevice);
	for (auto p : ctx->freepipe)
	{
		if (p) SDL_ReleaseGPUGraphicsPipeline(ctx->device->gpuDevice, p);
	}
	for (auto& [k, v] : ctx->textures) {
		if (v) destroy_texture(v);
	}
	for (auto& [key, p] : ctx->geomPipelines) {
		if (p.pipeline)      SDL_ReleaseGPUGraphicsPipeline(ctx->device->gpuDevice, p.pipeline);
		if (p.defaultSampler) SDL_ReleaseGPUSampler(ctx->device->gpuDevice, p.defaultSampler);
	}
	ctx->textures.clear();
	ctx->freepipe.clear();
	ctx->geomPipelines.clear();

	delete ctx->gpubuf;

	delete ctx;
}

// ========================================================================
// FBO 管理
// ========================================================================
vg_fbo_t new_vgfbo_sdl3(ovg_ctx_t* ctx, int width, int height, SDL_Window* window) {
	vg_fbo_t fbo = {};
	fbo.width = width;
	fbo.height = height;
	if (!ctx || !ctx->device) return fbo;
	fbo.ctx = ctx;
	ovg_device_t* dev = ctx->device;
	if (window)
		fbo.window = window;
	else
		fbo.colorTex = new_texture(dev, ctx->colorFormat, width, height, SDL_GPU_TEXTUREUSAGE_SAMPLER | SDL_GPU_TEXTUREUSAGE_COLOR_TARGET);

	if (ctx->samples > SDL_GPU_SAMPLECOUNT_1) {
		fbo.colorTexMS = new_msaa_texture(dev, ctx->colorFormat, width, height, ctx->samples);
	}

	fbo.depthStencilTex = new_depth_stencil_texture(dev, ctx->depthFormat, width, height, ctx->samples);
	fbo.hasStencil = true;

	return fbo;
}

void free_vgfbo_sdl3(vg_fbo_t* fbo) {
	if (!fbo) return;
	if (fbo->colorTex) { destroy_texture((sdl3gpu_texture*)fbo->colorTex);     fbo->colorTex = nullptr; }
	if (fbo->colorTexMS) { destroy_texture((sdl3gpu_texture*)fbo->colorTexMS);   fbo->colorTexMS = nullptr; }
	if (fbo->depthStencilTex) { destroy_texture((sdl3gpu_texture*)fbo->depthStencilTex); fbo->depthStencilTex = nullptr; }
}

// ========================================================================
// MSAA 解析
// ========================================================================
void ovg_resolve_msaa_sdl3(ovg_ctx_t* ctx, SDL_GPUCommandBuffer* cmdBuf, vg_fbo_t* fbo) {
	if (!ctx || !cmdBuf || !fbo) return;
	if (!fbo->colorTexMS || !fbo->colorTex) return;
	return;
	SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(cmdBuf);
	if (!copyPass) return;

	SDL_GPUTextureRegion srcRegion = {};
	srcRegion.texture = ((sdl3gpu_texture*)fbo->colorTexMS)->texture;
	srcRegion.w = fbo->width;
	srcRegion.h = fbo->height;
	srcRegion.d = 1;

	SDL_GPUTextureRegion dstRegion = {};
	dstRegion.texture = ((sdl3gpu_texture*)fbo->colorTex)->texture;
	dstRegion.w = fbo->width;
	dstRegion.h = fbo->height;
	dstRegion.d = 1;

	//SDL_ResolveGPUTexture(copyPass, &srcRegion, &dstRegion);
	SDL_EndGPUCopyPass(copyPass);
}

void set_viewport2d(SDL_GPURenderPass* pass, const glm::ivec2& size) {
	SDL_GPUViewport view = { 0,0,0,0,0,1 };
	view.x = 0;
	view.y = size.y;
	view.w = size.x;
	view.h = -size.y;
	SDL_SetGPUViewport(pass, &view);
}
void set_viewport3d(SDL_GPURenderPass* pass, const glm::ivec2& size) {
	SDL_GPUViewport view = { 0,0,0,0,0,1 };
	view.x = 0;
	view.y = 0;
	view.w = size.x;
	view.h = size.y;
	SDL_SetGPUViewport(pass, &view);
}
// ========================================================================
// 帧生命周期
// ========================================================================
SDL_GPUCommandBuffer* ovg_begin_frame(ovg_ctx_t* ctx, vg_fbo_t* fbo, bool clearAll) {
	if (!ctx || !fbo) return nullptr;

	SDL_GPUCommandBuffer* cmdBuf = ctx->currentCmdBuf;
	if (!cmdBuf) return nullptr;

	sdl3gpu_texture* colorTex = (sdl3gpu_texture*)fbo->colorTex;
	sdl3gpu_texture* colorTexMS = (sdl3gpu_texture*)fbo->colorTexMS;
	sdl3gpu_texture* depthStencil = (sdl3gpu_texture*)fbo->depthStencilTex;

	auto ct = fbo->swapchain ? fbo->swapchain : colorTex->texture;
	if (!ct || !depthStencil) {
		SDL_SubmitGPUCommandBuffer(cmdBuf);
		return nullptr;
	}


	SDL_GPUTexture* renderColorTex = colorTexMS ? colorTexMS->texture : ct;

	SDL_GPUColorTargetInfo colorTargetInfo = {};
	colorTargetInfo.texture = renderColorTex;
	colorTargetInfo.load_op = clearAll ? SDL_GPU_LOADOP_CLEAR : SDL_GPU_LOADOP_LOAD;
	colorTargetInfo.store_op = (fbo && fbo->colorTexMS && ct) ? SDL_GPU_STOREOP_RESOLVE : SDL_GPU_STOREOP_STORE;
	colorTargetInfo.clear_color = { 0.0f, 0.0f, 0.0f, 0.0f };
	//colorTargetInfo.layer_count = 1;
	colorTargetInfo.resolve_mip_level = 0;
	colorTargetInfo.resolve_layer = 0;
	colorTargetInfo.resolve_texture = ct;

	SDL_GPUDepthStencilTargetInfo depthTargetInfo = {};
	depthTargetInfo.texture = depthStencil->texture;
	depthTargetInfo.load_op = clearAll ? SDL_GPU_LOADOP_CLEAR : SDL_GPU_LOADOP_LOAD;
	depthTargetInfo.store_op = SDL_GPU_STOREOP_STORE;
	depthTargetInfo.stencil_load_op = clearAll ? SDL_GPU_LOADOP_CLEAR : SDL_GPU_LOADOP_LOAD;
	depthTargetInfo.stencil_store_op = SDL_GPU_STOREOP_STORE;
	depthTargetInfo.clear_depth = 1.0f;
	depthTargetInfo.clear_stencil = 0;

	SDL_GPURenderPass* renderPass = SDL_BeginGPURenderPass(cmdBuf, &colorTargetInfo, 1, &depthTargetInfo);
	if (!renderPass) {
		SDL_SubmitGPUCommandBuffer(cmdBuf);
		return nullptr;
	}

	ctx->pass = renderPass;
	ctx->currentVgPipeIndex = -1;

	// 设置视口 
	SDL_Rect rc = { 0, 0, fbo->width, fbo->height };
	set_viewport2d(renderPass, glm::ivec2(fbo->width, fbo->height));
	SDL_SetGPUScissor(renderPass, &rc);
	ctx->cmdStarted = true;
	ctx->viewportW = fbo->width;
	ctx->viewportH = fbo->height;


	return cmdBuf;
}

SDL_GPURenderPass* ovg_get_current_render_pass(ovg_ctx_t* ctx) {
	if (!ctx) return nullptr;
	return ctx->pass;
}

void ovg_end_frame(ovg_ctx_t* ctx, vg_fbo_t* fbo) {
	if (!ctx) return;

	SDL_GPUCommandBuffer* actualCmd = ctx->currentCmdBuf;

	if (ctx->pass) {
		SDL_EndGPURenderPass(ctx->pass);
		ctx->pass = nullptr;
	}

	if (fbo && fbo->colorTexMS && fbo->colorTex) {
		ovg_resolve_msaa_sdl3(ctx, actualCmd, fbo);
	}

	ctx->cmdStarted = false;
	ctx->currentCmdBuf = nullptr;
	ctx->currentVgPipeIndex = -1;
}

// ========================================================================
// 纹理绑定
// ========================================================================
void ovg_bind_texture(ovg_ctx_t* ctx, SDL_GPUCommandBuffer* cmdBuf, SDL_GPURenderPass* pass, sdl3gpu_texture* tex) {
	if (!ctx || !pass) return;

	sdl3gpu_texture* actualTex = tex ? tex : ctx->device->emptyTexture;
	ctx->currentTexture = actualTex;

	SDL_GPUTextureSamplerBinding binding = {};
	binding.texture = actualTex->texture;
	binding.sampler = actualTex->sampler ? actualTex->sampler : ctx->device->emptyTexture->sampler;
	SDL_BindGPUFragmentSamplers(pass, 0, &binding, 1);
}


// ========================================================================
// 几何管道绑定
// ========================================================================
void ovg_bind_geom_pipeline(ovg_ctx_t* ctx, SDL_GPUCommandBuffer* cmdBuf, SDL_GPURenderPass* pass, const gem_info_t* info) {
	if (!ctx || !pass || !info) return;

	pipelinestate_p_internal* p = get_geom_pipeline(ctx, info);
	if (p && p->pipeline) {
		SDL_BindGPUGraphicsPipeline(pass, p->pipeline);
	}
}

// ========================================================================
// 绘制命令
// ========================================================================
void ovg_draw_indexed(ovg_ctx_t* ctx, SDL_GPUCommandBuffer* cmdBuf, SDL_GPURenderPass* pass,
	uint32_t indexCount, uint32_t indexOffset, uint32_t vertexOffset) {
	if (!ctx || !pass) return;
	SDL_DrawGPUIndexedPrimitives(pass, indexCount, 1, indexOffset, vertexOffset, 0);
}

void ovg_draw_arrays(ovg_ctx_t* ctx, SDL_GPUCommandBuffer* cmdBuf, SDL_GPURenderPass* pass,
	uint32_t vertexCount, uint32_t vertexOffset) {
	if (!ctx || !pass) return;
	SDL_DrawGPUPrimitives(pass, vertexCount, 1, vertexOffset, 0);
}

// ========================================================================
// 数据上传
// ========================================================================

// ========================================================================
// 几何缓冲区绑定
// ========================================================================
void ovg_bind_geom_buffers(ovg_ctx_t* ctx, SDL_GPUCommandBuffer* cmdBuf, SDL_GPURenderPass* pass,
	uint32_t vboOffset, uint32_t iboOffset) {
	if (!ctx || !pass) return;

	//SDL_GPUBufferBinding vboBinding = {};
	//vboBinding.buffer = ctx->vboGeom.buffer;
	//vboBinding.offset = vboOffset;
	//SDL_BindGPUVertexBuffers(pass, 0, &vboBinding, 1);

	//SDL_GPUBufferBinding iboBinding = {};
	//iboBinding.buffer = ctx->iboGeom.buffer;
	//iboBinding.offset = iboOffset;
	//SDL_BindGPUIndexBuffer(pass, &iboBinding, SDL_GPU_INDEXELEMENTSIZE_32BIT);
}

// ========================================================================
// 视口与裁剪
// ========================================================================
void ovg_set_viewport(ovg_ctx_t* ctx, SDL_GPUCommandBuffer* cmdBuf, SDL_GPURenderPass* pass,
	float x, float y, float w, float h) {
	if (!ctx || !pass) return;
	SDL_GPUViewport vp = {};
	vp.x = x; vp.y = y; vp.w = w; vp.h = h;
	vp.min_depth = 0.0f; vp.max_depth = 1.0f;
	SDL_SetGPUViewport(pass, &vp);
}

void ovg_set_scissor(SDL_GPURenderPass* pass, int x, int y, int w, int h) {
	if (!pass) return;
	SDL_Rect rc = { x, y, w, h };
	SDL_SetGPUScissor(pass, &rc);
}

void ovg_reset_scissor(ovg_ctx_t* ctx, SDL_GPURenderPass* pass) {
	if (!ctx || !pass) return;
	SDL_Rect rc = { 0, 0, ctx->viewportW, ctx->viewportH };
	SDL_SetGPUScissor(pass, &rc);
}

// ========================================================================
// 命令缓冲区管理
// ========================================================================
SDL_GPUCommandBuffer* ovg_get_command_buffer(ovg_ctx_t* ctx) {
	if (!ctx || !ctx->device) return nullptr;
	return SDL_AcquireGPUCommandBuffer(ctx->device->gpuDevice);
}

void ovg_submit_command_buffer(SDL_GPUCommandBuffer* cmdBuf) {
	if (!cmdBuf) return;
	SDL_SubmitGPUCommandBuffer(cmdBuf);
}

void ovg_wait_idle(ovg_ctx_t* ctx) {
	if (!ctx || !ctx->device) return;
	SDL_WaitForGPUIdle(ctx->device->gpuDevice);
}

void ovg_sort_gradient_stops(vg_gradient_t* grad, float* stops, uint32_t count) {
	auto colors = grad->colors;
	for (uint32_t i = 1; i < count; i++) {
		float   key_stop = stops[i];
		auto key_color = colors[i];
		int j = (int)i - 1;
		while (j >= 0 && stops[j] > key_stop) {
			stops[j + 1] = stops[j];
			colors[j + 1] = colors[j];
			j--;
		}
		stops[j + 1] = key_stop;
		colors[j + 1] = key_color;
	}
}
void ovg_mul_pat(vg_gradient_t* grad, int type, const glm::mat3& mat, const glm::mat3& pat_mat, bool hasMatrix) {
	glm::vec3 cp0[2] = { glm::vec3(grad->cp[0].x, grad->cp[0].y,1.0f) ,glm::vec3(grad->cp[0].z, grad->cp[0].w,1.0f) };
	glm::vec3 cp1[2] = { glm::vec3(grad->cp[1].x, grad->cp[1].y,1.0f) ,glm::vec3(grad->cp[1].z, grad->cp[1].w,1.0f) };
	auto m = mat * glm::inverse(pat_mat);
	cp0[0] = m * cp0[0];
	if (type == vg_pattern_type_t::VG_PATTERN_TYPE_LINEAR) {
		cp0[1] = m * cp0[1];
	}
	else {
		cp1[0] = m * cp1[0];
		// radii
		cp0[1].z = 0.0;
		cp0[1] = m * cp0[1];
		cp1[1].z = 0.0;
		cp1[1] = m * cp1[1];
	}
	grad->cp[0] = glm::vec4(glm::vec2(cp0[0]), glm::vec2(cp0[1]));
	grad->cp[1] = glm::vec4(glm::vec2(cp1[0]), glm::vec2(cp1[1]));
	ovg_sort_gradient_stops(grad, grad->stops, grad->count);
}

SDL_Rect set_scissor_sdl3(vg_fbo_t* fbo, SDL_GPURenderPass* pass, glm::vec4* scissor)
{
	SDL_Rect r = { 0,0,-1,-1 };
	if (pass)
	{
		r = { 0,0,(int)fbo->width,(int)fbo->height };
		if (scissor)
		{
			r.x = scissor->x; r.y = scissor->y;
			r.w = scissor->z;
			if (scissor->x < 0)
			{
				r.w += scissor->x;
				r.x = 0;
			}
			r.h = scissor->w;
			if (scissor->y < 0)
			{
				r.h += scissor->y;
				r.y = 0;
			}
			r.w = (uint32_t)glm::max(scissor->z, 1.0f); r.h = (uint32_t)std::max(scissor->w, 1.0f);
		}
		SDL_SetGPUScissor(pass, &r);
	}
	return r;
}

void cmd_draw_full_screen_quad_sdl3(vg_fbo_t* fbo, SDL_GPURenderPass* pass, vgcmd_t* c, glm::vec4* scissor, SDL_Rect* clip, push_constants_t& pc)
{
#if defined(_DEBUG)
	SDL_PushGPUDebugGroup(fbo->cmd, "_draw_full_screen_quad");
#endif
	SDL_Rect bounds = { 0,0,fbo->width, fbo->height };
	if (scissor) {
		SDL_Rect r = { (int32_t)glm::max((int)scissor->x, 0), (int32_t)glm::max((int)scissor->y, 0),
					 (int32_t)glm::max((int)scissor->z - (int32_t)scissor->x + 1, 1),
					   (int32_t)glm::max((int)scissor->w - (int32_t)scissor->y + 1, 1) };
		SDL_SetGPUScissor(pass, &r);
	}
	uint32_t firstVertIdx = c->full_screen_quad;
	if (c->state)
		pc.fsq_patternType = c->state->pushConsts.fsq_patternType;
	pc.fsq_patternType |= FULLSCREEN_BIT;
	SDL_PushGPUVertexUniformData(fbo->cmd, 0, &pc, sizeof(pc));
	SDL_DrawGPUPrimitives(pass, 3, 1, firstVertIdx, 0);
	pc.fsq_patternType &= ~FULLSCREEN_BIT;
	SDL_PushGPUVertexUniformData(fbo->cmd, 0, &pc, sizeof(pc));
	if (scissor)
		SDL_SetGPUScissor(pass, clip && clip->w > 0 && clip->h > 0 ? clip : &bounds);
#if defined(_DEBUG) 
	SDL_PopGPUDebugGroup(fbo->cmd);
#endif
}
void vgClipReset(ovg_ctx_t* ctx) {
	SDL_BindGPUGraphicsPipeline(ctx->pass, ctx->pipeStencilClear);
	SDL_SetGPUStencilReference(ctx->pass, 0);
	SDL_DrawGPUPrimitives(ctx->pass, 3, 1, 0, 0);

	ctx->clipDepth = 0;
}
void vgClipPush(ovg_ctx_t* ctx) {
	if (ctx->clipDepth >= 255) return; // 防溢出

	ctx->clipDepth++;

	// ① 非零填充
	SDL_BindGPUGraphicsPipeline(ctx->pass, ctx->pipeStencilFill);
	SDL_SetGPUStencilReference(ctx->pass, 0);
	//_vgDrawCurrentShape(ctx); // 你自己的“画当前形状”函数

	// ② 非零 → 当前深度
	SDL_BindGPUGraphicsPipeline(ctx->pass, ctx->pipeClipInc);
	SDL_SetGPUStencilReference(ctx->pass, ctx->clipDepth - 1);
	SDL_DrawGPUPrimitives(ctx->pass, 3, 1, 0, 0);
}
void vgClipPop(ovg_ctx_t* ctx) {
	if (ctx->clipDepth <= 0) return;

	SDL_BindGPUGraphicsPipeline(ctx->pass, ctx->pipeClipDec);
	SDL_SetGPUStencilReference(ctx->pass, ctx->clipDepth);
	SDL_DrawGPUPrimitives(ctx->pass, 3, 1, 0, 0);

	ctx->clipDepth--;
}
bool vgClipIsActive(const ovg_ctx_t* ctx) {
	return ctx->clipDepth > 0;
}

int vgClipGetDepth(const ovg_ctx_t* ctx) {
	return ctx->clipDepth;
}
void vgDrawRect(ovg_ctx_t* ctx, float x, float y, float w, float h) {
	if (vgClipIsActive(ctx)) {
		SDL_BindGPUGraphicsPipeline(ctx->pass, ctx->pipeStencilDraw);
		SDL_SetGPUStencilReference(ctx->pass, ctx->clipDepth);
	}
	else {
		// 无裁剪时，用普通 pipeline（stencil test 关闭）
		SDL_BindGPUGraphicsPipeline(ctx->pass, ctx->pipeOVER);
	}

	//_vgDrawRectCmd(ctx, x, y, w, h);
}

void draw_ct(SDL_GPURenderPass* pass, vgcmd_t* c)
{
	if (c->index.y > 0)
	{
		SDL_DrawGPUIndexedPrimitives(pass, c->index.y, 1, c->index.x, static_cast<int32_t>(c->vertex.x), 0);
	}
	else
	{
		SDL_DrawGPUPrimitives(pass, c->vertex.y, 1, c->vertex.x, 0);
	}
}
void draw_vg(vg_fbo_t* fbo, SDL_GPURenderPass* pass, vgcmd_t* c, SDL_Rect* cuclip, const glm::uvec2& offset)
{
	push_constants_t pc = {};
	auto t = c->state;
	fbo->ctx->gpubuf->bindVBO(pass, offset.x);
	fbo->ctx->gpubuf->bindIBO(pass, offset.y);
	SDL_GPUTextureSamplerBinding binding = { .texture = fbo->ctx->device->emptyTexture->texture,	.sampler = fbo->ctx->device->emptyTexture->sampler, };

	if (t) {
		int smax = std::max(fbo->width, fbo->height);
		pc = t->pushConsts;
		pc.size = { fbo->width,fbo->height };
		ovg_bind_vg_pipeline(fbo->ctx, fbo->cmd, pass, (int)t->curOperator);
		if (t->pattern && t->pattern->type != vg_pattern_type_t::VG_PATTERN_TYPE_SOLID) {
			pc.source = { smax,smax,0,0 };
		}
		glm::mat3x3 inv = pc.mat;
		pc.matInv = glm::inverse(inv);
		if (t->pattern)
		{
			pc.fsq_patternType = (pc.fsq_patternType & FULLSCREEN_BIT) + t->pattern->type;
			if (t->pattern->type == vg_pattern_type_t::VG_PATTERN_TYPE_SURFACE) {
				auto tex = (sdl3gpu_texture*)t->pattern->data;
				if (tex)
				{
					binding.texture = tex->texture;
					binding.sampler = tex->sampler;
					pc.source.x;// 在vg设置了
					pc.source.y;
					pc.source.z = (float)fbo->width;
					pc.source.w = (float)fbo->height;
					glm::mat3x3 mat;
					if (t->pattern->hasMatrix) {
						mat = t->pattern->matrix;
						pc.matInv = pc.matInv * mat;
					}
				}
			}
		}
		if (t->pattern && t->pattern->data) {
			auto gr = *((vg_gradient_t*)t->pattern->data);
			glm::mat3 patmat = glm::mat3(1.0);
			if (t->pattern->hasMatrix)
			{
				patmat = t->pattern->matrix;
			}
			ovg_mul_pat(&gr, t->pattern->type, pc.mat, patmat, t->pattern->hasMatrix);
			SDL_PushGPUFragmentUniformData(fbo->cmd, 0, &gr, sizeof(vg_gradient_t));
		}
		else {
			vg_gradient_t gr = {};
			SDL_PushGPUFragmentUniformData(fbo->cmd, 0, &gr, sizeof(vg_gradient_t));
		}
		SDL_PushGPUVertexUniformData(fbo->cmd, 0, &pc, sizeof(pc));
	}
	else {
		pc.mat = glm::mat3x2(1.0);
		glm::mat3x3 inv = pc.mat;
		pc.matInv = glm::inverse(inv);
		ovg_bind_vg_pipeline(fbo->ctx, fbo->cmd, pass, (int)vg_operator_t::VG_OPERATOR_SOURCE);
		SDL_PushGPUVertexUniformData(fbo->cmd, 0, &pc, sizeof(pc));
	}
	SDL_BindGPUFragmentSamplers(pass, 0, &binding, 1);
	switch (c->type) {
	case 0:
	case 1:
		draw_ct(pass, c);
		break;
	case 2:
	{
		const int bw = c->bounds.z;
		const int bh = c->bounds.w;
		if (bw != 0 && bh != 0) {
			*cuclip = set_scissor_sdl3(fbo, pass, (bw < 0 || bh < 0) ? nullptr : &c->bounds);
			break;
		}
		if (c->vertex.y > 0 || c->index.y > 0) {
#if defined(_DEBUG)
			SDL_PushGPUDebugGroup(fbo->cmd, "clip");
#endif 
			SDL_BindGPUGraphicsPipeline(pass, fbo->ctx->pipeClipping);
			SDL_SetGPUStencilReference(pass, STENCIL_CLIP_BIT);
			draw_ct(pass, c);
#if defined(_DEBUG)
			SDL_PopGPUDebugGroup(fbo->cmd);
#endif
		}
		else if (c->full_screen_quad >= 0) {
			SDL_BindGPUGraphicsPipeline(pass, fbo->ctx->pipeStencilClear);
			SDL_SetGPUStencilReference(pass, c->ref);
			cmd_draw_full_screen_quad_sdl3(fbo, pass, c, nullptr, 0, pc);
			SDL_SetGPUStencilReference(pass, STENCIL_CLIP_BIT);
		}
	}
	break;
	case 3:
	{
		cmd_draw_full_screen_quad_sdl3(fbo, pass, c, nullptr, 0, pc);
	}
	break;
	case 4:
	{
		//SDL_ClearGPUColorTarget(cmd, pass, &colorTarget);
		//SDL_ClearGPUDepthStencilTarget(cmd, pass, &depthStencilTarget);
	}
	break;
	}
}

void draw_geom(vg_fbo_t* fbo, SDL_GPURenderPass* pass, geom_cmd_t* c, const glm::uvec3& offset)
{
	if (!c)return;
	pipelinestate_p_internal* pipe = get_geom_pipeline(fbo->ctx, &c->state);
	if (pipe && pipe->pipeline) {
		SDL_BindGPUGraphicsPipeline(pass, pipe->pipeline);
	}
	fbo->ctx->gpubuf->bindVBO(pass, c->v_offset ? offset.y : offset.x);
	fbo->ctx->gpubuf->bindIBO(pass, offset.z);
	if (c->instance_count > 1)
		fbo->ctx->gpubuf->bindSSBO(pass, true);
	SDL_GPUTextureSamplerBinding binding = { .texture = fbo->ctx->device->emptyTexture->texture,	.sampler = fbo->ctx->device->emptyTexture->sampler, };
	if (c->texture) {
		auto texid = (vg_image_t*)c->texture;
		auto tex = fbo->ctx->textures[texid->id];
		if (tex)
		{
			binding.texture = tex->texture;
			binding.sampler = tex->sampler;
		}
	}
	SDL_BindGPUFragmentSamplers(pass, 0, &binding, 1);
	if (c->texture_mask) {
		auto tex = (sdl3gpu_texture*)c->texture_mask;
		if (tex)
		{
			binding.texture = tex->texture;
			binding.sampler = tex->sampler;
		}
		SDL_BindGPUFragmentSamplers(pass, 1, &binding, 1);
		SDL_PushGPUFragmentUniformData(fbo->cmd, 0, &c->mask_time, sizeof(float) * 2);
	}
	struct PushConsts
	{
		glm::mat4 mvp;
		uint32_t instance_pos;
	};
	PushConsts pc = { c->mat, c->instance_ssbo_pos };
	SDL_PushGPUVertexUniformData(fbo->cmd, 0, &pc, sizeof(PushConsts));
	if (c->firstIndex >= 0)
	{
		SDL_DrawGPUIndexedPrimitives(pass, c->elemCount, c->instance_count > 1 ? c->instance_count : 1, c->firstIndex, c->vertexOffset, 0);
	}
	else
	{
		SDL_DrawGPUPrimitives(pass, c->elemCount, c->instance_count > 1 ? c->instance_count : 1, c->vertexOffset, 0);
	}
}

SDL_GPUCommandBuffer* ovg_get_window_swapchain(ovg_ctx_t* ctx, vg_fbo_t* fbo) {

	if (!ctx || !fbo || !(fbo->window || fbo->colorTex))return 0;
	SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(ctx->device->gpuDevice);
	if (!cmd)return 0;
	uint32_t sw = 0, sh = 0;
	SDL_GPUTexture* swapchain = NULL;
	if (fbo->window) {
		bool aq = SDL_AcquireGPUSwapchainTexture(cmd, fbo->window, &swapchain, &sw, &sh);
		fbo->display_size = { sw,sh };
		if (!aq || !sw || !sh) {
			SDL_CancelGPUCommandBuffer(cmd);
			return 0;
		}
		if (fbo->width != sw || fbo->height != sh)
		{
			SDL_WaitForGPUSwapchain(ctx->device->gpuDevice, fbo->window);
			free_vgfbo_sdl3(fbo);
			*fbo = new_vgfbo_sdl3(ctx, sw, sh, fbo->window);
			fbo->display_size = { sw,sh };
		}
	}

	fbo->swapchain = swapchain;
	fbo->cmd = cmd;
	return cmd;
}
void upload_pixels(ovg_ctx_t* r, sdl3gpu_texture* tex, vg_image_desc_t* desc)
{
	uint32_t bpp = (desc->format == VG_FORMAT_RGBA8) ? 4 : desc->stride;
	uint32_t pitch = desc->width * bpp;
	uint32_t size = pitch * desc->height;
	if (size == 0) return;

	SDL_GPUTransferBufferCreateInfo ti = {
		.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
		.size = size,
	};
	auto dev = r->device->gpuDevice;
	SDL_GPUTransferBuffer* tb = SDL_CreateGPUTransferBuffer(dev, &ti);
	if (!tb) return;

	void* ptr = SDL_MapGPUTransferBuffer(dev, tb, false);
	if (ptr) {
		SDL_memcpy(ptr, desc->pixels, size);
		SDL_UnmapGPUTransferBuffer(dev, tb);
	}

	SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(dev);
	SDL_GPUCopyPass* cp = SDL_BeginGPUCopyPass(cmd);

	SDL_GPUTextureTransferInfo src = { .transfer_buffer = tb, .offset = 0 };
	SDL_GPUTextureRegion dst = {
		.texture = tex->texture, .x = desc->x, .y = desc->y, .z = 0,
		.w = desc->w, .h = desc->h, .d = 1,
	};
	SDL_UploadToGPUTexture(cp, &src, &dst, false);

	SDL_EndGPUCopyPass(cp);
	SDL_SubmitGPUCommandBuffer(cmd);

	SDL_ReleaseGPUTransferBuffer(dev, tb);
}


struct upload_desc_t {
	void* pixels;     /* CPU 像素数据，调用方持有生命周期 */
	uint32_t width;
	uint32_t height;
	uint32_t stride;     /* 每行字节数（0 = 按 bpp * width 自动算） */
	vg_format_t format;     /* 像素格式 */
	sdl3gpu_texture* dst_texture;/* 目标 GPU 纹理 */
	uint32_t x, y;       /* 目标区域起点（纹理空间） */
	uint32_t w, h;       /* 目标区域尺寸（0 = 整张） */
	vg_image_t* img;
	bool is_copy_rect;		// 是否从区域复制
};

/* 批量上传结果 */
typedef struct {
	uint32_t           submitted;  /* 实际提交的数量 */
	uint32_t           skipped;    /* 因 size==0 / 非法而跳过的数量 */
} upload_result_t;

/*
 * 批量上传：只创建一个 SDL_GPUTransferBuffer，所有纹理数据连续拷贝，
 * 用 offset 寻址，一次 CopyPass + 一次 Submit 全部推上去。
 *
 * 要求：
 *   - descs[i].pixels 必须在本次调用期间保持有效（在 Submit 之前）
 *   - 所有 dst_texture 属于同一个 SDL_GPUDevice
 */
upload_result_t upload_pixels_batched(SDL_GPUDevice* device, SDL_GPUCommandBuffer* cmd, const upload_desc_t* descs, uint32_t count);

uint32_t format_bpp(vg_format_t fmt) {
	switch (fmt) {
	case VG_FORMAT_RGBA8:
	case VG_FORMAT_BGRA8:
	case VG_FORMAT_RGBA8_SRGB:
	case VG_FORMAT_BGRA8_SRGB:return 4;
	case VG_FORMAT_RGBA16F: return 8;   /* R16G16B16A16_FLOAT = 8 字节 */
	case VG_FORMAT_RGBA32F: return 16;  /* R32G32B32A32_FLOAT = 16 字节 */
	default: return 4;
	}
}

upload_result_t upload_pixels_batched(
	SDL_GPUDevice* device,
	SDL_GPUCommandBuffer* cmd,
	const upload_desc_t* descs,
	uint32_t count)
{
	upload_result_t result = { 0, 0 };

	if (!device || !descs || count == 0) return result;

	const uint32_t align = 256;

	struct per_item_t {
		uint32_t src_offset;       /* CPU 源像素起始字节（原始纹理坐标系） */
		uint32_t gpu_offset;       /* transfer buffer 中的字节偏移 */
		uint32_t pixels_per_row;   /* 传给 SDL：源 buffer 每行像素数（0=紧密） */
		uint32_t copy_h;           /* 拷贝行数（= 脏矩形高） */
		uint32_t dirty_w;          /* 脏矩形宽 */
		uint32_t dirty_h;          /* 脏矩形高 */
		uint32_t bpp;              /* 每像素字节 */
		uint32_t stride;           /* 原始纹理每行字节（含 padding） */
		uint32_t row_pitch;        /* memcpy 每行字节数（= dirty_w * bpp） */
		uint32_t gpu_row_bytes;    /* buffer 里每行实际字节（紧密 or 对齐后） */
	};
	std::vector<per_item_t> items(count);

	uint32_t total_size = 0;

	/* ── 1. 预扫描：计算每张图的源偏移、buffer 布局、对齐 ── */
	for (uint32_t i = 0; i < count; i++) {
		const upload_desc_t* d = &descs[i];

		/* 非法条目跳过 */
		if (!d->pixels || !d->dst_texture || d->width == 0 || d->height == 0) {
			items[i].copy_h = 0;
			result.skipped++;
			continue;
		}

		uint32_t bpp = format_bpp(d->format);
		uint32_t stride = (d->stride != 0) ? d->stride : (d->width * bpp);

		/* 目标区域（纹理空间），0 表示整张 */
		uint32_t dst_x = d->x;
		uint32_t dst_y = d->y;
		uint32_t dst_w = (d->w != 0) ? d->w : d->width;
		uint32_t dst_h = (d->h != 0) ? d->h : d->height;

		/* 裁剪到纹理边界，防止越界 */
		if (dst_x + dst_w > d->width)  dst_w = d->width - dst_x;
		if (dst_y + dst_h > d->height) dst_h = d->height - dst_y;
		if (dst_w == 0 || dst_h == 0) {
			items[i].copy_h = 0;
			result.skipped++;
			continue;
		}

		bool is_full_upload = (dst_x == 0 && dst_y == 0 &&
			dst_w == d->width && dst_h == d->height);

		uint32_t dirty_pitch = dst_w * bpp;  /* 每行有效像素字节数 */

		if (is_full_upload) {
			/* ── 整图上传：紧密打包，pixels_per_row = 0 ── */
			items[i].src_offset = 0;
			items[i].gpu_offset = total_size;
			items[i].pixels_per_row = 0;          /* 驱动按 region.w 推导行距 */
			items[i].copy_h = d->height;
			items[i].dirty_w = d->width;
			items[i].dirty_h = d->height;
			items[i].bpp = bpp;
			items[i].stride = stride;
			items[i].row_pitch = d->width * bpp;
			items[i].gpu_row_bytes = d->width * bpp;  /* 紧密：行距 = 行宽 */
			total_size += stride * d->height;
		}
		else {
			/* ── 脏矩形上传：buffer 每行对齐 256 字节 ── */
			uint32_t aligned_pitch = align_up(dirty_pitch, align);

			items[i].src_offset = dst_y * stride + dst_x * bpp;
			items[i].gpu_offset = total_size;
			items[i].pixels_per_row = aligned_pitch / bpp;  /* ← 关键：源 buffer 真实行像素宽 */
			items[i].copy_h = dst_h;
			items[i].dirty_w = dst_w;
			items[i].dirty_h = dst_h;
			items[i].bpp = bpp;
			items[i].stride = stride;
			items[i].row_pitch = dirty_pitch;          /* memcpy 只拷有效部分 */
			items[i].gpu_row_bytes = aligned_pitch;        /* buffer 行距 = 对齐后 */
			total_size += aligned_pitch * dst_h;
		}
	}

	if (total_size == 0) return result;

	/* ── 2. 创建唯一一个 transfer buffer ── */
	SDL_GPUTransferBufferCreateInfo ti = {
		.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
		.size = total_size,
	};
	SDL_GPUTransferBuffer* tb = SDL_CreateGPUTransferBuffer(device, &ti);
	if (!tb) return result;

	/* ── 3. 映射一次，逐张拷贝（各自按 gpu_offset / gpu_row_bytes 落位） ── */
	uint8_t* base = (uint8_t*)SDL_MapGPUTransferBuffer(device, tb, false);
	if (base) {
		for (uint32_t i = 0; i < count; i++) {
			if (items[i].copy_h == 0) continue;

			const upload_desc_t* d = &descs[i];
			const per_item_t* it = &items[i];

			uint8_t* src = (uint8_t*)d->pixels + it->src_offset;   /* 原始纹理脏区起点 */
			uint8_t* dst = base + it->gpu_offset;                    /* buffer 脏区起点 */

			/* 逐行拷贝：源按 stride 跳行，目的按 gpu_row_bytes 跳行 */
			for (uint32_t row = 0; row < it->copy_h; row++) {
				SDL_memcpy(
					dst + row * it->gpu_row_bytes,
					src + row * it->stride,
					it->row_pitch
				);
			}
		}
		SDL_UnmapGPUTransferBuffer(device, tb);
	}

	/* ── 4. 一个 CopyPass，全部上传（offset 寻址） ── */
	bool own_cmd = (cmd == NULL);
	if (own_cmd) cmd = SDL_AcquireGPUCommandBuffer(device);
	SDL_GPUCopyPass* cp = SDL_BeginGPUCopyPass(cmd);

	for (uint32_t i = 0; i < count; i++) {
		if (items[i].copy_h == 0) continue;

		const upload_desc_t* d = &descs[i];
		const per_item_t* it = &items[i];

		SDL_GPUTextureTransferInfo src = {
			.transfer_buffer = tb,
			.offset = it->gpu_offset,
			.pixels_per_row = it->pixels_per_row,   /* 0 或 aligned_pitch/bpp */
			.rows_per_layer = 0,
		};
		SDL_GPUTextureRegion dst = {
			.texture = d->dst_texture->texture,
			.x = d->x,  .y = d->y,  .z = 0,
			.w = it->dirty_w,
			.h = it->copy_h,
			.d = 1,
		};
		SDL_UploadToGPUTexture(cp, &src, &dst, false);
		result.submitted++;
	}

	SDL_EndGPUCopyPass(cp);
	if (own_cmd) SDL_SubmitGPUCommandBuffer(cmd);

	/* ── 5. 提交后释放 transfer buffer（GPU 已读完，引用计数回收） ── */
	SDL_ReleaseGPUTransferBuffer(device, tb);
	return result;
}
#if 0
struct pending_upload_t {
	SDL_GPUTransferBuffer* tb;
	SDL_GPUTexture* texture;
	uint32_t               x, y, w, h;
};
void upload_pixels_batched(ovg_ctx_t* r, vg_image_desc_t* descs, sdl3gpu_texture** texs, uint32_t count)
{
	auto dev = r->device->gpuDevice;
	std::vector<pending_upload_t> uploads;
	uploads.reserve(count);

	// ── 第一遍：建所有 transfer buffer + 拷贝数据 ──
	for (uint32_t i = 0; i < count; i++) {
		vg_image_desc_t* desc = &descs[i];
		sdl3gpu_texture* tex = texs[i];

		uint32_t bpp = (desc->format == VG_FORMAT_RGBA8) ? 4 : desc->stride;
		uint32_t pitch = desc->width * bpp;
		uint32_t size = pitch * desc->height;
		if (size == 0) continue;

		SDL_GPUTransferBufferCreateInfo ti = {
			.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
			.size = size,
		};
		SDL_GPUTransferBuffer* tb = SDL_CreateGPUTransferBuffer(dev, &ti);
		if (!tb) continue;

		void* ptr = SDL_MapGPUTransferBuffer(dev, tb, false);
		if (ptr) {
			SDL_memcpy(ptr, desc->pixels, size);
			SDL_UnmapGPUTransferBuffer(dev, tb);
		}

		uploads.push_back({ tb, tex->texture, desc->x, desc->y, desc->w, desc->h });
	}

	if (uploads.empty()) return;

	// ── 第二遍：一次 command buffer + 一次 copy pass 提交全部 ──
	SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(dev);
	SDL_GPUCopyPass* cp = SDL_BeginGPUCopyPass(cmd);

	for (auto& up : uploads) {
		SDL_GPUTextureTransferInfo src = {
			.transfer_buffer = up.tb,
			.offset = 0,
		};
		SDL_GPUTextureRegion dst = {
			.texture = up.texture,
			.x = up.x, .y = up.y, .z = 0,
			.w = up.w, .h = up.h, .d = 1,
		};
		SDL_UploadToGPUTexture(cp, &src, &dst, false);
	}

	SDL_EndGPUCopyPass(cp);
	SDL_SubmitGPUCommandBuffer(cmd);

	// ── 释放所有 transfer buffer ──
	for (auto& up : uploads) {
		SDL_ReleaseGPUTransferBuffer(dev, up.tb);
	}
}
#endif
/* ---- 延迟释放 ---- */

void deferred_free_push(ovg_ctx_t* ctx, sdl3gpu_texture* tex) {
	ctx->texq.push(tex);
}

void deferred_free_advance(ovg_ctx_t* r) {
	auto dev = r->device->gpuDevice;
	if (r->texq.size())
	{
		SDL_WaitForGPUIdle(r->device->gpuDevice);
		for (; r->texq.size();)
		{
			auto p = r->texq.front();
			if (p && p->id > 0)
			{
				r->textures.erase(p->id);
				destroy_texture(p);
			}
			r->texq.pop();
		}
	}
}

int build_devres(ovg_ctx_t* ctx, SDL_GPUCommandBuffer* cmd, ovg_draw_data_t* kd) {
	int errornum = 0;
	for (size_t y = 0; y < kd->pipeinfo_count; y++)
	{
		auto it = kd->pipeinfo + y;
		if (it)
		{
			pipelinestate_p_internal* p = get_geom_pipeline(ctx, it);
			if (!p) {
				errornum++;
			}
		}
	}
	std::vector<upload_desc_t> uploads;
	for (size_t i = 0; i < kd->image_desc_count; i++)
	{
		auto it = kd->image_desc[i];
		auto tex = ctx->textures[it->img->id];
		if (tex && it->is_destroy && tex != ctx->device->emptyTexture)
		{
			ctx->texq.push(tex);
		}
		if (it->img->id == 0)tex = 0;
		if (!tex) {
			tex = new_texture_def(ctx, it->width, it->height, it->format);
			if (tex)
			{
				create_sampler(tex, SDL_GPU_FILTER_NEAREST, SDL_GPU_FILTER_NEAREST, SDL_GPU_SAMPLERMIPMAPMODE_NEAREST, SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE);
				it->img->id = ctx->next_image_id++;
				tex->id = it->img->id;
				ctx->textures[it->img->id] = tex;
			}
			else {
				it->img->id = 0;
			}
		}
		if (!tex)continue;
		upload_desc_t d = {};
		d.pixels = (void*)it->pixels;     /* CPU 像素数据，调用方持有生命周期 */
		d.width = it->width;
		d.height = it->height;
		d.stride = it->stride;     /* 每行字节数（0 = 按 bpp * width 自动算） */
		d.format = it->format;     /* 像素格式 */
		d.dst_texture = tex;/* 目标 GPU 纹理 */
		d.x = it->x, d.y = it->y;       /* 目标区域起点（纹理空间） */
		d.w = it->w, d.h = it->h;       /* 目标区域尺寸（0 = 整张） */
		d.is_copy_rect = !it->is_copy;
		d.img = it->img;
		uploads.push_back(d);
	}

	upload_result_t hr = upload_pixels_batched(ctx->device->gpuDevice, cmd, uploads.data(), uploads.size());
	return errornum;
}
void ovg_draw_data(ovg_ctx_t* ctx, vg_fbo_t* fbo, ovg_draw_data_t* data, size_t count)
{
	if (!ctx || !fbo || !(fbo->window || fbo->colorTex) || !data || !count)return;
	if (!fbo->cmd)return;
	auto cmd = fbo->cmd;
	int smax = std::max(fbo->width, fbo->height);
	ctx->currentCmdBuf = fbo->cmd;
	size_t total_vbo = 0;
	size_t total_ibo = 0;
	size_t total_ssbo = 0;
	int errornum = 0;
	for (size_t i = 0; i < count; i++) {
		auto kd = &data[i];
		total_vbo += data[i].v_count * sizeof(ovgVertex);
		total_vbo += data[i].v1_count * sizeof(geomVertex1);
		total_vbo += data[i].v2_count * sizeof(geomVertex2);

		total_ibo += data[i].i_count * sizeof(uint32_t);
		total_ibo += data[i].ig_count * sizeof(uint32_t);
		total_ssbo += data[i].instance_count * sizeof(glm::mat4);
		errornum += build_devres(ctx, cmd, kd);
	}

	assert(errornum == 0);

	ctx->gpubuf->begin(total_vbo, total_ibo, total_ssbo);

	for (size_t i = 0; i < count; i++) {
		auto kd = &data[i];
		kd->vg_offset.x = ctx->gpubuf->add_vbo(kd->vg_vertex, kd->v_count * sizeof(ovgVertex));
		kd->vg_offset.y = ctx->gpubuf->add_ibo(kd->vg_indices, kd->i_count * sizeof(uint32_t));
		kd->geom_offset.x = ctx->gpubuf->add_vbo(kd->vertex1, kd->v1_count * sizeof(geomVertex1));
		kd->geom_offset.y = ctx->gpubuf->add_vbo(kd->vertex2, kd->v2_count * sizeof(geomVertex2));
		kd->geom_offset.z = ctx->gpubuf->add_ibo(kd->geom_indices, kd->ig_count * sizeof(uint32_t));
		ctx->gpubuf->add_ssbo(kd->instance_data, kd->instance_count * sizeof(glm::mat4));
	}
	ctx->gpubuf->end(cmd);
	auto cmd0 = ovg_begin_frame(ctx, fbo, true);
	auto pass = ctx->pass;
	SDL_Rect cuClip = {};
	for (size_t d = 0; d < count; d++)
	{
		auto kd = data + d;
		for (size_t i = 0; i < kd->count; i++)
		{
			auto& it = kd->d[i];
			switch ((cmd_type_e)it.g.stype) {
			case cmd_type_e::DRAW_VG:
			{
				draw_vg(fbo, pass, &it.vg, &cuClip, kd->vg_offset);
			}
			break;
			case cmd_type_e::DRAW_GEOM:
			{
				draw_geom(fbo, pass, &it.g, kd->geom_offset);
			}
			break;
			}
		}
	}

	ovg_end_frame(ctx, fbo);
	SDL_SubmitGPUCommandBuffer(cmd);

	deferred_free_advance(ctx);
}


bool vg_sdl3_init(ovg_sdl3_ctx* g, int width, int height, bool is_vulkan) {
	SDL_Init(SDL_INIT_VIDEO);

	g->window = SDL_CreateWindow("SDL3 GPU Vector Graphics",
		width, height,
		SDL_WINDOW_RESIZABLE |
		SDL_WINDOW_HIGH_PIXEL_DENSITY);
	if (!g->window) return false;
	SDL_PropertiesID props = SDL_CreateProperties();
	if (is_vulkan)
	{
		SDL_GPUVulkanOptions vo = {};
		VkPhysicalDeviceScalarBlockLayoutFeatures scalarFeatures = {
			.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SCALAR_BLOCK_LAYOUT_FEATURES,
			.pNext = NULL,
			.scalarBlockLayout = VK_TRUE,
		};
		VkPhysicalDeviceVulkan12Features enabledFeatures12 = { .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES };

		enabledFeatures12.scalarBlockLayout = VK_TRUE;
		// 1b. Vulkan 1.1 复合特性（包含 shaderDrawParameters） 
		VkPhysicalDeviceVulkan11Features vk11Features = { .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES,	   .pNext = &enabledFeatures12, };
		// 其他 1.1 特性默认由驱动填充，我们只关心 shaderDrawParameters
		vk11Features.shaderDrawParameters = VK_TRUE;
		// 以下字段留 0，让 SDL/Vulkan 使用默认值
		vk11Features.storageBuffer16BitAccess = VK_FALSE;
		vk11Features.uniformAndStorageBuffer16BitAccess = VK_FALSE;
		vk11Features.storagePushConstant16 = VK_FALSE;
		vk11Features.storageInputOutput16 = VK_FALSE;
		vk11Features.multiview = VK_FALSE;
		vk11Features.multiviewGeometryShader = VK_FALSE;
		vk11Features.multiviewTessellationShader = VK_FALSE;
		vk11Features.variablePointersStorageBuffer = VK_FALSE;
		vk11Features.variablePointers = VK_FALSE;
		vk11Features.protectedMemory = VK_FALSE;
		vk11Features.samplerYcbcrConversion = VK_TRUE;
		const char* devext[] = { VK_KHR_MAINTENANCE_4_EXTENSION_NAME,
				VK_KHR_MAINTENANCE_5_EXTENSION_NAME,VK_EXT_SCALAR_BLOCK_LAYOUT_EXTENSION_NAME };
		const char* insext[] = { VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME };
		// 2. 填充 SDL_GPUVulkanOptions
		SDL_GPUVulkanOptions vkOpts = {
			.vulkan_api_version = VK_API_VERSION_1_2,  // 必须 >= 1.2 才能启用 scalarBlockLayout
			.feature_list = &vk11Features,             // pNext 链头
			.vulkan_10_physical_device_features = NULL, // 不需要额外 1.0 特性
			.device_extension_count = 3,
			.device_extension_names = devext,
			.instance_extension_count = 1,
			.instance_extension_names = insext,
		};
		SDL_SetPointerProperty(props, SDL_PROP_GPU_DEVICE_CREATE_VULKAN_OPTIONS_POINTER, &vkOpts);
		SDL_SetStringProperty(props, SDL_PROP_GPU_DEVICE_CREATE_NAME_STRING, "vulkan");
		SDL_SetBooleanProperty(props, SDL_PROP_GPU_DEVICE_CREATE_SHADERS_SPIRV_BOOLEAN, true);
	}
	bool debugmode = false;
#ifdef _DEBUG
	debugmode = true;
#endif // _DEBUG

	// 3. 通过属性创建 GPU 设备
	SDL_SetBooleanProperty(props, SDL_PROP_GPU_DEVICE_CREATE_DEBUGMODE_BOOLEAN, debugmode);

	SDL_GPUDevice* device = g->device = SDL_CreateGPUDeviceWithProperties(props);
	SDL_DestroyProperties(props);
	if (!g->device) {
		SDL_Log("GPU device create failed: %s", SDL_GetError());
		return false;
	}
	SDL_ClaimWindowForGPUDevice(g->device, g->window);

	return true;
}

sdl3gpu_texture* new_texture_def(ovg_ctx_t* ctx, int w, int h, vg_format_t format)
{
	SDL_GPUTextureFormat f = vg_to_sdl_format(format);
	auto p = new_texture(ctx->device, f, w, h, 0);
	return p;
}
