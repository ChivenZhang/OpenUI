// sg_sdl3gpu.slang
// Target: SDL3 GPU + Slang -> SPIR-V
// Binding rule (SPIR-V):
//   VS: set1=binding0 UBO
//   FS: set2=binding0 sampled texture, set3=binding0 UBO
//   (no samplers used explicitly; sampler2D is combined image sampler)

//==============
// Vertex stage
//==============

struct VertexInput {
    float2 vertex : ATTRIB0;
    float2 tcoord : ATTRIB1;
};

struct VertexOutput {
    float4 position : SV_Position;
    float2 ftcoord  : TEXCOORD0;
    float2 fpos     : TEXCOORD1;
};

struct VertexUniforms {
    float4 viewSize;
};
// VS UBO: set=1 binding=0  (SDL3 GPU rule)
ConstantBuffer<VertexUniforms> vtx : register(b0, space1);

[shader("vertex")]
VertexOutput VSMain(VertexInput input) {
    VertexOutput output;
    output.ftcoord = input.tcoord;
    output.fpos    = input.vertex;
    output.position = float4(
        2.0 * input.vertex.x / vtx.viewSize.x - 1.0,
        1.0 - 2.0 * input.vertex.y / vtx.viewSize.y,
        0.0, 1.0);
    return output;
}

//================
// Fragment stage
//================

struct FragmentInput {
    float2 ftcoord : TEXCOORD0;
    float2 fpos    : TEXCOORD1;
};

struct FragmentOutput {
    float4 color : SV_Target;
};

struct FragUniforms {
    float4 scissorMat[3];
    float4 paintMat[3];
    float4 innerCol;
    float4 outerCol;
    float4 scissorExtScale;     // xy=extent zw=scale
    float4 extentRadiusFeather; // xy=extent z=radius w=feather
    float4 strokeData;          // x=strokeMult y=strokeThr z=texType w=type
};
// FS UBO: set=3 binding=0  (SDL3 GPU rule)
ConstantBuffer<FragUniforms> frag : register(b0, space3);

// FS sampled texture: set=2 binding=0  (SDL3 GPU rule)
// Slang 里用 CombinedImageSampler 对应 GLSL sampler2D
Texture2D<float4> tex : register(t0, space2);
SamplerState texSamp : register(s0, space2);

float2 sgMul3x3(float4 m[3], float2 p) {
    return float2(
        m[0].x*p.x + m[1].x*p.y + m[2].x,
        m[0].y*p.x + m[1].y*p.y + m[2].y);
}

float sgRoundRect(float2 pt, float2 ext, float rad) {
    float2 ext2 = ext - float2(rad, rad);
    float2 d = abs(pt) - ext2;
    return min(max(d.x, d.y), 0.0) + length(max(d, 0.0)) - rad;
}

float sgScissorMask(float2 p) {
    float2 sc = abs(sgMul3x3(frag.scissorMat, p)) - frag.scissorExtScale.xy;
    sc = float2(0.5, 0.5) - sc * frag.scissorExtScale.zw;
    return saturate(sc.x) * saturate(sc.y);
}

float sgStrokeMask(float2 ftcoord) {
    return min(1.0, (1.0 - abs(ftcoord.x*2.0 - 1.0)) * frag.strokeData.x)
         * min(1.0, ftcoord.y);
}

[shader("fragment")]
FragmentOutput FSMain(FragmentInput input) {
    FragmentOutput output;
    float scissor = sgScissorMask(input.fpos);
    float sa = sgStrokeMask(input.ftcoord);
    if (sa < frag.strokeData.y) discard;

    float4 result;

    if (frag.strokeData.w == 0.0) {               // gradient
        float2 pt = sgMul3x3(frag.paintMat, input.fpos);
        float d = saturate((sgRoundRect(pt, frag.extentRadiusFeather.xy,
                         frag.extentRadiusFeather.z)
                         + frag.extentRadiusFeather.w*0.5)
                         / frag.extentRadiusFeather.w);
        float4 c = lerp(frag.innerCol, frag.outerCol, d);
        c *= sa * scissor;
        result = c;

    } else if (frag.strokeData.w == 1.0) {        // image
        float2 pt = sgMul3x3(frag.paintMat, input.fpos)
                    / frag.extentRadiusFeather.xy;
        float4 c = tex.Sample(texSamp, pt);
        if (frag.strokeData.z == 1.0) c = float4(c.xyz*c.w, c.w);
        if (frag.strokeData.z == 2.0) c = float4(c.x);
        c *= frag.innerCol * sa * scissor;
        result = c;

    } else if (frag.strokeData.w == 2.0) {        // stencil
        result = float4(1.0);

    } else {                                      // textured tris
        float4 c = tex.Sample(texSamp, input.ftcoord);
        if (frag.strokeData.z == 1.0) c = float4(c.xyz*c.w, c.w);
        if (frag.strokeData.z == 2.0) c = float4(c.x);
        c *= scissor;
        result = c * frag.innerCol;
    }

    output.color = result;
    return output;
}