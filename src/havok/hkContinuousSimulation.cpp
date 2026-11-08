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
#include <havok/hkWorldCallbackUtil.h>
#include <havok/hkMonitorStream.h>
#include <havok/hkCollisionInput.h>

hkContinuousSimulation::hkContinuousSimulation(hkWorld* world) : hkSimulation(world) {
    m_unk0C = 1;
    hkDefaultToiResourceMgr* mgr = new hkDefaultToiResourceMgr();
    m_toiResourceMgr = mgr;
    m_unk38 = 0;
}

hkContinuousSimulation::~hkContinuousSimulation() {
    delete m_toiResourceMgr;
}

// Handles the pending time-of-impact events in time order, up to minTime: each step takes the earliest event out
// of the list, moves the time to it and simulates it with the world locked. Stops with result 3 when the memory
// system ran out during an event.
// HYPOTHESIS: the meaning of result 3 (the step failed inside the TOI pass); m_unk38 counts the handled events.
int hkContinuousSimulation::handleAllToisTill(hkReal minTime) {
    while (true) {
        hkReal earliestTime = minTime;
        int earliest = -1;
        for (int i = 0; i < m_toiEvents.getSize(); i++) {
            if (m_toiEvents[i].unk00 < earliestTime) {
                earliest = i;
                earliestTime = m_toiEvents[i].unk00;
            }
        }
        if (earliest < 0) {
            return 0;
        }
        hkToiEvent event = m_toiEvents[earliest];
        m_toiEvents.removeAt(earliest);
        m_unk10 = event.unk00;
        hkStepInfo stepInfo(m_unk10, m_unk14);
        m_world->m_dynamicsStepInfo.m_stepInfo = stepInfo;
        m_world->m_collisionInput->m_stepInfo = stepInfo;
        m_world->m_lockCount++;
        simulateToi(m_world, event, m_unk18);
        m_world->unlockAndAttemptToExecutePendingOperations();
        m_unk38++;
        if (hkMemory::getInstance().unk4 == 1) {
            return 3;
        }
    }
}

// Advances the time like hkSimulation::advanceTime, after re-colliding a failed step and handling the pending
// time-of-impact events up to the time to advance to (both inside the "Simulate" timer). A failure in either
// returns its result; a failed TOI pass is also kept as the step result.
int hkContinuousSimulation::advanceTime() {
    {
        HK_TIME_CODE_BLOCK("Simulate");
        if (m_unk24 != 0) {
            int result = reCollideAfterStepFailure();
            if (result != 0) {
                return result;
            }
        }
        hkReal timeToAdvanceTo = snapSimulateTimeAndGetTimeToAdvanceTo();
        if (m_toiEvents.getSize() != 0) {
            int result = handleAllToisTill(timeToAdvanceTo);
            if (result != 0) {
                m_unk24 = result;
                return result;
            }
        }
        m_unk10 = timeToAdvanceTo;
    }
    if (m_unk10 >= m_unk1C) {
        if (m_world->m_postSimulationListeners.m_size != 0) {
            HK_TIMER_BEGIN("PostSimCb");
            hkWorldCallbackUtil::firePostSimulationCallback(m_world);
            HK_TIMER_END();
        }
    }
    m_unk24 = 0;
    return 0;
}

// Debug-only consistency checks: empty in this build.
void hkContinuousSimulation::assertThereIsNoCollisionInformationForEntities(const void* entities) {}

void hkContinuousSimulation::assertThereIsNoCollisionInformationForAgent(const void* agent) {}

// Adds dt to the time of every pending TOI event of the continuous simulation owned by the world.
void hkContinuousSimulation::warpTime(hkReal dt) {
    hkContinuousSimulation* sim = (hkContinuousSimulation*)m_world->m_simulation;
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
            events[i] = events[m_toiEvents.m_size];
        }
    }
}
