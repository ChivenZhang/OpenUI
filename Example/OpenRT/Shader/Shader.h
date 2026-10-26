#pragma once
#include "OpenRT.h"
#include "OpenGL/Screen.h"
#include "Vulkan/Screen.h"
#include "DirectX/Screen.h"
#include "Metal/Screen.h"
#include "WebGPU/Screen.h"
#include "OpenGL/Fill.h"
#include "Vulkan/Fill.h"
#include "DirectX/Fill.h"
#include "Metal/Fill.h"
#include "WebGPU/Fill.h"

namespace shader
{
struct stage_t
{
	const char* code = nullptr;
	uint32_t size = 0;
	const char* entry = "main";
};

template <typename T>
inline stage_t make_stage(T const* data, size_t bytes, const char* entry)
{
	return { reinterpret_cast<const char*>(data), static_cast<uint32_t>(bytes), entry };
}

inline stage_t screen_vs(rt_backend_t backend)
{
	switch (backend)
	{
	case RT_VULKAN: return make_stage(vulkan::screen::VS, vulkan::screen::VS_sizeInBytes, "main");
	case RT_DIRECTX: return make_stage(directx::screen::VS, directx::screen::VS_sizeInBytes, "main");
	case RT_METAL: return make_stage(metal::screen::VS, metal::screen::VS_sizeInBytes, "VS");
	case RT_WEBGPU: return make_stage(webgpu::screen::VS, webgpu::screen::VS_sizeInBytes, "VS");
	default: return make_stage(opengl::screen::VS, opengl::screen::VS_sizeInBytes, "main");
	}
}

inline stage_t screen_fs(rt_backend_t backend)
{
	switch (backend)
	{
	case RT_VULKAN: return make_stage(vulkan::screen::FS, vulkan::screen::FS_sizeInBytes, "main");
	case RT_DIRECTX: return make_stage(directx::screen::FS, directx::screen::FS_sizeInBytes, "main");
	case RT_METAL: return make_stage(metal::screen::FS, metal::screen::FS_sizeInBytes, "FS");
	case RT_WEBGPU: return make_stage(webgpu::screen::FS, webgpu::screen::FS_sizeInBytes, "FS");
	default: return make_stage(opengl::screen::FS, opengl::screen::FS_sizeInBytes, "main");
	}
}

inline stage_t fill_vs(rt_backend_t backend)
{
	switch (backend)
	{
	case RT_VULKAN: return make_stage(vulkan::fill::VS, vulkan::fill::VS_sizeInBytes, "main");
	case RT_DIRECTX: return make_stage(directx::fill::VS, directx::fill::VS_sizeInBytes, "main");
	case RT_METAL: return make_stage(metal::fill::VS, metal::fill::VS_sizeInBytes, "VS");
	case RT_WEBGPU: return make_stage(webgpu::fill::VS, webgpu::fill::VS_sizeInBytes, "VS");
	default: return make_stage(opengl::fill::VS, opengl::fill::VS_sizeInBytes, "main");
	}
}

inline stage_t fill_fs(rt_backend_t backend)
{
	switch (backend)
	{
	case RT_VULKAN: return make_stage(vulkan::fill::FS, vulkan::fill::FS_sizeInBytes, "main");
	case RT_DIRECTX: return make_stage(directx::fill::FS, directx::fill::FS_sizeInBytes, "main");
	case RT_METAL: return make_stage(metal::fill::FS, metal::fill::FS_sizeInBytes, "FS");
	case RT_WEBGPU: return make_stage(webgpu::fill::FS, webgpu::fill::FS_sizeInBytes, "FS");
	default: return make_stage(opengl::fill::FS, opengl::fill::FS_sizeInBytes, "main");
	}
}

inline bool screen_needs_sampler(rt_backend_t backend)
{
	return backend != RT_OPENGL;
}

inline uint32_t screen_sampler_binding(rt_backend_t backend)
{
	return backend == RT_WEBGPU ? 1u : 0u;
}

inline uint32_t fill_sampler_binding(rt_backend_t backend)
{
	return backend == RT_WEBGPU ? 3u : 2u;
}
}
