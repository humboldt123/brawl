#include <gf/gf_application.h>
#include <gf/gf_capture_util.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_shutdown_manager.h>
#include <gm/gm_global.h>
#include <if/if_snap_save.h>
#include <memory.h>
#include <mu/mu_msg.h>
#include <mu/mu_object.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <revolution/OS/OSTime.h>
#include <snd/snd_system.h>
#include <types.h>

// HYPOTHESIS: still fn_ in other units; names from the map (mu_collection.o, nt_etc_file.o).
extern "C" muCollection* fn_800C35CC(void* mem, int kind, nw4r::g3d::ResFile resFile, nw4r::g3d::ResFile resFile2,
                                     void* msgData, nw4r::g3d::ScnGroup* scnGroup, int drawPriority,
                                     Heaps::HeapType heap, Heaps::HeapType heap2, int unk, int unk2);
extern "C" bool fn_800C39D8(muCollection* collection);
extern "C" void fn_800C6E3C(muCollection* collection, int unk, int unk2);
extern "C" void fn_800C6E64(muCollection* collection, int drive, int unk, int unk2);
extern "C" nteFileDataPhoto* fn_80152970(void* mem, Heaps::HeapType heap);
extern "C" void fn_80153488(nteSection* section);
extern "C" void fn_80153498(nteSection* section, int unk);
extern "C" void fn_80153610(nteSection* section, void* data, s64 time, Heaps::HeapType heap, int unk, int unk2);
extern "C" u32 fn_80153D50(nteSection* section);
extern "C" nw4r::g3d::ScnGroup* Construct__Q34nw4r3g3d12ScnMdlExpandFP12MEMAllocatorPUliPQ34nw4r3g3d6ScnMdl(
    MEMAllocator* allocator, u32* size, int capacity, nw4r::g3d::ScnMdl* scnMdl);
extern "C" bool PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjPCc(nw4r::g3d::ScnGroup* expand,
                                                                         nw4r::g3d::ScnObj* obj, const char* nodeName);

// HYPOTHESIS: IfSnapSaveTask::makeDataSizeMsg (0x800F227C has no map line of its own, still fn_) writes the
// free size text and returns its length.
extern "C" int fn_800F227C(IfSnapSaveTask* task, char* buf, u32 size, int unk);

// HYPOTHESIS: the replay file data keeps its kind in the first word.
struct IfSnapSaveFileData {
    s32 m_kind;
};

// HYPOTHESIS: views of the game records read by the save flow.
struct IfSnapSaveAppData {
    u8 unk0[0x10];
    u8 unk10; // 0x10 HYPOTHESIS: the SD card can be used
};
struct IfSnapSaveStageEditData {
    u8 unk0[0x249C];
    u8 unk249C;
    u8 unk249D[3];
    u32 unk24A0;
};

struct IfSnapSaveModelData {
    const char* m_name;
    u8 m_start;
    u8 m_end;
    u8 m_priority;
};

struct IfSnapSaveStepFunc {
    void (*m_init)(IfSnapSaveTask* task);
    s32 (*m_main)(IfSnapSaveTask* task);
};

struct IfSnapSaveCursorData {
    float m_frame;
    u8 m_play;
    s32 m_media;
};

struct IfSnapSaveErrorData {
    u8 m_objIndex;
    const char* m_nodeName;
};

struct IfSnapSaveSavingData {
    u8 m_cursorState;
    u32 m_lineIndex;
};

static const IfSnapSaveModelData s_modelTbl[5] = {
    { "InfPause0001_TopN", 0, 0, 0 },
    { "InfPause0100_TopN", 1, 1, 1 },
    { "InfPause0101_TopN", 2, 2, 3 },
    { "InfPause0102_TopN", 3, 5, 2 },
    { "InfCmnWait_TopN", 5, 5, 4 },
};

