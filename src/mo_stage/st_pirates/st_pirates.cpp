#include <cm/cm_camera_controller.h>
#include <cm/cm_quake.h>
#include <ec/ec_mgr.h>
#include <gf/gf_3d_scene.h>
#include <gf/gf_archive.h>
#include <gm/gm_global.h>
#include <gr/gr_gimmick_area_common.h>
#include <memory.h>
#include <mt/mt_prng.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <st/st_class_info.h>
#include <st/st_melee.h>
#include <st/st_trigger.h>
#include <st/st_utility.h>
#include <types.h>

#include <gr/gr_pirates_sling.h>
#include <gr/gr_pirates_tornado.h>
#include <st_pirates/gr_pirates_aground.h>
#include <st_pirates/gr_pirates_attack.h>
#include <st_pirates/gr_pirates_bg.h>
#include <st_pirates/gr_pirates_bomb.h>
#include <st_pirates/gr_pirates_cannon.h>
#include <st_pirates/gr_pirates_drop.h>
#include <st_pirates/gr_pirates_enkei.h>
#include <st_pirates/gr_pirates_lion_king.h>
#include <st_pirates/gr_pirates_rain.h>
#include <st_pirates/gr_pirates_sea_sky.h>
#include <st_pirates/gr_pirates_ship.h>
#include <st_pirates/gr_pirates_ship_ripple.h>
#include <st_pirates/gr_pirates_tower.h>
#include <st_pirates/gr_pirates_warning.h>
#include <st_pirates/gr_pirates_util.h>
#include <st_pirates/st_pirates.h>

// stTrigger::setWaterParam
extern "C" void fn_27_233C70(stTrigger* trigger, grGimmickWaterData* data);

stClassInfoImpl<Stages::Pirates, stPirates> stPirates::bss_loc_14;

stPirates* stPirates::create() {
    return new (Heaps::StageInstance) stPirates;
}

stPirates::stPirates() : stMelee("stPirates", Stages::Pirates) {
    unk1D8 = 0;
    m_sceneTimer = 0.0f;
    unk1E0 = 0;
    unk1E1 = 0;
    for (u8 i = 0; i < 0x12; i++) {
        mtx(i)->setIdentity();
    }
    m_posLimit[0].m_x = 0.0f;
    m_posLimit[0].m_y = 0.0f;
    m_posLimit[0].m_z = 0.0f;
    m_posLimit[1].m_x = 0.0f;
    m_posLimit[1].m_y = 0.0f;
    m_posLimit[1].m_z = 0.0f;
    unk55C = 9;
    unk55D = 0;
    unk560 = 0.0f;
    unk564 = 0.0f;
    unk568 = 0;
    unk569 = 0;
    unk56A = 1;
    unk56B = 0;
    unk56C = 0;
    unk56D = 0;
    unk56E = 7;
    unk56F = 0;
    unk570 = 0;
    memset(unk574, 0, sizeof(unk574));
    unk5D4.m_x = 0.0f;
    unk5D4.m_y = 0.0f;
    unk5D4.m_z = 0.0f;
    unk5E0 = 0.0f;
    unk5E4 = 0;
    unk5E8 = 0.0f;
    unk5EC = 0;
    unk5ED[0] = 7;
    unk5ED[1] = 7;
    unk5ED[2] = 7;
    unk5ED[3] = 7;
    unk5F1 = 7;
    m_waterData = NULL;
    m_waterTrigger = NULL;
    unk604 = 1.0f;
    unk608 = 0;
    unk609 = 7;
    unk60A = 7;
    unk60B = 7;
}

stPirates::~stPirates() {
    if (m_waterData != NULL) {
        delete m_waterData;
    }
    releaseArchive();
}

bool stPirates::loading() {
    return true;
}

// MATCH-ONLY: the bytes of the match settings the stage reads (their names are unknown): the game mode in the upper six bits
// of byte 8, the rule in the upper three bits of byte 9 and the event in byte 0x10.
struct stPiratesMeleeView {
    u8 _pad0[8];
    unsigned char m_mode : 6;
    unsigned char m_rest : 2;
    unsigned char m_rule : 3;
    unsigned char m_rest9 : 5;
    u8 _pad1[6];
    u8 m_event;
};

