/*=================================================
* Copyright © 2020-2026 ChivenZhang.
* All Rights Reserved.
* =====================Note=========================
*
*
* ====================History=======================
* Created by chivenzhang@gmail.com.
*
* =================================================*/
#ifdef OPENUI_ENABLE_SDLGPU
#include "SDLGPUDevice.h"
#include "SDLGPUMerger.h"
#include "SDLGPUPainter.h"
#include "SDLGPURender.h"
#include "../SDL3InputEnum.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>
#include <screen.vert.h>
#include <screen.frag.h>

struct Vertex { float x, y, u, v; };

SDLGPUDevice::SDLGPUDevice()
{
    auto window = SDL_CreateWindow("https://github.com/ChivenZhang/OpenUI.git", 1000, 600, SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN);
    if (window == nullptr)
    {
        UI_ERROR("Window could not be created! SDL_Error: %s", SDL_GetError());
        SDL_Quit();
        UI_FATAL("Window could not be created! ");
    }
    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    auto scale = SDL_GetWindowDisplayScale(window);

	// Initialize SDLGPU Context

	auto device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, true, "vulkan");
	if (device == nullptr)
	{
		SDL_DestroyWindow(window);
		UI_ERROR("GPU could not be created! SDL_Error: %s", SDL_GetError());
		SDL_Quit();
		UI_FATAL("SDLGPU could not be initialized!");
	}
	m_Device = device;
    m_Window = window;

	UI_INFO("Use GPU backend: %s", SDL_GetGPUDeviceDriver(device));

	if (SDL_ClaimWindowForGPUDevice(device, window) == false)
	{
		SDL_DestroyWindow(window);
		UI_ERROR("SDL_ClaimWindowForGPUDevice failed! SDL_Error: %s", SDL_GetError());
		SDL_Quit();
		UI_FATAL("SDL_ClaimWindowForGPUDevice failed!");
	}

    // Initialize OpenUI context

    UIConfig config{.DisplayScale = scale};
    auto canvas = UINew<UICanvas>(this, config);
	auto render = UINew<SDLGPUMerger>(canvas.get(), w, h);
	auto painter = UINew<SDLGPUPainter>(canvas.get(), w, h);
    canvas->setRender(render);
    canvas->setPainter(painter);
    m_Canvas = canvas;

    SDL_ShowWindow(window);

	SDL_GPUShaderCreateInfo vs_info = {
		.code_size = sizeof(screen_vert),
		.code = (uint8_t*)screen_vert,
		.entrypoint = "VSMain",
		.format = SDL_GPU_SHADERFORMAT_SPIRV,
		.stage = SDL_GPU_SHADERSTAGE_VERTEX,
		.num_samplers = 0,
		.num_storage_textures = 0,
		.num_storage_buffers = 0,
		.num_uniform_buffers = 0,
	};
	SDL_GPUShader *vs_shader = SDL_CreateGPUShader(device, &vs_info);

	SDL_GPUShaderCreateInfo fs_info = {
		.code_size = sizeof(screen_frag),
		.code = (uint8_t*)screen_frag,
		.entrypoint = "FSMain",
		.format = SDL_GPU_SHADERFORMAT_SPIRV,
		.stage = SDL_GPU_SHADERSTAGE_FRAGMENT,
		.num_samplers = 1,
		.num_storage_textures = 0,
		.num_storage_buffers = 0,
		.num_uniform_buffers = 0,
	};
	SDL_GPUShader *fs_shader = SDL_CreateGPUShader(device, &fs_info);

	SDL_GPUVertexBufferDescription vb_desc = {
		.slot = 0,
		.pitch = sizeof(Vertex),
		.input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX,
		.instance_step_rate = 0,
	};

	SDL_GPUVertexAttribute attrs[2] = {
		{
			.location = 0,
			.buffer_slot = 0,
			.format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2,
			.offset = offsetof(Vertex, x),
		},
		{
			.location = 1,
			.buffer_slot = 0,
			.format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2,
			.offset = offsetof(Vertex, u),
		},
	};

	SDL_GPUVertexInputState vertex_input = {
		.vertex_buffer_descriptions = &vb_desc,
		.num_vertex_buffers = 1,
		.vertex_attributes = attrs,
		.num_vertex_attributes = 2,
	};

	SDL_GPUTextureFormat swap_fmt = SDL_GetGPUSwapchainTextureFormat(device, window);

	SDL_GPUColorTargetDescription color_desc = {
		.format = swap_fmt,
		.blend_state = {
			.src_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE,
			.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ZERO,
			.color_blend_op = SDL_GPU_BLENDOP_ADD,
			.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE,
			.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ZERO,
			.alpha_blend_op = SDL_GPU_BLENDOP_ADD,
			.enable_blend = false,
		},
	};

	SDL_GPUMultisampleState ms_state = {
		.sample_count = SDL_GPU_SAMPLECOUNT_1,
	};

	SDL_GPUGraphicsPipelineTargetInfo target_info = {
		.color_target_descriptions = &color_desc,
		.num_color_targets = 1,
	};

	SDL_GPUGraphicsPipelineCreateInfo pipe_info = {
		.vertex_shader = vs_shader,
		.fragment_shader = fs_shader,
		.vertex_input_state = vertex_input,
		.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
		.multisample_state = ms_state,
		.target_info = target_info,
	};
	m_Pipeline = SDL_CreateGPUGraphicsPipeline(device, &pipe_info);
	SDL_ReleaseGPUShader(device, vs_shader);
	SDL_ReleaseGPUShader(device, fs_shader);

	SDL_GPUSamplerCreateInfo sampler_info = {
		.min_filter = SDL_GPU_FILTER_LINEAR,
		.mag_filter = SDL_GPU_FILTER_LINEAR,
		.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_LINEAR,
		.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
		.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
		.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
	};
	m_Sampler = SDL_CreateGPUSampler(device, &sampler_info);

	const Vertex vertices[] = {
		{-1.0f, -1.0f, 0.0f, 0.0f},  // 左下
		{ 3.0f, -1.0f, 2.0f, 0.0f},  // 右下
		{-1.0f,  3.0f, 0.0f, 2.0f},  // 左上
	};

	SDL_GPUBufferCreateInfo buf_info = {
		.usage = SDL_GPU_BUFFERUSAGE_VERTEX,
		.size = sizeof(vertices),
	};
	m_Buffer = SDL_CreateGPUBuffer(device, &buf_info);

	// 上传顶点数据
	SDL_GPUCommandBuffer *upload_cmd = SDL_AcquireGPUCommandBuffer(device);
	SDL_GPUCopyPass *copy_pass = SDL_BeginGPUCopyPass(upload_cmd);
	SDL_GPUTransferBufferCreateInfo tbInfo = {
		.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
		.size = sizeof(vertices),
	};
	SDL_GPUTransferBuffer *staging = SDL_CreateGPUTransferBuffer( device, &tbInfo);
	memcpy(SDL_MapGPUTransferBuffer(device, staging, false), vertices, sizeof(vertices));
	SDL_UnmapGPUTransferBuffer(device, staging);

	SDL_GPUBufferRegion buf_region = {
		.buffer = m_Buffer,
		.offset = 0,
		.size = sizeof(vertices),
	};
	SDL_GPUTransferBufferLocation transfer_loc = {
		.transfer_buffer = staging,
		.offset = 0,
	};
	SDL_UploadToGPUBuffer(copy_pass, &transfer_loc, &buf_region, false);
	SDL_EndGPUCopyPass(copy_pass);
	SDL_SubmitGPUCommandBuffer(upload_cmd);
	SDL_ReleaseGPUTransferBuffer(device, staging);
}

