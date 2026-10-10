#include <cm/cm_camera_controller.h>
#include <ec/ec_mgr.h>
#include <ft/ft_manager.h>
#include <gf/gf_3d_scene.h>
#include <gf/gf_archive.h>
#include <gm/gm_global.h>
#include <memory.h>
#include <mt/mt_prng.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <snd/snd_system.h>
#include <st/st_class_info.h>
#include <st/st_melee.h>
#include <st/st_positions.h>
#include <string.h>
#include <types.h>

#include <st_metalgear/gr_metalgear.h>
#include <st_metalgear/nw4r_camera.h>
#include <st_metalgear/st_metalgear.h>

// stCollisionWork::destroy (sora_melee, unnamed)
extern "C" void fn_27_239F6C(stCollisionWork* work);

// MATCH-ONLY: the owner of a fighter as the stage calls it: its virtual part starts at +0xC, and the functions get the
// object pointer itself (the slots are those of ftOwner).
struct metalgearOwnerPrefix {
    u8 _pad[0xC];
};
class metalgearOwnerView : public metalgearOwnerPrefix {
public:
    virtual ~metalgearOwnerView();
    virtual bool isSubOwner();
    virtual void setDamage(float damage, bool unk);
    virtual float getDamage();
};

// MATCH-ONLY: the first bytes of the match settings (the game rule is a bit field of three bits)
struct metalgearMeleeView {
    u8 m_gameMode : 6;
    u8 _unused0 : 2;
    u8 m_gameRule : 3;
    u8 _unused1 : 5;
};

// MATCH-ONLY: two bytes of the result record of a player (HYPOTHESIS: the stage reads them at the offsets, the layout of
// gmPlayerResultInfo has no names for them)
struct stMetalgearResultView {
    u8 _pad0[0x2E];
    s8 unk2E;
    u8 _pad1[4];
    u8 unk33;
    u8 _pad2[0x2AC - 0x34];
};
static_assert(sizeof(stMetalgearResultView) == 0x2AC, "Class is wrong size!");

stClassInfoImpl<Stages::MetalGear, stMetalgear> stMetalgear::bss_loc_14;

stMetalgear* stMetalgear::create() {
    return new (Heaps::StageInstance) stMetalgear;
}

stMetalgear::stMetalgear() : stMelee("stMetalgear", Stages::MetalGear) {
    memset(m_posGimmick, 0, sizeof(m_posGimmick));
    m_posGimmick[2].m_x = 0.0f;
    m_posGimmick[2].m_y = 0.0f;
    m_posGimmick[2].m_z = 200.0f;
    m_posGimmick[3].m_x = 0.0f;
    m_posGimmick[3].m_y = 0.0f;
    m_posGimmick[3].m_z = 200.0f;
    memset(m_cameraRange, 0, sizeof(m_cameraRange));
    m_goFlag = 0;
    m_bgState = 6;
    m_snow = 9;
    m_metalgearPhase = 0;
    m_timerA = 0.0f;
    m_timerB = 0.0f;
    m_timerC = 0.0f;
    m_metalgearKind = 9;
    m_metalgearState = 6;
    m_wallPhase[0] = 0;
    m_wallPhase[1] = 0;
    m_wallTimer[0] = 0.0f;
    m_wallTimer[1] = 0.0f;
    m_wallState[0] = 6;
    m_wallState[1] = 6;
    m_wallState[2] = 6;
    m_wallState[3] = 6;
    m_wallRotZ[0] = 0.0f;
    m_wallRotZ[1] = 0.0f;
    m_collLeft.initialize();
    m_collLeft.m_vtxLen = 2;
    m_collRight.initialize();
    m_collRight.m_vtxLen = 2;
    m_searchState = 6;
    m_searchTarget = -1;
    m_exclamationState = 6;
    m_appearTask = NULL;
    m_appearStarted = 0;
    m_eventA = 0;
    m_eventB = 0;
}

