#include <types.h>

#include <gf/gf_heap_manager.h>
#include <gf/gf_homemenu.h>
#include <gf/gf_memory_pool.h>
#include <gf/gf_pad_status.h>
#include <gf/gf_pad_system.h>
#include <memory.h>
#include <ms/ms_message.h>
#include <nt/nt_etc_global.h>
#include <nt/nt_match.h>
#include <nt/nt_offline.h>
#include <revolution/OS/OSError.h>
#include <snd/snd_system.h>
#include <stdio.h>
#include <string.h>

#include <mu/mu_wifi_cnct_wnd_task.h>
#include <mu/wifi/mu_wifi_interface.h>

muWifiCnctWndTask* muWifiCnctWndTask::create(int mode, nw4r::g3d::ResFile* resFile, void* msgData, nw4r::g3d::ScnGroup* group,
                                             int priority, nw4r::g3d::ResFile* keyRes, nw4r::g3d::ResFile* wndRes) {
    muWifiCnctWndTask* task = new (Heaps::MenuInstance) muWifiCnctWndTask();
    task->initProc(mode, resFile, msgData, group, priority, keyRes, wndRes);
    return task;
}

muWifiCnctWndTask::muWifiCnctWndTask() :
    gfTask("WifiCnctWnd", Category_Menu, 0xF, 8, true),
    m_mode(0),
    m_scnGroup(NULL),
    m_msg(NULL),
    m_unk120(0),
    m_yes(true),
    m_result(Result_Done),
    m_errorIndex(0),
    m_errorCode(0),
    m_isNewUser(false),
    m_step(Step_Appear),
    m_prevStep(Step_Num),
    m_timer(0),
    m_unk148(0),
    m_wndRes(NULL),
    m_dataLoadTask(NULL),
    m_priority(0),
    m_recorder(NULL),
    m_hasDelivery(false),
    m_canEnd(false) {
    unk2C_b1 = false;
    for (int i = 0; i < Obj_Num; i++) {
        m_objects[i] = NULL;
    }
    m_scnMdl = NULL;
}

void muWifiCnctWndTask::exit() {
    if (m_recorder != NULL) {
        gfSysRecorder2::dropInstance();
    }
    gfTask::exit();
}

muWifiCnctWndTask::~muWifiCnctWndTask() {
    m_nameEntry.close();
    if (m_dataLoadTask != NULL) {
        m_dataLoadTask->exit();
    }
    if (m_msg != NULL) {
        delete m_msg;
    }
    if (m_scnMdl != NULL) {
        m_scnMdl->Destroy();
    }
    for (int i = 0; i < Obj_Num; i++) {
        if (m_objects[i] != NULL) {
            delete m_objects[i];
        }
    }
}

const muWifiCnctWndTask::StepFunc muWifiCnctWndTask::sStepFuncs[Step_Num] = {
    { mainStepCreateWifiInstInit, mainStepCreateWifiInstMain },
    { mainStepWifiDataLoadInit, mainStepWifiDataLoadMain },
    { mainStepAppearInit, mainStepAppearMain },
    { mainStepConfirmInit, mainStepConfirmMain },
    { mainStepConnectInit, mainStepConnectMain },
    { mainStepDisconnectInit, mainStepDisconnectMain },
    { mainStepDoneInit, mainStepDoneMain },
    { mainStepAppearNameEntryInit, mainStepAppearNameEntryMain },
    { mainStepNameEntryInit, mainStepNameEntryMain },
    { mainStepLeaveNameEntryInit, mainStepLeaveNameEntryMain },
    { mainStepLeaveInit, mainStepLeaveMain },
    { mainStepPostInit, mainStepPostMain },
    { NULL, NULL },
};

