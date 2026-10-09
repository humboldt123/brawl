#include <cm/cm_camera_controller.h>
#include <ec/ec_mgr.h>
#include <gf/gf_application.h>
#include <gf/gf_archive.h>
#include <gf/gf_camera.h>
#include <gf/gf_scene.h>
#include <gm/gm_global.h>
#include <gm/gm_lib.h>
#include <gr/collision/gr_collision.h>
#include <if/if_mngr.h>
#include <it/it_manager.h>
#include <math.h>
#include <memory.h>
#include <mt/mt_prng.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <nw4r/math/math_arithmetic.h>
#include <nw4r/math/math_triangular.h>
#include <snd/snd_id.h>
#include <snd/snd_system.h>
#include <st/st_class_info.h>
#include <st/st_melee.h>
#include <st/st_positions.h>
#include <st/st_trigger.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <types.h>

#include <st_homerun/gr_homerun_anim.h>
#include <st_homerun/st_homerun.h>

// Unnamed helpers of other modules the stage calls directly (the names are the ones of the sora maps).
// gfCamera::init
extern "C" void fn_80018610(gfCameraManager* manager);
// gmlib::gmLotFigure: picks the figure that is thrown into the stadium
extern "C" u32 fn_800516F0();
// CameraController::setStageParamPaused
extern "C" void fn_8009D158(CameraController* camera, cmStageParamPaused* param);
// IfMngr::setHomerunDistance, dispHomerunResult, isFinishedHomerunResultAnim, dispHomerunVersusResult, dispHomerunHistory
extern "C" void fn_800DCD84(IfMngr* mngr, float distance);
extern "C" void fn_800DCD8C(IfMngr* mngr, bool newRecord);
extern "C" u32 fn_800DCD94(IfMngr* mngr);
extern "C" void fn_800DCDB0(IfMngr* mngr, float a, float b, float c);
extern "C" void fn_800DCDD4(IfMngr* mngr, int kind, float distance);
// IfPlayer::updateDamageHP / setDamageState (the barrier shows its health on the damage gauge of the player)
extern "C" void fn_800E14A4(IfPlayer* player, u32 value, int a);
extern "C" void fn_800E3684(IfPlayer* player, int state);
// stMelee::setPlayerPositionIndexFixed, setCamera (position, target, roll, fov), interpolate, interpolate1
extern "C" void fn_27_238DD8(stMelee* stage, int index);
extern "C" void fn_27_239AF4(stMelee* stage, Vec3f* eye, Vec3f* target, float roll, float fov);
extern "C" void fn_27_239BDC(stMelee* stage, Vec3f* out, Vec3f* from, Vec3f* to, float t, u8 ease);
extern "C" void fn_27_239E58(stMelee* stage, float* out, float from, float to, float t, u8 ease);
// stTrigger::setBeltConveyorParam
extern "C" void fn_27_233ACC(stTrigger* trigger, grGimmickBeltConveyorData* data);
// stPositions::getItemPos
extern "C" void fn_27_226CFC(stPositions* positions, int index, Vec3f* pos, Vec3f* safePos);
// BaseItem::move / getPos
extern "C" void fn_27_28DE90(BaseItem* item, Vec3f* pos);
extern "C" Vec3f fn_27_290060(BaseItem* item);
// soExternalValueAccesser::isGroundTouch
extern "C" int fn_27_8CC2C(BaseItem* item, int kind);

// MATCH-ONLY: two constructed statics (bss) that every stage TU built with this header set carries; same pair as st_heal.
struct bss_loc_8_t {
    u32 unk0;
    s32 unk4;
public:
    bss_loc_8_t(s32 p2) {
        unk0 = 0xFF;
        unk4 = p2;
    }
    bss_loc_8_t() { }
};

namespace {
    const bss_loc_8_t bss_loc_8(0);
    const bss_loc_8_t bss_loc_10(1);
}

stClassInfoImpl<Stages::HomeRunContest, stHomerun> stHomerun::bss_loc_14;

// MATCH-ONLY: the fields of the melee data the contest reads (they sit in a part of the structure the shared header
// does not name).
struct stHomerunMeleeView {
    char _0[0x63];
    bool m_single;     // 0x63: the single player mode
    bool m_combi;        // 0x64: two players throw together
    char _65[3];
    int m_teamA;       // 0x68
    int m_teamB;       // 0x6C
    u32 m_scoreA;      // 0x70
    u32 m_scoreB;      // 0x74
    u32 m_scoreBest;   // 0x78
};

#define HOMERUN_MELEE() reinterpret_cast<stHomerunMeleeView*>(g_GameGlobal->m_modeMelee)

stHomerun* stHomerun::create() {
    return new (Heaps::StageInstance) stHomerun;
}

stHomerun::stHomerun() : stMelee("stHomerun", Stages::HomeRunContest) {
    m_phase = 0;
    m_waitTimer = 0.0f;
    m_landTimer = 0.0f;
    m_sceneFrame = 0.0f;
    m_endTimer = 0.0f;
    m_sandBag = NULL;
    m_bat = NULL;
    m_sandBagWarped = false;
    m_batWarped = false;
    m_sandBagHp = 0.0f;
    m_cameraState = 0;
    m_cameraTimer = 0.0f;
    m_cameraMode = 0;
    m_cameraBlend = 0.0f;
    memset(&m_keyEye, 0, sizeof(m_keyEye));
    memset(&m_keyTarget, 0, sizeof(m_keyTarget));
    memset(&m_keyAngle, 0, sizeof(m_keyAngle));
    memset(&m_keyDist, 0, sizeof(m_keyDist));
    memset(&m_keyFov, 0, sizeof(m_keyFov));
    m_readyGo = false;
    m_rePlay = false;
    m_eventEnd = false;
    m_cameraSandBagY = 0.0f;
    memset(m_posGround, 0, sizeof(m_posGround));
    memset(m_posFloor, 0, sizeof(m_posFloor));
    memset(m_posSky, 0, sizeof(m_posSky));
    memset(m_posScore, 0, sizeof(m_posScore));
    memset(m_posNumber, 0, sizeof(m_posNumber));
    memset(m_posLimit, 0, sizeof(m_posLimit));
    m_zeroPos.m_x = 0.0f;
    m_zeroPos.m_y = 0.0f;
    m_zeroPos.m_z = 0.0f;
    m_scroll = 0.0f;
    m_scrollPrev = 0.0f;
    m_sandBagX = 0.0f;
    m_scrollRate = 0.0f;
    memset(m_score, 0, sizeof(m_score));
    m_dist = 0.0f;
    m_distPrev = 0.0f;
    m_scoreBest[0] = 0.0f;
    m_scoreBest[1] = 0.0f;
    m_scoreBestCombi = 0.0f;
    m_barrierState = 0;
    m_barrierHp = 0.0f;
    m_taskIdSandBag = 0;
    m_taskIdBat = 0;
    m_sandBagSpeed = 0.0f;
    m_sandBagLanded = 0;
    m_figurePhase = 0;
    m_figureRound = 0;
    memset(m_figureX, 0, sizeof(m_figureX));
    memset(m_figureId, 0, sizeof(m_figureId));
    m_sound[0] = -1;
    m_sound[1] = -1;
    m_sound[2] = -1;
    m_sound[3] = -1;
    m_sound[4] = -1;
    m_sound[5] = -1;
    stHomerunMeleeView* melee = HOMERUN_MELEE();
    if (melee == NULL) {
        return;
    }
    if (melee->m_single || melee->m_combi) {
        setPlayerPositionIndexSerial();
    } else {
        fn_27_238DD8(this, 0);
    }
    m_beltTrigger = NULL;
    m_beltData = NULL;
}

