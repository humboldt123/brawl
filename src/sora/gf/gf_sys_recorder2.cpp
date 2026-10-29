#include <cstdio>
#include <cstring>
#include <gf/gf_error_manager.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_shutdown_manager.h>
#include <gf/gf_sys_recorder2.h>
#include <gm/gm_global.h>
#include <types.h>

// HYPOTHESIS: stand-ins for callees that are not named yet; the map gives GameGlobal::setEnableSaveLoad (the
// save/load switches), getSaveDataSize, checkSaveData and getAdventureSaveDataSize, and the nteExchange destructor.
extern "C" u8 fn_8004EAD4(GameGlobal* global);
extern "C" u32 fn_8004EAEC();
extern "C" u32 fn_8004EFA0();
extern "C" bool fn_8004EAF8(void* data);
extern "C" void fn_8014E520(nteExchange* exchange, int del);
// HYPOTHESIS: this unit's request workers gfSysRecorder2::_processCheck, _processRenew and _processLoad (not matched
// yet).
extern "C" void fn_80030FA4(gfSysRecorder2* recorder);
extern "C" void fn_8003186C(gfSysRecorder2* recorder);
extern "C" void fn_80032B64(gfSysRecorder2* recorder);
extern "C" u32 NETCalcCRC32(const u8* data, u32 len);
extern "C" s32 DVDCancelAll();

// HYPOTHESIS: one file of the save data on the NAND; m_unk12 means the file has two copies (0.bin, 1.bin).
struct gfSysRecorder2FileInfo {
    int m_id;
    char m_name[12];
    u8 m_unk10;
    u8 m_unk11;
    u8 m_unk12;
    u8 m_unk13;
};

// HYPOTHESIS: data names are the split's labels.
gfSysRecorder2FileInfo lbl_80422F70[5] = {
    { 0, "autosv", 0, 0, 1, 0 },
    { 1, "advsv", 0, 0, 1, 0 },
    { 2, "net", 0, 0, 1, 0 },
    { 3, "collect.vff", 0, 0, 0, 1 },
    { 4, "banner.bin", 0, 0, 0, 0 },
};
char lbl_80422FD4[2][6] = { "0.bin", "1.bin" };
gfSRPermitRebootCallback lbl_804953A0;
gfSysRecorder2* lbl_805A0078;

gfSysRecorder2* gfSysRecorder2::createInstance(Heaps::HeapType heap) {
    if (lbl_805A0078) {
        return NULL;
    }
    lbl_805A0078 = new (heap) gfSysRecorder2();
    if (lbl_805A0078) {
        lbl_805A0078->m_heap = heap;
    }
    return lbl_805A0078;
}

bool gfSysRecorder2::isEnable(int kind) {
    u8 flags = fn_8004EAD4(g_GameGlobal);
    return flags & (1 << kind);
}

// HYPOTHESIS: a lookup of the file table by id.
static gfSysRecorder2FileInfo* getFileInfo(int id) {
    gfSysRecorder2FileInfo* info = NULL;
    for (int i = 0; i < 5; i++) {
        if (lbl_80422F70[i].m_id == id) {
            info = &lbl_80422F70[i];
            break;
        }
    }
    return info;
}

void gfSysRecorder2::getCollectionVFName(char* buf) {
    sprintf(buf, "nand:/%s", getFileInfo(3)->m_name);
}

gfSysRecorder2::gfSysRecorder2() {
    m_busy = 0;
    m_require[0].m_kind = 6;
    m_require[0].m_unk4 = 0;
    m_require[0].m_unk8 = 0;
    m_buffer = NULL;
    m_bufferSize = 0;
    m_result = 0;
    m_checkResult = 0;
    m_unk8C = NULL;
    m_unk90 = NULL;
    m_unk94 = NULL;
    m_exchange = NULL;
    m_vfMounted = false;
}

gfSysRecorder2::~gfSysRecorder2() {
    if (m_buffer) {
        gfHeapManager::free(m_buffer);
        m_buffer = NULL;
    }
    if (m_unk8C) {
        gfHeapManager::free(m_unk8C);
        m_unk8C = NULL;
    }
    if (m_unk90) {
        gfHeapManager::free(m_unk90);
        m_unk90 = NULL;
    }
    if (m_unk94) {
        gfHeapManager::free(m_unk94);
        m_unk94 = NULL;
    }
    if (m_exchange) {
        fn_8014E520(m_exchange, 1);
        m_exchange = NULL;
    }
}

void gfSysRecorder2::dropInstance() {
    if (lbl_805A0078) {
        delete lbl_805A0078;
        lbl_805A0078 = NULL;
    }
    lbl_804953A0.m_canReturn = true;
    g_gfShutdownManager.unkC = true;
}

