#pragma once

#include <havok/hkBase.h>
#include <havok/hkArray.h>
#include <havok/hkAabb.h>
#include <havok/hkBroadPhase.h>
#include <havok/hkCollidable.h>

// Three-axis sweep and prune broad phase.
// Only the fields the recovered code touches are named; the rest stay unkNN.
struct hk3AxisSweep : hkBroadPhase {
    // Object node (16 bytes). m_index holds six endpoint indices into the axis arrays, in the order
    // [0] y-min, [1] z-min, [2] y-max, [3] z-max, [4] x-min, [5] x-max (from getAabbFromNode).
    struct hkBpNode {
        u16 m_index[6];                    // 0x00
        hkTypedBroadPhaseHandle* m_handle; // 0x0C; low bit set marks a free node
    };

    // Axis endpoint (4 bytes), sorted by its 16-bit key. HYPOTHESIS: m_value is the key, the second word is unrecovered.
    struct hkBpEndPoint {
        u16 m_value; // 0x00
        u16 unk02;   // 0x02
    };

    // Check-marker list (HYPOTHESIS). A node handle with bit 0 set holds the byte offset of one of these from the broadphase.
    struct hkBpCheckMarker {
        u32 unk00;           // 0x00
        hkArray<u16> m_list; // 0x04
    };

    // Axis: a sorted array of endpoints (12 bytes, the hkArray header).
    struct hkBpAxis : hkArray<hkBpEndPoint> {
        hkBpAxis();

        // Lower bound: first endpoint in [begin, end) whose key is not below value.
        const hkBpEndPoint* find(const hkBpEndPoint* begin, const hkBpEndPoint* end, u16 value) const;

        // Inserts two endpoints (minKey, maxKey), each tagged with id, keeping the axis sorted.
        // minIndex and maxIndex receive the final endpoint indices.
        void insert(hkBpNode* node, int id, u16 minKey, u16 maxKey, u16& minIndex, u16& maxIndex);
    };

    u8 unk08[0x38];     // 0x08
    float m_offset[3];  // 0x40 HYPOTHESIS: per-axis offset subtracted after dequantization (x, y, z)
    float unk4C;        // 0x4C HYPOTHESIS: negated for the w components of the AABB
    u8 unk50[0x10];     // 0x50
    float m_scale[3];   // 0x60 HYPOTHESIS: quantization scale per axis, dequantization uses 1/scale
    u8 unk6C[4];        // 0x6C
    hkBpNode* m_nodes;  // 0x70
    int m_numNodes;     // 0x74 HYPOTHESIS: node count including the two sentinels
    u8 unk78[4];        // 0x78
    hkBpAxis m_axes[3]; // 0x7C (x, y, z)
    int unkA0;          // 0xA0

    int getNumObjects() const;
    void getAabb(const hkBroadPhaseHandle* handle, hkAabb& aabb) const;
    void getAllAabbs(hkArray<hkAabb>& aabbs) const;
    int getAabbCacheSize() const;
    void getAabbFromNode(const hkBpNode* node, hkAabb& aabb) const;

    static void beginOverlap(const hkBpNode* a, const hkBpNode* b, hkArray<hkBroadPhaseHandlePair>& pairs);
    static void endOverlap(const hkBpNode* a, const hkBpNode* b, hkArray<hkBroadPhaseHandlePair>& pairs);
    void beginOverlapCheckMarker(const hkBpNode* a, u32 value, const hkBpNode* b, hkArray<hkBroadPhaseHandlePair>& pairs);
    void endOverlapCheckMarker(const hkBpNode* a, u32 value, const hkBpNode* b, hkArray<hkBroadPhaseHandlePair>& pairs);
};
