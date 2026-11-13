// Havok translation unit hkBoxBoxAgent3.o (main.dol 0x802FB244-0x802FB894).
// Functions in address order (method names from the Havok TU map):
//   0x802FB244   164  registerAgent3   [map: hkBoxBoxAgent3__registerAgent3]
//   0x802FB2E8    60  create   [map: hkBoxBoxAgent3__create]
//   0x802FB324   128  cleanup   [map: hkBoxBoxAgent3__cleanup]
//   0x802FB3A4   116  removePoint   [map: hkBoxBoxAgent3__removePoint]
//   0x802FB418    68  commitPotential   [map: hkBoxBoxAgent3__commitPotential]
//   0x802FB45C     4  createZombie   [map: hkBoxBoxAgent3__createZombie]
//   0x802FB460     4  destroy   [map: hkBoxBoxAgent3__destroy]
//   0x802FB464  1072  process   [map: hkBoxBoxAgent3__process]

#include <havok/hkBoxBoxAgent3.h>
#include <havok/hkBoxBoxManifold.h>
#include <havok/hkAgent1nTrack.h>
#include <havok/hkCollisionAgent.h>
#include <havok/hkCollisionDispatcher.h>
#include <havok/hkShapeType.h>
#include <havok/hkBoxBoxCollisionDetection.h>
#include <havok/hkBoxShape.h>
#include <havok/hkMonitorStream.h>

// Box x box only, not predictive, no separating-normal function.
void hkBoxBoxAgent3::registerAgent3(hkCollisionDispatcher* dispatcher) {
    hkAgent3Funcs f;
    f.m_createFunc = (hkAgentFunc)create;
    f.m_processFunc = (hkAgentFunc)process;
    f.m_sepNormalFunc = 0;
    f.m_cleanupFunc = (hkAgentFunc)cleanup;
    f.m_removePointFunc = (hkAgentFunc)removePoint;
    f.m_commitPotentialFunc = (hkAgentFunc)commitPotential;
    f.m_createZombieFunc = (hkAgentFunc)createZombie;
    f.m_updateFilterFunc = 0;
    f.m_destroyFunc = (hkAgentFunc)destroy;
    f.m_isPredictive = false;
    dispatcher->registerAgent3(f, HK_SHAPE_BOX, HK_SHAPE_BOX);
}

void* hkBoxBoxAgent3::create(const hkAgent3Input& input, hkAgentEntry* entry, void* agentData) {
    new (agentData) hkBoxBoxManifold();
    return hkAddByteOffset(agentData, 0x50);
}

void* hkBoxBoxAgent3::cleanup(hkAgentEntry* entry, void* agentData, hkContactMgr* contactMgr) {
    hkBoxBoxManifold* manifold = (hkBoxBoxManifold*)agentData;
    for (int i = 0; i < manifold->m_numPoints; i++) {
        if (manifold->m_points[i].unk2 != 0xFFFF) {
            contactMgr->removeContactPoint(manifold->m_points[i].unk2);
        }
    }
    manifold->m_numPoints = 0;
    entry->m_numContactPoints = 0;
    return hkAddByteOffset(agentData, 0x50);
}

void hkBoxBoxAgent3::removePoint(hkAgentEntry* entry, void* agentData, u16 key) {
    hkBoxBoxManifold* manifold = (hkBoxBoxManifold*)agentData;
    for (int i = 0; i < manifold->m_numPoints; i++) {
        if (key == manifold->m_points[i].unk2) {
            manifold->removePoint(i);
            entry->m_numContactPoints--;
            return;
        }
    }
}

void hkBoxBoxAgent3::commitPotential(hkAgentEntry* entry, void* agentData, u16 key) {
    hkBoxBoxManifold* manifold = (hkBoxBoxManifold*)agentData;
    for (int i = 0; i < manifold->m_numPoints; i++) {
        if (manifold->m_points[i].unk2 == 0xFFFF) {
            manifold->m_points[i].unk2 = key;
            return;
        }
    }
}

void hkBoxBoxAgent3::createZombie(hkAgentEntry* entry, void* agentData, u16 key) {}

void hkBoxBoxAgent3::destroy(hkAgentEntry* entry, void* agentData, hkContactMgr* contactMgr) {
    cleanup(entry, agentData, contactMgr);
}

void* hkBoxBoxAgent3::process(const hkAgent3ProcessInput& input, hkAgentEntry* entry, void* agentData,
                              hkVector4* separatingNormal, hkProcessCollisionOutput& result) {
    HK_TIMER_BEGIN("BoxBox3");
    hkBoxBoxManifold* manifold = (hkBoxBoxManifold*)agentData;
    const hkBoxShape* boxA = (const hkBoxShape*)input.m_bodyA->getShape();
    const hkBoxShape* boxB = (const hkBoxShape*)input.m_bodyB->getShape();
    hkVector4 rA4;
    rA4.setAll(boxA->m_radius);
    hkVector4 rA;
    rA.setAdd4(boxA->m_halfExtents, rA4);
    hkVector4 rB4;
    rB4.setAll(boxB->m_radius);
    hkVector4 rB;
    rB.setAdd4(boxB->m_halfExtents, rB4);
    hkBoxBoxCollisionDetection detector(*input.m_bodyA, *input.m_bodyB, input.m_input, input.m_contactMgr, &result,
                                        input.m_aTb, input.m_bodyA->getTransform(), rA, input.m_bodyB->getTransform(),
                                        rB, input.m_input->getTolerance());
    detector.calcManifold(*manifold);
    entry->m_numContactPoints = manifold->m_numPoints;
    HK_TIMER_END();
    return hkAddByteOffset(agentData, 0x50);
}