void muWifiCnctWndTask::initProc(int mode, nw4r::g3d::ResFile* resFile, void* msgData, nw4r::g3d::ScnGroup* group,
                                 int priority, nw4r::g3d::ResFile* keyRes, nw4r::g3d::ResFile* wndRes) {
    m_mode = mode;
    m_scnGroup = group;
    m_priority = priority;
    fn_800B96E8(&m_controller, 0xF0);
    createObjResFile(sWndModels, 2, resFile, true, priority);
    m_scnMdl = nw4r::g3d::ScnMdlExpand::Construct(gfHeapManager::getMEMAllocator(Heaps::MenuInstance), NULL, Obj_Num,
                                                  m_objects[Obj_Wnd]->m_sceneModel);
    m_scnMdl->PushBack(m_objects[Obj_Icon]->m_sceneModel, "pos00");
    if (m_mode == 0) {
        createObjResFile(sKeyModels, 8, keyRes, false, priority);
        m_wndRes = wndRes;
    }
    initMsg(msgData);
    if (m_mode == 0) {
        // The name entry attaches the keys to the window (its parameter is typed ScnMdl, as the symbol says).
        m_nameEntry.init(NULL, reinterpret_cast<nw4r::g3d::ScnMdl*>(m_scnMdl), m_objects[Obj_KeyBase], &m_objects[Obj_KeySet0], m_objects[Obj_KeyCursor],
                         m_objects[Obj_KeyLine], 6, m_msg, 0, false);
    }
    if (m_mode == 0) {
        m_step = Step_CreateWifiInst;
    } else {
        m_step = Step_Appear;
    }
}

const muWifiCnctWndTask::Model muWifiCnctWndTask::sWndModels[2] = {
    { "MenCmnConnect0000_TopN", Obj_Wnd, Obj_Wnd, 0 },
    { "MenCmnConnect0001_TopN", Obj_Icon, Obj_Icon, 1 },
};

const muWifiCnctWndTask::Model muWifiCnctWndTask::sKeyModels[8] = {
    { "MenSelchrWbase", Obj_KeyBase, Obj_KeyBase, 2 },
    { "MenSelchrWhiragana", Obj_KeySet0, Obj_KeySet0, 3 },
    { "MenSelchrWkatakana", Obj_KeySet0 + 1, Obj_KeySet0 + 1, 3 },
    { "MenSelchrWalphabet", Obj_KeySet0 + 2, Obj_KeySet0 + 2, 3 },
    { "MenSelchrWeurope", Obj_KeySet0 + 4, Obj_KeySet0 + 4, 3 },
    { "MenSelchrWnumber", Obj_KeySet0 + 3, Obj_KeySet0 + 3, 3 },
    { "MenSelchrWcursor", Obj_KeyCursor, Obj_KeyCursor, 4 },
    { "MenSelchrWline", Obj_KeyLine, Obj_KeyLine, 5 },
};

void muWifiCnctWndTask::createObjResFile(const Model* models, int count, nw4r::g3d::ResFile* resFile, bool suffix, int priority) {
    int n;
    int i;
    int j;
    const Model* model = models;
    MuObject* object;
    for (i = 0; i < count; i++, model++) {
        n = 1;
        if (model->m_start < model->m_end) {
            n = model->m_end - model->m_start;
        }
        for (j = 0; j < n; j++) {
            object = MuObject::create(resFile, model->m_name, model->m_priority + priority, NULL, Heaps::MenuInstance);
            m_objects[model->m_start + j] = object;
            char animName[0x40];
            strcpy(animName, model->m_name);
            if (suffix) {
                strcat(animName, "__0");
            }
            object->changeAnimN(animName);
            if (object->m_modelAnim != NULL) {
                object->m_modelAnim->setUpdateRate(0.0f);
            }
        }
    }
}

