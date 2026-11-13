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

#include <havok/hkHeightFieldAgent.h>
#include <havok/hkCollisionInput.h>
#include <havok/hkSymmetricAgentFlipCollectors.h>
#include <havok/hkBvTreeAgent.h>

#include <havok/hkCollisionDispatcher.h>

// Height field x convex pairs in both orders (shape types 18 and 21).
void hkHeightFieldAgent::registerAgent(hkCollisionDispatcher* dispatcher) {
    hkAgentFuncs af;
    af.create = (hkAgentFunc)&hkHeightFieldAgent::createHeightFieldAAgent;
    af.staticGetPenetrations = (hkAgentFunc)hkSymmetricAgentLinearCast_18hkHeightFieldAgent::staticGetPenetrations;
    af.staticGetClosestPoints = (hkAgentFunc)hkSymmetricAgentLinearCast_18hkHeightFieldAgent::staticGetClosestPoints;
    af.staticLinearCast = (hkAgentFunc)hkSymmetricAgentLinearCast_18hkHeightFieldAgent::staticLinearCast;
    af.symmetricA = true;
    af.symmetricB = true;
    dispatcher->registerCollisionAgent(&af, 0x12, 0x15);
    hkAgentFuncs bf;
    bf.create = (hkAgentFunc)&hkHeightFieldAgent::createHeightFieldBAgent;
    bf.staticGetPenetrations = (hkAgentFunc)hkHeightFieldAgent::staticGetPenetrations;
    bf.staticGetClosestPoints = (hkAgentFunc)hkHeightFieldAgent::staticGetClosestPoints;
    bf.staticLinearCast = (hkAgentFunc)hkHeightFieldAgent::staticLinearCast;
    bf.symmetricA = false;
    bf.symmetricB = true;
    dispatcher->registerCollisionAgent(&bf, 0x15, 0x12);
}

// TODO: cleanup (shape from jj-link/brawl fn_802B48CC)
hkCollisionAgent* hkHeightFieldAgent::createHeightFieldBAgent(const hkCdBody& bodyA, const hkCdBody& bodyB,
                                                              hkCollisionInput& input, hkContactMgr* contactMgr) {
    return new hkHeightFieldAgent(bodyA, bodyB, input, contactMgr);
}

// TODO: cleanup (shape from jj-link/brawl fn_802B4948)
hkCollisionAgent* hkHeightFieldAgent::createHeightFieldAAgent(const hkCdBody& bodyA, const hkCdBody& bodyB,
                                                              hkCollisionInput& input, hkContactMgr* contactMgr) {
    return new hkSymmetricAgent_18hkHeightFieldAgent_(bodyB, bodyA, input, contactMgr);
}

#pragma dont_inline on
void hkHeightFieldAgent::getPenetrations(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* target) {
    staticGetPenetrations(a, b, c, target);
}

void hkHeightFieldAgent::getClosestPoints(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* target) {
    staticGetClosestPoints(a, b, c, target);
}

void hkHeightFieldAgent::linearCast(const hkCdBody& a, const hkCdBody& b, hkLinearCastCollisionInput& c, void* target, void* d) {
    staticLinearCast(a, b, c, target, d);
}
#pragma dont_inline reset

void hkSymmetricAgentLinearCast_18hkHeightFieldAgent::getPenetrations(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* target) {
    hkSymmetricAgentFlipBodyCollector wrap(target);
    hkHeightFieldAgent::getPenetrations(b, a, c, &wrap);
}

void hkSymmetricAgentLinearCast_18hkHeightFieldAgent::staticGetPenetrations(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* target) {
    hkSymmetricAgentFlipBodyCollector wrap(target);
    hkHeightFieldAgent::staticGetPenetrations(b, a, c, &wrap);
}

void hkSymmetricAgentLinearCast_18hkHeightFieldAgent::getClosestPoints(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* target) {
    hkSymmetricAgentFlipCollector wrap(target);
    hkHeightFieldAgent::getClosestPoints(b, a, c, &wrap);
}

void hkSymmetricAgentLinearCast_18hkHeightFieldAgent::staticGetClosestPoints(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* target) {
    hkSymmetricAgentFlipCollector wrap(target);
    hkHeightFieldAgent::staticGetClosestPoints(b, a, c, &wrap);
}

void hkSymmetricAgentLinearCast_18hkHeightFieldAgent::updateShapeCollectionFilter(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c) {}

