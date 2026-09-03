struct VSInput
{
	float3 pos;
	float2 uv;
	float4 color;
#ifdef DOUBLESIDEDCOLOR
	float4 color1;
#endif
};

struct VSOutput
{
	float4 pos : SV_Position;
	float2 uv;
	float4 color;
#ifdef DOUBLESIDEDCOLOR
	float4 color1;
#endif
};

struct PushConsts
{
	float4x4 mvp;
	uint instance_pos;
};

struct fragubo
{
	float masktime;
	float mul;
};
//[[vk::push_constant]] PushConsts pc;
[[vk::binding(0, 1)]] ConstantBuffer<PushConsts> pc;
#ifdef ID_INSTANCING
[[vk::binding(0, 0)]] StructuredBuffer<float4x4> instance_model_matrix;
[shader("vertex")]
VSOutput main(VSInput input, uint InstanceIndex : SV_InstanceID)
{
	VSOutput output;
	float4x4 m = mul(pc.mvp, instance_model_matrix[InstanceIndex + pc.instance_pos]);
	output.pos = mul(m, float4(input.pos, 1.0));
	output.uv = input.uv;
	output.color = input.color;
#ifdef DOUBLESIDEDCOLOR
	output.color1 = input.color1;
#endif
	return output;
}
#else
[shader("vertex")]
VSOutput main(VSInput input)
{
	VSOutput output;
	output.pos = mul(pc.mvp, float4(input.pos, 1.0));
	output.uv = input.uv;
	output.color = input.color;
#ifdef DOUBLESIDEDCOLOR
	output.color1 = input.color1;
#endif
	return output;
}
#endif

[[vk::binding(0, 2)]] Sampler2D samplerColor;
#ifdef COLOR_MASK
[[vk::binding(1, 2)]] Sampler2D samplerMask;
[[vk::binding(0, 3)]] ConstantBuffer<fragubo> pu;
#endif
[shader("fragment")]
float4 fragMain(VSOutput input, bool FrontFacing : SV_IsFrontFace) : SV_TARGET
{
	float4 color = input.color;
#ifdef DOUBLESIDEDCOLOR
	color = FrontFacing ? input.color : input.color1;
#endif
	float4 c = samplerColor.Sample(input.uv);
	color *= c;
#ifdef COLOR_MASK
	color *= 1.0 - step(pu.masktime, samplerMask.Sample(input.uv).r);
#endif
	c = color;
	float4 premul = color;
	premul.rgb *= premul.a;
	color = lerp(c, premul, pu.mul);
	return color;
}