#include "nanovg_rt.h"

#include <map>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <vector>

#include "nanovg_gl.h"

enum RTVGuniformLoc
{
    RTVG_LOC_VIEWSIZE,
    RTVG_LOC_TEX,
    RTVG_LOC_FRAG,
    RTVG_MAX_LOCS
};

enum RTVGshaderType
{
    NSVG_SHADER_FILLGRAD,
    NSVG_SHADER_FILLIMG,
    NSVG_SHADER_SIMPLE,
    NSVG_SHADER_IMG
};

enum RTVGuniformBindings
{
    RTVG_FRAG_BINDING = 0,
};

struct RTVGtexture
{
    rt_texture_t texture;

    // ==================

    int id;
    int width, height;
    int type;
    int flags;
};

struct RTVGblend
{
    GLenum srcRGB;
    GLenum dstRGB;
    GLenum srcAlpha;
    GLenum dstAlpha;
};

enum RTVGcallType
{
    RTVG_NONE = 0,
    RTVG_FILL,
    RTVG_CONVEXFILL,
    RTVG_STROKE,
    RTVG_TRIANGLES,
};

struct RTVGcall
{
    int type;
    int image;
    int pathOffset;
    int pathCount;
    int triangleOffset;
    int triangleCount;
    int uniformOffset;
    RTVGblend blendFunc;
};

struct RTVGpath
{
    int fillOffset;
    int fillCount;
    int strokeOffset;
    int strokeCount;
};

struct RTVGfragUniforms
{
    float scissorMat[12]; // matrices are actually 3 vec4s
    float paintMat[12];
    struct NVGcolor innerCol;
    struct NVGcolor outerCol;
    float scissorExt[2];
    float scissorScale[2];
    float extent[2];
    float radius;
    float feather;
    float strokeMult;
    float strokeThr;
    int texType;
    int type;
};

struct RTVGcontext
{
    rt_buffer_t vertBuf;
    rt_buffer_t fragBuf;
    rt_texture_t target;

    // ===================
    std::map<uint32_t, RTVGtexture> textures;
    float view[2];
    int flags;
    int dummyTex;

    // Per frame buffers
    int ncalls;
    int npaths;
    int nverts;
    int nuniforms;
    std::vector<RTVGcall> calls;
    std::vector<RTVGpath> paths;
    std::vector<NVGvertex> verts;
    std::vector<RTVGfragUniforms> uniforms;
};

static RTVGtexture* nvg_findTexture(RTVGcontext* gl, int id)
{
    auto result = gl->textures.find(id);
    if (result != gl->textures.end()) return &result->second;
    return nullptr;
}

static int rt_renderCreateTexture(void* uptr, int type, int w, int h, int imageFlags, const unsigned char* data)
{
    auto gl = (RTVGcontext*)uptr;

    rt_texture_info_t info = {.width = (uint32_t)w, .height = (uint32_t)h, .data = data,};
    info.format = (type == NVG_TEXTURE_RGBA) ? RT_TEXTURE_RGBA8UNORM : RT_TEXTURE_R8UNORM;
    info.address_u = (imageFlags & NVG_IMAGE_REPEATX) ? RT_REPEAT : RT_CLAMP_TO_EDGE;
    info.address_v = (imageFlags & NVG_IMAGE_REPEATY) ? RT_REPEAT : RT_CLAMP_TO_EDGE;
    info.min_filter = (imageFlags & NVG_IMAGE_GENERATE_MIPMAPS)
                          ? ((imageFlags & NVG_IMAGE_NEAREST) ? RT_NEAREST_MIPMAP_NEAREST : RT_LINEAR_MIPMAP_LINEAR)
                          : (((imageFlags & NVG_IMAGE_NEAREST)) ? RT_NEAREST : RT_LINEAR);
    info.mag_filter = (imageFlags & NVG_IMAGE_NEAREST) ? RT_NEAREST : RT_LINEAR;
    info.mipmaps = (imageFlags & NVG_IMAGE_GENERATE_MIPMAPS) ? 0 : 1;
    auto texture = rt_create_texture(info);

    auto& tex = gl->textures[texture.handle];
    tex.texture = rt_create_texture(info);
    tex.id = (int32_t)texture.handle;
    tex.width = w;
    tex.height = h;
    tex.type = type;
    tex.flags = imageFlags;
    return tex.id;
}

static int rt_renderDeleteTexture(void* uptr, int image)
{
    auto gl = (RTVGcontext*)uptr;

    if (auto result = nvg_findTexture(gl, image))
    {
        rt_destroy_texture(result->texture);
        gl->textures.erase(image);
        return 1;
    }
    return 0;
}

static int rt_renderUpdateTexture(void* uptr, int image, int x, int y, int w, int h, const unsigned char* data)
{
    auto gl = (RTVGcontext*)uptr;
    RTVGtexture* tex = nvg_findTexture(gl, image);
    if (tex == nullptr) return 0;

    rt_pass_transfer_t pass;
    rt_begin_transfer(pass);
    if (tex->type == NVG_TEXTURE_RGBA)
    {
        rt_copy_texture_data(
            {
                .data = data,
                .size = (size_t)w * h * 4,
                .bytesPerRow = (uint32_t)w * 4,
                .rowsPerImage = (uint32_t)h,
            },
            {
                .texture = tex->texture,
                .mipLevel = 1,
                .origin = {(uint32_t)x, (uint32_t)y, 0},
            },
            {(uint32_t)w, (uint32_t)h, 1U});
    }
    else
    {
        rt_copy_texture_data(
            {
                .data = data,
                .size = (size_t)w * h * 1,
                .bytesPerRow = (uint32_t)w * 1,
                .rowsPerImage = (uint32_t)h,
            },
            {
                .texture = tex->texture,
                .mipLevel = 1,
                .origin = {(uint32_t)x, (uint32_t)y, 0},
            },
            {(uint32_t)w, (uint32_t)h, 1U});
    }
    rt_end_transfer(pass);
    return 1;
}

