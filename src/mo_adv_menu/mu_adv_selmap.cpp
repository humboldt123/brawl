#include <gf/gf_3d_scene.h>
#include <gf/gf_application.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_pad_status.h>
#include <gf/gf_pad_system.h>
#include <gm/gm_global.h>
#include <memory.h>
#include <mt/mt_vector.h>
#include <nw4r/g3d/g3d_anmchr.h>
#include <nw4r/g3d/g3d_anmclr.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <snd/snd_system.h>
#include <stdio.h>
#include <string.h>
#include <types.h>

#include <mu/menu.h>
#include <mu/mu_adv_selmap.h>

// main.dol functions with names that the shared headers do not declare.
// muMenuController::getPadNo()
extern "C" int fn_800BA3D8(muMenuController* controller);
// muMenuController::exchangeDir8ToVec(direction): the vector of one of the eight directions of the stick
extern "C" Vec2f fn_800BA0E0(int direction);
// Vec2f::normalize()
extern "C" void fn_8003DBA8(Vec2f* vec);
// GameGlobal::getNameCount() and GameGlobal::getName(number, buffer): the names that were saved
extern "C" int fn_8004D148(GameGlobal* global);
// MuMsg::create(font setting, heap, heap): the third argument is not a HeapType in the symbol
extern "C" MuMsg* create__5MuMsgFUlQ25Heaps8HeapTypeUl(u32 fontSetting, Heaps::HeapType heap, u32 heap2);
// gfSceneRoot::layerUpdateFrame(layer), gfSceneRoot::removeInfoCamResAnm() and gfSceneRoot::setInfoCamResAnm(animation)
extern "C" void fn_8000DB40(gfSceneRoot* root, int layer);
extern "C" void fn_8000D7B8(gfSceneRoot* root);
extern "C" void fn_8000D6DC(gfSceneRoot* root, void* resAnmScn);
// The scene module: the loader of the pack (load(heap, path, archive, size) and isLoaded())
extern "C" void fn_1_3D44(scResourceLoader* loader, int heap, const char* path, gfArchive* archive, int size);
extern "C" bool fn_1_3E50(scResourceLoader* loader);
// nw4r::g3d::ScnGroup::Construct(allocator, size, capacity)
extern "C" nw4r::g3d::G3dObj* fn_801AB6CC(MEMAllocator* allocator, u32* size, int capacity);
// nw4r::g3d::ScnMdlExpand::Construct(allocator, size, capacity, base model) and PushBack(object, node name)
extern "C" nw4r::g3d::G3dObj* Construct__Q34nw4r3g3d12ScnMdlExpandFP12MEMAllocatorPUliPQ34nw4r3g3d6ScnMdl(MEMAllocator* allocator, u32* size,
                                                                                                         int capacity, void* scnMdl);
extern "C" void PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjPCc(void* expand, void* object, const char* name);
// nw4r::g3d::ResFile::GetResAnmScn(index) and ResAnmScn::Bind(animation)
extern "C" void* GetResAnmScn__Q34nw4r3g3d7ResFileCFi(const nw4r::g3d::ResFile* resFile, int index);
extern "C" void Bind__Q34nw4r3g3d9ResAnmScnFQ34nw4r3g3d9ResAnmScn(void* self, void* anm);
// nw4r::g3d::ScnMdl::CopiedVisAccess::SetVisibilityEx(visible)
extern "C" bool SetVisibilityEx__Q44nw4r3g3d6ScnMdl15CopiedVisAccessFb(nw4r::g3d::ScnMdl::CopiedVisAccess* access, bool visible);
// MuObject::getNodeID(name) and MuObject::getNodeAnimLength()
extern "C" int getNodeID__8MuObjectFPCc(MuObject* object, const char* name);
extern "C" int getNodeAnimLength__8MuObjectFv(MuObject* object);

// Whether the menu is finished with what it does for the scene (what it tells the scene is in the result).
static inline bool isFinished(int result) {
    if (result == 2 || result == 3 || result == 4 || result == 8 || result == 9 || static_cast<u32>(result - 5) <= 1) {
        return true;
    }
    return false;
}

muAdvSelmapTask* muAdvSelmapTask::create(const muAdvSelmapTaskParam* param) {
    muAdvSelmapTask* task = new (Heaps::MenuInstance) muAdvSelmapTask();
    task->initProc(param);
    return task;
}

muAdvSelmapTask::muAdvSelmapTask()
    : gfTask("muAdvSelmap", Category_Menu, 0xF, 8, true), m_result(0), m_subResult(0), m_node(0), m_button(5), m_isMap(true), m_has2P(false),
      m_pad1(0), m_pad2(0), m_wait(0), m_msg(NULL), m_group(NULL), m_step(Step_Load), m_stepPrev(Step_Count), m_scrollTarget(0),
      m_scrollMax(0), m_unk4C8(false), m_archive(NULL), m_loader(NULL), m_notice(NULL), m_msgData(NULL), m_noticeShown(false),
      m_unk5E5(false), m_unk5E6(false), m_frame(0) {
    m_resFileMap = nw4r::g3d::ResFile(static_cast<void*>(NULL));
    m_resFileScene = nw4r::g3d::ResFile(static_cast<void*>(NULL));
    m_resFileMsg = nw4r::g3d::ResFile(static_cast<void*>(NULL));
    for (int i = 0; i < 0xB2; i++) {
        m_objects[i] = NULL;
    }
    m_scnMdls[0] = NULL;
    m_scnMdls[1] = NULL;
    memset(m_name1, 0, sizeof(m_name1));
    memset(m_name2, 0, sizeof(m_name2));
    m_unk5CC = 0;
    m_unk5D0 = 0;
    m_count1 = 0;
    m_count2 = 0;
    if (isFinished(m_result)) {
        m_result = 0;
        m_wait = 10;
    }
    m_unk4B8 = false;
    m_unk4B9 = false;
    m_startButton = -1;
    m_archive = new (Heaps::MenuResource) gfArchive();
    m_loader = new (Heaps::MenuResource) scResourceLoader();
    fn_1_3D44(m_loader, Heaps::MenuResource, "menu2/sc_adv_selmap.pac", m_archive, 0);
    muMenu::loadMenuSound();
}