static const IfSnapSaveStepFunc s_stepTbl[12] = {
    { IfSnapSaveTask::photoStepNoneInit, NULL },
    { IfSnapSaveTask::photoStepShootInit, IfSnapSaveTask::photoStepShootMain },
    { IfSnapSaveTask::photoStepCaptureInit, IfSnapSaveTask::photoStepCaptureMain },
    { IfSnapSaveTask::photoStepFadeInit, IfSnapSaveTask::photoStepFadeMain },
    { IfSnapSaveTask::photoStepMediaScanWaitInit, IfSnapSaveTask::photoStepMediaScanWaitMain },
    { IfSnapSaveTask::photoStepSelectMediaInit, IfSnapSaveTask::photoStepSelectMediaMain },
    { IfSnapSaveTask::photoStepErrorInit, IfSnapSaveTask::photoStepErrorMain },
    { IfSnapSaveTask::photoStepConfirmStartInit, IfSnapSaveTask::photoStepConfirmStartMain },
    { IfSnapSaveTask::photoStepConfirmInit, IfSnapSaveTask::photoStepConfirmMain },
    { IfSnapSaveTask::photoStepSavingInit, IfSnapSaveTask::photoStepSavingMain },
    { IfSnapSaveTask::photoStepDoneInit, IfSnapSaveTask::photoStepDoneMain },
    { IfSnapSaveTask::photoStepWaitMediaInit, IfSnapSaveTask::photoStepWaitMediaMain },
};

static const IfSnapSaveCursorData s_cursorTbl[10] = {
    { 0.0f, 0, 1 },  { 0.0f, 1, 1 },  { 10.0f, 0, 1 }, { 10.0f, 1, 1 }, { 19.0f, 0, 1 },
    { 20.0f, 0, 0 }, { 20.0f, 1, 0 }, { 30.0f, 0, 0 }, { 30.0f, 1, 0 }, { 39.0f, 0, 0 },
};

static const IfSnapSaveErrorData s_errorTbl[2] = {
    { 4, "pos01" },
    { 3, "pos00" },
};

static const IfSnapSaveSavingData s_savingTbl[2] = {
    { 8, 0x55 },
    { 3, 0x54 },
};

IfSnapSaveTask* IfSnapSaveTask::create(nw4r::g3d::ResFile* resFile, nw4r::g3d::ResFile* resFile2, void* msgData,
                                       nw4r::g3d::ScnGroup* scnGroup, int drawPriority, Heaps::HeapType heap,
                                       Heaps::HeapType heap2, Heaps::HeapType dataHeap) {
    IfSnapSaveTask* task = new (heap) IfSnapSaveTask();
    task->initProc(0, resFile, resFile2, msgData, scnGroup, drawPriority, heap, heap2, dataHeap, true);
    return task;
}

// HYPOTHESIS: IfSnapSaveTask::create for a replay (0x800F18C4, still fn_: IfMngr calls it by that name).
extern "C" IfSnapSaveTask* fn_800F18C4(nw4r::g3d::ResFile* resFile, nw4r::g3d::ResFile* resFile2, void* msgData,
                                       nw4r::g3d::ScnGroup* scnGroup, int drawPriority, IfSnapSaveFileData* data,
                                       s32 unkA4, Heaps::HeapType heap, Heaps::HeapType heap2,
                                       Heaps::HeapType dataHeap) {
    static const u8 kinds[4] = { 0xFF, 0, 1, 2 };
    IfSnapSaveTask* task = new (heap) IfSnapSaveTask();
    Heaps::HeapType sectionHeap = dataHeap;
    if (dataHeap == 0xFF) {
        sectionHeap = heap2;
    }
    fn_80153610(&task->m_section, data, OSGetTime(), sectionHeap, 0, 0);
    task->unkA4 = unkA4;
    task->initProc(kinds[data->m_kind], resFile, resFile2, msgData, scnGroup, drawPriority, heap, heap2, dataHeap,
                   false);
    return task;
}

