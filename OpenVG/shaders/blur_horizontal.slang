// blur_horizontal.slang
// 高斯模糊 —— 水平 Pass
// 对应 CSS: filter: blur(Npx);

#include "filter_common.slang.h"

#define LOCAL_SIZE 16

groupshared float4 gSharedRow[LOCAL_SIZE + MAX_KERNEL_RADIUS * 2];

[numthreads(LOCAL_SIZE, 1, 1)]
void cs_main(
    uint3 groupID           : SV_GroupID,
    uint3 groupThreadID     : SV_GroupThreadID,
    uint3 dispatchThreadID  : SV_DispatchThreadID
) {
    int2 texSize = int2(gParams.textureSize);
    int radius   = int(gParams.blurRadius);
    int y        = int(dispatchThreadID.y);

    if (y >= texSize.y) return;

    // 将一行数据加载到 groupshared 内存
    int sharedIdx = int(groupThreadID.x) + radius;
    int x         = int(groupID.x) * LOCAL_SIZE + int(groupThreadID.x);

    if (x < texSize.x) {
        gSharedRow[sharedIdx] = gInputTex.Load(int3(x, y, 0));
    } else {
        gSharedRow[sharedIdx] = float4(0, 0, 0, 0);
    }

    // 左边界 halo
    if (groupThreadID.x < radius) {
        int haloX = int(groupID.x) * LOCAL_SIZE - radius + int(groupThreadID.x);
        haloX     = max(haloX, 0);
        gSharedRow[int(groupThreadID.x)] = gInputTex.Load(int3(haloX, y, 0));
    }

    // 右边界 halo
    if (groupThreadID.x >= LOCAL_SIZE - radius) {
        int haloIdx = sharedIdx + radius;
        int haloX   = int(groupID.x) * LOCAL_SIZE + int(groupThreadID.x) + radius;
        haloX       = min(haloX, texSize.x - 1);
        if (haloIdx < LOCAL_SIZE + radius * 2) {
            gSharedRow[haloIdx] = gInputTex.Load(int3(haloX, y, 0));
        }
    }

    GroupMemoryBarrierWithGroupSync();

    // 计算高斯权重
    float weights[MAX_KERNEL_RADIUS * 2 + 1];
    computeGaussianWeights(weights, radius, float(radius) * 0.5);

    if (x < texSize.x) {
        float4 color = float4(0, 0, 0, 0);
        for (int i = -radius; i <= radius; ++i) {
            int idx = sharedIdx + i;
            color += gSharedRow[idx] * weights[i + radius];
        }
        gOutputTex[int2(x, y)] = color;
    }
}