void stPirates::createObj() {
    testStageParamInit(m_fileData, 10);
    testStageDataInit(m_fileData, 0x14, 0x98);
    createObjBg(0);
    createObjSeaSky(1);
    createObjEnkei(3);
    createObjEnkei(2);
    createObjEnkei(4);
    createObjTornado(5);
    createObjTornado(6);
    createObjTower(7);
    createObjCannon(8);
    createObjCannon(9);
    createObjCannon(10);
    createObjCannon(11);
    createObjCannon(12);
    createObjCannon(13);
    createObjCannon(14);
    createObjCannon(15);
    createObjBomb(16);
    createObjAground(17);
    createObjShip(18);
    createObjShipRipple(19);
    createObjRain(20);
    createObjLionKing(21);
    createObjSling(22);
    createObjDrop(23);
    createObjDrop(24);
    createObjDrop(25);
    createObjDrop(26);
    createObjWarning(27);
    createObjRain(28);
    createObjAttack(29);
    createObjAttack(30);
    createObjAttack(31);
    createObjSwimArea();
    initCameraParam();
    void* posData = m_fileData->getData(Data_Type_Model, 100, 0xFFFE);
    if (posData != NULL) {
        nw4r::g3d::ResFile posFile(posData);
        createStagePositions(&posFile);
    } else {
        createStagePositions();
    }
    createWind2ndOnly();
    loadStageAttrParam(m_fileData, 0x1E);
    registScnAnim(static_cast<nw4r::g3d::ResFileData*>(m_fileData->getData(Data_Type_Scene, 0, 0xFFFE)), 0);
    unk55C = 0;
    initPosPokeTrainer(1, 0);
    createObjPokeTrainer(m_fileData, 0x65, "PokeTrainer00", m_pokeTrainerPos, NULL);
    if (g_GameGlobal->m_modeMelee != NULL) {
        stPiratesMeleeView* melee = reinterpret_cast<stPiratesMeleeView*>(g_GameGlobal->m_modeMelee);
        if (melee->m_mode == 7 && melee->m_event == 0x27) {
            unk608 = 1;
        }
    }
}

// The sky and the sea behind the ship.
void stPirates::createObjBg(int index) {
    grPiratesBg* ground = grPiratesBg::create(1, "StgPirates00", "grPiratesMainBg");
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setMatrixGimmickWork(mtx(0));
        ground->setEventWork(&unk568);
        ground->setShowFlgLionKingWork(&unk56D);
        ground->setStateLionKingWork(&unk56E);
        ground->setCtrlFrame(&m_sceneTimer);
    }
}

// The far view: two clouds and the island.
void stPirates::createObjEnkei(int index) {
    grPiratesEnkei* ground;
    Matrix* work;
    float angleDelta;
    switch (index) {
    case 3:
        ground = grPiratesEnkei::create(7, "StgPirates00Ea", "grPiratesCloud1");
        angleDelta = -1.15f;
        work = mtx(5);
        break;
    case 2:
        ground = grPiratesEnkei::create(8, "StgPirates00Eb", "grPiratesCloud2");
        angleDelta = -1.0083333f;
        work = mtx(5);
        break;
    case 4:
        ground = grPiratesEnkei::create(9, "StgPirates00Ec", "grPiratesIsland");
        angleDelta = -1.15f;
        work = mtx(5);
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setMatrixWork(work);
        ground->setAngleDelta(angleDelta);
        ground->setEventWork(&unk568);
        ground->setCtrlFrame(&m_sceneTimer);
        ground->setEventTotalFrame(&unk564);
    }
}

// The ship.
void stPirates::createObjShip(int index) {
    grPiratesShip* ground = grPiratesShip::create(10, "StgPirates00K", "grPiratesShip");
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setMatrixWork(mtx(1));
        ground->setMatrixGimmickWork(mtx(0));
        ground->setEventWork(&unk568);
        ground->setCountAground(&unk56C);
        ground->setCtrlFrame(&m_sceneTimer);
        createCollision(m_fileData, 3, ground);
    }
}

// The waves around the ship.
void stPirates::createObjShipRipple(int index) {
    grPiratesShipRipple* ground = grPiratesShipRipple::create(0xB, "StgPirates00KH", "grPiratesShipRipple");
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setMatrixWork(mtx(0));
        ground->setEventWork(&unk568);
        ground->setCtrlFrame(&m_sceneTimer);
    }
}

// The lion at the bow.
void stPirates::createObjLionKing(int index) {
    grPiratesLionKing* ground = grPiratesLionKing::create(0xC, "StgPirates00R", "grPiratesLionKing");
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setMatrixWork(mtx(2));
        ground->setStateWork(&unk56E);
        ground->setEventWork(&unk568);
        ground->setShowFlgWork(&unk56D);
        ground->setCtrlFrame(&m_sceneTimer);
        ground->setEventTotalFrame(&unk564);
        createCollision(m_fileData, 4, ground);
    }
}

// The catapult.
void stPirates::createObjSling(int index) {
    grPiratesSling* ground = grPiratesSling::create(0xD, "StgPirates00S", "grPiratesSling");
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setMatrixWork(mtx(7));
        ground->setEventWork(&unk568);
        createCollision(m_fileData, 5, ground);
    }
}