IfSnapSaveTask::IfSnapSaveTask() : gfTask("IfSnapSave", Category_Info, 0xE, 6, true) {
    m_scnGroup = NULL;
    m_step = 0;
    unk68 = 2;
    unk6C = 0;
    m_captureBuffer = NULL;
    m_jpegBuffer = NULL;
    m_result = 0;
    fn_80153488(&m_section);
    unkA4 = 0;
    unkA8 = 0;
    m_collection = NULL;
    m_scanEnable = false;
    m_scanWait = 0;
    m_scanState = 0;
    m_photoData = NULL;
    m_jpegWork = NULL;
    unkC8 = 0;
    m_kind = 0;
    m_dataHeap = (Heaps::HeapType)0xFF;
    unkD8 = 0;
    m_scanned = false;
    for (int i = 0; i < 6; i++) {
        m_objects[i] = NULL;
    }
    m_expand = NULL;
    memset(&m_stepWork, 0, sizeof(m_stepWork));
    for (int i = 0; i < 2; i++) {
        m_media[i].m_status = 0;
        m_media[i].m_free = 0;
    }
}

void IfSnapSaveTask::initProc(int kind, nw4r::g3d::ResFile* resFile, nw4r::g3d::ResFile* resFile2, void* msgData,
                              nw4r::g3d::ScnGroup* scnGroup, int drawPriority, Heaps::HeapType heap,
                              Heaps::HeapType heap2, Heaps::HeapType dataHeap, bool capture) {
    static const u8 collectionKinds[4] = { 1, 2, 3, 0 };
    unk2C_b1 = false;
    m_kind = kind;
    createModel(resFile, drawPriority, heap, heap2);
    initMsg(msgData, heap, heap2);
    m_scnGroup = scnGroup;
    m_cursorState = 0;
    if (kind == 2) {
        IfSnapSaveStageEditData* stageEdit = reinterpret_cast<IfSnapSaveStageEditData*>(g_GameGlobal->m_stageEditData);
        unkD8 = stageEdit->unk249C;
        unk68 = stageEdit->unk24A0 != 0;
    }
    m_dataHeap = dataHeap;
    if (dataHeap == 0xFF) {
        m_dataHeap = heap2;
    }
    int collectionKind = collectionKinds[kind];
    void* mem = operator new(sizeof(muCollection), heap);
    if (mem != NULL) {
        mem = fn_800C35CC(mem, collectionKind, *resFile, *resFile2, msgData, scnGroup, drawPriority + 5, heap, heap, 1,
                          0xF0);
    }
    m_collection = static_cast<muCollection*>(mem);
    if (capture) {
        m_step = 1;
        m_captureBuffer = gfHeapManager::alloc(m_dataHeap, 0x12C000, 0x20);
        m_jpegBuffer = gfHeapManager::alloc(m_dataHeap, 0x19008);
        void* photo = operator new(0x14, heap);
        if (photo != NULL) {
            photo = fn_80152970(photo, m_dataHeap);
        }
        m_photoData = static_cast<nteFileDataPhoto*>(photo);
        m_jpegWork = new (heap) gfJpegEncodeWork();
    } else {
        m_step = 3;
        m_captureBuffer = NULL;
    }
    if (s_stepTbl[m_step].m_init != NULL) {
        s_stepTbl[m_step].m_init(this);
    }
}

IfSnapSaveTask::~IfSnapSaveTask() {
    if (m_jpegWork != NULL) {
        delete m_jpegWork;
    }
    if (m_photoData != NULL) {
        delete m_photoData;
    }
    if (m_collection != NULL) {
        m_collection->exit();
    }
    destroyModel();
    if (m_jpegBuffer != NULL) {
        gfHeapManager::free(m_jpegBuffer);
    }
    if (m_captureBuffer != NULL) {
        gfHeapManager::free(m_captureBuffer);
    }
    fn_80153498(&m_section, -1);
}

void IfSnapSaveTask::createModel(nw4r::g3d::ResFile* resFile, int drawPriority, Heaps::HeapType heap,
                                 Heaps::HeapType heap2) {
    int i;
    const IfSnapSaveModelData* data;
    for (i = 0, data = s_modelTbl; i < 5; i++, data++) {
        int count = data->m_start < data->m_end ? data->m_end - data->m_start : 1;
        for (int j = 0; j < count; j++) {
            m_objects[data->m_start + j] =
                MuObject::create(resFile, data->m_name, data->m_priority + drawPriority, NULL, heap2);
            m_objects[data->m_start + j]->changeAnimN(data->m_name);
            m_objects[data->m_start + j]->m_modelAnim->setUpdateRate(0.0f);
        }
    }
    m_expand = Construct__Q34nw4r3g3d12ScnMdlExpandFP12MEMAllocatorPUliPQ34nw4r3g3d6ScnMdl(
        gfHeapManager::getMEMAllocator(heap2), NULL, 2, m_objects[1]->m_sceneModel);
}

