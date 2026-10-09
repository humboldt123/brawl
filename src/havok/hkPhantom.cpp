// Havok translation unit hkPhantom.o (main.dol 0x802E841C-0x802E90B4).
// Functions in address order (method names from the Havok TU map):
//   0x802E841C   124  firePhantomDeleted   [map: hkPhantom__firePhantomDeleted]
//   0x802E8498   236  firePhantomRemoved   [map: hkPhantom__firePhantomRemoved]
//   0x802E8584   236  firePhantomAdded   [map: hkPhantom__firePhantomAdded]
//   0x802E8670   872  updateBroadPhase   [map: hkPhantom__updateBroadPhase]
//   0x802E89D8   140  __dt   [map: hkArray_22hkBroadPhaseHandlePair_____dt]
//   0x802E8A64   188  __dt   [map: hkLocalArray_22hkBroadPhaseHandlePair_____dt]
//   0x802E8B20   108  addPhantomOverlapListener   [map: hkPhantom__addPhantomOverlapListener]
//   0x802E8B8C   120  removePhantomOverlapListener   [map: hkPhantom__removePhantomOverlapListener]
//   0x802E8C04   392  __dt   [map: hkPhantom____dt]
//   0x802E8D8C   180  calcStatistics   [map: hkPhantom__calcStatistics]
//   0x802E8E40   192  deallocateInternalArrays   [map: hkPhantom__deallocateInternalArrays]
//   0x802E8F00   172  addCollisionPair   [map: hkPhantomBroadPhaseListener__addCollisionPair]
//   0x802E8FAC   172  removeCollisionPair   [map: hkPhantomBroadPhaseListener__removeCollisionPair]
//   0x802E9058    92  __dt   [map: hkPhantomBroadPhaseListener____dt]

#include <havok/hkPhantom.h>
#include <havok/hkCollidable.h>
#include <havok/hkThreadMemory.h>

// Pair of broad-phase handles reported by the broad phase (8 bytes).
// HYPOTHESIS: the field names are not recovered; the handles are typed broad-phase handles.
struct hkBroadPhaseHandlePair {
    hkTypedBroadPhaseHandle* m_a; // 0x00
    hkTypedBroadPhaseHandle* m_b; // 0x04
};

// Fires a callback on every listener, last to first.
void hkPhantom::firePhantomDeleted() {
    for (int i = m_phantomListeners.getSize() - 1; i >= 0; i--) {
        hkPhantomListener* listener = m_phantomListeners[i];
        if (listener != 0) {
            listener->phantomDeleted(this);
        }
    }
}

void hkPhantom::firePhantomRemoved() {
    for (int i = m_phantomListeners.getSize() - 1; i >= 0; i--) {
        hkPhantomListener* listener = m_phantomListeners[i];
        if (listener != 0) {
            listener->phantomRemoved(this);
        }
    }
    for (int i = m_phantomListeners.m_size - 1; i >= 0; i--) {
        if (((hkPhantomListener**)m_phantomListeners.m_data)[i] == 0) {
            m_phantomListeners.m_size = m_phantomListeners.m_size - 1;
            for (int j = i; j < m_phantomListeners.m_size; j++) {
                m_phantomListeners[j] = m_phantomListeners[j + 1];
            }
        }
    }
}

void hkPhantom::firePhantomAdded() {
    for (int i = m_phantomListeners.getSize() - 1; i >= 0; i--) {
        hkPhantomListener* listener = m_phantomListeners[i];
        if (listener != 0) {
            listener->phantomAdded(this);
        }
    }
    for (int i = m_phantomListeners.m_size - 1; i >= 0; i--) {
        if (((hkPhantomListener**)m_phantomListeners.m_data)[i] == 0) {
            m_phantomListeners.m_size = m_phantomListeners.m_size - 1;
            for (int j = i; j < m_phantomListeners.m_size; j++) {
                m_phantomListeners[j] = m_phantomListeners[j + 1];
            }
        }
    }
}

hkPhantom::~hkPhantom() {
    firePhantomDeleted();
}

void hkPhantom::addPhantomOverlapListener(hkPhantomOverlapListener* listener) {
    m_overlapListeners.pushBack(listener);
}

void hkPhantom::removePhantomOverlapListener(hkPhantomOverlapListener* listener) {
    int idx = -1;
    for (int i = 0; i < m_overlapListeners.getSize(); i++) {
        if (m_overlapListeners[i] == listener) {
            idx = i;
            break;
        }
    }
    int newSize = m_overlapListeners.m_size - 1;
    m_overlapListeners.m_size = newSize;
    for (int j = idx; j < newSize; j++) {
        m_overlapListeners[j] = m_overlapListeners[j + 1];
    }
}