// The two tornados (the tornado itself, and the scene it is seen in).
void stPirates::createObjTornado(int index) {
    grPiratesTornado* ground;
    Matrix* work;
    switch (index) {
    case 6:
        ground = grPiratesTornado::create(0xE, "StgPirates00TS", "grPiratesTornado");
        work = mtx(4);
        break;
    case 5:
        ground = grPiratesTornado::create(0xF, "StgPirates00T", "grPiratesTornadoScene");
        work = mtx(3);
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setMatrixWork(work);
        ground->setEventWork(&unk568);
        ground->setCtrlFrame(&m_sceneTimer);
    }
}

// The reef the ship runs aground on.
void stPirates::createObjAground(int index) {
    grPiratesAground* ground;
    switch (index) {
    case 17:
        ground = grPiratesAground::create(0x13, "StgPirates00Z", "grPiratesAground");
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setEventWork(&unk568);
        ground->setCountAground(&unk56C);
        ground->setCtrlFrame(&m_sceneTimer);
        ground->setMatrixGimmickWork(mtx(0));
        createCollision(m_fileData, 6, ground);
    }
}

// The sea and the sky seen from the ship.
void stPirates::createObjSeaSky(int index) {
    grPiratesSeaSky* ground = grPiratesSeaSky::create(0x10, "StgPirates00U", "grPiratesSeaSky");
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setMatrixWork(mtx(0));
        ground->setMatrixGimmickWork(mtx(0));
        ground->setEventWork(&unk568);
        ground->setCtrlFrame(&m_sceneTimer);
    }
}

// The tower in the background.
void stPirates::createObjTower(int index) {
    grPiratesTower* ground = grPiratesTower::create(0x11, "StgPirates00Y", "grPiratesTower");
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setMatrixGimmickWork(mtx(0));
        ground->setEventWork(&unk568);
        ground->setCtrlFrame(&m_sceneTimer);
    }
}

// The eight cannons on the deck.
void stPirates::createObjCannon(int index) {
    grPiratesCannon* ground;
    Matrix* work;
    u8 id;
    switch (index) {
    case 8:
        ground = grPiratesCannon::create(0x12, "StgPirates00YH", "grPiratesCannon1");
        work = mtx(8);
        id = 0;
        break;
    case 9:
        ground = grPiratesCannon::create(0x12, "StgPirates00YH", "grPiratesCannon2");
        work = mtx(9);
        id = 1;
        break;
    case 10:
        ground = grPiratesCannon::create(0x12, "StgPirates00YH", "grPiratesCannon3");
        work = mtx(10);
        id = 2;
        break;
    case 11:
        ground = grPiratesCannon::create(0x12, "StgPirates00YH", "grPiratesCannon4");
        work = mtx(11);
        id = 3;
        break;
    case 12:
        ground = grPiratesCannon::create(0x12, "StgPirates00YH", "grPiratesCannon5");
        work = mtx(12);
        id = 4;
        break;
    case 13:
        ground = grPiratesCannon::create(0x12, "StgPirates00YH", "grPiratesCannon6");
        work = mtx(13);
        id = 5;
        break;
    case 14:
        ground = grPiratesCannon::create(0x12, "StgPirates00YH", "grPiratesCannon7");
        work = mtx(14);
        id = 6;
        break;
    case 15:
        ground = grPiratesCannon::create(0x12, "StgPirates00YH", "grPiratesCannon8");
        work = mtx(15);
        id = 7;
        break;
    default:
        work = NULL;
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setMatrixWork(work);
        ground->setEventWork(&unk568);
        ground->setID(id);
        ground->setIDActive(&unk570);
        ground->setFireWork(&unk56F);
        ground->setPosTgtWork(reinterpret_cast<float*>(&unk5D4));
        ground->setCtrlFrame(&m_sceneTimer);
    }
}

// The bomb.
void stPirates::createObjBomb(int index) {
    grPiratesBomb* ground = grPiratesBomb::create(2, "StgPirates00B", "grPiratesBomb");
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setMatrixWork(mtx(0));
        ground->setMatrixGimmickWork(mtx(0));
        ground->setMatrixIndex(&unk570);
        ground->setEventWork(&unk568);
        ground->setFireWork(&unk56F);
        ground->setPosTgtWork(reinterpret_cast<float*>(&unk5D4));
    }
}

