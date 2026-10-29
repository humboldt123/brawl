#include <cstring>
#include <gf/gf_archive.h>
#include <gf/gf_archive_db.h>
#include <memory.h>
#include <revolution/OS/OSError.h>
#include <sr/sr_common.h>
#include <types.h>
#include <ut/ut_archive_manager.h>
#include <ut/ut_list.h>
#include <ut/ut_relocate.h>

// HYPOTHESIS: view of the MSL std::string that gfArchive::getName/getPath return by value
// (the top bit of the first word selects the heap text at +8, short text starts at +1).
struct gfArchiveString {
    u32 m_word;
    u32 m_size;
    char* m_heapText;

    const char* c_str() const {
        if (m_word >> 31) {
            return m_heapText;
        }
        return reinterpret_cast<const char*>(this) + 1;
    }
    ~gfArchiveString() {
        if (m_word >> 31) {
            delete m_heapText;
        }
    }
};

extern "C" {
// HYPOTHESIS: unnamed gfArchive entry points (map: gfArchive __ct/getName/getPath/readFile/
// readFileRequest/cancelRequest/isCanceled/getReturnStatus/isExistEffect); types from register use.
gfArchive* fn_800137C0(gfArchive* archive, Heaps::HeapType heap, gfArchive* source);
gfArchiveString fn_800146F0(const char* path);
gfArchiveString fn_80014480(const char* path);
u32 fn_80014ED4(gfArchive* archive, const char* path, Heaps::HeapType heap, int offset);
u32 fn_800151FC(gfArchive* archive, const char* path, Heaps::HeapType heap, int offset, int size, u32 id);
u32 fn_8001583C(gfArchive* archive);
u32 fn_8001588C(gfArchive* archive);
s32 fn_80015DAC(gfArchive* archive);
bool fn_8001440C(gfArchive* archive);
}

utArchiveManager::utArchiveManager(u32 taskCount) : m_database(nullptr), m_taskCount(taskCount) {
    m_freeList.m_head = m_freeList.m_tail = nullptr;
    m_freeList.m_length = 0;
    m_loadingList.m_head = m_loadingList.m_tail = nullptr;
    m_loadingList.m_length = 0;
    m_loadedList.m_head = m_loadedList.m_tail = nullptr;
    m_loadedList.m_length = 0;
    m_removeList.m_head = m_removeList.m_tail = nullptr;
    m_removeList.m_length = 0;
    m_cancelList.m_head = m_cancelList.m_tail = nullptr;
    m_cancelList.m_length = 0;
    m_database = gfArchiveDatabase::create(taskCount, Heaps::System);
    m_tasks = new (Heaps::System) LoadTask[taskCount];
    for (u32 i = 0; i < taskCount; i++) {
        m_tasks[i].clear();
        m_freeList.addTail(&m_tasks[i]);
    }
}

// HYPOTHESIS: inline task release (no out-of-line copy exists); it unlinks the task, drops the
// database reference and returns the task to the free list.
inline void utArchiveManager::releaseTask(LoadTask* task) {
    if (m_removeList.isExistNode(task) != -1) {
        m_removeList.remove(task);
    }
    if (m_loadedList.isExistNode(task) != -1) {
        m_loadedList.remove(task);
    }
    if (m_loadingList.isExistNode(task) != -1) {
        m_loadingList.remove(task);
    }
    if (m_cancelList.isExistNode(task) != -1) {
        m_cancelList.remove(task);
    }
    if (task->m_archive->m_refCount <= 0) {
        OSReport("already release %d \n", task->m_archive->m_archiveFilePath);
    }
    m_database->release(task->m_resourceId);
    task->clear();
    m_freeList.addTail(task);
}

utArchiveManager::~utArchiveManager() {
    while (m_removeList.m_length != 0) {
        releaseTask(static_cast<LoadTask*>(m_removeList.getNode(0)));
    }
    while (m_loadingList.m_length != 0) {
        releaseTask(static_cast<LoadTask*>(m_loadedList.getNode(0)));
    }
    while (m_loadedList.m_length != 0) {
        releaseTask(static_cast<LoadTask*>(m_loadedList.getNode(0)));
    }
    while (m_cancelList.m_length != 0) {
        releaseTask(static_cast<LoadTask*>(m_cancelList.getNode(0)));
    }
    delete[] m_tasks;
    m_tasks = nullptr;
    delete m_database;
    m_database = nullptr;
}

