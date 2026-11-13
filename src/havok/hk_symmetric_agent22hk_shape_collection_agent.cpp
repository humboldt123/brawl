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
void hkSymmetricAgentLinearCast<T>::updateShapeCollectionFilter(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c) {
    T::updateShapeCollectionFilter(b, a, c);
}

template void hkSymmetricAgentLinearCast<hkShapeCollectionAgent>::updateShapeCollectionFilter(const hkCdBody&, const hkCdBody&, hkCollisionInput&);
template void hkSymmetricAgentLinearCast<hkShapeCollectionAgent>::processCollision(const hkCdBody&, const hkCdBody&, hkProcessCollisionInput&, void*);

template <class T>
void hkSymmetricAgentLinearCast<T>::getPenetrations(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* target) {
    hkSymmetricAgentFlipBodyCollector wrapped(target);
    T::getPenetrations(b, a, c, &wrapped);
}

template <class T>
void hkSymmetricAgentLinearCast<T>::getClosestPoints(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* target) {
    hkSymmetricAgentFlipCollector wrapped(target);
    T::getClosestPoints(b, a, c, &wrapped);
}

template void hkSymmetricAgentLinearCast<hkShapeCollectionAgent>::getPenetrations(const hkCdBody&, const hkCdBody&, hkCollisionInput&, void*);
template void hkSymmetricAgentLinearCast<hkShapeCollectionAgent>::getClosestPoints(const hkCdBody&, const hkCdBody&, hkCollisionInput&, void*);

// The linear cast with the bodies swapped: the path is negated and each collector is wrapped in one that turns
// the points back.
void hkShapeCollectionAgentVariant::linearCast(const hkCdBody& a, const hkCdBody& b, hkLinearCastCollisionInput& c, void* target, void* d) {
    const hkLinearCastCollisionInput& input = c;
    hkLinearCastCollisionInput flippedInput = input;
    flippedInput.m_path.setNeg4(input.m_path);
    hkSymmetricAgentFlipCastCollector flipped(input.m_path, target);
    if (d != 0) {
        hkSymmetricAgentFlipCastCollector flippedStart(input.m_path, d);
        hkShapeCollectionAgent::linearCast(b, a, flippedInput, &flipped, &flippedStart);
    } else {
        hkShapeCollectionAgent::linearCast(b, a, flippedInput, &flipped, 0);
    }
}

// fn_802C2318: body from https://github.com/jj-link/brawl src/havok/fn_802C2318.cpp (namespaced; memory singletons rebound to our names).
#include <types.h>
#include <havok/hkMemory.h>
#include <havok/hkThreadMemory.h>
#include <havok/hkArray.h>
namespace jj_fn_802C2318 {
typedef unsigned int u32;


class SoHeapAllocator {
public:
    virtual void vf0();
    virtual void vf1();
    virtual void vf2();
    virtual void vf3();
    virtual void vf4();
    virtual void free(void*, unsigned short, int);
};


struct Obj {
    void* vtable;
    unsigned short type;
    char pad[6];
    void* data;
    char pad2[4];
    unsigned int raw;

    bool isOwned() const {
        return (raw & 0x80000000) == 0;
    }

    void dealloc() {
        hkThreadMemory::s_instance->deallocateChunk(data, raw << 3, 0x15);
    }
};

// TODO: cleanup (shape from jj-link/brawl fn_802C2318)
extern "C" void* fn_802C2318(Obj* self, int flag) {
    if (self) {
        if ((u32)self && (u32)self && (u32)self + 0xc && (u32)self + 0xc && self->isOwned()) {
            self->dealloc();
        }
        if (flag > 0) {
            ((SoHeapAllocator*)hkMemory::s_instance)->free(self, self->type, 0x1d);
        }
    }
    return self;
}
} // namespace jj_fn_802C2318