// The texts: 0 and 1 the text of the window, 2 and 3 Yes and No, 4 the codes and the received data, 5 the question
// of the name entry, 6 the name that is typed.
void muWifiCnctWndTask::initMsg(void* msgData) {
    MuMsg* msg = MuMsg::create(1, Heaps::MenuInstance, Heaps::MenuResource);
    m_msg = msg;
    msg->allocMsgBuf(0x168, 7);
    msg->setMsgData(msgData);
    MuObject* wnd = m_objects[Obj_Wnd];
    msg->attachScnMdlSimple(0, reinterpret_cast<nw4r::g3d::ScnMdlSimple*>(wnd->m_sceneModel), 0, 1.0f / 16.0f);
    msg->getWScale(0, 48.0f);
    msg->setFontColor(0, 0x35, 0x47, 0x26, 0xFF);
    msg->attachScnMdlSimple(1, reinterpret_cast<nw4r::g3d::ScnMdlSimple*>(wnd->m_sceneModel), 1, 1.0f / 16.0f);
    msg->getWScale(1, 48.0f);
    msg->setFontColor(1, 0x35, 0x47, 0x26, 0xFF);
    msg->attachScnMdlSimple(2, reinterpret_cast<nw4r::g3d::ScnMdlSimple*>(wnd->m_sceneModel), 2, 1.0f / 16.0f);
    msg->attachScnMdlSimple(3, reinterpret_cast<nw4r::g3d::ScnMdlSimple*>(wnd->m_sceneModel), 3, 1.0f / 16.0f);
    msg->attachScnMdlSimple(4, reinterpret_cast<nw4r::g3d::ScnMdlSimple*>(wnd->m_sceneModel), 4, 1.0f / 16.0f);
    msg->setFontColor(4, 0x35, 0x47, 0x26, 0xFF);
    msg->getWScale(4, 48.0f);
    msg->attachScnMdlSimple(5, reinterpret_cast<nw4r::g3d::ScnMdlSimple*>(wnd->m_sceneModel), 5, 1.0f / 16.0f);
    msg->setFontColor(5, 0, 0, 0, 0xFF);
    if (m_mode == 0) {
        m_msg->attachScnMdlSimple(6, reinterpret_cast<nw4r::g3d::ScnMdlSimple*>(m_objects[Obj_KeyBase]->m_sceneModel), 0, 1.0f / 16.0f);
        m_msg->setFontType(6, 1);
        m_msg->setAlignMode(6, MuMsg::Align_Left);
        m_msg->setFontWidthModeFixed(6, 28.0f);
        m_msg->setFontColor(6, 0x80, 0x80, 0x80, 0xFF);
    }
}

void muWifiCnctWndTask::processDefault() {
    fn_800B9984(&m_controller);
    if (m_prevStep != m_step) {
        if (sStepFuncs[m_step].m_init != NULL) {
            sStepFuncs[m_step].m_init(this);
        }
        m_prevStep = m_step;
    }
    if (sStepFuncs[m_step].m_main != NULL) {
        m_step = sStepFuncs[m_step].m_main(this);
    }
}

// HYPOTHESIS: the map names a dispWndMsg here, between processDefault and saveWifiData, but nothing calls it: the
// linker dropped its code and kept its texts, which sit in .data in this order ("" and then the in and out node
// animations of the window). The body is rebuilt from those texts and from the steps that do the same work.
void muWifiCnctWndTask::dispWndMsg(bool disp, int msgNo) {
    m_msg->printf(2, "");
    m_msg->printf(3, "");
    m_msg->printIndex(1, msgNo);
    MuObject* wnd = m_objects[Obj_Wnd];
    wnd->changeNodeAnimN(disp ? "MenCmnConnect0000_TopN__0" : "MenCmnConnect0000_TopN__1");
    wnd->setFrameNode(0.0f);
}

// Saves the Wi-Fi data of the friends (after a disconnection); the recorder is kept until it has finished.
void muWifiCnctWndTask::saveWifiData() {
    int size = 0x2804;
    void* data;
    bool started = false;
    if (gfSysRecorder2::isEnable(2)) {
        m_recorder = gfSysRecorder2::createInstance(Heaps::MenuResource);
        if (m_recorder != NULL) {
            void* buffer = m_recorder->getBuffer(2);
            if (ntFriendInfo::getInstance()->getSaveData(&data, &size)) {
                memcpy(buffer, data, size);
                if (m_recorder->saveWifi()) {
                    started = true;
                }
            }
        }
        if (!started && m_recorder != NULL) {
            gfSysRecorder2::dropInstance();
            m_recorder = NULL;
        }
    }
}

void muWifiCnctWndTask::mainStepCreateWifiInstInit(muWifiCnctWndTask* task) { }

int muWifiCnctWndTask::mainStepCreateWifiInstMain(muWifiCnctWndTask* task) {
    if (g_muWifiInterfaceTask != NULL) {
        return Step_WifiDataLoad;
    }
    if (!g_gfHomeMenu->setBan(1, 2)) {
        return task->m_step;
    }
    if (!g_gfHomeMenu->releaseResource()) {
        return task->m_step;
    }
    muWifiInterfaceTask::create();
    return Step_WifiDataLoad;
}