stHomerun::~stHomerun() {
    gfCameraManager* cameraManager = gfCameraManager::getManager();
    if (cameraManager != NULL) {
        fn_80018610(cameraManager);
    }
    if (m_beltData != NULL) {
        delete m_beltData;
    }
    releaseArchive();
}

bool stHomerun::loading() {
    return true;
}

void stHomerun::createObj() {
    int size;
    void* data = m_fileData->getData(Data_Type_Misc, 0x2711, &size, 0xFFFE);
    if (data != NULL) {
        m_sandBagBrres.setFileImage(data, size, Heaps::StageResource);
    }
    data = m_fileData->getData(Data_Type_Misc, 0x2712, &size, 0xFFFE);
    if (data != NULL) {
        m_sandBagParam.setFileImage(data, size, Heaps::StageResource);
    }
    data = m_fileData->getData(Data_Type_Misc, 0x2713, &size, 0xFFFE);
    if (data != NULL) {
        m_batBrres.setFileImage(data, size, Heaps::StageResource);
    }
    data = m_fileData->getData(Data_Type_Misc, 0x2714, &size, 0xFFFE);
    if (data != NULL) {
        m_batParam.setFileImage(data, size, Heaps::StageResource);
    }
    testStageParamInit(m_fileData, 10);
    testStageDataInit(m_fileData, 0x14, 8);
    initCameraParam();

    createObjMainBg(0);
    createObjLoop(1);
    createObjLoop(2);
    createObjLoop(3);
    createObjLoop(4);
    createObjLoop(5);
    createObjLoop(6);
    createObjScore(7);
    createObjScore(8);
    createObjScore(9);
    createObjScore(10);
    createObjScore(11);
    createObjScore(12);
    createObjNumber(13);
    createObjNumber(14);
    createObjNumber(15);
    createObjNumber(16);
    createObjNumber(17);
    createObjNumber(18);
    createObjNumber(19);
    createObjNumber(20);
    createObjNumber(21);
    createObjNumber(22);
    createObjNumber(23);
    createObjNumber(24);
    createObjNumber(25);
    createObjNumber(26);
    createObjNumber(27);
    createObjNumber(28);
    createObjNumber(29);
    createObjNumber(30);
    createObjNumber(31);
    createObjNumber(32);
    createObjNumber(33);
    createObjNumber(34);
    createObjNumber(35);
    createObjNumber(36);
    createObjBarrier(37);
    createObjFloor();
    createCollision(m_fileData, 2, NULL);
    createObjBeltConv();

    void* posData = m_fileData->getData(Data_Type_Model, 100, 0xFFFE);
    if (posData) {
        nw4r::g3d::ResFile posFile(posData);
        createStagePositions(&posFile);
    } else {
        createStagePositions();
    }
    createWind2ndOnly();
    loadStageAttrParam(m_fileData, 30);
    registScnAnim(static_cast<nw4r::g3d::ResFileData*>(m_fileData->getData(Data_Type_Scene, 0, 0xFFFE)), 0);
    initPosPokeTrainer(1, 0);
    createObjPokeTrainer(m_fileData, 0x65, "PokeTrainer00", m_pokeTrainerPos, NULL);
}

void stHomerun::createObjMainBg(int index) {
    grHomerunBg* ground;
    switch (index) {
    case 0:
        ground = grHomerunBg::create(1, "ETC", "grHomerunMainBg");
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setStateBarrierWork(&m_barrierState);
        ground->setHPBarrierWork(&m_barrierHp);
        ground->setTaskIDSandBagWork(&m_taskIdSandBag);
        ground->setTaskIDHomerunBatWork(&m_taskIdBat);
        ground->setSpeedSandBagWork(&m_sandBagSpeed);
        ground->setLandingSandBagWork(&m_sandBagLanded);
        ground->setFrameSceneWork(&m_sceneFrame);
    }
}

void stHomerun::createObjLoop(int index) {
    grHomerunLoop* ground;
    int type;
    switch (index) {
    case 1:
        ground = grHomerunLoopGround::create(10, "LOOP_GROUND", "grHomerunLoopGround");
        type = 11;
        break;
    case 2:
        ground = grHomerunLoopSky::create(11, "Loop_Sky", "grHomerunLoopSkyA");
        type = 2;
        break;
    case 3:
        ground = grHomerunLoopSky::create(11, "Loop_Sky", "grHomerunLoopSkyB");
        type = 3;
        break;
    case 4:
        ground = grHomerunLoopSky::create(11, "Loop_Sky", "grHomerunLoopSkyC");
        type = 4;
        break;
    case 5:
        ground = grHomerunLoopSky::create(11, "Loop_Sky", "grHomerunLoopSkyD");
        type = 5;
        break;
    case 6:
        ground = grHomerunLoopStadium::create(12, "LOOP_STADIUM", "grHomerunLoopStadium");
        type = 6;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setPosGroundWork(m_posGround);
        ground->setPosSkyWork(m_posSky);
        ground->setPosScoreWork(m_posScore);
        ground->setPosLimitWork(&m_posLimit[0].m_x);
        ground->setScoreWork(&m_score[0]);
        ground->setScrollWork(&m_scroll);
        ground->setScrollRateWork(&m_scrollRate);
        ground->setType(type);
        ground->setFrameSceneWork(&m_sceneFrame);
    }
}

void stHomerun::createObjScore(int index) {
    grHomerunScore* ground;
    Vec3f* pos;
    Vec3f* posNumber;
    u32* score;
    switch (index) {
    case 7:
        ground = grHomerunScore::create(0x1E, "SCORE_BOARD", "grHomerunScoreA");
        pos = &m_posScore[0];
        posNumber = &m_posNumber[0];
        score = &m_score[0];
        break;
    case 8:
        ground = grHomerunScore::create(0x1E, "SCORE_BOARD", "grHomerunScoreB");
        pos = &m_posScore[1];
        posNumber = &m_posNumber[4];
        score = &m_score[1];
        break;
    case 9:
        ground = grHomerunScore::create(0x1E, "SCORE_BOARD", "grHomerunScoreC");
        pos = &m_posScore[2];
        posNumber = &m_posNumber[8];
        score = &m_score[2];
        break;
    case 10:
        ground = grHomerunScore::create(0x1E, "SCORE_BOARD", "grHomerunScoreD");
        pos = &m_posScore[3];
        posNumber = &m_posNumber[12];
        score = &m_score[3];
        break;
    case 11:
        ground = grHomerunScore::create(0x1E, "SCORE_BOARD", "grHomerunScoreE");
        pos = &m_posScore[4];
        posNumber = &m_posNumber[16];
        score = &m_score[4];
        break;
    case 12:
        ground = grHomerunScore::create(0x1E, "SCORE_BOARD", "grHomerunScoreF");
        pos = &m_posScore[5];
        posNumber = &m_posNumber[20];
        score = &m_score[5];
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setPosWork(pos);
        ground->setPosNumberWork(posNumber);
        ground->setPosZeroWork(&m_zeroPos);
        ground->setPosLimitWork(&m_posLimit[0].m_x);
        ground->setScoreWork(score);
        ground->setScoreSandBagWork(&m_distPrev);
        ground->setScrollWork(&m_scroll);
        ground->setScrollRateWork(&m_scrollRate);
    }
}

