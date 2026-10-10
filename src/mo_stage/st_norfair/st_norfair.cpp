#include <ai/ai_mgr.h>
#include <cm/cm_camera_controller.h>
#include <cm/cm_quake.h>
#include <ec/ec_mgr.h>
#include <gf/gf_3d_scene.h>
#include <gf/gf_archive.h>
#include <gm/gm_global.h>
#include <gr/gr_madein.h>
#include <it/it_manager.h>
#include <memory.h>
#include <mt/mt_prng.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <nw4r/ut/ut_Color.h>
#include <snd/snd_system.h>
#include <st/st_class_info.h>
#include <st/st_melee.h>
#include <st/st_positions.h>
#include <string.h>
#include <types.h>

#include <st_norfair/gr_norfair.h>
#include <st_norfair/st_norfair.h>

// itManager::lotCreateItem (sora_melee, unnamed)
extern "C" void fn_27_2AA7AC(itManager* manager, ItemKind kind, int lotId, Vec3f* pos, int a, int b, int c);
// cmSetQuakeScale (main, unnamed)
extern "C" void fn_8009E090(float scale);
// the stage that is played (sora_melee .bss)
extern "C" Stage* lbl_27_bss_5668;

// MATCH-ONLY: the first bytes of the match settings (the game rule is a bit field of three bits)
struct norfairMeleeView {
    u8 m_gameMode : 6;
    u8 _unused0 : 2;
    u8 m_gameRule : 3;
    u8 _unused1 : 5;
};

// A row of the table of the events (the file of the stage).
struct stNorfairEventRow {
    float m_waitMin;        // frames until the next event
    float m_waitMax;
    float m_chance[4];      // the chances of the events of the kind 1, 2, 3 and 4
};

#define NORFAIR_MADEIN(index) static_cast<grMadein*>(getGround(index))

// MATCH-ONLY: a word based view of the bit fields of soCollisionAttackData (from +0x30). The shared header declares its
// single-bit flags as bool, which the compiler accesses one byte at a time; the original sets all of them with one
// read-modify-write per word.
struct stNorfairAttackBits {
    u32 m_nodeIndex : 9;
    u32 m_targetCategory : 10;
    u32 m_targetSituation : 3;
    u32 m_targetLr : 1;
    u32 m_targetPart : 4;
    u32 m_attribute : 5;
    u32 m_soundLevel : 2;
    u32 m_soundAttribute : 5;
    u32 m_setOffKind : 2;
    u32 m_noScale : 1;
    u32 m_isShieldable : 1;
    u32 m_isReflectable : 1;
    u32 m_isAbsorbable : 1;
    u32 m_subShield : 9;
    u32 _34 : 10;
    u32 m_serialHitFrame : 16;
    u32 m_isDirect : 1;
    u32 m_isInvalidInvincible : 1;
    u32 m_isInvalidXlu : 1;
    u32 m_lrCheck : 3;
    u32 m_isCatch : 1;
    u32 m_noTeam : 1;
    u32 m_noHitStop : 1;
    u32 m_noEffect : 1;
    u32 m_noTransaction : 1;
    u32 m_region : 5;
    u32 m_shapeType : 1;
    u32 m_isDeath100 : 1;
    u32 _3C : 30;
};