void muWifiCnctWndTask::mainStepWifiDataLoadInit(muWifiCnctWndTask* task) {
    task->m_dataLoadTask = muWifiDataLoadTask::create(task->m_wndRes, task->m_scnGroup, task->m_msg->getBinary(), task->m_priority);
}

int muWifiCnctWndTask::mainStepWifiDataLoadMain(muWifiCnctWndTask* task) {
    muWifiDataLoadTask* dataLoadTask = task->m_dataLoadTask;
    if (dataLoadTask->m_step != 5) {
        return task->m_step;
    }
    int result = dataLoadTask->m_result;
    dataLoadTask->exit();
    task->m_dataLoadTask = NULL;
    int next;
    if (result == 1) {
        task->m_result = Result_Cancelled;
        next = Step_Post;
        if (g_muWifiInterfaceTask != NULL) {
            muWifiInterfaceTask::destroy();
        }
    } else {
        next = Step_Appear;
    }
    return next;
}

void muWifiCnctWndTask::mainStepAppearInit(muWifiCnctWndTask* task) {
    int line = task->m_mode != 0;
    int anim = 4;
    if (task->m_mode == 0) {
        anim = 0;
    }
    MuObject* icon = task->m_objects[Obj_Icon];
    icon->changeAnimN(anim, 0x18, false, false);
    icon->setFrame(0.0f);
    icon->m_modelAnim->setUpdateRate(1.0f);
    MuObject* wnd = task->m_objects[Obj_Wnd];
    wnd->setFrameVisible(1.0f);
    task->m_msg->printIndex(2, 0x11);
    task->m_msg->printIndex(3, 0x12);
    task->m_msg->printIndex(1, line);
    task->m_objects[Obj_Wnd]->setFrameMatCol(1.0f);
    task->m_yes = false;
    wnd->changeNodeAnimN("MenCmnConnect0000_TopN__0");
    wnd->setFrameNode(0.0f);
    wnd->m_modelAnim->m_anmObjChrRes->SetUpdateRate(1.0f);
    task->m_scnGroup->Insert(task->m_scnGroup->Size(), task->m_scnMdl);
    g_sndSystem->playSE(static_cast<SndID>(5), -1, 0, 0, -1);
}

int muWifiCnctWndTask::mainStepAppearMain(muWifiCnctWndTask* task) {
    if (task->m_objects[Obj_Wnd]->isNodeAnimFinished()) {
        return Step_Confirm;
    }
    return task->m_step;
}

void muWifiCnctWndTask::mainStepConfirmInit(muWifiCnctWndTask* task) { }

// Left selects Yes, right No (the frame of the colour animation of the window: 0 Yes, 1 No).
int muWifiCnctWndTask::mainStepConfirmMain(muWifiCnctWndTask* task) {
    gfPadStatus pad;
    g_gfPadSystem->getSysPadStatus(0xF0, &pad);
    u32 pressed = task->m_controller.m_pressed;
    if ((pressed & 1) && !task->m_yes) {
        task->m_objects[Obj_Wnd]->setFrameMatCol(0.0f);
        task->m_yes = true;
        g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
    }
    if ((pressed & 2) && task->m_yes) {
        task->m_objects[Obj_Wnd]->setFrameMatCol(1.0f);
        task->m_yes = false;
        g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
    }
    u32 buttons = pad.m_buttonsPressedThisFrame.bits;
    if ((buttons & (GF_PAD_BUTTON_BIT_A | GF_PAD_BUTTON_BIT_START)) || (buttons & GF_PAD_BUTTON_BIT_B)) {
        bool yes = (buttons & GF_PAD_BUTTON_BIT_B) ? false : task->m_yes;
        if (yes) {
            if (task->m_mode == 0) {
                return Step_Connect;
            }
            return Step_Disconnect;
        }
        task->m_result = Result_Cancelled;
        if (task->m_mode == 0 && g_muWifiInterfaceTask != NULL) {
            muWifiInterfaceTask::destroy();
        }
        return Step_Leave;
    }
    return task->m_step;
}