void IfSnapSaveTask::destroyModel() {
    MuMsg* msg = m_msg;
    if (m_expand != NULL) {
        m_expand->Destroy();
        m_expand = NULL;
    }
    msg->detachMuObject(m_objects[1]);
    msg->detachMuObject(m_objects[4]);
    msg->detachMuObject(m_objects[3]);
    delete msg;
    for (int i = 0; i < 6; i++) {
        if (m_objects[i] != NULL) {
            delete m_objects[i];
            m_objects[i] = NULL;
        }
    }
}

void IfSnapSaveTask::updateMediaFreeState() {
    static const u8 drives[2] = { 0, 1 };
    for (int i = 0; i < 2; i++) {
        if (i == 0 && reinterpret_cast<IfSnapSaveAppData*>(g_GameGlobal->m_appData)->unk10 == 0) {
            m_media[i].m_status = 5;
            m_media[i].m_free = 0;
        } else {
            muCollectionDrive* drive = &m_collection->m_drives[drives[i]];
            switch (drive->m_state) {
            case 0:
            case 1:
                if (drive->m_state == 1 && drive->unk1 == 0) {
                    m_media[i].m_status = 4;
                    m_media[i].m_free = 0;
                } else {
                    m_media[i].m_status = 1;
                    m_media[i].m_free = drive->m_free;
                }
                break;
            case 2:
                m_media[i].m_status = 2;
                m_media[i].m_free = 0;
                break;
            case 3:
                m_media[i].m_status = 3;
                m_media[i].m_free = 0;
                break;
            }
        }
    }
}

void IfSnapSaveTask::mediaCursorStateInit(int state) {
    MuObject* obj = m_objects[1];
    float play = s_cursorTbl[state].m_play;
    float frame = s_cursorTbl[state].m_frame;
    s32 media = s_cursorTbl[state].m_media;
    m_cursorState = state;
    obj->setFrameNode(frame);
    obj->m_modelAnim->m_anmObjChrRes->SetUpdateRate(play ? 1.0f : 0.0f);
    u32 blocks = (fn_80153D50(&m_section) + 0x1FFFF) / 0x20000;
    bool enough = false;
    if (m_media[media].m_status == 1 && m_media[media].m_free >= blocks) {
        enough = true;
    }
    m_objects[1]->setFrameMatCol(enough ? 0.0f : 1.0f);
}

void IfSnapSaveTask::mediaCursorStateMain() {
    float frame = m_objects[1]->m_modelAnim->m_anmObjChrRes->GetFrame();
    switch (m_cursorState) {
    case 1:
        if (frame >= 9.0f) {
            mediaCursorStateInit(2);
        }
        break;
    case 3:
        if (frame >= 19.0f) {
            mediaCursorStateInit(4);
        }
        break;
    case 6:
        if (frame >= 29.0f) {
            mediaCursorStateInit(7);
        }
        break;
    case 8:
        if (frame >= 39.0f) {
            mediaCursorStateInit(9);
        }
        break;
    case 9:
        break;
    }
}

void IfSnapSaveTask::processDefault() {
    s32 step = m_step;
    scanMediaMain();
    if (s_stepTbl[m_step].m_main != NULL) {
        m_step = s_stepTbl[m_step].m_main(this);
    }
    if (m_step != step && s_stepTbl[m_step].m_init != NULL) {
        s_stepTbl[m_step].m_init(this);
    }
    mediaCursorStateMain();
}

