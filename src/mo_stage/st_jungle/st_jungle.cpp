#include <ai/ai_mgr.h>
#include <cm/cm_camera_controller.h>
#include <cm/cm_controller_ai.h>
#include <ft/ft_manager.h>
#include <gf/gf_archive.h>
#include <gm/gm_global.h>
#include <gm/gm_global_mode_melee.h>
#include <gm/gm_stage_data.h>
#include <memory.h>
#include <mt/mt_matrix.h>
#include <mt/mt_prng.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <st/st_class_info.h>
#include <st/st_melee.h>
#include <string.h>
#include <types.h>

#include <st_jungle/gr_jungle.h>
#include <st_jungle/st_jungle.h>

// MATCH-ONLY: the bytes of the match settings the stage reads (their names are unknown): byte 0 holds the game mode in its
// upper six bits, byte 6 the setting that picks the speed in its upper three bits and byte 8 the event.
static inline u8* jungleMeleeBytes() {
    return reinterpret_cast<u8*>(&g_GameGlobal->m_modeMelee->m_meleeInitData);
}

// MATCH-ONLY: the setting that picks the speed is the upper three bits of the byte 6
struct jungleMeleeSpeed {
    u8 _pad[6];
    u8 level : 3;
    u8 rest : 5;
};
static inline u32 jungleMeleeLevel() {
    return reinterpret_cast<jungleMeleeSpeed*>(jungleMeleeBytes())->level;
}

// MATCH-ONLY: the first word of the AI camera controller after its base holds flags of which only the top byte is used here.
static inline u32* jungleCameraFlags() {
    return reinterpret_cast<u32*>(reinterpret_cast<u8*>(g_cmAIController) + 8);
}

stClassInfoImpl<Stages::Jungle, stJungle> stJungle::bss_loc_14;

stJungle* stJungle::create() {
    return new (Heaps::StageInstance) stJungle;
}

stJungle::stJungle() : stMelee("stJungle", Stages::Jungle) {
    memset(m_limit, 0, sizeof(m_limit));
    memset(m_posGimmick, 0, sizeof(m_posGimmick));
    memset(m_pos28C, 0, sizeof(m_pos28C));
    memset(m_pos298, 0, sizeof(m_pos298));
    memset(m_pos2BC, 0, sizeof(m_pos2BC));
    memset(m_pos2C8, 0, sizeof(m_pos2C8));
    memset(m_pos2E0, 0, sizeof(m_pos2E0));
    memset(m_pos2F8, 0, sizeof(m_pos2F8));
    memset(m_pos328, 0, sizeof(m_pos328));
    memset(m_pos358, 0, sizeof(m_pos358));
    memset(m_posPrev, 0, sizeof(m_posPrev));
    memset(m_pos3AC, 0, sizeof(m_pos3AC));
    m_mtxN[0].setIdentity();
    m_mtxN[1].setIdentity();
    m_ladderData = NULL;
    memset(m_enable, 0, sizeof(m_enable));
    m_state[0] = 0;
    m_state[1] = 0;
    m_state[2] = 0;
    m_state[3] = 0;
    m_state[4] = 0;
    m_timer12A = 0.0f;
    m_stateN[0] = 0;
    m_stateN[1] = 0;
    m_stateTrainer = 0;
    m_trainerAway[0] = 0;
    m_trainerAway[1] = 0;
    m_trainerAway[2] = 0;
    m_trainerAway[3] = 0;
    m_stateSpeed = 0;
    m_timerSpeed = 0.0f;
    m_timerWarning = 0.0f;
    m_warning = 1;
    m_scrollDist = 0.0f;
    m_isEvent = 0;
    m_speed = 0.0f;
    m_dangerZone = -1;
    // HYPOTHESIS: a flag of the stage (the other scrolling stages leave it alone)
    *(reinterpret_cast<u8*>(this) + 0xE8) = 1;
    if (g_cmAIController != NULL) {
        *jungleCameraFlags() = (*jungleCameraFlags() & 0xFFFFFF) | 0x1000000;
    }
}

stJungle::~stJungle() {
    if (g_cmAIController != NULL) {
        *jungleCameraFlags() = *jungleCameraFlags() & 0xFFFFFF;
    }
    if (m_ladderData != NULL) {
        delete[] m_ladderData;
    }
    releaseArchive();
}

bool stJungle::loading() {
    return true;
}