static int rt_renderGetTextureSize(void* uptr, int image, int* w, int* h)
{
    auto gl = (RTVGcontext*)uptr;
    RTVGtexture* tex = nvg_findTexture(gl, image);
    if (tex == nullptr) return 0;
    *w = tex->width;
    *h = tex->height;
    return 1;
}

[[deprecated]]
static void nvg_xformToMat3x4(float* m3, float* t)
{
    m3[0] = t[0];
    m3[1] = t[1];
    m3[2] = 0.0f;
    m3[3] = 0.0f;
    m3[4] = t[2];
    m3[5] = t[3];
    m3[6] = 0.0f;
    m3[7] = 0.0f;
    m3[8] = t[4];
    m3[9] = t[5];
    m3[10] = 1.0f;
    m3[11] = 0.0f;
}

[[deprecated]]
static NVGcolor nvg_premulColor(NVGcolor c)
{
    c.r *= c.a;
    c.g *= c.a;
    c.b *= c.a;
    return c;
}

static int nvg_convertPaint(RTVGcontext* gl, RTVGfragUniforms* frag, NVGpaint* paint, NVGscissor* scissor, float width,
                            float fringe, float strokeThr)
{
    RTVGtexture* tex = nullptr;
    float invxform[6];

    memset(frag, 0, sizeof(*frag));

    frag->innerCol = nvg_premulColor(paint->innerColor);
    frag->outerCol = nvg_premulColor(paint->outerColor);

    if (scissor->extent[0] < -0.5f || scissor->extent[1] < -0.5f)
    {
        memset(frag->scissorMat, 0, sizeof(frag->scissorMat));
        frag->scissorExt[0] = 1.0f;
        frag->scissorExt[1] = 1.0f;
        frag->scissorScale[0] = 1.0f;
        frag->scissorScale[1] = 1.0f;
    }
    else
    {
        nvgTransformInverse(invxform, scissor->xform);
        nvg_xformToMat3x4(frag->scissorMat, invxform);
        frag->scissorExt[0] = scissor->extent[0];
        frag->scissorExt[1] = scissor->extent[1];
        frag->scissorScale[0] = sqrtf(scissor->xform[0] * scissor->xform[0] + scissor->xform[2] * scissor->xform[2]) /
            fringe;
        frag->scissorScale[1] = sqrtf(scissor->xform[1] * scissor->xform[1] + scissor->xform[3] * scissor->xform[3]) /
            fringe;
    }

    memcpy(frag->extent, paint->extent, sizeof(frag->extent));
    frag->strokeMult = (width * 0.5f + fringe * 0.5f) / fringe;
    frag->strokeThr = strokeThr;

    if (paint->image != 0)
    {
        tex = nvg_findTexture(gl, paint->image);
        if (tex == nullptr) return 0;
        if ((tex->flags & NVG_IMAGE_FLIPY) != 0)
        {
            float m1[6], m2[6];
            nvgTransformTranslate(m1, 0.0f, frag->extent[1] * 0.5f);
            nvgTransformMultiply(m1, paint->xform);
            nvgTransformScale(m2, 1.0f, -1.0f);
            nvgTransformMultiply(m2, m1);
            nvgTransformTranslate(m1, 0.0f, -frag->extent[1] * 0.5f);
            nvgTransformMultiply(m1, m2);
            nvgTransformInverse(invxform, m1);
        }
        else
        {
            nvgTransformInverse(invxform, paint->xform);
        }
        frag->type = NSVG_SHADER_FILLIMG;

        if (tex->type == NVG_TEXTURE_RGBA)
            frag->texType = (tex->flags & NVG_IMAGE_PREMULTIPLIED) ? 0 : 1;
        else
            frag->texType = 2;

        //		printf("frag->texType = %d\n", frag->texType);
    }
    else
    {
        frag->type = NSVG_SHADER_FILLGRAD;
        frag->radius = paint->radius;
        frag->feather = paint->feather;
        nvgTransformInverse(invxform, paint->xform);
    }

    nvg_xformToMat3x4(frag->paintMat, invxform);

    return 1;
}

[[deprecated]]
static RTVGfragUniforms* nvg_fragUniformPtr(RTVGcontext* gl, int i);

// [[deprecated]]
// static void nvg_setUniforms(RTVGcontext* gl, int uniformOffset, int image)
// {
//     RTVGtexture* tex = nullptr;
//     glBindBufferRange(GL_UNIFORM_BUFFER, RTVG_FRAG_BINDING, gl->fragBuf, uniformOffset, sizeof(RTVGfragUniforms));
//
//     if (image != 0)
//     {
//         tex = nvg_findTexture(gl, image);
//     }
//     // If no image is set, use empty texture
//     if (tex == nullptr)
//     {
//         tex = nvg_findTexture(gl, gl->dummyTex);
//     }
//     glBindTexture(GL_TEXTURE_2D, tex != nullptr ? tex->tex : 0);
// }

static void rt_renderViewport(void* uptr, float width, float height, float devicePixelRatio)
{
    NVG_NOTUSED(devicePixelRatio);
    auto gl = (RTVGcontext*)uptr;
    gl->view[0] = width;
    gl->view[1] = height;

    if ((uint32_t)width != gl->target.width || (uint32_t)height != gl->target.height)
    {
        rt_destroy_texture(gl->target);
        gl->target = rt_create_texture({.width = (uint32_t)width, .height = (uint32_t)height, .format = RT_TEXTURE_RGBA8UNORM, .mipmaps = 1,});
    }
}

