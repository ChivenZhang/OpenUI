// blit_fs.slang
// 全屏 blit 像素着色器
// 采样 outputTex 并输出到交换链

struct VSOutput {
    float4 pos : SV_POSITION;
    float2 uv  : TEXCOORD0;
};
 
[[vk::binding(0, 2)]] Sampler2D gTex;

float4 main(VSOutput i) : SV_TARGET {
    // 可选：简单色调映射（如果用了 HDR 纹理）
    float4 color = gTex.Sample(i.uv);

    // sRGB 转换（如果输出是 sRGB 交换链）
    // color.rgb = pow(color.rgb, 1.0/2.2);

    return color;
}
