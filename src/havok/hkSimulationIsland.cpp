// Havok translation unit hkSimulationIsland.o (main.dol 0x802E9BFC-0x802EA88C).
// Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x802E9BFC   252  __ct   [map: hkSimulationIsland____ct]
//   0x802E9CF8   260  __dt   [map: hkSimulationIsland____dt]
//   0x802E9DFC   120  internalAddEntity   [map: hkSimulationIsland__internalAddEntity]
//   0x802E9E74   104  internalRemoveEntity   [map: hkSimulationIsland__internalRemoveEntity]
//   0x802E9EDC   796  calcStatistics   [map: hkSimulationIsland__calcStatistics]
//   0x802EA1F8   112  addAction   [map: hkSimulationIsland__addAction]
//   0x802EA268   104  removeAction   [map: hkSimulationIsland__removeAction]
//   0x802EA2D0   864  isFullyConnected   [map: hkSimulationIsland__isFullyConnected]
//   0x802EA630   476  shouldDeactivate   [map: hkSimulationIsland__shouldDeactivate]
//   0x802EA80C    12  addConstraintToCriticalLockedIsland   [map: hkSimulationIsland__addConstraintToCriticalLockedIsland]
//   0x802EA818    12  removeConstraintFromCriticalLockedIsland   [map: hkSimulationIsland__removeConstraintFromCriticalLockedIsland]
//   0x802EA824    96  mergeConstraintInfo   [map: hkSimulationIsland__mergeConstraintInfo]
//   0x802EA884     4  markForWrite   [map: hkSimulationIsland__markForWrite]
//   0x802EA888     4  checkAccessRw   [map: hkConstraintOwner__checkAccessRw]

#include <havok/hkSimulationIsland.h>
#include <havok/hkEntity.h>
#include <havok/hkWorldOperationUtil.h>

// Adds an entity to the island: the entity keeps the island pointer and its index in the island array.
void hkSimulationIsland::internalAddEntity(hkEntity* entity) {
    entity->m_simulationIsland = this;
    entity->m_storageIndex = m_entities.m_size;
    m_entities.pushBack(entity);
}

// Removes an entity by moving the last entity into its slot.
void hkSimulationIsland::internalRemoveEntity(hkEntity* entity) {
    int last = m_entities.m_size - 1;
    hkEntity* moved = m_entities[last];
    m_entities[entity->m_storageIndex] = moved;
    moved->m_storageIndex = entity->m_storageIndex;
    m_entities.m_size = last;
    entity->m_simulationIsland = 0;
    entity->m_storageIndex = 0xFFFF;
    // HYPOTHESIS: bits 0xC0 of 0x26 hold the island's state; "entity removed" sets it to 1.
    u8 state = 1;
    m_flags26 = (m_flags26 & ~0xC0) | (state << 6);
}

void hkSimulationIsland::addAction(hkAction* action) {
    m_actions.pushBack(action);
    *(hkSimulationIsland**)((char*)action + 0x0C) = this;
}

// Clears the matching action slot (nulls are compacted later) and the action's island pointer.
void hkSimulationIsland::removeAction(hkAction* action) {
    int index = -1;
    for (int i = 0; i < m_actions.getSize(); i++) {
        if (m_actions[i] == action) {
            index = i;
            break;
        }
    }
    m_actions[index] = 0;
    *(hkSimulationIsland**)((char*)action + 0x0C) = 0;
    // HYPOTHESIS: bit fields 0xC0 and 0x30 of 0x26, set as in internalRemoveEntity.
    m_flags26 = (m_flags26 & ~0xC0) | 0x40;
    m_flags26 = (m_flags26 & ~0x30) | 0x10;
}

// Forwards to the world operation with the world taken from the constraint's owner (world at +0x08 of the
// owner pointer stored at +0x14). HYPOTHESIS: field meanings of the constraint record.
void hkSimulationIsland::addConstraintToCriticalLockedIsland(hkConstraintInstance* constraint) {
    hkWorld* world = *(hkWorld**)(*(u8**)((u8*)constraint + 0x14) + 0x08);
    hkWorldOperationUtil::addConstraintToCriticalLockedIsland(world, constraint);
}

void hkSimulationIsland::removeConstraintFromCriticalLockedIsland(hkConstraintInstance* constraint) {
    hkWorld* world = *(hkWorld**)(*(u8**)((u8*)constraint + 0x14) + 0x08);
    hkWorldOperationUtil::removeConstraintFromCriticalLockedIsland(world, constraint);
}

// Merges the constraint info of another island into this one: the first word takes the larger of the two
// first words, then the larger of that and the other island's second word; the remaining three words are summed.
// HYPOTHESIS: field meanings of hkConstraintInfo.
void hkSimulationIsland::mergeConstraintInfo(const hkSimulationIsland* other) {
    int* self = (int*)&m_constraintInfo;
    const int* o = (const int*)&other->m_constraintInfo;
    int m = (self[0] > o[0]) ? self[0] : o[0];
    self[0] = m;
    int n = (m > o[1]) ? m : o[1];
    self[0] = n;
    self[1] += o[1];
    self[2] += o[2];
    self[3] += o[3];
}

// No-op in this build (the target function body is a bare return).
void hkSimulationIsland::markForWrite() {
}
