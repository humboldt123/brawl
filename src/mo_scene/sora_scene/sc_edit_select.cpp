#include <cm/cm_camera_controller.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_scene.h>
#include <gf/gf_task.h>
#include <gm/gm_global.h>
#include <gm/gm_stage_edit_data.h>
#include <if/if_snap_save.h>
#include <memory.h>
#include <mu/mu_menuroot.h>
#include <mu/mu_object.h>
#include <mu/mu_replay.h>
#include <mu/mu_stage_edit_ui.h>
#include <sc/sc_edit_select.h>
#include <snd/snd_bgm.h>
#include <sr/sr_common.h>
#include <string.h>
#include <types.h>

// muStageEditSelectTask::create(scEditSelect*) of sora_menu_edit.rel (not named there yet).
extern "C" muStageEditSelectTask* fn_5_1210C(scEditSelect* scene);
// The nteSection functions of main.dol (constructor, setSection, extractData, destructor), as in mu_replay.cpp.
extern "C" void fn_80153488(nteSection* section);
extern "C" void fn_80153504(nteSection* section, int index);
extern "C" muReplayData* fn_80153818(nteSection* section, Heaps::HeapType heap);
extern "C" void fn_80153498(nteSection* section, int flags);

scEditSelect* scEditSelect::create() {
    return new (Heaps::GlobalMode) scEditSelect("scEditSelect");
}

void scEditSelect::setModule() {
    setLoadModule("sora_menu_name.rel", false);
    setLoadModule("sora_menu_collect_viewer.rel", false);
    setLoadModule("sora_menu_edit.rel", false);
}

void scEditSelect::start() {
    m_work = static_cast<Work*>(gfHeapManager::alloc(Heaps::MenuInstance, sizeof(Work)));
    m_work->m_root = MenuRoot::create("StageEditSelectTask", 0x27, "/menu/defaultcamera/CharacterSelect.brres");
    m_work->m_objProcess = muObjProcessTask::create(Heaps::MenuInstance);
    m_work->m_task = fn_5_1210C(this);
    CameraController::getInstance()->changeInput(CameraController::MenuFixed);
}

s32 scEditSelect::process() {
    return 0;
}

// Leaving the browser: with modes 0 and 1 the chosen stage file is read into the stage edit record (mode 0 marks it
// HYPOTHESIS: as a new copy); with mode 2 the record is cleared for a new stage of the chosen size and music.
s32 scEditSelect::exit() {
    gmStageEditData* editData;
    muStageEditSelectTask* task = m_work->m_task;
    editData = g_GameGlobal->m_stageEditData;
    int mode = gfSceneManager::getInstance()->unk1;
    switch (mode) {
    case 0:
    case 1: {
        nteSection section;
        fn_80153488(&section);
        fn_80153504(&section, task->m_work->m_unk1CF0);
        muReplayData* data = fn_80153818(&section, Heaps::MenuResource);
        if (data != NULL) {
            memcpy(editData, static_cast<u8*>(data->m_data) + 0x34, 0x2470);
            memcpy(editData->m_unk2470, static_cast<u8*>(data->m_data) + 0xA, 0x2C);
            editData->m_cursor = task->m_work->m_curMedia;
            strcpy(editData->m_name, static_cast<char*>(task->m_work->m_collection) + 0x15D);
        }
        editData->m_isNew = mode == 0;
        delete data;
        fn_80153498(&section, -1);
        muStageEditSelectTask::playBGM();
        break;
    }
    case 2: {
        u8 sizes[3] = {0, 2, 1};
        muStageEditSelectTask::NewData* newData = &task->getWork()->m_newData;
        u8 size = sizes[newData->unk4];
        u16 bgm = newData->getBgmData()->m_playId;
        u8 unk0 = newData->unk0;
        memset(editData, 0, sizeof(gmStageEditData));
        editData->m_unk14 = size;
        editData->m_unk15 = unk0;
        editData->m_unk18 = bgm;
        editData->m_cursor = task->m_work->m_curMedia;
        muStageEditSelectTask::playBGM();
        break;
    }
    default:
        break;
    }
    if (m_work == NULL) {
        return 0;
    }
    m_work->m_root->exit();
    m_work->m_task->exit();
    m_work->m_objProcess->exit();
    gfHeapManager::free(m_work);
    m_work = NULL;
    return 0;
}

void scEditSelect::setMotionObj(MuObject** objs, int num) {
    muObjProcessTask* objProcess = m_work->m_objProcess;
    objProcess->m_objs = objs;
    objProcess->m_count = num;
}
