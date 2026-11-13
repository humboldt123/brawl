// Havok translation unit hkConstrainedSystemFilter.o (main.dol 0x8032BD74-0x8032C304).
// Functions in address order (method names from the Havok TU map). Only the functions listed below are
// written so far; the four isCollisionEnabled overloads are not done yet.
//   0x8032BDE4    72  finishLoadedObjecthkConstrainedSystemFilter   [map: hkConstrainedSystemFilter__finishLoadedObjecthkConstrainedSystemFilter]
//   0x8032BE2C    20  cleanupLoadedObjecthkConstrainedSystemFilter   [map: hkConstrainedSystemFilter__cleanupLoadedObjecthkConstrainedSystemFilter]
//   0x8032BE40   100  getVtablehkConstrainedSystemFilter   [map: hkConstrainedSystemFilter__getVtablehkConstrainedSystemFilter]
//   0x8032BEA4   100  __ct   [map: hkConstrainedSystemFilter____ct]
//   0x8032BF08   228  __dt   [map: hkConstrainedSystemFilter____dt]
//   0x8032C24C   100  constraintAddedCallback   [map: hkConstrainedSystemFilter__constraintAddedCallback]
//   0x8032C2B0     4  constraintRemovedCallback   [map: hkConstrainedSystemFilter__constraintRemovedCallback]
//   0x8032C2B4    80  __sinit_\hkConstrainedSystemFilter_cpp   [map: hkConstrainedSystemFiltercpp____sinit_]

#include <havok/hkConstrainedSystemFilter.h>
#include <havok/hkRegistry.h>
#include <havok/hkConstraintInstance.h>
#include <havok/hkConstraintData.h>
#include <havok/hkEntity.h>
#include <havok/hkAgentNnMachine.h>
#include <havok/hkWorldAgentUtil.h>

void hkConstrainedSystemFilter::finishLoadedObjecthkConstrainedSystemFilter(void* p) {
    hkFinishLoadedObjectFlag flag;
    flag.m_finishing = 1;
    ::new (p) hkConstrainedSystemFilter(flag);
}

void hkConstrainedSystemFilter::cleanupLoadedObjecthkConstrainedSystemFilter(void* p) {
    ((hkConstrainedSystemFilter*)p)->~hkConstrainedSystemFilter();
}

#pragma auto_inline off
const void* hkConstrainedSystemFilter::getVtablehkConstrainedSystemFilter() {
    // Temporary object on the stack; only its vtable pointer is read back.
    __attribute__((aligned(16))) char buf[0x20];
    hkFinishLoadedObjectFlag flag;
    flag.m_finishing = 0;
    ::new (buf) hkConstrainedSystemFilter(flag);
    return *(const void**)buf;
}
#pragma auto_inline reset

hkConstrainedSystemFilter::hkConstrainedSystemFilter(hkCollisionFilter* childFilter) {
    m_childFilter = childFilter;
    if (childFilter != 0) {
        childFilter->addReference();
    }
}

hkConstrainedSystemFilter::~hkConstrainedSystemFilter() {
    if (m_childFilter != 0) {
        m_childFilter->removeReference();
    }
}

// A new non-contact constraint removes the agent between its two bodies (and the agent's TOI events).
void hkConstrainedSystemFilter::constraintAddedCallback(const void* constraint) {
    hkConstraintInstance* instance = (hkConstraintInstance*)constraint;
    if (instance->getData()->getType() != 11) {
        hkAgentNnEntry* entry = hkAgentNnMachine_FindAgent(&instance->m_entities[0]->m_collidable,
                                                           &instance->m_entities[1]->m_collidable);
        if (entry != 0) {
            hkWorldAgentUtil::removeAgentAndItsToiEvents(entry);
        }
    }
}

void hkConstrainedSystemFilter::constraintRemovedCallback(const void* constraint) {}

hkBool hkConstrainedSystemFilter::isCollisionEnabled(const hkCollisionInput& input, const hkCdBody& a,
                                                     const hkCdBody& b, const hkShapeCollection& bContainer,
                                                     u32 bKey) const {
    hkBool enabled = true;
    hkCollisionFilter* child = m_childFilter;
    if (child != 0) {
        if (child->isCollisionEnabled(input, a, b, bContainer, bKey)) {
        } else {
            enabled = false;
        }
    }
    return enabled;
}

hkBool hkConstrainedSystemFilter::isCollisionEnabled(const hkShapeRayCastInput& aInput,
                                                     const hkShapeCollection& bContainer, u32 bKey) const {
    hkBool enabled = true;
    hkCollisionFilter* child = m_childFilter;
    if (child != 0) {
        if (child->isCollisionEnabled(aInput, bContainer, bKey)) {
        } else {
            enabled = false;
        }
    }
    return enabled;
}

hkBool hkConstrainedSystemFilter::isCollisionEnabled(const hkWorldRayCastInput& a,
                                                     const hkCollidable& collidableB) const {
    hkBool enabled = true;
    hkCollisionFilter* child = m_childFilter;
    if (child != 0) {
        if (child->isCollisionEnabled(a, collidableB)) {
        } else {
            enabled = false;
        }
    }
    return enabled;
}

static hkStaticTypeInfo hkConstrainedSystemFilterTypeInfo("hkConstrainedSystemFilter", hkConstrainedSystemFilter::finishLoadedObjecthkConstrainedSystemFilter,
                                                    hkConstrainedSystemFilter::cleanupLoadedObjecthkConstrainedSystemFilter,
                                                    hkConstrainedSystemFilter::getVtablehkConstrainedSystemFilter());

// fn_8032BD74: body from https://github.com/jj-link/brawl src/havok/fn_8032BD74.cpp (namespaced; labels as in our symbols).
#include <types.h>
namespace jj_fn_8032BD74 {
extern "C" {
extern "C" const float lbl_805A4840;
extern "C" const float lbl_805A4844;
}
// TODO: cleanup (shape from jj-link/brawl fn_8032BD74)
extern "C" void fn_8032BD74(float *p)
{
    float arr[4] __attribute__((aligned(16)));
    float zero = lbl_805A4840;
    float one = lbl_805A4844;
    p[0] = zero;
    p[5] = zero;
    p[10] = zero;
    p[1] = zero;
    p[2] = zero;
    p[3] = zero;
    p[4] = zero;
    p[6] = zero;
    p[7] = zero;
    p[8] = zero;
    p[9] = zero;
    p[11] = zero;
    arr[3] = zero;
    arr[2] = zero;
    arr[1] = zero;
    arr[0] = zero;
    p[0] = one;
    p[5] = one;
    p[10] = one;
}
} // namespace jj_fn_8032BD74
