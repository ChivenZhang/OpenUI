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
#include "../SDL3InputEnum.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>
#include "SDLGPUPainter.h"
#include "SDLGPURender.h"

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

	auto device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, false, "vulkan");
	if (device == nullptr)
	{
		SDL_DestroyWindow(window);
		UI_ERROR("GPU could not be created! SDL_Error: %s", SDL_GetError());
		SDL_Quit();
		UI_FATAL("SDLGPU could not be initialized!");
	}

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
    canvas->setPainter(UINew<SDLGPUPainter>(w, h, canvas.get()));
    canvas->setRender(UINew<SDLGPURender>(w, h, canvas.get()));
    m_UICanvas = canvas;

	const std::string html_content = R"(
		<!DOCTYPE html>
		<html>
		<head>
			<title>Lexbor: development of web browser engine</title>
			<meta charset="utf-8">
			<meta name="description" content="">
			<meta name="keywords" content="">
			<link rel="apple-touch-icon" sizes="120x120" href="/img/apple-touch-icon.png">
			<link rel="icon" type="image/png" sizes="32x32" href="/img/favicon-32x32.png">
			<link rel="icon" type="image/png" sizes="16x16" href="/img/favicon-16x16.png">
			<link rel="manifest" href="/img/site.webmanifest">
			<link rel="mask-icon" href="/img/safari-pinned-tab.svg" color="#da532c">
			<meta name="msapplication-TileColor" content="#da532c">
			<meta name="theme-color" content="#ffffff">
			<link rel="stylesheet" href="/css/bootstrap-grid.min.css">
			<link rel="stylesheet" href="/css/style.css?time=1569523147.4017382">
			<script src="https://cdn.rawgit.com/google/code-prettify/master/loader/run_prettify.js"></script>
		</head>

		<body>
			<div class="page">
				<div class="head">
					<div class="head-content">

						<div class="head-left"><span>lexbor</span></div>

						<div class="head-right">
							<ul>
								<li><a href="/download/">Download</a></li>
								<li><a href="/docs/lexbor/">Documentation</a></li>
								<li><a href="/support/">Support</a></li>
								<li><a href="/license/">License</a></li>
								<li><a href="/roadmap/">Roadmap</a></li>
							</ul>
						</div>
					</div>
				</div>
				<div class="body">
					<div class="content ">
						<div class="markdown-body"><div class="body">
		    <div class="container">
		        <div class="row">
		            <div class="col-md">
		                <h3>What is lexbor?</h3>
		                <p>
		                    Lexbor is development of web browser engine available as a software library; it ships with a free license and has no extra dependencies.
		                </p>
		            </div>
		            <div class="col-md">
		                <h3>What’s the news?</h3>
		                <p>
		                    You can check out the latest version of the library and our roadmap for the future <a href="roadmap/">here</a>. All development occurs on <a href="https://github.com/lexbor/lexbor">GitHub</a>.
		                </p>
		            </div>
		        </div>
		    </div>
		    <div class="container">
		        <div class="row-md">
		            <div class="col-md">
		                <h2 style="text-align: center; max-width: 450px; margin: 60px auto 10px">Targets</h2>
		            </div>
		        </div>
		    </div>
		    <div class="container pr">
		        <div class="row">
		            <div class="col-md">
		                <img class="pr-icons" src="img/speed.png">
		                <div class="pr-icons">
		                    <h1 class="pr-title">FAST</h1>
		                    <p>
		                        For us, speed is an absolute must-have.
		                    <p>
		                        In our development process, we focus on fastest parsing techniques for HTML, CSS, and fonts, fastest data processing methods, and fastest ways to serve content to end users.
		                    <p>
		                        Whether you are building a backend that handles millions of HTML documents or a UI-heavy user app, your software’s response rate always matters to users and developers alike.
		                    </p>
		                </div>
		            </div>
		            <div class="col-md">
		                <img class="pr-icons" src="img/embedding.png">
		                <div class="pr-icons">
		                    <h1 class="pr-title">EMBEDDABLE</h1>
		                    <p>
		                        Lexbor’s code is optimized for ease of access in end-user applications and across programming languages.
		                    <p>
		                        You can effortlessly wrap all the capabilities of the library in, say, Python to power your end-user or backend applications in a new environment. In other words, lexbor offers a feature-rich core that developers can build upon as they see fit.
		                    </p>
		                </div>
		            </div>
		        </div>
		        <div class="row">
		            <div class="col-md">
		                <img class="pr-icons" src="img/cross-platform.png">
		                <div class="pr-icons">
		                    <h1 class="pr-title">CROSS-PLATFORM</h1>
		                    <p>
		                        Lexbor’s availability and support across various platforms and environments is a priority for us.
		                    <p>
		                        When you develop with lexbor, you have zero reasons to worry about your application’s portability. Moreover, the library can be easily ported to various devices such as Smart TVs or other IoT appliances.
		                    </p>
		                </div>
		            </div>
		            <div class="col-md">
		                <img class="pr-icons" src="img/simple.png">
		                <div class="pr-icons">
		                    <h1 class="pr-title">SIMPLE</h1>
		                    <p>
		                        Easy-to-use, simple, comprehensible API is key to fast and safe software development.
		                    <p>
		                        Lexbor’s API allows you to fine-tune a variety of HTML processing aspects, offering you the degree of flexibility you wish for in your application development.
		                    </p>
		                </div>
		            </div>
		        </div>
		    </div>
		    <div class="container">
		        <div class="row">
		            <div class="col-md">
		                <h3>Articles</h3>
		                <p>
		                    In this article, I will tell you how to create a superfast HTML parser supporting DOM. We will look at the HTML specification and its disadvantages in terms of performance and resource consumption during HTML parsing.
		                <p>
		                    I assume the reader has a basic knowledge of HTML: tags, nodes, elements, and namespace.
		                </p>
		                <div class="row">
		                    <div class="col-md"><a href="/articles/html/">Read more</a></div>
		                </div>
		            </div>
		        </div>
		    </div>
		</div></div>
					</div>
				</div>
			</div>

			<div class="footer">
				<div class="footer-content">
					<span class="copy">© 2018-2019 Alexander Borisov</span>
				</div>
			</div>
			<!-- Global site tag (gtag.js) - Google Analytics -->
			<script async src="https://www.googletagmanager.com/gtag/js?id=UA-130050727-1"></script>
			<script>
				window.dataLayer = window.dataLayer || [];
				function gtag(){dataLayer.push(arguments);}
				gtag('js', new Date());
				gtag('config', 'UA-130050727-1');
			</script>
		</body>
		</html>
    )";
	canvas->getBuilder()->buildWidget(html_content);

    SDL_ShowWindow(window);
    m_Window = window;
	m_Device = device;
}