void stJungle::createObj() {
    testStageParamInit(m_fileData, 10);
    testStageDataInit(m_fileData, 0x14, 0x70);
    createObjBg(0);
    createObjBg(1);
    createObjAshiba(2);
    createObjAshiba(3);
    createObjAshiba(4);
    createObjAshiba(5);
    createObjAshiba(6);
    createObjAshiba(7);
    createObjAshiba(8);
    createObjAshiba(9);
    createObjAshiba(10);
    createObjAshiba(11);
    createObjAshiba(12);
    createObjAshiba(13);
    createObjAshiba(14);
    createObjAshiba(15);
    createObjAshiba(16);
    createObjAshiba(17);
    createObjAshiba(18);
    createObjAshiba(19);
    createObjAshiba(20);
    createObjAshiba(21);
    createObjAshiba(22);
    createObjAshiba(23);
    createObjAshiba(24);
    createObjAshiba(25);
    createObjN(26);
    createObjNB(27);
    createObjN(28);
    createObjNB(29);
    createObjS(30);
    createObjSB(31);
    createObjWater(32);
    createObjWater(33);
    createObjHashigo();
    createObjAttack();
    createObjWarning(39);
    initCameraParam();
    void* posData = m_fileData->getData(Data_Type_Model, 100, 0xFFFE);
    if (posData == NULL) {
        createStagePositions();
    } else {
        nw4r::g3d::ResFile posFile(posData);
        createStagePositions(&posFile);
    }
    createWind2ndOnly();
    initPosPokeTrainer(4, 0);
    createObjPokeTrainer(m_fileData, 0x65, "PokeTrainer00", m_pokeTrainerPos, &m_posGimmick[2]);
    createObjPokeTrainer(m_fileData, 0x66, "PokeTrainer01", &m_pokeTrainerPos[2], &m_posGimmick[6]);
    createObjPokeTrainer(m_fileData, 0x67, "PokeTrainer02", &m_pokeTrainerPos[4], &m_posGimmick[10]);
    createObjPokeTrainer(m_fileData, 0x68, "PokeTrainer03", &m_pokeTrainerPos[6], &m_posGimmick[0]);
    loadStageAttrParam(m_fileData, 100);
    registScnAnim(static_cast<nw4r::g3d::ResFileData*>(m_fileData->getData(Data_Type_Scene, 0, 0xFFFE)), 0);
    if (g_GameGlobal->m_modeMelee != NULL && (jungleMeleeBytes()[0] >> 2) == 7 && jungleMeleeBytes()[8] == 0x15) {
        m_isEvent = 1;
    }
}

// The backdrop (the model that publishes the places of its nodes) and the locator that gives the frame of the scroll.
void stJungle::createObjBg(int index) {
    grJungleBg* ground;
    Vec3f* posGimmick;
    Vec3f* posLimit;
    float* frameLoc;
    switch (index) {
    case 1:
        ground = grJungleBg::create(0x1B, "TopN", "grJungleLoc");
        posGimmick = m_posGimmick;
        frameLoc = &m_scrollDist;
        posLimit = NULL;
        break;
    case 0:
        ground = grJungleBg::create(0, "TopN", "grJungleMainBg");
        posLimit = m_limit;
        posGimmick = NULL;
        frameLoc = NULL;
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setPosGimmickWork(posGimmick);
        ground->setPosLimitWork(posLimit);
        ground->setFrameLocWork(frameLoc);
    }
}

// The small setters of the grounds are defined here, as in the original (the stage unit is the first one that needs them).
void grJungleBg::setPosGimmickWork(Vec3f* posGimmickWork) {
    m_posGimmickWork = posGimmickWork;
}

void grJungleBg::setPosLimitWork(Vec3f* posLimitWork) {
    m_posLimitWork = posLimitWork;
}

void grJungleBg::setFrameLocWork(float* frameLocWork) {
    m_frameLocWork = frameLocWork;
}