muAdvSelmapTask::~muAdvSelmapTask() {
    for (int i = 0; i < 2; i++) {
        if (m_scnMdls[i] != NULL) {
            m_scnMdls[i]->Destroy();
        }
    }
    for (int i = 0; i < 0xB2; i++) {
        delete m_objects[i];
    }
    fn_8000D7B8(g_gfSceneRoot);
    if (m_group != NULL) {
        m_group->Destroy();
        m_group = NULL;
    }
    if (m_msg != NULL) {
        delete m_msg;
        m_msg = NULL;
    }
    if (m_msgData != NULL) {
        m_msgData = NULL;
    }
    if (m_archive != NULL) {
        delete m_archive;
        m_archive = NULL;
    }
    if (m_loader != NULL) {
        delete m_loader;
        m_loader = NULL;
    }
    if (m_resFileMap.ptr() != NULL) {
        m_resFileMap = nw4r::g3d::ResFile(static_cast<void*>(NULL));
    }
    if (m_resFileScene.ptr() != NULL) {
        m_resFileScene = nw4r::g3d::ResFile(static_cast<void*>(NULL));
    }
    if (m_resFileMsg.ptr() != NULL) {
        m_resFileMsg = nw4r::g3d::ResFile(static_cast<void*>(NULL));
    }
}

static inline muAdvSelmapAdvData* getAdvData() {
    return reinterpret_cast<muAdvSelmapAdvData*>(g_GameGlobal->m_advData);
}

// Whether the second part of the adventure mode is open.
// HYPOTHESIS: the two words say which of the places after the first part were opened.
static inline bool isAdvOpen(muAdvSelmapAdvData* adv) {
    if (adv->m_unk48A0 != 0) {
        return true;
    }
    if (adv->m_unk48A4 != 0) {
        return true;
    }
    return false;
}

void muAdvSelmapTask::initProc(const muAdvSelmapTaskParam* param) {
    m_param3 = param->m_count1;
    m_param4 = param->m_count2;
    if (m_param4 >= 0) {
        m_has2P = true;
    }
    int limit = fn_8004D148(g_GameGlobal) - 5;
    if (m_param3 > 0) {
        m_count1 = m_param3;
        if (m_count1 > limit) {
            m_count1 = limit;
        }
        m_unk5CC = m_param3 - m_count1;
    }
    if (m_param4 > 0) {
        m_count2 = m_param4;
        if (m_count2 > limit) {
            m_count2 = limit;
        }
        m_unk5D0 = m_param4 - m_count2;
    }
    m_pad1 = param->m_pad1;
    fn_800B96E8(&m_controllers[0], m_pad1 & 0xFF);
    m_pad2 = param->m_pad2;
    fn_800B96E8(&m_controllers[1], m_pad2 & 0xFF);
    m_scroll = 0;
    m_unk58 = false;
    m_startNode = param->m_unk00;
    if (param->m_button > 0) {
        m_startButton = param->m_button;
        m_unk4B8 = true;
        m_unk4B9 = true;
    }
    muAdvSelmapAdvData* adv = getAdvData();
    m_unk4BA = false;
    if (isAdvOpen(adv) == false) {
        m_unk4BA = true;
        if (m_startButton == 2) {
            m_startButton = 3;
        }
    }
    m_frame = 0;
}

// The init and main function of each step.
static const muAdvSelmapTask::StepFunc sStepFuncs[muAdvSelmapTask::Step_Count] = {
    {muAdvSelmapTask::stepLoadInit, muAdvSelmapTask::stepLoadMain},
    {muAdvSelmapTask::stepAppearInit, muAdvSelmapTask::stepAppearMain},
    {muAdvSelmapTask::stepSelectInit, muAdvSelmapTask::stepSelectMain},
    {muAdvSelmapTask::stepEndInit, muAdvSelmapTask::stepEndMain},
};

// Makes the texts: the number of fighters and the names of the players.
void muAdvSelmapTask::initMsg() {
    char name[0x15];
    m_msg = create__5MuMsgFUlQ25Heaps8HeapTypeUl(3, Heaps::MenuInstance, Heaps::MenuResource);
    m_msg->allocMsgBuf(0x100, 0xC);
    m_msg->setMsgData(m_archive->getData(0, Data_Type_Misc, 2, 0xFFFE));
    m_msg->attachScnMdlSimple(1, m_objects[1]->m_sceneModel, 0, 0.0625f);
    m_msg->setAlignMode(1, MuMsg::Align_Center);
    m_msg->setFontColor(1, 0xFA, 0xFA, 0xFA, 0xFF);
    m_msg->attachScnMdlSimple(6, m_objects[1]->m_sceneModel, 1, 1.0f / 24.0f);
    m_msg->setFontType(6, 1);
    m_msg->setFontColor(6, 0xCF, 0xCF, 0xCF, 0xFF);
    m_msg->attachScnMdlSimple(2, m_objects[1]->m_sceneModel, 2, 1.0f / 24.0f);
    m_msg->setFontType(2, 1);
    m_msg->setFontColor(2, 0xFF, 0xDC, 0xB7, 0xFF);
    m_msg->attachScnMdlSimple(4, m_objects[1]->m_sceneModel, 3, 0.05f);
    m_msg->setFontType(4, 1);
    m_msg->setFontColor(4, 0xFF, 0xC9, 0xC9, 0xFF);
    m_msg->attachScnMdlSimple(5, m_objects[1]->m_sceneModel, 4, 0.05f);
    m_msg->setFontType(5, 1);
    m_msg->setFontColor(5, 0xC3, 0xDC, 0xFF, 0xFF);
    if (m_param3 > 0) {
        showPlayerName(0, m_param3, name, 4, 0x22, m_name1);
    }
    if (m_param4 > 0) {
        showPlayerName(1, m_param4, name, 5, 0x23, m_name2);
    }
    m_msgData = m_archive->getData(0, Data_Type_Misc, 4, 0xFFFE);
}

