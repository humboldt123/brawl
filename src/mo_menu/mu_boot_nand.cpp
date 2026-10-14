#include <gf/gf_heap_manager.h>
#include <gf/gf_homemenu.h>
#include <gf/gf_shutdown_manager.h>
#include <gm/gm_global.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <revolution/OS.h>
#include <types.h>

#include <mu/mu_boot_nand.h>
#include <mu/mu_simple_ending.h>

// main.dol functions with names that the shared headers do not declare.
// Creates the camera task that draws the menu models (the task keeps the scene group at +0x48)
extern "C" muEndCameraTask* fn_800AFBFC(const char* name, int heap, gfArchive* archive, int unk);
// GameGlobal::setSaveDataFlags(flags) and GameGlobal::setSystemData(data): they apply the parts of the save data that
// are in use and the loaded system data.
// HYPOTHESIS: the names; they are called after the check and after the load of the system data.
extern "C" void fn_8004EAE0(GameGlobal* global, u8 flags);
extern "C" void fn_8004EB20(GameGlobal* global, void* data);
// nw4r::g3d::ResFile::Release()
extern "C" void Release__Q34nw4r3g3d7ResFileFv(nw4r::g3d::ResFile* resFile);

muBootNandTask* muBootNandTask::create() {
    return new (Heaps::MenuInstance) muBootNandTask("NandBoot");
}

muBootNandTask::~muBootNandTask() { }

// Takes the open window away.
inline void muBootNandTask::closeWnd() {
    if (m_wnd != NULL) {
        fn_800BF830(m_wnd);
        m_wnd->exit();
        m_wnd = NULL;
    }
}

// Opens a window (the old one is taken away first) and marks it.
inline void muBootNandTask::openWnd(int kind, void* msgData, int msgId, int defaultButton) {
    closeWnd();
    m_wnd = fn_800BEEB8(kind, msgData, msgId, &m_resFileWnd, m_cameraTask->m_scnRoot, 0x14, defaultButton, Heaps::MenuInstance,
                        Heaps::MenuResource, 0, 0, 0xF0);
    m_wnd->m_unk120 = 1;
}

// Which of the things that the check found has to be done: the parts that are not wanted (the player said no) are
// skipped.
// HYPOTHESIS: the kinds 1 and 2 are the ones that the player can say no to.
static bool isWanted(const muBootNandTask* task, int kind) {
    if (kind == 1 && task->m_skipDone == 1) {
        return false;
    }
    if (kind == 2 && task->m_skipDone == 1) {
        return false;
    }
    return true;
}