static void nvg_fill(RTVGcontext* gl, RTVGcall* call)
{
    auto paths = &gl->paths[call->pathOffset];
    auto image = nvg_findTexture(gl, call->image);

    // glEnable(GL_STENCIL_TEST);
    // glnvg__stencilMask(gl, 0xff);
    // glnvg__stencilFunc(gl, GL_ALWAYS, 0, 0xff);
    // glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    //
    // // set bindpoint for solid loc
    // glnvg__setUniforms(gl, call->uniformOffset, 0);
    // glnvg__checkError(gl, "fill simple");
    //
    // glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_KEEP, GL_INCR_WRAP);
    // glStencilOpSeparate(GL_BACK, GL_KEEP, GL_KEEP, GL_DECR_WRAP);
    // glDisable(GL_CULL_FACE);
    // for (i = 0; i < npaths; i++)
    //     glDrawArrays(GL_TRIANGLES, paths[i].fillOffset, paths[i].fillCount);

    // Draw shapes
    {
        static auto module = rt_create_module_render({
            .vshader = {fillVertShader}, .fshader = {fillFragShader},
            .colors = {
                {
                    .write = {false, false, false, false},
                    .color = {.func = RT_FUNC_ADD, .src = RT_BLEND_ONE, .dst = RT_BLEND_ONE_MINUS_SRC_ALPHA,},
                    .alpha = {.func = RT_FUNC_ADD, .src = RT_BLEND_SRC_ALPHA, .dst = RT_BLEND_ONE_MINUS_SRC_ALPHA,},
                }},
            .stencil = {
                .front = { .func = RT_ALWAYS, .sfail = RT_STENCIL_KEEP, .zfail = RT_STENCIL_KEEP, .zpass = RT_STENCIL_DECR_WRAP },
                .back = { .func = RT_ALWAYS, .sfail = RT_STENCIL_KEEP, .zfail = RT_STENCIL_KEEP, .zpass = RT_STENCIL_INCR_WRAP },
            },
            .vertex = {{.attrib = {{.location = 0, .offset = 0, .format = RT_VERTEX_FLOAT32X2,},{.location = 1, .offset = sizeof(float) * 2, .format = RT_VERTEX_FLOAT32X2,},},}},
            .cull_mode = RT_CULL_NONE,
            .primitive = RT_TRIANGLES,
        });
        rt_pass_render_t pass{
            .colors = {
                {
                    .texture_view = gl->target.default_view,
                    .clear = false,
                }
            },
            .stencil = {.refer = 0},
        };
        rt_begin_render(pass);
        rt_bind_module_render(module);
        rt_bind_texture(image->texture, {.binding = 0,});
        rt_bind_buffer(gl->fragBuf, {.binding = 0, .offset = (size_t)call->uniformOffset,});
        for (auto i = 0; i < call->pathCount; ++i)
        {
            rt_draw_array(nullptr, 0, paths[i].fillCount, 1, paths[i].fillOffset, 0);
        }
        rt_end_render(pass);
    }

    // glEnable(GL_CULL_FACE);
    // // Draw anti-aliased pixels
    // glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    // glnvg__setUniforms(gl, call->uniformOffset + gl->fragSize, call->image);
    // glnvg__checkError(gl, "fill fill");
    // if (gl->flags & NVG_ANTIALIAS) {
    //     glnvg__stencilFunc(gl, GL_EQUAL, 0x00, 0xff);
    //     glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    //     // Draw fringes
    //     for (i = 0; i < npaths; i++)
    //         glDrawArrays(GL_TRIANGLES, paths[i].strokeOffset, paths[i].strokeCount);
    // }

    // Draw anti-aliased pixels
    {
        static auto module = rt_create_module_render({
            .vshader = {fillVertShader}, .fshader = {fillFragShader},
            .colors = {
                {
                    .color = {.func = RT_FUNC_ADD, .src = RT_BLEND_ONE, .dst = RT_BLEND_ONE_MINUS_SRC_ALPHA,},
                    .alpha = {.func = RT_FUNC_ADD, .src = RT_BLEND_SRC_ALPHA, .dst = RT_BLEND_ONE_MINUS_SRC_ALPHA,},
                }
            },
            .stencil = {
                .front = { .func = RT_EQUAL, .sfail = RT_STENCIL_KEEP, .zfail = RT_STENCIL_KEEP, .zpass = RT_STENCIL_KEEP },
                .back = { .func = RT_EQUAL, .sfail = RT_STENCIL_KEEP, .zfail = RT_STENCIL_KEEP, .zpass = RT_STENCIL_KEEP },
            },
            .vertex = {{.attrib = {{.location = 0, .offset = 0, .format = RT_VERTEX_FLOAT32X2,},{.location = 1, .offset = sizeof(float) * 2, .format = RT_VERTEX_FLOAT32X2,},},}},
            .cull_mode = RT_CULL_BACK,
            .primitive = RT_TRIANGLES,
        });
        rt_pass_render_t pass{
            .colors = {
                    {
                        .texture_view = gl->target.default_view,
                        .clear = false,
                    }
            },
            .stencil = {.refer = 0},
        };
        rt_begin_render(pass);
        rt_bind_module_render(module);
        rt_bind_texture(image->texture, {.binding = 0,});
        rt_bind_buffer(gl->fragBuf, {.binding = 0, .offset = (size_t)call->uniformOffset + sizeof(RTVGfragUniforms),});
        for (auto i = 0; i < call->pathCount; ++i)
        {
            rt_draw_array(nullptr, 0, paths[i].strokeCount, 1, paths[i].strokeOffset, 0);
        }
        rt_end_render(pass);
    }

    // glnvg__stencilFunc(gl, GL_NOTEQUAL, 0x0, 0xff);
    // glStencilOp(GL_ZERO, GL_ZERO, GL_ZERO);
    // glDrawArrays(GL_TRIANGLES, call->triangleOffset, call->triangleCount);

    // Draw fill
    {
        static auto module = rt_create_module_render({
            .vshader = {fillVertShader}, .fshader = {fillFragShader},
            .colors = {
                {
                    .color = {.func = RT_FUNC_ADD, .src = RT_BLEND_ONE, .dst = RT_BLEND_ONE_MINUS_SRC_ALPHA,},
                    .alpha = {.func = RT_FUNC_ADD, .src = RT_BLEND_SRC_ALPHA, .dst = RT_BLEND_ONE_MINUS_SRC_ALPHA,},
                }
            },
            .stencil = {
                .front = { .func = RT_NOTEQUAL, .sfail = RT_STENCIL_ZERO, .zfail = RT_STENCIL_ZERO, .zpass = RT_STENCIL_ZERO },
                .back = { .func = RT_NOTEQUAL, .sfail = RT_STENCIL_ZERO, .zfail = RT_STENCIL_ZERO, .zpass = RT_STENCIL_ZERO },
            },
            .vertex = {{.attrib = {{.location = 0, .offset = 0, .format = RT_VERTEX_FLOAT32X2,},{.location = 1, .offset = sizeof(float) * 2, .format = RT_VERTEX_FLOAT32X2,},},}},
            .cull_mode = RT_CULL_BACK,
            .primitive = RT_TRIANGLES,
        });
        rt_pass_render_t pass{
            .colors = {
                    {
                        .texture_view = gl->target.default_view,
                        .clear = false,
                    }
            },
            .stencil = {.refer = 0},
        };
        rt_begin_render(pass);
        rt_bind_module_render(module);
        rt_bind_texture(image->texture, {.binding = 0,});
        rt_bind_buffer(gl->fragBuf, {.binding = 0, .offset = (size_t)call->uniformOffset + sizeof(RTVGfragUniforms),});
        rt_draw_array(nullptr, 0, call->triangleCount, 1, call->triangleOffset, 0);
        rt_end_render(pass);
    }
}

