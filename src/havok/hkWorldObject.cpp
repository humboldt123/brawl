// Havok translation unit hkWorldObject.o (main.dol 0x802F0438-0x802F06A8).
// Functions in address order:
//   0x802F0438   116  __ct   [map: hkWorldObject____ct]
//   0x802F04AC   188  __ct   [map: hkWorldObject____ct1]
//   0x802F0568     8  setShape   [map: hkWorldObject__setShape]
//   0x802F0570   220  calcStatistics   [map: hkWorldObject__calcStatistics]
//   0x802F064C    28  addReference   [map: hkWorldObject__addReference]
//   0x802F0668    64  removeReference   [map: hkWorldObject__removeReference]

#include <havok/hkWorldObject.h>
#include <havok/hkShape.h>

hkWorldObject::hkWorldObject(hkFinishLoadedObjectFlag flag) : m_collidable(flag), m_properties(flag) {
    if (flag.m_finishing != 0) {
        m_collidable.m_ownerOffset = (int)((u8*)this - (u8*)&m_collidable);
    }
}

// The entity constructor passes BROAD_PHASE_ENTITY.
hkWorldObject::hkWorldObject(hkShape* shape, BroadPhaseType type)
    : m_world(0), m_userData(0), m_name(0), m_collidable(shape, type) {
    m_collidable.m_ownerOffset = (int)((u8*)this - (u8*)&m_collidable);
    if (shape != 0) {
        shape->addReference();
    }
}

bool hkWorldObject::setShape(hkShape* shape) {
    return true;
}

// Statistics collector interface: only the slots used here (0x10 and 0x14) are known; names are placeholders.
struct hkStatisticsCollectorIface {
    virtual void unk00();
    virtual void unk04();
    virtual void unk08();
    virtual void unk0C();
    virtual void unk10(const char* name, int alignment, const void* data, int size, int capacity);
    virtual void unk14(const char* name, int count, const void* object);
};

void hkWorldObject::calcStatistics(hkStatisticsCollector* collector) const {
    ((hkStatisticsCollectorIface*)collector)->unk14("Shape", 1, m_collidable.m_shape);
    if ((m_collidable.m_collisionEntries.m_capacityAndFlags & hkArrayBase::DONT_DEALLOCATE_FLAG) == 0) {
        ((hkStatisticsCollectorIface*)collector)->unk10("collidable", 8, m_collidable.m_collisionEntries.m_data,
                         m_collidable.m_collisionEntries.m_size * 8,
                         (m_collidable.m_collisionEntries.m_capacityAndFlags & hkArrayBase::CAPACITY_MASK) * 8);
    }
    if ((m_properties.m_capacityAndFlags & hkArrayBase::DONT_DEALLOCATE_FLAG) == 0) {
        ((hkStatisticsCollectorIface*)collector)->unk10("properties", 4, m_properties.m_data, m_properties.m_size * 16,
                         (m_properties.m_capacityAndFlags & hkArrayBase::CAPACITY_MASK) * 16);
    }
}

void hkWorldObject::addReference() {
    if (m_memSizeAndFlags != 0) {
        m_referenceCount++;
    }
}

void hkWorldObject::removeReference() {
    if (m_memSizeAndFlags != 0) {
        if (--m_referenceCount == 0) {
            delete this;
        }
    }
}