// The 24 platforms (the ones the stage does not know by name are the plain grJungleAshiba).
void stJungle::createObjAshiba(int index) {
    grJungleAshiba* ground;
    Vec3f* posWork;
    Vec3f* posGimmick;
    Vec3f* posHashigo;
    Matrix* mtx;
    u8* state;
    u8* enable;
    int collision;
    mtx = NULL;
    state = NULL;
    collision = 0;
    switch (index) {
    case 2:
        ground = grJungleAshiba01::create(7, "StgJungle00LV01", "grJungleAshiba01");
        posWork = &m_posGimmick[0];
        enable = &m_enable[0];
        posGimmick = NULL;
        posHashigo = NULL;
        collision = 0x1E;
        break;
    case 3:
        ground = grJungleAshiba02::create(8, "StgJungle00LV02", "grJungleAshiba02");
        posWork = &m_posGimmick[1];
        posGimmick = m_pos28C;
        state = &m_state[0];
        enable = &m_enable[1];
        posHashigo = NULL;
        collision = 0x1F;
        break;
    case 4:
        ground = grJungleAshiba02A::create(9, "StgJungle00LV02A", "grJungleAshiba02A");
        posWork = m_pos28C;
        state = &m_state[0];
        enable = &m_enable[1];
        posGimmick = NULL;
        posHashigo = NULL;
        collision = 0x20;
        break;
    case 5:
        ground = grJungleAshiba03::create(10, "StgJungle00LV03", "grJungleAshiba03");
        posWork = &m_posGimmick[2];
        posGimmick = m_pos298;
        state = &m_state[1];
        enable = &m_enable[2];
        posHashigo = NULL;
        collision = 0x21;
        break;
    case 6:
        ground = grJungleAshiba03A::create(11, "StgJungle00LV03A", "grJungleAshiba03A");
        posWork = m_pos298;
        state = &m_state[1];
        enable = &m_enable[2];
        posGimmick = NULL;
        posHashigo = NULL;
        collision = 0x22;
        break;
    case 7:
        ground = grJungleAshiba05::create(12, "StgJungle00LV05", "grJungleAshiba05");
        posWork = &m_posGimmick[3];
        posGimmick = m_pos2BC;
        posHashigo = m_pos3AC;
        enable = &m_enable[4];
        collision = 0x23;
        break;
    case 8:
        ground = grJungleAshiba05A::create(13, "StgJungle00LV05A", "grJungleAshiba05A");
        posWork = m_pos2BC;
        enable = &m_enable[4];
        posGimmick = NULL;
        posHashigo = NULL;
        collision = 0x24;
        break;
    case 9:
        ground = grJungleAshiba::create(14, "StgJungle00LV06", "grJungleAshiba06");
        posWork = &m_posGimmick[4];
        enable = &m_enable[5];
        posGimmick = NULL;
        posHashigo = NULL;
        collision = 0x25;
        break;
    case 10:
        ground = grJungleAshiba07::create(15, "StgJungle00LV07", "grJungleAshiba07");
        posWork = &m_posGimmick[5];
        posGimmick = m_pos2C8;
        posHashigo = &m_pos3AC[2];
        enable = &m_enable[5];
        collision = 0x26;
        break;
    case 11:
        ground = grJungleAshiba07A::create(16, "StgJungle00LV07A", "grJungleAshiba07A");
        posWork = m_pos2C8;
        enable = &m_enable[6];
        posGimmick = NULL;
        posHashigo = NULL;
        collision = 0x27;
        break;
    case 12:
        ground = grJungleAshiba07B::create(17, "StgJungle00LV07B", "grJungleAshiba07B");
        posWork = &m_pos2C8[1];
        enable = &m_enable[6];
        posGimmick = NULL;
        posHashigo = NULL;
        collision = 0x28;
        break;
    case 13:
        ground = grJungleAshiba08::create(18, "StgJungle00LV08", "grJungleAshiba08");
        posWork = &m_posGimmick[6];
        posGimmick = m_pos2E0;
        state = &m_state[3];
        enable = &m_enable[7];
        posHashigo = NULL;
        collision = 0x29;
        break;
    case 14:
        ground = grJungleAshiba08A::create(19, "StgJungle00LV08A", "grJungleAshiba08A");
        posWork = m_pos2E0;
        state = &m_state[3];
        enable = &m_enable[7];
        posGimmick = NULL;
        posHashigo = NULL;
        collision = 0x2A;
        break;
    case 15:
        ground = grJungleAshiba09::create(20, "StgJungle00LV09", "grJungleAshiba09");
        posWork = &m_posGimmick[7];
        posGimmick = m_pos2F8;
        enable = &m_enable[8];
        posHashigo = NULL;
        collision = 0x2B;
        break;
    case 16:
        ground = grJungleAshiba::create(21, "StgJungle00LV09A", "grJungleAshiba09A_1");
        posWork = m_pos2F8;
        enable = &m_enable[8];
        posGimmick = NULL;
        posHashigo = NULL;
        collision = 0x2C;
        break;
    case 17:
        ground = grJungleAshiba::create(21, "StgJungle00LV09A", "grJungleAshiba09A_2");
        posWork = &m_pos2F8[1];
        enable = &m_enable[8];
        posGimmick = NULL;
        posHashigo = NULL;
        collision = 0x2C;
        break;
    case 18:
        ground = grJungleAshiba::create(21, "StgJungle00LV09A", "grJungleAshiba09A_3");
        posWork = &m_pos2F8[2];
        enable = &m_enable[8];
        posGimmick = NULL;
        posHashigo = NULL;
        collision = 0x2C;
        break;
    case 19:
        ground = grJungleAshiba::create(21, "StgJungle00LV09A", "grJungleAshiba09A_4");
        posWork = &m_pos2F8[3];
        enable = &m_enable[8];
        posGimmick = NULL;
        posHashigo = NULL;
        collision = 0x2C;
        break;
    case 20:
        ground = grJungleAshiba10::create(22, "StgJungle00LV10", "grJungleAshiba10");
        posWork = &m_posGimmick[8];
        posGimmick = m_pos328;
        mtx = &m_mtxN[0];
        state = &m_stateN[0];
        enable = &m_enable[9];
        posHashigo = NULL;
        collision = 0x2D;
        break;
    case 21:
        ground = grJungleAshiba::create(23, "StgJungle00LV11", "grJungleAshiba11");
        posWork = &m_posGimmick[9];
        enable = &m_enable[10];
        posGimmick = NULL;
        posHashigo = NULL;
        collision = 0x2E;
        break;
    case 22:
        ground = grJungleAshiba12::create(24, "StgJungle00LV12", "grJungleAshiba12");
        posWork = &m_posGimmick[10];
        posGimmick = m_pos358;
        enable = &m_enable[11];
        posHashigo = NULL;
        break;
    case 23:
        ground = grJungleAshiba12A::create(25, "StgJungle00LV12A", "grJungleAshiba12A_1");
        posWork = m_pos358;
        posGimmick = NULL;
        posHashigo = NULL;
        enable = NULL;
        collision = 0x2F;
        break;
    case 24:
        ground = grJungleAshiba12A::create(25, "StgJungle00LV12A", "grJungleAshiba12A_2");
        posWork = &m_pos358[2];
        posGimmick = NULL;
        posHashigo = NULL;
        enable = NULL;
        collision = 0x2F;
        break;
    case 25:
        ground = grJungleAshiba::create(26, "StgJungle00LV13", "grJungleAshiba13");
        posWork = &m_posGimmick[11];
        enable = &m_enable[12];
        posGimmick = NULL;
        posHashigo = NULL;
        collision = 0x30;
        break;
    default:
        ground = NULL;
        posWork = NULL;
        posGimmick = NULL;
        posHashigo = NULL;
        enable = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setPosWork(posWork);
        ground->setPosGimmickWork(posGimmick);
        ground->setPosHashigoWork(posHashigo);
        ground->setPosLimitWork(m_limit);
        ground->setMtxGimmickWork(mtx);
        ground->setStateWork(state);
        ground->setEnableWork(enable);
        if (collision != 0) {
            createCollision(m_fileData, collision, ground);
        }
        ground->setEnableCollisionStatus(true);
    }
}

