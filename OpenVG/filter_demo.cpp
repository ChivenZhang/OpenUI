// =============================================================================
// filter_demo.cpp
// 示例：如何在 C++ / Slang 宿主程序中调度这些着色器
// 伪代码风格，适配 DirectX 12 / Vulkan 通用流程
// =============================================================================

#include <slang.h>
#include <vector>
#include <cstdint>

// -----------------------------------------------------------------------------
// 常量缓冲区结构体（必须与 Slang 的 cbuffer 完全对齐）
// -----------------------------------------------------------------------------
struct FilterParams
{
    float  texelSizeX;        // offset  0
    float  texelSizeY;        // offset  4
    uint32_t textureWidth;    // offset  8
    uint32_t textureHeight;   // offset 12
    float  blurRadius;        // offset 16
    float  brightness;         // offset 20
    float  contrast;           // offset 24
    float  grayscaleAmount;    // offset 28
    float  invertAmount;       // offset 32
    float  opacity;            // offset 36
    float  saturateAmount;     // offset 40
    float  sepiaAmount;        // offset 44
    uint32_t enableBlur;       // offset 48
    uint32_t enableBrightness; // offset 52
    uint32_t enableContrast;   // offset 56
    uint32_t enableGrayscale;  // offset 60
    uint32_t enableInvert;     // offset 64
    uint32_t enableOpacity;    // offset 68
    uint32_t enableSaturate;   // offset 72
    uint32_t enableSepia;      // offset 76
    // 总计 80 bytes
};

// -----------------------------------------------------------------------------
// 示例：构建参数（对应 CSS filter 链）
//   filter: blur(5px) brightness(1.2) contrast(150%)
//           grayscale(0%) invert(0%) opacity(100%)
//           saturate(2) sepia(0%);
// -----------------------------------------------------------------------------
FilterParams createParams(uint32_t width, uint32_t height)
{
    FilterParams p = {};
    p.texelSizeX      = 1.0f / float(width);
    p.texelSizeY      = 1.0f / float(height);
    p.textureWidth    = width;
    p.textureHeight   = height;

    // blur(5px)
    p.blurRadius      = 5.0f;
    p.brightness      = 1.2f;     // brightness(1.2)
    p.contrast        = 1.5f;     // contrast(150%)
    p.grayscaleAmount = 0.0f;     // grayscale(0%)
    p.invertAmount    = 0.0f;     // invert(0%)
    p.opacity         = 1.0f;     // opacity(100%)
    p.saturateAmount  = 2.0f;     // saturate(2)
    p.sepiaAmount     = 0.0f;     // sepia(0%)

    // 启用标记
    p.enableBlur       = 1;
    p.enableBrightness = 1;
    p.enableContrast   = 1;
    p.enableGrayscale  = 0;
    p.enableInvert     = 0;
    p.enableOpacity    = 0;
    p.enableSaturate   = 1;
    p.enableSepia      = 0;

    return p;
}

// -----------------------------------------------------------------------------
// 调度流程（伪代码）
// -----------------------------------------------------------------------------
/*
void dispatchFilters(
    Slang::ComputePipeline blurH,
    Slang::ComputePipeline blurV,
    Slang::ComputePipeline colorAdjust,
    Texture2D input,
    Texture2D temp,
    Texture2D output,
    FilterParams params)
{
    // Pass 1: 水平模糊 (input -> temp)
    blurH.setTexture("gInputTex", input);
    blurH.setRWTexture("gOutputTex", temp);
    blurH.setConstantBuffer("FilterParams", &params);
    blurH.dispatch((width + 7) / 8, (height + 7) / 8, 1);

    // Pass 2: 垂直模糊 (temp -> output)
    blurV.setTexture("gInputTex", temp);
    blurV.setRWTexture("gOutputCalc", output);
    blurV.setConstantBuffer("FilterParams", &params);
    blurV.dispatch((width + 7) / 8, (height + 7) / 8, 1);

    // Pass 3: 色彩调整 (output -> final)
    // 如果模糊输出在 temp2，则输入为 temp2
    colorAdjust.setTexture("gInputTex", output);
    colorAdjust.setRWTexture("gOutputTex", finalOutput);
    colorAdjust.setConstantBuffer("FilterParams", &params);
    colorAdjust.dispatch((width + 7) / 8, (height + 7) / 8, 1);
}
*/

// -----------------------------------------------------------------------------
// main
// -----------------------------------------------------------------------------
int main()
{
    uint32_t width  = 1920;
    uint32_t height = 1080;

    FilterParams params = createParams(width, height);

    // ... 初始化 Slang 会话、加载 .slang 文件、创建管线 ...
    // ... 上传纹理、绑定参数、调度计算着色器 ...

    return 0;
}
