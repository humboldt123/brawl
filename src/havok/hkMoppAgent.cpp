// Havok translation unit hkMoppAgent.o (main.dol 0x802A8E1C-0x802A99B0).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x802A8E1C    60  __ct   [map: hkMoppAgent____ct]
//   0x802A8E58   248  registerAgent   [map: hkMoppAgent__registerAgent]
//   0x802A8F50   212  createBvBvAgent   [map: hkMoppAgent__createBvBvAgent]
//   0x802A9024   156  __dt   [map: hkMoppAgent____dt]
//   0x802A90C0   640  staticLinearCast   [map: hkMoppAgent__staticLinearCast]
//   0x802A9380   352  linearCast   [map: hkSymmetricAgent_11hkMoppAgent___linearCast]
//   0x802A94E0    72  getPenetrations   [map: hkSymmetricAgentLinearCast_11hkMoppAgent___getPenetrations]
//   0x802A9528    72  staticGetPenetrations   [map: hkSymmetricAgentLinearCast_11hkMoppAgent___staticGetPenetrations]
//   0x802A9570    72  getClosestPoints   [map: hkSymmetricAgentLinearCast_11hkMoppAgent___getClosestPoints]
//   0x802A95B8    72  staticGetClosestPoints   [map: hkSymmetricAgentLinearCast_11hkMoppAgent___staticGetClosestPoints]
//   0x802A9600   352  staticLinearCast   [map: hkSymmetricAgentLinearCast_11hkMoppAgent___staticLinearCast]
//   0x802A9760   412  processCollision   [map: hkSymmetricAgentLinearCast_11hkMoppAgent___processCollision]
//   0x802A98FC    16  updateShapeCollectionFilter   [map: hkSymmetricAgentLinearCast_11hkMoppAgent___updateShapeCollectionFilter]
//   0x802A990C   164  __dt   [map: hkSymmetricAgent_11hkMoppAgent_____dt]

#include <havok/hkMoppAgent.h>
#include <havok/hkCollisionInput.h>
#include <havok/hkSymmetricAgentFlipCollectors.h>
#include <havok/hkSymmetricAgentProcessCollision.h>
#include <havok/hkCollisionDispatcher.h>

hkMoppAgent::hkMoppAgent(hkContactMgr* contactMgr) : hkBvTreeAgent(contactMgr) {}

// TODO: cleanup (shape from jj-link/brawl fn_802A8E58)
// MOPP x anything in both orders, and MOPP x MOPP (shape type 24; -1 is every type).
void hkMoppAgent::registerAgent(hkCollisionDispatcher* dispatcher) {
    hkAgentFuncs af;
    af.create = (hkAgentFunc)&hkBvTreeAgent::createBvTreeShapeAgent;
    af.staticGetPenetrations = (hkAgentFunc)hkSymmetricAgentLinearCast_11hkMoppAgent::staticGetPenetrations;
    af.staticGetClosestPoints = (hkAgentFunc)hkSymmetricAgentLinearCast_11hkMoppAgent::staticGetClosestPoints;
    af.staticLinearCast = (hkAgentFunc)hkSymmetricAgentLinearCast_11hkMoppAgent::staticLinearCast;
    af.symmetricA = true;
    af.symmetricB = false;
    dispatcher->registerCollisionAgent(&af, 0x18, -1);
    hkAgentFuncs bf;
    bf.create = (hkAgentFunc)&hkBvTreeAgent::createShapeBvAgent;
    bf.staticGetPenetrations = (hkAgentFunc)hkBvTreeAgent::staticGetPenetrations;
    bf.staticGetClosestPoints = (hkAgentFunc)hkBvTreeAgent::staticGetClosestPoints;
    bf.staticLinearCast = (hkAgentFunc)hkMoppAgent::staticLinearCast;
    bf.symmetricA = false;
    bf.symmetricB = false;
    dispatcher->registerCollisionAgent(&bf, -1, 0x18);
    hkAgentFuncs cf;
    cf.create = (hkAgentFunc)&hkMoppAgent::createBvBvAgent;
    cf.staticGetPenetrations = (hkAgentFunc)hkBvTreeAgent::staticGetPenetrations;
    cf.staticGetClosestPoints = (hkAgentFunc)hkBvTreeAgent::staticGetClosestPoints;
    cf.staticLinearCast = (hkAgentFunc)hkMoppAgent::staticLinearCast;
    cf.symmetricA = false;
    cf.symmetricB = true;
    dispatcher->registerCollisionAgent(&cf, 0x18, 0x18);
}

