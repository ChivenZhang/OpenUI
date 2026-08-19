// blit_vs.slang
// 全屏三角形顶点着色器
// 输出裁剪空间位置和 UV 坐标

struct VSOutput {
    float4 pos : SV_POSITION;
    float2 uv  : TEXCOORD0;
};

VSOutput main(uint vid : SV_VertexID) {
    // 全屏三角形（覆盖整个视口）
    float2 positions[3] = {
        float2(-1.0, -1.0),
        float2( 3.0, -1.0),
        float2(-1.0,  3.0),
    };
    float2 uvs[3] = {
        float2(0.0, 1.0),
        float2(2.0, 1.0),
        float2(0.0, -1.0),
    };

    VSOutput o;
    o.pos = float4(positions[vid], 0.0, 1.0);
    o.uv  = uvs[vid];
    return o;
}
