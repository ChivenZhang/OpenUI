// blur_vertical.slang
// 高斯模糊 —— 垂直 Pass
// 输入：水平模糊后的纹理
// 输出：最终模糊结果

#include "filter_common.slang.h"

#define LOCAL_SIZE 16

groupshared float4 gSharedCol[LOCAL_SIZE + MAX_KERNEL_RADIUS * 2];

[numthreads(1, LOCAL_SIZE, 1)]
void cs_main(
    uint3 groupID           : SV_GroupID,
    uint3 groupThreadID     : SV_GroupThreadID,
    uint3 dispatchThreadID  : SV_DispatchThreadID
) {
    int2 texSize = int2(gParams.textureSize);
    int radius   = int(gParams.blurRadius);
    int x        = int(dispatchThreadID.x);

    if (x >= texSize.x) return;

    int sharedIdx = int(groupThreadID.y) + radius;
    int y         = int(groupID.y) * LOCAL_SIZE + int(groupThreadID.y);

    if (y < texSize.y) {
        gSharedCol[sharedIdx] = gInputTex.Load(int3(x, y, 0));
    } else {
        gSharedCol[sharedIdx] = float4(0, 0, 0, 0);
    }

    // 上边界 halo
    if (groupThreadID.y < radius) {
        int haloY = int(groupID.y) * LOCAL_SIZE - radius + int(groupThreadID.y);
        haloY     = max(haloY, 0);
        gSharedCol[int(groupThreadID.y)] = gInputTex.Load(int3(x, haloY, 0));
    }

    // 下边界 halo
    if (groupThreadID.y >= LOCAL_SIZE - radius) {
        int haloIdx = sharedIdx + radius;
        int haloY   = int(groupID.y) * LOCAL_SIZE + int(groupThreadID.y) + radius;
        haloY       = min(haloY, texSize.y - 1);
        if (haloIdx < LOCAL_SIZE + radius * 2) {
            gSharedCol[haloIdx] = gInputTex.Load(int3(x, haloY, 0));
        }
    }

    GroupMemoryBarrierWithGroupSync();

    float weights[MAX_KERNEL_RADIUS * 2 + 1];
    computeGaussianWeights(weights, radius, float(radius) * 0.5);

    if (y < texSize.y) {
        float4 color = float4(0, 0, 0, 0);
        for (int i = -radius; i <= radius; ++i) {
            int idx = sharedIdx + i;
            color += gSharedCol[idx] * weights[i + radius];
        }
        gOutputTex[int2(x, y)] = color;
    }
}
