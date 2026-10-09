// Havok translation unit hkBvTreeAgent.o (main.dol 0x802A3A4C-0x802A8E1C).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x802A3A4C    84  __ct   [map: hkBvTreeAgent____ct]
//   0x802A3B30   248  registerAgent   [map: hkBvTreeAgent__registerAgent]
//   0x802A3C28   120  createBvTreeShapeAgent   [map: hkBvTreeAgent__createBvTreeShapeAgent]
//   0x802A3CA0   152  __dt   [map: hkBvTreeAgent____dt]
//   0x802A3D38   108  createShapeBvAgent   [map: hkBvTreeAgent__createShapeBvAgent]
//   0x802A3DA4   208  createBvBvAgent   [map: hkBvTreeAgent__createBvBvAgent]
//   0x802A3E74   148  cleanup   [map: hkBvTreeAgent__cleanup]
//   0x802A3F08   120  invalidateTim   [map: hkBvTreeAgent__invalidateTim]
//   0x802A3F80   152  warpTime   [map: hkBvTreeAgent__warpTime]
//   0x802A4018  3068  prepareCollisionPartners   [map: hkBvTreeAgent__prepareCollisionPartners]
//   0x802A4C14    44  quickSort<Ui>   [map: hkAlgorithm__quickSort_Ui_]
//   0x802A4C40    60  quickSort<Ui,Q211hkAlgorithm8less<Ui>>   [map: hkAlgorithm__quickSort_Ui_Q211hkAlgorithm8less_Ui__]
//   0x802A4C7C   296  quickSortRecursive<Ui,Q211hkAlgorithm8less<Ui>>   [map: hkAlgorithm__quickSortRecursive_Ui_Q211hkAlgorithm8less_Ui__]
//   0x802A4E34  1212  fastUpdate   [map: hkAgentDispatchUtil_Ui_Q213hkBvTreeAgent18hkBvAgentEntryInfo_25hkAgentDispatchUtilHelper___fastUpdate]
//   0x802A52F0  1392  update   [map: hkAgentDispatchUtil_Ui_Q213hkBvTreeAgent18hkBvAgentEntryInfo_25hkAgentDispatchUtilHelper___update]
//   0x802A5920  3048  prepareCollisionPartnersLinearCast   [map: hkBvTreeAgent__prepareCollisionPartnersLinearCast]
//   0x802A6508   524  updateShapeCollectionFilter   [map: hkBvTreeAgent__updateShapeCollectionFilter]
//   0x802A6714   804  processCollision   [map: hkBvTreeAgent__processCollision]
//   0x802A6A38   348  calcTimInfo   [map: hkSweptTransformUtil__calcTimInfo]
//   0x802A6B94  2944  calcAabbAndQueryTree   [map: hkBvTreeAgent__calcAabbAndQueryTree]
//   0x802A7714   484  linearCast   [map: hkBvTreeAgent__linearCast]
//   0x802A78F8  1116  staticLinearCast   [map: hkBvTreeAgent__staticLinearCast]
//   0x802A7D54   476  getClosestPoints   [map: hkBvTreeAgent__getClosestPoints]
//   0x802A7F30   724  staticGetClosestPoints   [map: hkBvTreeAgent__staticGetClosestPoints]
//   0x802A8204   500  getPenetrations   [map: hkBvTreeAgent__getPenetrations]
//   0x802A83F8   748  staticGetPenetrations   [map: hkBvTreeAgent__staticGetPenetrations]
//   0x802A86E4   248  calcStatistics   [map: hkBvTreeAgent__calcStatistics]
//   0x802A87DC   352  linearCast   [map: hkSymmetricAgent_13hkBvTreeAgent___linearCast]
//   0x802A893C    72  getPenetrations   [map: hkSymmetricAgentLinearCast_13hkBvTreeAgent___getPenetrations]
//   0x802A8984    72  staticGetPenetrations   [map: hkSymmetricAgentLinearCast_13hkBvTreeAgent___staticGetPenetrations]
//   0x802A89CC    72  getClosestPoints   [map: hkSymmetricAgentLinearCast_13hkBvTreeAgent___getClosestPoints]
//   0x802A8A14    72  staticGetClosestPoints   [map: hkSymmetricAgentLinearCast_13hkBvTreeAgent___staticGetClosestPoints]
//   0x802A8A5C   352  staticLinearCast   [map: hkSymmetricAgentLinearCast_13hkBvTreeAgent___staticLinearCast]
//   0x802A8BBC   412  processCollision   [map: hkSymmetricAgentLinearCast_13hkBvTreeAgent___processCollision]
//   0x802A8D58    16  updateShapeCollectionFilter   [map: hkSymmetricAgentLinearCast_13hkBvTreeAgent___updateShapeCollectionFilter]
//   0x802A8D68   160  __dt   [map: hkSymmetricAgent_13hkBvTreeAgent_____dt]
//   0x802A8E08    20  __sinit_\hkBvTreeAgent_cpp   [map: hkBvTreeAgentcpp____sinit_]

#pragma fp_contract on
#include <havok/hkBvTreeAgent.h>
#include <havok/hkCollisionDispatcher.h>

hkBvTreeAgent::hkBvTreeAgent(hkContactMgr* contactMgr) : hkCollisionAgent((int)contactMgr) {
    unk30[0] = 0.0f;
    unk30[1] = 0.0f;
    unk30[2] = 0.0f;
    unk30[3] = 0.0f;
    unk20[0] = 0.0f;
    unk20[1] = 0.0f;
    unk20[2] = 0.0f;
    unk20[3] = 0.0f;
}

