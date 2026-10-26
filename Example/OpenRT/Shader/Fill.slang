// Nanovg fill. std140 layout matches RTVGfragUniforms / RTVGviewUniforms.
// Matrices are three float4 columns, matching nvg_xformToMat3x4.
struct ViewUniforms
{
    float2 viewSize;
    float2 pad;
};

struct FragUniforms
{
    float4 scissorMat[3];
    float4 paintMat[3];
    float4 innerCol;
    float4 outerCol;
    float2 scissorExt;
    float2 scissorScale;
    float2 extent;
    float radius;
    float feather;
    float strokeMult;
    float strokeThr;
    int texType;
    int type;
};

[[vk::binding(1, 0)]]
ConstantBuffer<ViewUniforms> view : register(b1);

[[vk::binding(0, 0)]]
ConstantBuffer<FragUniforms> frag : register(b0);

// Occupy s0 and s1 so the combined sampler's HLSL register is s2, matching RTVG_TEX_BINDING.
// These are unused and stripped from GLSL, SPIR-V, WGSL, and DXIL.
SamplerState _samplerPad0 : register(s0);
SamplerState _samplerPad1 : register(s1);

[[vk::binding(2, 0)]]
Sampler2D tex : register(t2);

struct VSInput
{
    [[vk::location(0)]] float2 vertex : POSITION;
    [[vk::location(1)]] float2 tcoord : TEXCOORD0;
};

struct VSOutput
{
    float4 sv_position : SV_Position;
    [[vk::location(0)]] float2 ftcoord : TEXCOORD0;
    [[vk::location(1)]] float2 fpos : TEXCOORD1;
};

float3 mul3(float4 c0, float4 c1, float4 c2, float3 v)
{
    return c0.xyz * v.x + c1.xyz * v.y + c2.xyz * v.z;
}

[shader("vertex")]
VSOutput VS(VSInput input)
{
    VSOutput output;
    output.ftcoord = input.tcoord;
    output.fpos = input.vertex;
    float2 viewSize = view.viewSize;
    output.sv_position = float4(2.0 * input.vertex.x / viewSize.x - 1.0, 1.0 - 2.0 * input.vertex.y / viewSize.y, 0.0, 1.0);
    return output;
}

float sdroundrect(float2 pt, float2 ext, float rad)
{
    float2 ext2 = ext - float2(rad, rad);
    float2 d = abs(pt) - ext2;
    return min(max(d.x, d.y), 0.0) + length(max(d, 0.0)) - rad;
}

float scissorMask(float2 p)
{
    float2 sc = abs(mul3(frag.scissorMat[0], frag.scissorMat[1], frag.scissorMat[2], float3(p, 1.0)).xy) - frag.scissorExt;
    sc = float2(0.5, 0.5) - sc * frag.scissorScale;
    return clamp(sc.x, 0.0, 1.0) * clamp(sc.y, 0.0, 1.0);
}

float strokeMask(float2 ftcoord)
{
    return min(1.0, (1.0 - abs(ftcoord.x * 2.0 - 1.0)) * frag.strokeMult) * min(1.0, ftcoord.y);
}

[shader("fragment")]
float4 FS(VSOutput input) : SV_Target
{
    float scissor = scissorMask(input.fpos);
    float strokeAlpha = strokeMask(input.ftcoord);
    if (strokeAlpha < frag.strokeThr)
        discard;

    float4 result = float4(0.0, 0.0, 0.0, 0.0);
    if (frag.type == 0)
    {
        float2 pt = mul3(frag.paintMat[0], frag.paintMat[1], frag.paintMat[2], float3(input.fpos, 1.0)).xy;
        float d = clamp((sdroundrect(pt, frag.extent, frag.radius) + frag.feather * 0.5) / frag.feather, 0.0, 1.0);
        float4 color = lerp(frag.innerCol, frag.outerCol, d);
        color *= strokeAlpha * scissor;
        result = color;
    }
    else if (frag.type == 1)
    {
        float2 pt = mul3(frag.paintMat[0], frag.paintMat[1], frag.paintMat[2], float3(input.fpos, 1.0)).xy / frag.extent;
        float4 color = tex.Sample(pt);
        if (frag.texType == 1) color = float4(color.xyz * color.w, color.w);
        if (frag.texType == 2) color = float4(color.x, color.x, color.x, color.x);
        color *= frag.innerCol;
        color *= strokeAlpha * scissor;
        result = color;
    }
    else if (frag.type == 2)
    {
        result = float4(1.0, 1.0, 1.0, 1.0);
    }
    else if (frag.type == 3)
    {
        float4 color = tex.Sample(input.ftcoord);
        if (frag.texType == 1) color = float4(color.xyz * color.w, color.w);
        if (frag.texType == 2) color = float4(color.x, color.x, color.x, color.x);
        color *= scissor;
        result = color * frag.innerCol;
    }
    return result;
}
