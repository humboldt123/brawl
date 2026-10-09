#pragma once

#include <havok/hkSimulation.h>
#include <havok/hkArray.h>
#include <havok/hkToiResourceMgr.h>

struct hkEntity;

// One pending time-of-impact event (0x40 bytes, stored in an hkArray). Layout partly recovered from addToiEvent.
// Layout from addToiEvent and the removal code. The copy below moves the fields one by one; 0x1C is not copied.
struct hkToiEvent {
    hkReal unk00;            // 0x00
    int unk04;               // 0x04
    int unk08;               // 0x08
    hkReal unk0C;            // 0x0C
    int unk10;               // 0x10
    int unk14;               // 0x14
    u32 unk18;               // 0x18 (matched against the agent key at 0x10 when an agent's events are removed)
    int unk1C;               // 0x1C (not copied by copyFrom)
    hkReal unk20[8];         // 0x20 .. 0x3C (two vectors, copied as floats)

    void copyFrom(const hkToiEvent& src) {
        unk00 = src.unk00;
        unk04 = src.unk04;
        unk08 = src.unk08;
        unk0C = src.unk0C;
        unk10 = src.unk10;
        unk14 = src.unk14;
        unk18 = src.unk18;
        for (int i = 0; i < 8; i++) {
            unk20[i] = src.unk20[i];
        }
    }
};

// Continuous (time-of-impact) simulation (layout partly recovered; tail not recovered yet).
// Fields from the constructor/destructor (0x28..0x38 region).
struct hkContinuousSimulation : hkSimulation {
    hkArray<hkToiEvent> m_toiEvents;         // 0x28 (ctor: empty array with DONT_DEALLOCATE flag)
    hkToiResourceMgr* m_toiResourceMgr;      // 0x34 (owned, allocated by the constructor)
    int m_unk38;                             // 0x38 (set to 0 by the constructor)

    hkContinuousSimulation(hkWorld* world);
    virtual ~hkContinuousSimulation(); // 0x08

    // Debug-only consistency checks; empty in this build. Argument types are HYPOTHESIS (unused).
    void assertThereIsNoCollisionInformationForEntities(const void* entities);
    void assertThereIsNoCollisionInformationForAgent(const void* agent);
    void warpTime(hkReal dt);
    void removeCollisionInformationForAgent(const void* agent);
};
