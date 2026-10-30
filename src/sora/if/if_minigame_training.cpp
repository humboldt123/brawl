#include <StaticAssert.h>
#include <cm/cm_camera_controller.h>
#include <cstring>
#include <if/if_minigame_training.h>
#include <mu/mu_msg.h>
#include <mu/mu_object.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <nw4r/g3d/g3d_scnmdlsmpl.h>
#include <snd/snd_system.h>
#include <sr/sr_common.h>
#include <types.h>
#include <ut/ut_relocate.h>

// HYPOTHESIS: MuMsg::create takes its second heap as u32 (the symbol differs from the shared header).
// HYPOTHESIS: MuMsg keeps its Message text writer at 0x8 (private in the shared header).
class Message;
extern "C" void setBufField34__7MessageFii(Message* message, int index, int value);

// HYPOTHESIS: muMenuController::setControllerID (0x800B9804) and main (0x800B9984), still fn_.
extern "C" void fn_800B9804(muMenuController* controller, u8 controllerId);
// HYPOTHESIS: muMenu item helpers (0x800AFA04, 0x800AFA1C, still fn_; the map labels around them are shifted):
// the message of the name of a menu item kind, and the item kind to generate.
extern "C" u32 fn_800AFA04(int kind);
extern "C" int fn_800AFA1C(int kind);
// HYPOTHESIS: cmDemoController::setTargetType (0x800AAAD8, still fn_).
class cmDemoController;
extern "C" void fn_800AAAD8(cmDemoController* controller, int type);

// HYPOTHESIS: the screen range of the demo camera; offsets from setCameraType.
struct IfMinigameTrainingCameraRange {
    float m_left;
    float m_right;
    float m_top;
    float m_bottom;
};
class cmDemoController {
public:
    u8 unk0[0x20];
    IfMinigameTrainingCameraRange m_range;
};

// HYPOTHESIS: the button words of muMenuController (repeat at 0x10, trigger at 0xC).
struct IfMinigameTrainingButtons {
    u8 unk0[0xC];
    u32 m_trigger;
    u32 m_repeat;
};

struct IfMinigameTrainingNodeInfo {
    const char* m_name;
    u8 m_start;
    u8 m_end;
    u8 m_node;
};
static_assert(sizeof(IfMinigameTrainingNodeInfo) == 0x8, "Class is the wrong size!");

struct IfMinigameTrainingMsgInfo {
    u8 m_model;
    u8 m_node;
    u8 unk2;
};

struct IfMinigameTrainingStepFunc {
    void (*m_init)(IfMinigameTraining* training);
    s32 (*m_main)(IfMinigameTraining* training);
};

static const IfMinigameTrainingCameraRange s_defaultRange = { -1.0f, 1.0f, 1.0f, -1.0f };

static const IfMinigameTrainingNodeInfo s_nodeInfo[2] = {
    { "InfTraining0000_TopN", 0, 0, 2 },
    { "InfTraining0001_TopN", 1, 1, 1 },
};

static const u32 s_speedMsgTbl[6] = { 1, 2, 3, 4, 5, 0 };
static const u32 s_cpAIMsgTbl[6] = { 6, 7, 8, 9, 10, 11 };
static const u32 s_cursorMsgTbl[8] = { 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18 };

static const IfMinigameTrainingStepFunc s_stepFuncTbl[4] = {
    { NULL, NULL },
    { IfMinigameTraining::initStepAppear, IfMinigameTraining::mainStepAppear },
    { IfMinigameTraining::initStepMenu, IfMinigameTraining::mainStepMenu },
    { IfMinigameTraining::initStepLeave, IfMinigameTraining::mainStepLeave },
};

static const u32 s_cameraMsgTbl[2] = { 12, 13 };

static void (*s_curPosProcTbl[8])(IfMinigameTraining* training) = {
    IfMinigameTraining::curPosProcSpeed,
    IfMinigameTraining::curPosProcItem,
    IfMinigameTraining::curPosProcNumCP,
    IfMinigameTraining::curPosProcCPAI,
    IfMinigameTraining::curPosProcCPDamage,
    IfMinigameTraining::curPosProcCamera,
    IfMinigameTraining::curPosProcStatusVisible,
    IfMinigameTraining::curPosProcQuit,
};

IfMinigameTraining::IfMinigameTraining() {
    init();
}