SDLGPUDevice::~SDLGPUDevice()
{
    m_UICanvas = nullptr;
	SDL_ReleaseWindowFromGPUDevice(m_Device, m_Window);
	SDL_DestroyGPUDevice(m_Device);
    SDL_DestroyWindow(m_Window);
}

UICanvasRaw SDLGPUDevice::getCanvas() const
{
    return m_UICanvas.get();
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

	// Update layout and paint

	int width, height;
	SDL_GetWindowSize(window, &width, &height);
	UIRect client{0, 0, (float)width, (float)height};
	canvas->updateWidget(::clock() * 0.001f, client);

	// Output frame to screen

	// 获取命令缓冲区 (Command Buffer)
	SDL_GPUCommandBuffer *cmdBuf = SDL_AcquireGPUCommandBuffer(m_Device);
	if (cmdBuf == NULL)
	{
		UI_ERROR("获取 GPU Command Buffer 失败: %s", SDL_GetError());
		return false;
	}
	// 获取当前帧的交换链纹理
	SDL_GPUTexture* swapchainTexture = nullptr;
	uint32_t swapchainWidth = 0, swapchainHeight = 0;
	if (SDL_AcquireGPUSwapchainTexture(cmdBuf, window, &swapchainTexture, &swapchainWidth, &swapchainHeight) && swapchainTexture)
	{
		UIImage target
		{
			.Width = swapchainWidth,
			.Height = swapchainHeight,
			.Stride = swapchainWidth * 4,
			.Channel = 4,
			.Data = (uint64_t)swapchainTexture,
			.Type = UIImage::GPUByte,
		};
		canvas->setTarget(target);

		canvas->renderWidget(client);
	}
	// 提交命令缓冲区并呈现到屏幕
	SDL_SubmitGPUCommandBuffer(cmdBuf);
	return true;
}

SDL_Window* SDLGPUDevice::getWindow() const
{
    return m_Window;
}

#endif

