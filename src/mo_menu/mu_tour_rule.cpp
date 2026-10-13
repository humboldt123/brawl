#include <gf/gf_application.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_pad_system.h>
#include <gm/gm_global.h>
#include <memory.h>
#include <mu/mu_tour_panel_ctrl.h>
#include <mu/mu_tour_rule.h>
#include <mu/mu_tour_util.h>
#include <nw4r/g3d/g3d_resmdl.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <snd/snd_system.h>
#include <stdio.h>
#include <string.h>
#include <types.h>

// main.dol functions with names that the shared headers do not declare.
// The loading of a pack (archive, path, heap, offset, size, id)
extern "C" void fn_800151FC(gfArchive* archive, const char* path, Heaps::HeapType heap, int offset, int size, u32 id);
// MuMsg::create(font setting, heap, heap): the third argument is not a HeapType in the symbol
extern "C" MuMsg* create__5MuMsgFUlQ25Heaps8HeapTypeUl(u32 fontSetting, Heaps::HeapType heap, u32 heap2);
// The random numbers of main.dol: an integer from 0 up to the argument
long randi(long max);
// gmCharacterKind::gmCheckChKindUseEnable(kind): the fighter can be picked (it is unlocked)
extern "C" int fn_8004F4A0(int kind);
// GameGlobal::checkHideMenuRandomStageSW(): the random stage switch of the menus is hidden
extern "C" u8 fn_8004E834(GameGlobal* global);
// GameGlobal::getName(number, buffer)
extern "C" void fn_8004D6C4(GameGlobal* global, int number, char* buffer);
// muMenuController::getAButtonPushedControllerID(): the pad that pushed A
extern "C" int fn_800BA2E0();
// CameraController::getInstance() and changeInput(kind)
extern "C" void* getInstance__16CameraControllerFv();
extern "C" void changeInput__16CameraControllerFQ216CameraController4Kind(void* camera, int kind);
// sndSystem::playSEwithFixEffect(id, volume?, pitch?) and sndSystem::playBGM
extern "C" void fn_80074ECC(sndSystem* system, int id, float volume, float pitch);
// The system that keeps the picture of the screen while a menu changes
extern "C" void startKeepScreen__17gfKeepFrameBufferFv(void* keeper);
extern "C" void endKeepScreen__17gfKeepFrameBufferFv(void* keeper);
// muMenu: the numbers of the fighters, the colors of the costumes and the texts
extern "C" int exchangeMuCharKindToGmCharacterKind__6muMenuFiii(int kind);
extern "C" int exchangeMuCharKindToMuSelchkind__6muMenuFiii(int kind);
extern "C" int exchangeMuSelchkindToMuCharKind__6muMenuFiii(int kind);
extern "C" int getNumCharColor__6muMenuFiii(int kind);
extern "C" int getCharColor__6muMenuFiii(int kind, int color);
extern "C" int getCpLevelMessageId__6muMenuFiii(int level);
extern "C" int exchangeSrStageKindToMuStageKind__6muMenuFiii(int stageKind);
extern "C" float getStageFrameNo__6muMenuFiii(int stageKind);
extern "C" const char* exchangeMuStockchkind2MuCharName__6muMenuFi(int kind);
// nw4r::g3d::ScnMdlExpand::Construct(allocator, size, capacity, base model)
extern "C" nw4r::g3d::G3dObj* Construct__Q34nw4r3g3d12ScnMdlExpandFP12MEMAllocatorPUliPQ34nw4r3g3d6ScnMdl(MEMAllocator* allocator, u32* size,
                                                                                                         int capacity, void* scnMdl);
// The instance of the sound system that the menus ask for the music
extern void* lbl_805A01D8;

// The place in the application where the keeper of the picture of the screen is.
static inline void* getFrameKeeper() {
    return reinterpret_cast<char*>(g_gfApplication) + 0xD0;
}

static inline muTourData* getTourData() {
    return reinterpret_cast<muTourData*>(g_GameGlobal->m_tournamentData);
}

// What the buttons of the pads are (the bits of the pressed and repeated buttons of a gfPadStatus).
enum {
    PAD_LEFT = 0x1,
    PAD_RIGHT = 0x2,
    PAD_DOWN = 0x4,
    PAD_UP = 0x8,
    PAD_A = 0x100,
    PAD_B = 0x200,
    PAD_X = 0x400,
    PAD_Y = 0x800,
};

// The animations of the models of the screens. The entries are shared: each screen uses the ones that it needs.
static MuAnimNameData sAnimMain[8] = {
    {0.0f, 9.0f, 0, "MenTrnRule0001_TopN__0", 1},
    {0.0f, 9.0f, 0, "MenTrnRule0001_TopN__1", 1},
    {0.0f, 9.0f, 0, "MenTrnRule0001_TopN__2", 1},
    {0.0f, 9.0f, 0, "MenTrnRule0001_TopN__3", 1},
    {0.0f, 9.0f, 0, "MenTrnRule0001_TopN__4", 1},
    {0.0f, 9.0f, 0, "MenTrnRule0001_TopN__5", 1},
    {0.0f, 9.0f, 0, "MenTrnRule0001_TopN__6", 1},
    {0.0f, 0.0f, 0, "MenTrnRule0001_TopN__0", 2},
};
static MuAnimNameData sAnimPlayers[3] = {
    {0.0f, 5.0f, 0, "MenTrnRule0010_TopN__1", 3},
    {0.0f, 5.0f, 0, "MenTrnRule0010_TopN__2", 3},
    {0.0f, 5.0f, 0, "MenTrnRule0010_TopN__3", 3},
};
static MuAnimNameData sAnimWarning[2] = {
    {0.0f, 9.0f, 0, "MenTrnRule0033_TopN__0", 1},
    {0.0f, 9.0f, 0, "MenTrnRule0033_TopN__1", 1},
};
static MuAnimNameData sAnimPaging[2] = {
    {0.0f, 9.0f, 0, "MenTrnRule0040_TopN__0", 1},
    {0.0f, 9.0f, 0, "MenTrnRule0040_TopN__1", 1},
};
static MuAnimNameData sAnimCpuLevel[2] = {
    {0.0f, 9.0f, 0, "MenTrnRule0050_TopN__0", 1},
    {0.0f, 9.0f, 0, "MenTrnRule0050_TopN__1", 1},
};
static MuAnimNameData sAnimHandicap[2] = {
    {0.0f, 9.0f, 0, "MenTrnRule0041_TopN__0", 1},
    {10.0f, 19.0f, 0, "MenTrnRule0041_TopN__0", 1},
};
static MuAnimNameData sAnimConfirm[2] = {
    {0.0f, 9.0f, 0, "MenTrnRule0044_TopN__2", 1},
    {0.0f, 9.0f, 0, "MenTrnRule0044_TopN__3", 1},
};
static MuAnimNameData sAnimStageMode[2] = {
    {0.0f, 9.0f, 0, "MenTrnRule0070_TopN__0", 1},
    {0.0f, 9.0f, 0, "MenTrnRule0070_TopN__1", 1},
};

// The indexes of the objects of the task: the slots of the fighters have one object each.
enum {
    OBJ_BG = 0,                 // the background
    OBJ_TITLE = 1,              // the title and the texts of the screen
    OBJ_PLAYERS = 4,            // the arrows of the number of the fighters in a match
    OBJ_ENTRANTS = 5,           // the number of fighters that take part
    OBJ_CONFIRM = 6,
    OBJ_WARNING = 7,            // the notice that there are less controllers than players
    OBJ_HANDICAP_WND = 8,       // the window of the handicap
    OBJ_HANDICAP_LIST = 9,      // the list of the handicap
    OBJ_PAGING = 10,            // the buttons that change the page of the slots
    OBJ_CPU_LEVEL = 11,         // the level of the CPU
    OBJ_PAGE_COUNT = 12,        // the count of the pages
    OBJ_SLOT_CURSOR = 13,       // the cursor on a slot
    OBJ_CHAR_FRAME = 14,        // the frame of the picker of the fighter
    OBJ_CHAR_CURSOR = 15,       // the cursor on a fighter in the picker
    OBJ_STAGE_MODE = 16,        // how the stages are picked
    OBJ_COLOR_CURSOR = 17,      // the cursor on a costume
    OBJ_SLOT_FRAME = 0x13,      // the frame of each slot (0x20 of them)
    OBJ_SLOT_FACE = 0x33,       // the face of the fighter of each slot
    OBJ_SLOT_ICON = 0x53,       // the stock icon of the fighter of each slot
    OBJ_SLOT_HANDICAP = 0x73,   // the handicap of each slot
    OBJ_CHAR_FACES = 0x93,      // the faces in the picker of the fighter
    OBJ_COLOR_ICONS = 0xB7,     // the costumes of a fighter (9)
    OBJ_OPTION = 0xC0,          // the options of the stage mode
    OBJ_RULE_CONFIRM = 0xC1,    // the button of the confirmation
    OBJ_STAGE_ICON = 0xC2,      // the icon of the stage
};

// Copies the text of a message to the end of a buffer, and returns the length of the buffer then.
static int appendMsg(MuMsg* msg, char* buffer, int length, u32 msgNo) {
    int size;
    const char* text = msg->getMsgData(msgNo, &size);
    memcpy(buffer + length, text, size);
    return length + size;
}

// Writes the name of a slot: the text of the kind ("Player", "CPU") and the number with two digits.
static void makeSlotName(MuMsg* msg, muTourSlot* slot, u32 kindMsgNo, u32 number) {
    char* name = slot->m_name;
    memset(name, 0, 0x15);
    int length = appendMsg(msg, name, 0, kindMsgNo);
    length = appendMsg(msg, name, length, number / 10 + 0x2B);
    appendMsg(msg, name, length, number % 10 + 0x2B);
}

// The number of the icon of the first costume of a fighter in the file of the icons.
static inline u32 getStockIconBase(int character) {
    int number = muMenu::exchangeGmCharacterKind2Something(muMenu::exchangeMuSelchkind2MuStockchkind(character));
    return (u32)(float)(number * 10 + 1);
}

// Counts the controllers that are plugged in (up to 4).
static inline u32 countPads() {
    u32 count = 0;
    for (int i = 0; i < 8 && count < 4; i++) {
        gfPadStatus status;
        g_gfPadSystem->getSysPadStatus(i, &status);
        if (status.m_error >= 0) {
            count++;
        }
    }
    return count;
}

// Puts the icon of a costume of a fighter on a model: the face texture of the stock icon.
static void setStockIcon(MuObject* object, int character, int color, nw4r::g3d::ResFile* icons) {
    u32 base;
    if (character < 0x28) {
        base = getStockIconBase(character);
    } else {
        base = 0;
        color = 0;
    }
    char name[64];
    sprintf(name, "InfStc.%03d", base + color);
    object->changeMaterialTex(object->m_resMdl.GetResMat("lambert87")->m_id, name, icons);
}

muTourRuleTask::muTourRuleTask(int mode) : gfTask("muTourRuleTask", Category_Info, 14, 6, true) {
    m_state = 0;
    m_result = 0;
    m_timer = 0;
    m_edited = 0;
    m_nameDone = 0;
    m_resRuleA = nw4r::g3d::ResFile(static_cast<void*>(NULL));
    m_resRuleB = nw4r::g3d::ResFile(static_cast<void*>(NULL));
    m_table = NULL;
    m_padWarning = false;
    m_resC = nw4r::g3d::ResFile(static_cast<void*>(NULL));
    m_resNameEntry = nw4r::g3d::ResFile(static_cast<void*>(NULL));
    m_resD = nw4r::g3d::ResFile(static_cast<void*>(NULL));
    m_resWifiWnd = nw4r::g3d::ResFile(static_cast<void*>(NULL));
    m_resCharaFace = nw4r::g3d::ResFile(static_cast<void*>(NULL));
    m_resStockFace = nw4r::g3d::ResFile(static_cast<void*>(NULL));
    m_playersPerMatch = 0;
    m_sizeIndex = 0;
    m_entrants = 0;
    m_cursor = 0;
    m_transition = false;
    m_ctrlPad = 0;
    m_panel = NULL;
    m_ruleTask = NULL;
    m_charCursor = 0;
    m_slotCursor = 0;
    m_cpuLevel = 9;
    m_onPaging = false;
    m_onCpuLevel = false;
    m_hasCpu = false;
    m_cpuAnim = 0;
    m_padCount = 0;
    m_nameWnd = NULL;
    m_handicap = 0;
    m_handicapTop = 0;
    m_page = 0;
    m_pageY = 0.0f;
    m_pageMoving = false;
    m_charColumns = 0;
    m_charScroll = 0;
    m_charScrollX = 0.0f;
    m_charScrollMoving = false;
    m_charGridCursor = 0;
    m_selChar = 0x29;
    m_stageMode = 0;
    m_confirmSel = 0;
    m_archiveA = NULL;
    m_archiveB = NULL;
    m_mode = mode;
    m_colorShown = false;
    unk2C_b1 = false;

    m_root = MenuRoot::create("TourMeleeTask", 300, "/menu/defaultcamera/CharacterSelect.brres");
    m_edited = getTourData()->m_edited;
    m_ctrlPad = getTourData()->m_ctrlPad;

    // Two fighters at least in a match; as many as there are controllers (up to four).
    u32 pads = countPads();
    m_playersPerMatch = pads;
    if (pads < 2) {
        m_playersPerMatch = 2;
    }

    // The fighters that can be picked, in the order of the menu (and the random choice at the end).
    static const int sCharKinds[0x23] = {0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 29, 30, 15,
                                         16, 17, 18, 19, 20, 21, 22, 27, 23, 28, 24, 25, 31, 32, 33, 26, 34};
    m_charCount = 0;
    for (int i = 0; i < 0x23; i++) {
        int kind = sCharKinds[i];
        if (fn_8004F4A0(exchangeMuCharKindToGmCharacterKind__6muMenuFiii(kind)) == 1) {
            m_charList[m_charCount] = kind;
            m_charCount++;
        }
    }
    m_charList[m_charCount] = 0x23;
    m_charCount++;

    m_objProc = muObjProcessTask::create(Heaps::MenuInstance);
    m_objProc->m_objs = m_objs;
    m_objProc->m_count = 0xC3;
    for (int i = 0; i < 0xC3; i++) {
        m_objs[i] = NULL;
    }
    for (int i = 0; i < 0x28; i++) {
        m_mdls[i] = NULL;
    }
    for (int i = 0; i < 11; i++) {
        m_loaded[i] = false;
    }

    m_archiveA = new (Heaps::MenuResource) gfArchive;
    m_archiveB = new (Heaps::MenuResource) gfArchive;
    const char* files[8] = {
        "/menu/common/StockFaceTex.brres",
        "/menu/msg/sel_char.msbin",
        "/menu/msg/tour_rule.msbin",
        "/menu/msg/stage_name.msbin",
        "/menu/msg/name_remote.msbin",
        "/menu/common/name_entry.brres",
        "/menu/common/CharaFaceTex.brres",
        "/menu/cmnwindow/wifi_notice_wnd.brres",
    };
    for (int i = 0; i < 8; i++) {
        m_files[i].readRequest(files[i], Heaps::MenuResource, 0, 0);
    }
    fn_800151FC(m_archiveA, "/menu/tournament/TourRule.pac", Heaps::MenuResource, 0, 0, 0xFF);
    fn_800151FC(m_archiveB, "/menu/main/MenuRule.pac", Heaps::MenuResource, 0, 0, 0xFF);
    muMenu::loadMenuSound();

    m_msg = create__5MuMsgFUlQ25Heaps8HeapTypeUl(1, Heaps::MenuInstance, Heaps::MenuResource);
    m_msg->allocMsgBuf(0x100, 0x13);
    m_msgSlot = create__5MuMsgFUlQ25Heaps8HeapTypeUl(2, Heaps::MenuInstance, Heaps::MenuResource);
    m_msgSlot->allocMsgBuf(0x40, 0x25);
    m_msgCpu = create__5MuMsgFUlQ25Heaps8HeapTypeUl(1, Heaps::MenuInstance, Heaps::MenuResource);
    m_msgCpu->allocMsgBuf(0x100, 1);
    fn_800B96E8(&m_ctrl, 0xF0);

    memset(m_slots, 0, sizeof(m_slots));
    for (int i = 0; i < 0x20; i++) {
        m_slots[i].m_kind = 0;
        m_slots[i].m_nameTag = 0x78;
    }

    // The time of a match is one minute at least when the matches are not played with stocks.
    gmSetRule* rule = g_GameGlobal->m_setRule;
    if (rule->m_rule != 1 && rule->m_timeMinutes == 0) {
        rule->m_timeMinutes = 1;
    }
}