// Statistics collector interface: only the slot used here (0x10) is known; names are placeholders.
// Same layout as the collector interface in hkWorldObject.cpp.
struct hkStatisticsCollectorIface {
    virtual void unk00();
    virtual void unk04();
    virtual void unk08();
    virtual void unk0C();
    virtual void unk10(const char* name, int alignment, const void* data, int size, int capacity);
};

void hkPhantom::calcStatistics(hkStatisticsCollector* collector) const {
    hkWorldObject::calcStatistics(collector);
    if (m_overlapListeners.mustDeallocate()) {
        ((hkStatisticsCollectorIface*)collector)
            ->unk10("overlapListeners", 4, m_overlapListeners.m_data, m_overlapListeners.m_size * 4,
                    m_overlapListeners.getCapacity() * 4);
    }
    if (m_phantomListeners.mustDeallocate()) {
        ((hkStatisticsCollectorIface*)collector)
            ->unk10("phantomListeners", 4, m_phantomListeners.m_data, m_phantomListeners.m_size * 4,
                    m_phantomListeners.getCapacity() * 4);
    }
}

void hkPhantom::deallocateInternalArrays() {
    if (m_overlapListeners.m_size == 0) {
        if (m_overlapListeners.mustDeallocate()) {
            hkThreadMemory::s_instance->deallocateChunk(m_overlapListeners.m_data,
                                                        m_overlapListeners.getCapacity() * 4, 0x15);
        }
        m_overlapListeners.m_data = 0;
        m_overlapListeners.m_size = 0;
        m_overlapListeners.m_capacityAndFlags =
            (m_overlapListeners.m_capacityAndFlags & hkArrayBase::FORCE_SIGN_FLAG) | hkArrayBase::DONT_DEALLOCATE_FLAG;
    }
    if (m_phantomListeners.m_size == 0) {
        if (m_phantomListeners.mustDeallocate()) {
            hkThreadMemory::s_instance->deallocateChunk(m_phantomListeners.m_data,
                                                        m_phantomListeners.getCapacity() * 4, 0x15);
        }
        m_phantomListeners.m_data = 0;
        m_phantomListeners.m_size = 0;
        m_phantomListeners.m_capacityAndFlags =
            (m_phantomListeners.m_capacityAndFlags & hkArrayBase::FORCE_SIGN_FLAG) | hkArrayBase::DONT_DEALLOCATE_FLAG;
    }
}

// Collidable that owns a broad-phase handle (the handle stores a signed offset back to its collidable).
static hkCollidable* collidableOfHandle(hkTypedBroadPhaseHandle* handle) {
    return (hkCollidable*)((char*)handle + handle->m_ownerOffset);
}

// Owner object of a collidable (the collidable stores a signed offset back to its owner).
static hkPhantom* ownerOfCollidable(hkCollidable* collidable) {
    return (hkPhantom*)((char*)collidable + collidable->m_ownerOffset);
}

// Handle type id of a phantom (HK_BROAD_PHASE_PHANTOM, see hkWorldObject::BroadPhaseType).
enum {
    HK_BROAD_PHASE_PHANTOM_ID = 2,
};

void hkPhantomBroadPhaseListener::addCollisionPair(hkBroadPhaseHandlePair* pair) {
    if (pair->m_a->m_type == HK_BROAD_PHASE_PHANTOM_ID) {
        hkCollidable* collidableA = collidableOfHandle(pair->m_a);
        hkCollidable* collidableB = collidableOfHandle(pair->m_b);
        ownerOfCollidable(collidableA)->addOverlappingCollidable(collidableB);
    }
    if (pair->m_b->m_type == HK_BROAD_PHASE_PHANTOM_ID) {
        hkCollidable* collidableA = collidableOfHandle(pair->m_a);
        hkCollidable* collidableB = collidableOfHandle(pair->m_b);
        ownerOfCollidable(collidableB)->addOverlappingCollidable(collidableA);
    }
}

void hkPhantomBroadPhaseListener::removeCollisionPair(hkBroadPhaseHandlePair* pair) {
    if (pair->m_a->m_type == HK_BROAD_PHASE_PHANTOM_ID) {
        hkCollidable* collidableA = collidableOfHandle(pair->m_a);
        hkCollidable* collidableB = collidableOfHandle(pair->m_b);
        ownerOfCollidable(collidableA)->removeOverlappingCollidable(collidableB);
    }
    if (pair->m_b->m_type == HK_BROAD_PHASE_PHANTOM_ID) {
        hkCollidable* collidableA = collidableOfHandle(pair->m_a);
        hkCollidable* collidableB = collidableOfHandle(pair->m_b);
        ownerOfCollidable(collidableB)->removeOverlappingCollidable(collidableA);
    }
}
