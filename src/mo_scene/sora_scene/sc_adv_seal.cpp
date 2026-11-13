#include <ad/ad_keep_manager.h>
#include <gf/gf_application.h>
#include <gf/gf_resource_loader.h>
#include <gf/gf_scene.h>
#include <gf/gf_task.h>
#include <gm/gm_global.h>
#include <mu/adv/mu_adv_seal.h>
#include <sc/sc_adv_seal.h>
#include <sr/sr_common.h>
#include <types.h>

scAdvSeal* scAdvSeal::create() {
    return new (Heaps::GlobalMode) scAdvSeal("scAdvSeal");
}

void scAdvSeal::setModule() { }

void scAdvSeal::start() {
    const char* path;
    u32 size;

    m_archive = new (Heaps::MenuResource) gfArchive;
    m_loader = new (Heaps::MenuResource) gfResourceLoader;
    muAdvSealTask::getResourcePackData(&path, &size);
    m_loader->loadResourceMemSizeOrder(Heaps::MenuResource, path, m_archive, size);
}

typedef gmAdvData::PadInfo PadInfo;

s32 scAdvSeal::process() {
    switch (m_state) {
        case 4:
            if (m_loader->isLoaded()) {
                PadInfo padInfo;
                PadInfo info;
                PadInfo defaultInfo;
                muAdvSealTaskParam param = { };
                if (g_GameGlobal->m_advData) {
                    info = g_GameGlobal->m_advData->m_padInfo[0];
                } else {
                    defaultInfo.m_flag.m_isSet = false;
                    defaultInfo.m_padNo = -1;
                    info = defaultInfo;
                }
                padInfo = info;
                param.unk4 = padInfo.m_padNo;
                m_task = muAdvSealTask::create(&param, m_archive);
                g_gfApplication->unkD0.endKeepScreen();
                m_state = 0;
            }
            break;
        case 0:
            if (m_task && m_task->unk40) {
                m_state = 1;
            }
            break;
        case 1:
            adKeepManager::getInstance()->useSealInfoPrepareStart();
            m_state = 2;
            break;
        case 2:
            if (adKeepManager::getInstance()->isLoadFinishSeal()) {
                adKeepManager::getInstance()->getSealInfoFromGameGlobal();
                m_state = 3;
            }
            break;
        case 3: {
            m_state = 4;
            gfSceneManager* mgr = gfSceneManager::getInstance();
            mgr->unk1 = 0;
            mgr->processStep = 2;
            break;
        }
    }
    return 0;
}

s32 scAdvSeal::exit() {
    if (m_task) {
        m_task->exit();
        m_task = nullptr;
    }
    if (m_archive) {
        delete m_archive;
        m_archive = nullptr;
    }
    if (m_loader) {
        delete m_loader;
        m_loader = nullptr;
    }
    return 0;
}
