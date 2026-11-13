#include <gf/gf_application.h>
#include <gf/gf_keep_fb.h>
#include <gf/gf_scene.h>
#include <gf/gf_task.h>
#include <memory.h>
#include <sc/sc_simple_result.h>
#include <sr/sr_common.h>
#include <types.h>

// main.dol result task helpers, unnamed in the symbols yet.
// HYPOTHESIS: fn_800ED474 is the ifSimpleResultTask constructor and fn_800EE5EC its isEnd (main.dol map neighbours).
extern "C" gfTask* fn_800ED474(void* task);
extern "C" bool fn_800EE5EC(gfTask* task);

scSimpleResult* scSimpleResult::create() {
    return new (Heaps::GlobalMode) scSimpleResult("scSimpleResult");
}

void scSimpleResult::start() {
    gfTask* task = static_cast<gfTask*>(operator new(0x27C, Heaps::MenuInstance));
    if (task != NULL) {
        task = fn_800ED474(task);
    }
    task->init();
    m_task = task;
}

s32 scSimpleResult::process() {
    if (fn_800EE5EC(m_task)) {
        gfSceneManager* mgr = gfSceneManager::getInstance();
        mgr->unk1 = 0;
        mgr->processStep = 2;
        g_gfApplication->unkD0.startKeepScreen();
    }
    return 0;
}

s32 scSimpleResult::exit() {
    m_task->exit();
    m_task = nullptr;
    return 0;
}
