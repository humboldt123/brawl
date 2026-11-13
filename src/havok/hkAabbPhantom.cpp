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
#include <havok/hkMonitorStream.h>

hkAabbPhantom::hkAabbPhantom(const hkAabb& aabb, u32 collisionFilterInfo) : hkPhantom(0) {
    m_aabb = aabb;
    m_collidable.m_broadPhaseHandle.m_collisionFilterInfo = collisionFilterInfo;
}

// The arrays are rebuilt empty by their default constructors (hkPhantom's two, then this class's one after the vtable
// store).
hkAabbPhantom::hkAabbPhantom(hkFinishLoadedObjectFlag flag) : hkPhantom(flag) {}

// Calls the deleting destructor through the vtable with flag -1 (object is not freed).
void hkAabbPhantom::finishLoadedObjecthkAabbPhantom(void* p) {
    hkFinishLoadedObjectFlag flag;
    flag.m_finishing = 1;
    new (p) hkAabbPhantom(flag);
}

void hkAabbPhantom::cleanupLoadedObjecthkAabbPhantom(void* p) {
    ((hkAabbPhantom*)p)->~hkAabbPhantom();
}

static hkPhantomTypeInfo s_hkAabbPhantomTypeInfo("hkAabbPhantom", hkAabbPhantom::finishLoadedObjecthkAabbPhantom,
                                                 hkAabbPhantom::cleanupLoadedObjecthkAabbPhantom,
                                                 hkAabbPhantom::getVtablehkAabbPhantom());

const void* hkAabbPhantom::getVtablehkAabbPhantom() {
    hkFinishLoadedObjectFlag flag;
    flag.m_finishing = 0;
    hkVector4 buf[10]; // 0xA0 bytes of 16-byte-aligned storage for the placed object
    new (buf) hkAabbPhantom(flag);
    return *(const void**)buf;
}


hkAabbPhantom::~hkAabbPhantom() {
}

// Not in main.dol (the linker stripped it); only its timer literals ("TtrcPhantom", "Et") remain, at the start of
// this unit's string pool. HYPOTHESIS: body (Havok's setAabb moves the broad-phase entry when in a world).
void hkAabbPhantom::setAabb(const hkAabb& aabb) {
    HK_TIMER_BEGIN("rcPhantom");
    if (m_world) {
        updateBroadPhase((void*)&aabb);
    }
    m_aabb = aabb;
    HK_TIMER_END();
}

// The listener arrays (hkPhantom) and the property array (hkWorldObject) are copied with hkArray::operator=.
hkPhantom* hkAabbPhantom::clone() const {
    hkAabbPhantom* phantom = new hkAabbPhantom(m_aabb, m_collidable.m_broadPhaseHandle.m_collisionFilterInfo);
    phantom->m_overlapListeners = m_overlapListeners;
    phantom->m_phantomListeners = m_phantomListeners;
    phantom->m_properties = m_properties;
    return phantom;
}

// HYPOTHESIS: type id 0 (same as hkShape's invalid type).
int hkAabbPhantom::getType() const {
    return 0;
}

void hkAabbPhantom::calcAabb(hkAabb& out) {
    out = m_aabb;
}

hkBool hkAabbPhantom::isOverlappingCollidableAdded(hkCollidable* collidable) const {
    for (int i = 0; i < m_overlappingCollidables.getSize(); i++) {
        if (m_overlappingCollidables[i] == collidable) {
            return hkBool(true);
        }
    }
    return hkBool(false);
}

// The listener loops are inline helpers (HYPOTHESIS: hkPhantom's fireCollidableAdded/Removed): written in the callers
// the loop counter and the element offset swap registers.
static inline int hkAabbPhantom_fireCollidableAdded(hkAabbPhantom* phantom, hkCollidable* collidable) {
    hkCollidableAddedEvent event;
    event.m_collidable = collidable;
    event.m_phantom = phantom;
    event.m_collidableResult = 0;
    for (int i = phantom->m_overlapListeners.getSize() - 1; i >= 0; i--) {
        if (phantom->m_overlapListeners[i] != 0) {
            phantom->m_overlapListeners[i]->collidableAddedCallback(&event);
        }
    }
    return event.m_collidableResult;
}

static inline void hkAabbPhantom_fireCollidableRemoved(hkAabbPhantom* phantom, hkCollidable* collidable, bool wasAdded) {
    hkCollidableRemovedEvent event;
    event.m_collidable = collidable;
    event.m_phantom = phantom;
    event.m_collidableWasAdded = wasAdded;
    for (int i = phantom->m_overlapListeners.getSize() - 1; i >= 0; i--) {
        if (phantom->m_overlapListeners[i] != 0) {
            phantom->m_overlapListeners[i]->collidableRemovedCallback(&event);
        }
    }
}

void hkAabbPhantom::addOverlappingCollidable(hkCollidable* collidable) {
    if (hkAabbPhantom_fireCollidableAdded(this, collidable) == 0) {
        m_overlappingCollidables.pushBack(collidable);
    }
}

void hkAabbPhantom::removeOverlappingCollidable(hkCollidable* collidable) {
    int found = m_overlappingCollidables.indexOf(collidable);
    hkAabbPhantom_fireCollidableRemoved(this, collidable, found >= 0);
    if (found >= 0) {
        m_overlappingCollidables.removeAt(found);
    }
}

// Statistics collector slots used here (local view, as in hkBvTreeAgent.cpp). HYPOTHESIS: names.
struct hkAabbPhantomStatisticsCollector {
    virtual void unk08();
    virtual void beginObject(const char* name, int type, const void* object);                    // 0x0C
    virtual void addArray(const char* name, int type, const void* data, int size, int capacity); // 0x10
    virtual void unk14();
    virtual void unk18();
    virtual void unk1C();
    virtual void endObject();                                                                     // 0x20
};

void hkAabbPhantom::calcStatistics(hkStatisticsCollector* collector) const {
    hkAabbPhantomStatisticsCollector* c = (hkAabbPhantomStatisticsCollector*)collector;
    c->beginObject("AabbPhantom", 2, this);
    hkPhantom::calcStatistics(collector);
    const char* name = "OvrlpCollPtr";
    if (m_overlappingCollidables.mustDeallocate()) {
        c->addArray(name, 8, m_overlappingCollidables.m_data, m_overlappingCollidables.m_size * 4,
                    m_overlappingCollidables.getCapacity() * 4);
    }
    c->endObject();
}

void hkAabbPhantom::deallocateInternalArrays() {
    if (m_overlappingCollidables.m_size == 0) {
        if (m_overlappingCollidables.mustDeallocate()) {
            hkThreadMemory::s_instance->deallocateChunk(m_overlappingCollidables.m_data,
                                                        (m_overlappingCollidables.m_capacityAndFlags & hkArrayBase::CAPACITY_MASK) * sizeof(hkCollidable*), 0x15);
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