// The damage ground of the wave: it burns (power 18) and knocks the fighter away with the base knockback of the number given.
#define NORFAIR_WAVE_ATTACK(index, reactionAddArg)                                              \
    madein = NORFAIR_MADEIN(index);                                                              \
    *reinterpret_cast<u32*>(reinterpret_cast<u8*>(madein) + 0x174) = 14;                         \
    madein->setAttack(5.0f, &offset);                                                            \
    madein->setAttackPreset(grMadein::Attack_Overwrite);                                         \
    attackData = madein->getOverwriteAttackData();                                               \
    bits = reinterpret_cast<stNorfairAttackBits*>(reinterpret_cast<u8*>(attackData) + 0x30);     \
    attackData->m_reactionEffect = 50;                                                           \
    attackData->m_reactionFix = 0;                                                               \
    attackData->m_reactionAdd = (reactionAddArg);                                                \
    attackData->m_power = 18;                                                                    \
    attackData->m_vector = 80;                                                                   \
    attackData->m_nodeIndex = 0;                                                                 \
    bits->m_attribute = soCollisionAttackData::Attribute_Fire;                                   \
    bits->m_targetSituation = 7;                                                                 \
    attackData->m_size = 5.0f;                                                                   \
    bits->m_targetCategory = 0x3FF;                                                              \
    bits->m_targetLr = 0;                                                                        \
    bits->m_targetPart = 0xF;                                                                    \
    attackData->m_offsetPos.m_x = offset.m_x;                                                    \
    attackData->m_offsetPos.m_y = offset.m_y;                                                    \
    attackData->m_offsetPos.m_z = offset.m_z;                                                    \
    bits->m_setOffKind = 0;                                                                      \
    bits->m_noScale = 0;                                                                         \
    bits->m_soundLevel = soCollisionAttackData::Sound_Level_Large;                               \
    bits->m_soundAttribute = soCollisionAttackData::Sound_Attribute_None;                        \
    bits->m_isShieldable = 1;                                                                    \
    bits->m_isReflectable = 0;                                                                   \
    bits->m_isAbsorbable = 0;                                                                    \
    bits->m_serialHitFrame = 60;                                                                 \
    bits->m_isDirect = 0;                                                                        \
    bits->m_isInvalidInvincible = 0;                                                             \
    bits->m_isInvalidXlu = 0;                                                                    \
    bits->m_lrCheck = soCollisionAttackData::Lr_Check_Lr;                                        \
    bits->m_isCatch = 0;                                                                         \
    bits->m_noTeam = 0;                                                                          \
    bits->m_noHitStop = 1;                                                                       \
    bits->m_noEffect = 0;                                                                        \
    bits->m_noTransaction = 0;                                                                   \
    bits->m_shapeType = soCollision::Shape_Capsule;

// HYPOTHESIS: two small constant objects (an 0xFF marker with an index) that the unit's headers instantiate.
struct stNorfairMarker {
    int m_a;
    int m_b;
    stNorfairMarker(int a, int b) : m_a(a), m_b(b) { }
};
static stNorfairMarker sMarkerA(0xFF, 0);
static stNorfairMarker sMarkerB(0xFF, 1);

stClassInfoImpl<Stages::Norfair, stNorfair> stNorfair::bss_loc_14;

stNorfair* stNorfair::create() {
    return new (Heaps::StageInstance) stNorfair;
}

stNorfair::stNorfair() : stMelee("stNorfair", Stages::Norfair) {
    m_bgState = 0;
    m_posLimit[0].m_x = 0.0f;
    m_posLimit[0].m_y = 0.0f;
    m_posLimit[0].m_z = 0.0f;
    m_posLimit[1].m_x = 0.0f;
    m_posLimit[1].m_y = 0.0f;
    m_posLimit[1].m_z = 0.0f;
    m_eventWait = 0;
    m_eventTimerTarget = 0.0f;
    m_eventTimer = 0.0f;
    m_eventKind = 0;
    m_lastEventKind = 0;
    m_eventCount = 1;
    m_posMagma.m_x = 0.0f;
    m_posMagma.m_y = 0.0f;
    m_posMagma.m_z = 0.0f;
    m_cameraBottom = 0.0f;
    m_cameraInit = 1;
    memset(m_posZone, 0, sizeof(m_posZone));
    m_zoneIndex = 0;
    m_zoneState = 8;
    memset(m_zoneIn, 0, sizeof(m_zoneIn));
    memset(m_posAshibaT, 0, sizeof(m_posAshibaT));
    m_tblMagma = NULL;
    m_tblEvent = NULL;
    m_bgmStarted = 0;
    m_eventResult = 0;
    m_trainerEvent = 0;
    m_dangerZone[0] = -1;
    m_dangerZone[1] = -1;
    m_dangerZone[2] = -1;
    m_dangerZone[3] = -1;
    m_dangerZone[4] = -1;
}