SDLGPUDevice::~SDLGPUDevice()
{
    m_Canvas = nullptr;

	SDL_ReleaseGPUGraphicsPipeline(m_Device, m_Pipeline); m_Pipeline = nullptr;
	SDL_ReleaseGPUSampler(m_Device, m_Sampler); m_Sampler = nullptr;
	SDL_ReleaseGPUBuffer(m_Device, m_Buffer); m_Buffer = nullptr;

	SDL_ReleaseWindowFromGPUDevice(m_Device, m_Window);
	SDL_DestroyGPUDevice(m_Device); m_Device = nullptr;
    SDL_DestroyWindow(m_Window); m_Window = nullptr;
}

UICanvasRaw SDLGPUDevice::getCanvas() const
{
    return m_Canvas.get();
}

void SDLGPUDevice::setCursor(UIString type)
{
}

UIString SDLGPUDevice::getClipText() const
{
	return SDL_GetClipboardText();
}

void SDLGPUDevice::setClipText(UIString text)
{
	SDL_SetClipboardText(text.c_str());
}

void SDLGPUDevice::setKeyboard(bool value)
{
}

bool SDLGPUDevice::translateText(UIString text, UIString& result) const
{
	return false;
}

void SDLGPUDevice::logMessage(uint8_t type, UIString text) const
{
}