// One digit board of a score: `index` runs over the 24 digit grounds (4 digits for each of the 6 boards).
#define HOMERUN_NUMBER(caseIndex, taskName, group, digit)                           \
    case caseIndex:                                                                 \
        ground = grHomerunNumber::create(0x28, "NUMBER", taskName);                 \
        pos = &m_posNumber[(caseIndex)-13];                                         \
        score = &m_score[group];                                                    \
        type = 0x12 + (digit);                                                      \
        break;

void stHomerun::createObjNumber(int index) {
    grHomerunNumber* ground;
    Vec3f* pos;
    u32* score;
    int type;
    switch (index) {
        HOMERUN_NUMBER(13, "grHomerunNumberA1", 0, 0)
        HOMERUN_NUMBER(14, "grHomerunNumberA10", 0, 1)
        HOMERUN_NUMBER(15, "grHomerunNumberA100", 0, 2)
        HOMERUN_NUMBER(16, "grHomerunNumberA1000", 0, 3)
        HOMERUN_NUMBER(17, "grHomerunNumberB1", 1, 0)
        HOMERUN_NUMBER(18, "grHomerunNumberB10", 1, 1)
        HOMERUN_NUMBER(19, "grHomerunNumberB100", 1, 2)
        HOMERUN_NUMBER(20, "grHomerunNumberB1000", 1, 3)
        HOMERUN_NUMBER(21, "grHomerunNumberC1", 2, 0)
        HOMERUN_NUMBER(22, "grHomerunNumberC10", 2, 1)
        HOMERUN_NUMBER(23, "grHomerunNumberC100", 2, 2)
        HOMERUN_NUMBER(24, "grHomerunNumberC1000", 2, 3)
        HOMERUN_NUMBER(25, "grHomerunNumberD1", 3, 0)
        HOMERUN_NUMBER(26, "grHomerunNumberD10", 3, 1)
        HOMERUN_NUMBER(27, "grHomerunNumberD100", 3, 2)
        HOMERUN_NUMBER(28, "grHomerunNumberD1000", 3, 3)
        HOMERUN_NUMBER(29, "grHomerunNumberE1", 4, 0)
        HOMERUN_NUMBER(30, "grHomerunNumberE10", 4, 1)
        HOMERUN_NUMBER(31, "grHomerunNumberE100", 4, 2)
        HOMERUN_NUMBER(32, "grHomerunNumberE1000", 4, 3)
        HOMERUN_NUMBER(33, "grHomerunNumberF1", 5, 0)
        HOMERUN_NUMBER(34, "grHomerunNumberF10", 5, 1)
        HOMERUN_NUMBER(35, "grHomerunNumberF100", 5, 2)
        HOMERUN_NUMBER(36, "grHomerunNumberF1000", 5, 3)
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0x40, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setPosWork(pos);
        ground->setScoreWork(score);
        ground->setType(type);
    }
}

void stHomerun::createObjBarrier(int index) {
    grHomerunBarrier* ground;
    if (index == 37) {
        ground = grHomerunBarrier::create(0x3C, "EffStgHomerunBarrier", "grHomerunBarrier");
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setStateWork(&m_barrierState);
        ground->setHPWork(&m_barrierHp);
    }
}

// The floor is twelve pieces, painted in turn with the three colors.
void stHomerun::createObjFloor() {
    u8 type = 0;
    for (u32 i = 0; i < 12; i++) {
        createObjFloor1(i, type);
        type++;
        if (type == 3) {
            type = 0;
        }
    }
}

void stHomerun::createObjFloor1(u8 index, u8 type) {
    grHomerunLoopFloor* ground;
    switch (type) {
    case 0:
        ground = grHomerunLoopFloor::create(0x14, "LOOP_GROUND_FEET_01", "grHomerunFloorY");
        break;
    case 1:
        ground = grHomerunLoopFloor::create(0x15, "LOOP_GROUND_FEET_02", "grHomerunFloorR");
        break;
    case 2:
        ground = grHomerunLoopFloor::create(0x16, "LOOP_GROUND_FEET_03", "grHomerunFloorW");
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setPosGroundWork(m_posGround);
        ground->setPosFloorWork(m_posFloor);
        ground->setPosScoreWork(m_posScore);
        ground->setPosLimitWork(&m_posLimit[0].m_x);
        ground->setScoreWork(&m_score[0]);
        ground->setScrollWork(&m_scroll);
        ground->setScrollRateWork(&m_scrollRate);
        ground->setIndex(index);
    }
}

// The conveyor belt that carries the fighter back to the left while the sandbag is still on the platform.
void stHomerun::createObjBeltConv() {
    m_beltData = new (Heaps::StageResource) grGimmickBeltConveyorData;
    if (m_beltData != NULL) {
        memset(m_beltData, 0, sizeof(grGimmickBeltConveyorData));
        grGimmickBeltConveyorData* pos = m_beltData;
        pos->m_pos.m_x = 0.0f;
        pos->m_pos.m_y = 0.0f;
        pos->m_pos.m_z = 0.0f;
        m_beltData->m_speed = 0.0f;
        m_beltData->m_isRight = true;
        grGimmickBeltConveyorData* area = m_beltData;
        area->m_areaData.m_offsetPos.m_x = 0.0f;
        area->m_areaData.m_offsetPos.m_y = 0.0f;
        area->m_areaData.m_range.m_x = 0.0f;
        area->m_areaData.m_range.m_y = 0.0f;
        area->m_areaData.m_shapeType = gfArea::Shape_Rectangle;
        m_beltTrigger = g_stTriggerMng->createTrigger(Gimmick::Area_BeltConveyor, -1);
        m_beltTrigger->setBeltConveyorTrigger(m_beltData);
        m_beltTrigger->setAreaSleep(true);
    }
}

void stHomerun::update(float deltaFrame) {
    updateStagePositions();
    updateSandBag(deltaFrame);
    updateSandBagDmg(deltaFrame);
    updateCamera(deltaFrame);
    updateDist(deltaFrame);
    updateFigure(deltaFrame);
    updateLimit(deltaFrame);
    updateBeltConv(deltaFrame);
}

// The edges of the camera range, as 2D positions the grounds use to wrap their pieces around.
void stHomerun::updateLimit(float deltaFrame) {
    CameraController* camera = CameraController::getInstance();
    float* range = reinterpret_cast<float*>(reinterpret_cast<u8*>(camera) + 0x148);
    float up = range[2];
    float left = range[0];
    m_posLimit[0].m_x = left;
    m_posLimit[0].m_y = up;
    m_posLimit[0].m_z = 0.0f;
    float down = range[3];
    float right = range[1];
    m_posLimit[1].m_x = right;
    m_posLimit[1].m_y = down;
    m_posLimit[1].m_z = 0.0f;
}