stNorfair::~stNorfair() {
    if (m_tblMagma != NULL) {
        delete m_tblMagma;
    }
    m_tblMagma = NULL;
    if (m_tblEvent != NULL) {
        delete m_tblEvent;
    }
    m_tblEvent = NULL;
    releaseArchive();
}

bool stNorfair::loading() {
    return true;
}

// Every damage ground (fire from the lava, and the waves) is a madein ground without a model: only its attack matters.
#define NORFAIR_DAMAGE_GROUND(name)                                                              \
    ground = grMadein::create(9, "", name, Heaps::StageInstance);                               \
    addGround(ground);                                                                           \
    ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);                                   \
    ground->setStageData(m_stageData);                                                           \
    if (ground != NULL) {

// A fire from the lava: it burns (power 10) and sends the fighter up.
#define NORFAIR_FIRE_ATTACK(index, fireSize)                                                     \
    madein = NORFAIR_MADEIN(index);                                                              \
    madein->setAttack(fireSize, &offset);                                                        \
    madein->setAttackPreset(grMadein::Attack_Overwrite);                                         \
    attackData = madein->getOverwriteAttackData();                                               \
    madein->setAttackGimmickDetails(attackData, fireSize, 1.0f, 1.0f, 1.0f,                      \
        10, &offset, 361, 100, 0, 70, 0,                                                         \
        0x3FF, 7, false, 15,                                                                     \
        soCollisionAttackData::Attribute_Fire, soCollisionAttackData::Sound_Level_Medium,        \
        soCollisionAttackData::Sound_Attribute_Fire,                                             \
        false, false, false, true, false, false, 0, 60,                                          \
        false, false, false, soCollisionAttackData::Lr_Check_Pos,                                \
        false, false, false, false, false, soCollisionAttackData::Region_None, true);

void stNorfair::createObj() {
    testStageParamInit(m_fileData, 10);
    testStageDataInit(m_fileData, 0x14, 0x6C);
    initStageDataTbl();
    createObjBg(0);
    createObjMagma(1);
    createObjDoor(2);
    createObjZone(3);
    createObjShutter(4);
    createObjAshiba(5);
    createObjAshibaT(6);
    createObjWall(7);
    createObjWall(8);
    createObjWave(9);
    grMadein* ground;
    grMadein* madein;
    soCollisionAttackData* attackData;
    stNorfairAttackBits* bits;
    Vec3f offset;
    {
        NORFAIR_DAMAGE_GROUND("dmg1")
        NORFAIR_DAMAGE_GROUND("dmg2")
        NORFAIR_DAMAGE_GROUND("dmg3")
        NORFAIR_DAMAGE_GROUND("dmg4")
        NORFAIR_DAMAGE_GROUND("dmg5")
        NORFAIR_DAMAGE_GROUND("dmg_wave1")
        NORFAIR_DAMAGE_GROUND("dmg_wave2")
        NORFAIR_DAMAGE_GROUND("dmg_wave3")
        NORFAIR_DAMAGE_GROUND("dmg_wave4")
        offset.m_x = 0.0f;
        offset.m_y = -200.0f;
        offset.m_z = 0.0f;
        NORFAIR_FIRE_ATTACK(10, 22.0f)
        NORFAIR_FIRE_ATTACK(11, 22.0f)
        NORFAIR_FIRE_ATTACK(12, 25.0f)
        NORFAIR_FIRE_ATTACK(13, 22.0f)
        NORFAIR_FIRE_ATTACK(14, 22.0f)
        // The four damage grounds of the wave: the wave burns a fighter (power 18) and hits hard.
        offset.m_x = 0.0f;
        offset.m_y = 0.0f;
        offset.m_z = 0.0f;
        NORFAIR_WAVE_ATTACK(15, 80)
        NORFAIR_WAVE_ATTACK(16, 110)
        NORFAIR_WAVE_ATTACK(17, 110)
        NORFAIR_WAVE_ATTACK(18, 110)
        NORFAIR_MADEIN(10)->initializeEntity();
        NORFAIR_MADEIN(11)->initializeEntity();
        NORFAIR_MADEIN(12)->initializeEntity();
        NORFAIR_MADEIN(13)->initializeEntity();
        NORFAIR_MADEIN(14)->initializeEntity();
        NORFAIR_MADEIN(15)->initializeEntity();
        NORFAIR_MADEIN(16)->initializeEntity();
        NORFAIR_MADEIN(17)->initializeEntity();
        NORFAIR_MADEIN(18)->initializeEntity();
        createCollision(m_fileData, 2, NULL);
        initCameraParam();
        void* posData = m_fileData->getData(Data_Type_Model, 100, 0xFFFE);
        if (posData != NULL) {
            nw4r::g3d::ResFile posFile(posData);
            createStagePositions(&posFile);
        } else {
            createStagePositions();
        }
        createWind2ndOnly();
        registScnAnim(static_cast<nw4r::g3d::ResFileData*>(m_fileData->getData(Data_Type_Scene, 0, 0xFFFE)), 0);
        m_posAshiba[0].m_x = -90.0f;
        m_posAshiba[0].m_y = 0.0f;
        m_posAshiba[0].m_z = 50.0f;
        m_posAshiba[1].m_x = -50.0f;
        m_posAshiba[1].m_y = 0.0f;
        m_posAshiba[1].m_z = 25.0f;
        m_posAshiba[2].m_x = 0.0f;
        m_posAshiba[2].m_y = 0.0f;
        m_posAshiba[2].m_z = -25.0f;
        m_posAshiba[3].m_x = 50.0f;
        m_posAshiba[3].m_y = 0.0f;
        m_posAshiba[3].m_z = 25.0f;
        m_posAshiba[4].m_x = 90.0f;
        m_posAshiba[4].m_y = 0.0f;
        m_posAshiba[4].m_z = 50.0f;
        for (int i = 0; i < 5; i++) {
            m_eventAttack[i].set(30.0f, 600.0f);
            Vec3f pos;
            pos.m_x = m_posAshiba[i].m_x;
            pos.m_y = m_posAshiba[i].m_y;
            switch (i) {
            case 0:
                pos.m_y = 70.0f;
                break;
            case 1:
                pos.m_y = 40.0f;
                break;
            case 2:
                pos.m_y = 15.0f;
                break;
            case 3:
                pos.m_y = 40.0f;
                break;
            case 4:
                pos.m_y = 70.0f;
                break;
            }
            pos.m_z = 0.0f;
            NORFAIR_MADEIN(i + 10)->setPos(&pos);
        }
        m_eventQuake.set(200.0f, 400.0f);
        m_eventBg.set(100.0f, 200.0f);
        m_eventWave.set(100.0f, 200.0f);
        loadStageAttrParam(m_fileData, 0x1E);
        initPosPokeTrainer(1, 0);
        createObjPokeTrainer(m_fileData, 0x65, "PokeTrainer00", m_pokeTrainerPos, NULL);
        gmGlobalModeMelee* melee = g_GameGlobal->m_modeMelee;
        if (melee != NULL && reinterpret_cast<norfairMeleeView*>(&melee->m_meleeInitData)->m_gameMode == 7
            && reinterpret_cast<u8*>(&melee->m_meleeInitData)[8] == 0x1C) {
            m_trainerEvent = 1;
            m_eventCount = 0;
        }
    }
    }}}}}}}}}
}