// The steps of the check of the save data. The windows tell the player what is wrong with the save data and ask
// whether it may be made new; the recorder does the work while the "please wait" panel is shown.
void muBootNandTask::processDefault() {
    switch (m_step) {
    case 0x01:
        m_step = 2;
        break;
    case 0x02:
        // all the parts of the save data are in use until the player says no
        m_flags |= 0x1F;
        fn_8004EAE0(g_GameGlobal, m_flags);
        fn_80030954(m_recorder, Heaps::MenuResource, 8);
        m_step = 3;
        break;
    case 0x03:
        // the check is running
        if (m_recorder->m_busy == 0) {
            if (m_recorder->m_result == 0) {
                m_checkResult = m_recorder->m_checkResult;
                m_step = 4;
            } else {
                OSReport("SaveDataCheckError...\n");
                m_step = 0x3C;
            }
        }
        break;
    case 0x04:
        switch (m_checkResult) {
        case 0:
            m_step = 0x37;
            break;
        case 1:
            m_requireIndex = 0;
            m_require = m_recorder->m_require;
            m_step = 0x19;
            break;
        case 2:
            m_step = 5;
            break;
        }
        break;
    case 0x05:
        // asks whether the save data may be made new
        openWnd(1, m_msgData, 0, 0);
        fn_800BF97C(m_wnd, m_msgData, 0xE);
        fn_800BFA04(m_wnd, m_msgData, 0xF);
        m_step = 6;
        break;
    case 0x06:
        if (m_wnd->m_step == 3) {
            m_answer = m_wnd->m_selected;
            m_step = 7;
        }
        break;
    case 0x07:
        closeWnd();
        if (m_answer == 1) {
            m_step = 8;
        }
        if (m_answer == 0) {
            m_step = 0x16;
        }
        break;
    case 0x08: {
        openWnd(3, m_msgData, 1, -1);
        fn_800309F8(m_recorder, Heaps::MenuResource, 8);
        nw4r::g3d::ScnGroup* root = m_cameraTask->m_scnRoot;
        root->Insert(root->sceneItemsCount, reinterpret_cast<nw4r::g3d::ScnObj*>(m_object->m_scnMdl));
        m_object->setFrame(0.0f);
        m_step = 9;
        break;
    }
    case 0x09:
        if (g_gfHomeMenu->setBan(1, 0) == 1) {
            m_step = 0xA;
        }
        break;
    case 0x0A:
        if (m_recorder->m_busy == 0) {
            fn_800BF828(m_wnd, 2);
            nw4r::g3d::ScnGroup* root = m_cameraTask->m_scnRoot;
            root->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_object->m_scnMdl));
            m_step = 0xB;
        }
        break;
    case 0x0B:
        if (m_wnd->m_step == 3) {
            m_step = 0xC;
        }
        break;
    case 0x0C:
        g_gfHomeMenu->setBan(0, 0);
        closeWnd();
        switch (m_recorder->m_result) {
        case 0:
            m_step = 0xD;
            break;
        case 1:
            m_step = 0x10;
            break;
        case 2:
            m_step = 0x13;
            break;
        case 3:
            OSReport("CreateNewDataFailed...\n");
            m_step = 0x3C;
            break;
        }
        break;
    case 0x0D:
        openWnd(0, m_msgData, 2, 0);
        m_step = 0xE;
        break;
    case 0x0E:
        if (m_wnd->m_step == 3) {
            m_step = 0xF;
        }
        break;
    case 0x0F:
        closeWnd();
        m_step = 2;
        break;
    case 0x10:
        openWnd(0, m_msgData, 3, 0);
        m_step = 0x11;
        break;
    case 0x11:
        if (m_wnd->m_step == 3) {
            m_step = 0x12;
        }
        break;
    case 0x12:
        closeWnd();
        m_step = 0x16;
        break;
    case 0x13:
        openWnd(0, m_msgData, 4, 0);
        m_step = 0x14;
        break;
    case 0x14:
        if (m_wnd->m_step == 3) {
            m_step = 0x15;
        }
        break;
    case 0x15:
        closeWnd();
        m_step = 0x16;
        break;
    case 0x16:
        // asks whether the game should be started without the save data
        openWnd(1, m_msgData, 5, 0);
        fn_800BF97C(m_wnd, m_msgData, 0x10);
        fn_800BFA04(m_wnd, m_msgData, 0x11);
        m_step = 0x17;
        break;
    case 0x17:
        if (m_wnd->m_step == 3) {
            m_answer = m_wnd->m_selected;
            m_step = 0x18;
        }
        break;
    case 0x18:
        closeWnd();
        if (m_answer == 1) {
            m_flags &= 0xE0;
            m_step = 0x37;
        }
        if (m_answer == 0) {
            m_step = 0x3B;
        }
        break;
    case 0x19: {
        // goes through the things that have to be done
        int kind;
        while (!isWanted(this, kind = m_require[m_requireIndex].m_kind)) {
            m_requireIndex++;
        }
        m_requireKind = kind;
        if (kind == 6) {
            m_step = 0x37;
        } else if (kind == 0) {
            m_skipDone = 1;
            m_step = 0x1A;
        } else if (kind == 4) {
            m_step = 0x33;
        } else {
            m_step = 0x1A;
        }
        break;
    }
    case 0x1A:
        openWnd(1, m_msgData, m_requireKind + 6, 0);
        fn_800BF97C(m_wnd, m_msgData, 0xE);
        fn_800BFA04(m_wnd, m_msgData, 0xF);
        m_step = 0x1B;
        break;
    case 0x1B:
        if (m_wnd->m_step == 3) {
            m_answer = m_wnd->m_selected;
            m_step = 0x1C;
        }
        break;
    case 0x1C:
        closeWnd();
        if (m_answer == 1) {
            if (m_requireKind == 0) {
                m_step = 0x2E;
            } else {
                m_step = 0x1D;
            }
        }
        if (m_answer == 0) {
            m_step = 0x2B;
        }
        break;
    case 0x1D: {
        openWnd(3, m_msgData, 1, -1);
        fn_800309F8(m_recorder, Heaps::MenuResource, m_require[m_requireIndex].m_kind);
        nw4r::g3d::ScnGroup* root = m_cameraTask->m_scnRoot;
        root->Insert(root->sceneItemsCount, reinterpret_cast<nw4r::g3d::ScnObj*>(m_object->m_scnMdl));
        m_object->setFrame(0.0f);
        m_step = 0x1E;
        break;
    }
    case 0x1E:
        if (g_gfHomeMenu->setBan(1, 0) == 1) {
            m_step = 0x1F;
        }
        break;
    case 0x1F:
        if (m_recorder->m_busy == 0) {
            fn_800BF828(m_wnd, 2);
            nw4r::g3d::ScnGroup* root = m_cameraTask->m_scnRoot;
            root->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_object->m_scnMdl));
            m_step = 0x20;
        }
        break;
    case 0x20:
        if (m_wnd->m_step == 3) {
            m_step = 0x21;
        }
        break;
    case 0x21:
        g_gfHomeMenu->setBan(0, 0);
        closeWnd();
        switch (m_recorder->m_result) {
        case 0:
            m_step = 0x22;
            break;
        case 1:
            m_step = 0x25;
            break;
        case 2:
            m_step = 0x28;
            break;
        case 3:
            OSReport("CreateRenewDataFailed...\n");
            m_step = 0x3C;
            break;
        }
        break;
    case 0x22:
        openWnd(0, m_msgData, m_requireKind + 0xA, 0);
        m_step = 0x23;
        break;
    case 0x23:
        if (m_wnd->m_step == 3) {
            m_step = 0x24;
        }
        break;
    case 0x24:
        closeWnd();
        m_step = 0x19;
        m_requireIndex++;
        break;
    case 0x25:
        openWnd(0, m_msgData, 3, 0);
        m_step = 0x26;
        break;
    case 0x26:
        if (m_wnd->m_step == 3) {
            m_step = 0x27;
        }
        break;
    case 0x27:
        closeWnd();
        m_step = 0x2B;
        break;
    case 0x28:
        openWnd(0, m_msgData, m_requireKind + 0x12, 0);
        m_step = 0x29;
        break;
    case 0x29:
        if (m_wnd->m_step == 3) {
            m_step = 0x2A;
        }
        break;
    case 0x2A:
        closeWnd();
        m_step = 0x2B;
        break;
    case 0x2B:
        // asks whether this part of the save data may be left out
        openWnd(1, m_msgData, m_requireKind + 0x16, 0);
        fn_800BF97C(m_wnd, m_msgData, 0x10);
        fn_800BFA04(m_wnd, m_msgData, 0x11);
        m_step = 0x2C;
        break;
    case 0x2C:
        if (m_wnd->m_step == 3) {
            m_answer = m_wnd->m_selected;
            m_step = 0x2D;
        }
        break;
    case 0x2D:
        closeWnd();
        m_requireIndex++;
        if (m_answer == 1) {
            m_flags &= ~(1 << m_requireKind);
            if (m_requireKind == 0) {
                m_flags &= 0xF8;
            }
            m_step = 0x19;
        }
        if (m_answer == 0) {
            m_step = 0x3B;
        }
        break;
    case 0x2E: {
        // the record of the game is made new
        void* msgData = m_msgData;
        openWnd(3, msgData, 1, -1);
        m_requireKind = 0;
        fn_800309F8(m_recorder, Heaps::MenuResource, 7);
        nw4r::g3d::ScnGroup* root = m_cameraTask->m_scnRoot;
        root->Insert(root->sceneItemsCount, reinterpret_cast<nw4r::g3d::ScnObj*>(m_object->m_scnMdl));
        m_object->setFrame(0.0f);
        m_step = 0x2F;
        break;
    }
    case 0x2F:
        if (g_gfHomeMenu->setBan(1, 0) == 1) {
            m_step = 0x30;
        }
        break;
    case 0x30:
        if (m_recorder->m_busy == 0) {
            fn_800BF828(m_wnd, 2);
            nw4r::g3d::ScnGroup* root = m_cameraTask->m_scnRoot;
            root->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_object->m_scnMdl));
            m_step = 0x31;
        }
        break;
    case 0x31:
        if (m_wnd->m_step == 3) {
            m_step = 0x32;
        }
        break;
    case 0x32:
        g_gfHomeMenu->setBan(0, 0);
        closeWnd();
        m_requireKind = 0;
        switch (m_recorder->m_result) {
        case 0:
            m_step = 0x22;
            break;
        case 1:
            m_step = 0x25;
            break;
        case 2:
            m_step = 0x28;
            break;
        case 3:
            OSReport("CreateRenewSystemDataFailed...\n");
            m_step = 0x3C;
            break;
        }
        break;
    case 0x33:
        // the banner is made new
        fn_800309F8(m_recorder, Heaps::MenuResource, 4);
        m_step = 0x34;
        break;
    case 0x34:
        if (g_gfHomeMenu->setBan(1, 0) == 1) {
            m_step = 0x35;
        }
        break;
    case 0x35:
        if (m_recorder->m_busy == 0) {
            m_step = 0x36;
        }
        break;
    case 0x36:
        g_gfHomeMenu->setBan(0, 0);
        m_requireIndex++;
        switch (m_recorder->m_result) {
        case 0:
            m_step = 0x19;
            break;
        case 1:
        case 2:
        case 3:
            OSReport("CreateBannerFailed...\n");
            m_step = 0x19;
            break;
        }
        break;
    case 0x37:
        // the parts of the save data that are in use are told to the game, then the system data is loaded
        fn_8004EAE0(g_GameGlobal, m_flags);
        if (fn_800305B0(0) == 1) {
            m_step = 0x38;
        } else {
            m_step = 0x3D;
        }
        break;
    case 0x38:
        fn_80030F64(m_recorder, Heaps::MenuResource);
        m_step = 0x39;
        break;
    case 0x39:
        if (m_recorder->m_busy == 0) {
            m_step = 0x3A;
        }
        break;
    case 0x3A:
        if (m_recorder->m_result == 0) {
            fn_8004EB20(g_GameGlobal, m_recorder->m_handle.getBuffer());
            gfHeapManager::free(m_recorder->m_handle.getBuffer());
            m_step = 0x3D;
        } else {
            OSReport("SystemDataLoadError...\n");
            m_step = 0x3C;
        }
        break;
    case 0x3B:
        // the player wants to leave: the game goes back to the menu of the Wii
        g_gfShutdownManager.enterReturnToWiiMenuProcess();
        m_step = 0x3D;
        break;
    case 0x3C:
        // an error that has no window (the text is "an unexpected error happened")
        OSReport("\x91\x7A\x92\xE8\x8A\x4F\x82\xCC\x83\x47\x83\x89\x81\x5B\x94\xAD\x90\xB6\n");
        m_flags &= 0xE0;
        fn_8004EAE0(g_GameGlobal, m_flags);
        m_step = 0x3D;
        break;
    case 0x3D:
    case 0x3E:
        // the check is over
        break;
    }
    m_frame++;
}