// The four things that drop onto the deck (two barrels, two bottles).
void stPirates::createObjDrop(int index) {
    grPiratesDrop* ground;
    u8* state;
    switch (index) {
    case 23:
        ground = grPiratesDrop::create(3, "StgPirates00Da1", "grPiratesDropBarrelA");
        state = &unk5ED[0];
        break;
    case 24:
        ground = grPiratesDrop::create(4, "StgPirates00Da2", "grPiratesDropBarrelB");
        state = &unk5ED[1];
        break;
    case 25:
        ground = grPiratesDrop::create(5, "StgPirates00Db1", "grPiratesDropBottleA");
        state = &unk5ED[2];
        break;
    case 26:
        ground = grPiratesDrop::create(6, "StgPirates00Db2", "grPiratesDropBottleB");
        state = &unk5ED[3];
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setMatrixWork(mtx(6));
        ground->setPosLimitWork(reinterpret_cast<float*>(m_posLimit));
        ground->setStateWork(state);
    }
}

// The warning of the tornado.
void stPirates::createObjWarning(int index) {
    grPiratesWarning* ground;
    switch (index) {
    case 27:
        ground = grPiratesWarning::create(0x1E, "gr2_StgPirates00W", "grPiratesWarning");
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setStateWork(&unk5F1);
    }
}

// The rain (two kinds).
void stPirates::createObjRain(int index) {
    grPiratesRain* ground;
    switch (index) {
    case 20:
        ground = grPiratesRain::create(0x20, "Eff_ame2", "grPiratesRain");
        break;
    case 28:
        ground = grPiratesRain::create(0x1F, "gr2_EffAme", "grPiratesRainGR2");
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setEventWork(&unk568);
        ground->setCtrlFrame(&m_sceneTimer);
    }
}

// The attacks of the ship: the bottom of the ship (two) and the reef.
void stPirates::createObjAttack(int index) {
    grPiratesAttack* ground;
    Matrix* work;
    u8* state;
    int type;
    switch (index) {
    case 29:
        ground = grPiratesAttackBottom::create(0x28, "nodeIndex", "grPiratesAttackBottomA");
        work = mtx(16);
        state = &unk609;
        type = 0;
        break;
    case 30:
        ground = grPiratesAttackBottom::create(0x28, "nodeIndex", "grPiratesAttackBottomB");
        work = mtx(16);
        state = &unk60A;
        type = 1;
        break;
    case 31:
        ground = grPiratesAttackAground::create(0x28, "nodeIndex", "grPiratesAttackAground");
        work = mtx(17);
        state = &unk60B;
        type = 2;
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setMtxWork(work);
        ground->setStateWork(state);
        ground->setType(type);
    }
}

// The water of the stage: the swimming area (the trigger) is a box around the ship.
void stPirates::createObjSwimArea() {
    m_waterData = new (Heaps::StageInstance) grGimmickWaterData;
    if (m_waterData != NULL) {
        memset(m_waterData, 0, sizeof(grGimmickWaterData));
        m_waterData->m_swimHeight = 0.0f;
        m_waterData->m_canDrown = true;
        m_waterData->m_speed = 1.0f;
        grGimmickWaterData* area = m_waterData;
        area->m_areaData.m_offsetPos.m_x = 0.0f;
        area->m_areaData.m_offsetPos.m_y = -25.0f;
        area->m_areaData.m_range.m_x = 500.0f;
        area->m_areaData.m_range.m_y = 25.0f;
        m_waterTrigger = g_stTriggerMng->createTrigger(Gimmick::Area_Water, -1);
        m_waterTrigger->setWaterTrigger(m_waterData);
    }
}

void stPirates::update(float deltaFrame) {
    unk1E0 = 0;
    m_sceneTimer = m_sceneTimer - deltaFrame;
    if (m_sceneTimer < 0.0f) {
        unk1E0 = 1;
        m_sceneTimer += 480.0f;
    }
    if (m_isDevil == 1) {
        setCameraLimitRange(-240.0f, 240.0f, 170.5f, -20.0f);
    } else {
        resetCameraLimitRange();
    }
    updateLimit(deltaFrame);
    updateScene(deltaFrame);
    updateEvent(deltaFrame);
    updateSea(deltaFrame);
    updateDrop(deltaFrame);
}

// The left/top and right/bottom limit of the camera.
void stPirates::updateLimit(float deltaFrame) {
    CameraController* camera = CameraController::getInstance();
    float minY = camera->unk160;
    float minX = camera->unk158;
    m_posLimit[0].m_x = minX;
    m_posLimit[0].m_y = minY;
    m_posLimit[0].m_z = 0.0f;
    float maxY = camera->unk164;
    float maxX = camera->unk15C;
    m_posLimit[1].m_x = maxX;
    m_posLimit[1].m_y = maxY;
    m_posLimit[1].m_z = 0.0f;
}