void stNorfair::createObjBg(int index) {
    grNorfairBg* ground;
    switch (index) {
    case 0:
        ground = grNorfairBg::create(0, "TopN", "grNorfairMainBg");
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setPosZoneWork(m_posZone);
        ground->setPosAshibaTWork(m_posAshibaT);
    }
}

void stNorfair::createObjMagma(int index) {
    grNorfairMagma* ground;
    switch (index) {
    case 1:
        ground = grNorfairMagma::create(1, "magma", "grNorfairMagma");
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setEventIDWork(&m_eventKind);
        ground->setTblLevelAcc(m_tblMagma);
        ground->setPosMagmaWork(&m_posMagma);
        ground->setPosLimitWork(m_posLimit);
    }
}

void stNorfair::createObjDoor(int index) {
    grNorfairDoor* ground;
    switch (index) {
    case 2:
        ground = grNorfairDoor::create(2, "StgNorfairSafetydoor", "grNorfairDoor");
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setPosWork(m_posZone);
        ground->setPosIndex(&m_zoneIndex);
        ground->setStateWork(&m_zoneState);
        ground->setEventIDWork(&m_eventKind);
    }
}

void stNorfair::createObjZone(int index) {
    grNorfairZone* ground;
    switch (index) {
    case 3:
        ground = grNorfairZone::create(3, "StgNorfairSafetyZone", "grNorfairZone");
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setPosWork(m_posZone);
        ground->setPosIndex(&m_zoneIndex);
        ground->setStateWork(&m_zoneState);
        ground->setEventIDWork(&m_eventKind);
        ground->setInOutWork(m_zoneIn);
        ground->setPosLimitWork(m_posLimit);
    }
}