void muBootNandTask::initialize() {
    m_checkResult = 0;
    unk2C_b1 = false;
    m_flags = 0;
    m_flags |= 0x1F;
    m_requireIndex = 0;
    m_requireKind = 0;
    m_skipDone = 0;
    m_answer = -1;
    m_step = 0;
    m_frame = 0;
    m_recorder = fn_80030548(Heaps::MenuInstance);
}

void muBootNandTask::release() {
    fn_800307B4();
    m_recorder = NULL;
    closeWnd();
    m_cameraTask->exit();
    m_cameraTask = NULL;
    m_msgData = NULL;
    Release__Q34nw4r3g3d7ResFileFv(&m_resFileWnd);
    Release__Q34nw4r3g3d7ResFileFv(&m_resFileWait);
    delete m_object;
    m_object = NULL;
}

// Creates the camera, the "please wait" panel and the window models and loads the texts.
void muBootNandTask::createData(gfArchive* archive) {
    m_cameraTask = fn_800AFBFC("MenuTestTask", 0x10, archive, 0xA);
    m_msgData = archive->getData(Data_Type_Misc, 0, 0xFFFE);
    m_resFileWnd = nw4r::g3d::ResFile(archive->getData(Data_Type_Model, 0, 0xFFFE));
    m_resFileWait = nw4r::g3d::ResFile(archive->getData(Data_Type_Model, 1, 0xFFFE));
    nw4r::g3d::ResFile::Init(&m_resFileWnd);
    nw4r::g3d::ResFile::Init(&m_resFileWait);
    MuObject* object = MuObject::create(&m_resFileWait, "InfCmnWait_TopN", 1, NULL, Heaps::MenuInstance);
    m_object = object;
    object->m_scnMdl->SetPriorityDrawOpa(0xFF);
    object->m_scnMdl->SetPriorityDrawXlu(0xFF);
    Vec3f pos(0.0f, -7.0f, 0.0f);
    m_object->setTrans(&pos);
}