void gfSysRecorder2::update() {
    bool shutdown = false;
    if (g_gfShutdownManager.m_reset || g_gfShutdownManager.m_powerOff || g_gfShutdownManager.m_returnToWiiMenu) {
        shutdown = true;
    }
    if (shutdown && gfErrorManager::getInstance() && gfErrorManager::getInstance()->m_isSuspended &&
        !gfErrorManager::getInstance()->m_isFatal) {
        lbl_804953A0.m_canReturn = true;
        g_gfShutdownManager.unkC = true;
        return;
    }
    switch (m_busy) {
    case 1:
        fn_80030FA4(this);
        break;
    case 2:
        fn_8003186C(this);
        if (m_busy == 0) {
            lbl_804953A0.m_canReturn = true;
            g_gfShutdownManager.unkC = true;
        }
        break;
    case 3:
        _processSave();
        if (m_busy == 0) {
            lbl_804953A0.m_canReturn = true;
            g_gfShutdownManager.unkC = true;
        }
        break;
    case 4:
        fn_80032B64(this);
        break;
    }
}

bool gfSysRecorder2::requestCheck(Heaps::HeapType heap, int kind) {
    if (m_busy != 0) {
        return false;
    }
    m_busy = 1;
    m_step = 0;
    m_unkF9 = 0;
    m_unk100 = 0;
    m_requestHeap = heap;
    m_waitFile = false;
    m_unkF8_4 = false;
    m_unkF8_3 = false;
    m_isKind8 = kind == 8;
    m_isKind7 = kind == 7;
    bool special = kind == 8 || kind == 7;
    if (special) {
        kind = 0;
    }
    m_kind = kind;
    m_unkFC = 0;
    m_result = 0;
    m_checkResult = 0;
    if (kind == 0) {
        m_step = 4;
    }
    return true;
}

bool gfSysRecorder2::requestRenew(Heaps::HeapType heap, int kind) {
    if (m_busy != 0) {
        return false;
    }
    m_busy = 2;
    m_step = 4;
    m_unkF9 = 0;
    m_unk100 = 0;
    m_waitFile = false;
    m_unkF8_4 = false;
    m_unkF8_3 = false;
    m_isKind8 = kind == 8;
    m_isKind7 = kind == 7;
    bool special = kind == 8 || kind == 7;
    if (special) {
        kind = 0;
    }
    m_kind = kind;
    m_requestHeap = heap;
    m_result = 0;
    if (m_unk8C) {
        gfHeapManager::free(m_unk8C);
        m_unk8C = NULL;
    }
    if (m_unk90) {
        gfHeapManager::free(m_unk90);
        m_unk90 = NULL;
    }
    if (m_unk94) {
        gfHeapManager::free(m_unk94);
        m_unk94 = NULL;
    }
    if (m_exchange) {
        fn_8014E520(m_exchange, 1);
        m_exchange = NULL;
    }
    if (m_isKind8 || m_kind == 4) {
        m_step = 0;
    } else if (m_kind == 3) {
        m_step = 2;
    }
    lbl_804953A0.m_canReturn = false;
    g_gfShutdownManager.unkC = false;
    return true;
}

void* gfSysRecorder2::getBuffer(int kind) {
    if (m_busy != 0) {
        return NULL;
    }
    u32 size = 0;
    if (kind == 0) {
        u32 dataSize = fn_8004EAEC();
        size = 0;
        if (kind == 0 || kind == 2) {
            size = dataSize + 4;
            if (size & 0x1F) {
                size += 0x20 - (size & 0x1F);
            }
        } else if (kind == 1) {
            size = (dataSize + 4) * 0x33;
            if (size & 0x1F) {
                size += 0x20 - (size & 0x1F);
            }
        }
    } else if (kind == 2) {
        size = 0;
        if (kind == 0 || kind == 2) {
            size = 0x2820;
        } else if (kind == 1) {
            size = 0x7F9A0;
        }
    } else if (kind == 1) {
        size = fn_8004EFA0() + 4;
    }
    if (size == 0) {
        return NULL;
    }
    if (m_bufferSize < size) {
        if (m_buffer) {
            gfHeapManager::free(m_buffer);
        }
        m_buffer = gfHeapManager::alloc(m_heap, size, 0x20);
        m_bufferSize = size;
    }
    m_saveSize = size;
    m_saveBuffer = m_buffer;
    return m_buffer;
}

bool gfSysRecorder2::saveSystem() {
    return _save(0, 0);
}

