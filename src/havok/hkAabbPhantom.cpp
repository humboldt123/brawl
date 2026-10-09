// Havok translation unit hkAabbPhantom.o (main.dol 0x802E7878-0x802E841C).
// Functions in address order (method names from the Havok TU map):
//   0x802E7878   120  finishLoadedObjecthkAabbPhantom   [map: hkAabbPhantom__finishLoadedObjecthkAabbPhantom]
//   0x802E78F0    20  cleanupLoadedObjecthkAabbPhantom   [map: hkAabbPhantom__cleanupLoadedObjecthkAabbPhantom]
//   0x802E7904   132  getVtablehkAabbPhantom   [map: hkAabbPhantom__getVtablehkAabbPhantom]
//   0x802E7988   216  __ct   [map: hkAabbPhantom____ct]
//   0x802E7A60   160  __dt   [map: hkAabbPhantom____dt]
//   0x802E7B00     8  getType   [map: hkAabbPhantom__getType]
//   0x802E7B08  1316  clone   [map: hkAabbPhantom__clone]
//   0x802E802C    68  calcAabb   [map: hkAabbPhantom__calcAabb]
//   0x802E8070    60  isOverlappingCollidableAdded   [map: hkAabbPhantom__isOverlappingCollidableAdded]
//   0x802E80AC   220  addOverlappingCollidable   [map: hkAabbPhantom__addOverlappingCollidable]
//   0x802E8188   252  removeOverlappingCollidable   [map: hkAabbPhantom__removeOverlappingCollidable]
//   0x802E8284   188  calcStatistics   [map: hkAabbPhantom__calcStatistics]
//   0x802E8340   124  deallocateInternalArrays   [map: hkAabbPhantom__deallocateInternalArrays]
//   0x802E83BC     4  updateShapeCollectionFilter   [map: hkPhantom__updateShapeCollectionFilter]
//   0x802E83C0     8  getMotionState   [map: hkAabbPhantom__getMotionState]
//   0x802E83C8    84  __sinit_\hkAabbPhantom_cpp   [map: hkAabbPhantomcpp____sinit_]

#include <havok/hkAabbPhantom.h>
#include <havok/hkCollidable.h>
#include <havok/hkThreadMemory.h>

hkAabbPhantom::hkAabbPhantom(hkFinishLoadedObjectFlag flag) : hkPhantom(flag) {
}

// Calls the deleting destructor through the vtable with flag -1 (object is not freed).
void hkAabbPhantom::cleanupLoadedObject() {
    ((void (**)(hkAabbPhantom*, int))(*(void**)this))[2](this, -1);
}

hkAabbPhantom::~hkAabbPhantom() {
}

// HYPOTHESIS: type id 0 (same as hkShape's invalid type).
int hkAabbPhantom::getType() const {
    return 0;
}

void hkAabbPhantom::calcAabb(hkAabb* out) const {
    out->m_min = m_aabb.m_min;
    out->m_max = m_aabb.m_max;
}

hkBool hkAabbPhantom::isOverlappingCollidableAdded(hkCollidable* collidable) const {
    for (int i = 0; i < m_overlappingCollidables.getSize(); i++) {
        if (m_overlappingCollidables[i] == collidable) {
            return hkBool(true);
        }
    }
    return hkBool(false);
}

void hkAabbPhantom::addOverlappingCollidable(hkCollidable* collidable) {
    hkPhantomOverlapEvent event;
    event.m_phantom = this;
    event.m_collidable = collidable;
    event.m_handled = false;
    for (int i = m_overlapListeners.getSize() - 1; i >= 0; i--) {
        m_overlapListeners[i]->overlapAdded(&event);
    }
    if (!event.m_handled) {
        m_overlappingCollidables.pushBack(collidable);
    }
}

void hkAabbPhantom::removeOverlappingCollidable(hkCollidable* collidable) {
    int found = -1;
    for (int i = 0; i < m_overlappingCollidables.getSize(); i++) {
        if (m_overlappingCollidables[i] == collidable) {
            found = i;
            break;
        }
    }
    hkPhantomOverlapEvent event;
    event.m_phantom = this;
    event.m_collidable = collidable;
    event.m_handled = found >= 0;
    for (int i = m_overlapListeners.getSize() - 1; i >= 0; i--) {
        m_overlapListeners[i]->overlapRemoved(&event);
    }
    if (found >= 0) {
        // Swap-remove: the last entry takes the removed slot.
        int last = m_overlappingCollidables.m_size - 1;
        m_overlappingCollidables.m_size = last;
        m_overlappingCollidables[found] = m_overlappingCollidables[last];
    }
}

void hkAabbPhantom::deallocateInternalArrays() {
    if (m_overlappingCollidables.m_size == 0) {
        if (m_overlappingCollidables.mustDeallocate()) {
            hkThreadMemory::s_instance->deallocateChunk(m_overlappingCollidables.m_data,
                                                        m_overlappingCollidables.getCapacity() * 4, 0x15);
        }
        m_overlappingCollidables.m_data = 0;
        m_overlappingCollidables.m_size = 0;
        m_overlappingCollidables.m_capacityAndFlags =
            (m_overlappingCollidables.m_capacityAndFlags & hkArrayBase::FORCE_SIGN_FLAG) | hkArrayBase::DONT_DEALLOCATE_FLAG;
    }
    hkPhantom::deallocateInternalArrays();
}

void hkAabbPhantom::updateShapeCollectionFilter() {
}

hkMotion* hkAabbPhantom::getMotionState() const {
    return 0;
}