void stHomerun::updateBeltConv(float deltaFrame) {
    float limit;
    if (m_sandBag == NULL || !m_sandBag->m_alive) {
        limit = 0.0f;
    } else {
        limit = m_sandBag->getParamFloat(3);
    }
    if (m_zeroPos.m_x > limit) {
        m_beltTrigger->setAreaSleep(true);
    } else {
        m_beltTrigger->setAreaSleep(false);
        float x = 1500.0f + m_zeroPos.m_x;
        float y = m_zeroPos.m_y;
        grGimmickBeltConveyorData* pos = m_beltData;
        pos->m_pos.m_x = x;
        pos->m_pos.m_y = y;
        pos->m_pos.m_z = 0.0f;
        if (m_scrollRate > 0.0f) {
            m_beltData->m_speed = m_scroll;
        } else {
            m_beltData->m_speed = 0.0f;
        }
        m_beltData->m_isRight = false;
        grGimmickBeltConveyorData* area = m_beltData;
        area->m_areaData.m_offsetPos.m_x = 0.0f;
        area->m_areaData.m_offsetPos.m_y = 0.0f;
        area->m_areaData.m_range.m_x = 3000.0f;
        area->m_areaData.m_range.m_y = 10.0f;
        area->m_areaData.m_shapeType = gfArea::Shape_Rectangle;
        fn_27_233ACC(m_beltTrigger, m_beltData);
    }
}

void stHomerun::processFixPosition() {
    fixposSandBag();
    fixposFigure();
    fixposCamera();
}

// Which of the four result rows (one player, or the two teams in either order) the versus history shows.
static inline void homerunShowHistory(stHomerunMeleeView* melee, float distance) {
    int teamA = melee->m_teamA;
    int teamB = melee->m_teamB;
    int kind;
    if (teamA == 0 && teamB == 0) {
        kind = 0;
    }
    if (teamA == 1 && teamB == 0) {
        kind = 1;
    }
    if (teamA == 0 && teamB == 1) {
        kind = 2;
    }
    if (teamA == 1 && teamB == 1) {
        kind = 3;
    }
    fn_800DCDD4(g_IfMngr, kind, distance);
}

// The flow of a throw: set the sandbag and the bat up (phase 1), wait for "Go!" (3), watch the flight while the
// barrier may break (3/4/6), show the result (5/7) and the versus summary (8/9) and finally end the event (10).
void stHomerun::updateSandBag(float deltaFrame) {
    m_waitTimer -= deltaFrame;
    if (m_waitTimer < 0.0f) {
        m_waitTimer = 0.0f;
    }
    m_endTimer -= deltaFrame;
    if (m_endTimer < 0.0f) {
        m_endTimer = 0.0f;
    }
    float* param = static_cast<float*>(m_stageData);
    if (param != NULL) {
        stHomerunMeleeView* melee = HOMERUN_MELEE();
        if (melee != NULL) {
            Vec3f sandBagPos;
            int touch;
            if (m_sandBag != NULL && m_sandBag->m_alive) {
                sandBagPos.m_x = m_sandBag->getParamFloat(3);
                sandBagPos.m_y = m_sandBag->getParamFloat(4);
                sandBagPos.m_z = 0.0f;
                touch = fn_27_8CC2C(m_sandBag, 8);
                m_sandBagSpeed = m_sandBag->getParamFloat(0x3EF);
            }
            switch (m_phase) {
            case 0:
                m_scroll = 0.0f;
                m_sandBagX = 0.0f;
                m_scrollRate = 0.0f;
                m_score[0] = 100;
                m_score[1] = 200;
                m_score[2] = 300;
                m_barrierState = 0;
                m_barrierHp = param[0];
                m_phase = 1;
                break;
            case 1: {
                itManager* items = itManager::getInstance();
                if (items != NULL) {
                    if (!m_sandBagWarped) {
                        if (m_sandBag == NULL) {
                            break;
                        }
                        if (!m_sandBag->m_alive) {
                            break;
                        }
                        Vec3f pos(0.0f, 10.0f, 0.0f);
                        m_sandBag->warp(&pos);
                        m_sandBag->setVanishMode(false);
                        m_sandBagWarped = true;
                        m_taskIdSandBag = m_sandBag->m_taskId;
                    }
                    if (!m_batWarped) {
                        if (!items->isCompItemKindArchive(Item_HomeRunBat, 0, true)) {
                            break;
                        }
                        m_bat = items->createItem(Item_HomeRunBat, 0, -1);
                        if (m_bat == NULL) {
                            break;
                        }
                        Vec3f pos;
                        if (m_stagePositions != NULL) {
                            Vec3f itemPos;
                            Vec3f safePos;
                            fn_27_226CFC(m_stagePositions, 0, &itemPos, &safePos);
                            pos.m_x = itemPos.m_x;
                            pos.m_z = itemPos.m_z;
                            pos.m_y = 10.0f;
                        }
                        // The bat is warped to the first item position; without stage positions the original does not
                        // set the position either.
                        m_bat->warp(&pos);
                        m_batWarped = true;
                        m_taskIdBat = m_bat->m_taskId;
                    }
                    if (m_sandBagWarped == true && m_batWarped == true) {
                        m_zeroPos.m_x = m_posGround[0].m_x - 125.0f;
                        m_zeroPos.m_y = m_posGround[0].m_y;
                        m_zeroPos.m_z = 0.0f;
                        m_phase = 3;
                    }
                }
                break;
            }
            case 3:
                if (m_readyGo) {
                    if (m_sound[0] == -1) {
                        m_sound[0] = g_sndSystem->playSE(snd_se_audience_suddendeath, 0x10000, 0, 0, -1);
                    }
                    if (getFrameRuleTime() == 0.0f) {
                        if (m_waitTimer == 0.0f) {
                            m_phase = 6;
                            m_waitTimer = 90.0f;
                        }
                    } else if (getFrameRuleTime() < 30.0f) {
                        m_waitTimer = 90.0f;
                        switch (m_barrierState) {
                        case 0:
                        case 1:
                        case 2:
                            m_barrierState = 3;
                            break;
                        }
                    } else {
                        m_waitTimer = 90.0f;
                        switch (m_barrierState) {
                        case 0:
                        case 1:
                        case 2:
                            if (!(__fabsf(m_sandBagSpeed) < 9.0f)) {
                                if (m_sandBagSpeed > 0.0f && sandBagPos.m_x + m_sandBagSpeed >= 34.0f) {
                                    m_barrierState = 4;
                                } else if (m_sandBagSpeed < 0.0f && sandBagPos.m_x + m_sandBagSpeed <= -34.0f) {
                                    m_barrierState = 4;
                                }
                            }
                            break;
                        }
                    }
                    if (touch == 0) {
                        if (sandBagPos.m_x < -54.0f && m_sandBagSpeed < 0.0f) {
                            m_phase = 6;
                            m_waitTimer = 15.0f;
                        } else if (sandBagPos.m_x > 54.0f && m_sandBagSpeed > 0.0f) {
                            m_phase = 4;
                            m_waitTimer = 120.0f;
                        }
                    }
                }
                break;
            case 4: {
                float best;
                if (melee->m_combi) {
                    best = m_scoreBestCombi;
                } else {
                    switch (melee->m_teamA) {
                    case 0:
                        best = m_scoreBest[0];
                        break;
                    case 1:
                        best = m_scoreBest[1];
                        break;
                    default:
                        best = -1.0f;
                        break;
                    }
                }
                if (__fabsf(m_sandBagSpeed) > 0.0f) {
                    m_landTimer = 120.0f;
                }
                if (touch != 0) {
                    if (m_dist <= 15.0f && m_sound[1] == -1) {
                        m_sound[1] = g_sndSystem->playSE(snd_se_Audience_Zannen, 0x10000, 0, 0, -1);
                    }
                    m_landTimer -= deltaFrame;
                    if (m_landTimer < 0.0f) {
                        m_landTimer = 0.0f;
                    }
                    if (m_landTimer == 0.0f) {
                        bool newRecord;
                        if (-1.0f == best) {
                            newRecord = false;
                        } else {
                            newRecord = m_dist > best;
                        }
                        if (m_rePlay == true) {
                            newRecord = false;
                        }
                        m_sound[3] = g_sndSystem->playSE(snd_se_Audience_Hakusyu_l, 0x10000, 0, 0, -1);
                        if (newRecord == true) {
                            m_sound[4] = g_sndSystem->playSE(snd_se_Audience_Kansei_l, 0x10000, 0, 0, -1);
                            g_sndSystem->playSE(snd_se_narration_NewRec, 0x10000, 0, 0, -1);
                        }
                        fn_800DCD8C(g_IfMngr, newRecord);
                        if (isVs() == true) {
                            homerunShowHistory(melee, m_dist);
                        }
                        m_phase = 5;
                    }
                }
                break;
            }
            case 5:
                m_eventEnd = true;
                if (fn_800DCD94(g_IfMngr) == 1) {
                    if (isVsEnd() == true) {
                        m_phase = 8;
                        m_endTimer = 60.0f;
                    } else if (isVs() == true) {
                        m_phase = 10;
                        m_endTimer = 60.0f;
                    } else {
                        m_phase = 10;
                        m_endTimer = 30.0f;
                    }
                }
                break;
            case 6:
                if (m_waitTimer == 0.0f) {
                    if (m_sound[1] == -1) {
                        m_sound[1] = g_sndSystem->playSE(snd_se_Audience_Zannen, 0x10000, 0, 0, -1);
                    }
                    fn_800DCD8C(g_IfMngr, false);
                    if (isVs() == true) {
                        homerunShowHistory(melee, m_dist);
                    }
                    m_phase = 7;
                } else if (touch == 0) {
                    if (sandBagPos.m_x > 54.0f && m_sandBagSpeed > m_waitTimer) {
                        m_phase = 4;
                        m_waitTimer = 120.0f;
                    } else if (sandBagPos.m_x < -54.0f && m_sandBagSpeed < 0.0f) {
                        if (m_waitTimer > 0.25f) {
                            m_waitTimer = 0.25f;
                        }
                    }
                }
                break;
            case 7:
                m_eventEnd = true;
                if (fn_800DCD94(g_IfMngr) == 1) {
                    if (isVsEnd() == true) {
                        m_phase = 8;
                        m_endTimer = 60.0f;
                    } else if (isVs() == true) {
                        m_phase = 10;
                        m_endTimer = 60.0f;
                    } else {
                        m_phase = 10;
                        m_endTimer = 30.0f;
                    }
                }
                break;
            case 8:
                if (m_endTimer == 0.0f) {
                    Vec3f result;
                    if (melee->m_scoreA > melee->m_scoreB) {
                        result.m_x = (float)melee->m_scoreA / 10.0f;
                    } else {
                        result.m_x = (float)melee->m_scoreB / 10.0f;
                    }
                    if ((float)melee->m_scoreBest > 10.0f * m_dist) {
                        result.m_y = (float)melee->m_scoreBest / 10.0f;
                    } else {
                        result.m_y = m_dist;
                    }
                    result.m_z = result.m_x + result.m_y;
                    result.m_x += 0.05f;
                    result.m_y += 0.05f;
                    result.m_z += 0.05f;
                    m_sound[3] = g_sndSystem->playSE(snd_se_Audience_Hakusyu_l, 0x10000, 0, 0, -1);
                    fn_800DCDB0(g_IfMngr, result.m_x, result.m_y, result.m_z);
                    m_phase = 9;
                }
                break;
            case 9:
                if (fn_800DCD94(g_IfMngr) == 1) {
                    m_phase = 10;
                    m_endTimer = 30.0f;
                }
                break;
            case 2:
            case 10:
                break;
            }
        }
    }
}

