#pragma once

#include <havok/hkToiResourceMgr.h>
#include <havok/hkMemory.h>

// TOI resource manager that carves its block from the thread memory stack (0x0C bytes).
// Layout and vtable from the Havok TU map (hkDefaultToiResourceMgr__*).
struct hkDefaultToiResourceMgr : hkToiResourceMgr {
    HK_DECLARE_REF_ALLOCATOR(0x13)

    int m_stackSize; // 0x08 (0x20000 by default); HYPOTHESIS: size of the block requested per TOI

    hkDefaultToiResourceMgr();
    virtual ~hkDefaultToiResourceMgr();

    virtual hkResult beginToiAndSetupResources(const void* unk4, const void* unk5,
                                               hkToiResources& resources);
    virtual bool cannotSolve();
    virtual bool resourcesDepleted();
    virtual void endToiAndFreeResources(const void* unk4, const void* unk5, hkToiResources& resources);

    hkBool shouldHandleGivenToi() const;
};