void stNorfair::createObjShutter(int index) {
    grNorfairShutter* ground;
    switch (index) {
    case 4:
        ground = grNorfairShutter::create(4, "StgNorfairSafetyZoneshutter", "grNorfairShutter");
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setPosWork(m_posZone);
        ground->setPosIndex(&m_zoneIndex);
        ground->setEventIDWork(&m_eventKind);
    }
}

void stNorfair::createObjAshiba(int index) {
    grNorfairAshiba* ground;
    switch (index) {
    case 5:
        ground = grNorfairAshiba::create(10, "StgNorfairAsiba", "grNorfairAshiba");
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setPosIndex(&m_zoneIndex);
        ground->setEventIDWork(&m_eventKind);
    }
}

void stNorfair::createObjAshibaT(int index) {
    grNorfairAshibaT* ground;
    switch (index) {
    case 6:
        ground = grNorfairAshibaT::create(5, "asiba_trainer", "grNorfairAshibaT");
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setPosWork(m_posAshibaT);
        ground->setPosMagmaWork(&m_posMagma);
        ground->setEventIDWork(&m_eventKind);
        ground->setVisibility(isPokemonTrainer());
    }
}

void stNorfair::createObjWall(int index) {
    grNorfairWall* ground;
    bool isRight;
    switch (index) {
    case 7:
        ground = grNorfairWall::create(6, "magma_wall_a", "grNorfairWall01");
        isRight = true;
        break;
    case 8:
        ground = grNorfairWall::create(7, "magma_wall_b", "grNorfairWall02");
        isRight = false;
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setEventIDWork(&m_eventKind);
        ground->setPosLimitWork(m_posLimit);
        if (isRight == true) {
            ground->setTypeRight();
        } else {
            ground->setTypeLeft();
        }
    }
}

void stNorfair::createObjWave(int index) {
    grNorfairWave* ground;
    switch (index) {
    case 9:
        ground = grNorfairWave::create(8, "magma_wave", "grNorfairWave");
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setEventIDWork(&m_eventKind);
    }
}

void stNorfair::update(float deltaFrame) {
    updateLimit();
    updateEvent(deltaFrame);
    CameraController* camera = CameraController::getInstance();
    Rect2D range = *reinterpret_cast<Rect2D*>(reinterpret_cast<u8*>(camera) + 0x148);
    if (m_cameraInit == 1) {
        m_cameraBottom = range.m_down;
        m_cameraInit = 0;
    }
    range.m_down = getMagmaHeight();
    if (range.m_down < m_cameraBottom) {
        range.m_down = m_cameraBottom;
    }
    CameraController::getInstance()->setCameraRange(&range);
    if (m_bgmStarted == 0) {
        g_sndSystem->playSE(static_cast<SndID>(0x1BC3), 0, 0, 0, -1);
        m_bgmStarted = 1;
    }
}

// Mirrors the limits of the camera into the stage so the grounds can stay inside them.
static inline void norfairSetVec(Vec3f* vec, float x, float y, float z) {
    vec->m_x = x;
    vec->m_y = y;
    vec->m_z = z;
}

void stNorfair::updateLimit() {
    CameraController* camera = CameraController::getInstance();
    norfairSetVec(&m_posLimit[0], camera->unk158, camera->unk160, 0.0f);
    norfairSetVec(&m_posLimit[1], camera->unk15C, camera->unk164, 0.0f);
}