// MATCH-ONLY: the fields of the player's info panel that the barrier health is shown through.
struct stHomerunPlayerView {
    char _0[0x18];
    u32 m_unk18;
    u32 m_unk1C;
    char _20[0x244];
    float m_damageGauge; // 0x264
};

// The sandbag's hit points are shown on the damage gauge of the player: the gauge grows with every hit.
void stHomerun::updateSandBagDmg(float deltaFrame) {
    if (m_sandBag != NULL && m_sandBag->m_alive && m_readyGo && g_GameGlobal->m_modeMelee != NULL) {
        float hp = m_sandBag->getParamFloat(2);
        IfMngr* ifMngr = g_IfMngr;
        u8 playerIndex = reinterpret_cast<u8*>(ifMngr)[0x6A];
        fn_800E14A4(ifMngr->m_ifPlayers[playerIndex], (u32)hp, 0);
        if (hp != m_sandBagHp) {
            float gauge = 0.6f * ((hp - m_sandBagHp - 3.0f) / (15.0f - 3.0f));
            if (gauge < 0.0f) {
                gauge = 0.0f;
            }
            if (gauge > 1.0f) {
                gauge = 1.0f;
            }
            IfPlayer* player = g_IfMngr->m_ifPlayers[reinterpret_cast<u8*>(g_IfMngr)[0x6A]];
            stHomerunPlayerView* view = reinterpret_cast<stHomerunPlayerView*>(player);
            if (view->m_unk1C == view->m_unk18) {
                view->m_damageGauge = gauge;
                fn_800E3684(player, 1);
            }
            m_sandBagHp = hp;
        }
    }
}

// The camera follows the sandbag with a key per phase of the flight.
void stHomerun::updateCamera(float deltaFrame) {
    if ((*reinterpret_cast<u32*>(reinterpret_cast<u8*>(g_gfApplication) + 0xEC) >> 27) != 0) {
        return;
    }
    m_cameraTimer -= deltaFrame;
    if (0.0f == m_cameraTimer) {
        m_cameraTimer = 0.0f;
    }
    if (m_sandBag == NULL) {
        return;
    }
    if (!m_sandBag->m_alive) {
        return;
    }
    if (m_readyGo != true) {
        return;
    }
    switch (m_cameraState) {
    case 1:
        m_keyTarget.m_elapsed = 0.0f;
        m_keyTarget.m_duration = 90.0f;
        m_keyFov.m_elapsed = 0.0f;
        m_keyFov.m_duration = 90.0f;
        m_keyDist.m_elapsed = 0.0f;
        m_keyDist.m_duration = 90.0f;
        m_keyAngle.m_elapsed = 0.0f;
        m_keyAngle.m_duration = 150.0f;
        break;
    case 2:
        if (m_phase != 6 && m_phase != 7 && m_phase != 8 && m_phase != 9 && m_phase != 10) {
            m_keyTarget.m_elapsed = 0.0f;
            m_keyTarget.m_duration = 39.0f;
            m_keyDist.m_elapsed = 0.0f;
            m_keyDist.m_duration = 48.0f;
        }
        break;
    case 3:
        m_keyTarget.m_elapsed = 0.0f;
        m_keyDist.m_elapsed = 0.0f;
        m_keyTarget.m_duration = 15.0f * (1.0f - m_cameraBlend);
        m_keyDist.m_duration = 48.0f;
        m_keyAngle.m_elapsed = 0.0f;
        m_keyAngle.m_duration = 150.0f;
        break;
    }
    m_keyTarget.m_elapsed += deltaFrame;
    m_keyEye.m_elapsed += deltaFrame;
    m_keyAngle.m_elapsed += deltaFrame;
    m_keyFov.m_elapsed += deltaFrame;
    m_keyDist.m_elapsed += deltaFrame;
}

