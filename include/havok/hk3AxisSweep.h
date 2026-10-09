#pragma once

#include <havok/hkBase.h>
#include <havok/hkArray.h>
#include <havok/hkAabb.h>
#include <havok/hkBroadPhase.h>
#include <havok/hkCollidable.h>

// Three-axis sweep and prune broad phase.
// Only the fields the recovered code touches are named; 0x08..0x6F and 0x78..0x9F are not recovered yet.
struct hk3AxisSweep : hkBroadPhase {
    // Object node (16 bytes). The broad-phase handle id indexes the node array.
    struct hkBpNode {
        u32 unk00; // 0x00
        u32 unk04; // 0x04
        u32 unk08; // 0x08
        hkTypedBroadPhaseHandle* m_handle; // 0x0C; low bit set marks a free node
    };

    // Axis endpoint (4 bytes), sorted by its 16-bit key. HYPOTHESIS: m_value is the key, the second word is unrecovered.
    struct hkBpEndPoint {
        u16 m_value; // 0x00
        u16 unk02;   // 0x02
    };

    // Axis entry (12 bytes). The constructor sets unk08 to 0x80000000.
    struct hkBpAxis {
        u32 unk00; // 0x00
        u32 unk04; // 0x04
        u32 unk08; // 0x08
        hkBpAxis();

        // Lower bound: first endpoint in [begin, end) whose key is not below value.
        const hkBpEndPoint* find(const hkBpEndPoint* begin, const hkBpEndPoint* end, u16 value) const;
    };

    u8 unk08[0x68];    // 0x08
    hkBpNode* m_nodes; // 0x70
    int m_numNodes;    // 0x74 HYPOTHESIS: node count including the two sentinels
    u8 unk78[0x28];    // 0x78
    int unkA0;         // 0xA0

    int getNumObjects() const;
    void getAabb(const hkBroadPhaseHandle* handle, hkAabb& aabb) const;
    void getAllAabbs(hkArray<hkAabb>& aabbs) const;
    int getAabbCacheSize() const;
    void getAabbFromNode(const hkBpNode* node, hkAabb& aabb) const;

    static void beginOverlap(const hkBpNode* a, const hkBpNode* b, hkArray<hkBroadPhaseHandlePair>& pairs);
    static void endOverlap(const hkBpNode* a, const hkBpNode* b, hkArray<hkBroadPhaseHandlePair>& pairs);
};