// The wind of the stage while it rains (the rain blows the fighters; the values are the ones the original sets).
#define PIRATES_SET_WIND(speed, vector, p3c, p40, p48) \
    m_wind2ndData->m_speed = speed;                    \
    m_wind2ndData->m_vector = vector;                  \
    m_wind2ndData->m_60 = p3c;                         \
    m_wind2ndData->m_64 = p40;                         \
    m_wind2ndData->m_72 = p48;                         \
    m_wind2ndTrigger->setWindParam(m_wind2ndData, 0);  \
    m_wind2ndTrigger->setAreaSleep(false)

// The event state machine. Step 0 picks the time until the first event, step 1 waits for it and picks the event, 3 picks the
// weather, 4..6 start it, 7..8 and 10..14 are the tornado, the bombs and the running aground.
void stPirates::updateEvent(float deltaFrame) {
    stPiratesData* data = static_cast<stPiratesData*>(m_stageData);
    if (data != NULL) {
        unk560 -= deltaFrame;
        if (unk560 < 0.0f) {
            unk560 = 0.0f;
        }
        unk564 -= deltaFrame;
        if (unk564 < 0.0f) {
            unk564 = 0.0f;
        }
        switch (unk55D) {
        case 0:
            if (unk56A == 1) {
                float range;
                PIRATES_RANDF_RANGE(range, data->unk3C, data->unk38);
                unk56A = 0;
                unk560 = range;
            } else {
                float range;
                PIRATES_RANDF_RANGE(range, data->unk44, data->unk40);
                unk560 = range;
            }
            if (unk608 == 1) {
                unk560 = 0.0f;
            }
            unk609 = 0;
            unk60A = 0;
            unk60B = 7;
            unk55D = 1;
            break;
        case 1:
            if (unk560 == 0.0f) {
                updateEventSelect(deltaFrame);
            }
            break;
        case 3:
            updateEventWeatherSelect(deltaFrame);
            break;
        case 4:
        case 5:
        case 6:
            if (unk1E0 == 1) {
                updateEventWeather(deltaFrame);
            }
            break;
        case 7:
            if (unk1E0 == 1) {
                requestEventTornado();
                unk568 = 5;
                unk55D = 8;
            }
            break;
        case 8:
            if (unk1E0 == 1) {
                unk1E1 = 3;
                unk564 = 1440.0f;
                unk568 = 6;
                unk55D = 9;
            }
            break;
        case 9:
            if (unk564 < 190.0f) {
                PIRATES_SET_WIND(0.25f, 340.0f, 80.0f, 0.6f, 40);
                unk609 = 0;
                unk60A = 0;
            } else if (unk564 < 470.0f) {
                PIRATES_SET_WIND(0.6f, 0.0f, 22.0f, 1.1f, 80);
            } else if (unk564 < 700.0f) {
                m_wind2ndTrigger->setAreaSleep(true);
            } else if (unk564 < 960.0f) {
                PIRATES_SET_WIND(0.6f, 0.0f, 22.0f, 1.1f, 80);
                unk609 = 7;
                unk60A = 7;
            }
            if (unk1E0 == 1) {
                if (--unk1E1 == 0) {
                    if (unk608 == 1) {
                        unk55D = 7;
                    } else {
                        unk569 = unk568;
                        updateEventWeatherSelect(deltaFrame);
                        updateEventWeather(deltaFrame);
                    }
                }
            }
            break;
        case 10:
            if (unk1E0 == 1) {
                requestEventBomb();
                float range;
                PIRATES_RANDF_RANGE(range, data->unk70, data->unk6C);
                unk568 = 7;
                unk5E0 = 1920.0f;
                unk1E1 = 4;
                unk564 = 1920.0f;
                unk55D = 11;
                unk560 = range;
            }
            break;
        case 11:
            if (unk1E0 == 1) {
                if (--unk1E1 == 0) {
                    unk569 = unk568;
                    updateEventWeatherSelect(deltaFrame);
                    updateEventWeather(deltaFrame);
                }
            } else {
                updateEventBomb(deltaFrame);
            }
            break;
        case 12:
            if (unk1E0 == 1) {
                requestEventAground();
                unk56B = 0;
                unk1E1 = 1;
                unk568 = 8;
                unk55D = 13;
            }
            break;
        case 13:
            if (unk1E0 == 1) {
                if (--unk1E1 != 0) {
                    return;
                }
                unk568 = 9;
                switch (unk56B) {
                case 0:
                    unk1E1 = 3;
                    unk564 = 2400.0f;
                    unk5F1 = 7;
                    unk56B = 1;
                    break;
                case 1: {
                    float range;
                    PIRATES_RANDF_RANGE(range, data->unk8C, data->unk88);
                    unk56B = 2;
                    unk1E1 = (u8)(int)(range / 480.0f + 1.0f);
                    unk56C = unk1E1;
                    break;
                }
                case 2:
                    unk1E1 = 1;
                    unk56B = 0;
                    unk55D = 14;
                    break;
                }
            }
            if (unk56B != 0 && unk5F1 == 7 && unk564 < data->unk80 + 1500.0f) {
                unk5F1 = 0;
            }
            if (unk564 < 1500.0f && unk564 > 480.0f) {
                unk604 = 0.25f;
            } else {
                unk604 = 1.0f;
            }
            fn_27_233C70(m_waterTrigger, m_waterData);
            switch (unk56B) {
            case 1:
                if (unk564 < 1020.0f) {
                    unk60A = 1;
                    unk60B = 7;
                } else if (unk564 < 1100.0f) {
                    unk60B = 0;
                } else if (unk564 < 1430.0f) {
                    unk60B = 1;
                } else if (unk564 < 1500.0f) {
                    unk609 = 1;
                    unk60A = 7;
                    unk60B = 0;
                } else {
                    unk609 = 0;
                    unk60A = 0;
                }
                break;
            case 2:
                unk564 = 480.0f;
                break;
            }
            break;
        case 14:
            if (unk1E0 == 1) {
                if (--unk1E1 != 0) {
                    return;
                }
                unk569 = unk568;
                updateEventWeatherSelect(deltaFrame);
                updateEventWeather(deltaFrame);
            }
            if (unk564 < 320.0f) {
                unk609 = 0;
                unk60A = 0;
            } else if (unk564 < 480.0f) {
                unk60A = 7;
            } else {
                unk609 = 0;
                unk60A = 0;
            }
            break;
        }
    }
}

