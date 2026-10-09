#pragma once

#include <havok/hkBase.h>
#include <havok/hkArray.h>
#include <havok/hkConstraintData.h>

struct hkEntity;
struct hkAction; // HYPOTHESIS: action object; the island stores a back pointer at 0x0C (not in a header yet)
struct hkStatisticsCollector;
struct hkConstraintInstance;

// Simulation island (layout partly recovered; base and tail not recovered). Fields used by the island code:
// 0x26 bit field (bits 0xC0 and 0x30, written as flags by internalRemoveEntity/removeAction),
// 0x38 entity array (pointers), 0x60 action array (pointers).
struct hkSimulationIsland : hkReferencedObject {
    hkConstraintInfo m_constraintInfo; // 0x08 merged constraint info (fields 0x08..0x14, see mergeConstraintInfo)
    u8 m_unk20[0x06];                // 0x20 (not recovered; u16 index at 0x20, u16 dirty-list index at 0x22)
    u8 m_flags26;                    // 0x26 bit field, see above
    u8 m_unk27[0x11];                // 0x27 (not recovered)
    hkArray<hkEntity*> m_entities;   // 0x38
    u8 m_unk44[0x1C];                // 0x44 (not recovered)
    hkArray<hkAction*> m_actions;    // 0x60

    void internalAddEntity(hkEntity* entity);
    void internalRemoveEntity(hkEntity* entity);
    void addAction(hkAction* action);
    void removeAction(hkAction* action);
    void addConstraintToCriticalLockedIsland(hkConstraintInstance* constraint);
    void removeConstraintFromCriticalLockedIsland(hkConstraintInstance* constraint);
    void mergeConstraintInfo(const hkSimulationIsland* other);
    void markForWrite();
};
