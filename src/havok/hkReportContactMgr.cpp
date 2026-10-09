// Havok translation unit hkReportContactMgr.o (main.dol 0x802D7708-0x802D7FC8).
// Functions in address order (method names from the Havok TU map). Only the functions listed below are
// written so far; the contact point, TOI and processing functions are not done yet.
//   0x802D7708    60  __ct   [map: hkReportContactMgr____ct]
//   0x802D7744    92  __dt   [map: hkReportContactMgr____dt]
//   0x802D7F20    28  __ct   [map: hkReportContactMgr7FactoryFP7hkWorld____ct]
//   0x802D7F3C   132  createContactMgr   [map: hkReportContactMgr7FactoryFRC12hkCollidableRC12hkCollidableRC16hkCollisionInput__createContactMgr]
//   0x802D7FC0     8  getConstraintInstance   [map: hkDynamicsContactMgr__getConstraintInstance]

#include <havok/hkReportContactMgr.h>

hkReportContactMgr::hkReportContactMgr(hkWorld* world, const void* ownerA, const void* ownerB) {
    // HYPOTHESIS: the limit is the smaller of the two owners' u16 fields at 0x92.
    u16 limitB = *(const u16*)((const char*)ownerB + 0x92);
    u16 limitA = *(const u16*)((const char*)ownerA + 0x92);
    m_maxContacts = limitA < limitB ? limitA : limitB;
    m_world = world;
    m_ownerA = ownerA;
    m_ownerB = ownerB;
}

hkReportContactMgr::~hkReportContactMgr() {}

hkReportContactMgr::Factory::Factory(hkWorld* world) {
    m_factoryWorld = world;
}

hkReportContactMgr* hkReportContactMgr::Factory::createContactMgr(const hkCollidable& a, const hkCollidable& b,
                                                                  const void* input) {
    const void* ownerA = (const char*)&a + a.m_ownerOffset;
    const void* ownerB = (const char*)&b + b.m_ownerOffset;
    hkReportContactMgr* mgr = (hkReportContactMgr*)hkMemory::getInstance().allocateChunk(0x18, 0x20);
    if (mgr != 0) {
        mgr->m_memSizeAndFlags = 0x18;
        ::new (mgr) hkReportContactMgr(m_factoryWorld, ownerA, ownerB);
    }
    return mgr;
}

void* hkDynamicsContactMgr::getConstraintInstance() {
    return 0;
}
