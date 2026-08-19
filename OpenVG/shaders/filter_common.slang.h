// filter_common.slangh
// 公共头文件：纹理绑定、采样器、Uniform 参数、工具函数

#ifndef FILTER_COMMON_H
#define FILTER_COMMON_H

// ============================================================
// 纹理绑定
// ============================================================

// t0: 输入纹理
// t1: 输出纹理（UAV）
// t2: 中间纹理（用于多 Pass 读写）

[[vk::binding(0, 0)]] Texture2D    gInputTex  : register(t0);
[[vk::binding(1, 0)]] RWTexture2D<float4> gOutputTex : register(u0);

// 可选：线性采样器（用于模糊）
SamplerState gLinearSampler : register(s0);

// ============================================================
// Uniform / Constant Buffer
// ============================================================

struct FilterParams {
    float2  texelSize;          // 1.0 / textureSize
    float2  textureSize;        // 原始纹理尺寸

    // --- Blur ---
    float   blurRadius;         // 模糊半径（像素）
    int     enableBlur;         // 是否启用模糊
    int     _pad0;
    int     _pad1;

    // --- Color Adjust ---
    float   brightness;         // 亮度乘数（1.0 = 不变）
    float   contrast;           // 对比度乘数（1.0 = 不变）
    float   grayscaleAmount;    // 灰度程度 0~1
    float   invertAmount;       // 反色程度 0~1

    float   opacity;            // 透明度 0~1
    float   saturateAmount;     // 饱和度乘数
    float   sepiaAmount;        // 复古褐色程度 0~1
    int     enableColorAdjust;  // 是否启用色彩调整
};

[[vk::binding(2, 0)]] ConstantBuffer<FilterParams> gParams : register(b0);

// ============================================================
// 工具函数
// ============================================================

// 安全采样（带边界 clamp）
float4 sampleSafe(int2 uv, int2 size) {
    uv = clamp(uv, int2(0, 0), size - int2(1, 1));
    return gInputTex.Load(int3(uv, 0));
}

// 将 0~1 的 UV 转为像素坐标
int2 uvToInt2(float2 uv, float2 size) {
    return int2(uv * size);
}

// 预计算高斯权重（一维，可分离）
static const int MAX_KERNEL_RADIUS = 16;

void computeGaussianWeights(out float weights[MAX_KERNEL_RADIUS * 2 + 1], int radius, float sigma) {
    float sum = 0.0;
    int count = radius * 2 + 1;
    for (int i = 0; i < count; ++i) {
        float x = float(i - radius);
        weights[i] = exp(-(x * x) / (2.0 * sigma * sigma));
        sum += weights[i];
    }
    for (int j = 0; j < count; ++j) {
        weights[j] /= sum;
    }
}

#endif // FILTER_COMMON_H