// The position of a place of the map (the model of the map has a node for each place: "pos00", "pos01", ...).
static inline Vec2f getNodePos(muAdvSelmapTask* task, int node) {
    MuObject* map = task->m_objects[0];
    char name[6];
    sprintf(name, "pos%02d", node);
    Vec3f pos;
    map->getPos(&pos, name);
    return Vec2f(pos.m_x, pos.m_y);
}

// Finds the place that the stick points to from the place: the places that are in the direction of the stick are scored by
// how well the direction fits and how near they are.
int muAdvSelmapTask::getNextNode(int player, int node) {
    float best = 0.0f;
    int next = node;
    muAdvSelmapNode* target = getAdvData()->m_nodes;
    int direction = *reinterpret_cast<int*>(reinterpret_cast<u8*>(&m_controllers[player]) + 0x94);
    if (direction == 8) {
        return node;
    }
    Vec2f dir = fn_800BA0E0(direction);
    Vec2f delta;
    Vec2f current = getNodePos(this, node);
    for (int i = 0; i < 34; i++, target++) {
        if (i == node) {
            continue;
        }
        if (target->m_hidden == 1) {
            continue;
        }
        if (target->m_kind == 0) {
            continue;
        }
        Vec2f targetPos = getNodePos(this, i);
        delta = targetPos - current;
        float length = delta.length();
        if (length <= 0.0f) {
            continue;
        }
        delta.normalize();
        float dot = dir.m_x * delta.m_x + dir.m_y * delta.m_y;
        if (dot <= 0.2f) {
            continue;
        }
        float score = dot / length;
        if (score > best) {
            next = i;
            best = score;
        }
    }
    return next;
}

void muAdvSelmapTask::processDefault() {
    if (m_stepPrev != m_step) {
        if (sStepFuncs[m_step].m_init != NULL) {
            sStepFuncs[m_step].m_init(this);
        }
        m_stepPrev = m_step;
    }
    if (sStepFuncs[m_step].m_main != NULL) {
        m_step = sStepFuncs[m_step].m_main(this);
    }
    fn_800B9984(&m_controllers[0]);
    fn_800B9984(&m_controllers[1]);
    if (fn_1_3E50(m_loader) && muMenu::isLoadFinishMenuSound()) {
        mainProc();
    }
    if (m_result != 2 && m_result != 3 && m_result != 4 && m_result != 5 && m_result != 6) {
        fn_8000DB40(g_gfSceneRoot, 8);
    }
}

// The scroll of the map is as far as the farthest place that is open: the places are in groups.
int muAdvSelmapTask::getScrollLimit() {
    int limit = 0;
    muAdvSelmapNode* node = getAdvData()->m_nodes;
    for (int i = 0; i < 34; i++, node++) {
        if (node->m_hidden == 1) {
            continue;
        }
        if (node->m_kind == 0) {
            continue;
        }
        switch (i) {
        case 9:
        case 11:
        case 13:
        case 15:
        case 17:
        case 18:
        case 19:
        case 20:
        case 21:
        case 25:
        case 26:
        case 27:
            if (limit < 0x28) {
                limit = 0x28;
            }
            break;
        case 28:
        case 29:
        case 30:
        case 31:
        case 32:
        case 33:
            if (limit < 0x50) {
                limit = 0x50;
            }
            break;
        }
    }
    return limit;
}

// Whether the pad scrolls the map up: the stick (or the buttons) of the kind of the pad.
bool muAdvSelmapTask::isScrollUp(gfPadStatus* pad) {
    switch (pad->m_controllerType) {
    case gfPadType::GCC:
        if ((pad->m_buttonsCurrentFrame2.bits & GF_PAD_BUTTON_BIT_L) != 0 || (pad->m_buttonsCurrentFrame2.bits & GF_PAD_BUTTON_BIT_Y) != 0) {
            return true;
        }
        break;
    case gfPadType::WII_CLASSIC:
        if ((pad->m_buttonsCurrentFrame2.bits & GF_PAD_BUTTON_BIT_L) != 0 || (pad->m_buttonsCurrentFrame2.bits & GF_PAD_BUTTON_BIT_Y) != 0) {
            return true;
        }
        break;
    case gfPadType::WIIMOTE:
        if ((pad->m_buttonsCurrentFrame2.bits & 0x200000) != 0) {
            return true;
        }
        if ((pad->m_buttonsCurrentFrame2.bits & 0x400000) != 0 && (pad->m_buttonsCurrentFrame2.bits & 0x10000) != 0) {
            return true;
        }
        break;
    case gfPadType::NUNCHUK:
        if ((pad->m_buttonsCurrentFrame2.bits & 0x10000) != 0) {
            return true;
        }
        break;
    }
    return false;
}

bool muAdvSelmapTask::isScrollDown(gfPadStatus* pad) {
    switch (pad->m_controllerType) {
    case gfPadType::GCC:
        if ((pad->m_buttonsCurrentFrame2.bits & GF_PAD_BUTTON_BIT_R) != 0 || (pad->m_buttonsCurrentFrame2.bits & GF_PAD_BUTTON_BIT_X) != 0) {
            return true;
        }
        break;
    case gfPadType::WII_CLASSIC:
        if ((pad->m_buttonsCurrentFrame2.bits & GF_PAD_BUTTON_BIT_R) != 0 || (pad->m_buttonsCurrentFrame2.bits & GF_PAD_BUTTON_BIT_X) != 0) {
            return true;
        }
        break;
    case gfPadType::WIIMOTE:
        if ((pad->m_buttonsCurrentFrame2.bits & 0x80000) != 0) {
            return true;
        }
        if ((pad->m_buttonsCurrentFrame2.bits & 0x400000) != 0 && (pad->m_buttonsCurrentFrame2.bits & 0x20000) != 0) {
            return true;
        }
        break;
    case gfPadType::NUNCHUK:
        if ((pad->m_buttonsCurrentFrame2.bits & 0x20000) != 0) {
            return true;
        }
        break;
    }
    return false;
}

