// Havok translation unit hk3AxisSweep.o (main.dol 0x8030880C-0x80312F7C).
// Partially recovered. Original function inventory (map names need independent verification):
//   0x8030880C   136  beginOverlap   [map: hk3AxisSweep__beginOverlap]
//   0x80308894   136  endOverlap   [map: hk3AxisSweep__endOverlap]
//   0x8030891C   220  beginOverlapCheckMarker   [map: hk3AxisSweep__beginOverlapCheckMarker]
//   0x803089F8   256  endOverlapCheckMarker   [map: hk3AxisSweep__endOverlapCheckMarker]
//   0x80308AF8  3676  updateAabbs   [map: hk3AxisSweep__updateAabbs]
//   0x80309954  2852  __ct   [map: hk3AxisSweep____ct]
//   0x8030A478    24  __ct   [map: hk3AxisSweep8hkBpAxisFv____ct]
//   0x8030A578   324  __dt   [map: hk3AxisSweep____dt]
//   0x8030A6BC    32  __dl   [map: hkBroadPhase____dl]
//   0x8030A6DC  1328  mergeBatch   [map: hk3AxisSweep8hkBpAxisFPQ212hk3AxisSweep8hkBpNodeiiiPQ212hk3AxisSweep12hkBpEndPoint__mergeBatch]
//   0x8030AC0C   236  removeBatch   [map: hk3AxisSweep8hkBpAxisFPQ212hk3AxisSweep8hkBpNodeiRC15hkFixedArray_i___removeBatch]
//   0x8030ACF8   268  insert   [map: hk3AxisSweep8hkBpAxisFPQ212hk3AxisSweep8hkBpNodeiUsUsRUsRUs__insert]
//   0x8030AE04   552  remove   [map: hk3AxisSweep8hkBpAxisFii__remove]
//   0x8030B02C   428  updateNodesAfterInsert   [map: hk3AxisSweep__updateNodesAfterInsert]
//   0x8030B1D8   352  updateNodesAfterDelete   [map: hk3AxisSweep__updateNodesAfterDelete]
//   0x8030B338   900  setBitsBasedOnXInterval   [map: hk3AxisSweep__setBitsBasedOnXInterval]
//   0x8030B6BC   456  reQuerySingleObject   [map: hk3AxisSweep__reQuerySingleObject]
//   0x8030B884  1844  addObject   [map: hk3AxisSweep__addObject]
//   0x8030BFB8  1180  removeObject   [map: hk3AxisSweep__removeObject]
//   0x8030C454  2608  addObjectBatch   [map: hk3AxisSweep__addObjectBatch]
//   0x8030CE84    44  quickSort<Q212hk3AxisSweep12hkBpEndPoint>   [map: hkAlgorithm__quickSort_Q212hk3AxisSweep12hkBpEndPoint_]
//   0x8030CEB0    60  quickSort<Q212hk3AxisSweep12hkBpEndPoint,Q211hkAlgorithm3   [map: hkAlgorithm__quickSort_Q212hk3AxisSweep12hkBpEndPoint_Q211hkAlgorithm36less_Q212hk3AxisSweep12hkBpEndPoint__]
//   0x8030CEEC   344  quickSortRecursive<Q212hk3AxisSweep12hkBpEndPoint,Q211hkA   [map: hkAlgorithm__quickSortRecursive_Q212hk3AxisSweep12hkBpEndPoint_Q211hkAlgorithm36less_Q212hk3AxisSweep12hkBpEndPoi___]
//   0x8030D044  1624  removeObjectBatch   [map: hk3AxisSweep__removeObjectBatch]
//   0x8030D69C    12  getNumObjects   [map: hk3AxisSweep__getNumObjects]
//   0x8030D6A8   176  getAllAabbs   [map: hk3AxisSweep__getAllAabbs]
//   0x8030D758    20  getAabb   [map: hk3AxisSweep__getAabb]
//   0x8030D76C    92  find   [map: hk3AxisSweep8hkBpAxisCFPCQ212hk3AxisSweep12hkBpEndPointPCQ212hk3AxisSweep12hkBpEndPointUs__find]
//   0x8030D7C8  1316  queryBatchAabbSub   [map: hk3AxisSweep__queryBatchAabbSub]
//   0x8030DCEC  3484  querySingleAabb   [map: hk3AxisSweep__querySingleAabb]
//   0x8030EA88    28  getAabbCacheSize   [map: hk3AxisSweep__getAabbCacheSize]
//   0x8030EAA4  3688  calcAabbCache   [map: hk3AxisSweep__calcAabbCache]
//   0x8030FA54   332  calcAabbCache   [map: hk3AxisSweep__calcAabbCache1]
//   0x8030FBA0   820  calcAabbCacheInternal   [map: hk3AxisSweep__calcAabbCacheInternal]
//   0x8030FED4  1248  defragment   [map: hk3AxisSweep__defragment]
//   0x803103B4    44  quickSort<Q212hk3AxisSweep12ValueIntPair>   [map: hkAlgorithm__quickSort_Q212hk3AxisSweep12ValueIntPair_]
//   0x803103E0    60  quickSort<Q212hk3AxisSweep12ValueIntPair,Q211hkAlgorithm3   [map: hkAlgorithm__quickSort_Q212hk3AxisSweep12ValueIntPair_Q211hkAlgorithm36less_Q212hk3AxisSweep12ValueIntPair__]
//   0x8031041C   344  quickSortRecursive<Q212hk3AxisSweep12ValueIntPair,Q211hkA   [map: hkAlgorithm__quickSortRecursive_Q212hk3AxisSweep12ValueIntPair_Q211hkAlgorithm36less_Q212hk3AxisSweep12ValueIntPa___]
//   0x80310694  3864  castRay   [map: hk3AxisSweep__castRay]
//   0x803116CC  4316  castAabb   [map: hk3AxisSweep__castAabb]
//   0x803127A8   376  getAabbFromNode   [map: hk3AxisSweep__getAabbFromNode]
//   0x80312920   532  calcStatistics   [map: hk3AxisSweep__calcStatistics]
//   0x80312B34   564  shiftAllObjects   [map: hk3AxisSweep__shiftAllObjects]
//   0x80312D68   332  shiftBroadPhase   [map: hk3AxisSweep__shiftBroadPhase]
//   0x80312EB4   140  hk3AxisSweep16CreateBroadPhase   [map: hkVector4__hk3AxisSweep16CreateBroadPhase]
//   0x80312F40    60  __sinit_\hk3AxisSweep_cpp   [map: hk3AxisSweepcpp____sinit_]

