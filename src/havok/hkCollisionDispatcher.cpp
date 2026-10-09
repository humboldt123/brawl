// Havok translation unit hkCollisionDispatcher.o (main.dol 0x802CB630-0x802CD000).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x802CB630    16  create   [map: hkNullAgent3__create]
//   0x802CB640     4  destroy   [map: hkNullAgent3__destroy]
//   0x802CB644     8  process   [map: hkNullAgent3__process]
//   0x802CB64C   708  resetCreationFunctions   [map: hkCollisionDispatcher__resetCreationFunctions]
//   0x802CB910  1424  __ct   [map: hkCollisionDispatcher____ct]
//   0x802CBF2C    12  setEnableChecks   [map: hkCollisionDispatcher__setEnableChecks]
//   0x802CBF38   164  disableDebugging   [map: hkCollisionDispatcher__disableDebugging]
//   0x802CBFDC   272  __dt   [map: hkCollisionDispatcher____dt]
//   0x802CC0EC   296  registerCollisionAgent   [map: hkCollisionDispatcher__registerCollisionAgent]
//   0x802CC214   728  registerAgent3   [map: hkCollisionDispatcher__registerAgent3]
//   0x802CC4EC   576  internalRegisterCollisionAgent   [map: hkCollisionDispatcher__internalRegisterCollisionAgent]
//   0x802CC72C   148  updateHasAlternateType   [map: hkCollisionDispatcher__updateHasAlternateType]
//   0x802CC7C0   284  registerAlternateShapeType   [map: hkCollisionDispatcher__registerAlternateShapeType]
//   0x802CC8DC   288  registerContactMgrFactoryWithAll   [map: hkCollisionDispatcher__registerContactMgrFactoryWithAll]
//   0x802CC9FC   288  calcStatistics   [map: hkCollisionDispatcher__calcStatistics]
//   0x802CCB1C  1252  initCollisionQualityInfo   [map: hkCollisionDispatcher__initCollisionQualityInfo]

#include <havok/hkCollisionDispatcher.h>

// Null agent: a registration target that never produces contacts (the class is only used in this unit).
struct hkNullAgent3 {
    static void* create(void* unk0, u8* flag, void* agent);
    static void destroy(void* unk0);
    static void* process(void* unk0, void* unk1, void* agent);
};

// HYPOTHESIS: the first parameter is unused. Clears the flag byte and returns the agent.
void* hkNullAgent3::create(void* unk0, u8* flag, void* agent) {
    *flag = 0;
    return agent;
}

void hkNullAgent3::destroy(void* unk0) {}

void* hkNullAgent3::process(void* unk0, void* unk1, void* agent) {
    return agent;
}

void hkCollisionDispatcher::setEnableChecks(const hkBool& enable) {
    m_enableChecks = enable.m_bool;
}

void hkCollisionDispatcher::disableDebugging() {
    if (m_buffers[0] != 0) {
        hkMemory::getInstance().deallocate(m_buffers[0]);
        hkMemory::getInstance().deallocate(m_buffers[1]);
        hkMemory::getInstance().deallocate(m_buffers[2]);
        hkMemory::getInstance().deallocate(m_buffers[3]);
        m_buffers[0] = 0;
        m_buffers[1] = 0;
        m_buffers[2] = 0;
        m_buffers[3] = 0;
    }
}

void hkCollisionDispatcher::registerCollisionAgent(const hkAgentFuncs* funcs, int typeA, int typeB) {
    m_agentFuncs[m_numAgents] = *funcs;
    internalRegisterCollisionAgent(m_lookupE94, 1, typeA, typeB, typeA, typeB, m_buffers[2], 0);
    internalRegisterCollisionAgent(m_lookup190, m_numAgents, typeA, typeB, typeA, typeB, m_buffers[0], 0);
    if (funcs->symmetricB != 0) {
        internalRegisterCollisionAgent(m_lookup1294, 1, typeA, typeB, typeA, typeB, m_buffers[3], 0);
        internalRegisterCollisionAgent(m_lookup590, m_numAgents, typeA, typeB, typeA, typeB, m_buffers[1], 0);
    }
    m_numAgents++;
}
