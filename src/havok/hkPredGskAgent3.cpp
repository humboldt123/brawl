// Havok translation unit hkPredGskAgent3.o (main.dol 0x803007FC-0x803020E0).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x803007FC   192  registerAgent3   [map: hkPredGskAgent3__registerAgent3]
//   0x803008BC   188  create   [map: hkPredGskAgent3__create]
//   0x80300978    36  getTotalSizeInBytes   [map: hkGskManifold__getTotalSizeInBytes]
//   0x8030099C     8  hkAddByteOffset<v>   [map: hkPredGskAgent3__hkAddByteOffset_v_]
//   0x803009A4   396  sepNormal   [map: hkPredGskAgent3__sepNormal]
//   0x80300B30   124  cleanup   [map: hkPredGskAgent3__cleanup]
//   0x80300BAC    60  removePoint   [map: hkPredGskAgent3__removePoint]
//   0x80300BE8    72  commitPotential   [map: hkPredGskAgent3__commitPotential]
//   0x80300C30    60  createZombie   [map: hkPredGskAgent3__createZombie]
//   0x80300C6C    16  destroy   [map: hkPredGskAgent3__destroy]
//   0x80300C7C  1128  process   [map: hkPredGskAgent3__process]
//   0x803010E4    24  getRootCollidable   [map: hkCdBody__getRootCollidable]
//   0x803010FC     4  __ct   [map: hkGskManifoldWork____ct]
//   0x80301100  1656  hkGskManifold_init   [map: hkPredGskAgent3__hkGskManifold_init]
//   0x80301778     8  getTolerance   [map: hkCollisionInput__getTolerance]
//   0x80301780  2400  hkGskAgentUtil_processCollisionNoTim   [map: hkAgent3Input__hkGskAgentUtil_processCollisionNoTim]

#include <havok/hkPredGskAgent3.h>
#include <havok/hkGskManifoldUtil.h>


void* hkPredGskAgent3::cleanup(hkGskManifold* dst, hkPredGskAgent3* agent, void* arg) {
    hkGskManifold* manifold = &agent->m_manifold;
    hkGskManifoldUtil::hkGskManifold_cleanup(manifold, arg);
    dst->m_countC = manifold->m_countC;
    u8 a = manifold->m_countA;
    u8 b = manifold->m_countB;
    u8 c = manifold->m_countC;
    u32 size = ((a + b) << 1) + (c << 3);
    return (u8*)agent + ((size + 0x1f) & ~0xf);
}

void hkPredGskAgent3::destroy(void* ctx, hkPredGskAgent3* agent, void* arg) {
    hkGskManifoldUtil::hkGskManifold_cleanup(&agent->m_manifold, arg);
}

void hkPredGskAgent3::removePoint(void* ctx, hkPredGskAgent3* agent, u16 key) {
    hkGskManifold* manifold = &agent->m_manifold;
    u8* p = (u8*)manifold;
    for (int i = 0; i < manifold->m_countC; i++) {
        if (key == *(u16*)(p + 6)) {
            hkGskManifoldUtil::hkGskManifold_removePoint(manifold, i);
            return;
        }
        p += 8;
    }
}

void hkPredGskAgent3::commitPotential(void* ctx, hkPredGskAgent3* agent, u16 key) {
    hkGskManifold* manifold = &agent->m_manifold;
    u8* p = (u8*)manifold;
    for (int i = 0; i < manifold->m_countC; i++) {
        if (*(u16*)(p + 6) == 0xFFFF) {
            *(u16*)((u8*)manifold + i * 8 + 6) = key;
            return;
        }
        p += 8;
    }
}

void hkPredGskAgent3::createZombie(void* ctx, hkPredGskAgent3* agent, u16 key) {
    hkGskManifold* manifold = &agent->m_manifold;
    hkGskManifoldEntry* entry = manifold->getEntries();
    for (int i = 0; i < manifold->m_countC; i++) {
        if (entry->key == key) {
            entry->unk0 = 0;
            entry->unk1 = 0;
            return;
        }
        entry++;
    }
}

u32 hkGskManifold::getTotalSizeInBytes() const {
    u8 a = m_countA;
    u8 b = m_countB;
    u8 c = m_countC;
    return ((a + b) << 1) + (c << 3) + 4;
}