#include <havok/hk3AxisSweep.h>

void hk3AxisSweep::beginOverlap(const hkBpNode* a, const hkBpNode* b, hkArray<hkBroadPhaseHandlePair>& pairs) {
    hkBroadPhaseHandlePair& pair = pairs.expandOne();
    pair.m_a = a->m_handle;
    pair.m_b = b->m_handle;
}

void hk3AxisSweep::endOverlap(const hkBpNode* a, const hkBpNode* b, hkArray<hkBroadPhaseHandlePair>& pairs) {
    hkBroadPhaseHandlePair& pair = pairs.expandOne();
    pair.m_a = a->m_handle;
    pair.m_b = b->m_handle;
}

// Index of the first entry equal to value, or -1.
static int findCheckMarkerEntry(const hkArray<u16>& list, u16 value) {
    for (int i = 0; i < list.m_size; i++) {
        if (((u16*)list.m_data)[i] == value) {
            return i;
        }
    }
    return -1;
}

void hk3AxisSweep::beginOverlapCheckMarker(hkBpCheckMarker* markers, const hkBpNode* a, u32 value, const hkBpNode* b, hkArray<hkBroadPhaseHandlePair>& pairs) {
    if (((u32)b->m_handle & 1) == 0) {
        beginOverlap(a, b, pairs);
    } else {
        u16 key = (u16)value;
        hkBpCheckMarker* marker = (hkBpCheckMarker*)((u8*)markers + ((u32)b->m_handle & ~1));
        hkArray<u16>& list = marker->m_list;
        if (list.m_size == (list.m_capacityAndFlags & 0x3FFFFFFF)) {
            hkArrayUtil::_reserveMore(&list, sizeof(u16));
        }
        ((u16*)list.m_data)[list.m_size++] = key;
    }
}