// Picks the next event. The chances come from the stage data; the event that was on last time is made less likely.
void stPirates::updateEventSelect(float deltaFrame) {
    stPiratesData* data = static_cast<stPiratesData*>(m_stageData);
    if (data != NULL) {
        if (unk608 == 1) {
            unk55D = 7;
        } else if (g_GameGlobal->m_modeMelee != NULL) {
            u8 timeIsUp = 0;
            switch (reinterpret_cast<stPiratesMeleeView*>(g_GameGlobal->m_modeMelee)->m_rule) {
            case 0:
                if (getFrameRuleTime() != 0.0f && getFrameRuleTime() < data->unk5C) {
                    timeIsUp = 1;
                }
                break;
            }
            if (timeIsUp == 1) {
                unk55D = 3;
            } else {
                float chanceFine = data->unk48;
                float chanceCloud = data->unk4C;
                float chanceRain = data->unk50;
                float chanceTornado = data->unk54;
                switch (unk569) {
                case 0:
                    break;
                case 1:
                    chanceFine *= data->unk58;
                    break;
                case 2:
                    chanceFine *= data->unk58;
                    break;
                case 3:
                    chanceFine *= data->unk58;
                    break;
                case 4:
                case 5:
                    break;
                case 6:
                    chanceCloud *= data->unk58;
                    break;
                case 7:
                    chanceRain *= data->unk58;
                    break;
                case 8:
                    break;
                case 9:
                    chanceTornado *= data->unk58;
                    break;
                }
                float total = chanceFine + chanceCloud + chanceRain + chanceTornado;
                chanceFine = chanceFine / total;
                chanceCloud = chanceCloud / total;
                chanceRain = chanceRain / total;
                chanceTornado = chanceTornado / total;
                float rnd = randf();
                if (rnd < chanceTornado) {
                    unk55D = 12;
                } else if (rnd < chanceRain + chanceTornado) {
                    unk55D = 10;
                } else if (rnd < chanceTornado + (chanceCloud + chanceRain)) {
                    unk55D = 7;
                } else if (rnd < chanceTornado + (chanceRain + (chanceFine + chanceCloud))) {
                    unk569 = 4;
                    unk55D = 3;
                } else {
                    unk55D = 0;
                }
            }
        }
    }
}

// Picks the weather: rain, clouds or fine.
void stPirates::updateEventWeatherSelect(float deltaFrame) {
    stPiratesData* data = static_cast<stPiratesData*>(m_stageData);
    if (data != NULL) {
        float total = data->unk60 + data->unk64;
        float ratioRain = data->unk60 / total;
        float ratioCloud = data->unk64 / total;
        float rnd = randf();
        if (rnd < ratioCloud) {
            unk55D = 5;
        } else if (rnd < ratioRain + ratioCloud) {
            unk55D = 4;
        } else {
            unk55D = 0;
        }
    }
}