void IfMinigameTraining::init() {
    m_group = NULL;
    for (int i = 0; i < 2; i++) {
        m_models[i] = NULL;
    }
    m_msg = NULL;
    m_itemMsgData = NULL;
    m_cursor = 0;
    m_speed = 1;
    m_item = 0;
    m_numCP = 1;
    m_cpAI = 3;
    m_cpDamage = 0;
    m_camera = 0;
    m_statusVisible = false;
    m_step = 0;
    fn_800B96E8(&m_controller, 0);
    m_damage = 0;
    m_combo = 0;
    m_totalDamage = 0;
    memset(m_itemTbl, 0, sizeof(m_itemTbl));
    m_numItems = 0;
    m_generateItemKind = -1;
    unk2F8 = 0;
    unk2FC = false;
}

IfMinigameTraining::~IfMinigameTraining() {
    for (u32 i = 0; i < 2; i++) {
        if (m_models[i] != NULL) {
            delete m_models[i];
            m_models[i] = NULL;
        }
    }
    if (m_msg != NULL) {
        delete m_msg;
        m_msg = NULL;
    }
}

void IfMinigameTraining::createModel(nw4r::g3d::ResFile* resFile, void* msgData, void* itemMsgData, u8* itemTblData,
                                     u32 itemTblSize) {
    createObjResFile(s_nodeInfo, 2, resFile, 0xC0);
    initMsg(msgData);
    int i = 0;
    {
        utRelocate reloc(itemTblData, itemTblSize);
        const s32* tbl = static_cast<const s32*>(reloc.getPublicAddress("ifTrainingItemTbl"));
        for (; tbl[i] >= 0; i++) {
            m_itemTbl[i] = tbl[i];
        }
    }
    m_numItems = i;
    m_itemMsgData = itemMsgData;
}

void IfMinigameTraining::createObjResFile(const void* nodeInfo, s32 nodeCount, nw4r::g3d::ResFile* resFile, s32 prio) {
    s32 i;
    s32 j;
    const IfMinigameTrainingNodeInfo* p = static_cast<const IfMinigameTrainingNodeInfo*>(nodeInfo);
    char nameBuf[0x40];
    for (i = 0; i < nodeCount; i++, p++) {
        s32 nCopies = (p->m_start < p->m_end) ? p->m_end - p->m_start : 1;
        for (j = 0; j < nCopies; j++) {
            MuObject* obj = MuObject::create(resFile, p->m_name, p->m_node + prio + j, 0, Heaps::InfoInstance);
            m_models[p->m_start + j] = obj;
            strcpy(nameBuf, p->m_name);
            strcat(nameBuf, "__0");
            obj->changeAnimN(nameBuf);
            if (obj->m_modelAnim) {
                obj->m_modelAnim->setUpdateRate(0);
            }
        }
    }
}

void IfMinigameTraining::initMsg(void* msgData) {
    IfMinigameTrainingMsgInfo msgInfo[13] = {
        { 0, 0, 1 }, { 0, 1, 1 }, { 0, 2, 1 }, { 0, 3, 1 }, { 0, 4, 1 }, { 0, 5, 1 }, { 0, 6, 1 },
        { 0, 7, 0 }, { 1, 0, 1 }, { 1, 1, 1 }, { 1, 2, 1 }, { 1, 3, 1 }, { 1, 4, 1 },
    };
    m_msg = MuMsg::create(8, Heaps::InfoInstance, 0x2A);
    m_msg->allocMsgBuf(0x100, 13);
    m_msg->setMsgData(msgData);
    IfMinigameTrainingMsgInfo* info = msgInfo;
    for (int i = 0; i < 13; i++, info++) {
        float size = info->unk2 ? 1.0f / 24.0f : 0.0625f;
        m_msg->attachScnMdlSimple(i, reinterpret_cast<nw4r::g3d::ScnMdlSimple*>(m_models[info->m_model]->m_sceneModel),
                                  info->m_node, size);
        setBufField34__7MessageFii(reinterpret_cast<Message**>(m_msg)[2], i, 7);
        if (info->unk2) {
            m_msg->setFontColor(i, 0, 0, 0, 0xFF);
        } else {
            m_msg->getWScale(i, 48.0f);
        }
        m_msg->setFontType(i, 4);
    }
}