// The linear cast with the bodies swapped: the path is negated and each collector is wrapped in one that turns
// the points back.
void hkSymmetricAgent_18hkHeightFieldAgent_::linearCast(const hkCdBody& a, const hkCdBody& b, hkLinearCastCollisionInput& c, void* target, void* d) {
    const hkLinearCastCollisionInput& input = c;
    hkLinearCastCollisionInput flippedInput = input;
    flippedInput.m_path.setNeg4(input.m_path);
    hkSymmetricAgentFlipCastCollector flipped(input.m_path, target);
    if (d != 0) {
        hkSymmetricAgentFlipCastCollector flippedStart(input.m_path, d);
        hkHeightFieldAgent::linearCast(b, a, flippedInput, &flipped, &flippedStart);
    } else {
        hkHeightFieldAgent::linearCast(b, a, flippedInput, &flipped, 0);
    }
}

// The linear cast with the bodies swapped: the path is negated and each collector is wrapped in one that turns
// the points back.
void hkSymmetricAgentLinearCast_18hkHeightFieldAgent::staticLinearCast(const hkCdBody& a, const hkCdBody& b, hkLinearCastCollisionInput& c, void* target, void* d) {
    const hkLinearCastCollisionInput& input = c;
    hkLinearCastCollisionInput flippedInput = input;
    flippedInput.m_path.setNeg4(input.m_path);
    hkSymmetricAgentFlipCastCollector flipped(input.m_path, target);
    if (d != 0) {
        hkSymmetricAgentFlipCastCollector flippedStart(input.m_path, d);
        hkHeightFieldAgent::staticLinearCast(b, a, flippedInput, &flipped, &flippedStart);
    } else {
        hkHeightFieldAgent::staticLinearCast(b, a, flippedInput, &flipped, 0);
    }
}

// fn_802B4A60: body from https://github.com/jj-link/brawl src/havok/fn_802B4A60.cpp (namespaced; labels as in our symbols).
#include <types.h>
namespace jj_fn_802B4A60 {
class Inner {
public:
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4(unsigned int);
    virtual void v5();
    virtual void v6();
};

class Outer {
public:
    virtual void v0(int);
    virtual void v1();
    virtual void v2();
    char pad[4];
    Inner* m_inner;
    unsigned short* m_table;
    int m_count;
};

// TODO: cleanup (shape from jj-link/brawl fn_802B4A60)
extern "C" void fn_802B4A60(Outer* self) {
    if (self->m_inner != 0) {
        int off;
        int i;
        i = 0;
        off = 0;
        while (i < self->m_count) {
            unsigned int id = *(unsigned short*)((char*)self->m_table + off);
            if (id != 0xFFFF) {
                self->m_inner->v4(id);
            }
            off += 2;
            i++;
        }
    }
    if (self != 0) {
        self->v0(1);
    }
}
} // namespace jj_fn_802B4A60

// fn_802B49CC: body from https://github.com/jj-link/brawl src/havok/fn_802B49CC.cpp (namespaced; memory singletons rebound to our names).
#include <types.h>
#include <havok/hkMemory.h>
#include <havok/hkThreadMemory.h>
#include <havok/hkArray.h>
namespace jj_fn_802B49CC {
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
    u32 size;
    u32 raw;

    bool isOwned() const {
        return (raw & 0x80000000) == 0;
    }

    void dealloc() {
        hkThreadMemory::s_instance->deallocateChunk(data, (raw & 0x3FFFFFFF) * 2, 0x15);
    }
};

// TODO: cleanup (shape from jj-link/brawl fn_802B49CC)
extern "C" Obj* fn_802B49CC(Obj* self, int flag) {
    if (self) {
        if ((u32)self + 0xC != 0 && self->isOwned()) {
            self->dealloc();
        }
        if (flag > 0) {
            ((SoHeapAllocator*)hkMemory::s_instance)->free(self, self->type, 0x1d);
        }
    }
    return self;
}
} // namespace jj_fn_802B49CC

// fn_802B7898: body from https://github.com/jj-link/brawl src/havok/fn_802B7898.cpp (namespaced; memory singletons rebound to our names).
#include <types.h>
#include <havok/hkMemory.h>
#include <havok/hkThreadMemory.h>
#include <havok/hkArray.h>
namespace jj_fn_802B7898 {
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


struct Object {
    void* vtable;
    unsigned short type;
    unsigned short pad;
    u32 _pad;
    void* data;
    u32 size;
    u32 raw;

    bool isOwned() const {
        return (raw & 0x80000000) == 0;
    }

    void dealloc() {
        hkThreadMemory::s_instance->deallocateChunk(data, (raw * 2) & 0x7FFFFFFE, 0x15);
    }
};

// TODO: cleanup (shape from jj-link/brawl fn_802B7898)
extern "C" Object* fn_802B7898(Object* self, int flag) {
    if (self) {
        if (self && self && (u32)self + 0xC && self->isOwned()) {
            self->dealloc();
        }
        if (flag > 0) {
            ((SoHeapAllocator*)hkMemory::s_instance)->free(self, self->type, 0x1D);
        }
    }
    return self;
}
} // namespace jj_fn_802B7898
