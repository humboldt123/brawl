#include <havok/hkGroupFilter.h>
#include <havok/hkRegistry.h>
#include <havok/hkShape.h>
#include <havok/hkShapeCollection.h>
#include <havok/hkBvTreeShape.h>
#include <havok/hkCollisionDispatcher.h>
#include <havok/hkShapeType.h>

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
        m_collisionLookupTable[i] = 0xFFFFFFFF;
    }
    m_nextFreeSystemGroup = 0;
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
    u32 row = m_collisionLookupTable[infoA & 0x1F];
    return (row & (1u << (infoB & 0x1F))) != 0;
}
#pragma auto_inline reset

hkBool hkGroupFilter::isCollisionEnabled(const hkCollidable& a, const hkCollidable& b) const {
    return isCollisionEnabled(a.m_broadPhaseHandle.m_collisionFilterInfo, b.m_broadPhaseHandle.m_collisionFilterInfo);
}

// The child's own info word comes from the container. The other body's info is its root collidable's when it is a
// root itself; otherwise the first parent that is a collection, a bv tree (its collection) or a multi-sphere decides.
hkBool hkGroupFilter::isCollisionEnabled(const hkCollisionInput& input, const hkCdBody& a, const hkCdBody& b,
                                         const hkShapeCollection& bContainer, u32 bKey) const {
    u32 infoB = bContainer.getCollisionFilterInfo(bKey);
    u32 infoA;
    if (a.m_shapeKey == 0xFFFFFFFF) {
        const hkCdBody* root = &a;
        while (root->m_parent != 0) {
            root = root->m_parent;
        }
        infoA = ((const hkCollidable*)root)->m_broadPhaseHandle.m_collisionFilterInfo;
    } else {
        const hkCdBody* p = a.m_parent;
        while (true) {
            int type = p->m_shape->getType();
            if (input.m_dispatcher->hasAlternateType(type, HK_SHAPE_COLLECTION)) {
                infoA = ((const hkShapeCollection*)p->m_shape)->getCollisionFilterInfo(a.m_shapeKey);
                break;
            }
            if (input.m_dispatcher->hasAlternateType(type, HK_SHAPE_BV_TREE)) {
                infoA = ((const hkBvTreeShape*)p->m_shape)->getShapeCollection()->getCollisionFilterInfo(a.m_shapeKey);
                break;
            }
            if (input.m_dispatcher->hasAlternateType(type, HK_SHAPE_MULTI_SPHERE)) {
                const hkCdBody* root = &a;
                while (root->m_parent != 0) {
                    root = root->m_parent;
                }
                infoA = ((const hkCollidable*)root)->m_broadPhaseHandle.m_collisionFilterInfo;
                break;
            }
            p = p->m_parent;
            if (p == 0) {
                infoA = 0;
                break;
            }
        }
    }
    return isCollisionEnabled(infoA, infoB);
}

// The ray input carries its info word at 0x20.
hkBool hkGroupFilter::isCollisionEnabled(const hkShapeRayCastInput& aInput, const hkShapeCollection& bContainer,
                                         u32 bKey) const {
    u32 infoB = bContainer.getCollisionFilterInfo(bKey);
    u32 infoA = *(const u32*)((const char*)&aInput + 0x20);
    return isCollisionEnabled(infoA, infoB);
}

// The world ray input carries its info word at 0x24.
hkBool hkGroupFilter::isCollisionEnabled(const hkWorldRayCastInput& a, const hkCollidable& collidableB) const {
    return isCollisionEnabled(*(const u32*)((const char*)&a + 0x24), collidableB.m_broadPhaseHandle.m_collisionFilterInfo);
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
            row->m_collisionLookupTable[0] |= layersB;
        }
        if (layersB & bit) {
            row->m_collisionLookupTable[0] |= layersA;
        }
        layer++;
        bit = one << layer;
        if (layersA & bit) {
            row->m_collisionLookupTable[1] |= layersB;
        }
        if (layersB & bit) {
            row->m_collisionLookupTable[1] |= layersA;
        }
        layer++;
        bit = one << layer;
        if (layersA & bit) {
            row->m_collisionLookupTable[2] |= layersB;
        }
        if (layersB & bit) {
            row->m_collisionLookupTable[2] |= layersA;
        }
        layer++;
        bit = one << layer;
        if (layersA & bit) {
            row->m_collisionLookupTable[3] |= layersB;
        }
        if (layersB & bit) {
            row->m_collisionLookupTable[3] |= layersA;
        }
        layer++;
        row = (hkGroupFilter*)((char*)row + 0x10);
    }
}

void hkGroupFilter::disableCollisionsBetween(u32 layerA, u32 layerB) {
    m_collisionLookupTable[layerA] &= ~(1u << layerB);
    m_collisionLookupTable[layerB] &= ~(1u << layerA);
}

hkBool hkGroupFilter::dummyUnused() const {
    return false;
}

static hkStaticTypeInfo hkGroupFilterTypeInfo("hkGroupFilter", hkGroupFilter::finishLoadedObjecthkGroupFilter,
                                                    hkGroupFilter::cleanupLoadedObjecthkGroupFilter,
                                                    hkGroupFilter::getVtablehkGroupFilter());