void hk3AxisSweep::endOverlapCheckMarker(hkBpCheckMarker* markers, const hkBpNode* a, u32 value, const hkBpNode* b, hkArray<hkBroadPhaseHandlePair>& pairs) {
    if (((u32)b->m_handle & 1) == 0) {
        endOverlap(a, b, pairs);
    } else {
        hkBpCheckMarker* marker = (hkBpCheckMarker*)((u8*)markers + ((u32)b->m_handle & ~1));
        int index = findCheckMarkerEntry(marker->m_list, (u16)value);
        marker->m_list.removeAt(index);
    }
}

hk3AxisSweep::hkBpAxis::hkBpAxis() {
    m_data = 0;
    m_size = 0;
    m_capacityAndFlags = DONT_DEALLOCATE_FLAG;
}

const hk3AxisSweep::hkBpEndPoint* hk3AxisSweep::hkBpAxis::find(const hkBpEndPoint* begin, const hkBpEndPoint* end, u16 value) const {
    const hkBpEndPoint* lo = begin;
    const hkBpEndPoint* hi = end;
    while (hi - lo > 16) {
        const hkBpEndPoint* mid = lo + ((unsigned)(hi - lo) >> 1);
        if (mid->m_value < value) {
            lo = mid;
        }
        else {
            hi = mid;
        }
    }
    while (lo->m_value < value) {
        lo++;
    }
    return lo;
}

void hk3AxisSweep::hkBpAxis::insert(hkBpNode* node, int id, u16 minKey, u16 maxKey, u16& minIndex, u16& maxIndex) {
    int newSize = m_size + 2;
    int cap = getCapacity();
    if (cap < newSize) {
        int newCap = (newSize < cap * 2) ? cap * 2 : newSize;
        hkArrayUtil::_reserve(this, newCap, sizeof(hkBpEndPoint));
    }
    m_size = newSize;
    hkBpEndPoint* p = begin() + (newSize - 3);
    while (maxKey <= p->m_value) {
        p[2] = *p;
        p--;
    }
    p[2].m_nodeId = id;
    p[2].m_value = maxKey;
    maxIndex = (p - begin()) + 2;
    while (minKey < p->m_value) {
        p[1] = *p;
        p--;
    }
    p[1].m_value = minKey;
    p[1].m_nodeId = id;
    minIndex = (p - begin()) + 1;
}

int hk3AxisSweep::getNumObjects() const {
    return m_numNodes - 1;
}

void hk3AxisSweep::getAabb(const hkBroadPhaseHandle* handle, hkAabb& aabb) const {
    getAabbFromNode(&m_nodes[handle->m_id], aabb);
}

int hk3AxisSweep::getAabbCacheSize() const {
    return (m_numNodes - m_numMarkers) * 2 * sizeof(hkBpAxis) + 0x24;
}

void hk3AxisSweep::getAllAabbs(hkArray<hkAabb>& aabbs) const {
    if (aabbs.getCapacity() < m_numNodes - m_numMarkers) {
        hkArrayUtil::_reserve(&aabbs, m_numNodes - m_numMarkers, sizeof(hkAabb));
    }
    aabbs.m_size = m_numNodes - m_numMarkers;
    int out = 0;
    for (int i = 0; i < m_numNodes; i++) {
        const hkBpNode* node = &m_nodes[i];
        if (((u32)node->m_handle & 1) == 0) {
            getAabbFromNode(node, aabbs.begin()[out]);
            out++;
        }
    }
}

#pragma dont_inline on
void hk3AxisSweep::getAabbFromNode(const hkBpNode* node, hkAabb& aabb) const {
    const hkBpEndPoint* xEnds = (const hkBpEndPoint*)m_axes[0].m_data;
    const hkBpEndPoint* yEnds = (const hkBpEndPoint*)m_axes[1].m_data;
    const hkBpEndPoint* zEnds = (const hkBpEndPoint*)m_axes[2].m_data;
    float invX = 1.0f / m_scale[0];
    float invY = 1.0f / m_scale[1];
    float invZ = 1.0f / m_scale[2];
    aabb.m_min.x = (float)xEnds[node->m_index[4]].m_value * invX - m_offset[0];
    aabb.m_min.y = (float)yEnds[node->m_index[0]].m_value * invY - m_offset[1];
    aabb.m_min.z = (float)zEnds[node->m_index[1]].m_value * invZ - m_offset[2];
    aabb.m_min.w = 0.0f - unk4C;
    aabb.m_max.x = (float)xEnds[node->m_index[5]].m_value * invX - m_offset[0];
    aabb.m_max.y = (float)yEnds[node->m_index[2]].m_value * invY - m_offset[1];
    aabb.m_max.z = (float)zEnds[node->m_index[3]].m_value * invZ - m_offset[2];
    aabb.m_max.w = 0.0f - unk4C;
}
#pragma dont_inline reset