void IfMinigameTraining::startMelee(nw4r::g3d::ScnGroup* group) {
    m_group = group;
    group->Insert(group->sceneItemsCount, m_models[1]->m_sceneModel);
    bool visible = m_statusVisible;
    float frame = visible ? 1.0f : 0.0f;
    m_models[1]->setFrameVisible(frame);
    if (visible) {
        m_msg->printIndex(8, s_speedMsgTbl[m_speed]);
        m_msg->printIndex(9, s_cpAIMsgTbl[m_cpAI]);
        printStatusDamage(m_damage);
        printStatusCombo(m_combo);
        printStatusTotalDamage(m_totalDamage);
    } else {
        m_msg->printf(8, "");
        m_msg->printf(9, "");
        m_msg->printf(10, "");
        m_msg->printf(11, "");
        m_msg->printf(12, "");
    }
    m_statusVisible = visible;
    m_msg->printIndex(0, s_speedMsgTbl[m_speed]);
    int kind;
    if (m_item < 0 || m_item >= m_numItems) {
        kind = 0x38;
    } else {
        kind = m_itemTbl[m_item];
    }
    m_msg->printIndex(1, fn_800AFA04(kind), m_itemMsgData);
    m_msg->printf(2, "%d", m_numCP);
    m_msg->printIndex(3, s_cpAIMsgTbl[m_cpAI]);
    m_msg->printf(4, "%d", m_cpDamage);
    m_msg->printIndex(5, s_cameraMsgTbl[m_camera]);
    m_msg->printIndex(6, m_statusVisible ? StatusVisibleMsg_On : StatusVisibleMsg_Off);
    m_models[0]->changeAnimN(2, 0x10, true, false);
    m_models[0]->setFrameVisible(1.0f);
    m_cursor = 0;
    m_msg->printIndex(7, 0x11);
}

void IfMinigameTraining::printStatusDamage(int damage) {
    damage = damage > 0 ? damage : 0;
    m_msg->printf(10, "%d", damage < 999 ? damage : 999);
}

void IfMinigameTraining::printStatusCombo(int combo) {
    combo = combo > 0 ? combo : 0;
    m_msg->printf(11, "%d", combo < 999 ? combo : 999);
}

void IfMinigameTraining::printStatusTotalDamage(int damage) {
    damage = damage > 0 ? damage : 0;
    m_msg->printf(12, "%d", damage < 999 ? damage : 999);
}

void IfMinigameTraining::openMenu(int padNo, int playerNo) {
    fn_800B9804(&m_controller, padNo);
    unk2F8 = playerNo;
    setCameraType(CameraType_Menu);
    unk2FC = false;
    m_step = 1;
    if (s_stepFuncTbl[1].m_init != NULL) {
        s_stepFuncTbl[1].m_init(this);
    }
}

void IfMinigameTraining::setCameraType(int type) {
    CameraController* camera = CameraController::getInstance();
    IfMinigameTrainingCameraRange followRange = { -0.8f, 0.8f, 0.8f, -0.8f };
    IfMinigameTrainingCameraRange menuRange = { -0.5f, 1.0f, 1.0f, -1.0f };
    cmDemoController* demo = reinterpret_cast<cmDemoController*>(camera->m_demoController);
    if (type == CameraType_Menu || type == CameraType_Follow) {
        camera->changeInput(CameraController::Demo);
        fn_800AAAD8(demo, unk2F8 + 5);
    } else {
        camera->changeInput(CameraController::AI);
    }
    switch (type) {
    case CameraType_Menu:
        demo->m_range = menuRange;
        break;
    case CameraType_Follow:
        demo->m_range = followRange;
        break;
    case CameraType_Normal: {
        IfMinigameTrainingCameraRange range = s_defaultRange;
        demo->m_range = range;
        break;
    }
    }
}

void IfMinigameTraining::closeMenu() {
    if (m_step != 0) {
        if (!unk2FC) {
            setCameraType(m_camera == 0 ? CameraType_Normal : CameraType_Follow);
        }
        m_step = 3;
        if (s_stepFuncTbl[3].m_init != NULL) {
            s_stepFuncTbl[3].m_init(this);
        }
    }
}

void IfMinigameTraining::main() {
    m_generateItemKind = -1;
    m_quit = false;
    fn_800B9984(&m_controller);
    s32 step = m_step;
    if (s_stepFuncTbl[step].m_main != NULL) {
        s32 next = s_stepFuncTbl[step].m_main(this);
        if (next != step) {
            m_step = next;
            if (s_stepFuncTbl[next].m_init != NULL) {
                s_stepFuncTbl[next].m_init(this);
            }
        }
    }
}

