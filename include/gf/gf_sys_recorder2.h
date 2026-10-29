#pragma once

#include <StaticAssert.h>
#include <gf/gf_archive.h>
#include <gf/gf_file_io_handle.h>
#include <gf/gf_system_callback.h>
#include <sr/sr_common.h>
#include <types.h>

class nteExchange;

// One thing the save data check wants to have done to the save data: what to do (0: the record of the game, 1: the
// adventure data, ..., 4: the banner, 6: the end of the list) and two more words.
// HYPOTHESIS: the names; the words are not used by the boot menu.
struct gfSysRecorder2Require {
    int m_kind;     // 0x00
    int m_unk4;     // 0x04
    int m_unk8;     // 0x08
};

// The recorder of the save data of the system (the NAND): it checks the files of the game, renews them, saves and
// loads the system, adventure and Wi-Fi data. The boot menu starts a request, waits until it is not busy any more and
// looks at the result.
// HYPOTHESIS: the names of the fields follow how the boot menu and the request functions use them.
class gfSysRecorder2 {
public:
    void* m_buffer;                         // 0x00
    u32 m_bufferSize;                       // 0x04
    gfFileIOHandle m_handle;                // 0x08 the file of the system data (the buffer is the loaded data)
    gfArchive m_archive;                    // 0x0C
    void* m_unk8C;                          // 0x8C
    void* m_unk90;
    void* m_unk94;
    bool m_vfMounted;                       // 0x98
    nteExchange* m_exchange;                // 0x9C
    int m_result;                           // 0xA0 how the last request ended (0: done, 1..3: the kinds of failure)
    int m_checkResult;                      // 0xA4 what the check found (0: nothing to do, 1: renew, 2: create)
    gfSysRecorder2Require m_require[6];     // 0xA8 the things that have to be done
    int m_busy;                             // 0xF0 a request is still running (1: check, 2: renew, 3: save, 4: load)
    int m_step;                             // 0xF4
    bool m_waitFile : 1;                    // 0xF8
    bool m_isKind8 : 1;
    bool m_isKind7 : 1;
    bool m_unkF8_4 : 1;
    bool m_unkF8_3 : 1;
    u8 m_unkF9;
    u8 m_index;                             // 0xFA
    u16 m_unkFC;
    int m_unk100;
    int m_kind;                             // 0x104 (0: system, 1: adventure, 2: Wi-Fi)
    Heaps::HeapType m_requestHeap;          // 0x108
    void* m_saveBuffer;                     // 0x10C
    u32 m_saveSize;                         // 0x110
    Heaps::HeapType m_heap;                 // 0x114

    static gfSysRecorder2* createInstance(Heaps::HeapType heap);
    // HYPOTHESIS: the argument is the bit of the GameGlobal save/load switches.
    static bool isEnable(int kind);
    // HYPOTHESIS: the argument is the buffer that gets the path.
    static void getCollectionVFName(char* buf);
    gfSysRecorder2();
    ~gfSysRecorder2();
    static void dropInstance();
    void update();
    // HYPOTHESIS: kind is the gfSysRecorder2Require kind.
    bool requestCheck(Heaps::HeapType heap, int kind);
    bool requestRenew(Heaps::HeapType heap, int kind);
    // HYPOTHESIS: the NAND files and blocks one gfSysRecorder2Require kind needs.
    void getRequire(int kind, int* files, int* blocks, bool noExtra);
    void* getBuffer(int kind);
    bool saveSystem();
    bool saveAdventure(u32 index);
    bool saveWifi();
    bool loadSystem(Heaps::HeapType heap);
    bool loadAdventure(Heaps::HeapType heap, u32 index);
    bool loadWifi(Heaps::HeapType heap);
    void _processCheck();
    void _processRenew();
    void returnToIdle(int result);
    void _processSave();
    void _processLoad();
    bool _save(int kind, u32 index);
    bool _load(Heaps::HeapType heap, int kind, u32 index);
};
static_assert(sizeof(gfSysRecorder2) == 0x118, "Class is the wrong size!");

class gfSRPermitRebootCallback : public gfReturnStatusCallback {
public:
    bool m_canReturn; // 0x08

    // HYPOTHESIS: the static initializer stores the gfReturnStatusCallback vtable first, so m_next is probably
    // cleared by a base constructor that the shared header does not have.
    gfSRPermitRebootCallback() {
        m_next = NULL;
        m_canReturn = true;
    }
    virtual bool returnStatus() const;
};
static_assert(sizeof(gfSRPermitRebootCallback) == 0xC, "Class is the wrong size!");
