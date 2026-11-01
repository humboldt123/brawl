#include <types.h>

#include <gf/gf_scene.h>
#include <gm/gm_global.h>
#include <mu/mu_menu_main.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <mu/mu_name.h>
#include <snd/snd_system.h>

#include <nw4r/g3d/g3d_scnobj.h>

#include <mu/mu_proc_opt_btn.h>

void muProcOptBtn::init(nw4r::g3d::ScnGroup* group, int from) {
    m_group = group;
    m_unk670 = 0;
    m_unk66C_0 = false;
    m_unk710 = 0;
    m_unk712 = 0;
    m_nameTask = NULL;
    m_shown = false;
    m_panel->changeProcess1(0x17, 0x42);
    m_msgNo = 0x42;
    MuObject* objects[4] = {
        m_objs[Obj_NameList0], m_objs[Obj_NameList0 + 1], m_objs[Obj_NameList0 + 2], m_objs[Obj_NameRemote],
    };
    nw4r::g3d::ScnMdl* mdls[2] = {
        reinterpret_cast<nw4r::g3d::ScnMdl*>(m_mdls[Mdl_Name0]), reinterpret_cast<nw4r::g3d::ScnMdl*>(m_mdls[Mdl_Name0 + 1]),
    };
    muMenuMain* menu = dynamic_cast<muMenuMain*>(gfSceneManager::getInstance()->m_currentScene);
    if (m_nameTask != NULL) {
        m_nameTask->exit();
        m_nameTask = NULL;
    }
    m_nameTask = muNameTask::create1(group, 0, objects, mdls, &menu->m_resFileA98, 0xF0, 0x19, NULL, NULL, m_remoteRes,
                                     m_msgData, m_wndRes);
    m_unk664 = Step_Name;
}

int muProcOptBtn::proc() {
    int ret = 0;
    switch (m_unk664) {
    case Step_Name:
        ret = selectNameMain();
        break;
    case Step_Done:
        ret = 3;
        break;
    }
    return ret;
}

int muProcOptBtn::selectNameMain() {
    int ret = 0;
    if (!m_shown && m_nameTask->m_step != muNameTask::Step_Appear) {
        m_shown = true;
        ret = 4;
    }
    switch (m_nameTask->m_resultKind) {
    case muNameTask::Result_Chosen:
    case muNameTask::Result_NewName: {
        // A name was chosen or made: its controls are set in the Controls scene
        gmSetRule* rule = g_GameGlobal->m_setRule;
        rule->m_ctrlNameNo = m_nameTask->m_resultNameNo;
        rule->m_ctrlNamePlayerNo = m_nameTask->m_playerNo;
        m_nameTask->clearResult();
        m_nameTask->deleteModel();
        m_nameTask->exit();
        m_nameTask = NULL;
        exitScene(0x16, true);
        break;
    }
    case muNameTask::Result_Cancelled:
        // Back to the Options screen
        m_result = 5;
        ret = 1;
        m_nameTask->clearResult();
        break;
    case muNameTask::Result_Closed:
        m_unk664 = Step_Done;
        ret = 3;
        m_nameTask->exit();
        m_nameTask = NULL;
        break;
    }
    if (m_nameTask != NULL) {
        int msgNo = (m_nameTask->m_step == muNameTask::Step_NameEntry) + 0x42;
        if (m_msgNo != msgNo) {
            m_panel->setMsg(msgNo, 1, 0);
            m_msgNo = msgNo;
        }
    }
    return ret;
}

void muProcOptBtn::exit() {
    m_group->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_mdls[Mdl_Name0]));
    m_group->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_mdls[Mdl_Name0 + 1]));
    if (m_nameTask != NULL) {
        m_nameTask->exit();
        m_nameTask = NULL;
    }
}

void muProcOptBtn::playSE_CursorMove() {
    g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
}