void hkBvTreeAgent::registerAgent(hkCollisionDispatcher* dispatcher) {
    hkAgentFuncs funcs;
    funcs.create = (hkAgentFunc)createBvTreeShapeAgent;
    funcs.staticGetPenetrations = (hkAgentFunc)hkSymmetricAgentLinearCast_13hkBvTreeAgent::staticGetPenetrations;
    funcs.staticGetClosestPoints = (hkAgentFunc)hkSymmetricAgentLinearCast_13hkBvTreeAgent::staticGetClosestPoints;
    funcs.staticLinearCast = (hkAgentFunc)hkSymmetricAgentLinearCast_13hkBvTreeAgent::staticLinearCast;
    funcs.symmetricA = 1;
    funcs.symmetricB = 1;
    dispatcher->registerCollisionAgent(&funcs, 3, -1);

    funcs.create = (hkAgentFunc)createShapeBvAgent;
    funcs.staticGetPenetrations = (hkAgentFunc)staticGetPenetrations;
    funcs.staticGetClosestPoints = (hkAgentFunc)staticGetClosestPoints;
    funcs.staticLinearCast = (hkAgentFunc)staticLinearCast;
    funcs.symmetricA = 0;
    funcs.symmetricB = 1;
    dispatcher->registerCollisionAgent(&funcs, -1, 3);

    funcs.create = (hkAgentFunc)createBvBvAgent;
    dispatcher->registerCollisionAgent(&funcs, 3, 3);
}

hkBvTreeAgent* hkBvTreeAgent::createBvTreeShapeAgent(void* a, void* b, void* c, hkContactMgr* contactMgr) {
    hkBvTreeAgent* agent = (hkBvTreeAgent*)hkMemory::s_instance->allocateChunk(sizeof(hkBvTreeAgent), 0x1d);
    agent->m_memSizeAndFlags = sizeof(hkBvTreeAgent);
    new (agent) hkSymmetricAgentLinearCast_13hkBvTreeAgent(contactMgr);
    return agent;
}

hkBvTreeAgent* hkBvTreeAgent::createShapeBvAgent(void* a, void* b, void* c, hkContactMgr* contactMgr) {
    hkBvTreeAgent* agent = (hkBvTreeAgent*)hkMemory::s_instance->allocateChunk(sizeof(hkBvTreeAgent), 0x1d);
    agent->m_memSizeAndFlags = sizeof(hkBvTreeAgent);
    new (agent) hkBvTreeAgent(contactMgr);
    return agent;
}

hkBvTreeAgent* hkBvTreeAgent::createBvBvAgent(void* a, void* b, void* c, hkContactMgr* contactMgr) {
    // HYPOTHESIS: the sort key is the float at 0xA0 of each body's motion (not identified yet).
    float keyA = *(float*)((u8*)((hkCdBody*)a)->m_motion + 0xA0);
    float keyB = *(float*)((u8*)((hkCdBody*)b)->m_motion + 0xA0);
    if (keyA < keyB) {
        hkBvTreeAgent* agent = (hkBvTreeAgent*)hkMemory::s_instance->allocateChunk(sizeof(hkBvTreeAgent), 0x1d);
        agent->m_memSizeAndFlags = sizeof(hkBvTreeAgent);
        new (agent) hkBvTreeAgent(contactMgr);
        return agent;
    }
    hkBvTreeAgent* agent = (hkBvTreeAgent*)hkMemory::s_instance->allocateChunk(sizeof(hkBvTreeAgent), 0x1d);
    agent->m_memSizeAndFlags = sizeof(hkBvTreeAgent);
    new (agent) hkSymmetricAgentLinearCast_13hkBvTreeAgent(contactMgr);
    return agent;
}

void hkSymmetricAgentLinearCast_13hkBvTreeAgent::getPenetrations(void* a, void* b, void* c, void* target) {
    hkSymmetricFlagTarget wrap((hkPenetrationTarget*)target);
    hkBvTreeAgent::getPenetrations(b, a, c, &wrap);
}

void hkSymmetricAgentLinearCast_13hkBvTreeAgent::staticGetPenetrations(void* a, void* b, void* c, void* target) {
    hkSymmetricFlagTarget wrap((hkPenetrationTarget*)target);
    hkBvTreeAgent::staticGetPenetrations(b, a, c, &wrap);
}

void hkSymmetricAgentLinearCast_13hkBvTreeAgent::getClosestPoints(void* a, void* b, void* c, void* target) {
    hkSymmetricClosestTarget wrap((hkPenetrationTarget*)target);
    hkBvTreeAgent::getClosestPoints(b, a, c, &wrap);
}

void hkSymmetricAgentLinearCast_13hkBvTreeAgent::staticGetClosestPoints(void* a, void* b, void* c, void* target) {
    hkSymmetricClosestTarget wrap((hkPenetrationTarget*)target);
    hkBvTreeAgent::staticGetClosestPoints(b, a, c, &wrap);
}

void hkSymmetricAgentLinearCast_13hkBvTreeAgent::updateShapeCollectionFilter(void* a, void* b, void* c) {
    hkBvTreeAgent::updateShapeCollectionFilter(b, a, c);
}
