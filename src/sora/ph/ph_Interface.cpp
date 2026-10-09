// Brawl physics wrapper translation unit ph_Interface.o (main.dol 0x8009A104-0x8009BA40).
// Not yet decompiled. Functions in address order with their map names:
//   0x8009A104   220  __ct   [map: phInterface____ct]
//   0x8009A1E0   200  __dt   [map: phInterface____dt]
//   0x8009A2A8   228  deleteInstance   [map: phInterface__deleteInstance]
//   0x8009A38C   352  init   [map: phInterface__init]
//   0x8009A4EC    16  sysAlloc   [map: phInterface__sysAlloc]
//   0x8009A4FC     4  gfMemFree   [map: phInterface__gfMemFree]
//   0x8009A500    64  __dt   [map: hkMemory____dt]
//   0x8009A540   540  removeHavokSystem   [map: phInterface__removeHavokSystem]
//   0x8009A75C   248  get2ndaryController   [map: phInterface__get2ndaryController]
//   0x8009A854   152  get2ndary2Controller   [map: phInterface__get2ndary2Controller]
//   0x8009A8EC   948  copy2ndaryMatrix   [map: phInterface__copy2ndaryMatrix]
//   0x8009ACA0  1432  copy2ndaryMatrix2   [map: phInterface__copy2ndaryMatrix2]
//   0x8009B238   236  getIKController   [map: phInterface__getIKController]
//   0x8009B324     8  getArray2ndaryController   [map: phInterface__getArray2ndaryController]
//   0x8009B32C     8  getArrayIKController   [map: phInterface__getArrayIKController]
//   0x8009B334   632  initialize   [map: phInterface__initialize]
//   0x8009B5AC   756  processDefault   [map: phInterface__processDefault]
//   0x8009B8A0    92  create2ndaryWorld   [map: phInterface__create2ndaryWorld]
//   0x8009B8FC    84  delete2ndaryWorld   [map: phInterface__delete2ndaryWorld]
//   0x8009B950   116  delete2ndaryController   [map: phInterface__delete2ndaryController]
//   0x8009B9C4   100  renderDebug   [map: phInterface__renderDebug]
//   0x8009BA28     4  notifiAllEndCalcWorldTimingC   [map: phInterface__notifiAllEndCalcWorldTimingC]
//   0x8009BA2C    20  errorReport   [map: phInterface__errorReport]

#include <ph/ph_Interface.h>
#include <gf/gf_heap_manager.h>
#include <revolution/OS/OSError.h>
#include <havok/hkThreadMemory.h>

phInterface* phInterface::s_instance = nullptr;

phInterface::phInterface() : gfTask("PhysicsSystem", Category_Physics, 0xe, 0xd, true) {
    m_flags40[0] = 1;
    m_flags40[1] = 0;
    m_flags40[2] = 1;
    m_flags40[3] = 1;
    m_flags40[4] = 1;
    m_flags40[5] = 1;
    m_flags40[6] = 0;
    m_flags40[7] = 0;
    unk48 = 0;
    unk4C = 0;
    unk50 = 0;
    unk54 = 0;
    m_2ndaryWorld = nullptr;
    m_worldFlag = 0;
    m_2ndaryControllers.m_data = nullptr;
    m_2ndaryControllers.m_size = 0;
    m_2ndaryControllers.m_capacityAndFlags = hkArrayBase::DONT_DEALLOCATE_FLAG;
    m_ikControllers.m_data = nullptr;
    m_ikControllers.m_size = 0;
    m_ikControllers.m_capacityAndFlags = hkArrayBase::DONT_DEALLOCATE_FLAG;
    unk78 = 0;
    unk84[0] = 0;
    unk84[1] = 0;
    unk84[2] = 0;
    unk84[3] = 0;
    unk84[4] = 0;
    unk84[5] = 0;
    unk84[6] = 0;
    unk84[7] = 0;
    unk84[8] = 0;
    unk84[9] = 0;
}

phInterface::~phInterface() {
    reinterpret_cast<hkArray<phIKController*>*>(&m_ikControllers)->~hkArray();
    reinterpret_cast<hkArray<ph2ndaryController*>*>(&m_2ndaryControllers)->~hkArray();
    if (m_flags40) {
        s_instance = nullptr;
    }
}

