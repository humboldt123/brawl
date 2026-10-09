// Havok translation unit hk_symmetric_agent21hk_multi_ray_convex_agent.o (main.dol 0x802BA204-0x802BAB24).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x802BA204   352  linearCast   [map: hkSymmetricAgent_21hkMultiRayConvexAgent___linearCast]
//   0x802BA364    72  getPenetrations   [map: hkSymmetricAgentLinearCast_21hkMultiRayConvexAgent___getPenetrations]
//   0x802BA3AC    72  staticGetPenetrations   [map: hkSymmetricAgentLinearCast_21hkMultiRayConvexAgent___staticGetPenetrations]
//   0x802BA3F4    72  getClosestPoints   [map: hkSymmetricAgentLinearCast_21hkMultiRayConvexAgent___getClosestPoints]
//   0x802BA43C    72  staticGetClosestPoints   [map: hkSymmetricAgentLinearCast_21hkMultiRayConvexAgent___staticGetClosestPoints]
//   0x802BA484   352  staticLinearCast   [map: hkSymmetricAgentLinearCast_21hkMultiRayConvexAgent___staticLinearCast]
//   0x802BA5E4  1340  processCollision   [map: hkSymmetricAgentLinearCast_21hkMultiRayConvexAgent___processCollision]
//   0x802BAB20     4  updateShapeCollectionFilter   [map: hkSymmetricAgentLinearCast_21hkMultiRayConvexAgent___updateShapeCollectionFilter]

#include <havok/hkMultiRayConvexAgent.h>

// Symmetric wrappers: the shapes are swapped and the collector is replaced by a flipping collector on the stack.
void hkSymmetricAgentLinearCast_21hkMultiRayConvexAgent_::getPenetrations(void* unk0, void* unk1, void* unk2, void* unk3) {
    hkSymmetricAgentFlipCollector flip(unk3);
    hkMultiRayConvexAgent::getPenetrations(unk1, unk0, unk2, &flip);
}

void hkSymmetricAgentLinearCast_21hkMultiRayConvexAgent_::staticGetPenetrations(void* unk0, void* unk1, void* unk2, void* unk3) {
    hkSymmetricAgentFlipCollector flip(unk3);
    hkMultiRayConvexAgent::staticGetPenetrations(unk1, unk0, unk2, &flip);
}

void hkSymmetricAgentLinearCast_21hkMultiRayConvexAgent_::getClosestPoints(void* unk0, void* unk1, void* unk2, void* unk3) {
    hkSymmetricAgentFlipCastCollector flip(unk3);
    hkMultiRayConvexAgent::getClosestPoints(unk1, unk0, unk2, &flip);
}

void hkSymmetricAgentLinearCast_21hkMultiRayConvexAgent_::staticGetClosestPoints(void* unk0, void* unk1, void* unk2, void* unk3) {
    hkSymmetricAgentFlipCastCollector flip(unk3);
    hkMultiRayConvexAgent::staticGetClosestPoints(unk1, unk0, unk2, &flip);
}

void hkSymmetricAgentLinearCast_21hkMultiRayConvexAgent_::updateShapeCollectionFilter(void* unk0, void* unk1, void* unk2) {}

hkSymmetricAgentLinearCast_21hkMultiRayConvexAgent_::~hkSymmetricAgentLinearCast_21hkMultiRayConvexAgent_() {}
hkSymmetricAgent_21hkMultiRayConvexAgent_::~hkSymmetricAgent_21hkMultiRayConvexAgent_() {}
