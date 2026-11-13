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
#include <types.h>
#include <havok/hkCollisionDispatcher.h>
#include <havok/hkAgent1nTrack.h>
#include <havok/hkCollidable.h>
#include <havok/hkCollisionInput.h>
#include <havok/hkProcessCollisionOutput.h>
#include <havok/hkMonitorStream.h>
#include <havok/hkMath.h>

// The agent3 functions of the predictive GSK agent; predictive, with a symmetric version.
void hkPredGskAgent3::registerAgent3(hkCollisionDispatcher* dispatcher, int typeA, int typeB) {
    hkAgent3Funcs f;
    f.m_createFunc = (hkAgentFunc)create;
    f.m_processFunc = (hkAgentFunc)process;
    f.m_sepNormalFunc = (hkAgentFunc)sepNormal;
    f.m_cleanupFunc = (hkAgentFunc)cleanup;
    f.m_removePointFunc = (hkAgentFunc)removePoint;
    f.m_commitPotentialFunc = (hkAgentFunc)commitPotential;
    f.m_createZombieFunc = (hkAgentFunc)createZombie;
    f.m_destroyFunc = (hkAgentFunc)destroy;
    f.m_isPredictive = true;
    dispatcher->registerAgent3(f, typeA, typeB);
}


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
    u8* p;
    hkGskManifold* manifold = &agent->m_manifold;
    int i;
    p = (u8*)manifold;
    for (i = 0; i < manifold->m_countC; i++) {
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

// BYTES: the whole function is compiled without inlining; every accessor is a call to its out-of-line copy.
#pragma push
#pragma dont_inline on
void* hkPredGskAgent3::process(const hkAgent3ProcessInput& input, hkAgentEntry* entry, void* agentData,
                               hkVector4* separatingNormal, hkProcessCollisionOutput& output) {
    HK_TIMER_BEGIN_ACCESSORS("PredGskf3");
    HK_TIMER_BEGIN_LIST_ACCESSORS("intern", "init");

    hkGskCache* gskCache = (hkGskCache*)agentData;
    hkGskManifold* gskManifold = (hkGskManifold*)(gskCache + 1);
    hkReal distAtT1 = input.m_distAtT1;
    {
        const hkCollisionQualityInfo& qi = *input.m_input->m_collisionQualityInfo;
        if (qi.unk10) {
            hkReal radiusA = ((const hkCollidable*)input.m_bodyA->getRootCollidable())->m_allowedPenetrationDepth;
            hkReal radiusB = ((const hkCollidable*)input.m_bodyB->getRootCollidable())->m_allowedPenetrationDepth;
            hkReal allowedPenetrationDepth = hkMath::min2(radiusA, radiusB);
            hkReal distAtT0 = (*separatingNormal)(3);
            hkReal minSeparation = hkMath::min2(qi.unk14[0] * allowedPenetrationDepth,
                                                distAtT0 + qi.unk14[1] * allowedPenetrationDepth);
            if (distAtT1 >= minSeparation) {
                goto QUICK_VERIFY_MANIFOLD;
            }
            hkReal toiSeparation = hkMath::min2(qi.unk14[4] * allowedPenetrationDepth,
                                                distAtT0 + qi.unk14[5] * allowedPenetrationDepth);
            HK_TIMER_SPLIT_LIST_ACCESSORS("toi");
            hk4dGskCollideCalcToi(input, allowedPenetrationDepth, minSeparation, toiSeparation, *gskCache,
                                  *separatingNormal, output);
            goto PROCESS_AT_T1;
        }
    QUICK_VERIFY_MANIFOLD:
        if (distAtT1 > qi.unk0[3]) {
            (*separatingNormal)(3) = distAtT1;
            if (gskManifold->m_countC) {
                HK_TIMER_SPLIT_LIST_ACCESSORS("getPoints");
                hkGskManifoldWork work;
                hkGskManifold_init(*gskManifold, *separatingNormal, *input.m_bodyA, *input.m_bodyB,
                                   input.m_input->getTolerance(), work);
                hkGskManifold_verifyAndGetPoints(*gskManifold, work, 0, output, input.m_contactMgr);
                if (gskManifold->m_countC && output.m_potentialContacts) {
                    *output.m_potentialContacts->m_firstFreeRepresentativeContact++ =
                        output.m_firstFreeContactPoint - gskManifold->m_countC;
                }
            }
            goto END;
        }
    }
PROCESS_AT_T1:
    HK_TIMER_SPLIT_LIST_ACCESSORS("process");
    hkGskAgentUtil_processCollisionNoTim(input, entry, agentData, *gskCache, *gskManifold, *separatingNormal, output);
END:
    entry->m_numContactPoints = gskManifold->m_countC;
    HK_TIMER_END_LIST_ACCESSORS();
    HK_TIMER_END_ACCESSORS();
    return hkAddByteOffset(agentData, (gskManifold->getTotalSizeInBytes() + sizeof(hkGskCache) + 15) & ~15);
}
#pragma pop

u32 hkGskManifold::getTotalSizeInBytes() const {
    u8 a = m_countA;
    u8 b = m_countB;
    u8 c = m_countC;
    return ((a + b) << 1) + (c << 3) + 4;
}