stMetalgear::~stMetalgear() {
    fn_27_239F6C(&m_collLeft);
    fn_27_239F6C(&m_collRight);
    releaseArchive();
    if (m_appearTask != NULL) {
        m_appearTask->exit();
    }
}

bool stMetalgear::loading() {
    return true;
}

void stMetalgear::createObj() {
    testStageParamInit(m_fileData, 10);
    testStageDataInit(m_fileData, 0x14, 0xA0);
    selectSnow();
    createObjEtc(0);
    createObjEtc(1);
    createObjBg(2);
    createObjWall(3);
    createObjWall(4);
    createObjWall(5);
    createObjWall(6);
    createCollision(m_fileData, 2, NULL);
    createObjSearch(7);
    createObjExclamation(8);
    createObjAttack(9);
    createObjAttack(10);
    createObjSnow();
    createObjMetalgear();
    initCameraParam();
    void* posData = m_fileData->getData(Data_Type_Model, 100, 0xFFFE);
    if (posData != NULL) {
        nw4r::g3d::ResFile posFile(posData);
        createStagePositions(&posFile);
    } else {
        createStagePositions();
    }
    createWind2ndOnly();
    if (m_wind2ndTrigger != NULL) {
        switch (m_snow) {
        case 3:
            m_wind2ndTrigger->setAreaSleep(1);
            break;
        case 4:
            m_wind2ndData->m_speed = 0.22f;
            m_wind2ndData->m_vector = 290.0f;
            m_wind2ndData->m_60 = 80.0f;
            m_wind2ndData->m_64 = 0.6f;
            m_wind2ndData->m_72 = 0x19;
            m_wind2ndTrigger->setWindParam(m_wind2ndData, 0);
            m_wind2ndTrigger->setAreaSleep(0);
            break;
        case 5:
            m_wind2ndData->m_speed = 0.3f;
            m_wind2ndData->m_vector = 315.0f;
            m_wind2ndData->m_60 = 50.0f;
            m_wind2ndData->m_64 = 0.8f;
            m_wind2ndData->m_72 = 0x28;
            m_wind2ndTrigger->setWindParam(m_wind2ndData, 0);
            m_wind2ndTrigger->setAreaSleep(0);
            break;
        case 6:
            m_wind2ndData->m_speed = 0.6f;
            m_wind2ndData->m_vector = 340.0f;
            m_wind2ndData->m_60 = 16.0f;
            m_wind2ndData->m_64 = 1.1f;
            m_wind2ndData->m_72 = 0x5A;
            m_wind2ndTrigger->setWindParam(m_wind2ndData, 0);
            m_wind2ndTrigger->setAreaSleep(0);
            break;
        }
    }
    switch (m_snow) {
    case 3:
        registScnAnim(static_cast<nw4r::g3d::ResFileData*>(m_fileData->getData(Data_Type_Scene, 0, 0xFFFE)), 0);
        break;
    case 4:
        registScnAnim(static_cast<nw4r::g3d::ResFileData*>(m_fileData->getData(Data_Type_Scene, 0, 0xFFFE)), 1);
        break;
    case 5:
        registScnAnim(static_cast<nw4r::g3d::ResFileData*>(m_fileData->getData(Data_Type_Scene, 0, 0xFFFE)), 2);
        break;
    case 6:
        registScnAnim(static_cast<nw4r::g3d::ResFileData*>(m_fileData->getData(Data_Type_Scene, 0, 0xFFFE)), 3);
        break;
    }
    loadStageAttrParam(m_fileData, 0x1E);
    initPosPokeTrainer(1, 0);
    createObjPokeTrainer(m_fileData, 0x65, "PokeTrainer00", m_pokeTrainerPos, NULL);
    m_appearTask = IfSnakeSmashAppearTask::create(m_fileData);
    gmGlobalModeMelee* melee = g_GameGlobal->m_modeMelee;
    if (melee != NULL && reinterpret_cast<metalgearMeleeView*>(&melee->m_meleeInitData)->m_gameMode == 7) {
        if (reinterpret_cast<u8*>(&melee->m_meleeInitData)[8] == 0x2D) {
            m_eventA = 1;
        }
        if (reinterpret_cast<u8*>(&melee->m_meleeInitData)[8] == 0xAD) {
            m_eventB = 1;
        }
    }
}

