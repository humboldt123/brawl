#pragma once

#include <StaticAssert.h>
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
        union {
            u16 m_index[6];                // 0x00
            // Packed Y/Z lanes used by the broad-phase separation test.
            struct {
                u32 m_minYZ;
                u32 m_maxYZ;
                u16 m_minX;
                u16 m_maxX;
            };
        };
        hkTypedBroadPhaseHandle* m_handle; // 0x0C; low bit set encodes a check-marker offset
    };

    // Axis endpoint (4 bytes), sorted by its 16-bit key. Even keys are minima,
    // odd keys are maxima; the node ID links each endpoint back to its object.
    struct hkBpEndPoint {
        u16 m_value; // 0x00
        u16 m_nodeId; // 0x02
    };

    // A tagged node handle stores the byte offset into the check-marker array.
    // Each marker caches the IDs crossing its X interval.
    struct hkBpCheckMarker {
        u16 m_nodeId;       // 0x00
        u16 m_value;       // 0x02 X key of this marker
        hkArray<u16> m_list; // 0x04
    };

    // Axis: a sorted array of endpoints (12 bytes, the hkArray header).
    struct hkBpAxis : hkArray<hkBpEndPoint> {
        hkBpAxis();
        // BYTES: the axis destructor (run by __destroy_arr) has no deleting branch, as with an empty class delete.
        static void operator delete(void*) {}

        // Lower bound: first endpoint in [begin, end) whose key is not below value.
        const hkBpEndPoint* find(const hkBpEndPoint* begin, const hkBpEndPoint* end, u16 value) const;

        // Inserts two endpoints (minKey, maxKey), each tagged with id, keeping the axis sorted.
        // minIndex and maxIndex receive the final endpoint indices.
        void insert(hkBpNode* node, int id, u16 minKey, u16 maxKey, u16& minIndex, u16& maxIndex);
        void remove(int minIndex, int maxIndex);
    };

    u8 unk14[0x2C];     // 0x14
    float m_offset[3];  // 0x40 HYPOTHESIS: per-axis offset subtracted after dequantization (x, y, z)
    float unk4C;        // 0x4C HYPOTHESIS: negated for the w components of the AABB
    u8 unk50[0x10];     // 0x50
    float m_scale[3];   // 0x60 HYPOTHESIS: quantization scale per axis, dequantization uses 1/scale
    u8 unk6C[4];        // 0x6C
    hkArray<hkBpNode> m_nodes; // 0x70; the count includes the sentinel
    hkBpAxis m_axes[3]; // 0x7C (x, y, z)
    int m_numMarkers;   // 0xA0 (1 << m_markerBits) - 1, from the constructor
    int m_markerBits;   // 0xA4 bits selecting a check-marker interval from an X key
    hkBpCheckMarker* m_markers; // 0xA8
    u32 unkAC;                  // 0xAC HYPOTHESIS: unknown or padding (hk3AxisSweep16CreateBroadPhase allocates 0xB0)

    // Map name hk3AxisSweep::hk3AxisSweep (not written yet). HYPOTHESIS: parameter names (the world extents and the
    // number of check markers).
    hk3AxisSweep(const hkVector4& worldMin, const hkVector4& worldMax, int numMarkers);
    ~hk3AxisSweep();

    // Existing node indices shift when the new pair of endpoints is inserted or removed.
    void updateNodesAfterInsert(hkBpNode* nodes, int count, const hkBpNode& inserted);
    void updateNodesAfterDelete(hkBpNode* nodes, int count, const hkBpNode& removed);
    void setBitsBasedOnXInterval(int count, int minKey, const hkBpNode& node, u32 id, u32* bits);
    void reQuerySingleObject(const hkBroadPhaseHandle* handle, hkArray<hkBroadPhaseHandlePair>& pairs);

    int getNumObjects() const;
    void getAabb(const hkBroadPhaseHandle* handle, hkAabb& aabb) const;
    void getAllAabbs(hkArray<hkAabb>& aabbs) const;
    int getAabbCacheSize() const;
    void getAabbFromNode(const hkBpNode* node, hkAabb& aabb) const;

    static void beginOverlap(const hkBpNode* a, const hkBpNode* b, hkArray<hkBroadPhaseHandlePair>& pairs);
    static void endOverlap(const hkBpNode* a, const hkBpNode* b, hkArray<hkBroadPhaseHandlePair>& pairs);
    static void beginOverlapCheckMarker(hkBpCheckMarker* markers, const hkBpNode* a, u32 value, const hkBpNode* b, hkArray<hkBroadPhaseHandlePair>& pairs);
    static void endOverlapCheckMarker(hkBpCheckMarker* markers, const hkBpNode* a, u32 value, const hkBpNode* b, hkArray<hkBroadPhaseHandlePair>& pairs);
};

static_assert(sizeof(hk3AxisSweep) == 0xB0, "Broad-phase object size");
static_assert(sizeof(hk3AxisSweep::hkBpNode) == 0x10, "Broad-phase node layout");
static_assert(sizeof(hk3AxisSweep::hkBpEndPoint) == 4, "Broad-phase endpoint layout");
static_assert(sizeof(hk3AxisSweep::hkBpCheckMarker) == 0x10, "Broad-phase marker layout");
static_assert(sizeof(hk3AxisSweep::hkBpAxis) == 0xC, "Broad-phase axis layout");