// The scheduler of the stage: it waits a time of the table, picks one of the events by the chances of the row of the table
// (the same event twice is less likely) and runs it. The five fires run their own sequence on their own.
void stNorfair::updateEvent(float deltaFrame) {
    Vec3f pos;
    Vec2f zoneB;
    Vec2f zoneA;
    Vec3f playerPos;
    Vec3f dropPos;
    Vec3f quakePos2;
    Vec3f quakePos;
    stNorfairData* data = static_cast<stNorfairData*>(m_stageData);
    if (data == NULL) {
        return;
    }
    m_eventTimer += deltaFrame;
    stNorfairEventRow* row = reinterpret_cast<stNorfairEventRow*>(m_tblEvent->getData(m_eventCount));
    if (row == NULL) {
        m_eventCount = 1;
        m_eventTimer = 0.0f;
        m_eventTimerTarget = 0.0f;
        return;
    }
    switch (m_eventWait) {
    case 0: {
        float r = randf();
        m_eventWait = 1;
        m_eventTimerTarget = m_eventTimerTarget + (row->m_waitMin + (row->m_waitMax - row->m_waitMin) * r);
        break;
    }
    case 1:
        if (m_eventTimer >= m_eventTimerTarget) {
            if (m_eventKind != 0 && m_eventBg.isEvent()) {
                m_eventBg.end();
            }
            gmGlobalModeMelee* melee = g_GameGlobal->m_modeMelee;
            if (melee == NULL) {
                return;
            }
            bool tooLate = false;
            switch (reinterpret_cast<norfairMeleeView*>(&melee->m_meleeInitData)->m_gameRule) {
            case 0:
                if (getFrameRuleTime() != 0.0f && getFrameRuleTime() < data->unk04) {
                    tooLate = true;
                    m_eventTimer = 0.0f;
                }
                break;
            }
            if (tooLate != true) {
                float chance1 = row->m_chance[0];
                float chance2 = row->m_chance[1];
                float chance3 = row->m_chance[2];
                float chance4 = row->m_chance[3];
                switch (m_lastEventKind) {
                case 1:
                    chance1 = chance1 * data->unk00;
                    break;
                case 2:
                    chance2 = chance2 * data->unk00;
                    break;
                case 3:
                    chance3 = chance3 * data->unk00;
                    break;
                case 4:
                    chance4 = chance4 * data->unk00;
                    break;
                }
                float total = chance4 + chance3 + chance1 + chance2;
                float p1 = chance1 / total;
                float p2 = chance2 / total;
                float p3 = chance3 / total;
                float p4 = chance4 / total;
                float r = randf();
                if (r < p1) {
                    m_eventKind = 1;
                } else if (r < (float)(p1 + p2)) {
                    m_eventKind = 2;
                } else {
                    float sum = (float)(p3 + (float)(p1 + p2));
                    if (r < sum) {
                        m_eventKind = 3;
                    } else if (r < (float)(p4 + sum)) {
                        m_eventKind = 4;
                    } else {
                        m_eventKind = 0;
                    }
                }
                m_lastEventKind = m_eventKind;
                m_eventCount = m_eventCount + 1;
                m_eventWait = 0;
                switch (m_eventKind) {
                case 3: {
                    m_eventQuake.end();
                    m_eventQuake.start();
                    int count = randi(5);
                    count = count + 1;
                    if (count >= 5) {
                        count = 5;
                    }
                    for (int i = 0; i < count; i++) {
                        int fire = randi(5);
                        if (fire >= 4) {
                            fire = 4;
                        }
                        m_eventAttack[fire].start();
                    }
                    break;
                }
                case 1:
                    m_eventQuake.end();
                    m_eventQuake.start();
                    m_eventBg.end();
                    m_eventBg.start();
                    m_eventBg.setPhase(0);
                    break;
                case 2:
                    m_eventQuake.end();
                    m_eventQuake.start();
                    m_eventBg.end();
                    m_eventBg.start();
                    m_eventBg.setPhase(10);
                    break;
                case 4: {
                    float r2 = randf();
                    if (r2 < 0.33333334f) {
                        m_zoneIndex = 0;
                    } else if (r2 < 0.6666667f) {
                        m_zoneIndex = 2;
                    } else {
                        m_zoneIndex = 4;
                    }
                    m_eventWave.end();
                    m_eventWave.start();
                    m_eventResult = 0;
                    m_bgState = 1;
                    break;
                }
                }
            }
        }
        break;
    }
    for (int i = 0; i < 5; i++) {
        m_eventAttack[i].update(deltaFrame);
        if (m_eventAttack[i].isReadyEnd() == 1) {
            switch (m_eventAttack[i].getPhase()) {
            case 0: {
                g_ecMgr->setDrawPrio(1);
                u32 effect = g_ecMgr->setEffect(static_cast<EfID>(0x3C0001));
                g_ecMgr->setDrawPrio(-1);
                g_ecMgr->setPos(effect, &m_posAshiba[i]);
                m_eventAttack[i].setPhase(1);
                m_eventAttack[i].m_manualFramesLeft = 0.0f;
                g_sndSystem->playSE(static_cast<SndID>(0x1BC7), 0, 0, 0, -1);
                pos = NORFAIR_MADEIN(i + 10)->getPos();
                zoneA.m_x = pos.m_x - 30.0f;
                zoneA.m_y = pos.m_y + 50.0f;
                zoneB.m_x = pos.m_x + 30.0f;
                zoneB.m_y = pos.m_y - 100.0f;
                m_dangerZone[i] = g_aiMgr->setDangerZone(&zoneA, &zoneB, m_dangerZone[i], false, false);
                break;
            }
            case 1: {
                float frames = m_eventAttack[i].m_manualFramesLeft;
                m_eventAttack[i].m_manualFramesLeft = frames + deltaFrame;
                if (135.0f < frames + deltaFrame) {
                    m_eventAttack[i].m_manualFramesLeft = 0.0f;
                    m_eventAttack[i].setPhase(2);
                    NORFAIR_MADEIN(i + 10)->startEntity();
                }
                break;
            }
            case 2: {
                float frames = m_eventAttack[i].m_manualFramesLeft;
                m_eventAttack[i].m_manualFramesLeft = frames + deltaFrame;
                if (70.0f < frames + deltaFrame) {
                    m_eventAttack[i].end();
                    NORFAIR_MADEIN(i + 10)->endEntity();
                    if (m_dangerZone[i] != -1) {
                        g_aiMgr->delDangerZone(m_dangerZone[i]);
                    }
                    m_dangerZone[i] = -1;
                }
                break;
            }
            }
        }
    }
    if (m_eventKind != 4 && m_eventKind < 4 && m_eventKind != 0) {
        m_eventQuake.update(deltaFrame);
        if (m_eventQuake.isEvent()) {
            if (m_eventQuake.isReadyEnd() == 1) {
                cmRemoveQuake(1);
                m_eventQuake.end();
            } else if (100.0f <= m_eventQuake.m_framesElapsed) {
                quakePos.m_x = 0.0f;
                quakePos.m_y = 0.0f;
                quakePos.m_z = 0.0f;
                cmReqQuake(cmQuake::Amplitude_Middle, &quakePos);
            }
        }
    }
    m_eventBg.update(deltaFrame);
    m_eventWave.update(deltaFrame);
    if (m_eventWave.isEvent()) {
        switch (m_eventWave.getPhase()) {
        case 1:
            for (int i = 0; i < 4; i++) {
                grMadein* madein = NORFAIR_MADEIN(i + 15);
                if (madein != NULL) {
                    madein->endEntity();
                }
            }
            m_eventWave.end();
            break;
        case 0: {
            quakePos2.m_x = 0.0f;
            quakePos2.m_y = 0.0f;
            quakePos2.m_z = 0.0f;
            cmReqQuake(cmQuake::Amplitude_Middle, &quakePos2);
            fn_8009E090(1.0f);
            m_eventWave.m_manualFramesLeft = m_eventWave.m_manualFramesLeft + deltaFrame;
            if (g_GameGlobal->isPrevJustGameFrame() == 1 && 420.0f <= m_eventWave.m_manualFramesLeft) {
                bool anyIn = false;
                for (int i = 0; i < 4; i++) {
                    if (getPlayerPosition(i, &playerPos) == 1) {
                        if (m_zoneIn[i] == 1) {
                            if (m_trainerEvent == 1 && i == 0) {
                                m_eventResult = 1;
                            }
                            anyIn = true;
                        } else if (playerPos.m_y < 100.0f) {
                            NORFAIR_MADEIN(i + 15)->startEntity();
                            NORFAIR_MADEIN(i + 15)->setPos(&playerPos);
                        }
                    }
                }
                if (g_GameGlobal->m_modeMelee != NULL && reinterpret_cast<u8*>(&g_GameGlobal->m_modeMelee->m_meleeInitData)[0x16 - 8] == 0) {
                    anyIn = false;
                }
                if (anyIn) {
                    dropPos.m_x = m_posZone[m_zoneIndex].m_x;
                    dropPos.m_y = m_posZone[m_zoneIndex].m_y + 10.0f;
                    dropPos.m_z = m_posZone[m_zoneIndex].m_z;
                    ItemKind kind;
                    kind.m_kind = static_cast<itKind>(*reinterpret_cast<int*>(reinterpret_cast<u8*>(lbl_27_bss_5668) + 0x44));
                    kind.m_variation = 0;
                    fn_27_2AA7AC(itManager::getInstance(), kind, 1, &dropPos, -1, -1, -1);
                }
                m_eventWave.setPhase(1);
                m_bgState = 3;
            }
            break;
        }
        }
    }
    if (m_eventBg.isEvent() && m_eventKind == 0) {
        m_eventBg.end();
    }
    switch (m_bgState) {
    case 2:
        g_gfSceneRoot->setCurrentFrame(60.0f);
        break;
    case 0:
        g_gfSceneRoot->setCurrentFrame(0.0f);
        break;
    case 1: {
        float frame = 0.0f;
        if (g_gfSceneRoot->m_anmScnRes != NULL) {
            frame = g_gfSceneRoot->m_anmScnRes->GetFrame();
        }
        if (60.0f <= frame) {
            g_gfSceneRoot->setCurrentFrame(60.0f);
            m_bgState = 2;
        }
        break;
    }
    case 3: {
        float frame = 0.0f;
        if (g_gfSceneRoot->m_anmScnRes != NULL) {
            frame = g_gfSceneRoot->m_anmScnRes->GetFrame();
        }
        if (frame < 60.0f) {
            g_gfSceneRoot->setCurrentFrame(0.0f);
            m_bgState = 0;
        }
        break;
    }
    }
}