void stMetalgear::createObjBg(int index) {
    grMetalgearMainBg* ground = grMetalgearMainBg::create(0, "StgMetalgear00_base", "grMetalgearMainBg");
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setPosGimmickWork(m_posGimmick);
        ground->setStateWork(&m_bgState);
        ground->setStateWallWork(m_wallState);
    }
}

void stMetalgear::createObjEtc(int index) {
    grMetalgear* ground = NULL;
    switch (index) {
    case 0:
        switch (m_snow) {
        case 3:
            ground = grMetalgear::create(5, "StgMetalgear00_Aurora", "grMetalgearAurora");
            break;
        }
        break;
    case 1:
        switch (m_snow) {
        case 5:
            ground = grMetalgear::create(0x29, "StgMetalgear00_Snow2", "grMetalgearSnow02");
            break;
        case 6:
            ground = grMetalgear::create(0x2A, "StgMetalgear00_Snow3", "grMetalgearSnow03");
            break;
        }
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
    }
}

void stMetalgear::createObjWall(int index) {
    grMetalgearWall* ground;
    float* rotZ;
    u8* state;
    u8 type;
    switch (index) {
    case 3:
        ground = grMetalgearWall::create(1, "StgMetalgear_L_TOP", "grMetalgearWallLeftUp");
        state = &m_wallState[0];
        rotZ = &m_wallRotZ[0];
        type = 0;
        break;
    case 5:
        ground = grMetalgearWall::create(3, "StgMetalgear_R_TOP", "grMetalgearWallRightUp");
        state = &m_wallState[2];
        rotZ = &m_wallRotZ[1];
        type = 2;
        break;
    case 4:
        ground = grMetalgearWall::create(2, "StgMetalgear_L_UNDER", "grMetalgearWallLeftDown");
        state = &m_wallState[1];
        rotZ = NULL;
        type = 1;
        break;
    case 6:
        ground = grMetalgearWall::create(4, "StgMetalgear_R_UNDER", "grMetalgearWallRightDown");
        state = &m_wallState[3];
        rotZ = NULL;
        type = 3;
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setPosGimmickWork(m_posGimmick);
        ground->setRotZWork(rotZ);
        ground->setStateWork(state);
        ground->setType(type);
    }
}

void stMetalgear::createObjSearch(int index) {
    grMetalgearSearch* ground;
    switch (index) {
    case 7:
        ground = grMetalgearSearch::create(8, "StgMetalgearSLightTarget", "grMetalgearSearch");
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setPosGimmickWork(m_posGimmick);
        ground->setStateWork(&m_searchState);
        ground->setStateExclamationWork(&m_exclamationState);
        ground->setTgtWork(&m_searchTarget);
    }
}

void stMetalgear::createObjExclamation(int index) {
    grMetalgearExclamation* ground;
    switch (index) {
    case 8:
        ground = grMetalgearExclamation::create(9, "StgMetalgearExclamation", "grMetalgearExclamation");
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->m_posWork = &m_posGimmick[10];
        ground->m_stateWork = &m_exclamationState;
    }
}

