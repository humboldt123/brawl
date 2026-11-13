#include <gf/gf_archive.h>
#include <gf/gf_scene.h>
#include <gf/gf_task.h>
#include <memory.h>
#include <mu/mu_stage_edit_ui.h>
#include <sc/sc_edit_select.h>
#include <sr/sr_common.h>
#include <types.h>

// HYPOTHESIS: what the editor is created with (seven words; only their values are known).
struct muStageEditUIParam {
    int unk00;
    int unk04;
    int unk08;
    int unk0C;
    int unk10;
    int unk14;
    int unk18;
};

// The muStageEditUI constructor of sora_menu_edit.rel (not named there yet).
extern "C" muStageEditUI* fn_5_0(void* self, gfArchive* archive, muStageEditUIParam* param, int unk);
// gfArchive::readFile of main.dol.
extern "C" u32 fn_80014ED4(gfArchive* archive, const char* path, Heaps::HeapType heap, int offset);

// The archive of the editor parts.
static gfArchive* s_archive;

scEditMenu* scEditMenu::create() {
    return new (Heaps::GlobalMode) scEditMenu("scEditMenu");
}

void scEditMenu::start() {
    s_archive = new (Heaps::MenuResource) gfArchive;
    fn_80014ED4(s_archive, "/stage/melee/STGEDIT_2.ORG", Heaps::MenuResource, 0);
    s_archive->getData(static_cast<ARCNodeType>(2), 10, 0xFFFE);
    gfArchive* archive = s_archive;
    muStageEditUIParam param;
    param.unk00 = 2;
    param.unk04 = 2;
    param.unk08 = 0;
    param.unk0C = 0;
    param.unk10 = 0;
    param.unk14 = 0;
    param.unk18 = 0x4917;
    muStageEditUI* ui = static_cast<muStageEditUI*>(operator new(0xC478C, Heaps::MenuInstance));
    if (ui != NULL) {
        ui = fn_5_0(ui, archive, &param, 0);
    }
    ui->init();
    m_ui = ui;
}

s32 scEditMenu::process() {
    if (m_ui->m_exitRequest != 0) {
        gfSceneManager* mgr = gfSceneManager::getInstance();
        mgr->unk1 = 0;
        mgr->processStep = 2;
    }
    return 0;
}

s32 scEditMenu::exit() {
    m_ui->exit();
    m_ui = NULL;
    delete s_archive;
    return 0;
}