utArchiveManager::LoadTask* utArchiveManager::isExist(utList* list, const char* path) {
    for (LoadTask* task = static_cast<LoadTask*>(list->getNode(0)); task != nullptr;
         task = static_cast<LoadTask*>(task->m_next)) {
        if (strcmp(fn_800146F0(task->m_archive->m_archiveFilePath).c_str(), fn_800146F0(path).c_str()) == 0) {
            if (strcmp(fn_80014480(task->m_archive->m_archiveFilePath).c_str(), fn_80014480(path).c_str()) == 0) {
                return task;
            }
        }
    }
    return nullptr;
}

u32 utArchiveManager::reqLoad(const char* path, Heaps::HeapType heap, u32 mode, int offset, int size, u32 id,
                              utArchiveEventObserver* observer, bool async) {
    LoadTask* source = nullptr;
    LoadTask* found = nullptr;
    if (path != nullptr) {
        found = isExist(&m_loadedList, path);
        if (found == nullptr) {
            found = isExist(&m_loadingList, path);
        }
    }
    if (found != nullptr) {
        if (mode == 1) {
            source = found;
        } else {
            found->m_refCount++;
            return found->m_resourceId;
        }
    }
    LoadTask* task = static_cast<LoadTask*>(m_freeList.getNode(0));
    if (task != nullptr) {
        m_freeList.remove(task);
        u32 sourceId = 0xFFFF;
        task->init();
        u32 result = 0;
        gfArchive* archive;
        if (source == nullptr) {
            archive = new (heap) gfArchive();
            if (strstr(path, ".lst") == nullptr) {
                if (async == true) {
                    result = fn_800151FC(archive, path, heap, offset, size, id);
                } else {
                    result = fn_80014ED4(archive, path, heap, offset);
                }
            }
        } else {
            archive = static_cast<gfArchive*>(operator new(sizeof(gfArchive), heap));
            if (archive != nullptr) {
                archive = fn_800137C0(archive, heap, source->m_archive);
            }
            sourceId = source->m_resourceId;
            result = 1;
        }
        if (result == 1) {
            task->m_resourceId = m_database->add(archive, true);
            task->m_archive = archive;
            task->m_sourceId = sourceId;
            if (async == true) {
                m_loadingList.addTail(task);
            } else {
                m_loadedList.addTail(task);
            }
            return task->m_resourceId;
        }
        OSReport("#failed utArchiveManager::reqLoad [%s] \n", path);
        delete archive;
        return 0xFFFF;
    }
    return 0xFFFF;
}

inline utArchiveManager::LoadTask* utArchiveManager::getTask(gfArchive* archive) {
    if (archive != nullptr) {
        for (u32 i = 0; i < m_taskCount; i++) {
            LoadTask* task = &m_tasks[i];
            if (task->m_resourceId != 0xFFFF) {
                gfArchive* other = m_database->get(task->m_resourceId);
                if (other != nullptr) {
                    if (other == archive) {
                        return task;
                    }
                } else {
                    return nullptr;
                }
            }
        }
    }
    return nullptr;
}

// HYPOTHESIS: utArchiveManager::addNoManageArchive(gfArchive*); em_manager.cpp still calls it by this name.
extern "C" u32 fn_800455E0(utArchiveManager* manager, gfArchive* archive) {
    utArchiveManager::LoadTask* found = manager->getTask(archive);
    if (found != nullptr) {
        found->m_refCount++;
        return found->m_resourceId;
    }
    utArchiveManager::LoadTask* task = static_cast<utArchiveManager::LoadTask*>(manager->m_freeList.getNode(0));
    if (task != nullptr) {
        manager->m_freeList.remove(task);
        task->init();
        task->m_resourceId = manager->m_database->add(archive, false);
        task->m_archive = archive;
        manager->m_loadedList.addTail(task);
        return task->m_resourceId;
    }
    return 0xFFFF;
}