bool gfSysRecorder2::saveAdventure(u32 index) {
    if (index >= 0x33) {
        return false;
    }
    return _save(1, index);
}

bool gfSysRecorder2::saveWifi() {
    return _save(2, 0);
}

bool gfSysRecorder2::loadSystem(Heaps::HeapType heap) {
    return _load(heap, 0, 0);
}

bool gfSysRecorder2::loadAdventure(Heaps::HeapType heap, u32 index) {
    if (index != 0 && index > 0x33) {
        return false;
    }
    return _load(heap, 1, index);
}

bool gfSysRecorder2::loadWifi(Heaps::HeapType heap) {
    return _load(heap, 2, 0);
}

void gfSysRecorder2::returnToIdle(int result) {
    m_result = result;
    if (m_vfMounted) {
        m_handle.unmountVF(0);
        m_waitFile = true;
        m_step = 0x1D;
    } else {
        m_busy = 0;
    }
    if (m_unk8C) {
        gfHeapManager::free(m_unk8C);
        m_unk8C = NULL;
    }
    if (m_unk90) {
        gfHeapManager::free(m_unk90);
        m_unk90 = NULL;
    }
    if (m_unk94) {
        gfHeapManager::free(m_unk94);
        m_unk94 = NULL;
    }
    if (m_exchange) {
        fn_8014E520(m_exchange, 1);
        m_exchange = NULL;
    }
}

void gfSysRecorder2::_processSave() {
    switch (m_step) {
    case 0: {
        gfSysRecorder2FileInfo* info = getFileInfo(m_kind);
        if (!info) {
            m_busy = 0;
            m_result = 3;
            break;
        }
        int copy = m_unk100;
        char path[0x80];
        strcpy(path, "nand:/");
        strcat(path, info->m_name);
        if (info->m_unk12) {
            strcat(path, lbl_80422FD4[copy]);
        }
        int offset = 0;
        if (m_kind == 1) {
            offset = m_saveSize * m_index;
        }
        m_handle.writeRequest(path, m_saveBuffer, m_saveSize, offset);
        m_step = 1;
        break;
    }
    case 1:
        if (m_handle.isReady()) {
            if (m_handle.getReturnStatus() == 0) {
                m_step = 2;
            } else {
                m_busy = 0;
                m_result = 3;
            }
        } else {
            bool shutdown;
            if (g_gfShutdownManager.unkD == true || g_gfShutdownManager.unkC) {
                shutdown = false;
            } else {
                shutdown = false;
                if (g_gfShutdownManager.m_reset || g_gfShutdownManager.m_powerOff ||
                    g_gfShutdownManager.m_returnToWiiMenu) {
                    shutdown = true;
                }
            }
            if (shutdown) {
                DVDCancelAll();
            }
        }
        break;
    case 2: {
        gfSysRecorder2FileInfo* info = getFileInfo(m_kind);
        if (info->m_unk12 && m_unk100 == 0) {
            m_unk100++;
            m_step = 0;
        } else {
            m_busy = 0;
            m_result = 0;
        }
        break;
    }
    }
}

// HYPOTHESIS: the saved data ends with the CRC-32 of the rest.
static inline void setCRC(void* data, u32 size) {
    *reinterpret_cast<u32*>(static_cast<u8*>(data) + (size - 4)) = NETCalcCRC32(static_cast<const u8*>(data), size - 4);
}

bool gfSysRecorder2::_save(int kind, u32 index) {
    if (kind >= 3) {
        return false;
    }
    if (!m_saveBuffer || !m_saveSize) {
        return false;
    }
    if (m_busy != 0) {
        return false;
    }
    if (!isEnable(kind)) {
        m_busy = 0;
        m_result = 3;
        return true;
    }
    setCRC(m_saveBuffer, m_saveSize);
    m_busy = 3;
    m_step = 0;
    m_waitFile = false;
    m_unk100 = 0;
    m_index = index;
    m_kind = kind;
    m_result = 0;
    lbl_804953A0.m_canReturn = false;
    g_gfShutdownManager.unkC = false;
    return true;
}

bool gfSysRecorder2::_load(Heaps::HeapType heap, int kind, u32 index) {
    if (kind >= 3) {
        return false;
    }
    if (m_busy != 0) {
        return false;
    }
    if (!isEnable(kind)) {
        m_busy = 0;
        m_result = 3;
        return true;
    }
    m_busy = 4;
    m_step = 0;
    m_waitFile = false;
    m_unk100 = 0;
    m_index = index;
    m_kind = kind;
    m_requestHeap = heap;
    m_result = 0;
    return true;
}

bool gfSRPermitRebootCallback::returnStatus() const {
    return m_canReturn;
}