// Whether the pad decides (A on the pads with buttons; on the Wii remote the button 2 only when 1 is not held).
static inline bool isDecide(const gfPadStatus& pad) {
    switch (pad.m_controllerType) {
    case gfPadType::GCC:
    case gfPadType::WII_CLASSIC:
    case gfPadType::NUNCHUK:
        if ((pad.m_buttonsPressedThisFrame.bits & GF_PAD_BUTTON_BIT_A) != 0) {
            return true;
        }
        break;
    case gfPadType::WIIMOTE:
        if ((pad.m_buttonsCurrentFrame2.bits & 0x400000) == 0 && (pad.m_buttonsPressedThisFrame.bits & 0x20000) != 0) {
            return true;
        }
        break;
    }
    return false;
}

// Whether the pad cancels (B).
static inline bool isCancel(const gfPadStatus& pad) {
    switch (pad.m_controllerType) {
    case gfPadType::GCC:
    case gfPadType::WII_CLASSIC:
    case gfPadType::NUNCHUK:
        if ((pad.m_buttonsPressedThisFrame.bits & GF_PAD_BUTTON_BIT_B) != 0) {
            return true;
        }
        break;
    case gfPadType::WIIMOTE:
        if ((pad.m_buttonsCurrentFrame2.bits & 0x400000) == 0 && (pad.m_buttonsPressedThisFrame.bits & 0x10000) != 0) {
            return true;
        }
        break;
    }
    return false;
}

// Whether the stick (or the directions of the pad) move the cursor of the map: the buttons that scroll do not.
static inline bool isMoveInput(muAdvSelmapTask* task, gfPadStatus* pad, u32 bits) {
    if (task->isScrollUp(pad) == true || task->isScrollDown(pad) == true) {
        return false;
    }
    if (bits & GF_PAD_BUTTON_BIT_DPAD_UP) {
        return true;
    }
    if (bits & GF_PAD_BUTTON_BIT_DPAD_DOWN) {
        return true;
    }
    if (bits & GF_PAD_BUTTON_BIT_DPAD_LEFT) {
        return true;
    }
    return (bits >> 1) & 1;
}

// Plays the animation of the model that leaves the map and goes to the menu (the second model).
static inline void startMapOut(muAdvSelmapTask* task) {
    MuObject* object = task->m_objects[1];
    object->changeAnimN("MenAdvMap0001_TopN__1");
    object->setFrameNode(0.0f);
    object->m_modelAnim->m_anmObjChrRes->SetUpdateRate(1.0f);
    object->setFrameMatCol(0.0f);
    object->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(1.0f);
}