void grJungleAshiba::setPosWork(Vec3f* posWork) {
    m_posWork = posWork;
}

void grJungleAshiba::setPosGimmickWork(Vec3f* posGimmickWork) {
    m_posGimmickWork = posGimmickWork;
}

void grJungleAshiba::setPosHashigoWork(Vec3f* posHashigoWork) {
    m_posHashigoWork = posHashigoWork;
}

void grJungleAshiba::setPosLimitWork(Vec3f* posLimitWork) {
    m_posLimitWork = posLimitWork;
}

void grJungleAshiba::setMtxGimmickWork(Matrix* mtxGimmickWork) {
    m_mtxGimmickWork = mtxGimmickWork;
}

void grJungleAshiba::setStateWork(u8* stateWork) {
}

void grJungleAshiba::setEnableWork(u8* enableWork) {
    m_enableWork = enableWork;
}

// The stone blocks on the sides: the left one (with the matrix, the state and the flag the stage gives).
void stJungle::createObjN(int index) {
    grJungleN* ground;
    Matrix* mtx;
    u8* state;
    u8* enable;
    int collision;
    collision = 0;
    state = NULL;
    enable = NULL;
    switch (index) {
    case 26:
        ground = grJungleN::create(1, "StgJungle00NL", "grJungleMainNL");
        mtx = &m_mtxN[0];
        state = &m_stateN[0];
        enable = &m_enable[9];
        collision = 0x32;
        break;
    case 28:
        ground = grJungleN::create(3, "StgJungle00NR", "grJungleMainNR");
        mtx = &m_mtxN[1];
        state = &m_stateN[1];
        enable = &m_enable[9];
        collision = 0x33;
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setMtxWork(mtx);
        ground->setStateWork(state);
        ground->setEnableWork(enable);
        if (collision != 0) {
            createCollision(m_fileData, collision, ground);
        }
    }
}

void grJungleN::setMtxWork(Matrix* mtxWork) {
    m_mtxWork = mtxWork;
}