// Removing the minimum shifts the endpoints between min and max by one;
// the remainder shifts by two once both endpoints have been removed.
void hk3AxisSweep::hkBpAxis::remove(int minIndex, int maxIndex) {
    hkBpEndPoint* cursor = begin() + minIndex;
    hkBpEndPoint* beforeMax = begin() + maxIndex - 1;
    for (; cursor < beforeMax; ++cursor) {
        *cursor = cursor[1];
    }
    m_size -= 2;
    hkBpEndPoint* end = begin() + m_size;
    for (; cursor < end; ++cursor) {
        *cursor = cursor[2];
    }
}

void hk3AxisSweep::updateNodesAfterInsert(hkBpNode* nodes, int count, const hkBpNode& inserted) {
    // Compare against positions before insertion: min shifted by one, max by two.
    // Snapshot first: inserted can refer to an entry in the same node array.
    u16 xMin = inserted.m_index[4] - 1;
    u16 yMin = inserted.m_index[0] - 1;
    u16 zMin = inserted.m_index[1] - 1;
    u16 xMax = inserted.m_index[5] - 2;
    u16 yMax = inserted.m_index[2] - 2;
    u16 zMax = inserted.m_index[3] - 2;
    hkBpNode* end = nodes + count;
    for (; nodes < end; ++nodes) {
        nodes->m_index[4] += (u16)((nodes->m_index[4] > xMin) + (nodes->m_index[4] > xMax));
        nodes->m_index[0] += (u16)((nodes->m_index[0] > yMin) + (nodes->m_index[0] > yMax));
        nodes->m_index[5] += (u16)((nodes->m_index[5] > xMin) + (nodes->m_index[5] > xMax));
        nodes->m_index[2] += (u16)((nodes->m_index[2] > yMin) + (nodes->m_index[2] > yMax));
        nodes->m_index[1] += (u16)((nodes->m_index[1] > zMin) + (nodes->m_index[1] > zMax));
        nodes->m_index[3] += (u16)((nodes->m_index[3] > zMin) + (nodes->m_index[3] > zMax));
    }
}

void hk3AxisSweep::updateNodesAfterDelete(hkBpNode* nodes, int count, const hkBpNode& removed) {
    u16 xMin = removed.m_index[4];
    u16 yMin = removed.m_index[0];
    u16 zMin = removed.m_index[1];
    u16 xMax = removed.m_index[5];
    u16 yMax = removed.m_index[2];
    u16 zMax = removed.m_index[3];
    hkBpNode* end = nodes + count;
    for (; nodes != end; ++nodes) {
        nodes->m_index[4] -= (u16)((nodes->m_index[4] > xMin) + (nodes->m_index[4] > xMax));
        nodes->m_index[5] -= (u16)((nodes->m_index[5] > xMin) + (nodes->m_index[5] > xMax));
        nodes->m_index[0] -= (u16)((nodes->m_index[0] > yMin) + (nodes->m_index[0] > yMax));
        nodes->m_index[2] -= (u16)((nodes->m_index[2] > yMin) + (nodes->m_index[2] > yMax));
        nodes->m_index[1] -= (u16)((nodes->m_index[1] > zMin) + (nodes->m_index[1] > zMax));
        nodes->m_index[3] -= (u16)((nodes->m_index[3] > zMin) + (nodes->m_index[3] > zMax));
    }
}

