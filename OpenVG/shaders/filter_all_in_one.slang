// filter_all_in_one.slang
// 单 Pass 全量版：小半径模糊 + 全部色彩滤镜
// 适合快速预览 / 移动端 / 性能敏感场景
// 注意：此版本模糊质量较低（小核 + 无 groupshared），仅用于轻量场景

#include "filter_common.slang.h"

// ============================================================
// 快速模糊（固定小核，适合 radius <= 4）
// ============================================================

float4 fastBlur(int2 uv, int radius) {
    if (radius <= 0) {
        return gInputTex.Load(int3(uv, 0));
    }

    float4 sum = float4(0, 0, 0, 0);
    float weightSum = 0.0;
    int2 texSize = int2(gParams.textureSize);

    for (int y = -radius; y <= radius; ++y) {
        for (int x = -radius; x <= radius; ++x) {
            int2 sampleUV = uv + int2(x, y);
            sampleUV = clamp(sampleUV, int2(0, 0), texSize - int2(1, 1));

            float dist = length(float2(x, y));
            float weight = exp(-dist * dist / (2.0 * float(radius) * 0.5 * float(radius) * 0.5));
            sum += gInputTex.Load(int3(sampleUV, 0)) * weight;
            weightSum += weight;
        }
    }
    return sum / weightSum;
}

// ============================================================
// 色彩滤镜（同 color_adjust.slang）
// ============================================================

float4 applyBrightness(float4 c, float v) { return c * v; }
float4 applyContrast(float4 c, float v)   { return (c - 0.5) * v + 0.5; }
float4 applyGrayscale(float4 c, float a)  { float g = dot(c.rgb, float3(0.299,0.587,0.114)); return float4(lerp(c.rgb, float3(g), a), c.a); }
float4 applyInvert(float4 c, float a)     { return float4(lerp(c.rgb, 1.0-c.rgb, a), c.a); }
float4 applyOpacity(float4 c, float v)    { return float4(c.rgb, c.a * v); }
float4 applySaturate(float4 c, float v)   { float g = dot(c.rgb, float3(0.299,0.587,0.114)); return float4(lerp(float3(g), c.rgb, v), c.a); }

float4 applySepia(float4 c, float a) {
    float3x3 m = float3x3(0.393,0.769,0.189, 0.349,0.686,0.168, 0.272,0.534,0.131);
    return float4(lerp(c.rgb, mul(m, c.rgb), a), c.a);
}

// ============================================================
// 主入口
// ============================================================

[numthreads(8, 8, 1)]
void cs_main(uint3 dispatchThreadID : SV_DispatchThreadID) {
    int2 uv = int2(dispatchThreadID.xy);
    int2 texSize = int2(gParams.textureSize);
    if (uv.x >= texSize.x || uv.y >= texSize.y) return;

    float4 color = gParams.enableBlur
        ? fastBlur(uv, int(gParams.blurRadius))
        : gInputTex.Load(int3(uv, 0));

    color = applyBrightness(color, gParams.brightness);
    color = applyContrast(color, gParams.contrast);
    color = applyGrayscale(color, gParams.grayscaleAmount);
    color = applyInvert(color, gParams.invertAmount);
    color = applyOpacity(color, gParams.opacity);
    color = applySaturate(color, gParams.saturateAmount);
    color = applySepia(color, gParams.sepiaAmount);

    gOutputTex[uv] = color;
}