void grJungleN::setStateWork(u8* stateWork) {
    m_stateWork = stateWork;
}

void grJungleN::setEnableWork(u8* enableWork) {
    m_enableWork = enableWork;
}

// The pieces of the stone blocks.
void stJungle::createObjNB(int index) {
    grJungleNB* ground;
    Matrix* mtx;
    u8* state;
    mtx = NULL;
    switch (index) {
    case 27:
        ground = grJungleNB::create(2, "gr2_StgJungle00NLB", "grJungleMainNLB");
        mtx = &m_mtxN[0];
        state = &m_stateN[0];
        break;
    case 29:
        ground = grJungleNB::create(4, "gr2_StgJungle00NRB", "grJungleMainNRB");
        mtx = &m_mtxN[1];
        state = &m_stateN[1];
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setMtxWork(mtx);
        ground->setStateWork(state);
    }
}

void grJungleNB::setMtxWork(Matrix* mtxWork) {
    m_mtxWork = mtxWork;
}

void grJungleNB::setStateWork(u8* stateWork) {
    m_stateWork = stateWork;
}

// The stone.
void stJungle::createObjS(int index) {
    grJungleS* ground;
    Vec3f* pos;
    u8* state;
    u8* enable;
    int collision;
    collision = 0;
    state = NULL;
    enable = NULL;
    switch (index) {
    case 30:
        ground = grJungleS::create(5, "StoneN", "grJungleMainS");
        pos = &m_pos298[1];
        state = &m_state[2];
        enable = &m_enable[2];
        collision = 0x34;
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setPosWork(pos);
        ground->setStateWork(state);
        ground->setEnableWork(enable);
        if (collision != 0) {
            createCollision(m_fileData, collision, ground);
        }
    }
}

void grJungleS::setPosWork(Vec3f* posWork) {
    m_posWork = posWork;
}

void grJungleS::setStateWork(u8* stateWork) {
    m_stateWork = stateWork;
}

void grJungleS::setEnableWork(u8* enableWork) {
    m_enableWork = enableWork;
}

// The pieces of the stone.
void stJungle::createObjSB(int index) {
    grJungleSB* ground;
    Vec3f* pos;
    u8* state;
    state = NULL;
    switch (index) {
    case 31:
        ground = grJungleSB::create(6, "gr2_StgJungle00SB", "grJungleMainSB");
        pos = &m_pos298[1];
        state = &m_state[2];
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setPosWork(pos);
        ground->setStateWork(state);
    }
}

void grJungleSB::setPosWork(Vec3f* posWork) {
    m_posWork = posWork;
}

void grJungleSB::setStateWork(u8* stateWork) {
    m_stateWork = stateWork;
}

// The water of the second and the third part.
void stJungle::createObjWater(int index) {
    grJungleWater* ground;
    Vec3f* pos;
    u8* enable;
    switch (index) {
    case 33:
        ground = grJungleWater::create(0x47, "gr2_StgJungle00LV03Water", "grJungleWater03");
        pos = &m_posGimmick[2];
        enable = &m_enable[2];
        break;
    case 32:
        ground = grJungleWater::create(0x46, "gr2_StgJungle00LV02Water", "grJungleWater02");
        pos = &m_posGimmick[1];
        enable = &m_enable[1];
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setPosWork(pos);
        ground->setEnableWork(enable);
    }
}

void grJungleWater::setPosWork(Vec3f* posWork) {
    m_posWork = posWork;
}

void grJungleWater::setEnableWork(u8* enableWork) {
    m_enableWork = enableWork;
}

// MATCH-ONLY: the ladder entries are set up by hand (the motion path model of the first entry is cleared each time).
static inline void jungleInitLadder(grGimmickLadderData* data, grGimmickLadderData* first, u8 mdlIndex, const char* nodeName,
                                    float offsetY, float rangeX, float rangeY) {
    MEMINIT(data);
    data->m_mdlIndex = mdlIndex;
    data->m_49 = 0;
    data->m_restrictUpExit = false;
    data->m_51 = 1;
    strcpy(data->m_nodeName, nodeName);
    first->m_motionPathData.m_mdlIndex = -1;
    data->m_areaData.m_offsetPos.m_x = 0.0f;
    data->m_areaData.m_offsetPos.m_y = offsetY;
    data->m_areaData.m_range.m_x = rangeX;
    data->m_areaData.m_range.m_y = rangeY;
}

