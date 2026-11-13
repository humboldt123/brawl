#include <cm/cm_camera_controller.h>
#include <gf/gf_application.h>
#include <gf/gf_archive.h>
#include <gf/gf_keep_fb.h>
#include <gf/gf_scene.h>
#include <memory.h>
#include <mu/mu_boot_nand.h>
#include <mu/mu_boot_network.h>
#include <sc/sc_boot.h>
#include <sr/sr_common.h>
#include <types.h>

// Loads an archive file into an archive object (main.dol, not named yet).
extern "C" void fn_800151FC(gfArchive* archive, const char* path, Heaps::HeapType heap, int offset, int size, u32 id);

scBoot* scBoot::create() {
    return new (Heaps::GlobalMode) scBoot("scBoot");
}

void scBoot::start() {
    m_step = 0;
}

s32 scBoot::process() {
    switch (m_step) {
    case 0:
        CameraController::getInstance()->changeInput(CameraController::MenuFixed);
        m_archive = new (Heaps::MenuInstance) gfArchive();
        fn_800151FC(m_archive, "/menu/boot/Boot.pac", Heaps::MenuResource, 0, 0, 0xFF);
        m_nandTask = muBootNandTask::create();
        m_nandTask->initialize();
        m_networkTask = muBootNetworkTask::create();
        m_networkTask->initialize();
        m_step = 1;
        break;
    case 1:
        if (m_archive->unk9) {
            m_step = 2;
        }
        break;
    case 2:
        m_nandTask->createData(m_archive);
        m_networkTask->createData(m_archive);
        g_gfApplication->unkD0.endKeepScreen();
        m_step = 3;
        break;
    case 3:
        m_nandTask->m_step = 1;
        m_step = 4;
        break;
    case 4:
        if (m_nandTask->m_step == 0x3D) {
            m_step = 5;
        }
        break;
    case 5:
        m_step = 6;
        break;
    case 6:
        m_networkTask->m_step = 1;
        m_step = 7;
        break;
    case 7:
        if (m_networkTask->m_step == 0xF) {
            m_step = 8;
        }
        break;
    case 8:
        m_step = 9;
        break;
    case 9: {
        delete m_archive;
        m_archive = NULL;
        m_nandTask->release();
        m_nandTask->exit();
        m_nandTask = NULL;
        m_networkTask->destroyData();
        m_networkTask->releaseWnd();
        m_networkTask->exit();
        m_networkTask = NULL;
        g_gfApplication->unkD0.startKeepScreen();
        gfSceneManager* mgr = gfSceneManager::getInstance();
        mgr->unk1 = 0;
        mgr->processStep = 2;
        m_step++;
        break;
    }
    }
    return 0;
}

s32 scBoot::exit() {
    return 0;
}

void scBoot::setModule() {
    setLoadModule("sora_menu_boot.rel", false);
}
