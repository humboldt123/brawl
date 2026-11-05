// Havok translation unit hk_symmetric_agent22hk_shape_collection_agent.o (main.dol 0x802C1F7C-0x802C23B8).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x802C1F7C   352  linearCast   [map: hkSymmetricAgent_22hkShapeCollectionAgent___linearCast]
//   0x802C20DC    72  getPenetrations   [map: hkSymmetricAgentLinearCast_22hkShapeCollectionAgent___getPenetrations]
//   0x802C2124    72  getClosestPoints   [map: hkSymmetricAgentLinearCast_22hkShapeCollectionAgent___getClosestPoints]
//   0x802C216C   412  processCollision   [map: hkSymmetricAgentLinearCast_22hkShapeCollectionAgent___processCollision]
//   0x802C2308    16  updateShapeCollectionFilter   [map: hkSymmetricAgentLinearCast_22hkShapeCollectionAgent___updateShapeCollectionFilter]
//   0x802C2318   160  __dt   [map: hkSymmetricAgent_22hkShapeCollectionAgent_____dt]

#include <havok/hkShapeCollectionAgent.h>
#include <havok/hkCollisionInput.h>
#include <havok/hkSymmetricAgentFlipCollectors.h>
#include <havok/hkSymmetricAgentProcessCollision.h>
#include <havok/hkBvAgent.h>

// Symmetric wrapper of hkShapeCollectionAgent (map name hkSymmetricAgentLinearCast_22hkShapeCollectionAgent___
// updateShapeCollectionFilter): swap the bodies and forward to the plain agent.
template <class T>
void hkSymmetricAgentLinearCast<T>::updateShapeCollectionFilter(void* a, void* b, void* c) {
    T::updateShapeCollectionFilter(b, a, c);
}

template void hkSymmetricAgentLinearCast<hkShapeCollectionAgent>::updateShapeCollectionFilter(void*, void*, void*);
template void hkSymmetricAgentLinearCast<hkShapeCollectionAgent>::processCollision(void*, void*, void*, void*);

template <class T>
void hkSymmetricAgentLinearCast<T>::getPenetrations(void* a, void* b, void* c, void* target) {
    hkSymmetricAgentFlipBodyCollector wrapped(target);
    T::getPenetrations(b, a, c, &wrapped);
}

template <class T>
void hkSymmetricAgentLinearCast<T>::getClosestPoints(void* a, void* b, void* c, void* target) {
    hkSymmetricAgentFlipCollector wrapped(target);
    T::getClosestPoints(b, a, c, &wrapped);
}

template void hkSymmetricAgentLinearCast<hkShapeCollectionAgent>::getPenetrations(void*, void*, void*, void*);
template void hkSymmetricAgentLinearCast<hkShapeCollectionAgent>::getClosestPoints(void*, void*, void*, void*);

// The linear cast with the bodies swapped: the path is negated and each collector is wrapped in one that turns
// the points back.
void hkShapeCollectionAgentVariant::linearCast(void* a, void* b, void* c, void* target, void* d) {
    const hkLinearCastCollisionInput& input = *(const hkLinearCastCollisionInput*)c;
    hkLinearCastCollisionInput flippedInput = input;
    flippedInput.m_path.setNeg4(input.m_path);
    hkSymmetricAgentFlipCastCollector flipped(input.m_path, target);
    if (d != 0) {
        hkSymmetricAgentFlipCastCollector flippedStart(input.m_path, d);
        hkShapeCollectionAgent::linearCast(b, a, &flippedInput, &flipped, &flippedStart);
    } else {
        hkShapeCollectionAgent::linearCast(b, a, &flippedInput, &flipped, 0);
    }
}