static void nvg_convexFill(RTVGcontext* gl, RTVGcall* call)
{
    auto paths = &gl->paths[call->pathOffset];
    auto image = nvg_findTexture(gl, call->image);

    // nvg_setUniforms(gl, call->uniformOffset, call->image);
    // for (i = 0; i < npaths; i++)
    // {
    //     glDrawArrays(GL_TRIANGLES, paths[i].fillOffset, paths[i].fillCount);
    //     // Draw fringes
    //     if (paths[i].strokeCount > 0)
    //     {
    //         glDrawArrays(GL_TRIANGLES, paths[i].strokeOffset, paths[i].strokeCount);
    //     }
    // }

    {
        static auto module = rt_create_module_render({
            .vshader = {fillVertShader}, .fshader = {fillFragShader},
            .colors = {
                {
                    .color = {.func = RT_FUNC_ADD, .src = RT_BLEND_ONE, .dst = RT_BLEND_ONE_MINUS_SRC_ALPHA,},
                    .alpha = {.func = RT_FUNC_ADD, .src = RT_BLEND_SRC_ALPHA, .dst = RT_BLEND_ONE_MINUS_SRC_ALPHA,},
                }
            },
            .stencil = {
                .front = { .func = RT_ALWAYS, .sfail = RT_STENCIL_KEEP, .zfail = RT_STENCIL_KEEP, .zpass = RT_STENCIL_INCR },
                .back = { .func = RT_ALWAYS, .sfail = RT_STENCIL_KEEP, .zfail = RT_STENCIL_KEEP, .zpass = RT_STENCIL_INCR },
            },
            .vertex = {{.attrib = {{.location = 0, .offset = 0, .format = RT_VERTEX_FLOAT32X2,},{.location = 1, .offset = sizeof(float) * 2, .format = RT_VERTEX_FLOAT32X2,},},}},
            .primitive = RT_TRIANGLES,
        });
        rt_pass_render_t pass{
            .colors = {
                    {
                        .texture_view = gl->target.default_view,
                        .clear = false,
                    }
            },
            .stencil = {.refer = 0},
        };
        rt_begin_render(pass);
        rt_bind_module_render(module);
        rt_bind_texture(image->texture, {.binding = 0,});
        rt_bind_buffer(gl->fragBuf, {.binding = 0, .offset = (size_t)call->uniformOffset,});

        for (auto i = 0; i < call->pathCount; i++)
        {
            rt_draw_array(nullptr, 0, paths[i].fillCount, 1, paths[i].fillOffset, 0);

            if (paths[i].strokeCount > 0)
            {
                rt_draw_array(nullptr, 0, paths[i].strokeCount, 1, paths[i].strokeOffset, 0);
            }
        }
        rt_end_render(pass);
    }
}