// The distance of the throw: it is measured from the launch point in feet, rounded to a tenth, and drives the scene
// animation that scrolls the background (it repeats every 3000 feet).
void stHomerun::updateDist(float deltaFrame) {
    float lastDist = m_dist;
    m_dist = (m_sandBagX - m_zeroPos.m_x) / 10.0f * 3.28084f;
    if (m_dist < 0.0f) {
        m_dist = 0.0f;
    }
    m_distPrev = m_dist;
    m_dist = (float)(int)(10.0f * m_dist) / 10.0f;
    m_sceneFrame = 400.0f * ((m_dist - 3000.0f * (float)(int)(m_dist / 3000.0f)) / 3000.0f);
    g_gfSceneRoot->setCurrentFrame(m_sceneFrame);
    if (m_sandBag != NULL && m_sandBag->m_alive && m_readyGo) {
        switch (m_phase) {
        case 3:
        case 4: {
            fn_800DCD84(g_IfMngr, m_dist);
            double integral;
            double lastIntegral;
            modf(m_dist, &integral);
            modf(lastDist, &lastIntegral);
            if (integral - lastIntegral >= 1.0) {
                g_sndSystem->playSE(snd_se_Homerun_meter, 0, 0, 0, -1);
            }
            break;
        }
        }
    }
}

// The trophy "figures" that are thrown into the stadium: three random spots (at least 100 feet apart) per 1000 feet of
// the flight are picked, and figures that were passed are removed again.
void stHomerun::updateFigure(float deltaFrame) {
    float feet = 3.28084f;
    switch (m_figurePhase) {
    case 0: {
        float start;
        if (m_figureRound == 0) {
            start = 50.0f * feet;
        } else {
            start = 0.0f;
        }
        u32 count = (u32)((1000.0f * feet - start) / (50.0f * feet));
        float step = 50.0f * feet;
        float maxDist = 1000.0f * feet;
        float gap = 2.0f * step;
        for (u32 slot = 0; slot != 3; slot++) {
            u32 tries = 0;
            do {
                u32 pick = (u32)(randf() * (float)count);
                pick = (pick > 0) ? pick : 0;
                pick = (pick < count - 1) ? pick : count - 1;
                double position = start + step * (float)pick;
                m_figureX[slot] = position;
                m_figureX[slot] = (float)position + maxDist * (float)m_figureRound;
                // a spot that is closer than 100 feet to another one is dropped
                for (u32 other = 0; other < 3; other++) {
                    if (slot != other && __fabsf(m_figureX[slot] - m_figureX[other]) < gap) {
                        m_figureX[slot] = 0.0f;
                        break;
                    }
                }
                if (m_figureX[slot] != 0.0f) {
                    break;
                }
                tries++;
            } while (tries != 5);
        }
        m_figurePhase = 1;
    }
        // FALL-THROUGH
    case 1:
        m_figurePhase = 2;
        // FALL-THROUGH
    case 2:
        for (u8 i = 0; i != 3; i++) {
            if (m_figureId[i] == 0) {
                continue;
            }
            itManager* items = itManager::getInstance();
            if (items == NULL) {
                break;
            }
            BaseItem* item = items->getItemFromInstanceId(m_figureId[i]);
            if (item != NULL) {
                Vec3f pos;
                pos = fn_27_290060(item);
                if (pos.m_x < m_posLimit[0].m_x - 112.5f) {
                    item->deactivate();
                    m_figureId[i] = 0;
                }
            }
        }
        break;
    }
}

static inline float homerunClamp(float value, float lo, float hi) {
    value = nw4r::math::FSelect(value - lo, value, lo);
    return nw4r::math::FSelect(value - hi, hi, value);
}

static inline float homerunLength(const Vec3f& v) {
    float lengthSq = v.m_x * v.m_x + v.m_y * v.m_y + v.m_z * v.m_z;
    if ((float)fabs(lengthSq) <= 1.17549435e-38f) {
        return 0.0f;
    }
    return lengthSq * rsqrtf(lengthSq);
}

// The sandbag drives the scroll of the background: it runs along the screen until it is 2000 units away from the
// launch point, then the stage scrolls instead.
void stHomerun::fixposSandBag() {
    if (m_sandBag == NULL || !m_sandBag->m_alive) {
        return;
    }
    Vec3f pos;
    pos.m_x = m_sandBag->getParamFloat(3);
    pos.m_y = m_sandBag->getParamFloat(4);
    pos.m_z = 0.0f;
    if (m_scrollRate > 0.0f) {
        pos.m_x = 2000.0f;
        fn_27_28DE90(m_sandBag, &pos);
    }
    m_scrollPrev = m_scroll;
    m_scroll = homerunClamp(pos.m_x, 0.0f, pos.m_x);
    if (m_sandBagX >= 2000.0f) {
        m_scroll = m_sandBagSpeed;
        m_sandBagX += m_sandBagSpeed;
    } else {
        m_sandBagX += m_scroll - m_scrollPrev;
        if (m_scrollRate == 0.0f && m_sandBagX >= 2000.0f) {
            m_scroll = m_sandBagX - 2000.0f;
            pos.m_x = 2000.0f;
            fn_27_28DE90(m_sandBag, &pos);
        }
    }
    if (m_sandBagX < 2000.0f) {
        m_scrollRate = 0.0f;
        m_cameraBlend = homerunClamp(m_sandBagX / 1900.0f, 0.0f, 1.0f);
    } else if (m_sandBagX > 3000.0f) {
        m_scrollRate = 1.0f;
        m_cameraBlend = 1.0f;
    } else {
        m_cameraBlend = 1.0f;
        m_scrollRate = homerunClamp((m_sandBagX - 2000.0f) / 1000.0f, 0.0f, 1.0f);
    }
}

// Throws the figures into the stadium: every figure whose spot is within 100 feet of the sandbag is created.
void stHomerun::fixposFigure() {
    switch (m_figurePhase) {
    case 2: {
        float feet = 3.28084f;
        if (m_dist < 50.0f * feet) {
            return;
        }
        if (m_dist > 2000.0f * feet) {
            return;
        }
        u8 done = 0;
        for (u8 i = 0; i != 3; i++) {
            if (m_figureX[i] == 0.0f) {
                done++;
                continue;
            }
            if (m_figureX[i] < m_dist + 100.0f * feet) {
                double x;
                if (m_scrollRate > 0.0f) {
                    x = 2000.0f + (500.0f * (m_figureX[i] - m_dist)) / 50.0f / feet;
                } else {
                    x = m_zeroPos.m_x + (10.0f * m_figureX[i]) / feet;
                }
                Vec3f pos;
                pos.m_x = x;
                pos.m_y = m_posFloor[0].m_y - 2.5f;
                pos.m_z = m_zeroPos.m_z;
                Vec3f safePos;
                safePos.m_x = x;
                safePos.m_y = 2.5f + m_posFloor[0].m_y;
                safePos.m_z = m_zeroPos.m_z;
                if (m_scrollRate > 0.0f) {
                    pos.m_x = (float)x - m_scroll;
                    safePos.m_x = (float)x - m_scroll;
                }
                u16 variation = fn_800516F0();
                BaseItem* item = itManager::getInstance()->createItem(&safePos, &pos, 1.0f, Item_Figure, variation);
                if (item == NULL) {
                    return;
                }
                item->action(0, 1.0f);
                m_figureX[i] = 0.0f;
                if (m_figureId[0] == 0) {
                    m_figureId[0] = item->m_instanceId;
                } else if (m_figureId[1] == 0) {
                    m_figureId[1] = item->m_instanceId;
                } else if (m_figureId[2] == 0) {
                    m_figureId[2] = item->m_instanceId;
                }
            }
        }
        if (done == 3) {
            m_figurePhase = 0;
            m_figureRound++;
        }
        break;
    }
    }
}

