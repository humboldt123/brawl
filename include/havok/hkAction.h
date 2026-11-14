#pragma once

#include <havok/hkBase.h>
#include <havok/hkArray.h>

struct hkWorld;
struct hkSimulationIsland;
struct hkEntity;
struct hkStepInfo;

// Base of the world actions (0x18 bytes). Layout and names from main.dol's hkActionClass; the world and
// island pointers are runtime-only (not serialized).
// HYPOTHESIS: the names of the two virtuals after hkReferencedObject's, from their callers:
// hkSimulation::applyActions calls 0x10 with the world's step info, hkWorldOperationQueue calls 0x14 to collect
// the entities an action touches.
struct hkAction : hkReferencedObject {
    virtual void applyAction(const hkStepInfo& stepInfo) = 0;      // 0x10
    virtual void getEntities(hkArray<hkEntity*>& entitiesOut) = 0; // 0x14
    virtual void unk18();                                          // 0x18
    // HYPOTHESIS: the name (hkEntityCallbackUtil::fireEntityRemoved calls it until the entity has no actions).
    virtual void entityRemovedCallback(hkEntity* entity);           // 0x1C

    hkWorld* m_world;              // 0x08 HYPOTHESIS: type (the reflection only says pointer)
    hkSimulationIsland* m_island;  // 0x0C HYPOTHESIS: type; the island sets it in addAction
    u32 m_userData;                // 0x10
    const char* m_name;            // 0x14
};