// Build candidates on X before testing their Y/Z intervals. Crossing either
// endpoint toggles a node's bit; a minimum inside our interval adds that node.
void hk3AxisSweep::setBitsBasedOnXInterval(int count, int minKey, const hkBpNode& node,
                                         u32 id, u32* bits) {
    int minIndex = node.m_index[4];
    int maxIndex = node.m_index[5];
    // The native implementation clears whole 128-bit vectors, including padding.
    hkVector4 zero;
    zero.setZero4();
    hkVector4* clear = (hkVector4*)bits;
    int vectors = (count >> 7) + 1;
    for (int i = 0; i < vectors; ++i) {
        clear[i] = zero;
    }
    const hkBpEndPoint* cursor = m_axes[0].begin() + 1;
    if (m_numMarkers != 0 && (minKey >>= 16 - m_markerBits) > 0) {
        const hkBpCheckMarker& marker = m_markers[minKey - 1];
        int markerId = marker.m_nodeId;
        bits[markerId >> 5] ^= 1u << (markerId & 31);
        const u16* entry = marker.m_list.begin();
        for (int i = marker.m_list.m_size - 1; i >= 0; --i, ++entry) {
            int otherId = *entry;
            if (otherId != id) bits[otherId >> 5] ^= 1u << (otherId & 31);
        }
        const hkBpNode& markerNode = m_nodes[marker.m_nodeId];
        cursor = m_axes[0].begin() + (markerNode.m_index[4] + 1);
        const hkBpEndPoint* end = m_axes[0].begin() + markerNode.m_index[5];
        for (; cursor < end; ++cursor) {
            if ((cursor->m_value & 1) == 0) {
                int otherId = cursor->m_nodeId;
                bits[otherId >> 5] &= ~(1u << (otherId & 31));
            }
        }
        cursor = m_axes[0].begin() + (markerNode.m_index[4] + 1);
    }
    const hkBpEndPoint* min = m_axes[0].begin() + minIndex;
    for (; cursor < min; ++cursor) {
        int otherId = cursor->m_nodeId;
        bits[otherId >> 5] ^= 1u << (otherId & 31);
    }
    ++cursor;
    const hkBpEndPoint* max = m_axes[0].begin() + maxIndex;
    for (; cursor < max; ++cursor) {
        if ((cursor->m_value & 1) == 0) {
            int otherId = cursor->m_nodeId;
            bits[otherId >> 5] ^= 1u << (otherId & 31);
        }
    }
}

void hk3AxisSweep::reQuerySingleObject(const hkBroadPhaseHandle* handle,
                                      hkArray<hkBroadPhaseHandlePair>& pairs) {
    u32* bits;
    const u32* word;
    const hkBpNode* query;
    const hkBpNode* block;
    int count = m_numNodes;
    bits = (u32*)hkThreadMemory::s_instance->allocateStack(((count >> 5) + 8) * sizeof(u32));
    word = bits;
    query = &m_nodes[handle->m_id];
    setBitsBasedOnXInterval(count, m_axes[0][query->m_index[4]].m_value,
                           *query, (u16)handle->m_id, bits);
    hkBroadPhaseHandlePair pair;
    pair.m_a = (hkTypedBroadPhaseHandle*)handle;
    const u32* end = bits + (m_numNodes >> 5) + 1;
    block = m_nodes;
    for (; word < end; block += 32, ++word) {
        const hkBpNode* candidate = block;
        u32 candidates = *word;
        while (candidates != 0) {
            if ((candidates & 0xFF) == 0) {
                candidates >>= 8;
                candidate += 8;
                continue;
            }
            if (candidates & 1) {
                // Two packed 16-bit comparisons reject disjoint Y/Z intervals.
                u32 candidateMin = candidate->m_minYZ;
                u32 queryMax = query->m_maxYZ;
                u32 queryMin = query->m_minYZ;
                u32 candidateMax = candidate->m_maxYZ;
                if ((((candidateMax - queryMin) | (queryMax - candidateMin)) & 0x80008000u) == 0 &&
                    ((u32)candidate->m_handle & 1) == 0) {
                    pair.m_b = candidate->m_handle;
                    pairs.pushBack(pair);
                }
            }
            candidates >>= 1;
            ++candidate;
        }
    }
    hkThreadMemory::s_instance->deallocateStack(bits);
}

