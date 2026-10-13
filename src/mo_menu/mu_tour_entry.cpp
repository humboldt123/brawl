#include <gf/gf_application.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_pad_system.h>
#include <gm/gm_global.h>
#include <memory.h>
#include <mu/mu_tour_entry.h>
#include <mu/mu_tour_util.h>
#include <nw4r/g3d/g3d_resmdl.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <snd/snd_system.h>
#include <stdio.h>
#include <string.h>
#include <types.h>

// main.dol functions with names that the shared headers do not declare.
// MuMsg::create(font setting, heap, heap): the third argument is not a HeapType in the symbol
extern "C" MuMsg* create__5MuMsgFUlQ25Heaps8HeapTypeUl(u32 fontSetting, Heaps::HeapType heap, u32 heap2);
// The random numbers of main.dol: an integer from 0 up to the argument
long randi(long max);
// CameraController::getInstance() and changeInput(kind)
extern "C" void* getInstance__16CameraControllerFv();
extern "C" void changeInput__16CameraControllerFQ216CameraController4Kind(void* camera, int kind);
// sndSystem::playSEwithFixEffect(id, volume?, pitch?) and sndSystem::isPlaySEID(id)
extern "C" void fn_80074ECC(sndSystem* system, int id, float volume, float pitch);
extern "C" int fn_80074DEC(sndSystem* system, int id);
// The system that keeps the picture of the screen while a menu changes
extern "C" void startKeepScreen__17gfKeepFrameBufferFv(void* keeper);
extern "C" void endKeepScreen__17gfKeepFrameBufferFv(void* keeper);
// muMenu: the numbers of the fighters and the narrations
extern "C" int exchangeMuSelchkind2GmCharacterKind__6muMenuFiii(int kind);
extern "C" int exchangeSelchkind2SelCharNarrationSndID__6muMenuFiii(int kind);
extern "C" void startRumbleController__6muMenuFiii(int pad, int length, int tag);
// The first costume of the picture of the fighter in the files of the faces (stock icon index)
extern "C" int fn_800AF6F0(int stock);
// Message::appendSubstr(dst, src, start, length) and Message::utf8to16(dst, src)
extern "C" void appendSubstr__7MessageFPcPCcii(char* dst, const char* src, int start, int length);
extern "C" void utf8to16__7MessageFPwPCc(wchar_t* dst, const char* src);
// GameGlobal: the settings of the rumble of a player and of a saved name
extern "C" int fn_8004D9A8(GameGlobal* global, int nameTag);
extern "C" int fn_8004DA94(GameGlobal* global, int player);
// The instance of the sound system that the menus ask for the music
extern void* lbl_805A01D8;
// nw4r::g3d::ScnMdlExpand::Construct(allocator, size, capacity, base model)
extern "C" nw4r::g3d::G3dObj* Construct__Q34nw4r3g3d12ScnMdlExpandFP12MEMAllocatorPUliPQ34nw4r3g3d6ScnMdl(MEMAllocator* allocator, u32* size,
                                                                                                         int capacity, void* scnMdl);

enum {
    PAD_A = 0x100,
    PAD_B = 0x200,
    PAD_X = 0x400,
    PAD_START = 0x1000,
    PAD_LEFT = 0x1,
    PAD_RIGHT = 0x2,
};

// The indexes of the objects of the task.
enum {
    OBJ_BG = 0,
    OBJ_INTRO = 1,              // the title of the screen
    OBJ_TITLE = 2,
    OBJ_ROUND = 3,              // the name of the round
    OBJ_ROUND_TEXT = 4,
    OBJ_PANEL = 5,              // the panel of each fighter (4)
    OBJ_SELECT = 9,             // the cursor on the pad of a fighter
    OBJ_BUTTONS = 10,           // the buttons of the screen
    OBJ_CHAR_A = 11,            // the picture of a fighter that is played by nobody (4)
    OBJ_CHAR_B = 15,            // the picture of a fighter that a CPU plays (4)
};

static inline void* getFrameKeeper() {
    return reinterpret_cast<char*>(g_gfApplication) + 0xD0;
}

// The number of the icon of the first costume of a fighter in the files of the faces.
static inline u32 getStockIconBase(int character) {
    int number = muMenu::exchangeGmCharacterKind2Something(muMenu::exchangeMuSelchkind2MuStockchkind(character));
    return (u32)(float)(number * 10 + 1);
}

muTourEntryTask::muTourEntryTask(int mode, int round, int match, int roundKind, int count, int nextCount, muTourTable* table,
                                 const muTourFighter* fighters, const muTourFighter* nextFighters)
    : gfTask("muTourEntryTask", Category_Info, 14, 6, true) {
    m_state = 0;
    m_result = 0;
    m_timer = 0;
    m_dialogPending = false;
    m_hasNext = false;
    m_resGame = nw4r::g3d::ResFile(static_cast<void*>(NULL));
    for (int i = 0; i < 8; i++) {
        m_resFace[i] = nw4r::g3d::ResFile(static_cast<void*>(NULL));
    }
    m_table = table;
    m_cursor = 0;
    m_count = count;
    m_nextCount = nextCount;
    m_round = round;
    m_match = match;
    m_roundKind = roundKind;
    m_taken = 0;
    m_skipped = 0;
    m_winner = 0;
    m_finalWinner = 0;
    m_mode = mode;
    unk2C_b1 = false;
    m_root = MenuRoot::create("TourMeleeTask", 0x100, "/menu/defaultcamera/CharacterSelect.brres");
    for (int i = 0; i < 0x13; i++) {
        m_objs[i] = NULL;
    }
    m_objProc = muObjProcessTask::create(Heaps::MenuInstance);
    m_objProc->m_objs = m_objs;
    m_objProc->m_count = 0x13;
    for (int i = 0; i < 12; i++) {
        m_loaded[i] = false;
    }
    m_sizeIndex = reinterpret_cast<muTourData*>(g_GameGlobal->m_tournamentData)->m_sizeIndex;
    for (u32 i = 0; i < (u32)m_count; i++) {
        m_fighters[i] = fighters[i];
    }
    memset(m_nextFighters, 0, sizeof(m_nextFighters));
    if (nextFighters != NULL) {
        for (u32 i = 0; i < (u32)m_nextCount; i++) {
            m_nextFighters[i] = nextFighters[i];
        }
        m_hasNext = true;
    }
    m_resFace[0] = nw4r::g3d::ResFile(static_cast<void*>(NULL));
    loadResource();
    m_msg = create__5MuMsgFUlQ25Heaps8HeapTypeUl(1, Heaps::MenuInstance, Heaps::MenuResource);
    m_msg->allocMsgBuf(0x100, 8);
    m_msgNames = NULL;
    fn_800B96E8(&m_ctrl, 0xF0);
    m_pad[0] = -1;
    m_pad[1] = -1;
    m_pad[2] = -1;
    m_pad[3] = -1;
}

muTourEntryTask::~muTourEntryTask() {
    if (m_resGame.ptr() != NULL) {
        gfHeapManager::free(m_resGame.ptr());
    }
    for (u32 i = 0; i < 8; i++) {
        if (m_resFace[i].ptr() != NULL) {
            gfHeapManager::free(m_resFace[i].ptr());
        }
    }
    for (u32 i = 0; i < 7; i++) {
        if (m_mdls[i] != NULL) {
            m_mdls[i]->Destroy();
            m_mdls[i] = NULL;
        }
    }
    for (u32 i = 0; i < 0x13; i++) {
        if (m_objs[i] != NULL) {
            delete m_objs[i];
            m_objs[i] = NULL;
        }
    }
    gfHeapManager::free(m_files[0].getBuffer());
    m_files[0].release();
    delete m_msg;
    delete m_msgNames;
    m_root->exit();
    m_root = NULL;
    m_objProc->exit();
    m_objProc = NULL;
}

