
struct VSInput {
    float3 position : POSITION;
    float2 uv       : TEXCOORD0;
};

struct VSOutput {
    float4 position : SV_Position;
    float2 uv       : TEXCOORD0;
};

[shader("vertex")]
VSOutput VSMain(VSInput input) {
    VSOutput output;
    output.position = float4(input.position, 1.0);
    output.uv       = input.uv;
    return output;
}

// ── 片段着色器 ──
Texture2D    srcTexture : register(t0);
Texture2D    dstTexture : register(t1);
SamplerState linearSampler : register(s0);

[shader("fragment")]
float4 FSMain(VSOutput input) : SV_Target0 {
    float4 src = srcTexture.Sample(linearSampler, input.uv);
    float4 dst = dstTexture.Sample(linearSampler, input.uv);

    // 标准 Alpha 混合：result = src * src.a + dst * (1 - src.a)
    float3 color = src.rgb * src.a + dst.rgb * (1.0 - src.a);
    float  alpha = src.a + dst.a * (1.0 - src.a);

    return float4(color, alpha);
}