void muWifiCnctWndTask::mainStepConnectInit(muWifiCnctWndTask* task) {
    MuObject* icon = task->m_objects[Obj_Icon];
    icon->changeAnimN(1, 0x18, false, false);
    icon->setFrame(0.0f);
    icon->m_modelAnim->setUpdateRate(1.0f);
    task->m_objects[Obj_Wnd]->setFrameVisible(0.0f);
    task->m_msg->printf(2, "");
    task->m_msg->printf(3, "");
    task->m_msg->printIndex(1, 2);
    task->m_seHandle = g_sndSystem->playSE(static_cast<SndID>(0x204E), -1, 0, 0, -1);
    g_muWifiInterfaceTask->connect(true);
}

int muWifiCnctWndTask::mainStepConnectMain(muWifiCnctWndTask* task) {
    int result = g_muWifiInterfaceTask->getConnectResult(&task->m_errorIndex, &task->m_errorCode, &task->m_isNewUser);
    if (result != 1) {
        g_sndSystem->stopSE(task->m_seHandle, 0);
        task->m_result = Result_Error;
        if (result == 2) {
            g_sndSystem->playSE(static_cast<SndID>(0x204F), -1, 0, 0, -1);
            task->m_result = Result_Done;
        }
        if (result == 4) {
            task->m_result = Result_BadName;
        }
        if (result == 3) {
            task->m_result = Result_Error;
        }
        return Step_Done;
    }
    return task->m_step;
}

static void netReport(const char* format, ...);

void muWifiCnctWndTask::mainStepDisconnectInit(muWifiCnctWndTask* task) {
    MuObject* icon = task->m_objects[Obj_Icon];
    icon->changeAnimN(5, 0x18, false, false);
    icon->setFrame(0.0f);
    icon->m_modelAnim->setUpdateRate(1.0f);
    task->m_objects[Obj_Wnd]->setFrameVisible(0.0f);
    task->m_msg->printf(2, "");
    task->m_msg->printf(3, "");
    task->m_msg->printIndex(1, 3);
    if (task->m_mode == 1) {
        task->saveWifiData();
    }
    netReport("mainStepDisconnectInit\n");
    g_muWifiInterfaceTask->disconnect();
}

// An empty report of the network code (the printing is compiled out of the game).
static void netReport(const char* format, ...) { }

int muWifiCnctWndTask::mainStepDisconnectMain(muWifiCnctWndTask* task) {
    int result = g_muWifiInterfaceTask->getDisconnectResult(&task->m_errorIndex);
    if (result != 1) {
        task->m_result = Result_Error;
        if (result == 2) {
            g_sndSystem->playSE(static_cast<SndID>(0x2050), -1, 0, 0, -1);
            task->m_result = Result_Done;
        }
        return Step_Done;
    }
    return task->m_step;
}

