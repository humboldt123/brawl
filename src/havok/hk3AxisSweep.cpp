// Havok translation unit hk3AxisSweep.o (main.dol 0x8030880C-0x80312F7C).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
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

hk3AxisSweep::hkBpAxis::hkBpAxis() {
    unk00 = 0;
    unk04 = 0;
    unk08 = 0x80000000;
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

int hk3AxisSweep::getNumObjects() const {
    return m_numNodes - 1;
}

void hk3AxisSweep::getAabb(const hkBroadPhaseHandle* handle, hkAabb& aabb) const {
    getAabbFromNode(&m_nodes[handle->m_id], aabb);
}

int hk3AxisSweep::getAabbCacheSize() const {
    return (m_numNodes - unkA0) * 2 * sizeof(hkBpAxis) + 0x24;
}

void hk3AxisSweep::getAllAabbs(hkArray<hkAabb>& aabbs) const {
    if (aabbs.getCapacity() < m_numNodes - unkA0) {
        hkArrayUtil::_reserve(&aabbs, m_numNodes - unkA0, sizeof(hkAabb));
    }
    aabbs.m_size = m_numNodes - unkA0;
    int out = 0;
    for (int i = 0; i < m_numNodes; i++) {
        const hkBpNode* node = &m_nodes[i];
        if (((u32)node->m_handle & 1) == 0) {
            getAabbFromNode(node, aabbs.begin()[out]);
            out++;
        }
    }
}