// The camera follows the sandbag in four steps: it waits on the launch (state 0, 1), then looks at the sandbag from
// where the camera was (2), and finally keeps its distance while the sandbag flies (3).
void stHomerun::fixposCamera() {
    if (m_sandBag == NULL || !m_sandBag->m_alive) {
        return;
    }
    gfCameraManager* manager = gfCameraManager::getManager();
    if (manager == NULL) {
        return;
    }
    gfCamera* camera = &manager->m_cameras[0];

    Vec3f center;
    Vec3f lookAt;
    center = camera->m_centerPos;
    lookAt = camera->m_targetPos;
    Vec3f delta = center - lookAt;
    Vec3f dir;
    float dist;
    if (homerunIsZero(delta) == true) {
        dist = 0.0f;
        dir = delta;
    } else {
        dist = homerunLength(delta);
        dir = delta;
        dir.normalize();
    }

    Vec3f target;
    Vec3f angle;
    Vec3f eye;
    float fov;
    switch (m_cameraState) {
    case 0:
        m_cameraMode = 9;
        m_cameraState = 1;
        break;
    case 1:
        if (!m_readyGo) {
            return;
        }
        m_keyTarget.m_start.m_x = camera->m_targetPos.m_x;
        m_keyTarget.m_start.m_y = camera->m_targetPos.m_y;
        m_keyTarget.m_start.m_z = camera->m_targetPos.m_z;
        m_keyTarget.m_end.m_x = m_sandBag->getParamFloat(3);
        m_keyTarget.m_end.m_y = m_sandBag->getParamFloat(4);
        m_keyTarget.m_end.m_z = 0.0f;
        m_keyTarget.m_elapsed = 0.0f;
        m_keyTarget.m_duration = 90.0f;
        m_keyTarget.m_ease = 1;
        // MATCH-ONLY: the camera field at 0xD4 is not named by the shared header (the field of view the camera starts from).
        m_keyFov.m_start = *reinterpret_cast<float*>(reinterpret_cast<char*>(camera) + 0xD4);
        m_keyFov.m_end = 1.0f;
        m_keyFov.m_elapsed = 0.0f;
        m_keyFov.m_duration = 90.0f;
        m_keyFov.m_ease = 0;
        {
            float horizontal = nw4r::math::FSqrt(dir.m_x * dir.m_x + dir.m_z * dir.m_z);
            angle.m_x = nw4r::math::AtanDeg(dir.m_y / horizontal);
            angle.m_y = nw4r::math::AtanDeg(dir.m_z / dir.m_x);
            angle.m_z = 0.0f;
            if (angle.m_y < 0.0f) {
                angle.m_y += 180.0f;
            }
        }
        m_keyDist.m_start = dist;
        m_keyDist.m_end = 178.0f + 10.0f * randf();
        m_keyDist.m_elapsed = 0.0f;
        m_keyDist.m_duration = 90.0f;
        m_keyDist.m_ease = 1;
        m_keyAngle.m_start.m_x = angle.m_x;
        m_keyAngle.m_start.m_y = angle.m_y;
        m_keyAngle.m_start.m_z = angle.m_z;
        m_keyAngle.m_end.m_x = 3.0f;
        m_keyAngle.m_end.m_y = 90.0f;
        m_keyAngle.m_end.m_z = angle.m_z;
        m_keyAngle.m_elapsed = 0.0f;
        m_keyAngle.m_duration = 150.0f;
        m_keyAngle.m_ease = 1;
        m_cameraMode = 10;
        m_cameraState = 2;
        // FALL-THROUGH
    case 2: {
        u8 phase = m_phase;
        if (phase == 6 || phase == 7 || phase == 8 || phase == 9 || phase == 10) {
            break;
        }
        target.m_x = m_sandBag->getParamFloat(3);
        target.m_y = m_sandBag->getParamFloat(4);
        target.m_z = 0.0f;
        if (target.m_y > 90.0f) {
            target.m_y = 90.0f;
        }
        float t = homerunClamp(target.m_y / 90.0f, 0.0f, 1.0f);
        m_keyTarget.m_start.m_x = camera->m_targetPos.m_x;
        m_keyTarget.m_start.m_y = camera->m_targetPos.m_y;
        m_keyTarget.m_start.m_z = camera->m_targetPos.m_z;
        m_keyTarget.m_end = target;
        m_keyTarget.m_end.m_y = target.m_y + 15.0f;
        m_keyTarget.m_ease = 1;
        m_keyDist.m_start = dist;
        m_keyDist.m_end = 178.0f + 250.0f * t;
        m_keyDist.m_ease = 1;
        if (m_phase != 4) {
            break;
        }
        m_cameraSandBagY = m_sandBag->getParamFloat(4);
        m_cameraState = 3;
    }
        // FALL-THROUGH
    case 3: {
        target.m_x = m_sandBag->getParamFloat(3);
        target.m_y = m_sandBag->getParamFloat(4);
        target.m_z = 0.0f;
        if (target.m_y > 90.0f) {
            target.m_y = 90.0f;
        }
        float t = homerunClamp((target.m_y - m_posGround[0].m_y) / (90.0f - m_posGround[0].m_y), 0.0f, 1.0f);
        if (m_cameraSandBagY > m_sandBag->getParamFloat(4)) {
            // the sandbag falls: the camera eases down with a sine curve
            float s = nw4r::math::SinIdx((u16)(16384.0f * t));
            target.m_y = (target.m_y - 30.0f) + 30.0f * (1.0f - s);
        } else if (m_cameraSandBagY < m_sandBag->getParamFloat(4)) {
            target.m_y -= 30.0f * t;
        }
        m_cameraSandBagY = m_sandBag->getParamFloat(4);
        float distEnd = 128.0f + 472.0f * t;
        m_keyTarget.m_start.m_x = camera->m_targetPos.m_x;
        m_keyTarget.m_start.m_y = camera->m_targetPos.m_y;
        m_keyTarget.m_start.m_z = camera->m_targetPos.m_z;
        m_keyTarget.m_end = target;
        m_keyTarget.m_end.m_y = target.m_y + 15.0f;
        m_keyTarget.m_ease = 1;
        m_keyDist.m_start = dist;
        m_keyDist.m_end = distEnd;
        m_keyDist.m_ease = 1;
        m_keyAngle.m_start.m_x = m_keyAngle.m_end.m_x;
        m_keyAngle.m_start.m_y = m_keyAngle.m_end.m_y;
        m_keyAngle.m_start.m_z = m_keyAngle.m_end.m_z;
        m_keyAngle.m_end.m_x = 3.0f;
        m_keyAngle.m_end.m_y = 90.0f;
        m_keyAngle.m_end.m_z = 0.0f;
        m_keyAngle.m_ease = 1;
        break;
    }
    }

    if (!m_readyGo) {
        return;
    }
    interpolateSub(&target, &m_keyTarget);
    interpolateSub(&angle, &m_keyAngle);
    interpolateSub1(&fov, &m_keyFov);
    switch (m_cameraMode) {
    case 9: {
        interpolateSub(&eye, &m_keyEye);
        Vec3f toTarget;
        toTarget = target - eye;
        if (homerunIsZero(toTarget) == true) {
            dist = 0.0f;
        } else {
            dist = homerunLength(toTarget);
        }
        break;
    }
    case 10: {
        interpolateSub1(&dist, &m_keyDist);
        calcCameraParam(dist, &eye, &target, &angle);
        Vec3f offset;
        if (m_sandBagX >= 2000.0f) {
            offset.m_x = 2000.0f;
            offset.m_y = 0.0f;
            offset.m_z = 0.0f;
        } else {
            offset.m_x = m_scroll;
            offset.m_y = 0.0f;
            offset.m_z = 0.0f;
        }
        m_stagePositions->m_offsetPos.m_x = offset.m_x;
        m_stagePositions->m_offsetPos.m_y = offset.m_y;
        m_stagePositions->m_offsetPos.m_z = offset.m_z;
        updateStagePositions();
        break;
    }
    }
    fn_27_239AF4(this, &eye, &target, 0.0f, fov);
    fixposPauseCamera();
}

