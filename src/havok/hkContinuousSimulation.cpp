// Havok translation unit hkContinuousSimulation.o (main.dol 0x80325FDC-0x8032BBE8).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x80325FDC   156  __ct   [map: hkContinuousSimulation____ct]
//   0x80326078    32  __dl   [map: hkReferencedObject____dl]
//   0x80326124   208  __dt   [map: hkContinuousSimulation____dt]
//   0x803261F4   736  handleAllToisTill   [map: hkContinuousSimulation__handleAllToisTill]
//   0x803264D4   600  advanceTime   [map: hkContinuousSimulation__advanceTime]
//   0x8032672C   500  reintegrateAndRecollideEntities   [map: hkContinuousSimulation__reintegrateAndRecollideEntities]
//   0x80326920   120  resetCollisionInformationForEntities   [map: hkContinuousSimulation__resetCollisionInformationForEntities]
//   0x80326998     4  assertThereIsNoCollisionInformationForEntities   [map: hkContinuousSimulation__assertThereIsNoCollisionInformationForEntities]
//   0x8032699C   196  removeCollisionInformationForAgent   [map: hkContinuousSimulation__removeCollisionInformationForAgent]
//   0x80326A60     4  assertThereIsNoCollisionInformationForAgent   [map: hkContinuousSimulation__assertThereIsNoCollisionInformationForAgent]
//   0x80326A64    60  warpTime   [map: hkContinuousSimulation__warpTime]
//   0x80326AA0  1100  collideInternal   [map: hkContinuousSimulation__collideInternal]
//   0x80326EEC  2432  collideEntitiesBroadPhaseContinuous   [map: hkContinuousSimulation__collideEntitiesBroadPhaseContinuous]
//   0x8032786C   752  collideIslandNarrowPhaseContinuous   [map: hkContinuousSimulation__collideIslandNarrowPhaseContinuous]
//   0x80327B5C    68  collideEntitiesNarrowPhaseContinuous   [map: hkContinuousSimulation__collideEntitiesNarrowPhaseContinuous]
//   0x80327BA0   316  processAgentCollideContinuous   [map: hkContinuousSimulation__processAgentCollideContinuous]
//   0x80327CDC   272  addToiEvent   [map: hkContinuousSimulation__addToiEvent]
//   0x80327DEC   500  removeToiEventsOfEntities   [map: hkContinuousSimulation__removeToiEventsOfEntities]
//   0x80327FE0   324  removeToiEventsOfEntity   [map: hkContinuousSimulation__removeToiEventsOfEntity]
//   0x80328124  2900  simulateToi   [map: hkContinuousSimulation__simulateToi]
//   0x80328E50  1228  hkLs_doSimpleCollisionResponse   [map: hkWorld__hkLs_doSimpleCollisionResponse]
//   0x8032931C   660  hkLs_updateEntityManifolds   [map: hkEntity__hkLs_updateEntityManifolds]
//   0x803295B0   108  hkLs_areVelocitiesOk   [map: hkConstraintSchemaInfo__hkLs_areVelocitiesOk]
//   0x8032961C  1076  hkLs_toiCheckValidityOfConstraints   [map: hkConstraintSolverResources__hkLs_toiCheckValidityOfConstraints]
//   0x80329A50   976  hkLs_toiActivateEntitiesAndCheckConstraints   [map: hkProcessCollisionInput__hkLs_toiActivateEntitiesAndCheckConstraints]
//   0x80329E20   296  hkLs_toiActivateConstraintsLinkingToFixedAndKeyframedEnti   [map: hkArray_22hkConstraintSchemaInfo___hkLs_toiActivateConstraintsLinkingToFixedAndKeyframedEntities]
//   0x80329F48   192  hkLs_toiActivateConstraintsLinkingActivatedEntities   [map: hkArray_22hkConstraintSchemaInfo___hkLs_toiActivateConstraintsLinkingActivatedEntities]
//   0x8032A008   228  hkLs_toiResetVelocityAccumulatorsForEntities   [map: hkConstraintSolverResources__hkLs_toiResetVelocityAccumulatorsForEntities]
//   0x8032A0EC   320  hkLs_backstepAndFreezeEntireIsland   [map: hkSimulationIsland__hkLs_backstepAndFreezeEntireIsland]
//   0x8032A22C   320  hkLs_restoreTransformOnBodiesWithUpdatedTransform   [map: hkSimulationIsland__hkLs_restoreTransformOnBodiesWithUpdatedTransform]
//   0x8032A36C   392  hkLs_toiCheckFinalValidityOfCriticalConstraints   [map: hkConstraintSolverResources__hkLs_toiCheckFinalValidityOfCriticalConstraints]
//   0x8032A4F4  4468  hkLs_localizedSolveToi   [map: hkToiResources__hkLs_localizedSolveToi]
//   0x8032B8DC   672  insertAt   [map: hkArray_P8hkEntity___insertAt]
//   0x8032BB7C    96  insertAt   [map: hkArray_P8hkEntity___insertAt1]
//   0x8032BBDC    12  __sinit_\hkContinuousSimulation_cpp   [map: hkContinuousSimulationcpp____sinit_]

#include <havok/hkContinuousSimulation.h>
#include <havok/hkDefaultToiResourceMgr.h>
#include <havok/hkWorld.h>

hkContinuousSimulation::hkContinuousSimulation(hkWorld* world) : hkSimulation(world) {
    m_unk0C = 1;
    hkDefaultToiResourceMgr* mgr = new hkDefaultToiResourceMgr();
    m_toiResourceMgr = mgr;
    m_unk38 = 0;
}

hkContinuousSimulation::~hkContinuousSimulation() {
    if (m_toiResourceMgr != 0) {
        delete m_toiResourceMgr;
    }
}

// Debug-only consistency checks: empty in this build.
void hkContinuousSimulation::assertThereIsNoCollisionInformationForEntities(const void* entities) {}

void hkContinuousSimulation::assertThereIsNoCollisionInformationForAgent(const void* agent) {}

// Adds dt to the time of every pending TOI event of the continuous simulation owned by the world.
void hkContinuousSimulation::warpTime(hkReal dt) {
    hkContinuousSimulation* sim = (hkContinuousSimulation*)m_world->m_unk08; // HYPOTHESIS: world field holds the simulation
    for (int i = 0; i < sim->m_toiEvents.m_size; i++) {
        sim->m_toiEvents.begin()[i].unk00 += dt;
    }
}

// Removes every TOI event whose key (0x18) matches the agent key (0x10 of the agent). Walks backwards and
// replaces a removed event with the last one. HYPOTHESIS: agent key is the word at 0x10.
void hkContinuousSimulation::removeCollisionInformationForAgent(const void* agent) {
    for (int i = m_toiEvents.m_size - 1; i >= 0; i--) {
        hkToiEvent* events = m_toiEvents.begin();
        if (events[i].unk18 == ((const u32*)agent)[4]) {
            m_toiEvents.m_size--;
            events[i].copyFrom(events[m_toiEvents.m_size]);
        }
    }
}
