// Havok translation unit hkListAgent.o (main.dol 0x802B85EC-0x802B8E78).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x802B85EC   204  registerAgent   [map: hkListAgent__registerAgent]
//   0x802B86B8   132  createListAAgent   [map: hkListAgent__createListAAgent]
//   0x802B873C   124  createListBAgent   [map: hkListAgent__createListBAgent]
//   0x802B87B8   132  __ct   [map: hkListAgent____ct]
//   0x802B883C    88  cleanup   [map: hkListAgent__cleanup]
//   0x802B8894     8  invalidateTim   [map: hkListAgent__invalidateTim]
//   0x802B889C     8  warpTime   [map: hkListAgent__warpTime]
//   0x802B88A4   832  processCollision   [map: hkListAgent__processCollision]
//   0x802B8BE4    20  getClosestPoints   [map: hkListAgent__getClosestPoints]
//   0x802B8BF8   176  staticGetClosestPoints   [map: hkListAgent__staticGetClosestPoints]
//   0x802B8CA8    24  linearCast   [map: hkListAgent__linearCast]
//   0x802B8CC0   176  staticLinearCast   [map: hkListAgent__staticLinearCast]
//   0x802B8D70    20  getPenetrations   [map: hkListAgent__getPenetrations]
//   0x802B8D84   176  staticGetPenetrations   [map: hkListAgent__staticGetPenetrations]
//   0x802B8E34    68  updateShapeCollectionFilter   [map: hkListAgent__updateShapeCollectionFilter]

#include <havok/hkListAgent.h>

// Stand-ins for the member functions of the embedded object at 0x10 (other unit, not recovered yet).
extern "C" void fn_802FC824(void* self, void* arg);
extern "C" void fn_802FC988(void* self, float t0, float t1, void* arg);

void hkListAgent::invalidateTim(void* arg) {
    fn_802FC824((char*)this + 0x10, arg);
}

void hkListAgent::warpTime(float t0, float t1, void* arg) {
    fn_802FC988((char*)this + 0x10, t0, t1, arg);
}

void hkListAgent::getClosestPoints(void* a, void* b, void* c, hkPenetrationTarget* target) {
    staticGetClosestPoints(a, b, c, target);
}

void hkListAgent::getPenetrations(void* a, void* b, void* c, hkPenetrationTarget* target) {
    staticGetPenetrations(a, b, c, target);
}

void hkListAgent::linearCast(void* a, void* b, void* c, hkPenetrationTarget* target, void* d) {
    staticLinearCast(a, b, c, target, d);
}