// Starts to read the files of the screen: the texts, the models, and the pictures of the fighters (the ones of the
// next match are read as well when there is a next match).
void muTourEntryTask::loadResource() {
    m_files[0].readRequest("/menu/msg/tour_entry.msbin", Heaps::MenuResource, 0, 0);
    m_files[1].readRequest("/menu/tournament/TourGame.brres", Heaps::MenuResource, 0, 0);
    for (u32 i = 0; i < (u32)m_count; i++) {
        char name[64];
        sprintf(name, "/menu/common/char_bust_tex/MenSelchrFaceB%03d.brres", getStockIconBase(m_fighters[i].m_character));
        m_files[2 + i].readRequest(name, Heaps::MenuResource, 0, 0);
    }
    for (u32 i = m_count; i < 4; i++) {
        m_loaded[2 + i] = true;
    }
    if (m_hasNext) {
        for (u32 i = 0; i < (u32)m_nextCount; i++) {
            char name[64];
            sprintf(name, "/menu/common/char_bust_tex/MenSelchrFaceB%03d.brres", getStockIconBase(m_nextFighters[i].m_character));
            m_files[6 + i].readRequest(name, Heaps::MenuResource, 0, 0);
        }
        for (u32 i = m_nextCount; i < 4; i++) {
            m_loaded[6 + i] = true;
        }
    } else {
        m_loaded[6] = true;
        m_loaded[7] = true;
        m_loaded[8] = true;
        m_loaded[9] = true;
    }
}

// The names of the fighters of the match, and the number of the match.
void muTourEntryTask::setName() {
    for (u32 i = 0; i < (u32)m_count; i++) {
        tourPrintData(m_msgNames, i + 6, m_fighters[i].m_name, strlen(m_fighters[i].m_name));
    }
    if (m_roundKind == 0xB) {
        m_msg->printf(0, "");
    } else {
        char text[0x40];
        memset(text, 0, sizeof(text));
        int length = 0;
        int size;
        const char* first = m_msg->getMsgData(0, &size);
        memcpy(text, first, size);
        length = sprintf(text, "%s%d", text, m_match + 1);
        const char* second = m_msg->getMsgData(1, &size);
        memcpy(text + length, second, size);
        tourPrintData(m_msg, 0, text, length + size);
    }
}

// The picture that shows the controller of a pad at a fighter, and the text under it.
void muTourEntryTask::setCtrlFrame(MuObject* object, const gfPadStatus* pad, u32 padNo, u32 msgIndex) {
    object->setFrameTex((float)(padNo & 3));
    if (pad->m_error >= 0) {
        switch (pad->m_controllerType) {
        case gfPadType::WII_CLASSIC:
            object->setFrameVisible(1.0f);
            m_msg->printIndex(msgIndex, 8);
            break;
        case gfPadType::GCC:
            object->setFrameVisible(2.0f);
            m_msg->printIndex(msgIndex, 9);
            break;
        case gfPadType::WIIMOTE:
            object->setFrameVisible(3.0f);
            m_msg->printIndex(msgIndex, 10);
            break;
        case gfPadType::NUNCHUK:
            object->setFrameVisible(4.0f);
            m_msg->printIndex(msgIndex, 11);
            break;
        }
    } else {
        object->setFrameVisible(0.0f);
        m_msg->printf(msgIndex, "");
    }
}

// Sets the next match of the tournament (the fighters of the match and where it is in the table), and starts to read the
// pictures of its fighters.
void muTourEntryTask::setNextGame(int round, int match, int roundKind, int nextCount, const muTourFighter* nextFighters) {
    m_round = round;
    m_match = match;
    m_roundKind = roundKind;
    m_nextCount = nextCount;
    for (u32 i = 0; i < (u32)m_nextCount; i++) {
        memcpy(&m_nextFighters[i], &nextFighters[i], sizeof(muTourFighter));
        m_loaded[6 + i] = false;
        char name[64];
        sprintf(name, "/menu/common/char_bust_tex/MenSelchrFaceB%03d.brres", getStockIconBase(m_nextFighters[i].m_character));
        m_files[6 + i].readRequest(name, Heaps::MenuResource, 0, 0);
    }
    for (u32 i = m_nextCount; i < 4; i++) {
        m_loaded[6 + i] = true;
    }
    m_hasNext = true;
}

// A match with a fighter that has a bye (or all fighters are CPUs): the winner is the one that stays.
void muTourEntryTask::setDrawBye() {
    if (m_roundKind == 0xB) {
        m_result = Result_AutoFinal;
        m_winner = m_count;
        m_finalWinner = m_count;
        for (u32 i = 0; i < (u32)m_count; i++) {
            if (m_fighters[i].m_kind != 2) {
                m_winner = i;
                m_finalWinner = i;
            }
        }
        if ((u32)m_winner == (u32)m_count) {
            m_winner = 0;
            m_finalWinner = 0;
        }
        setFinalWinner(m_winner);
        g_sndSystem->playBGM(0x2703, 0, true);
    } else {
        m_result = Result_Auto;
        m_state = State_Table;
        m_objs[OBJ_INTRO + 1]->changeAnimN(1, 1, true, false);
        m_objs[OBJ_ROUND]->changeAnimN(1, 0x11, true, false);
        m_objs[OBJ_ROUND_TEXT]->changeAnimN(1, 0x13, true, false);
        m_objs[OBJ_ROUND_TEXT]->changeAnimN(1, 4, false, false);
        m_objs[OBJ_ROUND_TEXT]->setFrameTex(1.0f);
        m_winner = m_count;
        m_finalWinner = m_count;
        for (u32 i = 0; i < (u32)m_count; i++) {
            if (m_fighters[i].m_kind == 2) {
                m_objs[OBJ_PANEL + i]->changeAnimN(8, 0x11, true, false);
            } else {
                m_objs[OBJ_PANEL + i]->changeAnimN(8, 0x11, false, false);
                m_winner = i;
                m_finalWinner = i;
            }
        }
        if ((u32)m_winner == (u32)m_count) {
            m_winner = 0;
            m_finalWinner = 0;
            m_objs[OBJ_PANEL]->changeAnimN(8, 0x11, false, false);
        }
        tourPrintData(m_msgNames, 0, m_fighters[m_winner].m_name, strlen(m_fighters[m_winner].m_name));
    }
}

// The winner of the tournament is shown: its name letter by letter.
void muTourEntryTask::setFinalWinner(u32 winner) {
    m_state = State_FinalWinner;
    m_finalWinner = winner;
    m_timer = 0;
    m_voiceState = 0;
    m_objs[OBJ_INTRO + 1]->changeAnimN(3, 1, true, false);
    m_objs[OBJ_ROUND]->changeAnimN(3, 0x11, true, false);
    m_objs[OBJ_ROUND_TEXT]->changeAnimN(3, 0x13, true, false);
    for (u32 i = 0; i < (u32)m_count; i++) {
        if (i != winner) {
            m_objs[OBJ_PANEL + i]->changeAnimN(8, 0x11, true, false);
        } else {
            m_objs[OBJ_PANEL + i]->changeAnimN(9, 0x11, true, false);
        }
    }
    for (int i = 0; i < 5; i++) {
        char letter[8];
        memset(letter, 0, sizeof(letter));
        appendSubstr__7MessageFPcPCcii(letter, m_fighters[winner].m_name, i, 1);
        if (strlen(letter) == 0) {
            m_msgNames->printf(i + 1, "");
        } else {
            tourPrintData(m_msgNames, i + 1, letter, strlen(letter));
        }
    }
}