// Factory of the 3-axis sweep broadphase (map name hk3AxisSweep16CreateBroadPhase).
hkBroadPhase* hk3AxisSweep16CreateBroadPhase(const hkVector4& worldMin, const hkVector4& worldMax, int numMarkers) {
    return new hk3AxisSweep(worldMin, worldMax, numMarkers);
}

// Six words filled by the __sinit (lbl_805332B0, 16-aligned .bss); read by three functions of the unit through a
// base pointer. HYPOTHESIS: a constructed table of per-axis offsets; the meaning of the values is not recovered.
struct hk3AxisSweepStaticTable {
    int m_values[6];
    hk3AxisSweepStaticTable() {
        m_values[0] = 8;
        m_values[1] = 10;
        m_values[2] = 0;
        m_values[3] = 4;
        m_values[4] = 2;
        m_values[5] = 6;
    }
};
static hk3AxisSweepStaticTable s_hk3AxisSweepStaticTable;

// Weak inline copies emitted in this unit; extern "C" placeholders under their fn_ names until the
// instantiating callers are written (shape from https://github.com/jj-link/brawl src/havok/fn_<addr>.cpp).
// hkAlgorithm::quickSort<hk3AxisSweep::hkBpEndPoint> / <hk3AxisSweep::ValueIntPair> and their less<> overloads
extern "C" void fn_8030CEEC(void* a, int lo, int hi, unsigned char* cmp);
extern "C" void fn_8031041C(void* a, int lo, int hi, unsigned char* cmp);
extern "C" void fn_8030CEB0(void* a, int n, unsigned char* cmp);
extern "C" void fn_803103E0(void* a, int n, unsigned char* cmp);
// TODO: cleanup (shape from jj-link/brawl fn_8030CE84)
extern "C" void fn_8030CE84(void* a, int n) {
    unsigned char c = 0;
    fn_8030CEB0(a, n, &c);
}
#pragma push
#pragma dont_inline on
// TODO: cleanup (shape from jj-link/brawl fn_8030CEB0)
extern "C" void fn_8030CEB0(void* a, int n, unsigned char* cmp) {
    if (n > 1) {
        unsigned char c = *cmp;
        fn_8030CEEC(a, 0, n - 1, &c);
    }
}
#pragma pop
// TODO: cleanup (shape from jj-link/brawl fn_803103B4)
extern "C" void fn_803103B4(void* a, int n) {
    unsigned char c = 0;
    fn_803103E0(a, n, &c);
}
#pragma push
#pragma dont_inline on
// TODO: cleanup (shape from jj-link/brawl fn_803103E0)
extern "C" void fn_803103E0(void* a, int n, unsigned char* cmp) {
    if (n > 1) {
        unsigned char c = *cmp;
        fn_8031041C(a, 0, n - 1, &c);
    }
}
#pragma pop

// fn_8030CEEC: body from https://github.com/jj-link/brawl src/havok/fn_8030CEEC.cpp (namespaced; labels as in our symbols).
#include <types.h>
namespace jj_fn_8030CEEC {
struct Pair { unsigned short key; unsigned short val; };
struct Flag { signed char value; Flag(bool b) : value(b) {} operator bool() const { return value != 0; } };
struct Comp {
    Flag operator()(const Pair& a, const Pair& b) const { return (int)a.key - (int)b.key < 0; }
};
// TODO: cleanup (shape from jj-link/brawl fn_8030CEEC)
extern "C" void fn_8030CEEC(Pair* arr, int lo, int hi, Comp comp) {
top:
    Pair pivot __attribute__((aligned(16)));
    pivot = arr[(unsigned int)(lo + hi) >> 1];
    int j = hi;
    int i = lo;
    do {
        while (comp(arr[i], pivot)) { ++i; }
        while (comp(pivot, arr[j])) { --j; }
        if (j >= i) {
            if (j != i) {
                Pair temp __attribute__((aligned(16)));
                temp = arr[j];
                arr[j] = arr[i];
                arr[i] = temp;
            }
            --j;
            ++i;
        }
    } while (i <= j);
    if (lo < j) {
        fn_8030CEEC(arr, lo, j, comp);
    }
    if (i < hi) { lo = i; goto top; }
}
} // namespace jj_fn_8030CEEC

