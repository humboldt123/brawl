#include <cm/cm_camera_controller.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_scene.h>
#include <gf/gf_task.h>
#include <if/if_utility.h>
#include <memory.h>
#include <mu/mu_menuroot.h>
#include <mu/mu_object.h>
#include <sc/sc_snap_shot.h>
#include <snd/snd_bgm.h>
#include <snd/snd_system.h>
#include <sr/sr_common.h>
#include <types.h>

// The viewer of sora_menu_snap_shot.rel (declared in full in mu/mu_snap_shot.h, which has its own view of the scene).
class muSnapShotTask : public utility::StandardMenu {
public:
    static muSnapShotTask* create(scSnapShot* scene);
};

// Gives the object process of the menu the objects it updates. sora_menu_snap_shot.rel imports it by this name.
extern "C" void fn_1_7A94(scSnapShot* scene, MuObject** objs, int num);

scSnapShot* scSnapShot::create() {
    return new (Heaps::GlobalMode) scSnapShot("scSnapShot");
}

void scSnapShot::setModule() {
    setLoadModule("sora_menu_name.rel", false);
    setLoadModule("sora_menu_collect_viewer.rel", false);
    setLoadModule("sora_menu_snap_shot.rel", false);
}

void scSnapShot::start() {
    m_work = static_cast<Work*>(gfHeapManager::alloc(Heaps::MenuInstance, sizeof(Work)));
    m_work->m_root = MenuRoot::create("SnapShotTask", 0x21, "/menu/defaultcamera/CharacterSelect.brres");
    m_work->m_objProcess = muObjProcessTask::create(Heaps::MenuInstance);
    m_work->m_task = muSnapShotTask::create(this);
    CameraController::getInstance()->changeInput(CameraController::MenuFixed);
    m_work->m_bgmStarted = false;
}

// The album music starts once the viewer is ready.
s32 scSnapShot::process() {
    Work* work = m_work;
    if (work->m_task->m_unk42_6 && !work->m_bgmStarted) {
        work->m_bgmStarted = true;
        g_sndSystem->playBGM(0x270c, 0, true);
        lbl_805A01D8->m_category = 45;
    }
    return 0;
}

s32 scSnapShot::exit() {
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

extern "C" void fn_1_7A94(scSnapShot* scene, MuObject** objs, int num) {
    muObjProcessTask* objProcess = scene->m_work->m_objProcess;
    objProcess->m_objs = objs;
    objProcess->m_count = num;
}