// Reads the pads: the map is moved with the stick and a place is decided with A, the menu is moved with the directions. The
// second player joins with start on another pad and leaves with B.
void muAdvSelmapTask::mainProc() {
    gfPadStatus pad2;
    gfPadStatus pad1;
    gfPadStatus tmp;
    u32 buttons;
    if (m_wait > 0) {
        m_wait--;
        if (m_wait > 0) {
            return;
        }
    }
    if (!m_unk5E6) {
        buttons = *reinterpret_cast<u32*>(reinterpret_cast<u8*>(&m_controllers[0]) + 0x10);
        g_gfPadSystem->getSysPadStatus(m_pad1, &pad1);
        if (m_pad2 >= 0) {
            g_gfPadSystem->getSysPadStatus(m_pad2, &pad2);
        } else {
            pad2.init();
        }
    } else {
        buttons = 0;
        pad1.init();
        pad2.init();
    }
    int node = m_node;
    int button = m_button;
    int newNode = node;
    if (isFinished(m_result) == true) {
        return;
    }
    if (m_step == Step_End) {
        return;
    }
    if (!m_noticeShown && !m_unk5E6) {
        int joined = -1;
        for (int i = 0; i < 8; i++) {
            if (i != m_pad1) {
                g_gfPadSystem->getSysPadStatus(i, &tmp);
                if ((tmp.m_buttonsCurrentFrame2.bits & GF_PAD_BUTTON_BIT_START) != 0) {
                    joined = i;
                }
            }
        }
        if (joined >= 0 && !m_has2P) {
            m_pad2 = joined;
            m_result = 4;
            fn_800B96E8(&m_controllers[1], joined & 0xFF);
            MuObject* join = m_objects[0x8D];
            join->changeAnimN("MenAdvMap0010_TopN__1");
            join->m_modelAnim->m_anmObjChrRes->SetUpdateRate(1.0f);
            join->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(1.0f);
            g_sndSystem->playSE(static_cast<SndID>(0x2288), -1, 0, 0, -1);
            g_sndSystem->playSE(static_cast<SndID>(0x26), -1, 0, 0, -1);
        }
        if ((pad2.m_buttonsPressedThisFrame.bits & GF_PAD_BUTTON_BIT_B) != 0) {
            if (m_has2P == true) {
                g_sndSystem->playSE(static_cast<SndID>(0x2052), -1, 0, 0, -1);
                m_result = 0;
                m_has2P = false;
                resetJoinFrame();
                m_msg->printf(5, "");
            } else {
                g_sndSystem->playSE(static_cast<SndID>(2), -1, 0, 0, -1);
            }
        }
    }
    if (isFinished(m_result) == true) {
        return;
    }
    if (m_isMap) {
        if (!m_unk5E6 && !m_unk4B8) {
            if (isMoveInput(this, &pad1, buttons) == true) {
                newNode = getNextNode(0, node);
            }
        } else {
            newNode = node;
        }
        Vec2f pos = getNodePos(this, newNode);
        if (isMoveInput(this, &pad1, buttons) == true) {
            if (pos.m_x <= -15.0f) {
                m_scrollTarget = 0;
            }
            if (-15.0f < pos.m_x && pos.m_x < 15.0f) {
                m_scrollTarget = 0x28;
            }
            if (15.0f <= pos.m_x) {
                m_scrollTarget = 0x50;
            }
        }
        if (isScrollDown(&pad1) == true) {
            if (m_scrollTarget < m_scrollMax) {
                m_scrollTarget++;
            }
        }
        if (isScrollUp(&pad1) == true) {
            if (m_scrollTarget > 0) {
                m_scrollTarget--;
            }
        }
        if (m_scrollTarget < m_scroll) {
            m_scroll--;
        }
        if (m_scroll < m_scrollTarget) {
            m_scroll++;
        }
        if (m_scroll < 0) {
            m_scroll = 0;
        }
        if (m_scrollMax < m_scroll) {
            m_scroll = m_scrollMax;
        }
        m_frame++;
        if (isDecide(pad1) == true && m_frame >= 0x3C) {
            g_sndSystem->playSE(static_cast<SndID>(1), -1, 0, 0, -1);
            if (getAdvData()->m_nodes[node].m_kind == 1) {
                if (node == 4 || node == 0xD || node == 0x11 || node == 0x12 || node == 0x1A) {
                    m_result = 5;
                } else if (static_cast<u32>(node - 0x1E) <= 3) {
                    m_result = 6;
                } else {
                    m_subResult = 10;
                    m_step = Step_End;
                }
                if (m_subResult != 10) {
                    startMapOut(this);
                }
            } else {
                m_result = 2;
                startMapOut(this);
            }
        }
        if (m_unk4B8 == true && !m_unk5E6) {
            m_unk4B8 = false;
            m_isMap = false;
            m_button = m_startButton;
        }
        if (isCancel(pad1) == true) {
            g_sndSystem->playSE(static_cast<SndID>(2), -1, 0, 0, -1);
            m_isMap = false;
            m_button = 3;
        }
        selectNode(newNode);
        if (newNode != m_node) {
            g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
            m_node = newNode;
        }
    } else {
        m_objects[0x8E]->setFrameVisible(0.0f);
        if (m_noticeShown) {
            if (m_notice->m_step == 3) {
                if (m_notice->m_selected != 0) {
                    m_step = Step_End;
                    m_noticeShown = false;
                } else {
                    m_subResult = 0;
                    m_noticeShown = false;
                }
                m_notice->exit();
                m_notice = NULL;
            }
        } else {
            if ((buttons & GF_PAD_BUTTON_BIT_DPAD_UP)) {
                if (static_cast<u32>(button - 2) <= 2) {
                    button = 6;
                }
            } else if ((buttons & GF_PAD_BUTTON_BIT_DPAD_DOWN)) {
                if (button == 6) {
                    button = 3;
                }
            } else if ((buttons & GF_PAD_BUTTON_BIT_DPAD_RIGHT)) {
                if (static_cast<u32>(button - 2) <= 1) {
                    button++;
                } else if (button == 0) {
                    button = 2;
                }
                if (m_unk4BA) {
                    if (button == 2) {
                        button = 3;
                    }
                }
            } else if ((buttons & GF_PAD_BUTTON_BIT_DPAD_LEFT)) {
                if (static_cast<u32>(button - 3) <= 1) {
                    button--;
                } else if (button == 2) {
                    button = 0;
                }
                if (m_unk4BA) {
                    if (button == 2) {
                        button = 0;
                    }
                }
            }
            if (button != m_button) {
                g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
                m_button = button;
            } else if ((pad1.m_buttonsPressedThisFrame.bits & GF_PAD_BUTTON_BIT_A) != 0) {
                g_sndSystem->playSE(static_cast<SndID>(1), -1, 0, 0, -1);
                if (button != m_button) {
                    m_button = button;
                }
                switch (m_button) {
                case 0:
                    m_result = 3;
                    break;
                case 2:
                    m_result = 7;
                    m_subResult = 7;
                    m_step = Step_End;
                    break;
                case 3:
                    m_result = 8;
                    m_subResult = 8;
                    m_step = Step_End;
                    break;
                case 4:
                    m_notice = fn_800BEEB8(1, m_msgData, 2, &m_resFileMsg, m_group, 0x40, 0, Heaps::MenuInstance, Heaps::MenuResource, 1,
                                           1, m_pad1 & 0xFF);
                    m_subResult = 9;
                    m_noticeShown = true;
                    break;
                case 6:
                    m_isMap = true;
                    m_button = 5;
                    break;
                }
            } else if ((pad1.m_buttonsPressedThisFrame.bits & GF_PAD_BUTTON_BIT_B) != 0) {
                g_sndSystem->playSE(static_cast<SndID>(2), -1, 0, 0, -1);
                m_button = 4;
            }
        }
    }
    updateButtons(button);
}

// Creates the models of the table; they are stored in the slots of the task and start with the animation "__0".
void muAdvSelmapTask::createObjResFile(const muAdvSelmapModel* models, int count, nw4r::g3d::ResFile* resFile) {
    for (int i = 0; i < count; i++, models++) {
        int num = 1;
        if (models->m_start < models->m_end) {
            num = models->m_end - models->m_start + 1;
        }
        for (int j = 0; j < num; j++) {
            MuObject* object = MuObject::create(resFile, models->m_name, models->m_priority, NULL, Heaps::MenuInstance);
            m_objects[models->m_start + j] = object;
            char animName[0x40];
            strcpy(animName, models->m_name);
            strcat(animName, "__0");
            object->changeAnimN(animName);
            if (object->m_modelAnim != NULL) {
                object->m_modelAnim->setUpdateRate(0.0f);
            }
        }
    }
}

// Makes the models that the other models are attached to.
// MATCH-ONLY: the game does not inline this function into the step that loads the menu.
#pragma dont_inline on
void muAdvSelmapTask::createExpMdl(const muAdvSelmapScnMdl* models, int count) {
    MEMAllocator* allocator = gfHeapManager::getMEMAllocator(Heaps::MenuInstance);
    for (int i = 0; i < count; i++, models++) {
        int num = 1;
        if (models->m_start < models->m_end) {
            num = models->m_end - models->m_start + 1;
        }
        for (int j = 0; j < num; j++) {
            m_scnMdls[models->m_start + j] = Construct__Q34nw4r3g3d12ScnMdlExpandFP12MEMAllocatorPUliPQ34nw4r3g3d6ScnMdl(
                allocator, NULL, models->m_capacity, m_objects[models->m_objectNo + j]->m_sceneModel);
        }
    }
}