// Puts the models of a screen of the rules in the groups they belong to.
void muTourRuleTask::pushbackPanel(int mode) {
    switch (mode) {
    case 1:
        tourClearGroup(m_mdls[0]);
        tourPushBack(m_mdls[0], m_objs[2]->m_scnMdl, "pos02");
        tourPushBack(m_mdls[0], reinterpret_cast<nw4r::g3d::ScnMdl*>(m_mdls[1]), "pos00");
        break;
    case 2:
        tourClearGroup(m_mdls[0]);
        tourClearGroup(m_mdls[1]);
        tourPushBack(m_mdls[0], reinterpret_cast<nw4r::g3d::ScnMdl*>(m_table->m_group), "pos01");
        tourPushBack(m_mdls[0], reinterpret_cast<nw4r::g3d::ScnMdl*>(m_mdls[1]), "pos00");
        break;
    case 3:
        tourClearGroup(m_mdls[0]);
        m_table->setTableData(m_playersPerMatch, m_sizeIndex);
        tourPushBack(m_mdls[1], reinterpret_cast<nw4r::g3d::ScnMdl*>(m_table->m_group), "pos00");
        tourPushBack(m_mdls[0], reinterpret_cast<nw4r::g3d::ScnMdl*>(m_mdls[1]), "pos02");
        tourPushBack(m_mdls[0], m_objs[OBJ_CONFIRM]->m_scnMdl, "pos00");
        break;
    }
}

muTourRuleTask::~muTourRuleTask() {
    for (int i = 0; i < 0x28; i++) {
        if (m_mdls[i] != NULL) {
            m_mdls[i]->Destroy();
            m_mdls[i] = NULL;
        }
    }
    for (int i = 0; i < 0xC3; i++) {
        if (m_objs[i] != NULL) {
            delete m_objs[i];
            m_objs[i] = NULL;
        }
    }
    for (int i = 0; i < 8; i++) {
        gfHeapManager::free(m_files[i].getBuffer());
        m_files[i].release();
    }
    delete m_archiveA;
    delete m_archiveB;
    delete m_msg;
    delete m_msgCpu;
    delete m_msgSlot;
    delete m_table;
    m_objProc->exit();
    m_objProc = NULL;
    m_panel->exit();
    m_panel = NULL;
    m_root->exit();
    m_root = NULL;
    getTourData()->m_edited = m_edited;
    getTourData()->m_ctrlPad = m_ctrlPad;
}

// Shows how many fighters take part (the arrows, the number and the table), and the texts of the screen the first time.
void muTourRuleTask::drawSelectTotalPlayer(bool first) {
    // 100 and more: 100 plus the number of the picture of the table that fits; the real number is the part of it that is
    // not the hundreds (HYPOTHESIS: the sizes with such a code have a picture of their own)
    const int sizes[3][9] = {
        {4, 6, 8, 12, 16, 24, 32, 0, 0},
        {6, 9, 12, 15, 18, 21, 27, 30, 0},
        {8, 12, 164, 162, 20, 24, 28, 324, 322},
    };
    const int lastSize[3] = {6, 7, 8};

    if (m_sizeIndex == 0) {
        m_objs[OBJ_PLAYERS]->setFrameVisible(2.0f);
    } else if (m_sizeIndex == lastSize[m_playersPerMatch - 2]) {
        m_objs[OBJ_PLAYERS]->setFrameVisible(1.0f);
    } else {
        m_objs[OBJ_PLAYERS]->setFrameVisible(3.0f);
    }

    int size = sizes[m_playersPerMatch - 2][m_sizeIndex];
    if (size > 100) {
        switch (size) {
        case 0xA2:
        case 0xA3:
            m_objs[OBJ_ENTRANTS]->setFrameFlg(34.0f, 6);
            m_entrants = 0x10;
            break;
        case 0xA4:
        case 0xA5:
            m_objs[OBJ_ENTRANTS]->setFrameFlg(33.0f, 6);
            m_entrants = 0x10;
            break;
        case 0x142:
            m_objs[OBJ_ENTRANTS]->setFrameFlg(36.0f, 6);
            m_entrants = 0x20;
            break;
        case 0x144:
            m_objs[OBJ_ENTRANTS]->setFrameFlg(35.0f, 6);
            m_entrants = 0x20;
            break;
        }
    } else {
        m_objs[OBJ_ENTRANTS]->setFrameFlg((float)size, 6);
        m_entrants = size;
    }
    m_table->setTableData(m_playersPerMatch, m_sizeIndex);
    if (first) {
        pushbackPanel(2);
        m_msg->printIndex(0, 1);
        m_msg->printIndex(1, 0x13);
    }
}

// The texts of the screen of the rules and the start of the tournament.
void muTourRuleTask::drawSelectConf(bool first) {
    const int sizes[3][9] = {
        {4, 6, 8, 12, 16, 24, 32, 0, 0},
        {6, 9, 12, 15, 18, 21, 27, 30, 0},
        {8, 12, 16, 16, 20, 24, 28, 32, 32},
    };
    m_objs[OBJ_CONFIRM]->setFrameNode((float)m_cursor);
    m_msg->printIndex(0, m_cursor + 2);
    m_msg->setAlignMode(1, MuMsg::Align_Center);
    m_msg->printIndex(1, m_cursor + 0x14);
    if (!first) {
        return;
    }
    pushbackPanel(3);

    // "N fighters in a match"
    int length = 0;
    char buffer[0x80];
    memset(buffer, 0, sizeof(buffer));
    int players = m_playersPerMatch;
    if (players >= 10) {
        length = appendMsg(m_msg, buffer, 0, players / 10 + 0x2B);
    }
    length = appendMsg(m_msg, buffer, length, players % 10 + 0x2B);
    length = appendMsg(m_msg, buffer, length, 4);
    tourPrintData(m_msg, 2, buffer, length);

    // "N fighters take part"
    length = 0;
    char buffer2[0x80];
    memset(buffer2, 0, sizeof(buffer2));
    int size = sizes[m_playersPerMatch - 2][m_sizeIndex];
    if (size >= 10) {
        length = appendMsg(m_msg, buffer2, 0, size / 10 + 0x2B);
    }
    length = appendMsg(m_msg, buffer2, length, size % 10 + 0x2B);
    length = appendMsg(m_msg, buffer2, length, 5);
    tourPrintData(m_msg, 3, buffer2, length);

    // The rules of the matches: the time, or the number of stocks.
    length = 0;
    char rules[0x100];
    memset(rules, 0, sizeof(rules));
    gmSetRule* rule = g_GameGlobal->m_setRule;
    switch (rule->m_rule) {
    case 0:
        if (rule->m_timeMinutes >= 10) {
            length = appendMsg(m_msg, rules, 0, rule->m_timeMinutes / 10 + 0x2B);
        }
        length = appendMsg(m_msg, rules, length, rule->m_timeMinutes % 10 + 0x2B);
        length = appendMsg(m_msg, rules, length, 0x35);
        length = appendMsg(m_msg, rules, length, 6);
        break;
    case 1:
        if (rule->m_stockCount >= 10) {
            length = appendMsg(m_msg, rules, 0, rule->m_stockCount / 10 + 0x2B);
        }
        length = appendMsg(m_msg, rules, length, rule->m_stockCount % 10 + 0x2B);
        length = appendMsg(m_msg, rules, length, 0x36);
        length = appendMsg(m_msg, rules, length, 7);
        break;
    case 2:
        if (rule->m_timeMinutes >= 10) {
            length = appendMsg(m_msg, rules, 0, rule->m_timeMinutes / 10 + 0x2B);
        }
        length = appendMsg(m_msg, rules, length, rule->m_timeMinutes % 10 + 0x2B);
        length = appendMsg(m_msg, rules, length, 0x35);
        length = appendMsg(m_msg, rules, length, 8);
        break;
    }
    tourPrintData(m_msg, 4, rules, length);
    setWarning();
}

// Shows the slots of the fighters (the ones of the page), the cursor and the texts of the screen. The two flags say which part
// of the screen the cursor is on (the buttons that change the page, or the level of the CPU). When first is true the screen is
// set up as well.
void muTourRuleTask::drawSelectName(u8 onPaging, u8 onCpuLevel, bool first) {
    if (first) {
        m_objs[OBJ_PAGE_COUNT]->setFrameFlg((float)(((m_entrants - 1) >> 2) + 1), 3);
        tourClearGroup(m_mdls[0]);
        tourClearGroup(m_mdls[2]);
        tourAttach(m_root, reinterpret_cast<nw4r::g3d::ScnMdl*>(m_mdls[4]));
        tourClearGroup(m_mdls[6]);
        for (u32 i = 0; i < (u32)m_entrants; i++) {
            setPlayerWindow(i);
            char node[16];
            sprintf(node, "pos%02d", i);
            tourPushBack(m_mdls[6], reinterpret_cast<nw4r::g3d::ScnMdl*>(m_mdls[8 + i]), node);
        }
        m_msg->setAlignMode(1, MuMsg::Align_Left);
        m_msg->printf(0, "");
        m_objs[2]->setFrameVisible(2.0f);
        if (m_state >= State_ResumeOpen && m_state <= State_StageModeBack) {
            m_objs[2]->setAnimName(&sAnimMain[4], true);
        } else {
            m_objs[2]->setAnimName(&sAnimMain[2], true);
        }
        m_objs[OBJ_PAGING]->setAnimName(&sAnimPaging[0], true);
    }

    // The cursor on the slot (when the cursor is not on the buttons or the level of the CPU).
    m_mdls[6]->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[OBJ_SLOT_CURSOR]->m_scnMdl));
    if (!m_onPaging && !m_onCpuLevel) {
        char node[8];
        sprintf(node, "pos%02d", m_slotCursor);
        tourPushBack(m_mdls[6], m_objs[OBJ_SLOT_CURSOR]->m_scnMdl, node);
        m_objs[OBJ_SLOT_CURSOR]->setFrameVisible((float)m_cursor + 1.0f);
    }
    setPlayerWindow(m_slotCursor);

    // The costumes of the fighter of the slot.
    if (!m_onCpuLevel && !m_onPaging) {
        muTourSlot* slot = &m_slots[m_slotCursor];
        if (m_cursor == 1) {
            if (slot->m_character == 0x29 || slot->m_kind == 2) {
                m_colorShown = false;
                drawColorIcon(0x29, 0);
            } else {
                m_colorShown = true;
                drawColorIcon(slot->m_character, slot->m_color);
            }
            m_msg->printf(7, "");
        } else if (m_cursor == 0) {
            m_colorShown = true;
            m_msg->printIndex(7, 0x10);
            for (u32 i = 0; i < 9; i++) {
                m_mdls[4]->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[OBJ_COLOR_ICONS + i]->m_scnMdl));
            }
            m_mdls[4]->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[OBJ_COLOR_CURSOR]->m_scnMdl));
        } else if (m_cursor < 3) {
            m_colorShown = false;
            m_msg->printf(7, "");
            for (u32 i = 0; i < 9; i++) {
                m_mdls[4]->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[OBJ_COLOR_ICONS + i]->m_scnMdl));
            }
            m_mdls[4]->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[OBJ_COLOR_CURSOR]->m_scnMdl));
        }
    } else {
        m_colorShown = false;
        m_msg->printf(7, "");
        for (u32 i = 0; i < 9; i++) {
            m_mdls[4]->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[OBJ_COLOR_ICONS + i]->m_scnMdl));
        }
        m_mdls[4]->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[OBJ_COLOR_CURSOR]->m_scnMdl));
    }

    // The level of the CPU shows up when a CPU takes part.
    if (!m_hasCpu) {
        if (m_cpuAnim == 1) {
            m_cpuAnim = 2;
            m_objs[OBJ_CPU_LEVEL]->setAnimName(&sAnimCpuLevel[1], true);
        } else if (m_cpuAnim == 2 && m_objs[OBJ_CPU_LEVEL]->m_modelAnim->getUpdateRate() == 0.0f) {
            tourAttach(m_root, m_objs[OBJ_CPU_LEVEL]->m_scnMdl);
            m_cpuAnim = 0;
        }
    } else {
        if (m_cpuAnim != 1) {
            m_cpuAnim = 1;
            tourAttach(m_root, m_objs[OBJ_CPU_LEVEL]->m_scnMdl);
            m_objs[OBJ_CPU_LEVEL]->setAnimName(&sAnimCpuLevel[0], true);
        }
        if (m_onCpuLevel != onCpuLevel) {
            if (m_onCpuLevel == 0) {
                m_objs[OBJ_CPU_LEVEL]->changeClrAnimNIf("MenTrnRule0050_TopN__0");
            } else {
                m_objs[OBJ_CPU_LEVEL]->changeClrAnimNIf("MenTrnRule0050_TopN__2");
            }
        }
        float frame = 0.0f;
        if (m_cpuLevel != 0) {
            frame = 1.0f;
        }
        if (m_cpuLevel < 9) {
            frame += 2.0f;
        }
        m_objs[OBJ_CPU_LEVEL]->setFrameVisible(frame);
        m_msg->printIndex(8, 0x11);
        if (m_cpuLevel == 9) {
            m_msg->printf(9, "");
        } else {
            m_msg->printIndex(9, m_cpuLevel + 0x2C);
        }
        m_msgCpu->printIndex(0, getCpLevelMessageId__6muMenuFiii(m_cpuLevel));
    }

    // The buttons that change the page.
    if (m_onPaging != onPaging) {
        if (m_onPaging == 0) {
            m_objs[OBJ_PAGING]->changeClrAnimNIf("MenTrnRule0040_TopN__0");
        } else {
            m_objs[OBJ_PAGING]->changeClrAnimNIf("MenTrnRule0040_TopN__2");
        }
    }
    float pagingFrame = 0.0f;
    if (m_page != 0) {
        pagingFrame = 1.0f;
    }
    if ((u32)((m_page + 1) * 4 + 4) < (u32)m_entrants) {
        pagingFrame += 2.0f;
    }
    m_objs[OBJ_PAGING]->setFrameVisible(pagingFrame);

    // The help text.
    if (m_onPaging) {
        m_msg->printIndex(1, 0x1E);
    } else if (!m_onCpuLevel) {
        switch (m_cursor) {
        case 0:
            m_msg->printIndex(1, 0x16);
            break;
        case 1:
            m_msg->printIndex(1, 0x17);
            break;
        case 2:
            m_msg->printIndex(1, 0x18);
            break;
        }
    } else {
        m_msg->printIndex(1, 0x19);
    }
}

