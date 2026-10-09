#pragma once

#include <havok/hkBase.h>

// Output block of the vertex/point collision GSK query (map name hk4dGskVertexCollidePointsOutput). The constructor
// stores one float at 0x00 (HYPOTHESIS: a distance or time initialised to a constant); other members not recovered.
struct hk4dGskVertexCollidePointsOutput {
    float unk0; // 0x00

    hk4dGskVertexCollidePointsOutput(); // stores 0.0f (HYPOTHESIS: constant not verified)
};
