#pragma once
// Local BrawlHeaders shadow: verified manager and load-task layouts, the load observer
// and the recovered request, polling, release and lookup operations.

#include <StaticAssert.h>
#include <gf/gf_archive.h>
#include <gf/gf_archive_db.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <sr/sr_common.h>
#include <types.h>
#include <ut/ut_list.h>

class utArchiveEventObserver {
public:
    virtual ~utArchiveEventObserver() { }
    // HYPOTHESIS: parameter types; called with the task's resource id and 1 (ok) or 2 (error).
    virtual void notifyComp(u32 resourceId, s32 result) = 0;
};

class utArchiveEventObserverNull : public utArchiveEventObserver {
public:
    virtual ~utArchiveEventObserverNull() { }
    virtual void notifyComp(u32 resourceId, s32 result) { }
};

extern utArchiveEventObserverNull lbl_805A00C0;

class utArchiveManager {
public:
    // HYPOTHESIS: field names; meanings from the request, process, unload and cancel code.
    struct LoadTask : public utListNode {
        u16 m_refCount;                     // 0x08
        u32 m_resourceId;                   // 0x0C
        u32 m_sourceId;                     // 0x10
        gfArchive* m_archive;               // 0x14
        utArchiveEventObserver* m_observer; // 0x18

        LoadTask() : m_archive(nullptr), m_observer(&lbl_805A00C0) { }
        ~LoadTask() { }
        // HYPOTHESIS: name; the set-up every new request shares.
        void init() {
            m_resourceId = 0xFFFF;
            m_sourceId = 0xFFFF;
            m_archive = nullptr;
            m_refCount = 1;
        }
        // HYPOTHESIS: name; the reset the constructor and the release share.
        void clear() {
            m_resourceId = 0xFFFF;
            m_sourceId = 0xFFFF;
            m_refCount = 0;
            m_archive = nullptr;
        }
    };

    utArchiveManager(u32 taskCount);
    virtual ~utArchiveManager();

    gfArchiveDatabase* m_database; // 0x04
    u32 m_taskCount;               // 0x08
    LoadTask* m_tasks;             // 0x0C
    utList m_freeList;             // 0x10
    // HYPOTHESIS: list roles; the remove and cancel lists are the ones isExistRemoveTask/isExistCancelTask test.
    utList m_loadingList;          // 0x1C
    utList m_loadedList;           // 0x28
    utList m_removeList;           // 0x34
    utList m_cancelList;           // 0x40

    LoadTask* isExist(utList* list, const char* path);
    // HYPOTHESIS: parameter types from the sora_melee callers and the gfArchive read calls.
    u32 reqLoad(const char* path, Heaps::HeapType heap, u32 mode, int offset, int size, u32 id,
                utArchiveEventObserver* observer, bool async);
    // HYPOTHESIS: parameter types from gfArchive::setFileImage.
    u32 addNoManageArchive(void* data, int size, Heaps::HeapType heap);
    void process();
    void cancel(u32 resourceId);
    // HYPOTHESIS: parameter types from the gfArchive::getData call.
    void* getData(u32 resourceId, ARCNodeType type, int fileIndex, int* size, int groupID, s32 archiveId);
    void* getPublicAddress(u32 paramArcId, const char* nodeName, u16 fileIndex, u32 groupID, u32 endian);
    void resolveReference(u32 paramArcId, u32 emCommonArcId, u32 p3, u32 p4, u32 p5, u32 p6);
    // HYPOTHESIS: parameter types from the utRelocate constructor and resolveReference.
    void resolveReferenceExternalData(u8* data, u32 size, u32 resourceId, u32 fileIndex, u32 groupID);
    nw4r::g3d::ResFileData* getResFileFromId(s32 p1, ARCNodeType type, u32 p3, u32 p4, u32 p5);
    bool isExistEffect(u32 resourceId);

    // HYPOTHESIS: inline helpers (no out-of-line copies exist); the names are local.
    void releaseTask(LoadTask* task);
    void unloadTask(LoadTask* task);
    LoadTask* getWaitingCopy(utList* list, u32 sourceId);
    LoadTask* getListTask(utList* list, u32 resourceId);
    LoadTask* getTask(u32 resourceId);
    LoadTask* getTask(gfArchive* archive);
};
static_assert(sizeof(utArchiveManager::LoadTask) == 0x1C, "Class is the wrong size!");
static_assert(sizeof(utArchiveManager) == 0x4C, "Class is the wrong size!");