void hkSymmetricAgentLinearCast_11hkMoppAgent::getPenetrations(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* target) {
    hkSymmetricAgentFlipBodyCollector wrap(target);
    hkMoppAgent::getPenetrations(b, a, c, &wrap);
}

void hkSymmetricAgentLinearCast_11hkMoppAgent::staticGetPenetrations(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* target) {
    hkSymmetricAgentFlipBodyCollector wrap(target);
    hkMoppAgent::staticGetPenetrations(b, a, c, &wrap);
}

void hkSymmetricAgentLinearCast_11hkMoppAgent::getClosestPoints(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* target) {
    hkSymmetricAgentFlipCollector wrap(target);
    hkMoppAgent::getClosestPoints(b, a, c, &wrap);
}

void hkSymmetricAgentLinearCast_11hkMoppAgent::staticGetClosestPoints(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* target) {
    hkSymmetricAgentFlipCollector wrap(target);
    hkMoppAgent::staticGetClosestPoints(b, a, c, &wrap);
}

void hkSymmetricAgentLinearCast_11hkMoppAgent::updateShapeCollectionFilter(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c) {
    hkMoppAgent::updateShapeCollectionFilter(b, a, c);
}

// The linear cast with the bodies swapped: the path is negated and each collector is wrapped in one that turns
// the points back.
void hkSymmetricAgent_11hkMoppAgent_::linearCast(const hkCdBody& a, const hkCdBody& b, hkLinearCastCollisionInput& c, void* target, void* d) {
    const hkLinearCastCollisionInput& input = c;
    hkLinearCastCollisionInput flippedInput = input;
    flippedInput.m_path.setNeg4(input.m_path);
    hkSymmetricAgentFlipCastCollector flipped(input.m_path, target);
    if (d != 0) {
        hkSymmetricAgentFlipCastCollector flippedStart(input.m_path, d);
        hkBvTreeAgent::linearCast(b, a, flippedInput, &flipped, &flippedStart);
    } else {
        hkBvTreeAgent::linearCast(b, a, flippedInput, &flipped, 0);
    }
}

// The linear cast with the bodies swapped: the path is negated and each collector is wrapped in one that turns
// the points back.
void hkSymmetricAgentLinearCast_11hkMoppAgent::staticLinearCast(const hkCdBody& a, const hkCdBody& b, hkLinearCastCollisionInput& c, void* target, void* d) {
    const hkLinearCastCollisionInput& input = c;
    hkLinearCastCollisionInput flippedInput = input;
    flippedInput.m_path.setNeg4(input.m_path);
    hkSymmetricAgentFlipCastCollector flipped(input.m_path, target);
    if (d != 0) {
        hkSymmetricAgentFlipCastCollector flippedStart(input.m_path, d);
        hkMoppAgent::staticLinearCast(b, a, flippedInput, &flipped, &flippedStart);
    } else {
        hkMoppAgent::staticLinearCast(b, a, flippedInput, &flipped, 0);
    }
}

void hkSymmetricAgentLinearCast_11hkMoppAgent::processCollision(const hkCdBody& a, const hkCdBody& b, hkProcessCollisionInput& c, void* d) {
    hkSymmetricAgent_processCollision<hkMoppAgent>(this, a, b, c, d);
}

// fn_802A990C: body from https://github.com/jj-link/brawl src/havok/fn_802A990C.cpp (namespaced; memory singletons rebound to our names).
#include <types.h>
#include <havok/hkMemory.h>
#include <havok/hkThreadMemory.h>
#include <havok/hkArray.h>
namespace jj_fn_802A990C {
class SoHeapAllocator {
public:
    virtual void vf0();
    virtual void vf1();
    virtual void vf2();
    virtual void vf3();
    virtual void vf4();
    virtual void free(void*, unsigned short, int);
};


struct Array {
    void* data;
    int size;
    unsigned int raw;

    bool isOwned() const {
        return (raw & 0x80000000) == 0;
    }

    void dealloc() {
        hkThreadMemory::s_instance->deallocateChunk(data, (raw & 0x3FFFFFFF) * 0xC, 0x15);
    }
};

struct Obj {
    void* vtable;
    unsigned short type;
    unsigned short pad;
    int f8;
    Array array;
};

// TODO: cleanup (shape from jj-link/brawl fn_802A990C)
extern "C" Obj* fn_802A990C(Obj* self, int flag) {
    if (self) {
        if (self && self && self && (unsigned)self + 0xC != 0 &&
            self->array.isOwned()) {
            self->array.dealloc();
        }

        if (flag > 0) {
            ((SoHeapAllocator*)hkMemory::s_instance)->free(self, self->type, 0x1D);
        }
    }

    return self;
}
} // namespace jj_fn_802A990C