void muWifiCnctWndTask::mainStepDoneInit(muWifiCnctWndTask* task) {
    // The names of the kinds of data that the connection received (nteGlobal::isGotDeliveryData 2, 1 and 4).
    static const u32 sDeliveryMsgs[3] = {0x29, 0x2A, 0x2B};
    int msgNo;
    bool haveErrorCode = false;
    switch (task->m_result) {
    case Result_Done:
        task->m_timer = 120;
        msgNo = task->m_mode == 0 ? 4 : 5;
        break;
    case Result_Error:
        msgNo = muWifiInterfaceTask::getErrorMsgID(task->m_errorIndex);
        haveErrorCode = muWifiInterfaceTask::haveErrorCode(task->m_errorIndex);
        break;
    case Result_BadName:
        msgNo = muWifiInterfaceTask::getBadNameErrorMsgID();
        break;
    }
    task->m_objects[Obj_Wnd]->setFrameVisible(0.0f);
    task->m_msg->printf(2, "");
    task->m_msg->printf(3, "");
    task->m_msg->printIndex(1, msgNo);
    bool got[3];
    int len;
    u8 text[0x100];
    char buf[0x100];
    if (task->m_result == Result_Error && haveErrorCode) {
        const char* data = task->m_msg->getMsgData(0x13, &len);
        memcpy(buf, data, len);
        len += sprintf(buf + len, "%05d", task->m_errorCode);
        task->m_msg->printData(4, buf, len);
    }
    if (task->m_mode == 0 && task->m_result == Result_Done) {
        if (task->m_isNewUser) {
            // The friend code of the new Wi-Fi user, in three groups of four digits.
            u64 code = g_muWifiInterfaceTask->getUserID();
            int digits3 = code % 10000;
            code /= 10000;
            const char* data = task->m_msg->getMsgData(0x14, &len);
            memcpy(buf, data, len);
            int pos = len;
            pos += sprintf(buf + pos, " %04d-%04d-%04d ", (u16)(code / 10000 % 10000), (u16)(code % 10000), (u16)digits3);
            data = task->m_msg->getMsgData(0x15, &len);
            memcpy(buf + pos, data, len);
            task->m_msg->printData(4, buf, pos + len);
        } else {
            got[0] = nteGlobal::isGotDeliveryData(2);
            got[1] = nteGlobal::isGotDeliveryData(1);
            got[2] = nteGlobal::isGotDeliveryData(4);
            if (got[0] || got[1] || got[2]) {
                task->m_hasDelivery = true;
                buf[0] = '\n';
                const char* data = task->m_msg->getMsgData(0x17, &len);
                memcpy(buf + 1, data, len);
                int pos = len + 1;
                buf[pos++] = '\n';
                for (int i = 0; i < 3; i++) {
                    if (got[i]) {
                        data = task->m_msg->getMsgData(0x18, &len);
                        memcpy(buf + pos, data, len);
                        pos += len;
                        data = task->m_msg->getMsgData(sDeliveryMsgs[i], &len);
                        memcpy(buf + pos, data, len);
                        pos += len;
                        data = task->m_msg->getMsgData(0x19, &len);
                        memcpy(buf + pos, data, len);
                        pos += len;
                    }
                }
                int textLen = Message::stripTags(reinterpret_cast<const u8*>(buf), pos, 0xFFFF, text);
                task->m_msg->printData(4, text, textLen);
            }
        }
    }
    int anim;
    if (task->m_result == Result_Error) {
        anim = 3;
    } else {
        anim = 6;
        if (task->m_mode == 0) {
            anim = 2;
        }
    }
    MuObject* icon = task->m_objects[Obj_Icon];
    icon->changeAnimN(anim, 0x18, false, false);
    icon->setFrame(0.0f);
    icon->m_modelAnim->setUpdateRate(1.0f);
}

// A or START closes the window (after a connection it also closes by itself, unless it lists received data); after
// the first connection the name entry comes next.
int muWifiCnctWndTask::mainStepDoneMain(muWifiCnctWndTask* task) {
    bool timeUp = false;
    bool destroyInterface = false;
    if (task->m_result == Result_Done && !task->m_isNewUser && !task->m_hasDelivery && --task->m_timer <= 0) {
        timeUp = true;
    }
    gfPadStatus pad;
    g_gfPadSystem->getSysPadStatus(0xF0, &pad);
    bool canClose;
    if (task->m_mode == 1) {
        if (task->m_recorder == NULL) {
            canClose = true;
        } else if (task->m_recorder->m_busy == 0) {
            gfSysRecorder2::dropInstance();
            task->m_recorder = NULL;
            canClose = true;
        } else {
            canClose = false;
        }
        if (canClose) {
            destroyInterface = true;
        }
    } else {
        canClose = true;
        if (task->m_result == Result_Error || task->m_result == Result_BadName) {
            destroyInterface = true;
        }
    }
    if (destroyInterface && g_muWifiInterfaceTask != NULL) {
        muWifiInterfaceTask::destroy();
    }
    if (((pad.m_buttonsPressedThisFrame.bits & (GF_PAD_BUTTON_BIT_A | GF_PAD_BUTTON_BIT_START)) || timeUp) && canClose) {
        if (task->m_result == Result_Done && task->m_isNewUser) {
            task->m_nameEntry.m_padNo = muMenuController::getAButtonPushedControllerID();
            return Step_AppearNameEntry;
        }
        return Step_Leave;
    }
    return task->m_step;
}

void muWifiCnctWndTask::mainStepAppearNameEntryInit(muWifiCnctWndTask* task) {
    task->m_nameEntry.open(NULL, NULL, 5);
    task->m_objects[Obj_Wnd]->changeAnimN(2, 1, true, false);
    task->m_msg->printIndex(5, 0x16);
}