// The three ladders (ids 0x3C to 0x3E), each created right after its data.
void stJungle::createObjHashigo() {
    m_ladderData = new (Heaps::StageResource) grGimmickLadderData[3];
    if (m_ladderData != NULL) {
        jungleInitLadder(&m_ladderData[0], m_ladderData, 0x3C, "StgJungle00LV05Ladder1", 24.0f, 10.0f, 48.0f);
        createObjHashigo1(0x22);
        jungleInitLadder(&m_ladderData[1], m_ladderData, 0x3D, "StgJungle00LV05Ladder2", 24.0f, 10.0f, 48.0f);
        createObjHashigo1(0x23);
        jungleInitLadder(&m_ladderData[2], m_ladderData, 0x3C, "StgJungle00LV07Ladder", 41.6f, 10.0f, 83.2f);
        createObjHashigo1(0x24);
    }
}

void stJungle::createObjHashigo1(int index) {
    grJungleLadder* ground;
    Vec3f* pos;
    Vec3f* posHashigo;
    grGimmickLadderData* data;
    switch (index) {
    case 0x23:
        ground = grJungleLadder::create(0x3D, "StgJungle00LV05Ladder2", "grJungleLadder0502");
        pos = &m_posGimmick[3];
        posHashigo = &m_pos3AC[1];
        data = &m_ladderData[1];
        break;
    case 0x22:
        ground = grJungleLadder::create(0x3C, "StgJungle00LV05Ladder1", "grJungleLadder0501");
        data = m_ladderData;
        pos = &m_posGimmick[3];
        posHashigo = m_pos3AC;
        break;
    case 0x24:
        ground = grJungleLadder::create(0x3E, "StgJungle00LV07Ladder", "grJungleLadder07");
        pos = &m_posGimmick[5];
        posHashigo = &m_pos3AC[2];
        data = &m_ladderData[2];
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->setGimmickData(data);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setPosWork(pos);
        ground->setPosHashigoWork(posHashigo);
    }
}

void grJungleLadder::setPosWork(Vec3f* posWork) {
    m_posWork = posWork;
}

void grJungleLadder::setPosHashigoWork(Vec3f* posHashigoWork) {
    m_posHashigoWork = posHashigoWork;
}

// The two things that hurt: the spikes (03) and the stone (08), they only have an attack.
void stJungle::createObjAttack() {
    grJungleAttack03* ground03 = grJungleAttack03::create(0x5A, "nodeIndex", "grJungleAttack03");
    if (ground03 != NULL) {
        addGround(ground03);
        ground03->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground03->setStageData(m_stageData);
        ground03->setPosWork(&m_pos298[2]);
        ground03->setPosLimitWork(m_limit);
        ground03->setEnableWork(&m_enable[2]);
        grJungleAttack08* ground08 = grJungleAttack08::create(0x5A, "nodeIndex", "grJungleAttack08");
        if (ground08 != NULL) {
            addGround(ground08);
            ground08->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
            ground08->setStageData(m_stageData);
            ground08->setPosWork(&m_pos2E0[1]);
            ground08->setPosLimitWork(m_limit);
            ground08->setEnableWork(&m_enable[7]);
        }
    }
}

void grJungleAttack::setPosWork(Vec3f* posWork) {
    m_posWork = posWork;
}

void grJungleAttack::setPosLimitWork(Vec3f* posLimitWork) {
    m_posLimitWork = posLimitWork;
}

void grJungleAttack::setEnableWork(u8* enableWork) {
    m_enableWork = enableWork;
}

void stJungle::createObjWarning(int index) {
    grJungleWarning* ground;
    switch (index) {
    case 39:
        ground = grJungleWarning::create(0x50, "gr2_StgJungle00Caution", "grJungleWarning");
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setStateWork(&m_warning);
    }
}

void grJungleWarning::setStateWork(u8* stateWork) {
    m_stateWork = stateWork;
}

void stJungle::update(float deltaFrame) {
    updateLimit(deltaFrame);
    updateAI(deltaFrame);
    updateAshiba(deltaFrame);
    updateAshiba12A(deltaFrame);
    updateTrainer(deltaFrame);
    updateSpeed(deltaFrame);
}

void stJungle::updateLimit(float deltaFrame) {
    CameraController* camera = CameraController::getInstance();
    m_limit[0].m_x = camera->unk158;
    m_limit[0].m_y = camera->unk160;
    m_limit[0].m_z = 0.0f;
    m_limit[1].m_x = camera->unk15C;
    m_limit[1].m_y = camera->unk164;
    m_limit[1].m_z = 0.0f;
}