// The slots of the fighters get their start values: the controllers that are plugged in come first as players, the other
// slots are CPUs (when there are less controllers than fighters in a match), or all of them are players.
void muTourRuleTask::initPlayerData() {
    u32 pads = countPads();
    if (pads < (u32)m_playersPerMatch) {
        m_hasCpu = true;
        for (u8 i = 0; i < pads; i++) {
            muTourSlot* slot = &m_slots[i];
            slot->m_kind = 0;
            slot->m_nameTag = 0x78;
            slot->m_index = i;
            slot->m_character = 0x29;
            slot->m_color = 0;
            slot->m_handicap = 0;
            slot->m_named = 0;
            makeSlotName(m_msg, slot, 0xC, i + 1);
        }
        u8 number = 1;
        for (u32 i = pads & 0xFF; i < (u32)m_entrants; i++) {
            muTourSlot* slot = &m_slots[i];
            slot->m_kind = 1;
            slot->m_nameTag = 0x78;
            slot->m_index = number - 1;
            slot->m_character = 0x29;
            slot->m_color = 0;
            slot->m_handicap = 0;
            slot->m_named = 0;
            makeSlotName(m_msg, slot, 0xD, number);
            number++;
        }
    } else {
        m_hasCpu = false;
        for (u8 i = 0; i < (u32)m_entrants; i++) {
            muTourSlot* slot = &m_slots[i];
            slot->m_kind = 0;
            slot->m_nameTag = 0x78;
            slot->m_index = i;
            slot->m_character = 0x29;
            slot->m_color = 0;
            slot->m_handicap = 0;
            slot->m_named = 0;
            makeSlotName(m_msg, slot, 0xC, i + 1);
        }
    }
}

// Shows a slot: the name (in the color of its kind), the face and the stock icon of the fighter, the color of the frame and the
// handicap.
void muTourRuleTask::setPlayerWindow(int slot) {
    muTourSlot* s = &m_slots[slot];
    switch (s->m_kind) {
    case 1:
        m_objs[OBJ_SLOT_FRAME + slot]->setFrameVisible(0.0f);
        m_msgSlot->setFontColor(slot + 5, 0x6E, 0x76, 0x7C, 0xFF);
        tourPrintData(m_msgSlot, slot + 5, s->m_name, strlen(s->m_name));
        break;
    case 0:
        m_objs[OBJ_SLOT_FRAME + slot]->setFrameVisible(0.0f);
        m_msgSlot->setFontColor(slot + 5, 0, 0, 0, 0xFF);
        tourPrintData(m_msgSlot, slot + 5, s->m_name, strlen(s->m_name));
        break;
    case 2:
        m_objs[OBJ_SLOT_FRAME + slot]->setFrameVisible(1.0f);
        tourPrintData(m_msgSlot, slot + 5, "", 0);
        break;
    }

    MuObject* face = m_objs[OBJ_SLOT_FACE + slot];
    if (s->m_character < 0x28) {
        face->setFrameTex((float)getStockIconBase(s->m_character));
    } else {
        face->setFrameTex(501.0f);
    }
    setStockIcon(m_objs[OBJ_SLOT_ICON + slot], s->m_character, s->m_color, &m_resStockFace);
    if (s->m_character == 0x29 || s->m_kind == 2) {
        m_objs[OBJ_SLOT_FRAME + slot]->setFrameMatCol(20.0f);
    } else {
        m_objs[OBJ_SLOT_FRAME + slot]->setFrameMatCol((float)(u8)getCharColor__6muMenuFiii(s->m_character, s->m_color));
    }
    m_objs[OBJ_SLOT_HANDICAP + slot]->setFrameTex((float)(s->m_handicap / 10));
}

// The list of the handicaps (five lines, in tens of percent), with the line of the cursor dark.
void muTourRuleTask::drawHandiWindow() {
    u32 value = m_handicapTop * 10;
    u32 selected = m_handicap - m_handicapTop;
    for (u32 row = 0; (int)row < 5; row++, value += 10) {
        char buffer[0x40];
        memset(buffer, 0, sizeof(buffer));
        int length = 0;
        if (value > 99) {
            length = appendMsg(m_msg, buffer, 0, value / 100 + 0x2B);
        }
        if (value > 9) {
            length = appendMsg(m_msg, buffer, length, (value % 100) / 10 + 0x2B);
        }
        length = appendMsg(m_msg, buffer, length, value % 10 + 0x2B);
        length = appendMsg(m_msg, buffer, length, 0x37);
        if (selected == row) {
            m_msgSlot->setFontColor(row, 0x19, 0x19, 0x19, 0xFF);
        } else {
            m_msgSlot->setFontColor(row, 0x80, 0x80, 0x80, 0xFF);
        }
        tourPrintData(m_msgSlot, row, buffer, length);
    }
    m_objs[OBJ_HANDICAP_LIST]->setFrameNode((float)selected);
    if (m_handicap == 0) {
        m_objs[OBJ_HANDICAP_LIST]->setFrameVisible(1.0f);
    } else if (m_handicap == 0x1E) {
        m_objs[OBJ_HANDICAP_LIST]->setFrameVisible(2.0f);
    } else {
        m_objs[OBJ_HANDICAP_LIST]->setFrameVisible(3.0f);
    }
}

// Shows the fighter that is on in the picker: the cursor, the face, the stock icon, the costumes and the name.
void muTourRuleTask::drawSelectChar() {
    m_mdls[7]->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[OBJ_CHAR_CURSOR]->m_scnMdl));
    char node[8];
    if ((u32)m_charGridCursor < (u32)m_charColumns) {
        sprintf(node, "pos%02d", m_charGridCursor << 1);
    } else {
        sprintf(node, "pos%02d", (m_charGridCursor - m_charColumns) * 2 + 1);
    }
    tourPushBack(m_mdls[7], m_objs[OBJ_CHAR_CURSOR]->m_scnMdl, node);

    if (m_selChar == 0x29) {
        m_objs[OBJ_SLOT_FRAME + m_slotCursor]->setFrameMatCol(20.0f);
        m_colorShown = false;
    } else {
        u32 color = getCharColor__6muMenuFiii(m_selChar, m_selColor);
        m_objs[OBJ_SLOT_FRAME + m_slotCursor]->setFrameMatCol((float)(color & 0xFF));
        m_colorShown = true;
    }
    MuObject* face = m_objs[OBJ_SLOT_FACE + m_slotCursor];
    if (m_selChar < 0x28) {
        face->setFrameTex((float)getStockIconBase(m_selChar));
    } else {
        face->setFrameTex(501.0f);
    }
    setStockIcon(m_objs[OBJ_SLOT_ICON + m_slotCursor], m_selChar, m_selColor, &m_resStockFace);
    drawColorIcon(m_selChar, m_selColor);

    // "<name of the fighter>"
    char text[0x100];
    memset(text, 0, sizeof(text));
    int length = appendMsg(m_msg, text, 0, 0x1B);
    if (m_selChar == 0x29) {
        length = appendMsg(m_msg, text, length, 0xE);
    } else {
        int stock = muMenu::exchangeMuSelchkind2MuStockchkind(m_selChar);
        length = sprintf(text, "%s%s", text, exchangeMuStockchkind2MuCharName__6muMenuFi(stock));
    }
    length = appendMsg(m_msg, text, length, 0x1C);
    tourPrintData(m_msg, 1, text, length);
}

// Shows the costumes of a fighter in a row of icons, and the cursor on the costume that is on.
void muTourRuleTask::drawColorIcon(int character, u8 color) {
    for (u32 i = 0; i < 9; i++) {
        m_mdls[4]->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[OBJ_COLOR_ICONS + i]->m_scnMdl));
    }
    m_mdls[4]->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[OBJ_COLOR_CURSOR]->m_scnMdl));
    if (character == 0x29) {
        return;
    }
    if (character == 0x15) {
        // One of the three trainers of the Pokemon Trainer has six costumes, the others have the other ones
        u32 first = 0;
        u32 last = 6;
        if (color >= 6) {
            first = 6;
            last = getNumCharColor__6muMenuFiii(0x15);
        }
        u32 index = 0;
        for (u32 costume = first; (u8)costume < (u8)last; costume++, index++) {
            MuObject* icon = m_objs[OBJ_COLOR_ICONS + (index & 0xFF)];
            char name[64];
            sprintf(name, "InfStc.%03d", getStockIconBase(0x15) + (costume & 0xFF));
            icon->changeMaterialTex(icon->m_resMdl.GetResMat("lambert87")->m_id, name, &m_resStockFace);
            char node[8];
            sprintf(node, "pos%02d", (index & 0xFF) + 2);
            tourPushBack(m_mdls[4], icon->m_scnMdl, node);
            if ((costume & 0xFF) == color) {
                tourPushBack(m_mdls[4], m_objs[OBJ_COLOR_CURSOR]->m_scnMdl, node);
            }
        }
    } else {
        for (u32 costume = 0; (int)(costume & 0xFF) < getNumCharColor__6muMenuFiii(character); costume++) {
            MuObject* icon = m_objs[OBJ_COLOR_ICONS + (costume & 0xFF)];
            setStockIcon(icon, character, costume & 0xFF, &m_resStockFace);
            char node[8];
            sprintf(node, "pos%02d", (costume & 0xFF) + 2);
            tourPushBack(m_mdls[4], icon->m_scnMdl, node);
            if ((costume & 0xFF) == color) {
                tourPushBack(m_mdls[4], m_objs[OBJ_COLOR_CURSOR]->m_scnMdl, node);
            }
        }
    }
}

// Some fighters have another one that is picked with the same button (Zelda and Sheik, and so on): the buttons Z or R change
// the fighter to the other one of the pair. Returns the fighter that the buttons ask for.
int muTourRuleTask::charSelOption(int character) {
    u32 buttons = *reinterpret_cast<u32*>(reinterpret_cast<char*>(&m_ctrl) + 0x1C);
    for (int i = 0; i < 8; i++) {
        gfPadStatus status;
        g_gfPadSystem->getSysPadStatus(i, &status);
        if (status.m_error >= 0) {
            if (status.m_controllerType == gfPadType::NUNCHUK) {
                if (status.m_buttonsPressedThisFrame.bits & 0x40000) {
                    buttons |= 2;
                }
            } else if (status.m_controllerType < 3 && status.m_controllerType > 1) {
                if (status.m_buttonsPressedThisFrame.bits & 0x80000) {
                    buttons |= 2;
                }
            }
        }
    }
    bool previous;
    if ((buttons & 1) == 0 && (buttons & 4) == 0) {
        if ((buttons & 2) == 0 && (buttons & 8) == 0) {
            return character;
        }
        previous = true;
    } else {
        previous = false;
    }
    switch (character) {
    case 3:
        character = 4;
        break;
    case 4:
        character = 3;
        break;
    case 0xE:
        character = 0xF;
        break;
    case 0xF:
        character = 0xE;
        break;
    case 0x1B:
        character = 0x1E;
        if (previous) {
            character = 0x1C;
        }
        break;
    case 0x1C:
        character = 0x1B;
        if (previous) {
            character = 0x1D;
        }
        break;
    case 0x1D:
        character = 0x1C;
        if (previous) {
            character = 0x1E;
        }
        break;
    case 0x1E:
        character = 0x1D;
        if (previous) {
            character = 0x1B;
        }
        break;
    }
    return character;
}

// Tells that there are less controllers than the fighters in a match need ("... controllers are needed").
void muTourRuleTask::setWarning() {
    char text[0x100];
    memset(text, 0, sizeof(text));
    int length = appendMsg(m_msg, text, 0, 9);
    text[length] = '\n';
    length++;
    length = appendMsg(m_msg, text, length, 10);
    length = appendMsg(m_msg, text, length, m_playersPerMatch + 0x2B);
    length = appendMsg(m_msg, text, length, 0xB);
    tourPrintData(m_msg, 5, text, length);
}

// The last screen: the table with the names, the rules, and the way the stages are picked.
void muTourRuleTask::drawAllConf() {
    tourDetach(m_root, reinterpret_cast<nw4r::g3d::ScnMdl*>(m_mdls[0]));
    m_mdls[2]->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_table->m_group));
    tourPushBack(m_mdls[3], reinterpret_cast<nw4r::g3d::ScnMdl*>(m_table->m_group), "pos00");
    tourAttach(m_root, reinterpret_cast<nw4r::g3d::ScnMdl*>(m_mdls[3]));
    tourAttach(m_root, m_objs[OBJ_RULE_CONFIRM]->m_scnMdl);
    m_table->setName();
    m_objs[OBJ_RULE_CONFIRM]->setAnimName(&sAnimConfirm[0], true);
    m_objs[OBJ_STAGE_ICON]->setAnimName(&sAnimStageMode[0], true);
    m_objs[OBJ_RULE_CONFIRM]->changeClrAnimNIf(m_confirmSel == 0 ? "MenTrnRule0044_TopN__0" : "MenTrnRule0044_TopN__1");

    // The rules of the matches
    int length = 0;
    char text[0x40];
    memset(text, 0, sizeof(text));
    gmSetRule* rule = g_GameGlobal->m_setRule;
    if (rule->m_rule == 1) {
        m_msg->printIndex(0xE, 7);
        if (rule->m_stockCount > 9) {
            length = appendMsg(m_msg, text, 0, rule->m_stockCount / 10 + 0x2B);
        }
        length = appendMsg(m_msg, text, length, rule->m_stockCount % 10 + 0x2B);
        length = appendMsg(m_msg, text, length, 0x36);
    } else if (rule->m_rule == 0) {
        m_msg->printIndex(0xE, 6);
        if (rule->m_timeMinutes > 9) {
            length = appendMsg(m_msg, text, 0, rule->m_timeMinutes / 10 + 0x2B);
        }
        length = appendMsg(m_msg, text, length, rule->m_timeMinutes % 10 + 0x2B);
        length = appendMsg(m_msg, text, length, 0x35);
    } else if (rule->m_rule < 3) {
        m_msg->printIndex(0xE, 8);
        if (rule->m_timeMinutes > 9) {
            length = appendMsg(m_msg, text, 0, rule->m_timeMinutes / 10 + 0x2B);
        }
        length = appendMsg(m_msg, text, length, rule->m_timeMinutes % 10 + 0x2B);
        length = appendMsg(m_msg, text, length, 0x35);
    }
    tourPrintData(m_msg, 0xF, text, length);
    m_msg->printIndex(0x10, 0x27);

    // The way the stages are picked
    muTourData* data = getTourData();
    MuObject* icon = m_objs[OBJ_STAGE_ICON];
    int matId;
    if (data->m_stageMode == 1) {
        m_msg->printIndex(0x11, 0x29);
        matId = icon->m_resMdl.GetResMat("iconM")->m_id;
        icon->changeMaterialTex(matId, "MenSelmapIcon.60", NULL);
    } else if (data->m_stageMode == 0) {
        m_msg->printIndex(0x11, 0x28);
        matId = icon->m_resMdl.GetResMat("iconM")->m_id;
        icon->changeMaterialTex(matId, "MenSelmapIcon.61", NULL);
    } else if (data->m_stageMode < 3) {
        int frame;
        if (data->m_stageFlag == 1) {
            frame = 0x3E;
        } else {
            int stageKind = exchangeSrStageKindToMuStageKind__6muMenuFiii(*reinterpret_cast<u16*>(reinterpret_cast<char*>(g_GameGlobal->m_selStageData) + 0x22));
            m_msg->printIndex(0x11, stageKind, m_files[3].getBuffer());
            frame = (int)getStageFrameNo__6muMenuFiii(stageKind);
        }
        char name[32];
        sprintf(name, "MenSelmapIcon.%02d", frame);
        matId = icon->m_resMdl.GetResMat("iconM")->m_id;
        icon->changeMaterialTex(matId, name, NULL);
    }
    m_msg->printIndex(0x12, 0x2A);
}