void stMetalgear::createObjAttack(int index) {
    grMetalgearAttack* ground;
    Vec3f* pos;
    float* rotZ;
    u8* state;
    stCollisionWork* collision;
    u8 type;
    switch (index) {
    case 9:
        ground = grMetalgearAttack::create(0x5A, "nodeIndex", "grMetalgearAttackLeft");
        pos = &m_posGimmick[11];
        rotZ = &m_wallRotZ[0];
        state = &m_wallState[0];
        collision = &m_collLeft;
        type = 7;
        break;
    case 10:
        ground = grMetalgearAttack::create(0x5A, "nodeIndex", "grMetalgearAttackRight");
        pos = &m_posGimmick[12];
        rotZ = &m_wallRotZ[1];
        state = &m_wallState[2];
        collision = &m_collRight;
        type = 8;
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
        ground->setPosLimitWork(m_cameraRange);
        ground->setRotWork(rotZ);
        ground->setStateWork(state);
        ground->setType(type);
        createCollisionSelf(collision, ground, "nodeIndex", "nodeIndex", 0x8400);
        ground->disableCalcCollision();
    }
}

// The metalgear (kind by the sub kind of the match settings) comes with a chance of the stage data.
void stMetalgear::createObjMetalgear() {
    stMetalgearData* data = static_cast<stMetalgearData*>(m_stageData);
    if (data != NULL) {
        gmGlobalModeMelee* melee = g_GameGlobal->m_modeMelee;
        if (melee != NULL) {
            if (randf() <= data->unk38) {
                int index;
                switch (melee->m_meleeInitData.m_subStageKind) {
                case 1:
                    index = 0xE;
                    break;
                case 0:
                    index = 0xD;
                    break;
                case 2:
                    index = 0xF;
                    break;
                default:
                    return;
                }
                createObjMetalgear1(index);
            }
        }
    }
}

void stMetalgear::createObjMetalgear1(int index) {
    grMetalgearMetalgear* ground;
    grMetalgearMetalgear* ground2;
    switch (index) {
    case 0xD:
        ground = grMetalgearGekko::create(0x14, "StgMetalgearGekko_TopN", "grMetalgearGekko_0");
        ground2 = grMetalgearGekko::create(0x14, "StgMetalgearGekko_TopN", "grMetalgearGekko_1");
        m_metalgearKind = 0;
        break;
    case 0xE:
        ground = grMetalgearRay::create(0x14, "StgMetalgearRay_TopN", "grMetalgearRay");
        ground2 = NULL;
        m_metalgearKind = 1;
        break;
    case 0xF:
        ground = grMetalgearRex::create(0x14, "StgMetalgearRex_TopN", "grMetalgearRex");
        ground2 = NULL;
        m_metalgearKind = 2;
        break;
    default:
        ground = NULL;
        ground2 = NULL;
        break;
    }
    if (ground != NULL || ground2 != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setPosWork(m_posGimmick);
        ground->setStateWork(&m_metalgearState);
        ground->setType(m_metalgearKind);
        ground->setNumber(0);
        if (ground2 != NULL) {
            addGround(ground2);
            ground2->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
            ground2->setStageData(m_stageData);
            ground2->setPosWork(m_posGimmick);
            ground2->setStateWork(&m_metalgearState);
            ground2->setType(m_metalgearKind);
            ground2->setNumber(1);
        }
    }
}

void stMetalgear::createObjSnow() {
    g_ecMgr->pushsetCurrentGroup(0x2000000);
    switch (m_snow) {
    case 4:
        g_ecMgr->setEffect(ef_ptc_stg_metalgear_snow_1);
        break;
    case 5:
        createObjSnow1(0xB);
        g_ecMgr->setEffect(ef_ptc_stg_metalgear_snow_2);
        g_sndSystem->playSE(snd_se_stage_Metalgear_blizzard_01, 0, 0, 0, -1);
        break;
    case 6:
        createObjSnow1(0xC);
        g_ecMgr->setEffect(ef_ptc_stg_metalgear_snow_3);
        g_sndSystem->playSE(snd_se_stage_Metalgear_blizzard_02, 0, 0, 0, -1);
        break;
    }
    g_ecMgr->popCurrentGroup();
}

void stMetalgear::createObjSnow1(int index) {
    grMetalgear* ground;
    switch (index) {
    case 0xB:
        ground = grMetalgear::create(0x1F, "gr2_EffSnow2", "grMetalgearEffSnow02");
        break;
    case 0xC:
        ground = grMetalgear::create(0x20, "gr2_EffSnow3", "grMetalgearEffSnow03");
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
    }
}