#pragma dont_inline reset

// The models of the map: the map (slot 0), the panel (1), the frames (2, 3, 4), the icons of the places (5 ...), the join
// frame (0x8D), the arrows (0x8E), the cursor (0x8F) and the markers (0x90 ...).
static const muAdvSelmapModel sModels[] = {
    {"MenAdvMap0000_TopN", 0, 0, 0x10},
    {"MenAdvMap0001_TopN", 1, 1, 0x36},
    {"MenAdvMap0002_TopN", 2, 2, 0x37},
    {"MenAdvMap0003_TopN", 3, 3, 0x38},
    {"MenAdvMap0004_TopN", 4, 4, 0x39},
    {"MenAdvMap0006_TopN", 5, 0x26, 0x11},
    {"MenAdvMap0007_TopN", 0x27, 0x48, 0x12},
    {"MenAdvMap0008_TopN", 0x49, 0x6A, 0x13},
    {"MenAdvMap0009_TopN", 0x6B, 0x8C, 0x14},
    {"MenAdvMap0010_TopN", 0x8D, 0x8D, 0x3A},
    {"MenAdvMap0011_TopN", 0x8E, 0x8E, 0x3B},
    {"MenAdvMap0012_TopN", 0x8F, 0x8F, 0x3C},
    {"MenAdvMap0013_TopN", 0x90, 0xB1, 0x3D},
};
// The models that the other models are attached to: the map and the panel.
static const muAdvSelmapScnMdl sScnMdls[] = {
    {0, 0, 0, 0x23},
    {1, 1, 1, 5},
};

// The slot of the model that shows a place: the models are in groups by the kind of the place.
static inline int getNodeSlot(int kind, int node) {
    switch (kind) {
    case 1:
        return node + 5;
    case 2:
        return node + 0x27;
    case 3:
        return node + 0x49;
    case 4:
        return node + 0x6B;
    default:
        return node + 5;
    }
}

// Puts the models into the scene: the icons of the places are attached to the map, the parts of the map that are not open are
// hidden and the cursor is put on the place that the menu starts with.
void muAdvSelmapTask::initScene() {
    char name[6] = {0};
    muAdvSelmapAdvData* adv = getAdvData();
    nw4r::g3d::G3dObj* map = m_scnMdls[0];
    g_gfSceneRoot->add(gfSceneRoot::Layer_Info, reinterpret_cast<nw4r::g3d::ScnMdl*>(map));
    muAdvSelmapNode* node = adv->m_nodes;
    for (int i = 0; i < 34; i++, node++) {
        sprintf(name, "pos%02d", i);
        if (node->m_hidden != 1 && node->m_kind != 0) {
            int slot = getNodeSlot(node->m_kind, i);
            MuObject* icon = m_objects[slot];
            PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjPCc(map, icon->m_sceneModel, name);
            icon = m_objects[slot];
            icon->m_scnMdl->SetPriorityDrawOpa(0x11 + i);
            icon->m_scnMdl->SetPriorityDrawXlu(0x11 + i);
        }
    }
    PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjPCc(map, m_objects[0x90]->m_sceneModel, "pos00");
    nw4r::g3d::G3dObj* panel = m_scnMdls[1];
    g_gfSceneRoot->add(gfSceneRoot::Layer_Info, reinterpret_cast<nw4r::g3d::ScnMdl*>(panel));
    PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjPCc(panel, m_objects[2]->m_sceneModel, "pos00");
    PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjPCc(panel, m_objects[3]->m_sceneModel, "pos01");
    PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjPCc(panel, m_objects[4]->m_sceneModel, "pos02");
    PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjPCc(panel, m_objects[0x8E]->m_sceneModel, "pos04");
    g_gfSceneRoot->add(gfSceneRoot::Layer_Info, reinterpret_cast<nw4r::g3d::ScnMdl*>(m_objects[0x8D]->m_sceneModel));
    if (m_has2P) {
        MuObject* join = m_objects[0x8D];
        join->changeAnimN("MenAdvMap0010_TopN__1");
        join->setFrame(static_cast<float>(getNodeAnimLength__8MuObjectFv(join) - 1));
    }
    // The parts of the map that are not open are hidden with the nodes "map_mask01" ... "map_mask09".
    MuObject* mapObj = m_objects[0];
    mapObj->m_modelAnim->unbindVisibleAnim(mapObj->m_sceneModel);
    nw4r::g3d::ScnMdl* mapMdl = mapObj->m_scnMdl;
    mapMdl->SetScnObjOption(0x30001, 0);
    char mask[] = "map_mask01";
    u8 order[9] = {9, 6, 7, 8, 1, 2, 3, 4, 5};
    for (u32 i = 1; i <= 9; i++) {
        mask[9] = '0' + order[i - 1];
        nw4r::g3d::ScnMdl::CopiedVisAccess access(mapMdl, getNodeID__8MuObjectFPCc(mapObj, mask));
        if ((adv->m_mapFlags & (1 << i)) != 0) {
            SetVisibilityEx__Q44nw4r3g3d6ScnMdl15CopiedVisAccessFb(&access, true);
        } else {
            SetVisibilityEx__Q44nw4r3g3d6ScnMdl15CopiedVisAccessFb(&access, false);
        }
    }
    if (m_unk4BA) {
        MuObject* panelObj = m_objects[4];
        panelObj->m_modelAnim->unbindVisibleAnim(panelObj->m_sceneModel);
        nw4r::g3d::ScnMdl* panelMdl = panelObj->m_scnMdl;
        panelMdl->SetScnObjOption(0x30001, 0);
        const char* hideNames[5] = {"base_2", "button_frame_2", "cursor2", "EN01", "shadow3"};
        for (u32 i = 0; i < 5; i++) {
            nw4r::g3d::ResMdl resMdl(*reinterpret_cast<void**>(reinterpret_cast<u8*>(panelObj->m_sceneModel) + 0xE8));
            if (resMdl.GetResNode(hideNames[i]).IsValid()) {
                nw4r::g3d::ScnMdl::CopiedVisAccess access(panelMdl, getNodeID__8MuObjectFPCc(panelObj, hideNames[i]));
                SetVisibilityEx__Q44nw4r3g3d6ScnMdl15CopiedVisAccessFb(&access, false);
            }
        }
    }
    int start = m_startNode;
    if (start != 0) {
        Vec2f pos = getNodePos(this, start);
        m_scroll = 0x28;
        m_scroll = m_scroll + static_cast<int>(pos.m_x) * 3;
        if (m_scroll < 0) {
            m_scroll = 0;
        }
        if (m_scroll > 0x50) {
            m_scroll = 0x50;
        }
        m_startNode = 0;
        selectNode(start);
        m_node = start;
        m_objects[0]->setFrameNode(static_cast<float>(m_scroll));
        m_scrollTarget = m_scroll;
    }
    m_scrollMax = getScrollLimit();
}