// Reads the settings of the tournament from the data of the game (the fighters, the numbers and the rules).
void muTourRuleTask::setGlobalData() {
    muTourData* data = getTourData();
    m_entrants = data->m_count;
    m_playersPerMatch = data->m_playersPerMatch;
    m_sizeIndex = data->m_sizeIndex;
    m_stageMode = data->m_stageMode;
    m_cpuLevel = data->m_cpuLevel;
    m_hasCpu = false;
    for (u32 i = 0; i < (u32)m_entrants; i++) {
        muTourEntrant* entrant = &data->m_entrants[i];
        muTourSlot* slot = &m_slots[i];
        switch (entrant->m_kind) {
        case 1:
            slot->m_kind = 1;
            m_hasCpu = true;
            break;
        case 0:
            slot->m_kind = 0;
            break;
        case 2:
            slot->m_kind = 2;
            break;
        }
        sprintf(slot->m_name, "%s", entrant->m_name);
        slot->m_nameTag = entrant->m_nameTag;
        slot->m_index = entrant->m_index;
        slot->m_named = entrant->m_named;
        slot->m_character = entrant->m_character;
        slot->m_color = entrant->m_color;
        slot->m_handicap = entrant->m_handicap;
    }
}

// The button that shows the pad that picked the slot: its picture follows the kind of the controller.
void muTourRuleTask::setEntryButton() {
    int pad = m_ctrlPad;
    bool shown = m_colorShown;
    gfPadStatus status;
    if (pad < 0 || pad > 7) {
        shown = false;
    } else {
        g_gfPadSystem->getSysPadStatus(pad, &status);
    }
    if (shown == true) {
        int type = status.m_controllerType;
        if (type == 2) {
            m_objs[OBJ_PAGING]->setFrameTex(2.0f);
        } else if (type < 2) {
            if (type == 0) {
                m_objs[OBJ_PAGING]->setFrameTex(0.0f);
            } else if (type >= 0) {
                m_objs[OBJ_PAGING]->setFrameTex(3.0f);
            } else {
                m_objs[OBJ_PAGING]->setFrameTex(4.0f);
            }
        } else if (type < 4) {
            m_objs[OBJ_PAGING]->setFrameTex(1.0f);
        } else {
            m_objs[OBJ_PAGING]->setFrameTex(4.0f);
        }
    } else {
        m_objs[OBJ_PAGING]->setFrameTex(4.0f);
    }
}

// Writes the settings of the tournament in the data of the game: the fighters get their place in the table at random, and a
// fighter that was left to chance gets one now (not one that an earlier fighter already has).
void muTourRuleTask::setupTournament() {
    u32 order[0x20];
    for (int i = 0; i < 0x20; i++) {
        order[i] = (u32)-1;
    }
    for (u32 i = 0; i < (u32)m_entrants; i++) {
        int place;
        do {
            place = randi(m_entrants);
        } while (order[place] != (u32)-1);
        order[place] = i;
    }

    muTourData* data = getTourData();
    for (u32 i = 0; i < (u32)m_entrants; i++) {
        muTourSlot* slot = &m_slots[i];
        muTourEntrant* entrant = &data->m_entrants[i];
        entrant->m_pickedCharacter = slot->m_character;
        switch (slot->m_kind) {
        case 1:
            entrant->m_kind = 1;
            if (m_cpuLevel == 9) {
                entrant->m_cpuLevel = randi(9);
            } else {
                entrant->m_cpuLevel = m_cpuLevel;
            }
            break;
        case 0:
            entrant->m_kind = 0;
            break;
        case 2:
            entrant->m_kind = 2;
            break;
        }
        sprintf(entrant->m_name, "%s", slot->m_name);
        entrant->m_nameTag = slot->m_nameTag;
        entrant->m_index = slot->m_index;
        entrant->m_named = slot->m_named;
        if (slot->m_character == 0x29) {
            int tries = 0;
            bool again = true;
            while (again) {
                again = false;
                int kind = m_charList[randi(m_charCount - 1)];
                slot->m_character = exchangeMuCharKindToMuSelchkind__6muMenuFiii(kind);
                slot->m_color = randi(getNumCharColor__6muMenuFiii(slot->m_character));
                bool giveUp = tries > 299;
                tries++;
                if (giveUp) {
                    break;
                }
                for (u32 j = 0; j < i; j++) {
                    if (m_slots[j].m_character == slot->m_character && m_slots[j].m_color == slot->m_color) {
                        again = true;
                        break;
                    }
                }
            }
        }
        entrant->m_character = slot->m_character;
        entrant->m_color = slot->m_color;
        entrant->m_handicap = slot->m_handicap;
        entrant->m_slot = order[i];
    }
}