void stMetalgear::update(float deltaFrame) {
    updateLimit();
    updateMetalgear(deltaFrame);
    updateWallLeft(deltaFrame);
    updateWallRight(deltaFrame);
    updateSearch();
}

void stMetalgear::updateLimit() {
    CameraController* camera = CameraController::getInstance();
    Vec3f rangeMin;
    rangeMin.m_x = camera->unk158;
    rangeMin.m_y = camera->unk160;
    rangeMin.m_z = 0.0f;
    m_cameraRange[0] = rangeMin;
    Vec3f rangeMax;
    rangeMax.m_x = camera->unk15C;
    rangeMax.m_y = camera->unk164;
    rangeMax.m_z = 0.0f;
    m_cameraRange[1] = rangeMax;
}

// The metalgear is chosen once: a time that it waits to come (and one that it stays), then it leaves when the fighters
// are far enough.
void stMetalgear::updateMetalgear(float deltaFrame) {
    if (m_metalgearKind != 9 && g_GameGlobal->m_modeMelee != NULL) {
        stMetalgearData* data = static_cast<stMetalgearData*>(m_stageData);
        if (data != NULL) {
            m_timerA = m_timerA - deltaFrame;
            if (m_timerA < 0.0f) {
                m_timerA = 0.0f;
            }
            switch (m_metalgearPhase) {
            case 0:
                m_timerC = data->unk3C + (data->unk40 - data->unk3C) * randf();
                m_timerB = data->unk44 + (data->unk48 - data->unk44) * randf();
                m_metalgearPhase = 1;
                break;
            case 1:
                if (m_goFlag != 0 && m_timerA == 0.0f) {
                    if (isMetalgear() == 1) {
                        m_metalgearState = 4;
                        m_bgState = 0;
                        m_metalgearPhase = 5;
                    }
                    m_timerA = 60.0f;
                }
                break;
            case 5:
                break;
            }
        }
    }
}

// The wall on the left breaks (phase 3 - 4) and rises again after a while (phase 1 - 2); the position data of the stage
// follow (the other wall decides which one).
void stMetalgear::updateWallLeft(float deltaFrame) {
    stMetalgearData* data = static_cast<stMetalgearData*>(m_stageData);
    if (data != NULL) {
        m_wallTimer[0] = m_wallTimer[0] - deltaFrame;
        if (m_wallTimer[0] < 0.0f) {
            m_wallTimer[0] = 0.0f;
        }
        switch (m_wallPhase[0]) {
        case 0:
            m_wallPhase[0] = 1;
        case 1:
            if (m_wallState[1] == 0) {
                if (m_wallState[0] != 0) {
                    m_wallState[0] = 1;
                    m_wallState[1] = 1;
                }
                m_wallTimer[0] = data->unk30 - data->unk34;
                int id;
                switch (m_wallPhase[1]) {
                case 1:
                    id = 0x6E;
                    break;
                default:
                    id = 0x6F;
                    break;
                }
                void* posData = m_fileData->getData(Data_Type_Model, id, 0xFFFE);
                if (posData != NULL) {
                    nw4r::g3d::ResFile posFile(posData);
                    m_stagePositions->loadPositionData(&posFile);
                }
                updateStagePositions();
                m_wallPhase[0] = 2;
            }
            break;
        case 2:
            if (m_wallTimer[0] == 0.0f) {
                m_wallState[0] = 2;
                m_wallState[1] = 2;
                m_wallTimer[0] = data->unk34;
                m_wallPhase[0] = 3;
            }
            break;
        case 3:
            if (m_wallTimer[0] == 0.0f) {
                m_wallState[0] = 3;
                m_wallState[1] = 3;
                g_ecMgr->setEffect(ef_ptc_stg_metalgear_tower_revival_l);
                int id;
                switch (m_wallPhase[1]) {
                case 1:
                    id = 100;
                    break;
                default:
                    id = 0x70;
                    break;
                }
                void* posData = m_fileData->getData(Data_Type_Model, id, 0xFFFE);
                if (posData != NULL) {
                    nw4r::g3d::ResFile posFile(posData);
                    m_stagePositions->loadPositionData(&posFile);
                }
                updateStagePositions();
                m_wallPhase[0] = 4;
            }
            break;
        case 4:
            if (m_wallState[0] == 6 && m_wallState[1] == 6) {
                g_sndSystem->playSE(snd_se_stage_Metalgear_wall_build, 0, 0, 0, -1);
                m_wallPhase[0] = 0;
            }
            break;
        }
    }
}