// HYPOTHESIS: the test reads backwards (the step moves on when isNodeAnimFinished is 0), as in mainStepLeaveNameEntryMain.
int muWifiCnctWndTask::mainStepAppearNameEntryMain(muWifiCnctWndTask* task) {
    if (task->m_objects[Obj_Wnd]->isNodeAnimFinished()) {
        return task->m_step;
    }
    return Step_NameEntry;
}

void muWifiCnctWndTask::mainStepNameEntryInit(muWifiCnctWndTask* task) { }

int muWifiCnctWndTask::mainStepNameEntryMain(muWifiCnctWndTask* task) {
    int next = task->m_step;
    gfPadStatus pad;
    g_gfPadSystem->getSysPadStatus(0xF0, &pad);
    pad.m_buttonsCurrentFrame2.bits = task->m_controller.m_held;
    pad.m_buttonsPressedThisFrame.bits = task->m_controller.m_pressed;
    pad.m_buttonsPressedThisFrame2.bits = task->m_controller.m_trigger;
    char name[0x1C];
    wchar_t wideName[6];
    switch (task->m_nameEntry.main(&pad, name, true, false)) {
    case MuSelctChrNameEntry::Result_Decided:
        Message::utf8to16(wideName, name);
        g_muWifiInterfaceTask->putMyFriendStatusName(wideName);
        next = Step_LeaveNameEntry;
        break;
    case MuSelctChrNameEntry::Result_Cancelled:
    case MuSelctChrNameEntry::Result_Editing:
    case MuSelctChrNameEntry::Result_Closed:
        break;
    }
    return next;
}

void muWifiCnctWndTask::mainStepLeaveNameEntryInit(muWifiCnctWndTask* task) {
    task->m_objects[Obj_Wnd]->changeAnimN(3, 1, true, false);
    task->m_msg->printf(5, "");
}

int muWifiCnctWndTask::mainStepLeaveNameEntryMain(muWifiCnctWndTask* task) {
    if (task->m_objects[Obj_Wnd]->isNodeAnimFinished()) {
        return task->m_step;
    }
    task->m_nameEntry.close();
    g_muWifiInterfaceTask->unkFCA_b4 = false;
    NtMatching::thSaveUserDataNow();
    return Step_Leave;
}

void muWifiCnctWndTask::mainStepLeaveInit(muWifiCnctWndTask* task) {
    MuObject* wnd = task->m_objects[Obj_Wnd];
    wnd->changeNodeAnimN("MenCmnConnect0000_TopN__1");
    wnd->setFrameNode(0.0f);
    wnd->m_modelAnim->m_anmObjChrRes->SetUpdateRate(1.0f);
    g_sndSystem->playSE(static_cast<SndID>(6), -1, 0, 0, -1);
}

int muWifiCnctWndTask::mainStepLeaveMain(muWifiCnctWndTask* task) {
    if (task->m_objects[Obj_Wnd]->isNodeAnimFinished()) {
        task->m_scnGroup->Remove(task->m_scnMdl);
        return Step_Post;
    }
    return task->m_step;
}

void muWifiCnctWndTask::mainStepPostInit(muWifiCnctWndTask* task) {
    task->m_canEnd = false;
    if (g_muWifiInterfaceTask != NULL) {
        task->m_canEnd = true;
    }
}

// Without the Wi-Fi interface (it was destroyed), waits for the save of the friend data, frees the network heap and
// gives the HOME Menu its memory back before the window ends.
int muWifiCnctWndTask::mainStepPostMain(muWifiCnctWndTask* task) {
    int next = task->m_step;
    if (task->m_canEnd) {
        if (g_muWifiInterfaceTask == NULL) {
            g_gfHomeMenu->setBan(0, 2);
            g_gfHomeMenu->loadResource();
        }
        next = Step_End;
    } else if (ntFriendInfoSave::isNotSaving()) {
        task->m_canEnd = true;
        static_cast<gfMemoryPool*>(gfHeapManager::getHeap(Heaps::Network))->clear();
    } else {
        OSReport("waiting... ntFriendInfoSave::isNotSaving() \n");
    }
    return next;
}