// Plays the narration of the winner of the tournament: the jingle first, then the voice of the fighter two seconds later.
static inline void playFinalNarration(muTourEntryTask* task) {
    if (task->m_voiceState == 1) {
        fn_80074ECC(g_sndSystem, 0x203A, 1.0f, 1.0f);
        task->m_voiceTimer = 0x78;
        task->m_voiceState = 2;
    } else if (task->m_voiceState > 0 && task->m_voiceState < 3) {
        int timer = task->m_voiceTimer;
        task->m_voiceTimer = timer - 1;
        if (timer < 1) {
            int sound = exchangeSelchkind2SelCharNarrationSndID__6muMenuFiii(task->m_fighters[task->m_finalWinner].m_character);
            fn_80074ECC(g_sndSystem, sound, 1.0f, 1.0f);
            task->m_voiceState = 0;
        }
    }
}

// Tells the game who plays the match: the fighters (a fighter that is left to chance gets one of its alternatives), the kind
// of each, the costume, the handicap, the name, and the pad that plays it.
void muTourEntryTask::setVsState() {
    gmSelCharData* selChar = g_GameGlobal->m_selCharData;
    reinterpret_cast<u8*>(selChar)[0x33] = 0;
    u8* kinds = reinterpret_cast<u8*>(selChar) + 10;
    gmPlayerInitData* player = selChar->m_playersInitData;
    for (u32 i = 0; i < (u32)m_count; i++, kinds += 4, player++) {
        muTourFighter* fighter = &m_fighters[i];
        *kinds = (u8)fighter->m_character;
        if (fighter->m_character == 0x1B) {
            const u8 alternatives[3] = {0x1C, 0x1D, 0x1E};
            *kinds = alternatives[randi(2)];
        }
        player->m_characterKind = static_cast<gmCharacterKind>(exchangeMuSelchkind2GmCharacterKind__6muMenuFiii(*kinds));
        u8* raw = reinterpret_cast<u8*>(player);
        switch (fighter->m_kind) {
        case 1:
            player->m_state = 1;
            raw[0x1F] = fighter->m_cpuLevel;
            break;
        case 0:
            player->m_state = 0;
            break;
        case 2:
            player->m_state = 3;
            break;
        }
        player->m_colorFileNo = fighter->m_color;
        *reinterpret_cast<u16*>(raw + 0x26) = fighter->m_handicap;
        wchar_t name[6];
        utf8to16__7MessageFPwPCc(name, fighter->m_name);
        raw[0x18] = fighter->m_nameTag;
        memcpy(&player->m_name, name, 0xC);
        raw[7] = m_pad[i] + 1;
        int rumble;
        if (fighter->m_nameTag == 0x78) {
            if (player->m_state == 0) {
                rumble = fn_8004DA94(g_GameGlobal, raw[7] - 1);
                raw[0x1B] = (raw[0x1B] & ~0x40) | ((rumble << 6) & 0x40);
            } else {
                raw[0x1B] = raw[0x1B] & ~0x40;
            }
        } else {
            rumble = fn_8004D9A8(g_GameGlobal, fighter->m_nameTag);
            raw[0x1B] = (raw[0x1B] & ~0x40) | ((rumble << 6) & 0x40);
        }
    }
    for (u32 i = m_count; i < 4; i++) {
        selChar->m_playersInitData[i].m_state = 3;
    }
}

// Sets the panel of a fighter up: the frame (the kind of the fighter), the picture of the fighter and the picture of its name.
static void setFighterPanel(muTourEntryTask* task, u32 i) {
    muTourFighter* fighter = &task->m_fighters[i];
    MuObject* panel = task->m_objs[OBJ_PANEL + i];
    if (fighter->m_kind == 1) {
        panel->setFrameVisible(1.0f);
        task->m_objs[OBJ_CHAR_B + i]->changeAnimN(0, 4, false, false);
        task->m_objs[OBJ_CHAR_B + i]->setFrameTex((float)(fighter->m_cpuLevel + 1));
    } else {
        panel->setFrameVisible(0.0f);
        task->m_objs[OBJ_CHAR_A + i]->changeAnimN(0, 6, false, false);
        if (fighter->m_kind == 2) {
            panel->changeAnimN(6, 0x11, false, false);
            panel->setFrameFlg(19.0f, 0x11);
        }
    }
    int character = fighter->m_character;
    u8 color = fighter->m_color;
    panel->setFrameTex((float)fn_800AF6F0(muMenu::exchangeMuSelchkind2MuStockchkind(character)) + 1.0f);
    char name[32];
    sprintf(name, "MenSelchrFaceB.%03d", color + getStockIconBase(character));
    panel->changeMaterialTex(panel->m_resMdl.GetResMat("charbust_mat")->m_id, name, &task->m_resFace[i]);
    sprintf(name, "MenSelchrChrNm.%03d", getStockIconBase(character));
    panel->changeMaterialTex(panel->m_resMdl.GetResMat("Card08")->m_id, name, NULL);
    panel->changeMaterialTex(panel->m_resMdl.GetResMat("charnameshadow_mat")->m_id, name, NULL);
}