void stMetalgear::updateWallRight(float deltaFrame) {
    stMetalgearData* data = static_cast<stMetalgearData*>(m_stageData);
    if (data != NULL) {
        m_wallTimer[1] = m_wallTimer[1] - deltaFrame;
        if (m_wallTimer[1] < 0.0f) {
            m_wallTimer[1] = 0.0f;
        }
        switch (m_wallPhase[1]) {
        case 0:
            m_wallPhase[1] = 1;
            break;
        case 1:
            if (m_wallState[3] == 0) {
                if (m_wallState[2] != 0) {
                    m_wallState[2] = 1;
                    m_wallState[3] = 1;
                }
                m_wallTimer[1] = data->unk30 - data->unk34;
                int id;
                switch (m_wallPhase[0]) {
                case 1:
                    id = 0x70;
                    break;
                default:
                    id = 0x6F;
                    break;
                }
                void* posData = m_fileData->getData(Data_Type_Model, id, 0xFFFE);
                if (posData != NULL) {
                    nw4r::g3d::ResFile posFile(posData);
                    m_stagePositions->loadPositionData(&posFile);
                }
                updateStagePositions();
                m_wallPhase[1] = 2;
            }
            break;
        case 2:
            if (m_wallTimer[1] == 0.0f) {
                m_wallState[2] = 2;
                m_wallState[3] = 2;
                m_wallTimer[1] = data->unk34;
                m_wallPhase[1] = 3;
            }
            break;
        case 3:
            if (m_wallTimer[1] == 0.0f) {
                m_wallState[2] = 3;
                m_wallState[3] = 3;
                g_ecMgr->setEffect(ef_ptc_stg_metalgear_tower_revival_r);
                int id;
                switch (m_wallPhase[0]) {
                case 1:
                    id = 100;
                    break;
                default:
                    id = 0x6E;
                    break;
                }
                void* posData = m_fileData->getData(Data_Type_Model, id, 0xFFFE);
                if (posData != NULL) {
                    nw4r::g3d::ResFile posFile(posData);
                    m_stagePositions->loadPositionData(&posFile);
                }
                updateStagePositions();
                m_wallPhase[1] = 4;
            }
            break;
        case 4:
            if (m_wallState[2] == 6 && m_wallState[3] == 6) {
                g_sndSystem->playSE(snd_se_stage_Metalgear_wall_build, 0, 0, 0, -1);
                m_wallPhase[1] = 0;
            }
            break;
        }
    }
}

// The search light (the third place) moves toward its target place, not faster than the stage data allows.
void stMetalgear::updateSearch() {
    stMetalgearData* data = static_cast<stMetalgearData*>(m_stageData);
    if (data != NULL) {
        float speed;
        switch (m_searchState) {
        case 4:
            speed = data->unk88;
            break;
        default:
            speed = data->unk84;
            break;
        }
        Vec3f diff;
        bool near = false;
        diff = m_posGimmick[3] - m_posGimmick[2];
        if (metalgearIsNearZero(diff.m_x) && metalgearIsNearZero(diff.m_y) && metalgearIsNearZero(diff.m_z)) {
            near = true;
        }
        if (near != true) {
            float length = metalgearLength(diff);
            diff.normalize();
            if (length > speed) {
                diff.m_x = diff.m_x * speed;
                diff.m_y = diff.m_y * speed;
                diff.m_z = diff.m_z * speed;
            } else {
                diff.m_x = diff.m_x * length;
                diff.m_y = diff.m_y * length;
                diff.m_z = diff.m_z * length;
            }
            m_posGimmick[2] = m_posGimmick[2] + diff;
        }
    }
}