static void nvg_stroke(RTVGcontext* gl, RTVGcall* call)
{
    auto paths = &gl->paths[call->pathOffset];
    auto image = nvg_findTexture(gl, call->image);

    // Fill the stroke base without overlap
    {
        static auto module = rt_create_module_render({
            .vshader = {fillVertShader}, .fshader = {fillFragShader},
            .colors = {
                {
                    .color = {.func = RT_FUNC_ADD, .src = RT_BLEND_ONE, .dst = RT_BLEND_ONE_MINUS_SRC_ALPHA,},
                    .alpha = {.func = RT_FUNC_ADD, .src = RT_BLEND_SRC_ALPHA, .dst = RT_BLEND_ONE_MINUS_SRC_ALPHA,},
                }
            },
            .stencil = {
                .front = { .func = RT_EQUAL, .sfail = RT_STENCIL_KEEP, .zfail = RT_STENCIL_KEEP, .zpass = RT_STENCIL_INCR },
                .back = { .func = RT_EQUAL, .sfail = RT_STENCIL_KEEP, .zfail = RT_STENCIL_KEEP, .zpass = RT_STENCIL_INCR },
            },
            .vertex = {{.attrib = {{.location = 0, .offset = 0, .format = RT_VERTEX_FLOAT32X2,},{.location = 1, .offset = sizeof(float) * 2, .format = RT_VERTEX_FLOAT32X2,},},}},
            .primitive = RT_TRIANGLE_STRIP,
        });
        rt_pass_render_t pass{
            .colors = {
                    {
                        .texture_view = gl->target.default_view,
                        .clear = false,
                    }
            },
            .stencil = {.refer = 0},
        };
        rt_begin_render(pass);
        rt_bind_module_render(module);
        rt_bind_texture(image->texture, {.binding = 0,});
        rt_bind_buffer(gl->fragBuf, {.binding = 0, .offset = (size_t)call->uniformOffset + sizeof(RTVGfragUniforms),});
        for (auto i = 0; i < call->pathCount; ++i)
        {
            rt_draw_array(nullptr, 0, paths[i].strokeCount, 1, paths[i].strokeOffset, 0);
        }
        rt_end_render(pass);
    }

    // glnvg__setUniforms(gl, call->uniformOffset, call->image);
    // glnvg__stencilFunc(gl, GL_EQUAL, 0x00, 0xff);
    // glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    // for (i = 0; i < npaths; i++)
    //     glDrawArrays(GL_TRIANGLES, paths[i].strokeOffset, paths[i].strokeCount);

    // Draw anti-aliased pixels.
    {
        static auto module = rt_create_module_render({
            .vshader = {fillVertShader}, .fshader = {fillFragShader},
            .colors = {
                {
                    .color = {.func = RT_FUNC_ADD, .src = RT_BLEND_ONE, .dst = RT_BLEND_ONE_MINUS_SRC_ALPHA,},
                    .alpha = {.func = RT_FUNC_ADD, .src = RT_BLEND_SRC_ALPHA, .dst = RT_BLEND_ONE_MINUS_SRC_ALPHA,},
                }
            },
            .stencil = {
                .front = { .func = RT_ALWAYS, .sfail = RT_STENCIL_KEEP, .zfail = RT_STENCIL_KEEP, .zpass = RT_STENCIL_KEEP },
                .back = { .func = RT_ALWAYS, .sfail = RT_STENCIL_KEEP, .zfail = RT_STENCIL_KEEP, .zpass = RT_STENCIL_KEEP },
            },
            .vertex = {{.attrib = {{.location = 0, .offset = 0, .format = RT_VERTEX_FLOAT32X2,},{.location = 1, .offset = sizeof(float) * 2, .format = RT_VERTEX_FLOAT32X2,},},}},
            .primitive = RT_TRIANGLES,
        });
        rt_pass_render_t pass{
            .colors = {
                    {
                        .texture_view = gl->target.default_view,
                        .clear = false,
                    }
            },
            .stencil = {.refer = 0},
        };
        rt_begin_render(pass);
        rt_bind_module_render(module);
        rt_bind_texture(image->texture, {.binding = 0,});
        rt_bind_buffer(gl->fragBuf, {.binding = 0, .offset = (size_t)call->uniformOffset,});
        for (auto i = 0; i < call->pathCount; ++i)
        {
            rt_draw_array(nullptr, 0, paths[i].strokeCount, 1, paths[i].strokeOffset, 0);
        }
        rt_end_render(pass);
    }

    // glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    // glnvg__stencilFunc(gl, GL_ALWAYS, 0x0, 0xff);
    // glStencilOp(GL_ZERO, GL_ZERO, GL_ZERO);
    // glnvg__checkError(gl, "stroke fill 1");
    // for (i = 0; i < npaths; i++)
    //     glDrawArrays(GL_TRIANGLES, paths[i].strokeOffset, paths[i].strokeCount);
    // glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

    // Clear stencil buffer.
    {
        static auto module = rt_create_module_render({
            .vshader = {fillVertShader}, .fshader = {fillFragShader},
            .colors = {
                {
                    .write = {false, false, false, false},
                    .color = {.func = RT_FUNC_ADD, .src = RT_BLEND_ONE, .dst = RT_BLEND_ONE_MINUS_SRC_ALPHA,},
                    .alpha = {.func = RT_FUNC_ADD, .src = RT_BLEND_SRC_ALPHA, .dst = RT_BLEND_ONE_MINUS_SRC_ALPHA,},
                }
            },
            .stencil = {
                .front = { .func = RT_ALWAYS, .sfail = RT_STENCIL_ZERO, .zfail = RT_STENCIL_ZERO, .zpass = RT_STENCIL_ZERO },
                .back = { .func = RT_ALWAYS, .sfail = RT_STENCIL_ZERO, .zfail = RT_STENCIL_ZERO, .zpass = RT_STENCIL_ZERO },
            },
            .vertex = {{.attrib = {{.location = 0, .offset = 0, .format = RT_VERTEX_FLOAT32X2,},{.location = 1, .offset = sizeof(float) * 2, .format = RT_VERTEX_FLOAT32X2,},},}},
            .primitive = RT_TRIANGLES,
        });
        rt_pass_render_t pass{
            .colors = {
                    {
                        .texture_view = gl->target.default_view,
                        .clear = false,
                    }
            },
            .stencil = {.refer = 0},
        };
        rt_begin_render(pass);
        rt_bind_module_render(module);
        rt_bind_texture(image->texture, {.binding = 0,});
        rt_bind_buffer(gl->fragBuf, {.binding = 0, .offset = (size_t)call->uniformOffset,});
        for (auto i = 0; i < call->pathCount; ++i)
        {
            rt_draw_array(nullptr, 0, paths[i].strokeCount, 1, paths[i].strokeOffset, 0);
        }
        rt_end_render(pass);
    }
}

