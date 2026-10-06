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
#define OPENRTX_IMPLEMENTATION
#include <OpenRTX.h>
#include <OpenGL.h>
#include <Example/Slang/OpenGL-Default.h>
#include <SDL3/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "OpenRTDevice.h"
#include "OpenRTPainter.h"
#include "OpenRTRender.h"

void frame(int width, int height)
{
    static auto module = rt_create_module_render({
        .vshader = {VS}, .fshader = {FS},
        .colors = {{.format = RT_TEXTURE_RGBA8UNORM,}},
        .vertex = {rt_vertex_vertex, rt_vertex_normal, rt_vertex_uv,},
    });
    static auto pass_color = rt_create_texture_color(width, height);
    {
        rt_pass_render_t pass = {.colors = {{.texture_view = pass_color.default_view, .clear = true,}},};
        rt_begin_render(pass);

        rt_bind_module_render(module);

        static auto mesh = rt_create_mesh_triangle(1);
        rt_draw_mesh(mesh);

        rt_end_render(pass);
    }

    gl_draw_screen(width, height, {}, pass_color);

    rt_submit();
}

int main()
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    auto window = SDL_CreateWindow("OpenGL Context", 1, 1, SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);
    auto context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, context);
    SDL_GL_SetSwapInterval(0);
    rt_load_library();

    auto device = UINew<OpenRTDevice>();
    while (device->update()) continue;
    device = nullptr;

    SDL_GL_MakeCurrent(window, context);
    rt_unload_library();

    SDL_Quit();
    return 0;
}