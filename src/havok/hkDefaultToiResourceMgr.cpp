// Havok translation unit hkDefaultToiResourceMgr.o (main.dol 0x8032BBE8-0x8032BD74).
// Functions in address order (method names from the Havok TU map):
//   0x8032BBE8    32  __ct   [map: hkDefaultToiResourceMgr____ct]
//   0x8032BC08   204  beginToiAndSetupResources   [map: hkDefaultToiResourceMgr__beginToiAndSetupResources]
//   0x8032BCD4    44  endToiAndFreeResources   [map: hkDefaultToiResourceMgr__endToiAndFreeResources]
//   0x8032BD00    92  __dt   [map: hkDefaultToiResourceMgr____dt]
//   0x8032BD5C     8  shouldHandleGivenToi   [map: hkDefaultToiResourceMgr__shouldHandleGivenToi]
//   0x8032BD64     8  resourcesDepleted   [map: hkDefaultToiResourceMgr__resourcesDepleted]
//   0x8032BD6C     8  cannotSolve   [map: hkDefaultToiResourceMgr__cannotSolve]

#include <havok/hkDefaultToiResourceMgr.h>
#include <havok/hkThreadMemory.h>

hkDefaultToiResourceMgr::hkDefaultToiResourceMgr() {
    m_stackSize = 0x20000;
}

hkResult hkDefaultToiResourceMgr::beginToiAndSetupResources(const void* unk4, const void* unk5,
                                                            hkToiResources& resources) {
    hkBool shouldHandle = shouldHandleGivenToi();
    if (shouldHandle.m_bool != 0) {
        int size = m_stackSize;
        resources.m_stackBlockSize = size;
        resources.m_stackBlock = (char*)hkThreadMemory::s_instance->allocateStack(size);
        resources.unk10 = 3;
        resources.unk14 = 4;
        resources.unk08 = 1000;
        resources.unk0C = 1000;
        resources.unk04 = 1000;
        resources.unk00 = 2;
        return HK_SUCCESS;
    }
    return HK_FAILURE;
}

void hkDefaultToiResourceMgr::endToiAndFreeResources(const void* unk4, const void* unk5,
                                                      hkToiResources& resources) {
    hkThreadMemory::s_instance->deallocateStack(resources.m_stackBlock);
}

hkDefaultToiResourceMgr::~hkDefaultToiResourceMgr() {}

#pragma auto_inline off
hkBool hkDefaultToiResourceMgr::shouldHandleGivenToi() const {
    return true;
}
#pragma auto_inline reset

bool hkDefaultToiResourceMgr::resourcesDepleted() {
    return true;
}

bool hkDefaultToiResourceMgr::cannotSolve() {
    return false;
}