// Makes the models of the menu, puts the texts on them and gets the groups of the scene ready.
void muTourRuleTask::initMdlData() {
    // The number of the model of each object (MenTrnRule<number>_TopN), the draw priorities of the later ones and the colors
    // of the texts of the screen.
    const int modelNo[19] = {0, 1, 10, 11, 30, 31, 32, 33, 41, 42, 40, 50, 51, 53, 54, 55, 57, 58, 56};
    const int priority[9] = {0x1D, 0x12, 0x13, 0x1F, 0x11, 0x1F, 0x12, 0x1F, 0x1C};
    const u8 textColor[19][3] = {
        {200, 200, 200}, {232, 232, 232}, {10, 80, 160}, {10, 80, 160}, {0, 0, 0}, {255, 255, 255}, {95, 108, 168},
        {95, 108, 168}, {255, 255, 255}, {255, 255, 255}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0},
        {10, 80, 160}, {10, 80, 160}, {10, 80, 160}, {10, 80, 160}, {10, 80, 160},
    };
    // The objects that are the base of every group (see below) and how many models the group takes.
    const int groupCapacity[8] = {6, 1, 1, 1, 12, 1, 0x21, 0x26};
    const int groupBase[40] = {1, 4, 3, 0xC2, 0xA, 8, 0xC, 0xE};

    m_objs[0] = MuObject::create(&m_resRuleA, "MenTrnRule0000_TopN", 0x10, NULL, Heaps::MenuInstance);
    m_objs[1] = MuObject::create(&m_resRuleA, "MenTrnRule0001_TopN", 0x11, NULL, Heaps::MenuInstance);
    for (int i = 2; i < 8; i++) {
        char name[16];
        sprintf(name, "MenTrnRule%04d_TopN", modelNo[i]);
        m_objs[i] = MuObject::create(&m_resRuleA, name, 0x12, NULL, Heaps::MenuInstance);
    }
    nw4r::g3d::ScnMdl* warning = m_objs[OBJ_WARNING]->m_scnMdl;
    warning->SetPriorityDrawOpa(0x20);
    warning->SetPriorityDrawXlu(0x20);
    m_objs[OBJ_HANDICAP_WND] = MuObject::create(&m_resC, "MenTrnRule0041_TopN", 0x20, NULL, Heaps::MenuInstance);
    m_objs[OBJ_HANDICAP_LIST] = MuObject::create(&m_resC, "MenTrnRule0042_TopN", 0x21, NULL, Heaps::MenuInstance);
    for (int i = 10; i < 19; i++) {
        char name[16];
        sprintf(name, "MenTrnRule%04d_TopN", modelNo[i]);
        m_objs[i] = MuObject::create(&m_resRuleA, name, priority[i - 10], NULL, Heaps::MenuInstance);
    }
    for (int i = 0; i < 0x20; i++) {
        m_objs[OBJ_SLOT_FRAME + i] = MuObject::create(&m_resRuleA, "MenTrnRule0052_TopN", (i % 2) * 5 + 0x15, NULL, Heaps::MenuInstance);
        m_objs[OBJ_SLOT_HANDICAP + i] = MuObject::create(&m_resRuleA, "MenTrnRule0059_TopN", 0x1B, NULL, Heaps::MenuInstance);
        m_objs[OBJ_SLOT_FACE + i] = MuObject::create(&m_resRuleA, "MenTrnFace0000_TopN", 0x1B, &m_resCharaFace, Heaps::MenuInstance);
        m_objs[OBJ_SLOT_ICON + i] = MuObject::create(&m_resRuleA, "MenTrnFace0001_TopN", 0x1C, &m_resStockFace, Heaps::MenuInstance);
    }

    // The faces of the fighters in the picker
    for (u32 i = 0; i < (u32)m_charCount; i++) {
        MuObject* face = MuObject::create(&m_resRuleA, "MenTrnFace0000_TopN", 0x15, &m_resCharaFace, Heaps::MenuInstance);
        m_objs[OBJ_CHAR_FACES + i] = face;
        face->changeTexPatAnimNIf("MenTrnFace0000_TopN__0");
        face->setUpdateRateFlg(0.0f, 4);
        int selchk = exchangeMuCharKindToMuSelchkind__6muMenuFiii(m_charList[i]);
        if (selchk < 0x28) {
            face->setFrameTex((float)getStockIconBase(selchk));
        } else {
            face->setFrameTex(501.0f);
        }
    }
    for (int i = 0; i < 9; i++) {
        m_objs[OBJ_COLOR_ICONS + i] = MuObject::create(&m_resRuleA, "MenTrnFace0001_TopN", 0x1E, &m_resStockFace, Heaps::MenuInstance);
    }
    m_objs[OBJ_OPTION] = MuObject::create(&m_resRuleB, "MenTrnOption0000_TopN", 0x10, NULL, Heaps::MenuInstance);
    m_objs[OBJ_RULE_CONFIRM] = MuObject::create(&m_resRuleA, "MenTrnRule0044_TopN", 0x20, NULL, Heaps::MenuInstance);
    m_objs[OBJ_STAGE_ICON] = MuObject::create(&m_resRuleA, "MenTrnRule0070_TopN", 0x14, NULL, Heaps::MenuInstance);

    // The texts of the models
    m_msg->setMsgData(m_files[2].getBuffer());
    m_msg->attachScnMdlSimple(0, m_objs[1]->m_sceneModel, 0, 1.0f / 16.0f);
    m_msg->attachScnMdlSimple(1, m_objs[1]->m_sceneModel, 1, 1.0f / 16.0f);
    m_msg->attachScnMdlSimple(2, m_objs[3]->m_sceneModel, 0, 1.0f / 16.0f);
    m_msg->attachScnMdlSimple(3, m_objs[3]->m_sceneModel, 1, 1.0f / 16.0f);
    m_msg->attachScnMdlSimple(4, m_objs[6]->m_sceneModel, 0, 1.0f / 16.0f);
    m_msg->attachScnMdlSimple(5, m_objs[7]->m_sceneModel, 0, 1.0f / 16.0f);
    m_msg->attachScnMdlSimple(6, m_objs[OBJ_PAGING]->m_sceneModel, 0, 1.0f / 18.0f);
    m_msg->attachScnMdlSimple(7, m_objs[OBJ_PAGING]->m_sceneModel, 1, 1.0f / 18.0f);
    m_msg->attachScnMdlSimple(8, m_objs[OBJ_CPU_LEVEL]->m_sceneModel, 0, 1.0f / 18.0f);
    m_msg->attachScnMdlSimple(9, m_objs[OBJ_CPU_LEVEL]->m_sceneModel, 1, 1.0f / 18.0f);
    for (u32 i = 0; i < 4; i++) {
        m_msg->attachScnMdlSimple(i + 10, m_objs[OBJ_STAGE_MODE]->m_sceneModel, i, 1.0f / 16.0f);
        m_msg->setAlignMode(i + 10, MuMsg::Align_Center);
        m_msg->setFontWidthModeAuto(i + 10);
        m_msg->setFontColor(i + 10, 0, 0, 0, 0xFF);
        m_msg->printIndex(i + 10, i + 0x23);
        m_msg->attachScnMdlSimple(i + 14, m_objs[OBJ_STAGE_ICON]->m_sceneModel, i, 1.0f / 16.0f);
        m_msg->setAlignMode(i + 14, MuMsg::Align_Center);
        m_msg->setFontWidthModeAuto(i + 14);
    }
    if (fn_8004E834(g_GameGlobal) == 0) {
        m_msg->printf(13, "");
    }
    m_msg->attachScnMdlSimple(0x12, m_objs[OBJ_RULE_CONFIRM]->m_sceneModel, 0, 1.0f / 16.0f);
    m_msg->setAlignMode(0x12, MuMsg::Align_Left);
    for (u32 i = 0; i < 0x13; i++) {
        m_msg->setAlignMode(i, MuMsg::Align_Center);
        m_msg->setFontWidthModeAuto(i);
        m_msg->setFontColor(i, textColor[i][0], textColor[i][1], textColor[i][2], 0xFF);
    }
    m_msg->getWScale(5, 50.0f);
    m_msg->setFontWidthModeProportional(5);

    // The texts of the slots (the names, then the handicaps), and the text of the level of the CPU
    m_msgSlot->setMsgData(*reinterpret_cast<void**>(reinterpret_cast<char*>(m_msg) + 4));
    for (u32 i = 0; i < 5; i++) {
        m_msgSlot->attachScnMdlSimple(i, m_objs[OBJ_HANDICAP_LIST]->m_sceneModel, i, 1.0f / 16.0f);
        m_msgSlot->setAlignMode(i, MuMsg::Align_Center);
        m_msgSlot->setFontWidthModeAuto(i);
        m_msgSlot->setFontType(i, 1);
    }
    for (int i = 0; i < 0x20; i++) {
        m_msgSlot->attachScnMdlSimple(i + 5, m_objs[OBJ_SLOT_FRAME + i]->m_sceneModel, 0, 1.0f / 16.0f);
        m_msgSlot->setAlignMode(i + 5, MuMsg::Align_Center);
        m_msgSlot->setFontWidthModeAuto(i + 5);
        m_msgSlot->setFontColor(i + 5, 0, 0, 0, 0xFF);
        m_msgSlot->setFontType(i + 5, 1);
    }
    m_msgCpu->setMsgData(m_files[1].getBuffer());
    m_msgCpu->attachScnMdlSimple(0, m_objs[OBJ_CPU_LEVEL]->m_sceneModel, 2, 1.0f / 18.0f);
    m_msgCpu->setAlignMode(0, MuMsg::Align_Center);
    m_msgCpu->setFontWidthModeAuto(0);
    m_msgCpu->setFontColor(0, 0xFF, 0xFF, 0xFF, 0xFF);

    // The balloon of the help
    muTourPanelCtrlTask* panel = new (Heaps::MenuInstance) muTourPanelCtrlTask(m_objs[OBJ_OPTION], NULL);
    panel->init();
    m_panel = panel;

    // The groups of the models: each takes the models that are shown together.
    MEMAllocator* allocator = gfHeapManager::getMEMAllocator(Heaps::MenuInstance);
    for (int i = 0; i < 8; i++) {
        u32 size;
        m_mdls[i] = (nw4r::g3d::ScnGroup*)Construct__Q34nw4r3g3d12ScnMdlExpandFP12MEMAllocatorPUliPQ34nw4r3g3d6ScnMdl(
            allocator, &size, groupCapacity[i], m_objs[groupBase[i]]->m_scnMdl);
    }
    for (int i = 0; i < 0x20; i++) {
        u32 size;
        m_mdls[8 + i] = (nw4r::g3d::ScnGroup*)Construct__Q34nw4r3g3d12ScnMdlExpandFP12MEMAllocatorPUliPQ34nw4r3g3d6ScnMdl(
            allocator, &size, 4, m_objs[OBJ_SLOT_FRAME + i]->m_scnMdl);
    }
    tourPushBack(m_mdls[1], m_objs[5]->m_scnMdl, "pos00");
    tourPushBack(m_mdls[3], m_objs[9]->m_scnMdl, "pos00");
    tourAttach(m_root, m_objs[0]->m_scnMdl);
    tourAttach(m_root, m_objs[7]->m_scnMdl);

    // The start state of the animations of the models
    m_objs[0]->changeNodeAnimNIf("MenTrnRule0000_TopN__0");
    m_objs[1]->setAnimName(&sAnimMain[7], false);
    m_objs[1]->m_modelAnim->m_anmObjVisRes->SetUpdateRate(0.0f);
    m_objs[1]->setFrameVisible(0.0f);
    m_objs[1]->setAnimName(&sAnimMain[0], false);
    m_objs[4]->changeVisAnimNIf("MenTrnRule0030_TopN__0");
    m_objs[4]->m_modelAnim->m_anmObjVisRes->SetUpdateRate(0.0f);
    m_objs[4]->changeClrAnimNIf("MenTrnRule0030_TopN__0");
    m_objs[5]->changeVisAnimNIf("MenTrnRule0031_TopN__0");
    m_objs[5]->changeTexPatAnimNIf("MenTrnRule0031_TopN__0");
    m_objs[5]->setUpdateRateFlg(0.0f, 6);
    m_objs[6]->changeNodeAnimNIf("MenTrnRule0032_TopN__0");
    m_objs[6]->m_modelAnim->m_anmObjChrRes->SetUpdateRate(0.0f);
    m_objs[6]->changeClrAnimNIf("MenTrnRule0032_TopN__0");
    m_objs[OBJ_HANDICAP_WND]->changeVisAnimNIf("MenTrnRule0041_TopN__0");
    m_objs[OBJ_HANDICAP_WND]->m_modelAnim->m_anmObjVisRes->SetUpdateRate(0.0f);
    m_objs[OBJ_HANDICAP_WND]->setFrameVisible(1.0f);
    m_objs[OBJ_HANDICAP_LIST]->changeNodeAnimNIf("MenTrnRule0042_TopN__0");
    m_objs[OBJ_HANDICAP_LIST]->changeVisAnimNIf("MenTrnRule0042_TopN__0");
    m_objs[OBJ_HANDICAP_LIST]->changeClrAnimNIf("MenTrnRule0042_TopN__0");
    m_objs[OBJ_HANDICAP_LIST]->setUpdateRateFlg(0.0f, 3);
    m_objs[OBJ_PAGING]->changeVisAnimNIf("MenTrnRule0040_TopN__0");
    m_objs[OBJ_PAGING]->m_modelAnim->m_anmObjVisRes->SetUpdateRate(0.0f);
    m_objs[OBJ_PAGING]->changeAnimN(0, 4, false, false);
    m_objs[OBJ_CPU_LEVEL]->changeVisAnimNIf("MenTrnRule0050_TopN__0");
    m_objs[OBJ_CPU_LEVEL]->m_modelAnim->m_anmObjVisRes->SetUpdateRate(0.0f);
    m_objs[OBJ_PAGE_COUNT]->changeNodeAnimNIf("MenTrnRule0051_TopN__0");
    m_objs[OBJ_PAGE_COUNT]->changeVisAnimNIf("MenTrnRule0051_TopN__0");
    m_objs[OBJ_PAGE_COUNT]->setUpdateRateFlg(0.0f, 3);
    m_objs[OBJ_SLOT_CURSOR]->changeClrAnimNIf("MenTrnRule0053_TopN__0");
    m_objs[OBJ_SLOT_CURSOR]->changeVisAnimNIf("MenTrnRule0053_TopN__0");
    m_objs[OBJ_SLOT_CURSOR]->m_modelAnim->m_anmObjVisRes->SetUpdateRate(0.0f);
    m_objs[OBJ_SLOT_CURSOR]->m_scnMdl->SetPriorityDrawOpa(0x1F);
    m_objs[OBJ_SLOT_CURSOR]->m_scnMdl->SetPriorityDrawXlu(0x1F);

    for (int i = 0; i < 0x20; i++) {
        m_objs[OBJ_SLOT_FACE + i]->changeTexPatAnimNIf("MenTrnFace0000_TopN__0");
        m_objs[OBJ_SLOT_FACE + i]->setUpdateRateFlg(0.0f, 4);
        m_objs[OBJ_SLOT_FRAME + i]->changeTexPatAnimNIf("MenTrnRule0052_TopN__0");
        m_objs[OBJ_SLOT_FRAME + i]->changeVisAnimNIf("MenTrnRule0052_TopN__0");
        m_objs[OBJ_SLOT_FRAME + i]->changeClrAnimNIf("MenTrnRule0052_TopN__0");
        m_objs[OBJ_SLOT_FRAME + i]->setUpdateRateFlg(0.0f, 0x16);
        m_objs[OBJ_SLOT_FRAME + i]->setFrameTex((float)(i + 1));
        m_objs[OBJ_SLOT_HANDICAP + i]->changeTexPatAnimNIf("MenTrnRule0059_TopN__0");
        m_objs[OBJ_SLOT_HANDICAP + i]->m_modelAnim->m_anmObjTexPatRes->SetUpdateRate(0.0f);
        nw4r::g3d::ScnMdl* face = m_objs[OBJ_SLOT_FACE + i]->m_scnMdl;
        nw4r::g3d::ScnMdl* icon = m_objs[OBJ_SLOT_ICON + i]->m_scnMdl;
        nw4r::g3d::ScnMdl* handicap = m_objs[OBJ_SLOT_HANDICAP + i]->m_scnMdl;
        nw4r::g3d::ScnGroup* group = m_mdls[8 + i];
        group->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(face));
        group->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(icon));
        group->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(handicap));
        if (m_slots[i].m_kind < 2 && m_slots[i].m_kind >= 0) {
            tourPushBack(group, face, "pos00");
            tourPushBack(group, icon, "pos01");
            tourPushBack(group, handicap, "pos02");
        }
    }

    m_objs[OBJ_COLOR_CURSOR + 1]->changeVisAnimNIf("MenTrnRule0056_TopN__0");
    m_objs[OBJ_COLOR_CURSOR + 1]->m_modelAnim->m_anmObjVisRes->SetUpdateRate(0.0f);
    m_objs[OBJ_COLOR_CURSOR + 1]->setFrameVisible(1.0f);
    m_objs[OBJ_CHAR_FRAME]->changeNodeAnimNIf("MenTrnRule0054_TopN__0");
    m_objs[OBJ_CHAR_FRAME]->m_modelAnim->m_anmObjChrRes->SetUpdateRate(0.0f);
    if (m_charCount < 0x13) {
        m_objs[OBJ_CHAR_FRAME]->setFrameNode(0.0f);
    } else if (m_charCount < 0x15) {
        m_objs[OBJ_CHAR_FRAME]->setFrameNode(1.0f);
    } else if (m_charCount < 0x17) {
        m_objs[OBJ_CHAR_FRAME]->setFrameNode(2.0f);
    } else {
        m_objs[OBJ_CHAR_FRAME]->setFrameNode(3.0f);
    }
    m_objs[OBJ_CHAR_CURSOR]->changeClrAnimNIf("MenTrnRule0055_TopN__0");

    // The picker puts the fighters in two rows: the even ones in the first row.
    if ((m_charCount & 1) == 1) {
        m_charColumns = (m_charCount >> 1) + 1;
    } else {
        m_charColumns = m_charCount >> 1;
    }
    u32 column = 0;
    int node = 0;
    for (; column < (u32)m_charColumns; column++, node += 2) {
        char name[16];
        sprintf(name, "pos%02d", node);
        tourPushBack(m_mdls[7], m_objs[OBJ_CHAR_FACES + column]->m_scnMdl, name);
        int selchk = exchangeMuCharKindToMuSelchkind__6muMenuFiii(m_charList[column]);
        if (selchk < 0x28) {
            m_objs[OBJ_CHAR_FACES + column]->setFrameTex((float)getStockIconBase(selchk));
        } else {
            m_objs[OBJ_CHAR_FACES + column]->setFrameTex(501.0f);
        }
    }
    for (; column < (u32)(m_charCount - 1); column++) {
        char name[16];
        sprintf(name, "pos%02d", (column - m_charColumns) * 2 + 1);
        tourPushBack(m_mdls[7], m_objs[OBJ_CHAR_FACES + column]->m_scnMdl, name);
        int selchk = exchangeMuCharKindToMuSelchkind__6muMenuFiii(m_charList[column]);
        if (selchk < 0x28) {
            m_objs[OBJ_CHAR_FACES + column]->setFrameTex((float)getStockIconBase(selchk));
        } else {
            m_objs[OBJ_CHAR_FACES + column]->setFrameTex(501.0f);
        }
    }
    char name[16];
    sprintf(name, "pos%02d", (m_charCount - m_charColumns) * 2 - 1);
    tourPushBack(m_mdls[7], m_objs[OBJ_CHAR_FACES + m_charCount - 1]->m_scnMdl, name);
    m_objs[OBJ_CHAR_FACES + m_charCount - 1]->setFrameTex(501.0f);

    tourPushBack(m_mdls[4], reinterpret_cast<nw4r::g3d::ScnMdl*>(m_mdls[5]), "pos00");
    tourPushBack(m_mdls[4], reinterpret_cast<nw4r::g3d::ScnMdl*>(m_mdls[6]), "pos01");
    tourPushBack(m_mdls[4], m_objs[OBJ_COLOR_CURSOR + 1]->m_scnMdl, "pos01");
    m_objs[OBJ_STAGE_MODE]->changeAnimN(0, 3, false, false);
    m_objs[OBJ_STAGE_MODE]->changeAnimN(0, 0x10, true, false);
    if (fn_8004E834(g_GameGlobal) == 0) {
        m_objs[OBJ_STAGE_MODE]->setFrameVisible(0.0f);
    } else {
        m_objs[OBJ_STAGE_MODE]->setFrameVisible(1.0f);
    }
}

// The slots of the models of the scene (the groups) are cleared and filled again in a few places.
static void resetSlotModels(muTourRuleTask* task) {
    for (int i = 0; i < 0x20; i++) {
        nw4r::g3d::ScnMdl* face = task->m_objs[OBJ_SLOT_FACE + i]->m_scnMdl;
        nw4r::g3d::ScnMdl* icon = task->m_objs[OBJ_SLOT_ICON + i]->m_scnMdl;
        nw4r::g3d::ScnMdl* handicap = task->m_objs[OBJ_SLOT_HANDICAP + i]->m_scnMdl;
        nw4r::g3d::ScnGroup* group = task->m_mdls[8 + i];
        group->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(face));
        group->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(icon));
        group->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(handicap));
        if (task->m_slots[i].m_kind < 2 && task->m_slots[i].m_kind >= 0) {
            tourPushBack(group, face, "pos00");
            tourPushBack(group, icon, "pos01");
            tourPushBack(group, handicap, "pos02");
        }
    }
}

// The notice about the controllers goes away.
static inline void hideWarning(muTourRuleTask* task) {
    task->m_objs[OBJ_WARNING]->setAnimName(&sAnimWarning[1], true);
    task->m_padWarning = false;
}

