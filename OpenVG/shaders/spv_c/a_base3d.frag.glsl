#version 450
#extension GL_EXT_scalar_block_layout : require
layout(row_major) uniform;
layout(row_major) buffer;

#line 62 0
layout(binding = 0, set = 2)
uniform sampler2D samplerColor_0;


#line 1092 1
layout(location = 0)
out vec4 entryPointParam_fragMain_0;


#line 1092
layout(location = 0)
in vec2 input_uv_0;


#line 1092
layout(location = 1)
in vec4 input_color_0;


#line 68 0
void main()
{

#line 74
    vec4 _S1 = input_color_0 * (texture((samplerColor_0), (input_uv_0)));

#line 70
    vec4 color_0 = _S1;

#line 79
    color_0.xyz = _S1.xyz * _S1.w;

#line 79
    entryPointParam_fragMain_0 = color_0;

#line 79
    return;
}