// Shows the place that the cursor is on: the marker, the arrows of the scroll and the texts (the number of the place, the
// numbers of the data).
void muAdvSelmapTask::selectNode(int node) {
    char buf1[10] = {0};
    char buf2[10] = {0};
    muAdvSelmapAdvData* adv = getAdvData();
    moveMarker(node);
    updateArrows();
    m_msg->printIndex(1, node, NULL);
    m_objects[2]->setFrameTex(1.0f + static_cast<float>(node));
    muAdvSelmapNode* entry = &adv->m_nodes[node];
    MuObject* label = m_objects[3];
    label->setFrameTex(static_cast<float>(entry->m_value + 1));
    label->setFrameMatCol(static_cast<float>(entry->m_value + 1));
    sprintf(buf1, "%d", adv->m_unk60C);
    m_msg->printf(2, buf1);
    sprintf(buf2, "%d", static_cast<int>(adv->m_unk2A8));
    m_msg->printf(6, buf2);
}

// Moves the marker model to the place: the marker is attached to the node of the place on the map.
void muAdvSelmapTask::moveMarker(int node) {
    char name[6] = {0};
    m_markerNo = node + 0x90;
    nw4r::g3d::ScnGroup* map = reinterpret_cast<nw4r::g3d::ScnGroup*>(m_scnMdls[0]);
    sprintf(name, "pos%02d", node);
    if (node != m_node) {
        map->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_objects[m_node + 0x90]->m_sceneModel));
        PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjPCc(map, m_objects[m_markerNo]->m_sceneModel, name);
    }
    if (!m_isMap) {
        map->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_objects[m_markerNo]->m_sceneModel));
        m_unk58 = true;
    }
    if (m_unk58 == true && m_isMap == true) {
        PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjPCc(map, m_objects[m_markerNo]->m_sceneModel, name);
        m_unk58 = false;
    }
    MuObject* marker = m_objects[m_markerNo];
    if (m_isMap && !m_unk5E6) {
        marker->m_modelAnim->setUpdateRate(1.0f);
    } else {
        marker->m_modelAnim->setUpdateRate(0.0f);
    }
}

// Shows the arrows of the scroll: up when the map can be scrolled up, down when it can be scrolled down.
void muAdvSelmapTask::updateArrows() {
    MuObject* arrows = m_objects[0x8E];
    if (m_unk5E6) {
        arrows->setFrameVisible(0.0f);
    } else {
        if (m_scroll == 0 && m_scroll == m_scrollMax) {
            arrows->setFrameVisible(0.0f);
        }
        if (m_scroll != 0 && m_scroll == m_scrollMax) {
            arrows->setFrameVisible(1.0f);
        }
        if (m_scroll == 0 && m_scroll != m_scrollMax) {
            arrows->setFrameVisible(2.0f);
        }
        if (m_scroll != 0 && m_scroll != m_scrollMax) {
            arrows->setFrameVisible(3.0f);
        }
    }
}

// Shows the button that is chosen in the menu (the frame of the button depends on the kind of the pad) and the names of the
// players.
void muAdvSelmapTask::updateButtons(int button) {
    MuObject* buttons = m_objects[4];
    gfPadStatus pad;
    g_gfPadSystem->getSysPadStatus(fn_800BA3D8(&m_controllers[0]), &pad);
    switch (pad.m_controllerType) {
    case gfPadType::GCC:
        buttons->setFrameTex(1.0f);
        break;
    case gfPadType::WII_CLASSIC:
        buttons->setFrameTex(4.0f);
        break;
    case gfPadType::NUNCHUK:
        buttons->setFrameTex(2.0f);
        break;
    case gfPadType::WIIMOTE:
        buttons->setFrameTex(3.0f);
        break;
    default:
        buttons->setFrameTex(0.0f);
        break;
    }
    switch (button) {
    case 0:
        buttons->setFrameNode(0.0f);
        buttons->setFrameMatCol(0.0f);
        break;
    case 1:
        buttons->setFrameNode(1.0f);
        buttons->setFrameMatCol(1.0f);
        break;
    case 2:
        buttons->setFrameNode(2.0f);
        buttons->setFrameMatCol(2.0f);
        break;
    case 3:
        buttons->setFrameNode(3.0f);
        buttons->setFrameMatCol(3.0f);
        break;
    case 4:
        buttons->setFrameNode(4.0f);
        buttons->setFrameMatCol(4.0f);
        break;
    case 5:
        buttons->setFrameNode(5.0f);
        buttons->setFrameMatCol(5.0f);
        break;
    case 6:
        buttons->setFrameNode(6.0f);
        buttons->setFrameMatCol(6.0f);
        break;
    }
    if (button == 0) {
        m_msg->setFontColor(4, 0xB8, 0x2E, 0x2E, 0xFF);
    } else {
        m_msg->setFontColor(4, 0x9A, 0x65, 0x65, 0xFF);
    }
    if (m_unk5CC == 0 && m_count1 == 0) {
        m_msg->printIndex(4, 0x22, NULL);
    } else {
        m_msg->printf(4, m_name1);
    }
    if (button == 1) {
        m_msg->setFontColor(5, 0x2E, 0x2E, 0xB8, 0xFF);
    } else {
        m_msg->setFontColor(5, 0x65, 0x65, 0x9A, 0xFF);
    }
    if (m_has2P) {
        if (m_unk5D0 == 0 && m_count2 == 0) {
            m_msg->printIndex(5, 0x23, NULL);
        } else {
            m_msg->printf(5, m_name2);
        }
    }
}