static void nvg_triangles(RTVGcontext* gl, RTVGcall* call)
{
    // nvg_setUniforms(gl, call->uniformOffset, call->image);
    // glDrawArrays(GL_TRIANGLES, call->triangleOffset, call->triangleCount);

    {
        static auto module = rt_create_module_render({
            .vshader = {fillVertShader}, .fshader = {fillFragShader},
            .colors = {
                {
                    .write = {false, false, false, false},
                    .color = {.func = RT_FUNC_ADD, .src = RT_BLEND_ONE, .dst = RT_BLEND_ONE_MINUS_SRC_ALPHA,},
                    .alpha = {.func = RT_FUNC_ADD, .src = RT_BLEND_SRC_ALPHA, .dst = RT_BLEND_ONE_MINUS_SRC_ALPHA,},
                }
            },
            .stencil = {
                .front = { .func = RT_ALWAYS, .sfail = RT_STENCIL_KEEP, .zfail = RT_STENCIL_KEEP, .zpass = RT_STENCIL_INCR },
                .back = { .func = RT_ALWAYS, .sfail = RT_STENCIL_KEEP, .zfail = RT_STENCIL_KEEP, .zpass = RT_STENCIL_INCR },
            },
            .vertex = {{.attrib = {{.location = 0, .offset = 0, .format = RT_VERTEX_FLOAT32X2,},{.location = 1, .offset = sizeof(float) * 2, .format = RT_VERTEX_FLOAT32X2,},},}},
            .primitive = RT_TRIANGLES,
        });
        rt_pass_render_t pass{
            .colors = {
                    {
                        .texture_view = gl->target.default_view,
                        .clear = false,
                    }
            },
            .stencil = {.refer = 0},
        };
        rt_begin_render(pass);
        rt_bind_module_render(module);
        // TODO: bind uniform buffer & texture
        rt_draw_array(nullptr, 0, call->triangleCount, 1, call->triangleOffset, 0);
        rt_end_render(pass);
    }
}

static void rt_renderCancel(void* uptr)
{
    auto gl = (RTVGcontext*)uptr;
    gl->nverts = 0;
    gl->npaths = 0;
    gl->ncalls = 0;
    gl->nuniforms = 0;
}

static GLenum RTVG_convertBlendFuncFactor(int factor)
{
    if (factor == NVG_ZERO)
        return GL_ZERO;
    if (factor == NVG_ONE)
        return GL_ONE;
    if (factor == NVG_SRC_COLOR)
        return GL_SRC_COLOR;
    if (factor == NVG_ONE_MINUS_SRC_COLOR)
        return GL_ONE_MINUS_SRC_COLOR;
    if (factor == NVG_DST_COLOR)
        return GL_DST_COLOR;
    if (factor == NVG_ONE_MINUS_DST_COLOR)
        return GL_ONE_MINUS_DST_COLOR;
    if (factor == NVG_SRC_ALPHA)
        return GL_SRC_ALPHA;
    if (factor == NVG_ONE_MINUS_SRC_ALPHA)
        return GL_ONE_MINUS_SRC_ALPHA;
    if (factor == NVG_DST_ALPHA)
        return GL_DST_ALPHA;
    if (factor == NVG_ONE_MINUS_DST_ALPHA)
        return GL_ONE_MINUS_DST_ALPHA;
    if (factor == NVG_SRC_ALPHA_SATURATE)
        return GL_SRC_ALPHA_SATURATE;
    return GL_INVALID_ENUM;
}

static RTVGblend nvg_blendCompositeOperation(NVGcompositeOperationState op)
{
    RTVGblend blend;
    blend.srcRGB = RTVG_convertBlendFuncFactor(op.srcRGB);
    blend.dstRGB = RTVG_convertBlendFuncFactor(op.dstRGB);
    blend.srcAlpha = RTVG_convertBlendFuncFactor(op.srcAlpha);
    blend.dstAlpha = RTVG_convertBlendFuncFactor(op.dstAlpha);
    if (blend.srcRGB == GL_INVALID_ENUM || blend.dstRGB == GL_INVALID_ENUM || blend.srcAlpha == GL_INVALID_ENUM || blend
        .dstAlpha == GL_INVALID_ENUM)
    {
        blend.srcRGB = GL_ONE;
        blend.dstRGB = GL_ONE_MINUS_SRC_ALPHA;
        blend.srcAlpha = GL_ONE;
        blend.dstAlpha = GL_ONE_MINUS_SRC_ALPHA;
    }
    return blend;
}

static void rt_renderFlush(void* uptr)
{
    auto gl = (RTVGcontext*)uptr;

    // Upload ubo for frag shaders
    // glBindBuffer(GL_UNIFORM_BUFFER, gl->fragBuf);
    // glBufferData(GL_UNIFORM_BUFFER, gl->nuniforms * gl->fragSize, gl->uniforms, GL_STREAM_DRAW);

    // glBindBuffer(GL_ARRAY_BUFFER, gl->vertBuf);
    // glBufferData(GL_ARRAY_BUFFER, gl->nverts * sizeof(NVGvertex), gl->verts, GL_STREAM_DRAW);

    {
        rt_pass_transfer_t pass;
        rt_begin_transfer(pass);
        rt_copy_buffer_data(
            {
                .data = (uint8_t*)gl->uniforms.data(),
                .size = (size_t)gl->nuniforms * sizeof(RTVGfragUniforms),
            },
            {
                .buffer = gl->fragBuf,
            },
            gl->nuniforms * sizeof(RTVGfragUniforms)
            );
        rt_copy_buffer_data(
            {
                .data = (uint8_t*)gl->verts.data(),
                .size = (size_t)gl->nverts * sizeof(NVGvertex),
            },
            {
                .buffer = gl->vertBuf,
            },
            gl->nverts * sizeof(NVGvertex)
            );
        rt_end_transfer(pass);
    }

    for (auto i = 0; i < gl->ncalls; i++)
    {
        RTVGcall* call = &gl->calls[i];

        if (call->type == RTVG_FILL)
            nvg_fill(gl, call);
        else if (call->type == RTVG_CONVEXFILL)
            nvg_convexFill(gl, call);
        else if (call->type == RTVG_STROKE)
            nvg_stroke(gl, call);
        else if (call->type == RTVG_TRIANGLES)
            nvg_triangles(gl, call);
    }

    // Reset calls
    gl->nverts = 0;
    gl->npaths = 0;
    gl->ncalls = 0;
    gl->nuniforms = 0;
}