// Makes the models of the screen, puts the texts on them, and starts at the state that the mode asks for.
void muTourEntryTask::initMdlData() {
    // The number of the model of each object (MenTrnGame<number>_TopN), their draw priorities, and the groups.
    const int modelNo[11] = {0, 1, 2, 3, 4, 10, 11, 12, 13, 15, 16};
    const int priority[11] = {0x10, 0x11, 0x12, 0x18, 0x1D, 0x1C, 0x1B, 0x1A, 0x19, 0x20, 0x20};
    const int groupBase[7] = {2, 3, 4, 5, 6, 7, 8};
    const int groupCapacity[7] = {1, 4, 1, 3, 3, 3, 3};
    const u8 textColor[8][3] = {{255, 234, 114}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}};

    for (u32 i = 0; i < 11; i++) {
        char name[64];
        sprintf(name, "MenTrnGame%04d_TopN", modelNo[i]);
        m_objs[i] = MuObject::create(&m_resGame, name, priority[i], NULL, Heaps::MenuInstance);
    }
    for (u32 i = 0; i < 4; i++) {
        m_objs[OBJ_CHAR_A + i] = MuObject::create(&m_resGame, "MenTrnGame0014_TopN", 0x1F, NULL, Heaps::MenuInstance);
        m_objs[OBJ_CHAR_B + i] = MuObject::create(&m_resGame, "MenTrnGame0017_TopN", 0x20, NULL, Heaps::MenuInstance);
    }
    MEMAllocator* allocator = gfHeapManager::getMEMAllocator(Heaps::MenuInstance);
    for (u32 i = 0; i < 7; i++) {
        u32 size;
        m_mdls[i] = (nw4r::g3d::ScnGroup*)Construct__Q34nw4r3g3d12ScnMdlExpandFP12MEMAllocatorPUliPQ34nw4r3g3d6ScnMdl(
            allocator, &size, groupCapacity[i], m_objs[groupBase[i]]->m_scnMdl);
    }
    tourPushBack(m_mdls[0], reinterpret_cast<nw4r::g3d::ScnMdl*>(m_table->m_group), "pos00");
    const int firstPos[3] = {0, 2, 5};
    for (u32 i = 0; i < (u32)m_count; i++) {
        char node[8];
        sprintf(node, "pos%02d", i + firstPos[m_count - 2]);
        tourPushBack(m_mdls[1], reinterpret_cast<nw4r::g3d::ScnMdl*>(m_mdls[3 + i]), node);
    }
    for (u32 i = 0; i < (u32)m_count; i++) {
        if (m_fighters[i].m_kind == 1) {
            tourPushBack(m_mdls[3 + i], m_objs[OBJ_CHAR_B + i]->m_scnMdl, "pos02");
        } else {
            tourPushBack(m_mdls[3 + i], m_objs[OBJ_CHAR_A + i]->m_scnMdl, "pos00");
        }
    }
    tourAttach(m_root, m_objs[0]->m_scnMdl);
    m_objs[0]->changeAnimN(0, 0x11, true, false);
    m_objs[OBJ_ROUND]->changeAnimN(0, 2, false, false);
    m_objs[OBJ_ROUND]->setFrameVisible((float)m_count - 2.0f);
    m_objs[OBJ_ROUND_TEXT]->changeAnimN(0, 4, false, false);
    if (m_roundKind == 10) {
        m_objs[OBJ_ROUND_TEXT]->setFrameTex(4.0f);
    } else if (m_roundKind == 0xB) {
        m_objs[OBJ_ROUND_TEXT]->setFrameTex(5.0f);
    } else {
        m_objs[OBJ_ROUND_TEXT]->setFrameTex((float)m_round);
    }
    for (u32 i = 0; i < 4; i++) {
        m_objs[OBJ_PANEL + i]->changeAnimN(10, 6, false, false);
    }
    for (u32 i = 0; i < (u32)m_count; i++) {
        setFighterPanel(this, i);
    }
    m_objs[OBJ_BUTTONS]->changeAnimN(0, 0x10, false, false);

    // The texts
    m_msg->setMsgData(m_files[0].getBuffer());
    m_msg->attachScnMdlSimple(0, m_objs[OBJ_ROUND_TEXT]->m_sceneModel, 1, 1.0f / 16.0f);
    m_msg->attachScnMdlSimple(1, m_objs[OBJ_BUTTONS]->m_sceneModel, 0, 1.0f / 16.0f);
    m_msg->getWScale(1, 48.0f);
    m_msg->attachScnMdlSimple(2, m_objs[OBJ_BUTTONS]->m_sceneModel, 1, 1.0f / 16.0f);
    m_msg->attachScnMdlSimple(3, m_objs[OBJ_BUTTONS]->m_sceneModel, 2, 1.0f / 16.0f);
    for (int i = 0; i < 4; i++) {
        m_msg->attachScnMdlSimple(i + 4, m_objs[OBJ_PANEL + i]->m_sceneModel, 1, 1.0f / 16.0f);
    }
    for (u32 i = 0; i < 8; i++) {
        m_msg->setAlignMode(i, MuMsg::Align_Center);
        m_msg->setFontWidthModeAuto(i);
        m_msg->setFontColor(i, textColor[i][0], textColor[i][1], textColor[i][2], 0xFF);
    }
    m_msgNames = create__5MuMsgFUlQ25Heaps8HeapTypeUl(2, Heaps::MenuInstance, Heaps::MenuResource);
    m_msgNames->allocMsgBuf(0x40, 10);
    m_msgNames->attachScnMdlSimple(0, m_objs[OBJ_ROUND_TEXT]->m_sceneModel, 0, 1.0f / 16.0f);
    for (int i = 0; i < 5; i++) {
        m_msgNames->attachScnMdlSimple(i + 1, m_objs[OBJ_ROUND_TEXT]->m_sceneModel, i + 2, 1.0f / 16.0f);
    }
    for (int i = 0; i < 4; i++) {
        m_msgNames->attachScnMdlSimple(i + 6, m_objs[OBJ_PANEL + i]->m_sceneModel, 0, 1.0f / 16.0f);
    }
    for (u32 i = 0; i < 10; i++) {
        m_msgNames->setAlignMode(i, MuMsg::Align_Center);
        m_msgNames->setFontWidthModeAuto(i);
        m_msgNames->setFontType(i, 1);
        m_msgNames->setFontColor(i, 0xFF, 0xFF, 0xFF, 0xFF);
    }

    // The state the screen starts at
    switch (m_mode) {
    case 1:
        tourAttach(m_root, reinterpret_cast<nw4r::g3d::ScnMdl*>(m_mdls[0]));
        tourAttach(m_root, reinterpret_cast<nw4r::g3d::ScnMdl*>(m_mdls[1]));
        tourAttach(m_root, reinterpret_cast<nw4r::g3d::ScnMdl*>(m_mdls[2]));
        m_objs[OBJ_INTRO + 1]->changeAnimN(1, 1, true, false);
        m_objs[OBJ_ROUND]->changeAnimN(1, 0x11, true, false);
        m_objs[OBJ_ROUND_TEXT]->changeAnimN(1, 0x13, true, false);
        m_objs[OBJ_ROUND_TEXT]->changeAnimN(1, 4, false, false);
        m_objs[OBJ_ROUND_TEXT]->setFrameTex(0.0f);
        m_state = State_Table;
        for (u32 i = 0; i < (u32)m_count; i++) {
            if (m_fighters[i].m_kind == 2) {
                m_objs[OBJ_PANEL + i]->changeAnimN(8, 0x11, true, false);
            } else if (reinterpret_cast<u8*>(g_GameGlobal->m_resultInfo)[0x33 + i * 0x2AC] == 0) {
                m_objs[OBJ_PANEL + i]->changeAnimN(8, 0x11, false, false);
                tourPrintData(m_msgNames, 0, m_fighters[i].m_name, strlen(m_fighters[i].m_name));
            } else {
                m_objs[OBJ_PANEL + i]->changeAnimN(8, 0x11, true, false);
                m_finalWinner = i;
            }
        }
        break;
    case 0:
        tourAttach(m_root, m_objs[OBJ_INTRO]->m_scnMdl);
        m_objs[OBJ_INTRO]->changeAnimN(0, 0x1B, true, false);
        m_state = State_Appear;
        break;
    case 2:
        tourAttach(m_root, reinterpret_cast<nw4r::g3d::ScnMdl*>(m_mdls[0]));
        tourAttach(m_root, reinterpret_cast<nw4r::g3d::ScnMdl*>(m_mdls[1]));
        tourAttach(m_root, reinterpret_cast<nw4r::g3d::ScnMdl*>(m_mdls[2]));
        m_objs[OBJ_ROUND_TEXT]->changeAnimN(1, 4, false, false);
        m_objs[OBJ_ROUND_TEXT]->setFrameTex(0.0f);
        for (u32 i = 0; i < (u32)m_count; i++) {
            if (m_fighters[i].m_kind != 2 && reinterpret_cast<u8*>(g_GameGlobal->m_resultInfo)[0x33 + i * 0x2AC] == 0) {
                m_finalWinner = i;
            }
        }
        setFinalWinner(m_finalWinner);
        break;
    }
    setName();
}

// The number of fighters of the match that nobody plays (they have a bye).
static inline u32 countByes(muTourEntryTask* task) {
    u32 count = 0;
    for (int i = task->m_count; i != 0; i--) {
        if (task->m_fighters[task->m_count - i].m_kind == 2) {
            count++;
        }
    }
    return count;
}