// The danger zone of the AI is the part of the screen that is above a line that moves with the scroll: low at the start and
// at the end (a quarter of the screen), higher in the middle of the stage.
void stJungle::updateAI(float deltaFrame) {
    Vec2f max;
    Vec2f min;
    float dist = m_scrollDist;
    if (dist >= 3250.0f) {
        min.m_y = m_limit[1].m_y - 0.25f * (m_limit[1].m_y - m_limit[0].m_y);
    } else if (dist >= 3100.0f) {
        float c = grJungleClamp((dist - 3100.0f) / 150.0f, 0.0f, 1.0f);
        min.m_y = m_limit[1].m_y - (0.45f - 0.2f * c) * (m_limit[1].m_y - m_limit[0].m_y);
    } else if (dist >= 2300.0f) {
        float c = grJungleClamp((dist - 2300.0f) / 800.0f, 0.0f, 1.0f);
        min.m_y = m_limit[1].m_y - (0.25f + 0.2f * c) * (m_limit[1].m_y - m_limit[0].m_y);
    } else {
        min.m_y = m_limit[1].m_y - 0.25f * (m_limit[1].m_y - m_limit[0].m_y);
    }
    min.m_x = m_limit[0].m_x;
    // (the original reads the top of the zone before it is set)
    max.m_y = max.m_y - 10.0f;
    max.m_x = m_limit[1].m_x;
    max.m_y = m_limit[1].m_y;
    m_dangerZone = g_aiMgr->setDangerZone(&min, &max, m_dangerZone, false, false);
}

// Tells the grounds which platforms are on the screen (the flags go to the grounds) and measures how far the scroll went.
void stJungle::updateAshiba(float deltaFrame) {
    for (int i = 0; i < 13; i++) {
        int prev = i - 1;
        if (prev < 0) {
            prev += 13;
        }
        int next = i + 1;
        if (next >= 13) {
            next -= 13;
        }
        if (m_posGimmick[prev].m_y > m_limit[0].m_y || m_posGimmick[next].m_y < m_limit[1].m_y) {
            m_enable[i] = 0;
        } else {
            m_enable[i] = 1;
        }
    }
    Vec3f scroll;
    scroll.m_x = 0.0f;
    scroll.m_y = 0.0f;
    scroll.m_z = 0.0f;
    if (m_posPrev[0].m_y > m_posGimmick[0].m_y) {
        scroll = m_posGimmick[0] - m_posPrev[0];
    } else if (m_posPrev[1].m_y > m_posGimmick[6].m_y) {
        scroll = m_posGimmick[6] - m_posPrev[1];
    }
    m_scroll = scroll;
    m_posPrev[0] = m_posGimmick[0];
    m_posPrev[1] = m_posGimmick[6];
}

// The platform 12A moves every time the delay of the stage data is over, in turns with its twin.
void stJungle::updateAshiba12A(float deltaFrame) {
    stJungleData* data = static_cast<stJungleData*>(m_stageData);
    if (data != NULL) {
        float timer = m_timer12A;
        m_timer12A = timer - deltaFrame;
        if (m_timer12A < 0.0f) {
            m_timer12A = 0.0f;
        }
        if (m_timer12A == 0.0f) {
            m_timer12A = data->unk28;
            grJungleAshiba12A* ground;
            switch (m_state[4]) {
            case 0:
                ground = static_cast<grJungleAshiba12A*>(getGround(0x17));
                m_state[4] = 1;
                break;
            case 1:
                ground = static_cast<grJungleAshiba12A*>(getGround(0x18));
                m_state[4] = 0;
                break;
            default:
                ground = NULL;
                break;
            }
            if (ground != NULL) {
                ground->requestMove();
            }
        }
    }
}

// Keeps track of the Pokemon Trainer: he may only be replaced when his position is out of the camera area (below it).
void stJungle::updateTrainer(float deltaFrame) {
    switch (m_stateTrainer) {
    case 0:
        m_pokeTrainerPosIndex = 3;
        g_ftManager->notifyReplacePokeTrainer(3);
        m_stateTrainer++;
        break;
    case 1: {
        u32 index = m_pokeTrainerPosIndex;
        if (m_trainerAway[index] == 0) {
            float limit = m_limit[1].m_y - 10.0f;
            u8 slot = index << 1;
            if (m_pokeTrainerPos[slot].m_y < limit && m_pokeTrainerPos[slot + 1].m_y < limit) {
                m_trainerAway[index] = 1;
                m_pokeTrainerPosIndex++;
                if (m_pokeTrainerPosIndex >= 4) {
                    m_pokeTrainerPosIndex = 0;
                }
                m_stateTrainer = 2;
            }
        }
        break;
    }
    case 2: {
        float limit = 10.0f + m_limit[0].m_y;
        u32 slot = m_pokeTrainerPosIndex << 1;
        if (m_pokeTrainerPos[slot].m_y < limit || m_pokeTrainerPos[slot + 1].m_y < limit) {
            g_ftManager->notifyReplacePokeTrainer(m_pokeTrainerPosIndex);
            m_stateTrainer = 1;
        }
        break;
    }
    }
    if (m_trainerAway[0] == 1) {
        if (m_pokeTrainerPos[0].m_y > m_limit[0].m_y && m_pokeTrainerPos[1].m_y > m_limit[0].m_y) {
            m_trainerAway[0] = 0;
        }
    }
    if (m_trainerAway[1] == 1) {
        if (m_pokeTrainerPos[2].m_y > m_limit[0].m_y && m_pokeTrainerPos[3].m_y > m_limit[0].m_y) {
            m_trainerAway[1] = 0;
        }
    }
    if (m_trainerAway[2] == 1) {
        if (m_pokeTrainerPos[4].m_y > m_limit[0].m_y && m_pokeTrainerPos[5].m_y > m_limit[0].m_y) {
            m_trainerAway[2] = 0;
        }
    }
    if (m_trainerAway[3] == 1) {
        if (m_pokeTrainerPos[6].m_y > m_limit[0].m_y && m_pokeTrainerPos[7].m_y > m_limit[0].m_y) {
            m_trainerAway[3] = 0;
        }
    }
}