u32 utArchiveManager::addNoManageArchive(void* data, int size, Heaps::HeapType heap) {
    LoadTask* task = static_cast<LoadTask*>(m_freeList.getNode(0));
    if (task != nullptr) {
        m_freeList.remove(task);
        gfArchive* archive = new (heap) gfArchive();
        archive->setFileImage(data, size, heap);
        task->init();
        task->m_resourceId = m_database->add(archive, false);
        task->m_archive = archive;
        m_loadedList.addTail(task);
        return task->m_resourceId;
    }
    return 0xFFFF;
}

// HYPOTHESIS: inline lookup of a copy task that still waits for its source archive.
inline utArchiveManager::LoadTask* utArchiveManager::getWaitingCopy(utList* list, u32 sourceId) {
    for (utArchiveManager::LoadTask* task = static_cast<utArchiveManager::LoadTask*>(list->getNode(0));
         task != nullptr; task = static_cast<utArchiveManager::LoadTask*>(task->m_next)) {
        if (task->m_sourceId == sourceId && !task->m_archive->unk9) {
            return task;
        }
    }
    return nullptr;
}

// HYPOTHESIS: inline reference drop shared by unload and process.
inline void utArchiveManager::unloadTask(LoadTask* task) {
    if (task->m_refCount != 0) {
        if (--task->m_refCount == 0) {
            if (m_loadingList.isExistNode(task) != -1 && !task->m_archive->unk9) {
                m_loadingList.remove(task);
                m_removeList.addTail(task);
            } else {
                releaseTask(task);
            }
        }
    } else {
        releaseTask(task);
    }
}

void utArchiveManager::process() {
    LoadTask* task = static_cast<LoadTask*>(m_loadingList.m_head);
    while (task != nullptr) {
        LoadTask* next = static_cast<LoadTask*>(task->m_next);
        if (task->m_archive->unk9 == true) {
            s32 result = 2;
            if (fn_80015DAC(task->m_archive) == 0) {
                result = 1;
            }
            task->m_observer->notifyComp(task->m_resourceId, result);
            task->m_sourceId = 0xFFFF;
            m_loadingList.remove(task);
            m_loadedList.addTail(task);
        }
        task = next;
    }

    task = static_cast<LoadTask*>(m_removeList.m_head);
    while (task != nullptr) {
        LoadTask* next = static_cast<LoadTask*>(task->m_next);
        bool waiting = false;
        if (getWaitingCopy(&m_loadingList, task->m_resourceId) != nullptr ||
            getWaitingCopy(&m_removeList, task->m_resourceId) != nullptr) {
            waiting = true;
        }
        if (!waiting && task->m_archive->unk9 == true) {
            unloadTask(task);
        }
        task = next;
    }

    task = static_cast<LoadTask*>(m_cancelList.m_head);
    while (task != nullptr) {
        LoadTask* next = static_cast<LoadTask*>(task->m_next);
        if (task->m_archive->unk9 == true || fn_8001588C(task->m_archive) == 1) {
            releaseTask(task);
        }
        task = next;
    }
}

// HYPOTHESIS: inline lookup of a list's task by resource id.
inline utArchiveManager::LoadTask* utArchiveManager::getListTask(utList* list, u32 resourceId) {
    if (resourceId != 0xFFFF) {
        for (utArchiveManager::LoadTask* task = static_cast<utArchiveManager::LoadTask*>(list->getNode(0));
             task != nullptr; task = static_cast<utArchiveManager::LoadTask*>(task->m_next)) {
            if (task->m_resourceId == resourceId) {
                return task;
            }
        }
    }
    return nullptr;
}

// HYPOTHESIS: utArchiveManager::getLoadResult(u32, s32*); em_manager.cpp and it_gen_archive.cpp
// still call it by this name.
extern "C" s32 fn_80045DC0(utArchiveManager* manager, u32 resourceId, s32* status) {
    if (resourceId != 0xFFFF) {
        utArchiveManager::LoadTask* task = manager->getListTask(&manager->m_loadedList, resourceId);
        if (task != nullptr) {
            s32 result = fn_80015DAC(task->m_archive);
            if (status != nullptr) {
                *status = result;
            }
            s32 ret = 2;
            if (result == 0) {
                ret = 1;
            }
            return ret;
        }
    }
    return 0;
}