// The buttons of the confirmation show up: "start the match" and "decide it by chance".
static void showConfirm(muTourEntryTask* task) {
    task->m_cursor = 1;
    tourAttach(task->m_root, task->m_objs[OBJ_BUTTONS]->m_scnMdl);
    task->m_objs[OBJ_BUTTONS]->changeAnimN(0, 3, true, false);
    task->m_objs[OBJ_BUTTONS]->changeAnimN(0, 0x10, false, false);
    task->m_objs[OBJ_BUTTONS]->setFrameMatCol(1.0f);
    task->m_msg->printIndex(1, 5);
    task->m_msg->printIndex(2, 6);
    task->m_msg->printIndex(3, 7);
    fn_800B96E8(&task->m_ctrl, 0xF0);
    task->m_state = muTourEntryTask::State_Confirm;
}

// The pad conversion of the Wii Remote and the Nunchuk: their buttons are made to look like the ones of the GameCube pad.
static inline void convertPad(gfPadStatus* status) {
    u32* pressed = &status->m_buttonsPressedThisFrame.bits;
    if (status->m_controllerType == gfPadType::NUNCHUK) {
        u32 bits = *pressed & 0x10000;
        *pressed = *pressed & ~0x400;
        if (bits != 0) {
            *pressed |= PAD_X;
        }
    } else if (status->m_controllerType < 3 && status->m_controllerType > 1) {
        if (*pressed & 0x200000) {
            *pressed |= PAD_X;
        }
    }
}