static int nvg_maxVertCount(const NVGpath* paths, int npaths)
{
    int i, count = 0;
    for (i = 0; i < npaths; i++)
    {
        count += paths[i].nfill;
        count += paths[i].nstroke;
    }
    return count;
}

static RTVGcall* nvg_allocCall(RTVGcontext* gl)
{
    RTVGcall* ret = nullptr;
    if (gl->ncalls + 1 > gl->calls.size()) gl->calls.resize(gl->calls.size() + 1);
    ret = &gl->calls[gl->ncalls++];
    memset(ret, 0, sizeof(RTVGcall));
    return ret;
}

static int nvg_allocPaths(RTVGcontext* gl, int n)
{
    int ret = 0;
    if (gl->npaths + n > gl->paths.size()) gl->paths.resize(gl->paths.size() + n);
    ret = gl->npaths;
    gl->npaths += n;
    return ret;
}

static int nvg_allocVerts(RTVGcontext* gl, int n)
{
    int ret = 0;
    if (gl->nverts + n > gl->verts.size()) gl->verts.resize(gl->verts.size() + n);
    ret = gl->nverts;
    gl->nverts += n;
    return ret;
}

static int nvg_allocFragUniforms(RTVGcontext* gl, int n)
{
    if (gl->nuniforms + n > gl->uniforms.size()) gl->uniforms.resize(gl->uniforms.size() + n);
    auto ret = gl->nuniforms * sizeof(RTVGfragUniforms);
    gl->nuniforms += n;
    return ret;
}

static RTVGfragUniforms* nvg_fragUniformPtr(RTVGcontext* gl, int i)
{
    return (RTVGfragUniforms*)&gl->uniforms[i];
}

static void nvg_vset(NVGvertex* vtx, float x, float y, float u, float v)
{
    vtx->x = x;
    vtx->y = y;
    vtx->u = u;
    vtx->v = v;
}

static void rt_renderFill(void* uptr, NVGpaint* paint, NVGcompositeOperationState compositeOperation,
                          NVGscissor* scissor, float fringe,
                          const float* bounds, const NVGpath* paths, int npaths)
{
    auto gl = (RTVGcontext*)uptr;
    RTVGcall* call = nvg_allocCall(gl);
    NVGvertex* quad;
    RTVGfragUniforms* frag;
    int i, maxverts, offset;

    if (call == nullptr) return;

    call->type = RTVG_FILL;
    call->triangleCount = 4;
    call->pathOffset = nvg_allocPaths(gl, npaths);
    if (call->pathOffset == -1) goto error;
    call->pathCount = npaths;
    call->image = paint->image;
    call->blendFunc = nvg_blendCompositeOperation(compositeOperation);

    if (npaths == 1 && paths[0].convex)
    {
        call->type = RTVG_CONVEXFILL;
        call->triangleCount = 0; // Bounding box fill quad not needed for convex fill
    }

    // Allocate vertices for all the paths.
    maxverts = nvg_maxVertCount(paths, npaths) + call->triangleCount;
    offset = nvg_allocVerts(gl, maxverts);
    if (offset == -1) goto error;

    for (i = 0; i < npaths; i++)
    {
        RTVGpath* copy = &gl->paths[call->pathOffset + i];
        const NVGpath* path = &paths[i];
        memset(copy, 0, sizeof(RTVGpath));
        if (path->nfill > 0)
        {
            copy->fillOffset = offset;
            copy->fillCount = path->nfill;
            memcpy(&gl->verts[offset], path->fill, sizeof(NVGvertex) * path->nfill);
            offset += path->nfill;
        }
        if (path->nstroke > 0)
        {
            copy->strokeOffset = offset;
            copy->strokeCount = path->nstroke;
            memcpy(&gl->verts[offset], path->stroke, sizeof(NVGvertex) * path->nstroke);
            offset += path->nstroke;
        }
    }

    // Setup uniforms for draw calls
    if (call->type == RTVG_FILL)
    {
        // Quad
        call->triangleOffset = offset;
        quad = &gl->verts[call->triangleOffset];
        nvg_vset(&quad[0], bounds[2], bounds[3], 0.5f, 1.0f);
        nvg_vset(&quad[1], bounds[2], bounds[1], 0.5f, 1.0f);
        nvg_vset(&quad[2], bounds[0], bounds[3], 0.5f, 1.0f);
        nvg_vset(&quad[3], bounds[0], bounds[1], 0.5f, 1.0f);

        call->uniformOffset = nvg_allocFragUniforms(gl, 2);
        if (call->uniformOffset == -1) goto error;
        // Simple shader for stencil
        frag = nvg_fragUniformPtr(gl, call->uniformOffset);
        memset(frag, 0, sizeof(*frag));
        frag->strokeThr = -1.0f;
        frag->type = NSVG_SHADER_SIMPLE;
        // Fill shader
        nvg_convertPaint(gl, nvg_fragUniformPtr(gl, call->uniformOffset + sizeof(RTVGfragUniforms)), paint, scissor, fringe, fringe,
                         -1.0f);
    }
    else
    {
        call->uniformOffset = nvg_allocFragUniforms(gl, 1);
        if (call->uniformOffset == -1) goto error;
        // Fill shader
        nvg_convertPaint(gl, nvg_fragUniformPtr(gl, call->uniformOffset), paint, scissor, fringe, fringe, -1.0f);
    }

    return;

error:
    // We get here if call alloc was ok, but something else is not.
    // Roll back the last call to prevent drawing it.
    if (gl->ncalls > 0) gl->ncalls--;
}