// The camera that lights the stage looks at the search light.
void stMetalgear::renderOpa() {
    gfTask::renderOpa();
    if (g_gfSceneRoot != NULL && *reinterpret_cast<nw4r::g3d::ScnRoot**>(reinterpret_cast<u8*>(g_gfSceneRoot) + 0x40) != NULL) {
        nw4r::g3d::Camera::PostureInfo info;
        info.tp = nw4r::g3d::Camera::POSTURE_AIM;
        Vec3f target;
        target = m_posGimmick[2];
        info.cameraTarget = target;
        info.cameraTwist = 0.0f;
        nw4r::g3d::Camera camera = (*reinterpret_cast<nw4r::g3d::ScnRoot**>(reinterpret_cast<u8*>(g_gfSceneRoot) + 0x40))->GetCamera(1);
        camera.SetPosture(info);
    }
}

// The metalgear comes when it is time (in a match by time) or when the one that is behind the others has taken enough
// damage (in a stock match): the kind of player that is looked for depends on how many fighters are in the match.
bool stMetalgear::isMetalgear() {
    if (m_stageData == NULL) {
        return false;
    }
    gmGlobalModeMelee* melee = g_GameGlobal->m_modeMelee;
    if (melee == NULL) {
        return false;
    }
    switch (reinterpret_cast<metalgearMeleeView*>(&melee->m_meleeInitData)->m_gameRule) {
    case 1: {
        gmResultInfo* resultInfo = g_GameGlobal->m_resultInfo;
        if (resultInfo == NULL) {
            return false;
        }
        u8 playerNo[4];
        u8 unk33[4];
        u16 damage[4];
        memset(playerNo, 0, 4);
        memset(unk33, 0, 4);
        memset(damage, 0, 8);
        u8 count = 0;
        u8 valid = 0;
        for (u32 i = 0; i < 4; i++) {
            int entryId = g_ftManager->getEntryId(i & 0xFF);
            if (g_ftManager->isValidEntryId(entryId)) {
                stMetalgearResultView* info =
                    &reinterpret_cast<stMetalgearResultView*>(resultInfo)[melee->m_playersInitData[i & 0xFF].m_playerNo];
                Fighter* fighter = g_ftManager->getFighter(entryId, -1);
                if (fighter != NULL && info->unk2E == 1) {
                    playerNo[count] = i;
                    unk33[count] = info->unk33;
                    // MATCH-ONLY: the owner is the virtual function at 0x2EC of the fighter (the vtable of the header is one slot short)
                    typedef metalgearOwnerView* (*OwnerGetter)(Fighter*);
                    OwnerGetter ownerGetter = *reinterpret_cast<OwnerGetter*>(*reinterpret_cast<u8**>(reinterpret_cast<u8*>(fighter) + 0x3C) + 0x2EC);
                    float dmg = ownerGetter(fighter)->getDamage();
                    damage[count] = (int)dmg;
                    count++;
                }
                valid++;
            }
        }
        if (count == 0) {
            return false;
        }
        u8 wanted;
        switch (valid) {
        case 1:
            wanted = 0;
            break;
        case 2:
        case 3:
            wanted = 1;
            break;
        case 4:
            wanted = 2;
            break;
        default:
            return false;
        }
        u8 j = 0;
        for (u32 n = count; n != 0; n--) {
            if (unk33[j] == wanted) {
                if (m_timerC < (float)damage[j]) {
                    return true;
                }
            }
            j++;
        }
        return false;
    }
    default:
        break;
    }
    if (getFrameRuleTime() == 0.0f) {
        return false;
    }
    return !(getFrameRuleTime() > m_timerB);
}