void stNorfair::initStageDataTbl() {
    if (m_fileData != NULL) {
        stDataContainerData* data = static_cast<stDataContainerData*>(m_fileData->getData(Data_Type_Misc, 0x15, 0xFFFE));
        if (data != NULL) {
            m_tblMagma = stDataMultiContainer::create(data, Heaps::StageInstance);
        }
        data = static_cast<stDataContainerData*>(m_fileData->getData(Data_Type_Misc, 0x16, 0xFFFE));
        if (data != NULL) {
            m_tblEvent = stDataMultiContainer::create(data, Heaps::StageInstance);
        }
    }
}

// The match ends with the event of the adventure after the wave went over a fighter of the adventure.
bool stNorfair::isEventEnd(int param1, int* eventState, int* eventDecision) {
    if (m_trainerEvent == 0) {
        return false;
    }
    if (m_eventCount != 0 && !m_eventWave.isEvent()) {
        if (m_eventResult == 1) {
            *eventState = 7;
            *eventDecision = 4;
        } else {
            *eventState = 8;
            *eventDecision = 3;
        }
        return true;
    }
    return false;
}

int stNorfair::getZoneState() {
    return m_zoneState;
}

void stNorfair::getZonePos(Vec3f* pos) {
    if (pos == NULL) {
        return;
    }
    *pos = m_posZone[m_zoneIndex];
}

float stNorfair::getMagmaHeight() {
    return m_posMagma.m_y;
}

GXColor stNorfair::getFinalTechniqColor() {
    return nw4r::ut::Color(0x1400047D);
}