// fn_8031041C: body from https://github.com/jj-link/brawl src/havok/fn_8031041C.cpp (namespaced; labels as in our symbols).
#include <types.h>
namespace jj_fn_8031041C {
struct Pair { unsigned short key; unsigned short val; };
struct Flag { signed char value; Flag(bool b) : value(b) {} operator bool() const { return value != 0; } };
struct Comp {
    Flag operator()(const Pair& a, const Pair& b) const { return (int)a.key - (int)b.key < 0; }
};

// TODO: cleanup (shape from jj-link/brawl fn_8031041C)
extern "C" void fn_8031041C(Pair* arr, int lo, int hi, Comp comp) {
top:
    Pair pivot __attribute__((aligned(16)));
    pivot = arr[(unsigned int)(lo + hi) >> 1];
    int j = hi;
    int i = lo;
    do {
        while (comp(arr[i], pivot)) { ++i; }
        while (comp(pivot, arr[j])) { --j; }
        if (j >= i) {
            if (j != i) {
                Pair temp __attribute__((aligned(16)));
                temp = arr[j];
                arr[j] = arr[i];
                arr[i] = temp;
            }
            --j;
            ++i;
        }
    } while (i <= j);
    if (lo < j) {
        fn_8031041C(arr, lo, j, comp);
    }
    if (i < hi) { lo = i; goto top; }
}
} // namespace jj_fn_8031041C

// fn_80312920: body from https://github.com/jj-link/brawl src/havok/fn_80312920.cpp (namespaced; callee rebound to our C++ name).
#include <types.h>
namespace jj_fn_80312920 {
#pragma use_lmw_stmw on
extern "C" char lbl_80414360[];
class Object_vptr_0 {
public:
virtual void slot0();
virtual void slot1(const char*, int, void*);
virtual void slot2(const char*, int, void*, u32, u32);
virtual void slot3();
virtual void slot4();
virtual void slot5();
virtual void slot6();
};
class Class_804888F8 {
public:
virtual void fn_8030A578();
virtual void fn_80312920();
virtual void fn_8030B884();
virtual void fn_8030C454();
virtual void fn_8030BFB8();
virtual void fn_8030D044();
virtual void fn_8030D69C();
};
struct Ent { char pad[4]; void* ptr; u32 count; u32 flags; };
struct Self {
char pad0[0x70];
void* p70; u32 c74; u32 f78;
void* p7c; u32 c80; u32 f84;
void* p88; u32 c8c; u32 f90;
void* p94; u32 c98; u32 f9c;
u32 fa0; char padA4[4]; Ent* paa8;
};
inline void start(Self* self, Object_vptr_0* r, char* name, int size) {
r->slot1(name,size,self);
}
// TODO: cleanup (shape from jj-link/brawl fn_80312920)
extern "C" void fn_80312920(Self* self,Object_vptr_0* r) {
char* base=lbl_80414360;
start(self,r,base+0x3D,4);
char* s48=base+0x48;
if (!(self->f78 & 0x80000000)) r->slot2(s48,4,self->p70,self->c74*16,self->f78*16);
char* s4e=lbl_80414360+0x4E;
if (!(self->f84 & 0x80000000)) r->slot2(s4e,4,self->p7c,self->c80*4,self->f84*4);
s4e=lbl_80414360+0x4E;
if (!(self->f90 & 0x80000000)) r->slot2(s4e,4,self->p88,self->c8c*4,self->f90*4);
s4e=lbl_80414360+0x4E;
if (!(self->f9c & 0x80000000)) r->slot2(s4e,4,self->p94,self->c98*4,self->f9c*4);
if(self->fa0!=0) r->slot2(lbl_80414360+0x53,4,self->paa8,self->fa0*16,0);
char* p53;
int off;
int i=0;
if((int)self->fa0>0) {
off=0;
p53=lbl_80414360+0x53;
for(;i<(int)self->fa0;off+=16,i++) {
Ent* e=(Ent*)((char*)self->paa8+off);
if(!(e->flags&0x80000000)) r->slot2(p53,8,e->ptr,e->count*2,(e->flags&0x3FFFFFFF)*2);
}
}
r->slot6();
}
} // namespace jj_fn_80312920