void IfMinigameTraining::initStepAppear(IfMinigameTraining* training) {
    MuObject* obj = training->m_models[0];
    training->m_group->Remove(obj->m_sceneModel);
    training->m_group->Insert(training->m_group->sceneItemsCount, obj->m_sceneModel);
    obj->changeAnimN(0, 1, true, false);
    g_sndSystem->playSE(static_cast<SndID>(5), -1, 0, 0, -1);
}

s32 IfMinigameTraining::mainStepAppear(IfMinigameTraining* training) {
    if (training->m_models[0]->isNodeAnimFinished()) {
        return training->m_step;
    }
    return 2;
}

void IfMinigameTraining::initStepMenu(IfMinigameTraining* training) { }

s32 IfMinigameTraining::mainStepMenu(IfMinigameTraining* training) {
    u32 repeat = reinterpret_cast<IfMinigameTrainingButtons*>(&training->m_controller)->m_repeat;
    s32 cursor = training->m_cursor;
    s32 next = cursor;
    if (repeat & 8) {
        if (--next < 0) {
            next = 7;
        }
    }
    if (repeat & 4) {
        if (++next >= 8) {
            next = 0;
        }
    }
    if (next != cursor) {
        g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
        training->m_models[0]->setFrameVisible(1.0f + next);
        training->m_cursor = next;
        training->m_msg->printIndex(7, s_cursorMsgTbl[next]);
    }
    s_curPosProcTbl[next](training);
    return training->m_step;
}

void IfMinigameTraining::initStepLeave(IfMinigameTraining* training) {
    training->m_models[0]->changeAnimN(1, 1, true, false);
    g_sndSystem->playSE(static_cast<SndID>(6), -1, 0, 0, -1);
}

s32 IfMinigameTraining::mainStepLeave(IfMinigameTraining* training) {
    MuObject* obj = training->m_models[0];
    if (obj->isNodeAnimFinished()) {
        training->m_group->Remove(obj->m_sceneModel);
        return 0;
    }
    return training->m_step;
}

void IfMinigameTraining::curPosProcSpeed(IfMinigameTraining* training) {
    u32 repeat = reinterpret_cast<IfMinigameTrainingButtons*>(&training->m_controller)->m_repeat;
    s32 speed = training->m_speed;
    s32 next = speed;
    if (repeat & 2) {
        if (++next >= 5) {
            next = 0;
        }
    }
    if (repeat & 1) {
        if (--next < 0) {
            next = 4;
        }
    }
    if (next != speed) {
        g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
        training->m_speed = next;
        training->m_msg->printIndex(0, s_speedMsgTbl[next]);
        if (training->m_statusVisible) {
            training->m_msg->printIndex(8, s_speedMsgTbl[training->m_speed]);
        }
    }
}

void IfMinigameTraining::curPosProcItem(IfMinigameTraining* training) {
    u32 repeat = reinterpret_cast<IfMinigameTrainingButtons*>(&training->m_controller)->m_repeat;
    s32 item = training->m_item;
    s32 next = item;
    u32 trigger = reinterpret_cast<IfMinigameTrainingButtons*>(&training->m_controller)->m_trigger;
    s32 numItems = training->m_numItems;
    if (repeat & 2) {
        if (++next >= numItems) {
            next = 0;
        }
    }
    if (repeat & 1) {
        if (--next < 0) {
            next = numItems - 1;
        }
    }
    if (next != item) {
        g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
        training->m_item = next;
        int kind;
        if (next < 0 || next >= training->m_numItems) {
            kind = 0x38;
        } else {
            kind = training->m_itemTbl[next];
        }
        training->m_msg->printIndex(1, fn_800AFA04(kind), training->m_itemMsgData);
    }
    if (trigger & 0x100) {
        int kind;
        if (training->m_item < 0 || training->m_item >= training->m_numItems) {
            kind = 0x38;
        } else {
            kind = training->m_itemTbl[training->m_item];
        }
        training->m_generateItemKind = fn_800AFA1C(kind);
    }
}