bool SDLGPUDevice::update()
{
	auto canvas = getCanvas();
	auto window = getWindow();
	auto device = getDevice();

	// Send events to OpenUI

	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_EVENT_QUIT:
			{
				return false;
			}
			break;
		case SDL_EVENT_KEY_DOWN:
			{
				UIKeyDownEvent event2(SDL3InputEnum::GetKeyboardEnum(event.key.key), SDL3InputEnum::GetModifierEnum(event.key.mod), event.key.scancode, event.key.key, event.key.mod, UIString(),
									event.key.repeat);
				canvas->sendEvent(nullptr, &event2);
			}
			break;
		case SDL_EVENT_KEY_UP:
			{
				UIKeyUpEvent event2(SDL3InputEnum::GetKeyboardEnum(event.key.key), SDL3InputEnum::GetModifierEnum(event.key.mod), event.key.scancode, event.key.key, event.key.mod, UIString(),
									event.key.repeat);
				canvas->sendEvent(nullptr, &event2);
			}
			break;
		case SDL_EVENT_TEXT_EDITING:
			{
				UITextInputEvent event2(SDL3InputEnum::GetKeyboardEnum(event.key.key), SDL3InputEnum::GetModifierEnum(event.key.mod), event.key.scancode, event.key.key, event.key.mod,
										event.edit.text, event.key.repeat, false, event.edit.start, event.edit.length);
				canvas->sendEvent(nullptr, &event2);
			}
			break;
		case SDL_EVENT_TEXT_INPUT:
			{
				UITextInputEvent event2(SDL3InputEnum::GetKeyboardEnum(event.key.key), SDL3InputEnum::GetModifierEnum(event.key.mod), event.key.scancode, event.key.key, event.key.mod,
										event.edit.text, event.key.repeat, true);
				canvas->sendEvent(nullptr, &event2);
			}
			break;
		case SDL_EVENT_MOUSE_MOTION:
			{
				int x, y;
				SDL_GetWindowPosition(window, &x, &y);
				UIMouseMoveEvent event2(event.motion.x, event.motion.y, x + event.motion.x, y + event.motion.y, SDL3InputEnum::GetMouseEnum(event.button.button),
										SDL3InputEnum::GetMouseEnum(event.button.button), SDL3InputEnum::GetModifierEnum(event.key.mod));
				canvas->sendEvent(nullptr, &event2);
			}
			break;
		case SDL_EVENT_MOUSE_BUTTON_DOWN:
			{
				int x, y;
				SDL_GetWindowPosition(window, &x, &y);
				if (event.button.clicks == 1)
				{
					UIMouseDownEvent event2(event.motion.x, event.motion.y, x + event.motion.x, y + event.motion.y, SDL3InputEnum::GetMouseEnum(event.button.button),
											SDL3InputEnum::GetMouseEnum(event.button.button), SDL3InputEnum::GetModifierEnum(event.key.mod), event.button.clicks);
					canvas->sendEvent(nullptr, &event2);
				}
				else
				{
					UIMouseDblClickEvent event2(event.motion.x, event.motion.y, x + event.motion.x, y + event.motion.y, SDL3InputEnum::GetMouseEnum(event.button.button),
												SDL3InputEnum::GetMouseEnum(event.button.button), SDL3InputEnum::GetModifierEnum(event.key.mod), event.button.clicks);
					canvas->sendEvent(nullptr, &event2);
				}
			}
			break;
		case SDL_EVENT_MOUSE_BUTTON_UP:
			{
				int x, y;
				SDL_GetWindowPosition(window, &x, &y);
				UIMouseUpEvent event2(event.motion.x, event.motion.y, x + event.motion.x, y + event.motion.y, SDL3InputEnum::GetMouseEnum(event.button.button),
									SDL3InputEnum::GetMouseEnum(event.button.button), SDL3InputEnum::GetModifierEnum(event.key.mod));
				canvas->sendEvent(nullptr, &event2);
			}
			break;
		case SDL_EVENT_MOUSE_WHEEL:
			{
				int x, y;
				SDL_GetWindowPosition(window, &x, &y);
				UIMouseWheelEvent event2(event.wheel.x, event.wheel.y, event.wheel.x, event.wheel.y, event.wheel.mouse_x, event.wheel.mouse_y, x + event.wheel.mouse_x, y + event.wheel.mouse_y,
										SDL3InputEnum::GetMouseEnum(event.button.button), SDL3InputEnum::GetMouseEnum(event.button.button), SDL3InputEnum::GetModifierEnum(event.key.mod));
				canvas->sendEvent(nullptr, &event2);
			}
			break;
		case SDL_EVENT_WINDOW_MOUSE_ENTER:
			{
				int x, y;
				SDL_GetWindowPosition(window, &x, &y);
				UIMouseEnterEvent event2(event.motion.x, event.motion.y, x + event.motion.x, y + event.motion.y, SDL3InputEnum::GetMouseEnum(event.button.button),
										SDL3InputEnum::GetMouseEnum(event.button.button), SDL3InputEnum::GetModifierEnum(event.key.mod));
				canvas->sendEvent(nullptr, &event2);
			}
			break;
		case SDL_EVENT_WINDOW_MOUSE_LEAVE:
			{
				int x, y;
				SDL_GetWindowPosition(window, &x, &y);
				UIMouseLeaveEvent event2(event.motion.x, event.motion.y, x + event.motion.x, y + event.motion.y, SDL3InputEnum::GetMouseEnum(event.button.button),
										SDL3InputEnum::GetMouseEnum(event.button.button), SDL3InputEnum::GetModifierEnum(event.key.mod));
				canvas->sendEvent(nullptr, &event2);
			}
			break;
		case SDL_EVENT_WINDOW_SHOWN:
			{
				UIShowEvent event2;
				canvas->sendEvent(nullptr, &event2);
			}
			break;
		case SDL_EVENT_WINDOW_HIDDEN:
			{
				UIHideEvent event2;
				canvas->sendEvent(nullptr, &event2);
			}
			break;
		case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
			{
				UICloseEvent event2;
				canvas->sendEvent(nullptr, &event2);
			}
			break;
		case SDL_EVENT_WINDOW_MOVED:
			{
				UIMoveEvent event2(event.window.data1, event.window.data2);
				canvas->sendEvent(nullptr, &event2);
			}
			break;
		case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
			{
				canvas->layoutWidget();
				// UICast<SDLGPUPainter>(canvas->getPainter())->resize(event.window.data1, event.window.data2);
				UIResizeEvent event2(event.window.data1, event.window.data2);
				canvas->sendEvent(nullptr, &event2);
			}
			break;
		case SDL_EVENT_WINDOW_MINIMIZED:
			break;
		case SDL_EVENT_WINDOW_MAXIMIZED:
			break;
		case SDL_EVENT_WINDOW_RESTORED:
			break;
		case SDL_EVENT_WINDOW_FOCUS_GAINED:
			{
				UIFocusEvent event2(true);
				canvas->sendEvent(nullptr, &event2);
			}
			break;
		case SDL_EVENT_WINDOW_FOCUS_LOST:
			{
				UIFocusEvent event2(false);
				canvas->sendEvent(nullptr, &event2);
			}
			break;
		default:
			break;
		}
	}

	// Output frame to screen

	int32_t width = 0, height = 0;
	SDL_GetWindowSize(window, &width, &height);

	auto render = canvas->getRender({});
	auto source = render->newImage(width, height);
	canvas->setTarget(source);
	canvas->updateWidget(::clock() * 0.001f, UIRect{0, 0, (float)width, (float)height});

	// Copy frame to screen

	auto cmd = SDL_AcquireGPUCommandBuffer(m_Device);
	if (cmd == nullptr) UI_ERROR("获取 GPU Command Buffer 失败: %s", SDL_GetError());

	SDL_GPUTexture* screenRT = nullptr;
	if (cmd && SDL_AcquireGPUSwapchainTexture(cmd, window, &screenRT, nullptr, nullptr) && screenRT)
	{
		SDL_GPUColorTargetInfo color_attachment = {
			.texture = screenRT,
			.clear_color = {0, 0, 0, 1},
			.load_op = SDL_GPU_LOADOP_CLEAR,
			.store_op = SDL_GPU_STOREOP_STORE,
		};
		auto pass = SDL_BeginGPURenderPass(cmd, &color_attachment, 1, nullptr);
		SDL_GPUBufferBinding vb_binding = {
			.buffer = m_Buffer,
			.offset = 0,
		};
		SDL_BindGPUVertexBuffers(pass, 0, &vb_binding, 1);
		SDL_GPUTextureSamplerBinding sampler_binding = {
			.texture = (SDL_GPUTexture*)source.Handle,
			.sampler = m_Sampler,
		};
		SDL_BindGPUFragmentSamplers(pass, 0, &sampler_binding, 1);
		SDL_BindGPUGraphicsPipeline(pass, m_Pipeline);
		SDL_DrawGPUPrimitives(pass, 3, 1, 0, 0);
		SDL_EndGPURenderPass(pass);
		SDL_SubmitGPUCommandBuffer(cmd);

		render->delImage(source);
	}
	return true;
}

SDL_Window* SDLGPUDevice::getWindow() const
{
    return m_Window;
}

SDL_GPUDevice* SDLGPUDevice::getDevice() const
{
	return m_Device;
}

#endif