void IfSnapSaveTask::scanMediaMain() {
    switch (m_scanState) {
    case 0:
        if (m_scanEnable && m_collection->m_busy == 0) {
            if (!m_scanned) {
                fn_800C6E3C(m_collection, 0, 0);
                m_scanned = true;
            } else {
                fn_800C6E64(m_collection, 1, 0, 0);
            }
            m_scanState = 1;
        }
        break;
    case 1:
        if (m_collection->m_busy == 0) {
            updateMediaFreeState();
            m_scanWait = 60;
            m_scanState = 2;
        }
        break;
    case 2:
        if (--m_scanWait <= 0) {
            m_scanState = 0;
        }
        break;
    }
}

void IfSnapSaveTask::printMediaStatus(u32 msgIndex, u32 lineIndex, s32 status, u32 freeSize) {
    MuMsg* msg = m_msg;
    if (status == 1) {
        char buf[0x100];
        int length = fn_800F227C(this, buf, freeSize, 1);
        msg->printData(msgIndex, buf, length);
    } else {
        switch (status) {
        case 2:
            lineIndex = 0x4E;
            break;
        case 3:
            lineIndex = 0x51;
            break;
        case 4:
            lineIndex = 0x53;
            break;
        case 5:
            lineIndex = 0x30;
            break;
        }
        msg->printIndex(msgIndex, lineIndex);
    }
}

// HYPOTHESIS: IfSnapSaveTask::getState (0x800F2894, still fn_: IfMngr calls it by that name).
extern "C" s32 fn_800F2894(IfSnapSaveTask* task) {
    if (task->m_step != 0) {
        return 0;
    }
    if (fn_800C39D8(task->m_collection)) {
        return task->m_result;
    }
    return 0;
}

void IfSnapSaveTask::photoStepNoneInit(IfSnapSaveTask* task) { }

void IfSnapSaveTask::photoStepShootInit(IfSnapSaveTask* task) {
    for (int i = 0; i < 6; i++) {
        if (task->m_objects[i] != NULL) {
            task->m_scnGroup->Remove(task->m_objects[i]->m_sceneModel);
        }
    }
    if (task->m_expand != NULL) {
        task->m_scnGroup->Remove(task->m_expand);
    }
    MuObject* obj = task->m_objects[0];
    task->m_scnGroup->Insert(task->m_scnGroup->sceneItemsCount, obj->m_sceneModel);
    obj->setFrameNode(0.0f);
    obj->setFrameMatCol(0.0f);
    obj->m_modelAnim->m_anmObjChrRes->SetUpdateRate(1.0f);
    obj->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(1.0f);
    task->unkC8 = OSGetTime();
}

s32 IfSnapSaveTask::photoStepShootMain(IfSnapSaveTask* task) {
    MuObject* obj = task->m_objects[0];
    if (obj->m_modelAnim->m_anmObjChrRes->GetFrame() < 7.0f) {
        return task->m_step;
    }
    obj->setFrameNode(7.0f);
    obj->setFrameMatCol(7.0f);
    obj->m_modelAnim->m_anmObjChrRes->SetUpdateRate(0.0f);
    obj->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(0.0f);
    return 2;
}

void IfSnapSaveTask::photoStepCaptureInit(IfSnapSaveTask* task) {
    gfApplication* app = g_gfApplication;
    task->m_stepWork.m_count = 0;
    app->unkED |= 2;
    app->unk10C = task->m_captureBuffer;
    task->unkC0 = 0;
    memset(task->m_jpegBuffer, 0, 0x19008);
}

void IfSnapSaveTask::photoStepMediaScanWaitInit(IfSnapSaveTask* task) {
    task->m_scanEnable = true;
}

s32 IfSnapSaveTask::photoStepMediaScanWaitMain(IfSnapSaveTask* task) {
    // MATCH-ONLY: the early exit keeps the native per-drive branches.
    bool scanned;
    for (int i = 0; i < 2; i++) {
        if (task->m_media[i].m_status == 0) {
            scanned = false;
            goto done;
        }
    }
    scanned = true;
done:
    if (!scanned) {
        return task->m_step;
    }
    if (task->unkD8) {
        task->m_scnGroup->Insert(task->m_scnGroup->sceneItemsCount, task->m_expand);
        return 7;
    }
    return 5;
}