void muAdvSelmapTask::stepLoadInit(muAdvSelmapTask* task) { }

// The flag of the menu data (of the record of the game) that says that the screen is wide.
// HYPOTHESIS: the name; the flag is the high bit of the byte at 0x28.
struct muAdvSelmapMenuBits {
    u8 m_isWide : 1;
};

// Waits for the pack and the menu sound, then binds the camera and creates the models, the texts and the scene.
int muAdvSelmapTask::stepLoadMain(muAdvSelmapTask* task) {
    if (!fn_1_3E50(task->m_loader)) {
        return task->m_step;
    }
    if (!muMenu::isLoadFinishMenuSound()) {
        return task->m_step;
    }
    void* anm2;
    void* anm = NULL;
    void* data;
    if (reinterpret_cast<muAdvSelmapMenuBits*>(reinterpret_cast<u8*>(g_GameGlobal->getGlobalRecordMenuDatap()) + 0x28)->m_isWide == 0) {
        data = task->m_archive->getData(0, Data_Type_Misc, 1, 0xFFFE);
    } else {
        data = task->m_archive->getData(0, Data_Type_Misc, 0x65, 0xFFFE);
    }
    task->m_resFileScene = nw4r::g3d::ResFile(data);
    nw4r::g3d::ResFile::Init(&task->m_resFileScene);
    anm = GetResAnmScn__Q34nw4r3g3d7ResFileCFi(&task->m_resFileScene, 0);
    anm2 = anm;
    Bind__Q34nw4r3g3d9ResAnmScnFQ34nw4r3g3d9ResAnmScn(&anm, &anm2);
    fn_8000D6DC(g_gfSceneRoot, &anm);
    task->m_resFileMap = nw4r::g3d::ResFile(task->m_archive->getData(0, Data_Type_Misc, 0, 0xFFFE));
    nw4r::g3d::ResFile::Init(&task->m_resFileMap);
    task->createObjResFile(sModels, 13, &task->m_resFileMap);
    task->createExpMdl(sScnMdls, 2);
    task->initMsg();
    task->m_resFileMsg = nw4r::g3d::ResFile(task->m_archive->getData(0, Data_Type_Misc, 3, 0xFFFE));
    nw4r::g3d::ResFile::Init(&task->m_resFileMsg);
    task->m_group = fn_801AB6CC(gfHeapManager::getMEMAllocator(Heaps::MenuInstance), NULL, 0xB2);
    g_gfSceneRoot->add(gfSceneRoot::Layer_Info, reinterpret_cast<nw4r::g3d::ScnMdl*>(task->m_group));
    task->m_unk5E5 = true;
    task->initScene();
    return Step_Appear;
}

void muAdvSelmapTask::stepAppearInit(muAdvSelmapTask* task) {
    task->m_objects[0]->setFrameNode(static_cast<float>(task->m_scroll));
    MuObject* panel = task->m_objects[1];
    panel->setFrame(0.0f);
    panel->m_modelAnim->setUpdateRate(1.0f);
    if (task->m_unk4B9 == true) {
        panel->setFrame(9.0f);
        panel->m_modelAnim->setUpdateRate(0.0f);
        OSReport("MenuBoot\n");
    }
    g_gfApplication->unkD0.endKeepScreen();
    g_sndSystem->playBGM(0x281F, 0, true);
}

int muAdvSelmapTask::stepAppearMain(muAdvSelmapTask* task) {
    return Step_Select;
}

// Starts the animation of the join frame when there is no second player.
void muAdvSelmapTask::stepSelectInit(muAdvSelmapTask* task) {
    if (!task->m_has2P) {
        MuObject* join = task->m_objects[0x8D];
        join->setFrame(0.0f);
        join->m_modelAnim->setUpdateRate(1.0f);
    }
}

// Keeps the animations of the map and the icons: they run while the map is shown.
int muAdvSelmapTask::stepSelectMain(muAdvSelmapTask* task) {
    muAdvSelmapNode* node = getAdvData()->m_nodes;
    task->m_objects[0]->setFrameNode(static_cast<float>(task->m_scroll));
    for (int i = 0; i < 34; i++, node++) {
        if (node->m_hidden != 1 && node->m_kind != 0) {
            int slot = getNodeSlot(node->m_kind, i);
            if (task->m_isMap) {
                task->m_objects[slot]->m_modelAnim->setUpdateRate(1.0f);
            } else {
                task->m_objects[slot]->m_modelAnim->setUpdateRate(0.0f);
            }
        }
    }
    if (task->m_unk5E6) {
        task->m_objects[0x8D]->m_modelAnim->setUpdateRate(0.0f);
    } else {
        task->m_objects[0x8D]->m_modelAnim->setUpdateRate(1.0f);
    }
    return task->m_step;
}

void muAdvSelmapTask::stepEndInit(muAdvSelmapTask* task) { }

// The menu ends with the result that the step set.
int muAdvSelmapTask::stepEndMain(muAdvSelmapTask* task) {
    MuObject* panel = task->m_objects[1];
    getNodeAnimLength__8MuObjectFv(panel);
    panel->m_modelAnim->getFrame();
    switch (task->m_subResult) {
    case 10:
        task->m_result = 10;
        break;
    case 7:
        task->m_result = 7;
        break;
    case 8:
        task->m_result = 8;
        break;
    case 9:
        task->m_result = 9;
        break;
    }
    task->m_unk4C8 = true;
    return task->m_step;
}