// The scroll speed: in the event match it is set once from the stage data (by the difficulty of the match); otherwise the
// stage slows and speeds up at random times (a warning sign shows before the scroll gets faster).
void stJungle::updateSpeed(float deltaFrame) {
    stJungleData* data = static_cast<stJungleData*>(m_stageData);
    if (data != NULL) {
        if (m_isEvent == 1) {
            switch (m_stateSpeed) {
            case 0:
                if (g_GameGlobal->m_modeMelee != NULL) {
                    switch (jungleMeleeLevel()) {
                    case 0:
                        m_speed = data->unk64;
                        break;
                    case 1:
                        m_speed = data->unk68;
                        break;
                    case 2:
                        m_speed = data->unk6C;
                        break;
                    }
                    m_stateSpeed++;
                }
                break;
            case 1:
                break;
            case 2:
                g_GameGlobal->m_stageData->m_motionSubRatio = m_speed;
                break;
            }
        } else {
            m_timerSpeed -= deltaFrame;
            if (m_timerSpeed < 0.0f) {
                m_timerSpeed = 0.0f;
            }
            m_timerWarning -= deltaFrame;
            if (m_timerWarning < 0.0f) {
                m_timerWarning = 0.0f;
            }
            switch (m_stateSpeed) {
            case 0:
                m_timerSpeed = data->unk40;
                m_stateSpeed = 1;
            case 1:
                if (m_timerSpeed == 0.0f) {
                    bool warn = randf() < data->unk4C;
                    if (warn == true) {
                        m_warning = 0;
                        m_timerSpeed = data->unk54;
                        m_timerWarning = data->unk58;
                        m_stateSpeed = 2;
                    } else {
                        m_timerSpeed = data->unk44 + (data->unk48 - data->unk44) * randf();
                    }
                }
                break;
            case 2:
                if (m_timerSpeed == 0.0f) {
                    setMotionSubRatio(data->unk50, 90.0f);
                    float rand = randf();
                    m_timerSpeed = data->unk5C + (data->unk60 - data->unk5C) * rand;
                    m_stateSpeed = 3;
                }
                break;
            case 3:
                if (m_timerSpeed == 0.0f) {
                    setMotionSubRatio(1.0f, 90.0f);
                    float rand = randf();
                    m_timerSpeed = data->unk44 + (data->unk48 - data->unk44) * rand;
                    m_stateSpeed = 1;
                }
                break;
            }
            if (m_timerWarning != 0.0f) {
                m_warning = 0;
            } else {
                m_warning = 1;
            }
        }
    }
}

void stJungle::notifyEventInfoGo() {
    if (m_isEvent != 1) {
        return;
    }
    m_stateSpeed = 2;
}

int stJungle::getScrollDir(Vec3f* dir) {
    dir->m_x = m_scroll.m_x;
    dir->m_y = m_scroll.m_y;
    dir->m_z = m_scroll.m_z;
    return 1;
}

bool stJungle::isEventEnd(int param1, int* eventState, int* eventDecision) {
    if (m_isEvent == 0) {
        return false;
    }
    if (7740.0f <= m_scrollDist) {
        *eventState = 6;
        *eventDecision = 4;
        return true;
    }
    return false;
}

void stJungle::getBamperVector(Vec3f* vec) {
    if (vec == NULL) {
        return;
    }
    vec->m_x = m_scroll.m_x;
    vec->m_y = m_scroll.m_y;
    vec->m_z = m_scroll.m_z;
}