// HYPOTHESIS: inline lookup of a task slot by resource id.
inline utArchiveManager::LoadTask* utArchiveManager::getTask(u32 resourceId) {
    if (resourceId != 0xFFFF) {
        for (u32 i = 0; i < m_taskCount; i++) {
            if (resourceId == m_tasks[i].m_resourceId) {
                return &m_tasks[i];
            }
        }
    }
    return nullptr;
}

// HYPOTHESIS: utArchiveManager::unload(u32); em_manager.cpp still calls it by this name.
extern "C" void fn_80045E64(utArchiveManager* manager, u32 resourceId) {
    utArchiveManager::LoadTask* task = manager->getTask(resourceId);
    if (task != nullptr) {
        manager->unloadTask(task);
    }
}

void utArchiveManager::cancel(u32 resourceId) {
    LoadTask* task = getTask(resourceId);
    if (task != nullptr) {
        if (task->m_refCount != 0) {
            if (--task->m_refCount == 0) {
                if (m_loadingList.isExistNode(task) != -1 && !task->m_archive->unk9) {
                    m_loadingList.remove(task);
                    if (fn_8001583C(task->m_archive) == 1) {
                        m_cancelList.addTail(task);
                        return;
                    }
                }
                releaseTask(task);
            }
        } else {
            releaseTask(task);
        }
    }
}

void* utArchiveManager::getData(u32 resourceId, ARCNodeType type, int fileIndex, int* size, int groupID, s32 archiveId) {
    gfArchive* archive = m_database->get(resourceId);
    if (archive != nullptr) {
        if (archiveId == -1) {
            return archive->getData(groupID, type, fileIndex, size, 0xFFFE);
        }
        return archive->getData(groupID, type, fileIndex, size, archiveId);
    }
    return nullptr;
}

nw4r::g3d::ResFileData* utArchiveManager::getResFileFromId(s32 p1, ARCNodeType type, u32 p3, u32 p4, u32 p5) {
    int size = 0;
    return static_cast<nw4r::g3d::ResFileData*>(getData(p1, type, p3, &size, p4, p5));
}

void* utArchiveManager::getPublicAddress(u32 paramArcId, const char* nodeName, u16 fileIndex, u32 groupID, u32 endian) {
    int size = 0;
    u8* data = static_cast<u8*>(getData(paramArcId, Data_Type_Misc, fileIndex, &size, groupID, endian));
    if (data != nullptr) {
        utRelocate relocate(data, size);
        return relocate.getPublicAddress(nodeName);
    }
    return nullptr;
}

void utArchiveManager::resolveReference(u32 paramArcId, u32 emCommonArcId, u32 p3, u32 p4, u32 p5, u32 p6) {
    int size = 0;
    utRelocate relocate(static_cast<u8*>(getData(paramArcId, Data_Type_Misc, p3, &size, p5, -1)), size);
    int commonSize = 0;
    utRelocate common(static_cast<u8*>(getData(emCommonArcId, Data_Type_Misc, p4, &commonSize, p6, -1)), commonSize);
    relocate.resolveReference(&common);
}

void utArchiveManager::resolveReferenceExternalData(u8* data, u32 size, u32 resourceId, u32 fileIndex, u32 groupID) {
    utRelocate relocate(data, size);
    int externalSize = 0;
    utRelocate external(static_cast<u8*>(getData(resourceId, Data_Type_Misc, fileIndex, &externalSize, groupID, -1)),
                        externalSize);
    relocate.resolveReference(&external);
}

bool utArchiveManager::isExistEffect(u32 resourceId) {
    if (resourceId != 0xFFFF) {
        gfArchive* archive = m_database->get(resourceId);
        if (archive != nullptr) {
            return fn_8001440C(archive);
        }
    }
    return false;
}

utArchiveEventObserverNull lbl_805A00C0;
