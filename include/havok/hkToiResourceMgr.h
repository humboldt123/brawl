#pragma once

#include <havok/hkBase.h>

// Per-TOI resources handed to the TOI solver (0x20 bytes). Only the layout is recovered; the field names
// are not known yet. The values at 0x00-0x14 are constants written by hkDefaultToiResourceMgr.
// HYPOTHESIS: the struct is shared with hkContinuousSimulation (hkToiResources__hkLs_localizedSolveToi).
struct hkToiResources {
    int unk00;               // 0x00 (2 in the default manager)
    int unk04;               // 0x04 (1000)
    int unk08;               // 0x08 (1000)
    int unk0C;               // 0x0C (1000)
    int unk10;               // 0x10 (3)
    int unk14;               // 0x14 (4)
    char* m_stackBlock;      // 0x18: block taken from the thread memory stack
    int m_stackBlockSize;    // 0x1C
};

// Abstract manager of TOI resources (vtable: 0x08 dtor, 0x0C calcStatistics, then the four below).
struct hkToiResourceMgr : hkReferencedObject {
    virtual hkResult beginToiAndSetupResources(const void* unk4, const void* unk5,
                                               hkToiResources& resources) = 0; // 0x10
    virtual bool cannotSolve() = 0;                                               // 0x14
    virtual bool resourcesDepleted() = 0;                                         // 0x18
    virtual void endToiAndFreeResources(const void* unk4, const void* unk5,
                                        hkToiResources& resources) = 0;                // 0x1C
};
