#pragma once

// Local shadow of BrawlHeaders' so/so_archive_db.h: the original plus create (the map names fn_27_6FCF4
// soArchiveDb::create; the fighter manager constructor creates archive 0 with 0x60).
// Also soArchiveItem and soArchiveDb::Processer (names from the sora_melee map, so_archive_db.o).

#include <StaticAssert.h>
#include <gf/gf_task.h>
#include <ut/ut_archive_manager.h>

// TODO: class size
class soArchiveDb {
public:
    static utArchiveManager* getManager(u32 index);
    static bool create(u32 index, u32 size);

    // The task that drives every created archive manager (created with the first manager).
    class Processer : public gfTask {
    public:
        virtual void processBegin();
        virtual ~Processer() { }
    };
};

// One archive resource of one soArchiveDb manager.
class soArchiveItem {
public:
    u32 m_index;      // 0x0: soArchiveDb manager index
    u32 m_resourceId; // 0x4: 0xFFFF when nothing is loaded

    soArchiveItem(u32 index);
    void init();
    // HYPOTHESIS: parameter types (forwarded unchanged to utArchiveManager::reqLoad).
    void reqLoad(const char* path, Heaps::HeapType heap, u32 mode, int offset, int size, u32 id, bool async);
    // HYPOTHESIS: parameter types (forwarded unchanged to utArchiveManager::addNoManageArchive).
    void add(void* data, int size, Heaps::HeapType heap);
    void resolve(u32 emCommonArcId);
    // HYPOTHESIS: a nonzero argument cancels the load instead of unloading it.
    void unload(int cancel);
    bool isExistEffect();
    nw4r::g3d::ResFileData* getResFile(ARCNodeType type, u32 p2, u32 p3, u32 p4);
    void* getData(ARCNodeType type, int fileIndex, int* size, int groupID, s32 archiveId);
    void* getData(const char* nodeName);
};
static_assert(sizeof(soArchiveItem) == 0x8, "Class is wrong size!");