void IfMinigameTraining::curPosProcNumCP(IfMinigameTraining* training) {
    u32 repeat = reinterpret_cast<IfMinigameTrainingButtons*>(&training->m_controller)->m_repeat;
    s32 numCP = training->m_numCP;
    s32 next = numCP;
    if (repeat & 2) {
        if (++next >= 4) {
            next = 1;
        }
    }
    if (repeat & 1) {
        if (--next < 1) {
            next = 3;
        }
    }
    if (next != numCP) {
        g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
        training->m_numCP = next;
        training->m_msg->printf(2, "%d", next);
    }
}

void IfMinigameTraining::curPosProcCPAI(IfMinigameTraining* training) {
    u32 repeat = reinterpret_cast<IfMinigameTrainingButtons*>(&training->m_controller)->m_repeat;
    s32 cpAI = training->m_cpAI;
    s32 next = cpAI;
    if (repeat & 2) {
        if (++next >= 6) {
            next = 0;
        }
    }
    if (repeat & 1) {
        if (--next < 0) {
            next = 5;
        }
    }
    if (next != cpAI) {
        g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
        training->m_cpAI = next;
        training->m_msg->printIndex(3, s_cpAIMsgTbl[next]);
        if (training->m_statusVisible) {
            training->m_msg->printIndex(9, s_cpAIMsgTbl[training->m_cpAI]);
        }
    }
}

void IfMinigameTraining::curPosProcCPDamage(IfMinigameTraining* training) {
    u32 repeat = reinterpret_cast<IfMinigameTrainingButtons*>(&training->m_controller)->m_repeat;
    s32 cpDamage = training->m_cpDamage;
    s32 next = cpDamage;
    if (repeat & 2) {
        if (++next >= 1000) {
            next = 0;
        }
    }
    if (repeat & 1) {
        if (--next < 0) {
            next = 999;
        }
    }
    if (next != cpDamage) {
        g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
        training->m_cpDamage = next;
        training->m_msg->printf(4, "%d", next);
    }
}

void IfMinigameTraining::curPosProcCamera(IfMinigameTraining* training) {
    u32 repeat = reinterpret_cast<IfMinigameTrainingButtons*>(&training->m_controller)->m_repeat;
    s32 camera = training->m_camera;
    s32 next = camera;
    if (repeat & 2) {
        if (++next >= 2) {
            next = 0;
        }
    }
    if (repeat & 1) {
        if (--next < 0) {
            next = 1;
        }
    }
    if (next != camera) {
        g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
        training->m_camera = next;
        training->m_msg->printIndex(5, s_cameraMsgTbl[next]);
        training->setCameraType(training->m_camera == 0 ? CameraType_Normal : CameraType_Follow);
    }
}

void IfMinigameTraining::curPosProcStatusVisible(IfMinigameTraining* training) {
    u32 repeat = reinterpret_cast<IfMinigameTrainingButtons*>(&training->m_controller)->m_repeat;
    s32 visible = training->m_statusVisible;
    s32 next = visible;
    if (repeat & 2) {
        if (++next >= 2) {
            next = 0;
        }
    }
    if (repeat & 1) {
        if (--next < 0) {
            next = 1;
        }
    }
    if (next != visible) {
        g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
        training->m_statusVisible = next == 1;
        training->m_msg->printIndex(6, training->m_statusVisible ? StatusVisibleMsg_On : StatusVisibleMsg_Off);
        bool show = training->m_statusVisible;
        float frame = show ? 1.0f : 0.0f;
        training->m_models[1]->setFrameVisible(frame);
        if (show) {
            training->m_msg->printIndex(8, s_speedMsgTbl[training->m_speed]);
            training->m_msg->printIndex(9, s_cpAIMsgTbl[training->m_cpAI]);
            training->printStatusDamage(training->m_damage);
            training->printStatusCombo(training->m_combo);
            training->printStatusTotalDamage(training->m_totalDamage);
        } else {
            training->m_msg->printf(8, "");
            training->m_msg->printf(9, "");
            training->m_msg->printf(10, "");
            training->m_msg->printf(11, "");
            training->m_msg->printf(12, "");
        }
        training->m_statusVisible = show;
    }
}

void IfMinigameTraining::curPosProcQuit(IfMinigameTraining* training) {
    if (reinterpret_cast<IfMinigameTrainingButtons*>(&training->m_controller)->m_trigger & 0x100) {
        g_sndSystem->playSE(static_cast<SndID>(1), -1, 0, 0, -1);
        training->m_quit = true;
    }
}