void IfSnapSaveTask::photoStepErrorInit(IfSnapSaveTask* task) {
    nw4r::g3d::ScnGroup* expand = task->m_expand;
    const IfSnapSaveErrorData* data = &s_errorTbl[task->unk68];
    IfSnapSaveMedia* media = &task->m_media[task->unk68];
    MuObject* obj = task->m_objects[data->m_objIndex];
    expand->Remove(obj->m_sceneModel);
    PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjPCc(expand, obj->m_sceneModel, data->m_nodeName);
    obj->setFrameVisible(1.0f);
    u32 lineIndex;
    switch (media->m_status) {
    case 1:
        lineIndex = 0x50;
        if (task->unk68 == 0) {
            lineIndex = 0x4F;
        }
        break;
    case 2:
        lineIndex = 0x4E;
        break;
    case 3:
        lineIndex = 0x51;
        break;
    case 4:
        lineIndex = 0x52;
        break;
    case 5:
        lineIndex = 0x30;
        break;
    }
    u32 msgIndex = 5;
    if (task->unk68 == 0) {
        msgIndex = 4;
    }
    task->m_msg->printIndex(msgIndex, lineIndex);
    task->m_stepWork.m_count = 0;
}

void IfSnapSaveTask::photoStepConfirmStartInit(IfSnapSaveTask* task) {
    task->mediaCursorStateInit(task->unk68 == 0 ? 6 : 1);
    int msgIndex = 2;
    if (task->unk68 == 0) {
        msgIndex = 1;
    }
    task->m_msg->printf(msgIndex, "");
    nw4r::g3d::ScnGroup* expand = task->m_expand;
    expand->Remove(task->m_objects[3]->m_sceneModel);
    expand->Remove(task->m_objects[4]->m_sceneModel);
}

s32 IfSnapSaveTask::photoStepConfirmStartMain(IfSnapSaveTask* task) {
    // MATCH-ONLY: a loop-shaped test reproduces the native block order.
    while (task->m_cursorState == 6 || task->m_cursorState == 1) {
        return task->m_step;
    }
    return 8;
}

void IfSnapSaveTask::photoStepConfirmInit(IfSnapSaveTask* task) {
    task->mediaCursorStateInit(task->unk68 == 0 ? 7 : 2);
    MuObject* obj = task->m_objects[2];
    task->m_scnGroup->Insert(task->m_scnGroup->sceneItemsCount, obj->m_sceneModel);
    task->m_stepWork.m_flag = true;
    obj->setFrameNode(0.0f);
}

void IfSnapSaveTask::photoStepSavingInit(IfSnapSaveTask* task) {
    MuObject* obj;
    const IfSnapSaveSavingData* data = &s_savingTbl[task->unk68];
    task->mediaCursorStateInit(data->m_cursorState);
    obj = task->m_objects[5];
    task->m_scnGroup->Insert(task->m_scnGroup->sceneItemsCount, obj->m_sceneModel);
    Vec3f pos(0.0f, -3.0f, 0.0f);
    obj->setTrans(&pos);
    obj->setFrame(0.0f);
    obj->m_modelAnim->setUpdateRate(1.0f);
    task->m_msg->printIndex(3, data->m_lineIndex);
    task->unk94 = 0x78;
    task->unk98 = 0;
    task->m_scanEnable = false;
    g_gfShutdownManager.unkD = false;
}

void IfSnapSaveTask::photoStepDoneInit(IfSnapSaveTask* task) {
    task->m_stepWork.m_count = 0;
    g_sndSystem->playSE((SndID)0x27, -1, 0, 0, -1);
    task->m_msg->printIndex(3, 0x4C);
}

void IfSnapSaveTask::photoStepWaitMediaInit(IfSnapSaveTask* task) {
    task->m_scanEnable = false;
}

s32 IfSnapSaveTask::photoStepWaitMediaMain(IfSnapSaveTask* task) {
    if (task->m_collection->m_busy == 0) {
        return 0;
    }
    return task->m_step;
}