// The main function of the menu: every state of the menu reads the pads and moves on when it should.
void muTourRuleTask::processAnim() {
    nw4r::g3d::ScnGroup* scene = m_root->scene;
    if (!m_transition || m_state == State_Load || m_state == State_Setup) {
        if (m_state == State_ConfirmBack && m_objs[OBJ_TITLE]->m_modelAnim->getUpdateRate() == 0.0f) {
            m_state = State_Confirm;
            scene->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_mdls[4]));
        } else if (m_state == State_StageMode && m_objs[OBJ_TITLE]->m_modelAnim->getUpdateRate() == 0.0f) {
            m_state = State_ResumeOpen;
            scene->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_mdls[4]));
        } else if (m_state == State_StageModeBack && m_objs[OBJ_RULE_CONFIRM]->m_modelAnim->getUpdateRate() == 0.0f) {
            m_state = State_ResumeOpen;
            scene->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_mdls[3]));
            scene->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[OBJ_RULE_CONFIRM]->m_scnMdl));
            m_mdls[3]->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_table->m_group));
        }

        // The pads: the buttons of the Wii Remote and the Nunchuk are made to look like the ones of the GameCube pad.
        gfPadStatus pads[8];
        bool moved = false;
        bool extra = false;
        for (int i = 0; i < 8; i++) {
            g_gfPadSystem->getSysPadStatus(i, &pads[i]);
            if (pads[i].m_error >= 0) {
                u32* pressedBits = &pads[i].m_buttonsPressedThisFrame.bits;
                if (pads[i].m_controllerType == gfPadType::NUNCHUK) {
                    u32 pressed = *pressedBits;
                    *pressedBits = pressed & ~0xC00;
                    if (pressed & 0x10000) {
                        *pressedBits |= PAD_Y;
                    } else if (pressed & 0x20000) {
                        *pressedBits |= PAD_X;
                    }
                } else if (pads[i].m_controllerType < 3 && pads[i].m_controllerType > 1) {
                    if (*pressedBits & 0x200000) {
                        *pressedBits |= PAD_X;
                    } else if (*pressedBits & 0x400000) {
                        *pressedBits |= PAD_Y;
                    }
                }
            }
        }
        gfPadStatus status;
        gfPadSystem::merge(pads, 8, &status);
        status.clamp();
        fn_800B9984(&m_ctrl);
        status.m_buttonsPressedThisFrame2.bits |= *reinterpret_cast<u32*>(reinterpret_cast<char*>(&m_ctrl) + 0x10);
        u32 pressed = status.m_buttonsPressedThisFrame.bits;
        u32 repeat = status.m_buttonsPressedThisFrame2.bits;
        if (m_state != State_Load && m_state != State_Setup) {
            setEntryButton();
        }

        switch (m_state) {
        case State_Load: {
            if (!m_loaded[10]) {
                if (*(reinterpret_cast<char*>(m_root) + 0x1A4) == 0) {
                    break;
                }
                changeInput__16CameraControllerFQ216CameraController4Kind(getInstance__16CameraControllerFv(), 9);
                m_loaded[10] = true;
            }
            for (u32 i = 0; i < 8; i++) {
                if (!m_loaded[i] && m_files[i].m_request != NULL) {
                    if (!m_files[i].isReady()) {
                        return;
                    }
                    m_loaded[i] = true;
                    if (i == 0) {
                        m_resStockFace = nw4r::g3d::ResFile(m_files[0].getBuffer());
                        nw4r::g3d::ResFile::Init(&m_resStockFace);
                        m_table = new (Heaps::MenuInstance) muTourTable(&m_resStockFace, 0x13);
                    }
                }
            }
            if (!m_loaded[8]) {
                if (m_archiveA->unk9 == 0) {
                    break;
                }
                m_loaded[8] = true;
            }
            if (!m_loaded[9]) {
                if (m_archiveB->unk9 == 0) {
                    break;
                }
                m_loaded[9] = true;
            }
            if (muMenu::isLoadFinishMenuSound()) {
                m_resRuleA = nw4r::g3d::ResFile(m_archiveA->getData(static_cast<ARCNodeType>(2), 0, 0xFFFE));
                nw4r::g3d::ResFile::Init(&m_resRuleA);
                m_resRuleB = nw4r::g3d::ResFile(m_archiveA->getData(static_cast<ARCNodeType>(2), 1, 0xFFFE));
                nw4r::g3d::ResFile::Init(&m_resRuleB);
                m_resC = nw4r::g3d::ResFile(m_archiveA->getData(static_cast<ARCNodeType>(2), 2, 0xFFFE));
                nw4r::g3d::ResFile::Init(&m_resC);
                m_resD = nw4r::g3d::ResFile(m_archiveA->getData(static_cast<ARCNodeType>(2), 3, 0xFFFE));
                nw4r::g3d::ResFile::Init(&m_resD);
                m_resNameEntry = nw4r::g3d::ResFile(m_files[5].getBuffer());
                nw4r::g3d::ResFile::Init(&m_resNameEntry);
                m_resCharaFace = nw4r::g3d::ResFile(m_files[6].getBuffer());
                nw4r::g3d::ResFile::Init(&m_resCharaFace);
                m_resWifiWnd = nw4r::g3d::ResFile(m_files[7].getBuffer());
                nw4r::g3d::ResFile::Init(&m_resWifiWnd);
                initMdlData();
                g_sndSystem->playBGM(0x2701, 0, true);
                *reinterpret_cast<int*>(reinterpret_cast<char*>(lbl_805A01D8) + 0x174) = 0x2D;
                g_sndSystem->playBGM(0x2701, 0, true);
                *reinterpret_cast<int*>(reinterpret_cast<char*>(lbl_805A01D8) + 0x174) = 0x2D;
                m_state = State_Setup;
                endKeepScreen__17gfKeepFrameBufferFv(getFrameKeeper());
            }
            break;
        }
        case State_Setup: {
            if (m_table->isLoadEnd()) {
                m_table->clearTable(1);
                switch (m_mode) {
                case Mode_Resume:
                    setGlobalData();
                    m_table->setTableData(m_playersPerMatch, m_sizeIndex);
                    m_state = State_Resume;
                    m_confirmSel = 1;
                    setWarning();
                    tourPushBack(m_mdls[0], m_objs[OBJ_STAGE_MODE]->m_scnMdl, "pos03");
                    m_objs[OBJ_TITLE]->setFrameVisible(4.0f);
                    m_objs[OBJ_TITLE]->setAnimName(&sAnimMain[3], true);
                    m_objs[OBJ_TITLE]->setFrameNode(9.0f);
                    m_msg->setAlignMode(1, MuMsg::Align_Center);
                    m_msg->printIndex(1, m_stageMode + 0x1F);
                    m_objs[OBJ_STAGE_MODE]->setFrameNode((float)m_stageMode);
                    drawAllConf();
                    break;
                case Mode_New:
                    scene->Insert(scene->sceneItemsCount, reinterpret_cast<nw4r::g3d::ScnObj*>(m_mdls[0]));
                    m_state = State_SelectPlayers;
                    if (m_playersPerMatch == 2) {
                        m_objs[OBJ_PLAYERS]->setFrameVisible(2.0f);
                    } else if (m_playersPerMatch == 4) {
                        m_objs[OBJ_PLAYERS]->setFrameVisible(1.0f);
                    } else {
                        m_objs[OBJ_PLAYERS]->setFrameVisible(3.0f);
                    }
                    m_objs[OBJ_ENTRANTS]->setFrameFlg((float)m_playersPerMatch, 6);
                    m_objs[2]->setAnimName(&sAnimPlayers[m_playersPerMatch - 2], true);
                    pushbackPanel(1);
                    m_msg->printIndex(0, 0);
                    m_msg->printIndex(1, 0x12);
                    m_objs[2]->setFrameFlg(5.0f, 3);
                    {
                        // Nobody takes part yet.
                        muTourData* data = getTourData();
                        for (int i = 0; i < 32; i++) {
                            muTourEntrant* entrant = &data->m_entrants[i];
                            entrant->m_kind = 0;
                            entrant->m_character = 0x29;
                            entrant->m_color = 0;
                            entrant->m_handicap = 0;
                            entrant->m_slot = -1;
                        }
                    }
                    fn_80074ECC(g_sndSystem, 0x1FF0, 1.0f, 1.0f);
                    break;
                case Mode_Edit:
                    scene->Insert(scene->sceneItemsCount, reinterpret_cast<nw4r::g3d::ScnObj*>(m_mdls[0]));
                    m_state = State_StageMode;
                    m_stageMode = 2;
                    tourPushBack(m_mdls[0], m_objs[OBJ_STAGE_MODE]->m_scnMdl, "pos03");
                    m_objs[OBJ_TITLE]->setFrameVisible(4.0f);
                    m_objs[OBJ_TITLE]->setAnimName(&sAnimMain[3], true);
                    m_objs[OBJ_TITLE]->setFrameNode(9.0f);
                    setGlobalData();
                    for (u32 i = 0; i < (u32)m_entrants; i++) {
                        u8 picked = getTourData()->m_entrants[i].m_pickedCharacter;
                        m_slots[i].m_character = picked;
                        if (picked == 0x29) {
                            m_slots[i].m_color = 0;
                        }
                    }
                    m_table->setTableData(m_playersPerMatch, m_sizeIndex);
                    setWarning();
                    m_msg->setAlignMode(1, MuMsg::Align_Center);
                    m_msg->printIndex(1, m_stageMode + 0x1F);
                    m_objs[OBJ_STAGE_MODE]->setFrameNode((float)m_stageMode);
                    break;
                case Mode_Rules:
                    setGlobalData();
                    for (u32 i = 0; i < (u32)m_entrants; i++) {
                        u8 picked = getTourData()->m_entrants[i].m_pickedCharacter;
                        m_slots[i].m_character = picked;
                        if (picked == 0x29) {
                            m_slots[i].m_color = 0;
                        }
                    }
                    setupTournament();
                    m_table->setTableData(m_playersPerMatch, m_sizeIndex);
                    m_state = State_Resume;
                    m_confirmSel = 1;
                    setWarning();
                    tourPushBack(m_mdls[0], m_objs[OBJ_STAGE_MODE]->m_scnMdl, "pos03");
                    m_objs[OBJ_TITLE]->setFrameVisible(4.0f);
                    m_objs[OBJ_TITLE]->setAnimName(&sAnimMain[3], true);
                    m_objs[OBJ_TITLE]->setFrameNode(9.0f);
                    m_msg->setAlignMode(1, MuMsg::Align_Center);
                    m_msg->printIndex(1, m_stageMode + 0x1F);
                    m_objs[OBJ_STAGE_MODE]->setFrameNode((float)m_stageMode);
                    drawAllConf();
                    break;
                }
            }
            break;
        }
        case State_SelectPlayers:
            if (repeat & PAD_LEFT) {
                if (m_playersPerMatch > 2) {
                    moved = true;
                    m_playersPerMatch--;
                }
            } else if (repeat & PAD_RIGHT) {
                if (m_playersPerMatch < 4) {
                    moved = true;
                    m_playersPerMatch++;
                }
            } else if (pressed & PAD_A) {
                g_sndSystem->playSE(static_cast<SndID>(1), -1, 0, 0, -1);
                m_sizeIndex = 0;
                m_objs[OBJ_TITLE]->setAnimName(&sAnimMain[1], true);
                m_transition = true;
                m_state = State_SelectEntrants;
            } else if (pressed & PAD_B) {
                g_sndSystem->playSE(static_cast<SndID>(2), -1, 0, 0, -1);
                m_result = Result_Back;
                m_state = State_Leave;
                startKeepScreen__17gfKeepFrameBufferFv(getFrameKeeper());
            }
            if (moved) {
                g_sndSystem->playSE(static_cast<SndID>(0x25), -1, 0, 0, -1);
                if (m_playersPerMatch == 2) {
                    m_objs[OBJ_PLAYERS]->setFrameVisible(2.0f);
                } else if (m_playersPerMatch == 4) {
                    m_objs[OBJ_PLAYERS]->setFrameVisible(1.0f);
                } else {
                    m_objs[OBJ_PLAYERS]->setFrameVisible(3.0f);
                }
                m_objs[OBJ_ENTRANTS]->setFrameFlg((float)m_playersPerMatch, 6);
                m_objs[2]->setAnimName(&sAnimPlayers[m_playersPerMatch - 2], true);
                m_edited = 0;
            }
            break;
        case State_SelectEntrants: {
            const u32 lastSize[5] = {0, 0, 6, 7, 8};
            if (repeat & PAD_LEFT) {
                if (m_sizeIndex != 0) {
                    moved = true;
                    m_sizeIndex--;
                }
            } else if (repeat & PAD_RIGHT) {
                if ((u32)m_sizeIndex < lastSize[m_playersPerMatch]) {
                    moved = true;
                    m_sizeIndex++;
                }
            } else if (pressed & PAD_A) {
                g_sndSystem->playSE(static_cast<SndID>(1), -1, 0, 0, -1);
                m_objs[OBJ_TITLE]->setAnimName(&sAnimMain[1], true);
                m_transition = true;
                m_state = State_Confirm;
                m_cursor = 1;
            } else if (pressed & PAD_B) {
                g_sndSystem->playSE(static_cast<SndID>(2), -1, 0, 0, -1);
                m_objs[OBJ_TITLE]->setAnimName(&sAnimMain[1], true);
                m_transition = true;
                m_state = State_SelectPlayers;
            }
            if (moved) {
                g_sndSystem->playSE(static_cast<SndID>(0x25), -1, 0, 0, -1);
                drawSelectTotalPlayer(false);
                m_edited = 0;
            }
            break;
        }
        case State_Confirm:
            if ((repeat & PAD_UP) && m_cursor == 1) {
                moved = true;
                m_cursor = 0;
            } else if ((repeat & PAD_DOWN) && m_cursor == 0) {
                moved = true;
                m_cursor = 1;
            } else if (pressed & PAD_A) {
                g_sndSystem->playSE(static_cast<SndID>(1), -1, 0, 0, -1);
                if (m_cursor == 0) {
                    // The rules of the matches are set in the menu of the rules.
                    m_state = State_RuleMenu;
                    muRuleTask* rule = new (Heaps::MenuInstance) muRuleTask(scene, m_archiveB, static_cast<muPanelCtrlTask*>(m_panel), 1, 0x17);
                    rule->init();
                    m_ruleTask = rule;
                    scene->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[0]->m_scnMdl));
                    scene->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_mdls[0]));
                    scene->Insert(scene->sceneItemsCount, reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[OBJ_OPTION]->m_scnMdl));
                    if (m_padWarning) {
                        hideWarning(this);
                        break;
                    }
                } else {
                    // The fighters: the slots are shown.
                    m_state = State_SelectFighters;
                    m_cursor = 0;
                    m_slotCursor = 0;
                    m_onPaging = false;
                    m_onCpuLevel = false;
                    m_padCount = countPads();
                    m_nameDone = 0;
                    if (m_edited == 0) {
                        initPlayerData();
                    }
                    m_ctrlPad = fn_800BA2E0();
                    m_colorShown = true;
                    m_page = 0;
                    m_pageY = 0.0f;
                    m_pageMoving = false;
                    Vec3f origin(0.0f, 0.0f, 0.0f);
                    m_objs[OBJ_PAGE_COUNT]->setTrans(&origin);
                    resetSlotModels(this);
                    drawSelectName(1, 1, true);
                    if (m_padWarning) {
                        hideWarning(this);
                        break;
                    }
                }
            } else if (pressed & PAD_B) {
                g_sndSystem->playSE(static_cast<SndID>(2), -1, 0, 0, -1);
                m_objs[OBJ_TITLE]->setAnimName(&sAnimMain[1], true);
                m_transition = true;
                m_cursor = 0;
                m_state = State_SelectEntrants;
                if (m_padWarning) {
                    hideWarning(this);
                    break;
                }
            }
            if (moved) {
                g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
                drawSelectConf(false);
            }
            // The notice shows up when there are less controllers than the fighters in a match need.
            if (countPads() < (u32)m_playersPerMatch) {
                if (!m_padWarning) {
                    m_objs[OBJ_WARNING]->setAnimName(&sAnimWarning[0], true);
                    m_padWarning = true;
                }
            } else if (m_padWarning) {
                hideWarning(this);
            }
            break;
        case State_RuleMenu:
            if (m_ruleTask->m_end) {
                g_sndSystem->playSE(static_cast<SndID>(2), -1, 0, 0, -1);
                m_ruleTask->removeObj();
                m_ruleTask->exit();
                m_ruleTask = NULL;
                scene->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[OBJ_OPTION]->m_scnMdl));
                scene->Insert(scene->sceneItemsCount, reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[0]->m_scnMdl));
                scene->Insert(scene->sceneItemsCount, reinterpret_cast<nw4r::g3d::ScnObj*>(m_mdls[0]));
                m_state = State_Confirm;
                drawSelectConf(true);
            }
            break;
        case State_SelectFighters: {
            int dir = 0;
            u8 onPaging = m_onPaging;
            u8 onCpuLevel = m_onCpuLevel;
            if (pressed & PAD_X) {
                dir = 1;
            } else if (pressed & PAD_Y) {
                dir = -1;
            }
            if (repeat & PAD_LEFT) {
                if (!onCpuLevel) {
                    if (!onPaging) {
                        if (m_cursor == 0) {
                            if (m_slotCursor > 3) {
                                u32 slot = m_slotCursor - 4;
                                m_slotCursor = slot;
                                moved = true;
                                m_cursor = 2;
                                if ((slot >> 2) < (u32)m_page) {
                                    m_page--;
                                    m_pageMoving = true;
                                    m_pageY = (float)m_page * -25.5f;
                                }
                            }
                        } else {
                            moved = true;
                            m_cursor--;
                        }
                    }
                } else if (m_cpuLevel != 0) {
                    moved = true;
                    m_cpuLevel--;
                }
            } else if (repeat & PAD_RIGHT) {
                if (!onCpuLevel) {
                    if (!onPaging) {
                        if (m_cursor == 2) {
                            u32 slot = m_slotCursor + 4;
                            if (slot < (u32)m_entrants) {
                                m_slotCursor = slot;
                                u32 page = m_page + 1;
                                moved = true;
                                m_cursor = 0;
                                if (page < (slot >> 2)) {
                                    m_pageMoving = true;
                                    m_page = page;
                                    m_pageY = (float)page * -25.5f;
                                }
                            }
                        } else {
                            moved = true;
                            m_cursor++;
                        }
                    }
                } else if (m_cpuLevel < 9) {
                    moved = true;
                    m_cpuLevel++;
                }
            } else if (repeat & PAD_UP) {
                if (!onPaging) {
                    if (!onCpuLevel) {
                        if ((m_slotCursor & 3) == 0) {
                            if (m_hasCpu) {
                                moved = true;
                                m_onCpuLevel = true;
                            }
                        } else {
                            moved = true;
                            m_slotCursor--;
                        }
                    }
                } else {
                    moved = true;
                    m_onPaging = false;
                }
            } else if (repeat & PAD_DOWN) {
                if (!onCpuLevel) {
                    if (!onPaging) {
                        u32 slot = m_slotCursor;
                        if ((slot & 3) == 3 || slot == (u32)(m_entrants - 1)) {
                            moved = true;
                            m_onPaging = true;
                        } else {
                            moved = true;
                            m_slotCursor = slot + 1;
                        }
                    }
                } else {
                    moved = true;
                    m_onCpuLevel = false;
                }
            } else if (pressed & 0x1000) {
                if (!onCpuLevel) {
                    if (!onPaging) {
                        moved = true;
                        m_onPaging = true;
                    } else {
                        // Start on the buttons: the way the stages are picked.
                        g_sndSystem->playSE(static_cast<SndID>(1), -1, 0, 0, -1);
                        m_state = State_StageMode;
                        m_stageMode = 0;
                        tourPushBack(m_mdls[0], m_objs[OBJ_STAGE_MODE]->m_scnMdl, "pos03");
                        m_objs[OBJ_TITLE]->setFrameVisible(4.0f);
                        m_objs[OBJ_TITLE]->setAnimName(&sAnimMain[3], true);
                        m_objs[OBJ_PAGING]->setAnimName(&sAnimPaging[1], true);
                        m_msg->setAlignMode(1, MuMsg::Align_Center);
                        m_msg->printIndex(1, m_stageMode + 0x1F);
                        m_objs[OBJ_STAGE_MODE]->setFrameNode((float)m_stageMode);
                        if (m_cpuAnim == 1) {
                            m_cpuAnim = 0;
                            m_objs[OBJ_CPU_LEVEL]->setAnimName(&sAnimCpuLevel[1], true);
                        }
                    }
                } else {
                    m_onCpuLevel = false;
                    moved = true;
                    m_onPaging = true;
                }
            } else if (dir != 0) {
                // X and Y change the costume of the fighter, or the kind of the slot.
                if (!onPaging && !onCpuLevel) {
                    muTourSlot* slot = &m_slots[m_slotCursor];
                    if (m_cursor == 1) {
                        if (slot->m_character != 0x29 && slot->m_kind != 2) {
                            int count = getNumCharColor__6muMenuFiii(slot->m_character);
                            g_sndSystem->playSE(static_cast<SndID>(0x23), -1, 0, 0, -1);
                            if (dir == 1) {
                                slot->m_color = (slot->m_color + 1) % count;
                            } else {
                                u8 color = slot->m_color;
                                if (color == 0) {
                                    color = count;
                                }
                                slot->m_color = color - 1;
                            }
                            if (slot->m_character == 0x29 || slot->m_kind == 2) {
                                drawColorIcon(0x29, 0);
                            } else {
                                drawColorIcon(slot->m_character, slot->m_color);
                            }
                            setPlayerWindow(m_slotCursor);
                        }
                    } else if (m_cursor == 0) {
                        u32 humans = 0;
                        for (u32 i = 0; (i & 0xFF) < (u32)m_entrants; i++) {
                            if (m_slots[i & 0xFF].m_kind == 0) {
                                humans++;
                            }
                        }
                        int kind = slot->m_kind;
                        if (humans < 2 && kind == 0) {
                            g_sndSystem->playSE(static_cast<SndID>(3), -1, 0, 0, -1);
                        } else {
                            u32 newKind;
                            if (kind == 1) {
                                newKind = (dir == 1) ? 2 : 0;
                            } else if (kind < 1 || kind > 2) {
                                newKind = (dir == 1) ? 1 : 2;
                            } else {
                                newKind = (dir == 1) ? 0 : 1;
                            }
                            slot->m_kind = newKind;
                            moved = true;
                            extra = true;
                            nw4r::g3d::ScnMdl* face = m_objs[OBJ_SLOT_FACE + m_slotCursor]->m_scnMdl;
                            nw4r::g3d::ScnMdl* icon = m_objs[OBJ_SLOT_ICON + m_slotCursor]->m_scnMdl;
                            nw4r::g3d::ScnMdl* handicap = m_objs[OBJ_SLOT_HANDICAP + m_slotCursor]->m_scnMdl;
                            nw4r::g3d::ScnGroup* group = m_mdls[8 + m_slotCursor];
                            group->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(face));
                            group->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(icon));
                            group->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(handicap));
                            if (m_slots[m_slotCursor].m_kind < 2 && m_slots[m_slotCursor].m_kind >= 0) {
                                tourPushBack(group, face, "pos00");
                                tourPushBack(group, icon, "pos01");
                                tourPushBack(group, handicap, "pos02");
                            }
                            // A slot that was not named by the player gets the first free number of its new kind.
                            if (m_slots[m_slotCursor].m_named == 0) {
                                if (newKind == 1) {
                                    for (u8 number = 0; number < (u32)m_entrants; number++) {
                                        u32 other;
                                        for (other = 0; (other & 0xFF) < (u32)m_entrants; other++) {
                                            muTourSlot* s = &m_slots[other & 0xFF];
                                            if (s->m_kind == 1 && s->m_named == 0 && number == s->m_index && (other & 0xFF) != (u32)m_slotCursor) {
                                                break;
                                            }
                                        }
                                        if ((other & 0xFF) == (u32)m_entrants) {
                                            makeSlotName(m_msg, &m_slots[m_slotCursor], 0xD, number + 1);
                                            m_slots[m_slotCursor].m_index = number;
                                            break;
                                        }
                                    }
                                } else if (kind == 1) {
                                    for (u8 number = 0; number < (u32)m_entrants; number++) {
                                        u32 other;
                                        for (other = 0; (other & 0xFF) < (u32)m_entrants; other++) {
                                            muTourSlot* s = &m_slots[other & 0xFF];
                                            if (s->m_kind != 1 && s->m_named == 0 && number == s->m_index && (other & 0xFF) != (u32)m_slotCursor) {
                                                break;
                                            }
                                        }
                                        if ((other & 0xFF) == (u32)m_entrants) {
                                            makeSlotName(m_msg, &m_slots[m_slotCursor], 0xC, number + 1);
                                            m_slots[m_slotCursor].m_index = number;
                                            break;
                                        }
                                    }
                                }
                            }
                            // More players than controllers: the notice is shown.
                            if (newKind == 0 && (u32)m_padCount < (u32)m_playersPerMatch) {
                                u32 count = 0;
                                for (u32 i = 0; (i & 0xFF) < (u32)m_entrants; i++) {
                                    if (m_slots[i & 0xFF].m_kind == 0) {
                                        count++;
                                        if ((u32)m_padCount < count) {
                                            m_state = State_Warning;
                                            m_objs[OBJ_WARNING]->setAnimName(&sAnimWarning[0], true);
                                            m_padWarning = true;
                                            m_timer = 0;
                                            break;
                                        }
                                    }
                                }
                            }
                            m_hasCpu = false;
                            for (u32 i = 0; (i & 0xFF) < (u32)m_entrants; i++) {
                                if (m_slots[i & 0xFF].m_kind == 1) {
                                    m_hasCpu = true;
                                    break;
                                }
                            }
                        }
                    }
                }
            } else if (pressed & PAD_A) {
                if (!onPaging) {
                    if (!onCpuLevel && m_slots[m_slotCursor].m_kind != 2) {
                        g_sndSystem->playSE(static_cast<SndID>(1), -1, 0, 0, -1);
                        muTourSlot* slot = &m_slots[m_slotCursor];
                        if (m_cursor == 1) {
                            // The fighter and costume of the slot
                            m_state = State_SelectChar;
                            m_selChar = slot->m_character;
                            m_selColor = slot->m_color;
                            if (slot->m_character == 0x29) {
                                m_charGridCursor = m_charCursor;
                                int kind = m_charList[m_charCursor];
                                if (kind == 0x23) {
                                    m_selChar = 0x29;
                                } else {
                                    m_selChar = exchangeMuCharKindToMuSelchkind__6muMenuFiii(kind);
                                }
                            } else {
                                m_charGridCursor = 0;
                                for (u32 i = 0; i < (u32)m_charCount - 1; i++) {
                                    if (m_charList[i] == exchangeMuSelchkindToMuCharKind__6muMenuFiii(slot->m_character)) {
                                        m_charGridCursor = i;
                                        break;
                                    }
                                }
                            }
                            u32 column = m_charGridCursor % m_charColumns;
                            if ((int)(column - m_charScroll) < 0xB) {
                                if (column < (u32)m_charScroll) {
                                    m_charScroll = column;
                                    m_charScrollMoving = true;
                                    m_charScrollX = (float)column * -5.6f;
                                }
                            } else {
                                m_charScroll = column - 10;
                                m_charScrollMoving = true;
                                m_charScrollX = (float)(column - 10) * -5.6f;
                            }
                            m_objs[OBJ_COLOR_CURSOR + 1]->setFrameVisible(0.0f);
                            m_objs[OBJ_SLOT_CURSOR]->setFrameVisible(4.0f);
                            drawSelectChar();
                        } else if (m_cursor == 0) {
                            // The name: the window where a name is picked opens.
                            m_state = State_NameWindow;
                            m_nameWnd = muNameTask::create(scene, 3, &m_resC, &m_resNameEntry, 0xF0, 0x34, NULL, NULL, &m_resD,
                                                           m_files[4].getBuffer(), &m_resWifiWnd);
                            for (u32 i = 0; i < (u32)m_entrants; i++) {
                                if ((u32)m_slotCursor != i) {
                                    m_nameWnd->disableSelectName(m_slots[i].m_nameTag);
                                }
                            }
                            m_msg->printIndex(1, 0x1A);
                        } else if (m_cursor == 2) {
                            // The handicap
                            m_state = State_Handicap;
                            u32 value = m_slots[m_slotCursor].m_handicap / 10;
                            m_handicap = value;
                            m_handicapTop = value;
                            if (value > 0x1A) {
                                m_handicapTop = 0x1A;
                            }
                            m_objs[OBJ_HANDICAP_WND]->setAnimName(&sAnimHandicap[0], true);
                            scene->Insert(scene->sceneItemsCount, reinterpret_cast<nw4r::g3d::ScnObj*>(m_mdls[5]));
                            drawHandiWindow();
                            m_msg->printIndex(1, 0x1D);
                        }
                    }
                } else {
                    // A on the buttons: the way the stages are picked.
                    g_sndSystem->playSE(static_cast<SndID>(1), -1, 0, 0, -1);
                    m_state = State_StageMode;
                    m_stageMode = 0;
                    tourPushBack(m_mdls[0], m_objs[OBJ_STAGE_MODE]->m_scnMdl, "pos03");
                    m_objs[OBJ_TITLE]->setFrameVisible(4.0f);
                    m_objs[OBJ_TITLE]->setAnimName(&sAnimMain[3], true);
                    m_objs[OBJ_PAGING]->setAnimName(&sAnimPaging[1], true);
                    m_msg->setAlignMode(1, MuMsg::Align_Center);
                    m_msg->printIndex(1, m_stageMode + 0x1F);
                    m_objs[OBJ_STAGE_MODE]->setFrameNode((float)m_stageMode);
                    if (m_cpuAnim == 1) {
                        m_cpuAnim = 0;
                        m_objs[OBJ_CPU_LEVEL]->setAnimName(&sAnimCpuLevel[1], true);
                    }
                }
            } else if (pressed & PAD_B) {
                g_sndSystem->playSE(static_cast<SndID>(2), -1, 0, 0, -1);
                m_cursor = 1;
                m_transition = true;
                m_state = State_ConfirmBack;
                m_objs[OBJ_TITLE]->setAnimName(&sAnimMain[0], true);
                m_objs[OBJ_PAGING]->setAnimName(&sAnimPaging[1], true);
                if (m_cpuAnim == 1) {
                    m_cpuAnim = 0;
                    m_objs[OBJ_CPU_LEVEL]->setAnimName(&sAnimCpuLevel[1], true);
                }
            }

            if (moved || extra) {
                g_sndSystem->playSE(static_cast<SndID>(extra ? 0x23 : 0), -1, 0, 0, -1);
                drawSelectName(onPaging, onCpuLevel, false);
            } else if (m_cursor == 1) {
                muTourSlot* slot = &m_slots[m_slotCursor];
                int option = charSelOption(slot->m_character);
                if (option != slot->m_character) {
                    g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
                    m_slots[m_slotCursor].m_color = 0;
                    m_slots[m_slotCursor].m_character = option;
                    drawColorIcon(m_slots[m_slotCursor].m_character, 0);
                    setPlayerWindow(m_slotCursor);
                }
            }
            if (m_pageMoving) {
                MuObject* pageObj = m_objs[OBJ_PAGE_COUNT];
                float target = m_pageY;
                Vec3f pos;
                tourGetTrans(pageObj, &pos);
                pos.m_x = pos.m_x + (target - pos.m_x) * 0.25f;
                bool keepMoving = 0.05f <= fabsf(target - pos.m_x);
                if (!keepMoving) {
                    pos.m_x = target;
                }
                pageObj->setTrans(&pos);
                m_pageMoving = keepMoving;
                if (!keepMoving) {
                    m_pageY = 0.0f;
                }
            }
            // Controllers that are unplugged while the slots are shown.
            u32 pads = countPads();
            if (pads < (u32)m_padCount && pads < (u32)m_playersPerMatch) {
                u32 humans = 0;
                for (int i = m_entrants; i != 0; i--) {
                    if (m_slots[m_entrants - i].m_kind == 0) {
                        humans++;
                    }
                }
                if (pads < humans) {
                    m_state = State_Warning;
                    m_objs[OBJ_WARNING]->setAnimName(&sAnimWarning[0], true);
                    m_padWarning = true;
                    m_timer = 0;
                }
            }
            m_padCount = pads;
            break;
        }
        case State_Warning:
            m_timer++;
            if (m_timer > 0x14) {
                pressed &= ~0x800000;
                repeat &= ~0x800000;
                if (pressed != 0 || repeat != 0) {
                    m_padWarning = false;
                }
                u32 pads = countPads();
                if ((u32)m_padCount < pads) {
                    u32 humans = 0;
                    for (int i = m_entrants; i != 0; i--) {
                        if (m_slots[m_entrants - i].m_kind == 0) {
                            humans++;
                        }
                    }
                    if (humans <= pads || (u32)m_playersPerMatch <= pads) {
                        m_padWarning = false;
                    }
                }
                if (!m_padWarning) {
                    m_state = State_SelectFighters;
                    m_objs[OBJ_WARNING]->setAnimName(&sAnimWarning[1], true);
                }
                m_padCount = pads;
            }
            break;
        case State_NameWindow:
            switch (m_nameWnd->m_resultKind) {
            case 3:
                if (!m_nameDone) {
                    g_sndSystem->playSE(static_cast<SndID>(2), -1, 0, 0, -1);
                    m_nameDone = 1;
                }
                break;
            case 1:
            case 2:
                if (!m_nameDone) {
                    m_slots[m_slotCursor].m_named = 1;
                    m_slots[m_slotCursor].m_nameTag = m_nameWnd->m_resultNameNo;
                    fn_8004D6C4(g_GameGlobal, m_nameWnd->m_resultNameNo, m_slots[m_slotCursor].m_name);
                    setPlayerWindow(m_slotCursor);
                    m_nameWnd->m_step = muNameTask::Step_Leave;
                    m_edited = 1;
                    m_nameDone = 1;
                }
                break;
            case 4:
                m_state = State_SelectFighters;
                m_nameWnd->exit();
                m_nameWnd = NULL;
                m_msg->printIndex(1, 0x16);
                m_nameDone = 0;
                break;
            }
            break;
        case State_SelectChar:
            if (pressed & PAD_X) {
                // X and Y: the costume
            }
            {
                int dir = 0;
                if (pressed & PAD_X) {
                    dir = 1;
                } else if (pressed & PAD_Y) {
                    dir = -1;
                }
                if (repeat & PAD_LEFT) {
                    // one fighter to the left (up the first row, the last of the second row)
                    u32 columns = m_charColumns;
                    moved = true;
                    u32 cursor = m_charGridCursor;
                    if (cursor == (cursor / columns) * columns) {
                        if (cursor < columns) {
                            m_charGridCursor = columns - 1;
                        } else {
                            m_charGridCursor = m_charCount - 1;
                        }
                        u32 column = m_charGridCursor % m_charColumns;
                        if (column > 0xB) {
                            m_charScroll = column - 10;
                            m_charScrollMoving = true;
                            m_charScrollX = (float)(column - 10) * -5.6f;
                        }
                    } else {
                        cursor--;
                        m_charGridCursor = cursor;
                        if (cursor - (cursor / columns) * columns < (u32)m_charScroll) {
                            m_charScroll--;
                            m_charScrollMoving = true;
                            m_charScrollX = (float)m_charScroll * -5.6f;
                        }
                    }
                } else if (repeat & PAD_RIGHT) {
                    u32 columns = m_charColumns;
                    moved = true;
                    u32 cursor = m_charGridCursor;
                    if ((cursor - (cursor / columns) * columns) < columns - 1 && cursor + 1 < (u32)m_charCount) {
                        m_charGridCursor = cursor + 1;
                        u32 column = (cursor + 1) - ((cursor + 1) / columns) * columns;
                        if ((int)(column - m_charScroll) > 10) {
                            m_charScroll++;
                            m_charScrollMoving = true;
                            m_charScrollX = (float)m_charScroll * -5.6f;
                        }
                    } else {
                        if (cursor < columns) {
                            m_charGridCursor = 0;
                        } else {
                            m_charGridCursor = columns;
                        }
                        if (m_charScroll != 0) {
                            m_charScroll = 0;
                            m_charScrollMoving = true;
                            m_charScrollX = 0.0f;
                        }
                    }
                } else if (repeat & PAD_UP) {
                    if ((u32)m_charGridCursor >= (u32)m_charColumns) {
                        moved = true;
                        m_charGridCursor -= m_charColumns;
                    }
                } else if (repeat & PAD_DOWN) {
                    if ((u32)m_charGridCursor < (u32)m_charColumns) {
                        u32 below = m_charGridCursor + m_charColumns;
                        if (below < (u32)m_charCount) {
                            m_charGridCursor = below;
                            moved = true;
                        }
                    }
                } else if (dir == 0) {
                    if (pressed & PAD_A) {
                        m_edited = 1;
                        m_charCursor = m_charGridCursor;
                        g_sndSystem->playSE(static_cast<SndID>(1), -1, 0, 0, -1);
                        m_state = State_SelectFighters;
                        m_slots[m_slotCursor].m_character = m_selChar;
                        m_slots[m_slotCursor].m_color = m_selColor;
                        m_objs[OBJ_COLOR_CURSOR + 1]->setFrameVisible(1.0f);
                        m_mdls[7]->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[OBJ_CHAR_CURSOR]->m_scnMdl));
                        m_objs[OBJ_SLOT_CURSOR]->setFrameVisible(2.0f);
                        drawSelectName(m_onPaging, m_onCpuLevel, false);
                    } else if (pressed & PAD_B) {
                        m_charCursor = m_charGridCursor;
                        g_sndSystem->playSE(static_cast<SndID>(2), -1, 0, 0, -1);
                        m_state = State_SelectFighters;
                        setPlayerWindow(m_slotCursor);
                        drawColorIcon(m_slots[m_slotCursor].m_character, m_slots[m_slotCursor].m_color);
                        m_objs[OBJ_COLOR_CURSOR + 1]->setFrameVisible(1.0f);
                        m_mdls[7]->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[OBJ_CHAR_CURSOR]->m_scnMdl));
                        m_objs[OBJ_SLOT_CURSOR]->setFrameVisible(2.0f);
                        drawSelectName(m_onPaging, m_onCpuLevel, false);
                    }
                } else if (m_selChar != 0x29) {
                    int count = getNumCharColor__6muMenuFiii(m_selChar);
                    g_sndSystem->playSE(static_cast<SndID>(0x23), -1, 0, 0, -1);
                    if (dir == 1) {
                        m_selColor = (m_selColor + 1) % count;
                    } else {
                        u8 color = m_selColor;
                        if (color == 0) {
                            color = count;
                        }
                        m_selColor = color - 1;
                    }
                    drawSelectChar();
                }
                if (moved) {
                    g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
                    m_selColor = 0;
                    int kind = m_charList[m_charGridCursor];
                    if (kind == 0x23) {
                        m_selChar = 0x29;
                    } else {
                        m_selChar = exchangeMuCharKindToMuSelchkind__6muMenuFiii(kind);
                    }
                    drawSelectChar();
                } else {
                    int option = charSelOption(m_selChar);
                    if (m_selChar != option) {
                        g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
                        m_selChar = option;
                        m_selColor = 0;
                        drawSelectChar();
                    }
                }
                if (m_charScrollMoving) {
                    MuObject* frame = m_objs[14];
                    float target = m_charScrollX;
                    Vec3f pos;
                    tourGetTrans(frame, &pos);
                    pos.m_x = pos.m_x + (target - pos.m_x) * 0.25f;
                    bool keepMoving = 0.05f <= fabsf(target - pos.m_x);
                    if (!keepMoving) {
                        pos.m_x = target;
                    }
                    frame->setTrans(&pos);
                    m_charScrollMoving = keepMoving;
                    if (!keepMoving) {
                        m_charScrollX = 0.0f;
                    }
                }
            }
            break;
        case State_Handicap:
            if ((repeat & PAD_UP) && m_handicap != 0) {
                m_handicap--;
                moved = true;
                if ((u32)m_handicap < (u32)m_handicapTop) {
                    m_handicapTop = m_handicap;
                }
            } else if ((repeat & PAD_DOWN) && (u32)m_handicap <= 0x1D) {
                m_handicap++;
                moved = true;
                if ((u32)m_handicapTop + 5 <= (u32)m_handicap) {
                    m_handicapTop = m_handicap - 4;
                }
            } else if (pressed & PAD_A) {
                g_sndSystem->playSE(static_cast<SndID>(1), -1, 0, 0, -1);
                m_objs[OBJ_HANDICAP_WND]->setAnimName(&sAnimHandicap[1], true);
                m_state = State_HandicapEnd;
                m_slots[m_slotCursor].m_handicap = m_handicap * 10;
                setPlayerWindow(m_slotCursor);
                m_edited = 1;
                m_msg->printIndex(1, 0x18);
            } else if (pressed & PAD_B) {
                g_sndSystem->playSE(static_cast<SndID>(2), -1, 0, 0, -1);
                m_objs[OBJ_HANDICAP_WND]->setAnimName(&sAnimHandicap[1], true);
                m_state = State_HandicapEnd;
                m_msg->printIndex(1, 0x18);
            }
            if (moved) {
                g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
                drawHandiWindow();
            }
            break;
        case State_HandicapEnd:
            if (m_objs[OBJ_HANDICAP_WND]->m_modelAnim->getUpdateRate() == 0.0f) {
                scene->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_mdls[5]));
                m_state = State_SelectFighters;
            }
            break;
        case State_ResumeOpen:
        case State_StageMode:
        case State_StageModeBack:
            if ((repeat & PAD_UP) && m_stageMode != 0) {
                moved = true;
                m_stageMode--;
            } else if (repeat & PAD_DOWN) {
                u32 last = fn_8004E834(g_GameGlobal) ? 3 : 2;
                if ((u32)m_stageMode < last) {
                    moved = true;
                    m_stageMode++;
                }
            } else if (pressed & PAD_A) {
                if (m_state == State_StageMode) {
                    m_state = State_ResumeOpen;
                    scene->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_mdls[4]));
                }
                m_state = State_Finish;
                g_sndSystem->playSE(static_cast<SndID>(1), -1, 0, 0, -1);
            } else if (pressed & PAD_B) {
                // Back to the slots of the fighters.
                if (m_state == State_StageModeBack) {
                    m_state = State_ResumeOpen;
                    scene->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_mdls[3]));
                    scene->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[OBJ_RULE_CONFIRM]->m_scnMdl));
                    m_mdls[3]->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_table->m_group));
                }
                resetSlotModels(this);
                g_sndSystem->playSE(static_cast<SndID>(2), -1, 0, 0, -1);
                m_padCount = countPads();
                m_mdls[0]->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[OBJ_STAGE_MODE]->m_scnMdl));
                m_objs[OBJ_TITLE]->setFrameVisible(2.0f);
                drawSelectName(m_onPaging, m_onCpuLevel, true);
                m_state = State_SelectFighters;
            }
            if (moved) {
                g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
                m_msg->setAlignMode(1, MuMsg::Align_Center);
                m_msg->printIndex(1, m_stageMode + 0x1F);
                m_objs[OBJ_STAGE_MODE]->setFrameNode((float)m_stageMode);
            }
            break;
        case State_StageRuleMenu:
            if (m_ruleTask->m_end) {
                m_ruleTask->removeObj();
                m_ruleTask->exit();
                m_ruleTask = NULL;
                scene->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[OBJ_OPTION]->m_scnMdl));
                scene->Insert(scene->sceneItemsCount, reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[0]->m_scnMdl));
                scene->Insert(scene->sceneItemsCount, reinterpret_cast<nw4r::g3d::ScnObj*>(m_mdls[0]));
                m_state = State_StageMode;
                m_msg->setAlignMode(1, MuMsg::Align_Center);
                m_msg->printIndex(1, m_stageMode + 0x1F);
                m_objs[OBJ_STAGE_MODE]->setFrameNode((float)m_stageMode);
            }
            break;
        case State_Finish: {
            // The settings are written in the data of the game.
            muTourData* data = getTourData();
            data->m_count = m_entrants;
            data->m_playersPerMatch = m_playersPerMatch;
            data->m_unk02 = 0;
            data->m_unk03 = m_playersPerMatch - 1;
            data->m_sizeIndex = m_sizeIndex;
            data->m_stageMode = m_stageMode;
            data->m_cpuLevel = m_cpuLevel;
            if (m_stageMode != 3) {
                setupTournament();
            }
            if (m_stageMode == 2) {
                m_result = Result_Resume;
                m_state = State_Leave;
                startKeepScreen__17gfKeepFrameBufferFv(getFrameKeeper());
            } else if (m_stageMode < 2) {
                if (m_stageMode >= 0) {
                    m_state = State_Resume;
                    m_confirmSel = 1;
                    drawAllConf();
                }
            } else if (m_stageMode < 4) {
                m_state = State_StageRuleMenu;
                muRuleTask* rule = new (Heaps::MenuInstance) muRuleTask(scene, m_archiveB, static_cast<muPanelCtrlTask*>(m_panel), 4, 0x17);
                rule->init();
                m_ruleTask = rule;
                scene->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[0]->m_scnMdl));
                scene->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_mdls[0]));
                scene->Insert(scene->sceneItemsCount, reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[OBJ_OPTION]->m_scnMdl));
            }
            break;
        }
        case State_Resume:
            if ((repeat & PAD_UP) && m_confirmSel == 1) {
                m_confirmSel = 0;
                g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
                m_objs[OBJ_RULE_CONFIRM]->changeClrAnimNIf("MenTrnRule0044_TopN__0");
            } else if ((repeat & PAD_DOWN) && m_confirmSel == 0) {
                m_confirmSel = 1;
                g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
                m_objs[OBJ_RULE_CONFIRM]->changeClrAnimNIf("MenTrnRule0044_TopN__1");
            } else if (pressed & PAD_A) {
                g_sndSystem->playSE(static_cast<SndID>(1), -1, 0, 0, -1);
                if (m_confirmSel == 0) {
                    m_result = Result_Start;
                    m_state = State_Leave;
                    startKeepScreen__17gfKeepFrameBufferFv(getFrameKeeper());
                } else {
                    m_state = State_StageModeBack;
                    scene->Insert(scene->sceneItemsCount, reinterpret_cast<nw4r::g3d::ScnObj*>(m_mdls[0]));
                    m_objs[OBJ_RULE_CONFIRM]->setAnimName(&sAnimConfirm[1], true);
                    m_objs[OBJ_STAGE_ICON]->setAnimName(&sAnimStageMode[1], true);
                    muTourData* data = getTourData();
                    for (u32 i = 0; i < (u32)m_entrants; i++) {
                        u8 picked = data->m_entrants[i].m_pickedCharacter;
                        m_slots[i].m_character = picked;
                        if (picked == 0x29) {
                            m_slots[i].m_color = 0;
                        }
                    }
                }
            } else if (pressed & PAD_B) {
                g_sndSystem->playSE(static_cast<SndID>(2), -1, 0, 0, -1);
                m_state = State_StageModeBack;
                scene->Insert(scene->sceneItemsCount, reinterpret_cast<nw4r::g3d::ScnObj*>(m_mdls[0]));
                m_objs[OBJ_RULE_CONFIRM]->setAnimName(&sAnimConfirm[1], true);
                m_objs[OBJ_STAGE_ICON]->setAnimName(&sAnimStageMode[1], true);
                muTourData* data = getTourData();
                for (u32 i = 0; i < (u32)m_entrants; i++) {
                    u8 picked = data->m_entrants[i].m_pickedCharacter;
                    m_slots[i].m_character = picked;
                    if (picked == 0x29) {
                        m_slots[i].m_color = 0;
                    }
                }
            }
            break;
        }
    } else {
        // A screen is changing: when its animation is over the next one is drawn.
        if (m_objs[OBJ_TITLE]->m_modelAnim->getFrame() >= 5.0f) {
            switch (m_state) {
            case State_Confirm:
                drawSelectConf(true);
                break;
            case State_SelectPlayers:
                if (m_playersPerMatch == 2) {
                    m_objs[OBJ_PLAYERS]->setFrameVisible(2.0f);
                } else if (m_playersPerMatch == 4) {
                    m_objs[OBJ_PLAYERS]->setFrameVisible(1.0f);
                } else {
                    m_objs[OBJ_PLAYERS]->setFrameVisible(3.0f);
                }
                m_objs[OBJ_ENTRANTS]->setFrameFlg((float)m_playersPerMatch, 6);
                m_objs[2]->setAnimName(&sAnimPlayers[m_playersPerMatch - 2], true);
                pushbackPanel(1);
                m_msg->printIndex(0, 0);
                m_msg->printIndex(1, 0x12);
                m_objs[2]->setFrameFlg(5.0f, 3);
                break;
            case State_SelectEntrants:
                drawSelectTotalPlayer(true);
                break;
            case State_ConfirmBack:
                drawSelectConf(true);
                m_objs[OBJ_TITLE]->setFrameVisible(0.0f);
                break;
            }
            m_transition = false;
        }
    }
}