// Starts the weather that was picked.
void stPirates::updateEventWeather(float deltaFrame) {
    switch (unk55D) {
    case 4:
        requestEventWeatherFine();
        unk568 = 1;
        unk55D = 0;
        break;
    case 5:
        requestEventWeatherCloud();
        unk568 = 2;
        unk55D = 0;
        break;
    case 6:
        requestEventWeatherRain();
        unk568 = 3;
        unk55D = 0;
        break;
    }
    if (unk569 == 4) {
        unk569 = unk568;
    }
}

// The bombs: the cannon that fires and where the bomb is shot at.
void stPirates::updateEventBomb(float deltaFrame) {
    stPiratesData* data = static_cast<stPiratesData*>(m_stageData);
    if (data != NULL) {
        unk5E0 -= deltaFrame;
        if (unk5E0 < 0.0f) {
            unk5E0 = 0.0f;
        }
        switch (unk56F) {
        case 0: {
                // HYPOTHESIS: the three thirds of the bomb event (1920 frames) pick the cannon that fires.
                float eventFrames = 1920.0f;
                if (unk5E0 < 0.33333334f * eventFrames) {
                    unk570 = 4;
                } else if (unk5E0 < 0.6666667f * eventFrames) {
                    unk570 = 3;
                } else {
                    unk570 = 2;
                }
                Vec3f start;
                Vec3f dir;
                Vec3f hit;
                Vec3f normal;
                float width = m_posLimit[1].m_x - m_posLimit[0].m_x;
                float seaZ = mtx(6)->m[2][3];
                float seaY = mtx(6)->m[1][3];
                float seaX = mtx(6)->m[0][3];
                Vec3f seaPos(seaX, seaY, seaZ);
                float rnd = randf();
                float full = 0.7f * width;
                float half = 0.35f * width;
                start.m_x = -half + full * rnd;
                if (start.m_x > -40.0f && start.m_x < 40.0f) {
                    start.m_y = 20.0f;
                } else {
                    start.m_y = 100.0f;
                }
                start.m_z = 0.0f;
                dir.m_x = 0.0f;
                dir.m_y = -500.0f;
                dir.m_z = 0.0f;
                if (!stRayCheck(&start, &dir, &hit, &normal, true, NULL, false, 1) || normal.m_y <= 0.0f) {
                    unk5D4.m_x = start.m_x;
                    unk5D4.m_y = start.m_y;
                    unk5D4.m_y = seaPos.m_y;
                    unk5D4.m_z = 0.0f;
                } else {
                    unk5D4.m_x = hit.m_x;
                    unk5D4.m_y = hit.m_y;
                    unk5D4.m_z = 0.0f;
                }
                if (unk5E0 >= 480.0f && unk5E0 <= 1440.0f) {
                    unk56F = 1;
                }
            break;
        }
        case 1:
            break;
        case 2: {
            float range;
            PIRATES_RANDF_RANGE(range, data->unk70, data->unk6C);
            unk56F = 0;
            unk560 = range;
            break;
        }
        }
    }
}

// Changes the scene animation when the loop starts again.
void stPirates::updateScene(float deltaFrame) {
    if (unk1E0 != 0) {
        switch (unk55C) {
        case 3:
            unk55C = 1;
            break;
        case 4:
            unk55C = 2;
            break;
        case 5:
            unk55C = 0;
            break;
        case 6:
            unk55C = 2;
            break;
        case 7:
            unk55C = 0;
            break;
        case 8:
            unk55C = 1;
            break;
        default:
            return;
        }
        registScnAnim(static_cast<nw4r::g3d::ResFileData*>(m_fileData->getData(Data_Type_Scene, 0, 0xFFFE)), unk55C);
        g_gfSceneRoot->setCurrentFrame(480.0f - m_sceneTimer);
    }
}

// The water follows the sea of the scene (the height of the matrix), and the camera never looks below it.
void stPirates::updateSea(float deltaFrame) {
    if (m_waterTrigger != NULL && m_waterData != NULL) {
        stPiratesData* data = static_cast<stPiratesData*>(m_stageData);
        if (data != NULL) {
            Vec3f sea(mtx(6)->m[0][3], mtx(6)->m[1][3], mtx(6)->m[2][3]);
            m_waterData->m_swimHeight = sea.m_y;
            m_waterData->m_speed = data->waterVelocity * unk604;
            grGimmickWaterData* area = m_waterData;
            area->m_areaData.m_range.m_x = (m_posLimit[1].m_x - m_posLimit[0].m_x) + 500.0f;
            float height = (sea.m_y - m_posLimit[1].m_y) + 50.0f;
            area->m_areaData.m_range.m_y = height;
            area->m_areaData.m_offsetPos.m_x = sea.m_x;
            area->m_areaData.m_offsetPos.m_y = sea.m_y - height * 0.5f;
            fn_27_233C70(m_waterTrigger, m_waterData);
            float shift = 0.0f;
            CameraController* camera = CameraController::getInstance();
            Rect2D range = *reinterpret_cast<Rect2D*>(reinterpret_cast<u8*>(camera) + 0x148);
            if (range.m_down < sea.m_y) {
                range.m_down = sea.m_y;
                cmStageParam* param = &CameraController::getInstance()->m_stageCameraParam;
                if (param != NULL) {
                    float cosine = nw4r::math::CosFIdx(10.666667f);
                    float sine = nw4r::math::SinFIdx(10.666667f);
                    range.m_down -= 0.5f * (param->m_verticalRotationFactor * (sine / cosine));
                }
                shift = sea.m_y - range.m_down;
            }
            CameraController::getInstance()->setCameraRange(&range);
            *reinterpret_cast<float*>(reinterpret_cast<u8*>(CameraController::getInstance()) + 0x18C) = shift;
        }
    }
}

