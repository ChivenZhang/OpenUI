struct VSInput {
  float2 position : POSITION;
  float2 uv : TEXCOORD0;
};

struct VSOutput {
  float4 position : SV_Position;
  float2 uv : TEXCOORD0;
};

[shader("vertex")]
VSOutput VSMain(VSInput input) {
  VSOutput output;
  output.position = float4(input.position, 0.0, 1.0);
  output.uv = input.uv;
  return output;
}

Texture2D<float4> srcTexture : register(t0, space2);
SamplerState linearSampler : register(s0, space2);

[shader("fragment")]
float4 FSMain(VSOutput input) : SV_Target0 {
  return srcTexture.Sample(linearSampler, input.uv);
}