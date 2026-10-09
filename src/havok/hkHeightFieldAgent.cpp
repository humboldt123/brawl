// Havok translation unit hkHeightFieldAgent.o (main.dol 0x802B4524-0x802B7934).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x802B4524   592  __ct   [map: hkHeightFieldAgent____ct]
//   0x802B4800   204  registerAgent   [map: hkHeightFieldAgent__registerAgent]
//   0x802B48CC   124  createHeightFieldBAgent   [map: hkHeightFieldAgent__createHeightFieldBAgent]
//   0x802B4948   132  createHeightFieldAAgent   [map: hkHeightFieldAgent__createHeightFieldAAgent]
//   0x802B49CC   148  __dt   [map: hkHeightFieldAgent____dt]
//   0x802B4A60   168  cleanup   [map: hkHeightFieldAgent__cleanup]
//   0x802B4B08    20  getPenetrations   [map: hkHeightFieldAgent__getPenetrations]
//   0x802B4B1C  1904  staticGetPenetrations   [map: hkHeightFieldAgent__staticGetPenetrations]
//   0x802B528C    20  getClosestPoints   [map: hkHeightFieldAgent__getClosestPoints]
//   0x802B52A0  2224  staticGetClosestPoints   [map: hkHeightFieldAgent__staticGetClosestPoints]
//   0x802B5B50  1296  staticLinearCast   [map: hkHeightFieldAgent__staticLinearCast]
//   0x802B60A0    24  linearCast   [map: hkHeightFieldAgent__linearCast]
//   0x802B60B8   344  addRayHit   [map: hkHeightFieldRayForwardingCollector__addRayHit]
//   0x802B6210  2220  processCollision   [map: hkHeightFieldAgent__processCollision]
//   0x802B6ABC   352  linearCast   [map: hkSymmetricAgent_18hkHeightFieldAgent___linearCast]
//   0x802B6C1C    72  getPenetrations   [map: hkSymmetricAgentLinearCast_18hkHeightFieldAgent___getPenetrations]
//   0x802B6C64    72  staticGetPenetrations   [map: hkSymmetricAgentLinearCast_18hkHeightFieldAgent___staticGetPenetrations]
//   0x802B6CAC    72  getClosestPoints   [map: hkSymmetricAgentLinearCast_18hkHeightFieldAgent___getClosestPoints]
//   0x802B6CF4    72  staticGetClosestPoints   [map: hkSymmetricAgentLinearCast_18hkHeightFieldAgent___staticGetClosestPoints]
//   0x802B6D3C   352  staticLinearCast   [map: hkSymmetricAgentLinearCast_18hkHeightFieldAgent___staticLinearCast]
//   0x802B6E9C  2552  processCollision   [map: hkSymmetricAgentLinearCast_18hkHeightFieldAgent___processCollision]
//   0x802B7894     4  updateShapeCollectionFilter   [map: hkSymmetricAgentLinearCast_18hkHeightFieldAgent___updateShapeCollectionFilter]
//   0x802B7898   156  __dt   [map: hkSymmetricAgent_18hkHeightFieldAgent_____dt]

#pragma fp_contract on
#include <havok/hkHeightFieldAgent.h>
#include <havok/hkBvTreeAgent.h>

#pragma dont_inline on
void hkHeightFieldAgent::getPenetrations(void* a, void* b, void* c, void* target) {
    staticGetPenetrations(a, b, c, target);
}

void hkHeightFieldAgent::getClosestPoints(void* a, void* b, void* c, void* target) {
    staticGetClosestPoints(a, b, c, target);
}

void hkHeightFieldAgent::linearCast(void* a, void* b, void* c, void* target, void* d) {
    staticLinearCast(a, b, c, target, d);
}
#pragma dont_inline reset

void hkSymmetricAgentLinearCast_18hkHeightFieldAgent::getPenetrations(void* a, void* b, void* c, void* target) {
    hkSymmetricFlagTarget wrap((hkPenetrationTarget*)target);
    hkHeightFieldAgent::getPenetrations(b, a, c, &wrap);
}

void hkSymmetricAgentLinearCast_18hkHeightFieldAgent::staticGetPenetrations(void* a, void* b, void* c, void* target) {
    hkSymmetricFlagTarget wrap((hkPenetrationTarget*)target);
    hkHeightFieldAgent::staticGetPenetrations(b, a, c, &wrap);
}

void hkSymmetricAgentLinearCast_18hkHeightFieldAgent::getClosestPoints(void* a, void* b, void* c, void* target) {
    hkSymmetricClosestTarget wrap((hkPenetrationTarget*)target);
    hkHeightFieldAgent::getClosestPoints(b, a, c, &wrap);
}

void hkSymmetricAgentLinearCast_18hkHeightFieldAgent::staticGetClosestPoints(void* a, void* b, void* c, void* target) {
    hkSymmetricClosestTarget wrap((hkPenetrationTarget*)target);
    hkHeightFieldAgent::staticGetClosestPoints(b, a, c, &wrap);
}

void hkSymmetricAgentLinearCast_18hkHeightFieldAgent::updateShapeCollectionFilter(void* a, void* b, void* c) {}