// The main function of the screen: every state reads the pads and moves on when it should.
void muTourEntryTask::processAnim() {
    gfPadStatus status;
    g_gfPadSystem->getSysPadStatus(0xF0, &status);
    status.clamp();
    u32 pressed = status.m_buttonsPressedThisFrame.bits;
    u32 repeat = status.m_buttonsPressedThisFrame2.bits;
    nw4r::g3d::ScnGroup* scene = m_root->scene;

    if (m_dialogPending && m_objs[OBJ_BUTTONS]->m_modelAnim->m_anmObjVisRes->GetFrame() >= 4.0f) {
        m_dialogPending = false;
        scene->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[OBJ_BUTTONS]->m_scnMdl));
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
        if (!m_loaded[11]) {
            if (!m_table->isLoadEnd()) {
                break;
            }
            m_loaded[11] = true;
        }
        for (u32 i = 0; i < 6; i++) {
            if (!m_loaded[i] && m_files[i].m_request != NULL) {
                if (!m_files[i].isReady()) {
                    return;
                }
                m_loaded[i] = true;
            }
        }
        m_resGame = nw4r::g3d::ResFile(m_files[1].getBuffer());
        nw4r::g3d::ResFile::Init(&m_resGame);
        m_files[1].release();
        for (u32 i = 0; i < (u32)m_count; i++) {
            gfFileIOHandle* file = &m_files[2 + i];
            m_resFace[i] = nw4r::g3d::ResFile(file->getBuffer());
            nw4r::g3d::ResFile::Init(&m_resFace[i]);
            file->release();
        }
        switch (m_mode) {
        case 1:
        case 0:
            g_sndSystem->playBGM(0x2702, 0, true);
            break;
        case 2:
            g_sndSystem->playBGM(0x2703, 0, true);
            break;
        }
        *reinterpret_cast<int*>(reinterpret_cast<char*>(lbl_805A01D8) + 0x174) = 0x2D;
        initMdlData();
        endKeepScreen__17gfKeepFrameBufferFv(getFrameKeeper());
        break;
    }
    case State_Appear:
        if ((pressed & PAD_A) || m_objs[OBJ_INTRO]->isNodeAnimFinished()) {
            m_state = State_Title;
            m_objs[OBJ_BG]->setFrameFlg(119.0f, 0x11);
            m_objs[OBJ_INTRO]->changeAnimN(1, 0x1B, true, false);
        }
        break;
    case State_Title:
        if ((pressed & PAD_A) || (pressed & PAD_START)) {
            g_sndSystem->playSE(static_cast<SndID>(1), -1, 0, 0, -1);
            g_sndSystem->playSE(static_cast<SndID>(0x16), -1, 0, 0, -1);
            m_state = State_TitleOut;
            m_timer = 0;
            m_objs[OBJ_INTRO]->changeAnimN(2, 0x1B, true, false);
            tourAttach(m_root, reinterpret_cast<nw4r::g3d::ScnMdl*>(m_mdls[0]));
            tourAttach(m_root, reinterpret_cast<nw4r::g3d::ScnMdl*>(m_mdls[1]));
            tourAttach(m_root, reinterpret_cast<nw4r::g3d::ScnMdl*>(m_mdls[2]));
            m_objs[OBJ_TITLE]->changeAnimN(0, 1, true, false);
            m_objs[OBJ_ROUND]->changeAnimN(0, 0x11, true, false);
            m_objs[OBJ_ROUND_TEXT]->changeAnimN(0, 0x13, true, false);
        }
        break;
    case State_TitleOut:
        if (m_objs[OBJ_INTRO]->isNodeAnimFinished()) {
            scene->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[OBJ_INTRO]->m_scnMdl));
            m_state = State_Wait;
        }
        // fall through
    case State_Wait:
        m_timer++;
        if ((pressed & PAD_A) || (pressed & PAD_START) || (u32)m_timer > 0x96) {
            if ((pressed & PAD_A) || (pressed & PAD_START)) {
                g_sndSystem->playSE(static_cast<SndID>(1), -1, 0, 0, -1);
            }
            m_objs[OBJ_TITLE]->changeAnimN(2, 1, true, false);
            m_objs[OBJ_ROUND]->changeAnimN(2, 0x11, true, false);
            m_objs[OBJ_ROUND_TEXT]->changeAnimN(2, 0x13, true, false);
            m_table->m_playersPerMatch = m_count;
            m_table->moveTable(m_round, m_match, false);
            m_timer = 0;
            m_state = State_ShowMatch;
        }
        break;
    case State_Table: {
        m_table->winnerCalc();
        m_timer++;
        for (u32 i = 0; i < (u32)m_nextCount; i++) {
            if (!m_loaded[6 + i] && m_files[6 + i].m_request != NULL) {
                gfFileIOHandle* file = &m_files[6 + i];
                if (!file->isReady()) {
                    return;
                }
                m_loaded[6 + i] = true;
                m_resFace[4 + i] = nw4r::g3d::ResFile(file->getBuffer());
                nw4r::g3d::ResFile::Init(&m_resFace[4 + i]);
                file->release();
            }
        }
        if ((u32)m_timer > 0xB4) {
            // The next match: its fighters take the place of the fighters of this one.
            m_count = m_nextCount;
            m_table->m_playersPerMatch = m_nextCount;
            for (u32 i = 0; i < (u32)m_count; i++) {
                if (m_resFace[i].ptr() != NULL) {
                    gfHeapManager::free(m_resFace[i].ptr());
                    m_resFace[i] = nw4r::g3d::ResFile(static_cast<void*>(NULL));
                }
                m_resFace[i] = m_resFace[4 + i];
                m_resFace[4 + i] = nw4r::g3d::ResFile(static_cast<void*>(NULL));
                memcpy(&m_fighters[i], &m_nextFighters[i], sizeof(muTourFighter));
            }
            m_objs[OBJ_ROUND]->setFrameVisible((float)m_count - 2.0f);
            tourClearGroup(m_mdls[1]);
            const int firstPos[5] = {0, 0, 0, 2, 5};
            for (u32 i = 0; i < (u32)m_count; i++) {
                char node[8];
                sprintf(node, "pos%02d", i + firstPos[m_count]);
                tourPushBack(m_mdls[1], reinterpret_cast<nw4r::g3d::ScnMdl*>(m_mdls[3 + i]), node);
                m_mdls[3 + i]->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[OBJ_CHAR_A + i]->m_scnMdl));
                m_mdls[3 + i]->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[OBJ_CHAR_B + i]->m_scnMdl));
                if (m_fighters[i].m_kind == 1) {
                    tourPushBack(m_mdls[3 + i], m_objs[OBJ_CHAR_B + i]->m_scnMdl, "pos02");
                } else {
                    tourPushBack(m_mdls[3 + i], m_objs[OBJ_CHAR_A + i]->m_scnMdl, "pos00");
                }
                setFighterPanel(this, i);
                m_objs[OBJ_PANEL + i]->changeAnimN(0, 0x11, m_fighters[i].m_kind != 1, false);
            }
            m_objs[OBJ_ROUND]->changeAnimN(2, 0x11, true, false);
            m_objs[OBJ_ROUND_TEXT]->changeAnimN(0, 4, false, false);
            if (m_roundKind == 10) {
                m_objs[OBJ_TITLE]->changeAnimN(2, 1, true, false);
                m_objs[OBJ_ROUND_TEXT]->setFrameTex(4.0f);
            } else if (m_roundKind == 0xB) {
                m_objs[OBJ_TITLE]->changeAnimN(5, 1, true, false);
                m_objs[OBJ_ROUND_TEXT]->setFrameTex(5.0f);
            } else {
                m_objs[OBJ_TITLE]->changeAnimN(2, 1, true, false);
                m_objs[OBJ_ROUND_TEXT]->setFrameTex((float)m_round);
            }
            setName();
            m_objs[OBJ_ROUND_TEXT]->changeAnimN(2, 0x13, true, false);
            m_table->moveTable(m_round, m_match, false);
            m_table->m_playersPerMatch = m_count;
            m_timer = 0;
            m_state = State_ShowMatch;
        }
        break;
    }
    case State_ShowMatch: {
        m_timer++;
        if ((u32)m_timer < 5 && fn_80074DEC(g_sndSystem, 0x1B) == 0) {
            g_sndSystem->playSE(static_cast<SndID>(0x1B), -1, 0, 0, -1);
        }
        if ((u32)m_timer < 0x14) {
            m_table->calcTablePos();
        } else if (m_timer == 0x14) {
            m_table->moveTable(m_round, m_match, true);
            int slots[4];
            for (u32 i = 0; i < (u32)m_count; i++) {
                slots[i] = m_fighters[i].m_slot;
            }
            m_table->entrySet(slots, m_count, 0x14);
        } else if ((u32)m_timer < 0x29) {
            m_table->entryCalc();
        }
        if ((pressed & PAD_A) || (pressed & PAD_START) || m_objs[OBJ_TITLE]->isNodeAnimFinished()) {
            if ((pressed & PAD_A) || (pressed & PAD_START)) {
                g_sndSystem->playSE(static_cast<SndID>(1), -1, 0, 0, -1);
                m_objs[OBJ_TITLE]->setFrameFlg(90.0f, 1);
                m_objs[OBJ_ROUND]->setFrameFlg(90.0f, 0x11);
                m_objs[OBJ_ROUND_TEXT]->setFrameFlg(90.0f, 0x13);
                int slots[4];
                for (u32 i = 0; i < (u32)m_count; i++) {
                    slots[i] = m_fighters[i].m_slot;
                }
                m_table->entrySet(slots, m_count, 0x14);
                m_table->entryEnd();
                m_table->moveTable(m_round, m_match, true);
            }
            // Which of the fighters are played by players, which by CPUs, and which have a bye.
            u32 cpus = 0;
            m_taken = 0;
            bool noOne = true;
            m_skipped = 0;
            bool noPlayer = true;
            m_curSlot = 0;
            bool first = true;
            for (u32 i = 0; i < (u32)m_count; i++) {
                m_pad[i] = -1;
                switch (m_fighters[i].m_kind) {
                case 1:
                    noOne = false;
                    m_taken |= 1 << i;
                    cpus++;
                    m_objs[OBJ_PANEL + i]->changeAnimN(0, 0x11, true, false);
                    break;
                case 0:
                    noPlayer = false;
                    noOne = false;
                    if (first) {
                        m_curSlot = i;
                        first = false;
                        m_objs[OBJ_PANEL + i]->changeAnimN(2, 0x11, true, false);
                    } else {
                        m_objs[OBJ_PANEL + i]->changeAnimN(0, 0x11, true, false);
                    }
                    break;
                case 2:
                    m_taken |= 1 << i;
                    m_skipped |= 1 << i;
                    break;
                }
            }
            if (noOne) {
                setDrawBye();
            } else if (noPlayer) {
                if (cpus == 1) {
                    setDrawBye();
                } else {
                    showConfirm(this);
                }
            } else {
                m_state = State_WaitPad;
            }
        }
        break;
    }
    case State_PickPad:
        if (m_objs[OBJ_SELECT]->isNodeAnimFinished()) {
            m_mdls[3 + m_prevSlot]->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[OBJ_SELECT]->m_scnMdl));
            m_state = State_PickPadAnim;
        }
        // fall through
    case State_PickPadAnim:
        if (m_objs[OBJ_PANEL + m_curSlot]->isNodeAnimFinished()) {
            m_objs[OBJ_PANEL + m_curSlot]->changeAnimN(2, 0x11, true, false);
            m_state = State_WaitPad;
        }
        // fall through
    case State_WaitPad: {
        for (int pad = 0; pad < 8; pad++) {
            gfPadStatus padStatus;
            g_gfPadSystem->getSysPadStatus(pad, &padStatus);
            u32 padPressed = padStatus.m_buttonsPressedThisFrame.bits;
            if ((padPressed & PAD_A) || (padPressed & PAD_START)) {
                // A pad joins in: it plays the fighter, unless it already plays another one.
                bool used = false;
                for (int i = 0; i < m_count; i++) {
                    if (pad == m_pad[i] && m_fighters[i].m_kind != 2) {
                        used = true;
                        break;
                    }
                }
                if (!used) {
                    g_sndSystem->playSE(static_cast<SndID>(1), -1, 0, 0, -1);
                    m_pad[m_curSlot] = pad;
                    setCtrlFrame(m_objs[OBJ_CHAR_A + m_curSlot], &padStatus, pad, m_curSlot + 4);
                    if (countByes(this) == (u32)(m_count - 1)) {
                        m_mdls[3 + m_curSlot]->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[OBJ_SELECT]->m_scnMdl));
                        setDrawBye();
                    } else {
                        m_state = State_PadChosen;
                        m_objs[OBJ_PANEL + m_curSlot]->changeAnimN(3, 0x11, true, false);
                    }
                    break;
                }
            } else if (padPressed & PAD_B) {
                // B takes the pad off the fighter it was on before.
                u32 slot = m_curSlot;
                u32 order[3];
                u32 count = m_count - 1;
                for (u32 k = 0; k < count; k++) {
                    bool wrap = slot == 0;
                    slot--;
                    if (wrap) {
                        slot = count;
                    }
                    order[k] = slot;
                }
                for (u32 k = 0; k < count; k++) {
                    u32 other = order[k];
                    if (pad == m_pad[other]) {
                        g_sndSystem->playSE(static_cast<SndID>(2), -1, 0, 0, -1);
                        m_objs[OBJ_PANEL + m_curSlot]->changeAnimN(0, 0x11, true, false);
                        m_state = State_PadChosen;
                        m_curSlot = other;
                        setCtrlFrame(m_objs[OBJ_CHAR_A + other], &padStatus, pad, other + 4);
                        m_objs[OBJ_PANEL + m_curSlot]->changeAnimN(7, 0x11, true, false);
                        m_fighters[m_curSlot].m_kind = 0;
                        m_taken ^= 1 << m_curSlot;
                        break;
                    }
                }
                if (m_state != State_WaitPad) {
                    break;
                }
            }
        }
        break;
    }
    case State_PadTaken:
        if (m_objs[OBJ_SELECT]->isNodeAnimFinished()) {
            m_mdls[3 + m_prevSlot]->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[OBJ_SELECT]->m_scnMdl));
            m_mdls[3 + m_curSlot]->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[OBJ_SELECT]->m_scnMdl));
            m_state = State_PadFrame;
        }
        // fall through
    case State_PadFrame: {
        gfPadStatus padStatus;
        g_gfPadSystem->getSysPadStatus(m_pad[m_curSlot], &padStatus);
        setCtrlFrame(m_objs[OBJ_CHAR_A + m_curSlot], &padStatus, m_pad[m_curSlot], m_curSlot + 4);
        g_gfPadSystem->getSysPadStatus(m_pad[m_curSlot], &padStatus);
        convertPad(&padStatus);
        u32 padPressed = padStatus.m_buttonsPressedThisFrame.bits;
        if (padStatus.m_error < 0) {
            m_pad[m_curSlot] = -1;
            m_state = State_WaitPad;
            m_objs[OBJ_PANEL + m_curSlot]->changeAnimN(2, 0x11, true, false);
        } else if (!(padPressed & PAD_A) && !(padPressed & PAD_START)) {
            if (padPressed & PAD_B) {
                g_sndSystem->playSE(static_cast<SndID>(2), -1, 0, 0, -1);
                m_pad[m_curSlot] = -1;
                m_state = State_WaitPad;
                m_objs[OBJ_PANEL + m_curSlot]->changeAnimN(2, 0x11, true, false);
            } else if (padPressed & PAD_X) {
                if (countByes(this) < (u32)(m_count - 1)) {
                    g_sndSystem->playSE(static_cast<SndID>(1), -1, 0, 0, -1);
                    m_state = State_PadAsk;
                    m_cursor = 1;
                    m_objs[OBJ_SELECT]->changeAnimN(3, 0x10, true, false);
                    m_objs[OBJ_SELECT]->changeAnimN(0, 3, true, false);
                    tourPushBack(m_mdls[3 + m_curSlot], m_objs[OBJ_SELECT]->m_scnMdl, "pos01");
                    fn_800B96E8(&m_ctrl, m_pad[m_curSlot] & 0xFF);
                }
            }
        } else {
            // The pad is ready: the fighter is taken, and the rumble tells the player.
            g_sndSystem->playSE(static_cast<SndID>(1), -1, 0, 0, -1);
            int tag = m_fighters[m_curSlot].m_nameTag;
            if (tag == 0x78) {
                tag = -1;
            }
            startRumbleController__6muMenuFiii(m_pad[m_curSlot], 0x1E, tag);
            m_objs[OBJ_PANEL + m_curSlot]->changeAnimN(5, 0x11, true, false);
            const u32 fullMask[5] = {0, 0, 3, 7, 0xF};
            m_taken |= 1 << m_curSlot;
            m_skipped &= ~(1 << m_curSlot);
            if (m_taken == fullMask[m_count]) {
                if (fn_80074DEC(g_sndSystem, 0x1E) == 0) {
                    g_sndSystem->playSE(static_cast<SndID>(0x1E), -1, 0, 0, -1);
                }
                m_state = State_Bye;
            } else {
                m_prevSlot = m_curSlot;
                int next = (m_curSlot + 1) % m_count;
                while (m_taken & (1 << next)) {
                    next = (next + 1) % m_count;
                }
                m_curSlot = next;
                m_objs[OBJ_PANEL + next]->changeAnimN(1, 0x11, true, false);
                m_state = State_PickPadAnim;
            }
        }
        break;
    }
    case State_PadChosen:
        m_objs[OBJ_PANEL + m_curSlot]->changeAnimN(4, 0x11, true, false);
        m_state = State_PadFrame;
        break;
    case State_PadAsk: {
        gfPadStatus padStatus;
        g_gfPadSystem->getSysPadStatus(m_pad[m_curSlot], &padStatus);
        padStatus.clamp();
        fn_800B9984(&m_ctrl);
        u32 padPressed = padStatus.m_buttonsPressedThisFrame.bits;
        u32 padRepeat = padStatus.m_buttonsPressedThisFrame2.bits | *reinterpret_cast<u32*>(reinterpret_cast<char*>(&m_ctrl) + 0x10);
        if (padStatus.m_error < 0) {
            m_objs[OBJ_SELECT]->changeAnimN(1, 3, true, false);
            m_state = State_PadTaken;
        } else if (!((padRepeat & PAD_LEFT) && m_cursor == 1)) {
            if (!((padRepeat & PAD_RIGHT) && m_cursor == 0)) {
                if (!(padPressed & PAD_A) && !(padPressed & PAD_START)) {
                    if (padPressed & PAD_B) {
                        g_sndSystem->playSE(static_cast<SndID>(2), -1, 0, 0, -1);
                        m_objs[OBJ_SELECT]->changeAnimN(1, 3, true, false);
                        m_state = State_PadTaken;
                    }
                } else {
                    g_sndSystem->playSE(static_cast<SndID>(1), -1, 0, 0, -1);
                    if (m_cursor == 0) {
                        // The fighter is left to nobody: it has a bye.
                        u32 bit = 1 << m_curSlot;
                        m_taken |= bit;
                        m_skipped |= bit;
                        m_fighters[m_curSlot].m_kind = 2;
                        m_objs[OBJ_SELECT]->changeAnimN(1, 3, true, false);
                        const u32 fullMask[5] = {0, 0, 3, 7, 0xF};
                        if (m_taken == fullMask[m_count]) {
                            u32 byes = 0;
                            u32 cpus = 0;
                            for (int i = m_count; i != 0; i--) {
                                int kind = m_fighters[m_count - i].m_kind;
                                if (kind == 2) {
                                    byes++;
                                } else if (kind == 1) {
                                    cpus++;
                                }
                            }
                            if (byes == (u32)(m_count - 1)) {
                                m_mdls[3 + m_curSlot]->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[OBJ_SELECT]->m_scnMdl));
                                setDrawBye();
                            } else if ((u32)m_count == byes + cpus) {
                                if (cpus == 1) {
                                    m_mdls[3 + m_curSlot]->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[OBJ_SELECT]->m_scnMdl));
                                    setDrawBye();
                                } else {
                                    m_objs[OBJ_PANEL + m_curSlot]->changeAnimN(6, 0x11, true, false);
                                    m_objs[OBJ_CHAR_A + m_curSlot]->setFrameVisible(0.0f);
                                    m_mdls[3 + m_prevSlot]->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[OBJ_SELECT]->m_scnMdl));
                                    showConfirm(this);
                                }
                            } else {
                                m_objs[OBJ_PANEL + m_curSlot]->changeAnimN(6, 0x11, true, false);
                                m_objs[OBJ_CHAR_A + m_curSlot]->setFrameVisible(0.0f);
                                m_state = State_Bye;
                            }
                        } else {
                            m_objs[OBJ_PANEL + m_curSlot]->changeAnimN(6, 0x11, true, false);
                            m_objs[OBJ_CHAR_A + m_curSlot]->setFrameVisible(0.0f);
                            m_prevSlot = m_curSlot;
                            int next = (m_curSlot + 1) % m_count;
                            while (m_taken & (1 << next)) {
                                next = (next + 1) % m_count;
                            }
                            m_curSlot = next;
                            m_objs[OBJ_PANEL + next]->changeAnimN(2, 0x11, true, false);
                            m_state = State_PickPad;
                        }
                    } else {
                        m_objs[OBJ_SELECT]->changeAnimN(1, 3, true, false);
                        m_state = State_PadTaken;
                    }
                }
            } else {
                g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
                m_objs[OBJ_SELECT]->changeAnimN(3, 0x10, true, false);
                m_cursor = 1;
            }
        } else {
            g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
            m_objs[OBJ_SELECT]->changeAnimN(2, 0x10, true, false);
            m_cursor = 0;
        }
        break;
    }
    case State_Confirm: {
        fn_800B9984(&m_ctrl);
        repeat |= *reinterpret_cast<u32*>(reinterpret_cast<char*>(&m_ctrl) + 0x10);
        if ((repeat & PAD_LEFT) && m_cursor == 1) {
            g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
            m_cursor = 0;
            m_objs[OBJ_BUTTONS]->setFrameMatCol(0.0f);
        } else if ((repeat & PAD_RIGHT) && m_cursor == 0) {
            g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
            m_cursor = 1;
            m_objs[OBJ_BUTTONS]->setFrameMatCol(1.0f);
        } else if ((pressed & PAD_A) || (pressed & PAD_START)) {
            g_sndSystem->playSE(static_cast<SndID>(1), -1, 0, 0, -1);
            if (m_cursor == 0) {
                // The match starts.
                setVsState();
                m_state = State_Leave;
                m_result = Result_Start;
                startKeepScreen__17gfKeepFrameBufferFv(getFrameKeeper());
            } else {
                // The match is decided by chance: the stronger a CPU, the better its chance.
                u32 weights[4];
                u32 total = 0;
                u32 i;
                for (i = 0; i < (u32)m_count; i++) {
                    if (m_fighters[i].m_kind == 1) {
                        u32 weight = m_fighters[i].m_cpuLevel + 1;
                        weights[i] = weight;
                        total = (total + weight) & 0xFF;
                    } else {
                        weights[i] = 0;
                    }
                }
                int roll = randi(total);
                u32 pick = 0;
                u32 index = 0;
                for (int left = m_count; left != 0; left--, index++) {
                    pick = 0;
                    u32 weight = weights[index];
                    if (weight != 0) {
                        pick = index;
                        if (roll <= (int)weight) {
                            break;
                        }
                        roll -= weight;
                    }
                }
                m_dialogPending = true;
                m_objs[OBJ_BUTTONS]->changeAnimN(1, 3, true, false);
                if (m_roundKind == 0xB) {
                    m_winner = pick;
                    m_result = Result_AutoFinal;
                    setFinalWinner(pick);
                    g_sndSystem->playBGM(0x2703, 0, true);
                } else {
                    m_result = Result_Auto;
                    m_state = State_Table;
                    m_objs[OBJ_TITLE]->changeAnimN(1, 1, true, false);
                    m_objs[OBJ_ROUND]->changeAnimN(1, 0x11, true, false);
                    m_objs[OBJ_ROUND_TEXT]->changeAnimN(1, 0x13, true, false);
                    m_objs[OBJ_ROUND_TEXT]->changeAnimN(1, 4, false, false);
                    m_objs[OBJ_ROUND_TEXT]->setFrameTex(0.0f);
                    for (u32 k = 0; k < (u32)m_count; k++) {
                        if (k == pick) {
                            m_winner = k;
                            m_objs[OBJ_PANEL + k]->changeAnimN(8, 0x11, false, false);
                            tourPrintData(m_msgNames, 0, m_fighters[k].m_name, strlen(m_fighters[k].m_name));
                        } else {
                            m_objs[OBJ_PANEL + k]->changeAnimN(8, 0x11, true, false);
                        }
                    }
                }
            }
        }
        break;
    }
    case State_FinalWinner:
        if (m_timer == 0) {
            g_sndSystem->playSE(static_cast<SndID>(0x1D), -1, 0, 0, -1);
        }
        // fall through
    case State_FinalWait:
        m_timer++;
        if (m_state == State_FinalWinner) {
            if (m_table->winnerCalc()) {
                m_state = State_FinalWait;
                m_table->finalSet(0x1E);
                m_voiceState = 1;
            }
        } else {
            m_table->finalCalc();
        }
        playFinalNarration(this);
        if ((pressed & PAD_A) || (pressed & PAD_START)) {
            if (m_state == State_FinalWinner) {
                m_table->winnerEnd();
                m_table->finalSet(0x1E);
                m_voiceState = 1;
            }
            m_state = State_FinalEnd;
        }
        break;
    case State_FinalEnd:
        m_table->finalEnd();
        playFinalNarration(this);
        m_objs[OBJ_TITLE]->changeAnimN(4, 1, true, false);
        m_objs[OBJ_ROUND]->changeAnimN(4, 0x11, true, false);
        m_objs[OBJ_ROUND_TEXT]->changeAnimN(4, 0x13, true, false);
        m_cursor = 0;
        tourAttach(m_root, m_objs[OBJ_BUTTONS]->m_scnMdl);
        m_objs[OBJ_BUTTONS]->changeAnimN(0, 3, true, false);
        m_objs[OBJ_BUTTONS]->changeAnimN(0, 0x10, false, false);
        m_msg->printIndex(1, 2);
        m_msg->printIndex(2, 3);
        m_msg->printIndex(3, 4);
        fn_800B96E8(&m_ctrl, 0xF0);
        g_sndSystem->playSE(static_cast<SndID>(5), -1, 0, 0, -1);
        m_state = State_FinalAsk;
        break;
    case State_FinalAsk:
        playFinalNarration(this);
        fn_800B9984(&m_ctrl);
        repeat |= *reinterpret_cast<u32*>(reinterpret_cast<char*>(&m_ctrl) + 0x10);
        if ((repeat & PAD_LEFT) && m_cursor == 1) {
            g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
            m_cursor = 0;
            m_objs[OBJ_BUTTONS]->setFrameMatCol(0.0f);
        } else if ((repeat & PAD_RIGHT) && m_cursor == 0) {
            g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
            m_cursor = 1;
            m_objs[OBJ_BUTTONS]->setFrameMatCol(1.0f);
        } else if ((pressed & PAD_A) || (pressed & PAD_START)) {
            g_sndSystem->playSE(static_cast<SndID>(1), -1, 0, 0, -1);
            m_state = State_Leave;
            if (m_cursor == 0) {
                m_result = Result_Next;
            } else {
                m_result = Result_End;
            }
            startKeepScreen__17gfKeepFrameBufferFv(getFrameKeeper());
        }
        break;
    case State_Bye:
        if (m_objs[OBJ_PANEL + m_curSlot]->isNodeAnimFinished()) {
            setVsState();
            m_result = Result_Start;
            m_state = State_Leave;
            startKeepScreen__17gfKeepFrameBufferFv(getFrameKeeper());
        }
        break;
    }
}