void stMetalgear::selectSnow() {
    stMetalgearData* data = static_cast<stMetalgearData*>(m_stageData);
    m_snow = 3;
    if (data != NULL) {
        float total = data->unk0C + data->unk08 + data->unk00 + data->unk04;
        float chance3 = data->unk0C / total;
        float chance2 = data->unk08 / total;
        float chance1 = data->unk04 / total;
        float r = randf();
        if (r < chance3) {
            m_snow = 3;
        } else if (r < chance3 + chance2) {
            m_snow = 4;
        } else if (r < chance1 + (chance3 + chance2)) {
            m_snow = 5;
        } else {
            m_snow = 6;
        }
    }
}

void stMetalgear::notifyEventInfoGo() {
    m_goFlag = 1;
}

bool stMetalgear::startAppear() {
    // The id of the title that is shown in the intro for the fighter of this kind (0x29 = none).
    static int appearIdTable[50] = { 0, 14, 15, 16, 33, 17, 18, 1, 27, 2, 19, 3, 4, 28, 5, 29, 30, 30, 30, 31, 20, 39, 21, 6, 32,
                                     22, 34, 35, 23, 7, 7, 7, 7, 7, 7, 8, 36, 9, 24, 10, 37, 12, 11, 25, 4, 6, 41, 41, 41, 41 };
    bool foundSnake = false;
    int count = 0;
    int ids[4];
    int entryCount = g_ftManager->getEntryCount();
    if (entryCount >= 4) {
        entryCount = 4;
    }
    ids[0] = 0x29;
    ids[1] = 0x29;
    ids[2] = 0x29;
    ids[3] = 0x29;
    int* out = ids;
    for (int i = 0; i < entryCount; i++) {
        ftManager* manager = g_ftManager;
        int kind = manager->getResultFighterGmKind(manager->getEntryIdFromIndex(i));
        if (foundSnake == false && kind == 0x2A) {
            foundSnake = true;
        } else {
            count++;
            *out = appearIdTable[kind];
            out++;
        }
    }
    bool result;
    if (foundSnake) {
        if (count == 0) {
            result = false;
        } else {
            u32 last = count - 1;
            for (int n = 0; n < 0x20; n++) {
                u32 a = randi(count);
                if (a >= last) {
                    a = last;
                }
                u32 b = randi(count);
                if (b >= last) {
                    b = last;
                }
                int tmp = ids[a];
                ids[a] = ids[b];
                ids[b] = tmp;
            }
            if (ids[0] == 0x29) {
                result = false;
            } else {
                if (m_appearTask != NULL) {
                    m_appearTask->start(ids[0]);
                }
                m_appearStarted = 1;
                result = true;
            }
        }
    } else {
        result = false;
    }
    return result;
}

void stMetalgear::endAppear() {
    if (isAppear() && m_appearTask != NULL) {
        m_appearTask->dead();
    }
}

bool stMetalgear::isStartAppearTimming() {
    return !m_appearStarted;
}

bool stMetalgear::isEventEnd(int param1, int* eventState, int* eventDecision) {
    if (m_eventA == 0 && m_eventB == 0) {
        return false;
    }
    if (m_searchTarget == 0 || (m_eventB != 0 && m_searchTarget == 1)) {
        *eventState = 6;
        *eventDecision = 3;
        return true;
    }
    return false;
}

bool stMetalgear::isAppear() {
    return m_appearTask->isPlaying();
}

void stMetalgear::forceStopAppear() {
    if (m_appearTask != NULL) {
        m_appearTask->stop(1);
    }
}

GXColor stMetalgear::getFinalTechniqColor() {
    u32 packed = 0x1400047D;
    return *reinterpret_cast<GXColor*>(&packed);
}
