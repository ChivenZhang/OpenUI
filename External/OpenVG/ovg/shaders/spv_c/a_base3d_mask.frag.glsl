#version 450
#extension GL_EXT_scalar_block_layout : require
layout(row_major) uniform;
layout(row_major) buffer;

#line 64 0
layout(binding = 0, set = 2)
uniform sampler2D samplerColor_0;


#line 27
struct fragubo_0
{
    float masktime_0;
    float mul_0;
};


#line 67
layout(binding = 0, set = 3)
layout(scalar) uniform block_fragubo_0
{
    float masktime_0;
    float mul_0;
}pu_0;

#line 66
layout(binding = 1, set = 2)
uniform sampler2D samplerMask_0;


#line 11821 1
layout(location = 0)
out vec4 entryPointParam_fragMain_0;


#line 11821
layout(location = 0)
in vec2 input_uv_0;


#line 11821
layout(location = 1)
in vec4 input_color_0;


#line 70 0
void main()
{

#line 79
    vec4 color_0 = input_color_0 * (texture((samplerColor_0), (input_uv_0))) * (1.0 - step(pu_0.masktime_0, (texture((samplerMask_0), (input_uv_0))).x));


    vec4 premul_0 = color_0;
    premul_0.xyz = color_0.xyz * color_0.w;

#line 83
    entryPointParam_fragMain_0 = mix(color_0, premul_0, vec4(pu_0.mul_0));

#line 83
    return;
}