static void rt_renderStroke(void* uptr, NVGpaint* paint, NVGcompositeOperationState compositeOperation,
                            NVGscissor* scissor, float fringe,
                            float strokeWidth, const NVGpath* paths, int npaths)
{
    auto gl = (RTVGcontext*)uptr;
    RTVGcall* call = nvg_allocCall(gl);
    int i, maxverts, offset;

    if (call == nullptr) return;

    call->type = RTVG_STROKE;
    call->pathOffset = nvg_allocPaths(gl, npaths);
    if (call->pathOffset == -1) goto error;
    call->pathCount = npaths;
    call->image = paint->image;
    call->blendFunc = nvg_blendCompositeOperation(compositeOperation);

    // Allocate vertices for all the paths.
    maxverts = nvg_maxVertCount(paths, npaths);
    offset = nvg_allocVerts(gl, maxverts);
    if (offset == -1) goto error;

    for (i = 0; i < npaths; i++)
    {
        RTVGpath* copy = &gl->paths[call->pathOffset + i];
        const NVGpath* path = &paths[i];
        memset(copy, 0, sizeof(RTVGpath));
        if (path->nstroke)
        {
            copy->strokeOffset = offset;
            copy->strokeCount = path->nstroke;
            memcpy(&gl->verts[offset], path->stroke, sizeof(NVGvertex) * path->nstroke);
            offset += path->nstroke;
        }
    }

    // Fill shader
    call->uniformOffset = nvg_allocFragUniforms(gl, 2);
    if (call->uniformOffset == -1) goto error;

    nvg_convertPaint(gl, nvg_fragUniformPtr(gl, call->uniformOffset), paint, scissor, strokeWidth, fringe, -1.0f);
    nvg_convertPaint(gl, nvg_fragUniformPtr(gl, call->uniformOffset + sizeof(RTVGfragUniforms)), paint, scissor, strokeWidth,
                     fringe, 1.0f - 0.5f / 255.0f);

    return;

error:
    // We get here if call alloc was ok, but something else is not.
    // Roll back the last call to prevent drawing it.
    if (gl->ncalls > 0) gl->ncalls--;
}

static void rt_renderTriangles(void* uptr, NVGpaint* paint, NVGcompositeOperationState compositeOperation,
                               NVGscissor* scissor,
                               const NVGvertex* verts, int nverts, float fringe)
{
    auto gl = (RTVGcontext*)uptr;
    RTVGcall* call = nvg_allocCall(gl);
    RTVGfragUniforms* frag;

    if (call == nullptr) return;

    call->type = RTVG_TRIANGLES;
    call->image = paint->image;
    call->blendFunc = nvg_blendCompositeOperation(compositeOperation);

    // Allocate vertices for all the paths.
    call->triangleOffset = nvg_allocVerts(gl, nverts);
    if (call->triangleOffset == -1) goto error;
    call->triangleCount = nverts;

    memcpy(&gl->verts[call->triangleOffset], verts, sizeof(NVGvertex) * nverts);

    // Fill shader
    call->uniformOffset = nvg_allocFragUniforms(gl, 1);
    if (call->uniformOffset == -1) goto error;
    frag = nvg_fragUniformPtr(gl, call->uniformOffset);
    nvg_convertPaint(gl, frag, paint, scissor, 1.0f, fringe, -1.0f);
    frag->type = NSVG_SHADER_IMG;

    return;

error:
    // We get here if call alloc was ok, but something else is not.
    // Roll back the last call to prevent drawing it.
    if (gl->ncalls > 0) gl->ncalls--;
}

static int rt_renderCreate(void* uptr)
{
    auto gl = (RTVGcontext*)uptr;
    int align = 4;

    // Create dynamic vertex array
    gl->vertBuf = rt_create_buffer({.size = sizeof(NVGvertex) * 1024,});

    // Create UBOs
    gl->fragBuf = rt_create_buffer({.size = sizeof(RTVGfragUniforms) * 1024,});

    // Some platforms does not allow to have samples to unset textures.
    // Create empty one which is bound when there's no texture specified.
    gl->dummyTex = rt_renderCreateTexture(gl, NVG_TEXTURE_ALPHA, 1, 1, 0, nullptr);

    rt_submit();
    return 1;
}

static void rt_renderDelete(void* uptr)
{
    auto gl = (RTVGcontext*)uptr;
    int i;
    if (gl == nullptr) return;

    rt_destroy_buffer(gl->vertBuf);
    rt_destroy_buffer(gl->fragBuf);
    rt_destroy_texture(gl->target);

    for (auto& texture : gl->textures)
    {
        if (texture.second.id != 0)
            rt_destroy_texture(texture.second.texture);
    }
    gl->textures.clear();

    gl->paths.clear();
    gl->verts.clear();
    gl->uniforms.clear();
    gl->calls.clear();

    free(gl);
}

NVGcontext* nvgCreateGL3(int flags)
{
    NVGparams params;
    NVGcontext* ctx = nullptr;
    auto gl = (RTVGcontext*)malloc(sizeof(RTVGcontext));
    if (gl == nullptr) goto error;
    memset(gl, 0, sizeof(RTVGcontext));

    memset(&params, 0, sizeof(params));
    params.renderCreate = rt_renderCreate;
    params.renderCreateTexture = rt_renderCreateTexture;
    params.renderDeleteTexture = rt_renderDeleteTexture;
    params.renderUpdateTexture = rt_renderUpdateTexture;
    params.renderGetTextureSize = rt_renderGetTextureSize;
    params.renderViewport = rt_renderViewport;
    params.renderCancel = rt_renderCancel;
    params.renderFlush = rt_renderFlush;
    params.renderFill = rt_renderFill;
    params.renderStroke = rt_renderStroke;
    params.renderTriangles = rt_renderTriangles;
    params.renderDelete = rt_renderDelete;
    params.userPtr = gl;
    params.edgeAntiAlias = 1;

    gl->flags = flags;

    ctx = nvgCreateInternal(&params);
    if (ctx == nullptr) goto error;

    return ctx;

error:
    // 'gl' is freed by nvgDeleteInternal.
    if (ctx != nullptr) nvgDeleteInternal(ctx);
    return nullptr;
}

void nvgDeleteGL3(NVGcontext* ctx)
{
    nvgDeleteInternal(ctx);
}
