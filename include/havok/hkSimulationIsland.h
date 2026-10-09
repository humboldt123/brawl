#pragma once

#include <havok/hkBase.h>
#include <havok/hkArray.h>

struct hkEntity;
struct hkAction; // HYPOTHESIS: action object; the island stores a back pointer at 0x0C (not in a header yet)
struct hkStatisticsCollector;

// Simulation island (layout partly recovered; base and tail not recovered). Fields used by the island code:
// 0x26 bit field (bits 0xC0 and 0x30, written as flags by internalRemoveEntity/removeAction),
// 0x38 entity array (pointers), 0x60 action array (pointers).
struct hkSimulationIsland : hkReferencedObject {
    u8 m_unk08[0x1E];                // 0x08 (not recovered)
    u8 m_flags26;                    // 0x26 bit field, see above
    u8 m_unk27[0x11];                // 0x27 (not recovered)
    hkArray<hkEntity*> m_entities;   // 0x38
    u8 m_unk44[0x1C];                // 0x44 (not recovered)
    hkArray<hkAction*> m_actions;    // 0x60

    void internalAddEntity(hkEntity* entity);
    void internalRemoveEntity(hkEntity* entity);
    void addAction(hkAction* action);
    void removeAction(hkAction* action);
    void markForWrite();
};
