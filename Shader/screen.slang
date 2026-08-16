struct VSInput
{
    float2 pos : TEXCOORD0;  // location=0
    float2 uv  : TEXCOORD1;  // location=1
};

struct VSOutput
{
    float4 position : SV_Position;
    float2 texcoord : TEXCOORD0;
};

[shader("vertex")]
VSOutput VSMain(VSInput input)
{
    VSOutput o;
    o.position = float4(input.pos, 0.0f, 1.0f);
    o.texcoord = input.uv;
    return o;
}

Texture2D<float4> g_src   : register(t0, space2);
SamplerState      g_samp  : register(s0, space2);

[shader("fragment")]
float4 FSMain(VSOutput i) : SV_Target0
{
    float4 rgba = g_src.Sample(g_samp, i.texcoord);
    return float4(rgba.r, rgba.g, rgba.b, rgba.a);
}