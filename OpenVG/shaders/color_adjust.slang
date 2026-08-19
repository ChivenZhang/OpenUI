// color_adjust.slang
// 色彩调整合并 Pass
// 包含：brightness / contrast / grayscale / invert / opacity / saturate / sepia
// 严格按 CSS filter 从左到右顺序执行

#include "filter_common.slang.h"

// ============================================================
// 各滤镜实现（与 CSS 语义 1:1 对齐）
// ============================================================

float4 applyBrightness(float4 color, float value) {
    return color * value;
}

float4 applyContrast(float4 color, float value) {
    float midpoint = 0.5;
    return (color - midpoint) * value + midpoint;
}

float4 applyGrayscale(float4 color, float amount) {
    float gray = dot(color.rgb, float3(0.299, 0.587, 0.114));
    return float4(lerp(color.rgb, float3(gray), amount), color.a);
}

float4 applyInvert(float4 color, float amount) {
    return float4(lerp(color.rgb, 1.0 - color.rgb, amount), color.a);
}

float4 applyOpacity(float4 color, float value) {
    return float4(color.rgb, color.a * value);
}

float4 applySaturate(float4 color, float value) {
    float gray = dot(color.rgb, float3(0.299, 0.587, 0.114));
    return float4(lerp(float3(gray), color.rgb, value), color.a);
}

float4 applySepia(float4 color, float amount) {
    float3x3 sepiaMatrix = float3x3(
        0.393, 0.769, 0.189,
        0.349, 0.686, 0.168,
        0.272, 0.534, 0.131
    );
    float3 sepiaColor = mul(sepiaMatrix, color.rgb);
    return float4(lerp(color.rgb, sepiaColor, amount), color.a);
}

// ============================================================
// 主入口
// ============================================================

[numthreads(8, 8, 1)]
void cs_main(
    uint3 dispatchThreadID : SV_DispatchThreadID
) {
    int2 uv = int2(dispatchThreadID.xy);
    int2 texSize = int2(gParams.textureSize);

    if (uv.x >= texSize.x || uv.y >= texSize.y) return;

    float4 color = gInputTex.Load(int3(uv, 0));

    // 严格按照 CSS filter 从左到右顺序
    color = applyBrightness(color, gParams.brightness);
    color = applyContrast(color, gParams.contrast);
    color = applyGrayscale(color, gParams.grayscaleAmount);
    color = applyInvert(color, gParams.invertAmount);
    color = applyOpacity(color, gParams.opacity);
    color = applySaturate(color, gParams.saturateAmount);
    color = applySepia(color, gParams.sepiaAmount);

    gOutputTex[uv] = color;
}
