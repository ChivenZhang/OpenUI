// Fullscreen blit. Vertex locations match rt_vertex_vertex / rt_vertex_normal / rt_vertex_uv.
struct VSInput
{
    [[vk::location(0)]] float3 in_vertex : POSITION;
    [[vk::location(1)]] float3 in_normal : NORMAL;
    [[vk::location(2)]] float2 in_uv : TEXCOORD0;
};

struct VSOutput
{
    float4 sv_position : SV_Position;
    [[vk::location(1)]] float3 normal : NORMAL;
    [[vk::location(2)]] float2 uv : TEXCOORD0;
};

[shader("vertex")]
VSOutput VS(VSInput input)
{
    VSOutput output;
    output.normal = input.in_normal;
    output.uv = input.in_uv;
    output.sv_position = float4(input.in_vertex, 1.0);
    return output;
}

[[vk::binding(0, 0)]]
Sampler2D texture0 : register(t0);

[shader("fragment")]
float4 FS(VSOutput input) : SV_Target
{
    return texture0.Sample(input.uv);
}
