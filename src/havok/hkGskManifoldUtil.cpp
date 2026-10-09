// Havok translation unit hkGskManifoldUtil.o (main.dol 0x80315780-0x803183A0).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x80315780   396  hkGskManifold_garbageCollect   [map: hkGskManifold__hkGskManifold_garbageCollect]
//   0x8031590C   376  hkGskManifold_removePoint   [map: hkGskManifoldUtil__hkGskManifold_removePoint]
//   0x80315A84   764  hkGskManifold_verifyAndGetPoints   [map: hkGskManifoldUtil__hkGskManifold_verifyAndGetPoints]
//   0x80315D80   208  hkGskManifold_convertPointPoint   [map: hkGskManifoldWork__hkGskManifold_convertPointPoint]
//   0x80315E50   944  hkGskManifold_convertPointEdge   [map: hkGskManifoldWork__hkGskManifold_convertPointEdge]
//   0x80316200  1216  hkGskManifold_convertPointFace   [map: hkGskManifoldWork__hkGskManifold_convertPointFace]
//   0x803166C0   900  hkGskManifold_convertEdgePoint   [map: hkGskManifoldWork__hkGskManifold_convertEdgePoint]
//   0x80316A44  1172  hkGskManifold_convertEdgeEdge   [map: hkGskManifoldWork__hkGskManifold_convertEdgeEdge]
//   0x80316ED8  1352  hkGskManifold_convertFacePoint   [map: hkGskManifoldWork__hkGskManifold_convertFacePoint]
//   0x80317420   548  hkGskManifold_doesPointExistAndResort   [map: hkGskManifoldUtil__hkGskManifold_doesPointExistAndResort]
//   0x80317644  3272  hkGskManifold_addPoint   [map: hkGskManifoldUtil__hkGskManifold_addPoint]

#include <havok/hkGskManifoldUtil.h>

void hkGskManifoldUtil::hkGskManifold_cleanup(hkGskManifold* manifold, void* arg) {
    u8* p = (u8*)manifold;
    for (int i = 0; i < manifold->m_countC; i++) {
        if (*(u16*)(p + 6) != 0xFFFF) {
            ((hkGskPointRemover*)arg)->removePoint(*(u16*)(p + 6));
        }
        p += 8;
    }
    manifold->m_countC = 0;
    manifold->m_countA = 0;
    manifold->m_countB = 0;
}
