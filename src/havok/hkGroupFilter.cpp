#include <havok/hkGroupFilter.h>
#include <havok/hkRegistry.h>
#include <havok/hkShape.h>

// HYPOTHESIS: slot 0x44 of the object held by a body (shape or shape container) maps a shape key to a
// collision info word. Called through the vtable with the same slot as the original code.
typedef u32 (*ShapeKeyInfoFn)(const void* self, u32 shapeKey);
typedef u32 (*ObjectInfoFn)(const void* self, const void* arg);

static inline u32 callShapeKeyInfo(const void* obj, u32 shapeKey) {
    return ((ShapeKeyInfoFn)((void* const*)*(void* const*)obj)[0x44 / 4])(obj, shapeKey);
}

static inline u32 callObjectInfo(const void* obj, const void* arg) {
    return ((ObjectInfoFn)((void* const*)*(void* const*)obj)[0x44 / 4])(obj, arg);
}

void hkGroupFilter::finishLoadedObjecthkGroupFilter(void* p) {
    hkFinishLoadedObjectFlag flag;
    flag.m_finishing = 1;
    ::new (p) hkGroupFilter(flag);
}

void hkGroupFilter::cleanupLoadedObjecthkGroupFilter(void* p) {
    ((hkGroupFilter*)p)->~hkGroupFilter();
}

#pragma auto_inline off
const void* hkGroupFilter::getVtablehkGroupFilter() {
    // Temporary object on the stack; only its vtable pointer is read back.
    __attribute__((aligned(16))) char buf[0x9C];
    hkFinishLoadedObjectFlag flag = {0};
    ::new (buf) hkGroupFilter(flag);
    return *(const void**)buf;
}
#pragma auto_inline reset

hkGroupFilter::hkGroupFilter() {
    for (int i = 0; i < 32; i++) {
        collisionLookupTable[i] = 0xFFFFFFFF;
    }
    nextFreeSystemGroup = 0;
}

hkGroupFilter::~hkGroupFilter() {}

#pragma auto_inline off
hkBool hkGroupFilter::isCollisionEnabled(u32 infoA, u32 infoB) const {
    if ((((infoA ^ infoB) & 0xFFFF0000) == 0) && ((infoA & 0xFFFF0000) != 0)) {
        // Same non-zero system group: the subsystem fields decide.
        if ((int)((infoA >> 5) & 0x1F) == (int)((infoB >> 10) & 0x1F)) {
            return false;
        }
        if ((int)((infoB >> 5) & 0x1F) == (int)((infoA >> 10) & 0x1F)) {
            return false;
        }
        return true;
    }
    u32 row = collisionLookupTable[infoA & 0x1F];
    return (row & (1u << (infoB & 0x1F))) != 0;
}
#pragma auto_inline reset

hkBool hkGroupFilter::isCollisionEnabled1(const hkCollidable* a, const hkCollidable* b) const {
    return isCollisionEnabled(a->m_broadPhaseHandle.m_collisionFilterInfo,
                              b->m_broadPhaseHandle.m_collisionFilterInfo);
}

// a: holder of a pointer to a table of per-shape-type flags (flags at +0x10C); b: body with parent chain.
// Bit 2 uses the shape's slot 0x44, bit 3 the slot 0x44 of the shape's sub-object at +0x10, bit 11 walks
// up to the root body. A null chain falls back to a zero info word.
hkBool hkGroupFilter::isCollisionEnabled2(const void* a, const hkCollidable* b, const void* unk6,
                                          const void* c, const void* d) const {
    u32 infoB = callObjectInfo(c, d);
    if (b->m_shapeKey == 0xFFFFFFFF) {
        const hkCdBody* root = b;
        while (root->m_parent != 0) {
            root = root->m_parent;
        }
        return isCollisionEnabled(((const hkCollidable*)root)->m_broadPhaseHandle.m_collisionFilterInfo,
                                  infoB);
    }
    const u8* table = *(const u8* const*)a;
    const hkCdBody* body = b->m_parent;
    do {
        const hkShape* shape = (const hkShape*)body->m_shape;
        u32 flags = *(const u32*)(table + 0x10C + shape->getType() * 4);
        if (flags & 0x4) {
            return isCollisionEnabled(callShapeKeyInfo(body->m_shape, b->m_shapeKey), infoB);
        }
        if (flags & 0x8) {
            void* sub = *(void* const*)((const char*)body->m_shape + 0x10);
            return isCollisionEnabled(callShapeKeyInfo(sub, b->m_shapeKey), infoB);
        }
        if (flags & 0x800) {
            const hkCdBody* root = b;
            while (root->m_parent != 0) {
                root = root->m_parent;
            }
            return isCollisionEnabled(((const hkCollidable*)root)->m_broadPhaseHandle.m_collisionFilterInfo,
                                      infoB);
        }
        body = body->m_parent;
    } while (body != 0);
    return isCollisionEnabled(0u, infoB);
}

// a carries the body's info word at 0x20; c is passed to b's slot 0x44 to produce the other info word.
hkBool hkGroupFilter::isCollisionEnabled3(const void* a, const void* b, const void* c) const {
    u32 infoB = callObjectInfo(b, c);
    u32 infoA = *(const u32*)((const char*)a + 0x20);
    return isCollisionEnabled(infoA, infoB);
}

// a carries its info word at 0x24 (HYPOTHESIS: a different body layout from hkCollidable).
hkBool hkGroupFilter::isCollisionEnabled4(const void* a, const hkCollidable* b) const {
    return isCollisionEnabled(*(const u32*)((const char*)a + 0x24),
                              b->m_broadPhaseHandle.m_collisionFilterInfo);
}

void hkGroupFilter::enableCollisionsUsingBitfield(u32 layersA, u32 layersB) {
    // Four layers per step: each pair of tests ors the other mask into the row for that layer.
    // MATCH-ONLY: the row pointer walks over the object itself (table at +0x1C, 0x10 bytes per step).
    hkGroupFilter* row = this;
    u32 one = 1;
    int layer = 0;
    for (int i = 0; i < 8; i++) {
        u32 bit = one << layer;
        if (layersA & bit) {
            row->collisionLookupTable[0] |= layersB;
        }
        if (layersB & bit) {
            row->collisionLookupTable[0] |= layersA;
        }
        layer++;
        bit = one << layer;
        if (layersA & bit) {
            row->collisionLookupTable[1] |= layersB;
        }
        if (layersB & bit) {
            row->collisionLookupTable[1] |= layersA;
        }
        layer++;
        bit = one << layer;
        if (layersA & bit) {
            row->collisionLookupTable[2] |= layersB;
        }
        if (layersB & bit) {
            row->collisionLookupTable[2] |= layersA;
        }
        layer++;
        bit = one << layer;
        if (layersA & bit) {
            row->collisionLookupTable[3] |= layersB;
        }
        if (layersB & bit) {
            row->collisionLookupTable[3] |= layersA;
        }
        layer++;
        row = (hkGroupFilter*)((char*)row + 0x10);
    }
}

void hkGroupFilter::disableCollisionsBetween(u32 layerA, u32 layerB) {
    collisionLookupTable[layerA] &= ~(1u << layerB);
    collisionLookupTable[layerB] &= ~(1u << layerA);
}

hkBool hkGroupFilter::dummyUnused() const {
    return false;
}

static hkTypeInfo hkGroupFilterTypeInfo = {
    "hkGroupFilter",
    hkGroupFilter::finishLoadedObjecthkGroupFilter,
    hkGroupFilter::cleanupLoadedObjecthkGroupFilter,
    hkGroupFilter::getVtablehkGroupFilter(),
};