// The pause camera may not be moved or zoomed: it keeps the view of the contest.
void stHomerun::fixposPauseCamera() {
    cmStageParamPaused* param = &CameraController::getInstance()->m_stageCameraParamPaused;
    if (param != NULL) {
        param->m_angle = 30.0f;
        param->m_zoomIn = -3.4028235e38f;
        param->m_zoomOUt = 3.4028235e38f;
        param->m_rotYMin = 3.1415927f;
        param->m_rotYMax = 3.1415927f;
        param->m_rotXMin = 3.1415927f;
        param->m_rotXMax = 3.1415927f;
        fn_8009D158(CameraController::getInstance(), param);
    }
}

bool stHomerun::isEventEnd(int param1, int* eventState, int* eventDecision) {
    return m_eventEnd;
}

bool stHomerun::isRePlay() {
    return false;
}

bool stHomerun::isEnd() {
    switch (m_phase) {
    case 10:
        if (0.0f == m_endTimer) {
            return true;
        }
        break;
    }
    return false;
}

bool stHomerun::isFly() {
    if (m_phase > 3) {
        return true;
    }
    return m_sandBagLanded == 1;
}

bool stHomerun::isSingle() {
    stHomerunMeleeView* melee = HOMERUN_MELEE();
    if (melee == NULL) {
        return false;
    }
    return melee->m_single;
}

bool stHomerun::isCombi() {
    stHomerunMeleeView* melee = HOMERUN_MELEE();
    if (melee == NULL) {
        return false;
    }
    return melee->m_combi;
}

bool stHomerun::isVs() {
    stHomerunMeleeView* melee = HOMERUN_MELEE();
    if (melee == NULL) {
        return false;
    }
    if (melee->m_single) {
        return false;
    }
    return melee->m_combi == 0;
}

bool stHomerun::isVsEnd() {
    stHomerunMeleeView* melee = HOMERUN_MELEE();
    if (melee == NULL) {
        return false;
    }
    if (!melee->m_single && !melee->m_combi) {
        int teamA = melee->m_teamA;
        int teamB = melee->m_teamB;
        if (teamA == 1 && teamB == 1) {
            return true;
        }
    }
    return false;
}

float stHomerun::getScore() {
    return m_dist;
}

u32 stHomerun::getScoreU32() {
    return (u32)(10.0f * m_dist);
}

void stHomerun::getItemPac(gfArchive** brres, gfArchive** param, itKind itemID, int variantID, gfArchive** commonParam,
                           itCustomizerInterface** customizer) {
    if (itemID == Item_HomeRun_Sandbag) {
        *brres = &m_sandBagBrres;
        *param = &m_sandBagParam;
    } else if (itemID == Item_HomeRunBat) {
        *brres = &m_batBrres;
        *param = &m_batParam;
    }
}

// A figure was picked up: it is written down in the collection of the game.
void stHomerun::entryFigure(u32 instanceId, int itemKind, int variation) {
    if (itemKind != Item_Figure) {
        return;
    }
    if (instanceId == m_figureId[0]) {
        m_figureId[0] = 0;
    } else if (instanceId == m_figureId[1]) {
        m_figureId[1] = 0;
    } else if (instanceId == m_figureId[2]) {
        m_figureId[2] = 0;
    }
    gmStageData::ItemCollection* collection = &g_GameGlobal->m_stageData->m_itemCollection;
    if (collection->m_num < 0x100) {
        collection->m_kinds[collection->m_num] = itemKind;
        collection->m_variations[collection->m_num] = variation;
        OSReport("collection[%d] add Kind %d Variation %d\n", collection->m_num, (u8)itemKind, (u16)variation);
        collection->m_num++;
    }
    g_sndSystem->playSE(snd_se_Homerun_figure_get, 0, 0, 0, -1);
}

void stHomerun::interpolateSub(Vec3f* out, stHomerunKey3* key) {
    if (out != NULL && key != NULL) {
        float t;
        if (0.0f == key->m_duration) {
            t = 1.0f;
        } else {
            t = key->m_elapsed / key->m_duration;
            if (t < 0.0f) {
                t = 0.0f;
            }
            if (t > 1.0f) {
                t = 1.0f;
            }
        }
        fn_27_239BDC(this, out, &key->m_start, &key->m_end, t, key->m_ease);
        if (1.0f == t) {
            key->m_start.m_x = key->m_end.m_x;
            key->m_start.m_y = key->m_end.m_y;
            key->m_start.m_z = key->m_end.m_z;
        }
    }
}

void stHomerun::interpolateSub1(float* out, stHomerunKey1* key) {
    if (out != NULL && key != NULL) {
        float t;
        if (0.0f == key->m_duration) {
            t = 1.0f;
        } else {
            t = key->m_elapsed / key->m_duration;
            if (t < 0.0f) {
                t = 0.0f;
            }
            if (t > 1.0f) {
                t = 1.0f;
            }
        }
        fn_27_239E58(this, out, key->m_start, key->m_end, t, key->m_ease);
        if (1.0f == t) {
            key->m_start = key->m_end;
        }
    }
}

// Places the camera at the given distance from the target, looking along the direction the angle describes (the
// first angle is the pitch, the second the heading, both in degrees).
void stHomerun::calcCameraParam(float t, Vec3f* out, Vec3f* eye, Vec3f* angle) {
    if (out != NULL && eye != NULL && angle != NULL) {
        float sinV;
        float cosV;
        nw4r::math::SinCosDeg(&sinV, &cosV, angle->m_y);
        Vec3f dir;
        dir.m_x = cosV;
        float len = nw4r::math::FSqrt(cosV * cosV + sinV * sinV);
        float s = nw4r::math::SinDeg(angle->m_x);
        float c = nw4r::math::CosDeg(angle->m_x);
        dir.m_z = sinV;
        dir.m_y = (len * s) / c;
        dir.normalize();
        dir.m_x *= t;
        dir.m_y *= t;
        dir.m_z *= t;
        *out = *eye + dir;
    }
}

void stHomerun::setReadyGo(bool readyGo) {
    m_readyGo = readyGo;
}

void stHomerun::setRePlay(bool rePlay) {
    m_rePlay = rePlay;
}

void stHomerun::setScoreBest(u8 index, float score) {
    m_scoreBest[index] = score;
}

void stHomerun::setScoreBestCombi(float score) {
    m_scoreBestCombi = score;
}

void stHomerun::setItemSandBag(BaseItem* sandBag) {
    m_sandBag = sandBag;
}