// The things that drop onto the deck: every 600 frames one of the four (barrel, barrel, bottle, bottle) falls.
void stPirates::updateDrop(float deltaFrame) {
    unk5E8 -= deltaFrame;
    if (unk5E8 < 0.0f) {
        unk5E8 = 0.0f;
    }
    switch (unk5E4) {
    case 0:
        unk5E4 = 1;
        unk5E8 = 600.0f;
        break;
    case 1:
        if (unk5E8 == 0.0f) {
            float rnd = randf();
            if (rnd < 0.25f) {
                unk5EC = 0;
            } else if (rnd < 0.5f) {
                unk5EC = 1;
            } else if (rnd < 0.75f) {
                unk5EC = 2;
            } else {
                unk5EC = 3;
            }
            unk5ED[unk5EC] = 0;
            unk5E4 = 2;
        }
        break;
    case 2:
        if (unk5ED[unk5EC] != 0) {
            unk5E4 = 0;
        }
        break;
    }
}

// Changes the scene animation to the one of the weather: [current] -> new. The scene loops every 480 frames.
#define PIRATES_REQUEST_SCENE(zero, one, two, other)                                                             \
    switch (unk55C) {                                                                                            \
    case 0:                                                                                                      \
        unk55C = zero;                                                                                           \
        break;                                                                                                   \
    case 1:                                                                                                      \
        unk55C = one;                                                                                            \
        break;                                                                                                   \
    case 2:                                                                                                      \
        unk55C = two;                                                                                            \
        break;                                                                                                   \
    default:                                                                                                     \
        unk55C = other;                                                                                          \
        break;                                                                                                   \
    }                                                                                                            \
    registScnAnim(static_cast<nw4r::g3d::ResFileData*>(m_fileData->getData(Data_Type_Scene, 0, 0xFFFE)), unk55C); \
    g_gfSceneRoot->setCurrentFrame(480.0f - m_sceneTimer)

void stPirates::requestEventWeatherFine() {
    PIRATES_REQUEST_SCENE(0, 5, 7, 5);
}

void stPirates::requestEventWeatherCloud() {
    PIRATES_REQUEST_SCENE(3, 1, 8, 7);
    switch (unk55C) {
    case 3:
        m_sndGenerator.playSE(snd_se_stage_Pirates_02, 0, 0, -1);
        break;
    }
}

void stPirates::requestEventWeatherRain() {
    PIRATES_REQUEST_SCENE(4, 6, 2, 6);
}

void stPirates::requestEventTornado() {
    PIRATES_REQUEST_SCENE(4, 6, 2, 6);
    switch (unk55C) {
    case 4:
        m_sndGenerator.playSE(snd_se_stage_Pirates_02, 0, 0, -1);
        break;
    }
}

// The bomb and the running aground only move the scene to the frame of the loop.
void stPirates::requestEventBomb() {
    g_gfSceneRoot->setCurrentFrame(480.0f - m_sceneTimer);
}

void stPirates::requestEventAground() {
    g_gfSceneRoot->setCurrentFrame(480.0f - m_sceneTimer);
}

// The event ends when the loops are over (one or two loops from the end, by the times of the stage data).
bool stPirates::isEventEnd(int param1, int* eventState, int* eventDecision) {
    if (unk608 == 0) {
        return false;
    }
    stPiratesData* data = static_cast<stPiratesData*>(m_stageData);
    if (data == NULL) {
        return false;
    }
    if ((unk1E1 == 2 && 480.0f - m_sceneTimer > data->unk90 - 480.0f) ||
        (unk1E1 == 1 && 480.0f - m_sceneTimer < (data->unk94 - 480.0f) - 480.0f)) {
        *eventDecision = 1;
    } else {
        *eventDecision = 0;
    }
    return true;
}
