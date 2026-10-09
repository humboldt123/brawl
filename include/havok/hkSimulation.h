#pragma once

#include <havok/hkBase.h>
#include <havok/hkVector4.h>
#include <havok/hkMemory.h>

struct hkWorld;

// Simulation driver owned by an hkWorld (layout partly recovered; tail not recovered yet).
// Field offsets from the constructor (0x0C..0x24 region).
struct hkSimulation : hkReferencedObject {
    hkWorld* m_world;     // 0x08 HYPOTHESIS: world that owns this simulation (constructor argument)
    int m_unk0C;          // 0x0C (set to 1 by the constructor)
    hkReal m_unk10;       // 0x10 (set to 0 by the constructor)
    hkReal m_unk14;       // 0x14 (set to 0 by the constructor)
    u8 m_unk18[0x04];     // 0x18 (not recovered)
    hkReal m_unk1C;       // 0x1C (constructor constant)
    hkReal m_unk20;       // 0x20 (constructor constant)
    int m_unk24;          // 0x24 (set to 0 by the constructor)

    static void operator delete(void* p) {
        hkMemory::s_instance->deallocateChunk(p, ((hkReferencedObject*)p)->m_memSizeAndFlags, 0x13);
    }

    hkSimulation(hkWorld* world);
    hkReal snapSimulateTimeAndGetTimeToAdvanceTo();
    virtual ~hkSimulation(); // 0x08 (out of line in src/havok/hkSimulation.cpp)
};