void phInterface::deleteInstance() {
    if (s_instance != nullptr) {
        if (m_2ndaryControllers.m_size != 0) {
            if ((m_2ndaryControllers.m_capacityAndFlags & hkArrayBase::DONT_DEALLOCATE_FLAG) == 0) {
                hkThreadMemory::s_instance->deallocateChunk(m_2ndaryControllers.m_data,
                                                            m_2ndaryControllers.m_capacityAndFlags << 2, 0x15);
            }
            m_2ndaryControllers.m_data = nullptr;
            m_2ndaryControllers.m_size = 0;
            m_2ndaryControllers.m_capacityAndFlags =
                (m_2ndaryControllers.m_capacityAndFlags & hkArrayBase::FORCE_SIGN_FLAG) |
                hkArrayBase::DONT_DEALLOCATE_FLAG;
        }
        if (m_ikControllers.m_size != 0) {
            if ((m_ikControllers.m_capacityAndFlags & hkArrayBase::DONT_DEALLOCATE_FLAG) == 0) {
                hkThreadMemory::s_instance->deallocateChunk(m_ikControllers.m_data,
                                                            m_ikControllers.m_capacityAndFlags << 2, 0x15);
            }
            m_ikControllers.m_data = nullptr;
            m_ikControllers.m_size = 0;
            m_ikControllers.m_capacityAndFlags =
                (m_ikControllers.m_capacityAndFlags & hkArrayBase::FORCE_SIGN_FLAG) |
                hkArrayBase::DONT_DEALLOCATE_FLAG;
        }
        s_instance->exit();
        s_instance = nullptr;
    }
}

void* phInterface::sysAlloc(size_t a, size_t b) {
    return gfHeapManager::alloc((Heaps::HeapType)0xd, a + b);
}

void phInterface::gfMemFree(void* p) {
    gfHeapManager::free(p);
}

ph2ndaryController* phInterface::get2ndaryController(void* owner, int a, int b) {
    ph2ndaryController* controller = nullptr;
    if (s_instance != nullptr) {
        bool enabled = s_instance->m_flags40[0] && s_instance->m_flags40[3];
        if (enabled && owner != nullptr) {
            controller = new (Heaps::Physics) ph2ndaryController(owner, a, b);
            static_cast<hkArray<ph2ndaryController*>*>(&s_instance->m_2ndaryControllers)->pushBack(controller);
        }
    }
    return controller;
}

ph2ndary2Controller* phInterface::get2ndary2Controller(void* a, void* b) {
    ph2ndary2Controller* controller = nullptr;
    if (s_instance != nullptr) {
        bool enabled = s_instance->m_flags40[0] && s_instance->m_flags40[3];
        if (enabled && a != nullptr) {
            controller = new (Heaps::Physics) ph2ndary2Controller(a, b);
        }
    }
    return controller;
}

hkArray<ph2ndaryController*>* phInterface::getArray2ndaryController() {
    return static_cast<hkArray<ph2ndaryController*>*>(&m_2ndaryControllers);
}

hkArray<phIKController*>* phInterface::getArrayIKController() {
    return static_cast<hkArray<phIKController*>*>(&m_ikControllers);
}

void phInterface::delete2ndaryController() {
    if (m_2ndaryControllers.m_size == 0) {
        if ((m_2ndaryControllers.m_capacityAndFlags & hkArrayBase::DONT_DEALLOCATE_FLAG) == 0) {
            hkThreadMemory::s_instance->deallocateChunk(m_2ndaryControllers.m_data,
                                                        m_2ndaryControllers.m_capacityAndFlags << 2, 0x15);
        }
        m_2ndaryControllers.m_data = nullptr;
        m_2ndaryControllers.m_size = 0;
        m_2ndaryControllers.m_capacityAndFlags =
            (m_2ndaryControllers.m_capacityAndFlags & hkArrayBase::FORCE_SIGN_FLAG) |
            hkArrayBase::DONT_DEALLOCATE_FLAG;
    }
}

void phInterface::renderDebug() {
    for (int i = 0; i < m_2ndaryControllers.m_size; i++) {
        ((ph2ndaryController**)m_2ndaryControllers.m_data)[i]->draw();
    }
}

void phInterface::notifiAllEndCalcWorldTimingC() {
}

void phInterface::errorReport(const char* msg) {
    OSReport("[physics]:%s", msg);
}
