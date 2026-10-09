// Havok translation unit hkPhysicsSystem.o (main.dol 0x802E9890-0x802E9B90).
// Functions in address order:
//   0x802E9890    32  finishLoadedObjecthkPhysicsSystem   [map: hkPhysicsSystem__finishLoadedObjecthkPhysicsSystem]
//   0x802E98B0    20  cleanupLoadedObjecthkPhysicsSystem   [map: hkPhysicsSystem__cleanupLoadedObjecthkPhysicsSystem]
//   0x802E98C4    60  getVtablehkPhysicsSystem   [map: hkPhysicsSystem__getVtablehkPhysicsSystem]
//   0x802E9900   576  __dt   [map: hkPhysicsSystem____dt]
//   0x802E9B40    80  __sinit_\hkPhysicsSystem_cpp   [map: hkPhysicsSystemcpp____sinit_]

#include <havok/hkPhysicsSystem.h>
#include <havok/hkRegistry.h>
#include <havok/hkWorldObject.h>
#include <havok/hkThreadMemory.h>

void hkPhysicsSystem::finishLoadedObjecthkPhysicsSystem(void* p) {
    ::new (p) hkPhysicsSystem();
}

void hkPhysicsSystem::cleanupLoadedObjecthkPhysicsSystem(void* p) {
    ((hkPhysicsSystem*)p)->~hkPhysicsSystem();
}

#pragma auto_inline off
const void* hkPhysicsSystem::getVtablehkPhysicsSystem() {
    __attribute__((aligned(16))) char buf[0x44];
    hkPhysicsSystem* p = ::new (buf) hkPhysicsSystem();
    return *(const void**)p;
}

#pragma auto_inline reset

static void hkPhysicsSystemFreeArray(hkArrayBase& a) {
    if ((a.m_capacityAndFlags & hkArrayBase::DONT_DEALLOCATE_FLAG) == 0) {
        hkThreadMemory::s_instance->deallocateChunk(a.m_data, (a.m_capacityAndFlags & hkArrayBase::CAPACITY_MASK) * 4, 0x15);
    }
}

hkPhysicsSystem::~hkPhysicsSystem() {
    int i;
    int n = m_rigidBodies.m_size;
    for (i = 0; i < n; i++) {
        ((hkWorldObject**)m_rigidBodies.m_data)[i]->removeReference();
    }
    n = m_phantoms.m_size;
    for (i = 0; i < n; i++) {
        ((hkWorldObject**)m_phantoms.m_data)[i]->removeReference();
    }
    n = m_constraints.m_size;
    for (i = 0; i < n; i++) {
        hkReferencedObject* c = ((hkReferencedObject**)m_constraints.m_data)[i];
        if (c != 0) {
            c->removeReference();
        }
    }
    n = m_actions.m_size;
    for (i = 0; i < n; i++) {
        hkReferencedObject* a = ((hkReferencedObject**)m_actions.m_data)[i];
        if (a != 0) {
            a->removeReference();
        }
    }
    hkPhysicsSystemFreeArray(m_phantoms);
    hkPhysicsSystemFreeArray(m_actions);
    hkPhysicsSystemFreeArray(m_constraints);
    hkPhysicsSystemFreeArray(m_rigidBodies);
}

static hkTypeInfo hkPhysicsSystemTypeInfo = {
    "hkPhysicsSystem",
    hkPhysicsSystem::finishLoadedObjecthkPhysicsSystem,
    hkPhysicsSystem::cleanupLoadedObjecthkPhysicsSystem,
    hkPhysicsSystem::getVtablehkPhysicsSystem(),
};
