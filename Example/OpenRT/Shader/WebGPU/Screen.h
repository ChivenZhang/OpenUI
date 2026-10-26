#pragma once
#include <cstddef>
#include <cstdint>
namespace shader { namespace webgpu { namespace screen {
const char VS[] = 
    "struct VSOutput_0\n"
    "{\n"
    "    @builtin(position) sv_position_0 : vec4<f32>,\n"
    "    @location(0) normal_0 : vec3<f32>,\n"
    "    @location(1) uv_0 : vec2<f32>,\n"
    "};\n"
    "\n"
    "struct vertexInput_0\n"
    "{\n"
    "    @location(0) in_vertex_0 : vec3<f32>,\n"
    "    @location(1) in_normal_0 : vec3<f32>,\n"
    "    @location(2) in_uv_0 : vec2<f32>,\n"
    "};\n"
    "\n"
    "@vertex\n"
    "fn VS( _S1 : vertexInput_0) -> VSOutput_0\n"
    "{\n"
    "    var output_0 : VSOutput_0;\n"
    "    output_0.normal_0 = _S1.in_normal_0;\n"
    "    output_0.uv_0 = _S1.in_uv_0;\n"
    "    output_0.sv_position_0 = vec4<f32>(_S1.in_vertex_0, 1.0f);\n"
    "    return output_0;\n"
    "}\n"
    "\n"
    "\n"
;
const size_t VS_sizeInBytes = 542;


const char FS[] = 
    "@binding(0) @group(0) var texture0_texture_0 : texture_2d<f32>;\n"
    "\n"
    "@binding(1) @group(0) var texture0_sampler_0 : sampler;\n"
    "\n"
    "struct pixelOutput_0\n"
    "{\n"
    "    @location(0) output_0 : vec4<f32>,\n"
    "};\n"
    "\n"
    "struct pixelInput_0\n"
    "{\n"
    "    @location(0) normal_0 : vec3<f32>,\n"
    "    @location(1) uv_0 : vec2<f32>,\n"
    "};\n"
    "\n"
    "@fragment\n"
    "fn FS( _S1 : pixelInput_0, @builtin(position) sv_position_0 : vec4<f32>) -> pixelOutput_0\n"
    "{\n"
    "    ;\n"
    "    var _S2 : pixelOutput_0 = pixelOutput_0( (textureSample((texture0_texture_0), (texture0_sampler_0), (_S1.uv_0))) );\n"
    "    return _S2;\n"
    "}\n"
    "\n"
    "\n"
;
const size_t FS_sizeInBytes = 536;


} } }
