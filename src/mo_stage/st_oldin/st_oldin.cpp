#include <ai/ai_mgr.h>
#include <cm/cm_camera_controller.h>
#include <cm/cm_quake.h>
#include <cm/cm_subject.h>
#include <ec/ec_mgr.h>
#include <ef/ef_screen.h>
#include <gf/gf_archive.h>
#include <gf/gf_model.h>
#include <gm/gm_global.h>
#include <gm/gm_lib.h>
#include <gr/collision/gr_collision.h>
#include <gr/gr_madein.h>
#include <memory.h>
#include <mt/mt_matrix.h>
#include <mt/mt_prng.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <snd/snd_3d_generator.h>
#include <snd/snd_id.h>
#include <st/st_class_info.h>
#include <st/st_melee.h>
#include <st/st_utility.h>
#include <stdio.h>
#include <string.h>
#include <types.h>
#include <yk/yakumono.h>

#include <st_oldin/st_oldin.h>

// Unnamed helpers the stage calls directly (see st_dxcorneria.cpp for the grMadein ones).
// grMadein::setMatrix(Matrix*, bool): lets the ground (and optionally its attack) follow the matrix.
extern "C" void fn_27_279FC8(grMadein* ground, Matrix* matrix, bool followAttack);
// grMadein::switchToMatrix
extern "C" void fn_27_279FA4(grMadein* ground);
// grMadein: HYPOTHESIS: switches the looping flag of the ground's motion.
extern "C" void fn_27_279228(grMadein* ground, int loop);
// efScreen: ends a fill requested with requestFill (HYPOTHESIS: fades it out over `frames`).
extern "C" void fn_8005E3C8(efScreen* screen, efScreenHandle* handle, float frames);
// Matrix helpers of sora/mt (see mt_matrix.cpp).
extern "C" void fn_8003E918(Matrix* mtx, float angle);
extern "C" void fn_8003F074(Matrix* mtx, float x, float y, float z);

stClassInfoImpl<Stages::Oldin, stOldin> stOldin::bss_loc_14;

// The values the stage is tuned with, preceded by the word the coroutine tags of the stage start with.
struct stOldinStatics {
    int m_noTag;
    stOldinParam m_param;
};
static stOldinStatics sStatics = {
    0,
    {900.0f, 600.0f, 1200.0f, 0.6666667f, 180.0f, {5.0f, 2.0f, 2.0f, 1.0f}, 75.0f, 20.0f, 900.0f, 1200.0f},
};

// The coroutine markers of the enemies and the bridge (see NEWPORK_SAVE in st_newpork.cpp). The marker of the last
// state of a coroutine has the value 2.
#define OLDIN_SAVE(seq, lineNumber, tagValue) \
    {                                         \
        static int sTag = (tagValue);         \
        (seq).m_tag = sTag;                   \
        (seq).m_line = (lineNumber);          \
    }

// MATCH-ONLY: the original reads the stage the match is played on (6 bits at +0x8 of the melee data) and its number.
struct stOldinMeleeView {
    char _0[8];
    u8 mode : 6;
    u8 pad : 2;
    char _9[7];
    u8 stage;
};

// MATCH-ONLY: a word based view of the bit fields of soCollisionAttackData (from +0x30). The shared header declares its
// single-bit flags as bool, which the compiler accesses one byte at a time; the original sets all of them with one
// read-modify-write per word.
struct stOldinAttackBits {
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

// Parks a ground far below the stage (its first node matrix is moved to (0, -1000, 0)).
#define OLDIN_PARK(ground)                                                          \
    {                                                                               \
        Vec3f parked(0.0f, -1000.0f, 0.0f);                                         \
        grNodeCallbackData* data = (ground)->m_calcWorldCallBack.m_nodeCallbackDatas; \
        data->m_matrix.m[0][3] = parked.m_x;                                        \
        data->m_matrix.m[1][3] = parked.m_y;                                        \
        data->m_matrix.m[2][3] = parked.m_z;                                        \
    }

// Restarts the first motion of a ground at a given frame.
#define OLDIN_SET_FRAME(ground, frame)                      \
    {                                                       \
        gfModelAnimation* anim = (ground)->m_modelAnims[0]; \
        if (anim != NULL) {                                 \
            anim->setFrame(frame);                          \
        }                                                   \
    }

stOldin* stOldin::create() {
    return new (Heaps::StageInstance) stOldin;
}

stOldin::stOldin()
    : stMelee("stOldin", Stages::Oldin), m_lordSeq(sStatics.m_noTag), m_kingMtx(true), m_lordMtx(true),
      m_lordNodeMtx(true), m_lordSubject(0, 1), m_kingSeq(sStatics.m_noTag), m_bulblinSeq(sStatics.m_noTag),
      m_bulblinMtx(true), m_bulblinNodeMtx(true), m_taruSeq(sStatics.m_noTag), m_taruSubject(0, 1),
      m_bridgeSeq(sStatics.m_noTag), m_portalSubject(0, 1), m_bridgeSubject(0, 1) {
    reinterpret_cast<s8&>(m_screenHandle) = -1;
    m_param = NULL;
    m_ground = NULL;
    m_sun = NULL;
    m_bridge = NULL;
    m_bridgeShadow = NULL;
    m_portal = NULL;
    m_bridgeCrash = NULL;
    m_bulblin = NULL;
    m_kingBulblin = NULL;
    m_lordBullbo = NULL;
    m_taru = NULL;
    m_kingBulblinHit = NULL;
    m_lordBullboHit = NULL;
    m_bulblinHit = NULL;
    m_taruAttack = NULL;
    m_bridgeAttack = NULL;
    m_bridgeNode[0] = 0;
    m_bridgeNode[1] = 0;
    m_bridgeNode[2] = 0;
    m_portalNode = 0;
    m_noBattle = 0;
    m_lordActive = 0;
    m_kingMtx.setIdentity();
    m_lordMtx.setIdentity();
    m_lordNodeMtx.setIdentity();
    m_lordNode[0] = 0;
    m_lordNode[1] = 0;
    m_lordNode[2] = 0;
    m_lordHitOffset.m_x = 0.0f;
    m_lordHitOffset.m_y = 0.0f;
    m_lordHitOffset.m_z = 0.0f;
    m_lordTimer = 0.0f;
    m_kingTimer = 0.0f;
    m_kingSide = 0;
    m_kingLastSide = 0;
    m_withBulblin = 0;
    m_barrel = 0;
    m_barrelNow = 0;
    m_barrelLast = 0;
    m_kingHitOffset.m_x = 0.0f;
    m_kingHitOffset.m_y = 0.0f;
    m_kingHitOffset.m_z = 0.0f;
    m_lordHits = 0;
    m_bulblinActive = 0;
    m_bulblinLanded = 0;
    m_bulblinSwayTimer = 0.0f;
    m_bulblinMtx.setIdentity();
    m_bulblinNodeMtx.setIdentity();
    m_bulblinNode = 0;
    m_bulblinHp = 0.0f;
    m_bulblinSpeed.m_x = 0.0f;
    m_bulblinSpeed.m_y = 0.0f;
    m_bulblinSpeed.m_z = 0.0f;
    m_bulblinHits = 0;
    m_bulblinStun = 0.0f;
    m_taruActive = 0;
    m_taruTimer = 0.0f;
    m_taruNode = 0;
    m_taruEffect = 0;
    m_bridgeActive = 0;
    m_bridgeTimer = 0.0f;
    m_soundLordStep = 0;
    m_soundLordRoar = 0;
    m_soundLordTimer = 0.0f;
    m_soundLordRumble = 0;
    m_soundLordKing = 0;
    m_soundTaruThrow = 0;
    m_soundTaruDrop = 0;
    m_taruThrown = 0;
    m_soundBridgeSlide = 0;
}

stOldin::~stOldin() {
    releaseArchive();
}

bool stOldin::loading() {
    return true;
}

void stOldin::createObj() {
    testStageParamInit(m_fileData, 10);
    testStageDataInit(m_fileData, 0x14, 0x34);
    m_param = &sStatics.m_param;

    m_ground = grOldin::create(0, "", "grOldinGround");
    if (m_ground != NULL) {
        addGround(m_ground);
        m_ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        m_ground->setStageData(m_stageData);
        m_ground->initializeEntity();
        m_ground->startEntityAutoLoop();
    }

    m_sun = grOldin::create(0xB, "", "grOldinGroundSun");
    if (m_sun != NULL) {
        addGround(m_sun);
        m_sun->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        m_sun->setStageData(m_stageData);
        m_sun->initializeEntity();
        m_sun->startEntityAutoLoop();
    }

    m_bridge = grOldin::create(1, "", "grOldinBridge");
    if (m_bridge != NULL) {
        addGround(m_bridge);
        m_bridge->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        m_bridge->setStageData(m_stageData);
        m_bridge->initializeEntity();
        m_bridgeNode[0] = m_bridge->getNodeIndex(0, "StgOldinClash");
        m_bridgeNode[1] = m_bridge->getNodeIndex(0, "OldinClash06");
        m_bridgeNode[2] = m_bridge->getNodeIndex(0, "hashi_koware");
        m_bridgeAttack = grOldin::create(7, "", "grBridgeAttack");
        if (m_bridgeAttack != NULL) {
            addGround(m_bridgeAttack);
            m_bridgeAttack->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
            m_bridgeAttack->setStageData(m_stageData);
            setBridgeAttack();
            m_bridgeAttack->initializeEntity();
            Vec3f attackPos(-100.0f, -22.0f, 0.0f);
            m_bridgeAttack->setPos(&attackPos);
        }
    }

    m_portal = grOldin::create(9, "", "grOldinPrtal");
    if (m_portal != NULL) {
        addGround(m_portal);
        m_portal->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        m_portal->setStageData(m_stageData);
        m_portal->initializeEntity();
        m_portalNode = m_portal->getNodeIndex(0, "ef_warphole");
    }

    m_bridgeShadow = grOldin::create(0xA, "", "grOldinBridgeKage");
    if (m_bridgeShadow != NULL) {
        addGround(m_bridgeShadow);
        m_bridgeShadow->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        m_bridgeShadow->setStageData(m_stageData);
        m_bridgeShadow->initializeEntity();
        m_bridgeShadow->startEntityAutoLoop();
    }

    m_bridgeCrash = grOldin::create(8, "", "grOldinBridgeCrash");
    if (m_bridgeCrash != NULL) {
        addGround(m_bridgeCrash);
        m_bridgeCrash->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        m_bridgeCrash->setStageData(m_stageData);
        m_bridgeCrash->initializeEntity();
    }

    m_bulblin = grOldin::create(2, "", "grOldinBulblin");
    if (m_bulblin != NULL) {
        addGround(m_bulblin);
        m_bulblin->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        m_bulblin->setStageData(m_stageData);
        fn_27_279FA4(m_bulblin);
        m_bulblin->setVisibility(0);
        m_bulblin->setEnableCollisionStatus(false);
        m_bulblinNode = m_bulblin->getNodeIndex(0, "StgOldinBulblin_TransN");
        m_bulblinHit = grOldin::create(7, "", "grOldinKingBulblinHit");
        if (m_bulblinHit != NULL) {
            addGround(m_bulblinHit);
            m_bulblinHit->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
            m_bulblinHit->setStageData(m_stageData);
            Vec3f hitStart(0.0f, 0.0f, 0.0f);
            Vec3f hitEnd(4.0f, 8.0f, 0.0f);
            m_bulblinHit->setHitPoint(5.0f, &hitStart, &hitEnd, true, 0);
            m_bulblinHit->initializeEntity();
            fn_27_279FA4(m_bulblinHit);
        }
    }

    m_kingBulblin = grOldin::create(4, "", "grOldinKingBulblin");
    if (m_kingBulblin != NULL) {
        addGround(m_kingBulblin);
        m_kingBulblin->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        m_kingBulblin->setStageData(m_stageData);
        m_kingBulblin->startEntityAutoLoop();
        fn_27_279FA4(m_kingBulblin);
        m_kingBulblin->setVisibility(0);
        m_kingBulblin->setEnableCollisionStatus(false);
        m_kingBulblin->setMotionRatio(0.8f);
        m_kingBulblinHit = grOldin::create(7, "", "grOldinKingBulblinHit");
        if (m_kingBulblinHit != NULL) {
            addGround(m_kingBulblinHit);
            m_kingBulblinHit->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
            m_kingBulblinHit->setStageData(m_stageData);
            m_kingHitOffset.m_x = -2.0f;
            m_kingHitOffset.m_y = 14.0f;
            m_kingHitOffset.m_z = 0.0f;
            Vec3f hitStart(0.0f, 0.0f, 0.0f);
            Vec3f hitEnd(2.5f, 11.0f, 0.0f);
            m_kingBulblinHit->setHitPoint(6.5f, &hitStart, &hitEnd, true, 0);
            m_kingBulblinHit->initializeEntity();
            m_kingBulblinHit->startEntity();
            fn_27_279FA4(m_kingBulblinHit);
        }
    }

    m_lordBullbo = grOldin::create(5, "", "grOldinLordBullbo");
    if (m_lordBullbo != NULL) {
        addGround(m_lordBullbo);
        m_lordBullbo->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        m_lordBullbo->setStageData(m_stageData);
        m_lordNode[0] = m_lordBullbo->getNodeIndex(0, "StgOldinLordBullbo_TransN");
        m_lordNode[1] = m_lordBullbo->getNodeIndex(0, "StgOldinLordBullbo_center");
        m_lordNode[2] = m_lordBullbo->getNodeIndex(0, "StgOldinLordBullbo_center");
        fn_27_279FA4(m_lordBullbo);
        m_lordBullbo->setVisibility(0);
        m_lordBullbo->setEnableCollisionStatus(false);
        m_lordBullbo->setMotionRatio(0.8f);
        m_lordBullboHit = grOldin::create(7, "", "grOldinLordBullboHit");
        if (m_lordBullboHit != NULL) {
            addGround(m_lordBullboHit);
            m_lordBullboHit->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
            m_lordBullboHit->setStageData(m_stageData);
            Vec3f hitStart(6.0f, 1.0f, 0.0f);
            Vec3f hitEnd(-14.0f, 1.0f, 0.0f);
            m_lordBullboHit->setHitPoint(11.0f, &hitStart, &hitEnd, true, 0);
            m_lordHitOffset.m_x = 14.0f;
            m_lordHitOffset.m_y = 10.0f;
            m_lordHitOffset.m_z = 0.0f;
            Vec3f attackOffset(0.0f, 0.0f, 0.0f);
            setLordBullboAttack(&attackOffset);
            m_lordBullboHit->initializeEntity();
            m_lordBullboHit->startEntity();
            fn_27_279FA4(m_lordBullboHit);
        }
    }

    m_taru = grOldin::create(6, "", "grOldinTaru");
    if (m_taru != NULL) {
        addGround(m_taru);
        m_taru->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        m_taru->setStageData(m_stageData);
        fn_27_279FA4(m_taru);
        m_taru->setVisibility(0);
        m_taru->setEnableCollisionStatus(false);
        m_taruNode = m_taru->getNodeIndex(0, "StgOldinTaru_TaruM");
        m_taruAttack = grOldin::create(7, "", "grOldinTaruAttack");
        if (m_taruAttack != NULL) {
            addGround(m_taruAttack);
            m_taruAttack->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
            m_taruAttack->setStageData(m_stageData);
            Vec3f attackOffset(0.0f, 0.0f, 0.0f);
            setTaruAttack(&attackOffset);
            m_taruAttack->initializeEntity();
            m_taruAttack->startEntity();
            fn_27_279FA4(m_taruAttack);
        }
    }

    createCollision(m_fileData, 2, NULL);
    setLRHang(false);
    initCameraParam();
    void* posData = m_fileData->getData(Data_Type_Model, 100, 0xFFFE);
    if (posData) {
        nw4r::g3d::ResFile posFile(posData);
        createStagePositions(&posFile);
    } else {
        createStagePositions();
    }
    createWind2ndOnly();
    registScnAnim(static_cast<nw4r::g3d::ResFileData*>(m_fileData->getData(Data_Type_Scene, 0, 0xFFFE)), 0);
    m_lordSubject.clear();
    m_taruSubject.clear();
    m_portalSubject.clear();
    m_bridgeSubject.clear();

    m_kingSeq.m_tag = sStatics.m_noTag;
    m_kingSeq.m_line = 0;
    m_kingSide = 1;
    m_kingLastSide = 0;
    m_lordActive = 0;
    m_lordBullbo->setVisibility(0);
    m_lordBullbo->setEnableCollisionStatus(false);
    OLDIN_PARK(m_lordBullbo);
    OLDIN_PARK(m_lordBullboHit);
    m_lordBullboHit->endEntity();
    m_kingBulblin->setVisibility(0);
    m_kingBulblin->setEnableCollisionStatus(false);
    OLDIN_PARK(m_kingBulblin);
    OLDIN_PARK(m_kingBulblinHit);
    m_kingBulblinHit->endEntity();
    m_lordSubject.m_state = 1;
    m_bulblinActive = 0;
    m_bulblinLanded = 0;
    m_bulblin->setVisibility(0);
    m_bulblin->setEnableCollisionStatus(false);
    OLDIN_PARK(m_bulblin);
    OLDIN_PARK(m_bulblinHit);
    m_bulblinHit->endEntity();
    m_bulblinHp = m_param->m_bulblinHp;
    m_bulblinHits = 0;
    m_taruActive = 0;
    m_taru->setVisibility(0);
    m_taru->setEnableCollisionStatus(false);
    OLDIN_PARK(m_taru);
    OLDIN_PARK(m_taruAttack);
    m_taruSubject.m_state = 1;
    m_bridgeActive = 0;
    m_bridge->startEntity();
    m_bridge->setEnableCollisionStatus(true);
    {
        gfModelAnimation* anim = m_bridge->m_modelAnims[0];
        if (anim != NULL) {
            anim->setFrame((float)anim->getFrameCount());
        }
    }
    m_bridge->setMotionRatio(0.0f);
    m_bridgeShadow->setVisibility(1);
    Matrix identity;
    fn_27_279FC8(m_bridge, &identity, true);
    fn_27_279FC8(m_bridgeCrash, &identity, true);
    m_portalSubject.m_state = 1;
    m_bridgeSubject.m_state = 1;
    loadStageAttrParam(m_fileData, 30);
    initPosPokeTrainer(2, 0);
    createObjPokeTrainer(m_fileData, 0x65, "PokeTrainer00", m_pokeTrainerPos, NULL);
    createObjPokeTrainer(m_fileData, 0x66, "PokeTrainer01", m_pokeTrainerPos + 2, NULL);
    m_barrelNow = 0;
    m_barrelLast = 0;
    stOldinMeleeView* melee = reinterpret_cast<stOldinMeleeView*>(g_GameGlobal->m_modeMelee);
    m_noBattle = (melee->mode == 7 && (melee->stage & 0x7F) == 0x18);
}

void stOldin::update(float deltaFrame) {
    g_aiMgr->clearDangerZone(0);
    KingBlublinUpdate(deltaFrame);
    LordBullboUpdate(deltaFrame);
    BulblinUpdate(deltaFrame);
    TaruUpdate(deltaFrame);
    BridgeUpdate(deltaFrame);
}

// Lord Bullbo rides in with King Bulblin on his back; `full` is false when the ride is only restarted.
void stOldin::LordBullboStart(Matrix* matrix, bool full) {
    if (full) {
        m_lordActive = 1;
        m_lordSeq.m_tag = sStatics.m_noTag;
        m_lordSeq.m_line = 0;
        m_lordBullboHit->startEntity();
        m_kingBulblinHit->startEntity();
        m_lordTimer = 0.0f;
        m_soundLordTimer = 180.0f + (20.0f - 40.0f * randf());
        m_soundLordRoar = 0;
        m_soundLordRumble = playSeBasic(snd_se_stage_Oldin_rumble, 0.0f);
    }
    m_lordNodeMtx = m_lordMtx = *matrix;
    m_lordBullbo->setVisibility(1);
    fn_27_279FC8(m_lordBullbo, &m_lordMtx, false);
    gfModelAnimation* anim = m_lordBullbo->m_modelAnims[0];
    if (anim != NULL) {
        anim->setFrame(0.0f);
        m_soundLordStep = 0;
    }
    Matrix lordHitMtx = m_lordMtx;
    fn_8003F074(&lordHitMtx, m_lordHitOffset.m_x, m_lordHitOffset.m_y, m_lordHitOffset.m_z);
    fn_27_279FC8(m_lordBullboHit, &lordHitMtx, false);
    m_kingBulblin->setVisibility(1);
    fn_27_279FC8(m_kingBulblin, &m_lordMtx, false);
    Matrix kingHitMtx = m_lordMtx;
    fn_8003F074(&kingHitMtx, m_kingHitOffset.m_x, m_kingHitOffset.m_y, m_kingHitOffset.m_z);
    fn_27_279FC8(m_kingBulblinHit, &kingHitMtx, false);
    if (full) {
        m_lordHits = 0;
        m_kingBulblin->setMotion(0);
        m_kingBulblin->startEntityAutoLoop();
        OLDIN_SET_FRAME(m_kingBulblin, 0.0f);
    }
}

// Lord Bullbo gallops over the bridge: his roars, steps and the quake of every step, the hit boxes that follow him,
// the barrel King Bulblin throws and the moment King Bulblin is knocked off.
void stOldin::LordBullboUpdate(float deltaFrame) {
    if (m_lordActive) {
        m_soundLordTimer -= deltaFrame;
        if (m_soundLordTimer <= 0.0f) {
            m_soundLordTimer = 180.0f + (20.0f - 40.0f * randf());
            m_soundLordRoar = 0;
            int index = randi(3);
            if ((u32)index >= 2) {
                index = 2;
            }
            m_soundLordRoar = m_soundLord.playSE((SndID)(snd_se_stage_Oldin_bruboo_01 + index), -1, 0, -1);
            OSReport("SND_SE_STAGE_OLDIN_BRUBOO_01\n");
        }

        switch (m_lordSeq.m_line) {
            case 0:
                OLDIN_SAVE(m_lordSeq, 0x2E0, 1);
                // FALL-THROUGH
            case 0x2E0:
                OLDIN_SAVE(m_lordSeq, 0x2E1, 1);
                return;
            case 0x2E1:
            case 0x38D: {
                m_lordBullbo->getNodeMatrix(&m_lordNodeMtx, 0, m_lordNode[0]);
                Vec3f pos(m_lordNodeMtx.m[0][3], m_lordNodeMtx.m[1][3], m_lordNodeMtx.m[2][3]);
                m_soundLord.setPos(&pos);
                m_lordSubject.setPos(&pos);
                if (-320.0f <= pos.m_x && pos.m_x <= 320.0f) {
                    m_lordSubject.m_state = 0;
                    m_lordSubject.m_stateB = 0;
                } else {
                    m_lordSubject.m_state = 1;
                }

                // The part of the bridge in front of and behind Lord Bullbo is dangerous for the CPU fighters.
                Vec2f lowCorner(pos.m_x, pos.m_y);
                Vec2f highCorner(pos.m_x, pos.m_y);
                Vec2f lowSize(m_kingSide ? 150.0f : 10.0f, 20.0f);
                lowCorner.m_x -= lowSize.m_x;
                lowCorner.m_y -= lowSize.m_y;
                Vec2f highSize(m_kingSide ? 10.0f : 150.0f, 100.0f);
                highCorner.m_x += highSize.m_x;
                highCorner.m_y += highSize.m_y;
                g_aiMgr->setDangerZone(&lowCorner, &highCorner, -1, false, false);

                Matrix lordHitMtx = m_lordNodeMtx;
                fn_8003F074(&lordHitMtx, m_lordHitOffset.m_x, m_lordHitOffset.m_y, m_lordHitOffset.m_z);
                fn_27_279FC8(m_lordBullboHit, &lordHitMtx, false);

                // King Bulblin throws a barrel when he is about to pass the middle of the stage.
                if (m_barrel == 1 && m_taruActive == 0) {
                    Vec3f barrelPos(m_lordNodeMtx.m[0][3], m_lordNodeMtx.m[1][3], m_lordNodeMtx.m[2][3]);
                    if ((m_kingSide == 1 && -140.0f <= barrelPos.m_x && barrelPos.m_x <= -40.0f) ||
                        (m_kingSide == 0 && 40.0f <= barrelPos.m_x && barrelPos.m_x <= 140.0f)) {
                        m_taruActive = 1;
                        m_taruSeq.m_tag = sStatics.m_noTag;
                        m_taruSeq.m_line = 0;
                        Matrix taruMtx = m_lordNodeMtx;
                        Vec3f taruPos(taruMtx.m[0][3], 0.0f, taruMtx.m[2][3]);
                        taruMtx.m[0][3] = taruPos.m_x;
                        taruMtx.m[1][3] = taruPos.m_y;
                        taruMtx.m[2][3] = taruPos.m_z;
                        fn_27_279FC8(m_taru, &taruMtx, false);
                        OLDIN_SET_FRAME(m_taru, 0.0f);
                        m_barrel = 0;
                    }
                }

                fn_27_279FC8(m_kingBulblin, &m_lordNodeMtx, false);
                Matrix kingHitMtx = m_lordNodeMtx;
                fn_8003F074(&kingHitMtx, m_kingHitOffset.m_x, m_kingHitOffset.m_y, m_kingHitOffset.m_z);
                fn_27_279FC8(m_kingBulblinHit, &kingHitMtx, false);

                if (m_kingBulblinHit->isHit() || m_lordBullboHit->isHit()) {
                    // A fighter hit King Bulblin: he falls off Lord Bullbo.
                    m_kingBulblinHit->clearHit();
                    m_lordBullboHit->clearHit();
                    m_lordHits = 1;
                    m_kingBulblin->setMotion(1);
                    OLDIN_SET_FRAME(m_kingBulblin, 0.0f);
                    OLDIN_PARK(m_kingBulblinHit);
                    m_kingBulblinHit->endEntity();
                    m_lordBullboHit->endEntity();
                    m_lordTimer = 120.0f;
                    int kingIndex = randi(2);
                    if ((u32)kingIndex >= 1) {
                        kingIndex = 1;
                    }
                    m_soundLordKing = m_soundLord.playSE((SndID)(snd_se_stage_Oldin_king_01 + kingIndex), -1, 0, -1);
                } else {
                    gfModelAnimation* anim = m_kingBulblin->m_modelAnims[0];
                    if (anim != NULL) {
                        float last = (float)anim->getFrameCount();
                        if (anim->getFrame() >= last) {
                            if ((u32)(++m_lordHits) > 1) {
                                m_lordHits = 0;
                                m_kingBulblin->setMotion(0);
                                m_kingBulblin->startEntityAutoLoop();
                                OLDIN_SET_FRAME(m_kingBulblin, 0.0f);
                            } else {
                                anim->setFrame(0.0f);
                            }
                        }
                    }
                }

                if (0.0f < m_lordTimer) {
                    m_lordTimer -= deltaFrame;
                    if (m_lordTimer <= 0.0f) {
                        m_lordTimer = 0.0f;
                        m_kingBulblinHit->startEntity();
                        m_lordBullboHit->startEntity();
                    }
                }

                Vec3f quakePos(m_lordNodeMtx.m[0][3], m_lordNodeMtx.m[1][3], m_lordNodeMtx.m[2][3]);
                if (__fabsf(quakePos.m_x) < 455.0f) {
                    gfModelAnimation* anim = m_lordBullbo->m_modelAnims[0];
                    if (anim != NULL) {
                        float frame = anim->getFrame();
                        float end = (float)anim->getFrameCount() - 1.0f;
                        if (frame >= end) {
                            LordBullboStart(&m_lordNodeMtx, false);
                        } else {
                            // HYPOTHESIS: the frames of the animation Lord Bullbo's hoofs touch the ground at.
                            static float sStepFrames[5] = {18.0f, 22.0f, 24.0f, 30.0f, 10000.0f};
                            if (sStepFrames[m_soundLordStep] <= frame) {
                                m_soundLord.playSE((SndID)(snd_se_stage_Oldin_step_01 + m_soundLordStep), -1, 0, -1);
                                m_soundLordStep++;
                                cmReqQuake(cmQuake::Amplitude_Small, &quakePos);
                            }
                        }
                    }
                    OLDIN_SAVE(m_lordSeq, 0x38D, 1);
                } else {
                    // Lord Bullbo left the stage.
                    m_lordActive = 0;
                    m_lordBullbo->setVisibility(0);
                    m_lordBullbo->setEnableCollisionStatus(false);
                    OLDIN_PARK(m_lordBullbo);
                    OLDIN_PARK(m_lordBullboHit);
                    m_lordBullboHit->endEntity();
                    m_kingBulblin->setVisibility(0);
                    m_kingBulblin->setEnableCollisionStatus(false);
                    OLDIN_PARK(m_kingBulblin);
                    OLDIN_PARK(m_kingBulblinHit);
                    m_kingBulblinHit->endEntity();
                    m_lordSubject.m_state = 1;
                    stopSeBasic(m_soundLordRumble, 0.5f);
                    OLDIN_SAVE(m_lordSeq, 0x398, 2);
                }
                break;
            }
        }
    }
}

// King Bulblin decides when he rides over the bridge, from which side, and whether a Bulblin comes along.
void stOldin::KingBlublinUpdate(float deltaFrame) {
    if (!m_noBattle) {
        switch (m_kingSeq.m_line) {
            case 0:
                OLDIN_SAVE(m_kingSeq, 0x3AB, 1);
                // FALL-THROUGH
            case 0x3AB:
                m_kingTimer = m_param->m_kingStartWait;
            wait:
                if (m_param->m_kingChance < 1.0f * randf()) {
                    goto bridge;
                }
                // FALL-THROUGH
            case 0x3BF:
                while (0.0f < m_kingTimer) {
                    m_kingTimer -= deltaFrame;
                    OLDIN_SAVE(m_kingSeq, 0x3BF, 1);
                    return;
                }
                {
                    float range = m_param->m_kingWaitMax - m_param->m_kingWaitMin;
                    m_kingTimer = m_param->m_kingWaitMin + range * randf();
                }
                goto wait;
            bridge:
                // FALL-THROUGH
            case 0x3CB:
                while (m_bridgeActive) {
                    OLDIN_SAVE(m_kingSeq, 0x3CB, 1);
                    return;
                }
                // FALL-THROUGH
            case 0x3D5:
                while (m_param->m_kingHornTime < m_kingTimer) {
                    m_kingTimer -= deltaFrame;
                    OLDIN_SAVE(m_kingSeq, 0x3D5, 1);
                    return;
                }
                if (m_kingSide == m_kingLastSide) {
                    m_kingSide = (m_kingSide == 0);
                } else {
                    m_kingLastSide = m_kingSide;
                    m_kingSide = (1.0f * randf() <= 0.5f);
                }
                playSeBasic(snd_se_stage_Oldin_horn, m_kingSide ? 0.75f : -0.75f);
                // FALL-THROUGH
            case 0x3ED:
                while (0.0f < m_kingTimer) {
                    m_kingTimer -= deltaFrame;
                    OLDIN_SAVE(m_kingSeq, 0x3ED, 1);
                    return;
                }
                {
                    float total = m_param->m_weight[0] + m_param->m_weight[1] + m_param->m_weight[2] + m_param->m_weight[3];
                    float pick = total * randf();
                    if (pick <= m_param->m_weight[0]) {
                        m_withBulblin = 0;
                    } else {
                        pick -= m_param->m_weight[0];
                        if (pick <= m_param->m_weight[1]) {
                            m_withBulblin = 1;
                        } else {
                            pick -= m_param->m_weight[1];
                            if (pick <= m_param->m_weight[2]) {
                                m_withBulblin = 0;
                            } else {
                                m_withBulblin = 1;
                            }
                        }
                    }
                }
                if (m_barrelNow == 0 && m_barrelLast == 0) {
                    m_barrelNow = 1;
                    m_barrel = 1;
                } else {
                    m_barrelLast = m_barrelNow;
                    m_barrelNow = (100.0f * randf() <= m_param->m_barrelPercent);
                    m_barrel = m_barrelNow;
                }
                m_kingMtx.setIdentity();
                if (m_kingSide) {
                    fn_8003E918(&m_kingMtx, 3.1415927f);
                    m_lordBullboHit->setYakumonoLr(-1.0f);
                } else {
                    m_lordBullboHit->setYakumonoLr(1.0f);
                }
                {
                    Vec3f start(-450.0f, 0.0f, 0.0f);
                    fn_8003F074(&m_kingMtx, start.m_x, start.m_y, start.m_z);
                }
                if (m_withBulblin) {
                    // A Bulblin follows the ride: it starts where King Bulblin does.
                    m_bulblinMtx = m_kingMtx;
                    m_bulblinNodeMtx = m_kingMtx;
                    m_bulblinActive = 1;
                    m_bulblinSeq.m_tag = sStatics.m_noTag;
                    m_bulblinSeq.m_line = 0;
                    m_bulblin->startEntity();
                    m_bulblin->setVisibility(1);
                    m_bulblinHit->startEntity();
                    fn_27_279FC8(m_bulblin, &m_bulblinMtx, false);
                    m_bulblin->setMotion(0);
                    OLDIN_SET_FRAME(m_bulblin, 0.0f);
                    fn_27_279FC8(m_bulblinHit, &m_bulblinMtx, false);
                }
                m_bulblinSwayTimer = 60.0f;
                // FALL-THROUGH
            case 0x474:
                while (0.0f < m_bulblinSwayTimer) {
                    m_bulblinSwayTimer -= deltaFrame;
                    OLDIN_SAVE(m_kingSeq, 0x474, 1);
                    return;
                }
                LordBullboStart(&m_kingMtx, true);
                OLDIN_SAVE(m_kingSeq, 0x479, 1);
                return;
            case 0x479:
            case 0x489:
                while (m_lordActive || (m_withBulblin && m_bulblinActive)) {
                    OLDIN_SAVE(m_kingSeq, 0x489, 1);
                    return;
                }
                {
                    float range = m_param->m_kingWaitMax - m_param->m_kingWaitMin;
                    m_kingTimer = m_param->m_kingWaitMin + range * randf();
                }
                goto wait;
        }
    }
}

// A Bulblin walks over the bridge behind King Bulblin; it can be hit and thrown off.
void stOldin::BulblinUpdate(float deltaFrame) {
    if (m_bulblinActive) {
        m_bulblin->getNodeMatrix(&m_bulblinNodeMtx, 0, m_bulblinNode);
        Vec3f soundPos(m_bulblinNodeMtx.m[0][3], m_bulblinNodeMtx.m[1][3], m_bulblinNodeMtx.m[2][3]);
        m_soundBulblin.setPos(&soundPos);
        switch (m_bulblinSeq.m_line) {
            case 0:
                OLDIN_SAVE(m_bulblinSeq, 0x4DA, 1);
                // FALL-THROUGH
            case 0x4DA:
                OLDIN_SAVE(m_bulblinSeq, 0x4DB, 1);
                return;
            case 0x4DB:
                OLDIN_SAVE(m_bulblinSeq, 0x4DC, 1);
                // FALL-THROUGH
            case 0x4DC:
            case 0x52A:
            walk:
                if (m_bulblinHit->isHit()) {
                    m_bulblinHit->clearHit();
                    m_soundBulblin.playSE(snd_se_stage_Oldin_brublin_damage, -1, 0, -1);
                    goto hurt;
                } else {
                    Vec3f probe(m_bulblinNodeMtx.m[0][3], m_bulblinNodeMtx.m[1][3], m_bulblinNodeMtx.m[2][3]);
                    if (-100.0f <= probe.m_y && -208.0 <= probe.m_x && probe.m_x <= 200.0f) {
                        probe.m_z = 0.0f;
                        probe.m_y = probe.m_y + 2.0f;
                        Vec3f direction(0.0f, -10.0f, 0.0f);
                        Vec3f hitPos;
                        Vec3f hitNormal;
                        if (!stRayCheck(&probe, &direction, &hitPos, &hitNormal, false, NULL, false, 1)) {
                            if (m_bulblinLanded) {
                                if ((m_kingSide == 1 && 0.0f <= probe.m_x) || (m_kingSide == 0 && probe.m_x <= 0.0f)) {
                                    m_bulblinSpeed.m_x = 0.0f;
                                    m_bulblinSpeed.m_y = 1.0f;
                                    m_bulblinSpeed.m_z = 0.0f;
                                    m_soundBulblin.playSE(snd_se_stage_Oldin_brublin_damage, -1, 0, -1);
                                    goto flung;
                                }
                                m_soundBulblin.playSE(snd_se_stage_Oldin_brublin_damage, -1, 0, -1);
                                goto dead;
                            }
                        } else {
                            m_bulblinLanded = 1;
                        }
                    }
                }
                fn_27_279FC8(m_bulblinHit, &m_bulblinNodeMtx, false);
                {
                    Vec3f pos(m_bulblinNodeMtx.m[0][3], m_bulblinNodeMtx.m[1][3], m_bulblinNodeMtx.m[2][3]);
                    if (__fabsf(pos.m_x) < 455.0f) {
                        gfModelAnimation* anim = m_bulblin->m_modelAnims[0];
                        if (anim != NULL) {
                            float last = (float)anim->getFrameCount();
                            if (anim->getFrame() >= last) {
                                m_bulblinMtx = m_bulblinNodeMtx;
                                m_bulblinHit->startEntity();
                                fn_27_279FC8(m_bulblin, &m_bulblinMtx, false);
                                m_bulblin->setMotion(0);
                                OLDIN_SET_FRAME(m_bulblin, 0.0f);
                                fn_27_279FC8(m_bulblinHit, &m_bulblinMtx, false);
                            }
                        }
                        OLDIN_SAVE(m_bulblinSeq, 0x52A, 1);
                        return;
                    }
                }
                goto despawn;
            hurt:
                m_bulblinHp -= m_bulblinHit->getLastDamage();
                m_bulblinHit->endEntity();
                if (m_bulblinHp <= 0.0f) {
                    goto dead;
                }
                m_bulblinStun = 4.0f;
                fn_27_279228(m_bulblin, 1);
                // FALL-THROUGH
            case 0x540:
                while (0.0f < m_bulblinStun) {
                    m_bulblinStun -= deltaFrame;
                    OLDIN_SAVE(m_bulblinSeq, 0x540, 1);
                    return;
                }
                fn_27_279228(m_bulblin, 0);
                fn_27_279FC8(m_bulblin, &m_bulblinNodeMtx, false);
                m_bulblin->setMotion(2);
                OLDIN_SET_FRAME(m_bulblin, 0.0f);
                OLDIN_SAVE(m_bulblinSeq, 0x54F, 1);
                return;
            case 0x54F:
            case 0x55B: {
                gfModelAnimation* anim = m_bulblin->m_modelAnims[0];
                if (anim != NULL) {
                    float last = (float)anim->getFrameCount();
                    if (anim->getFrame() >= last) {
                        m_bulblinMtx = m_bulblinNodeMtx;
                        m_bulblinHit->startEntity();
                        fn_27_279FC8(m_bulblin, &m_bulblinMtx, false);
                        m_bulblin->setMotion(0);
                        OLDIN_SET_FRAME(m_bulblin, 0.0f);
                        fn_27_279FC8(m_bulblinHit, &m_bulblinMtx, false);
                        goto walk;
                    }
                }
                OLDIN_SAVE(m_bulblinSeq, 0x55B, 1);
                return;
            }
            dead:
                m_bulblinHit->endEntity();
                m_bulblinStun = 4.0f;
                fn_27_279228(m_bulblin, 1);
                // FALL-THROUGH
            case 0x569:
                while (0.0f < m_bulblinStun) {
                    m_bulblinStun -= deltaFrame;
                    OLDIN_SAVE(m_bulblinSeq, 0x569, 1);
                    return;
                }
                fn_27_279228(m_bulblin, 0);
                m_bulblinSpeed.m_x = -2.0f;
                m_bulblinSpeed.m_y = -0.2f;
                m_bulblinSpeed.m_z = 2.0f;
                {
                    int index = randi(8);
                    if ((u32)index >= 7) {
                        index = 7;
                    }
                    float angle = (index & 1) ? 1.5707964f : -1.5707964f;
                    angle += 0.7853982f * (1.0f - 2.0f * randf());
                    Matrix rotation;
                    fn_8003E918(&rotation, angle);
                    m_bulblinNodeMtx.mul(&rotation, &m_bulblinNodeMtx);
                }
            flung:
                m_bulblinHit->endEntity();
                fn_27_279FC8(m_bulblin, &m_bulblinNodeMtx, false);
                m_bulblin->setMotion(3);
                OLDIN_SET_FRAME(m_bulblin, 0.0f);
                OLDIN_SAVE(m_bulblinSeq, 0x58E, 1);
                return;
            case 0x58E:
            case 0x5A2: {
                Vec3f step(m_bulblinSpeed.m_x + m_bulblinSpeed.m_y, m_bulblinSpeed.m_z, 0.0f);
                fn_8003F074(&m_bulblinNodeMtx, step.m_x, step.m_y, step.m_z);
                m_bulblinSpeed.m_x = m_bulblinSpeed.m_x * (0.98f * deltaFrame);
                m_bulblinSpeed.m_z = m_bulblinSpeed.m_z - 0.1f * deltaFrame;
                fn_27_279FC8(m_bulblin, &m_bulblinNodeMtx, false);
                gfModelAnimation* anim = m_bulblin->m_modelAnims[0];
                if (anim != NULL) {
                    float last = (float)anim->getFrameCount();
                    if (anim->getFrame() >= last) {
                        goto landed;
                    }
                }
                OLDIN_SAVE(m_bulblinSeq, 0x5A2, 1);
                return;
            landed:
                Vec3f step2(m_bulblinSpeed.m_x + m_bulblinSpeed.m_y, m_bulblinSpeed.m_z, 0.0f);
                fn_8003F074(&m_bulblinNodeMtx, step2.m_x, step2.m_y, step2.m_z);
                m_bulblinSpeed.m_x = m_bulblinSpeed.m_x * (0.98f * deltaFrame);
                m_bulblinSpeed.m_z = m_bulblinSpeed.m_z - 0.1f * deltaFrame;
                fn_27_279FC8(m_bulblin, &m_bulblinNodeMtx, false);
                m_bulblin->setMotion(4);
                OLDIN_SET_FRAME(m_bulblin, 0.0f);
                m_bulblin->startEntityAutoLoop();
                OLDIN_SAVE(m_bulblinSeq, 0x5B8, 1);
                return;
            }
            case 0x5B8:
            case 0x5CD: {
                Vec3f fallPos(m_bulblinNodeMtx.m[0][3], m_bulblinNodeMtx.m[1][3], m_bulblinNodeMtx.m[2][3]);
                if (fallPos.m_y < -150.0f) {
                    goto despawn;
                }
                Vec3f step(m_bulblinSpeed.m_x + m_bulblinSpeed.m_y, m_bulblinSpeed.m_z, 0.0f);
                fn_8003F074(&m_bulblinNodeMtx, step.m_x, step.m_y, step.m_z);
                m_bulblinSpeed.m_x = m_bulblinSpeed.m_x * (0.98f * deltaFrame);
                m_bulblinSpeed.m_z = m_bulblinSpeed.m_z - 0.1f * deltaFrame;
                fn_27_279FC8(m_bulblin, &m_bulblinNodeMtx, false);
                OLDIN_SAVE(m_bulblinSeq, 0x5CD, 1);
                return;
            }
            despawn:
                m_bulblinActive = 0;
                m_bulblinLanded = 0;
                m_bulblin->setVisibility(0);
                m_bulblin->setEnableCollisionStatus(false);
                OLDIN_PARK(m_bulblin);
                OLDIN_PARK(m_bulblinHit);
                m_bulblinHit->endEntity();
                m_bulblinHp = m_param->m_bulblinHp;
                m_bulblinHits = 0;
                OLDIN_SAVE(m_bulblinSeq, 0x5D2, 2);
                break;
        }
    }
}

// The barrel King Bulblin throws: it flies, drops and explodes on the bridge, which starts to break.
void stOldin::TaruUpdate(float deltaFrame) {
    if (m_taruActive) {
        Vec2f dangerHigh(40.0f, 70.0f);
        Vec2f dangerLow(-40.0f, -40.0f);
        g_aiMgr->setDangerZone(&dangerHigh, &dangerLow, -1, false, false);
        switch (m_taruSeq.m_line) {
            case 0:
                OLDIN_SAVE(m_taruSeq, 0x5FE, 1);
                // FALL-THROUGH
            case 0x5FE:
                OLDIN_SAVE(m_taruSeq, 0x5FF, 1);
                return;
            case 0x5FF:
                m_soundTaruThrow = -1;
                m_soundTaruDrop = -1;
                m_taruTimer = 120.0f + 60.0f * randf();
                m_taru->setVisibility(1);
                m_taruThrown = 0;
                m_taruSubject.m_state = 0;
                m_taruSubject.m_stateB = 0;
                // FALL-THROUGH
            case 0x62C: {
                m_taruTimer -= deltaFrame;
                Matrix mtx(true);
                m_taru->getNodeMatrix(&mtx, 0, m_taruNode);
                Vec3f pos(mtx.m[0][3], mtx.m[1][3], mtx.m[2][3]);
                m_taruSubject.setPos(&pos);
                gfModelAnimation* anim = m_taru->m_modelAnims[0];
                if (anim != NULL) {
                    if (!m_taruThrown) {
                        m_taruThrown = 1;
                        m_soundTaru.setPos(&pos);
                        m_soundTaruThrow = m_soundTaru.playSE(snd_se_stage_Oldin_bomb_throw, -1, 0, -1);
                        OSReport("SND_SE_STAGE_OLDIN_BOMB_THROW: %f,%f,%f\n", pos.m_x, pos.m_y, pos.m_z);
                        m_taruEffect = g_ecMgr->setEffect((EfID)0x60);
                    }
                    if (m_soundTaruDrop < 0 && 20.0f <= anim->getFrame()) {
                        m_soundTaru.setPos(&pos);
                        m_soundTaruDrop = m_soundTaru.playSE(snd_se_stage_Oldin_bomb_drop, -1, 0, -1);
                        OSReport("SND_SE_STAGE_OLDIN_BOMB_DROP: %f,%f,%f\n", pos.m_x, pos.m_y, pos.m_z);
                        goto explode;
                    }
                    g_ecMgr->setPos(m_taruEffect, &pos);
                }
                OLDIN_SAVE(m_taruSeq, 0x62C, 1);
                return;
            }
            case 0x63C:
            explode:
                while (0.0f < m_taruTimer) {
                    m_taruTimer -= deltaFrame;
                    Matrix mtx(true);
                    m_taru->getNodeMatrix(&mtx, 0, m_taruNode);
                    Vec3f pos(mtx.m[0][3], mtx.m[1][3], mtx.m[2][3]);
                    m_taruSubject.setPos(&pos);
                    g_ecMgr->setPos(m_taruEffect, &pos);
                    OLDIN_SAVE(m_taruSeq, 0x63C, 1);
                    return;
                }
                {
                    Matrix mtx(true);
                    m_taru->getNodeMatrix(&mtx, 0, m_taruNode);
                    Vec3f pos(mtx.m[0][3], mtx.m[1][3], mtx.m[2][3]);
                    u32 effect = g_ecMgr->setEffect((EfID)0x18);
                    g_ecMgr->setPos(effect, &pos);
                    m_soundTaru.setPos(&pos);
                    m_soundTaru.playSE(snd_se_stage_Oldin_bomb_exp, -1, 0, -1);
                    OSReport("SND_SE_STAGE_OLDIN_BOMB_EXP: %f,%f,%f\n", pos.m_x, pos.m_y, pos.m_z);
                    cmReqQuake(cmQuake::Amplitude_More_Large, &pos);
                    m_taruSubject.setPos(&pos);
                    g_ecMgr->killEffect(m_taruEffect, 1, 1);
                    mtx.m[1][3] = mtx.m[1][3] - 10.0f;
                    fn_27_279FC8(m_taruAttack, &mtx, false);
                    m_taru->setVisibility(0);
                    OLDIN_PARK(m_taru);
                    m_bridgeActive = 1;
                    m_bridgeSeq.m_tag = sStatics.m_noTag;
                    m_bridgeSeq.m_line = 0;
                    playSeBasic(snd_se_stage_Oldin_bridge_crash, 0.0f);
                    m_taruTimer = 8.0f;
                }
                // FALL-THROUGH
            case 0x660:
                while (0.0f < m_taruTimer) {
                    m_taruTimer -= deltaFrame;
                    OLDIN_SAVE(m_taruSeq, 0x660, 1);
                    return;
                }
                m_taruActive = 0;
                m_taru->setVisibility(0);
                m_taru->setEnableCollisionStatus(false);
                OLDIN_PARK(m_taru);
                OLDIN_PARK(m_taruAttack);
                m_taruSubject.m_state = 1;
                OLDIN_SAVE(m_taruSeq, 0x663, 2);
                break;
        }
    }
}

// The bridge breaks, the portal opens, the bridge slides back in and the stage returns to normal.
void stOldin::BridgeUpdate(float deltaFrame) {
    if (m_bridgeActive) {
        switch (m_bridgeSeq.m_line) {
            case 0:
                OLDIN_SAVE(m_bridgeSeq, 0x695, 1);
                // FALL-THROUGH
            case 0x695: {
                Matrix mtx(true);
                m_bridge->getNodeMatrix(&mtx, 0, m_bridgeNode[0]);
                Vec3f pos(mtx.m[0][3], mtx.m[1][3], mtx.m[2][3]);
                g_ecMgr->setEffect(ef_ptc_stg_oldin_hashi_crash_1, &pos);
                m_bridgeSubject.setPos(&pos);
                m_bridgeSubject.m_state = 0;
                m_bridgeSubject.m_stateB = 0;
                setLRHang(true);
                OLDIN_SAVE(m_bridgeSeq, 0x6A8, 1);
                return;
            }
            case 0x6A8:
                OLDIN_SAVE(m_bridgeSeq, 0x6A9, 1);
                return;
            case 0x6A9: {
                Matrix mtx(true);
                m_bridge->getNodeMatrix(&mtx, 0, m_bridgeNode[0]);
                Vec3f pos(mtx.m[0][3] - 40.0f, mtx.m[1][3], mtx.m[2][3]);
                g_ecMgr->setEffect(ef_ptc_stg_oldin_hashi_crash_1, &pos);
                OLDIN_SAVE(m_bridgeSeq, 0x6B3, 1);
                return;
            }
            case 0x6B3:
                OLDIN_SAVE(m_bridgeSeq, 0x6B4, 1);
                return;
            case 0x6B4:
                OLDIN_SAVE(m_bridgeSeq, 0x6B5, 1);
                return;
            case 0x6B5:
                OLDIN_SAVE(m_bridgeSeq, 0x6B6, 1);
                return;
            case 0x6B6: {
                Matrix mtx(true);
                m_bridge->getNodeMatrix(&mtx, 0, m_bridgeNode[0]);
                Vec3f pos(mtx.m[0][3], mtx.m[1][3], mtx.m[2][3]);
                g_ecMgr->setEffect(ef_ptc_stg_oldin_hashi_crash_2, &pos);
                Matrix mtx2(true);
                m_bridge->getNodeMatrix(&mtx2, 0, m_bridgeNode[1]);
                Vec3f pos2(mtx2.m[0][3], mtx2.m[1][3], mtx2.m[2][3]);
                g_ecMgr->setEffect(ef_ptc_stg_oldin_hahenkemuri, &pos2);
                m_bridge->setVisibility(0);
                m_bridge->setEnableCollisionStatus(false);
                m_bridgeShadow->setVisibility(0);
                m_bridgeCrash->startEntity();
                m_bridgeTimer = 120.0f;
            }
                // FALL-THROUGH
            case 0x6D2:
                while (0.0f < m_bridgeTimer) {
                    m_bridgeTimer -= deltaFrame;
                    OLDIN_SAVE(m_bridgeSeq, 0x6D2, 1);
                    return;
                }
                m_bridgeSubject.m_state = 1;
                {
                    float range = m_param->m_bridgeWaitMax - m_param->m_bridgeWaitMin;
                    m_bridgeTimer = m_param->m_bridgeWaitMin + range * randf();
                }
                // FALL-THROUGH
            case 0x6DD:
                while (0.0f < m_bridgeTimer) {
                    m_bridgeTimer -= deltaFrame;
                    OLDIN_SAVE(m_bridgeSeq, 0x6DD, 1);
                    return;
                }
                m_bridgeCrash->endEntity();
                m_portal->startEntity();
                playSeBasic(snd_se_stage_Oldin_bridge_revival, 0.0f);
                OLDIN_SAVE(m_bridgeSeq, 0x6E6, 1);
                return;
            case 0x6E6: {
                Matrix mtx(true);
                m_portal->getNodeMatrix(&mtx, 0, m_portalNode);
                Vec3f pos(mtx.m[0][3], mtx.m[1][3], mtx.m[2][3]);
                g_ecMgr->setEffect(ef_ptc_stg_oldin_portaltubu, &pos);
                m_portalSubject.setPos(&pos);
                m_portalSubject.m_state = 0;
                m_portalSubject.m_stateB = 0;
                GXColor black;
                black.r = 0;
                black.g = 0;
                black.b = 0;
                black.a = 0x50;
                GXColor fill = black;
                int handle = g_efScreen->requestFill(60.0f, 0, 0x80, &fill);
                reinterpret_cast<u8&>(m_screenHandle) = (u32)handle >> 24;
                m_bridgeTimer = 159.0f;
            }
                // FALL-THROUGH
            case 0x700:
                while (0.0f < m_bridgeTimer) {
                    m_bridgeTimer -= deltaFrame;
                    OLDIN_SAVE(m_bridgeSeq, 0x700, 1);
                    return;
                }
                m_soundBridgeSlide = playSeBasic(snd_se_stage_Oldin_bridge_slide, 0.0f);
                m_bridge->startEntity();
                m_bridge->setMotionRatio(1.0f);
                OLDIN_SAVE(m_bridgeSeq, 0x707, 1);
                return;
            case 0x707: {
                Matrix mtx(true);
                m_bridge->getNodeMatrix(&mtx, 0, m_bridgeNode[2]);
                Vec3f pos(mtx.m[0][3], mtx.m[1][3], mtx.m[2][3]);
                g_ecMgr->setEffect(ef_ptc_stg_oldin_hukkatu, &pos);
                m_portalSubject.m_state = 0;
                m_bridgeTimer = 119.0f;
            }
                // FALL-THROUGH
            case 0x71B:
                while (0.0f < m_bridgeTimer) {
                    m_bridgeTimer -= deltaFrame;
                    OLDIN_SAVE(m_bridgeSeq, 0x71B, 1);
                    return;
                }
                stopSeBasic(m_soundBridgeSlide, 0.0f);
                playSeBasic(snd_se_stage_Oldin_bridge_fit, 0.0f);
                setLRHang(false);
                {
                    Matrix mtx(true);
                    m_bridge->getNodeMatrix(&mtx, 0, m_bridgeNode[2]);
                    Vec3f pos(mtx.m[0][3], mtx.m[1][3], mtx.m[2][3]);
                    g_ecMgr->setEffect(ef_ptc_stg_oldin_hashi_hukkatu_2, &pos);
                }
                m_bridgeShadow->setVisibility(1);
                m_bridge->setEnableCollisionStatus(true);
                m_bridgeAttack->startEntity();
                m_bridgeTimer = 239.0f;
                // FALL-THROUGH
            case 0x735:
                while (180.0f < m_bridgeTimer) {
                    m_bridgeTimer -= deltaFrame;
                    OLDIN_SAVE(m_bridgeSeq, 0x735, 1);
                    return;
                }
                m_bridgeAttack->endEntity();
                // FALL-THROUGH
            case 0x73B:
                while (0.0f < m_bridgeTimer) {
                    m_bridgeTimer -= deltaFrame;
                    OLDIN_SAVE(m_bridgeSeq, 0x73B, 1);
                    return;
                }
                {
                    efScreenHandle handle = m_screenHandle;
                    fn_8005E3C8(g_efScreen, &handle, 60.0f);
                }
                m_bridgeActive = 0;
                m_bridge->startEntity();
                m_bridge->setEnableCollisionStatus(true);
                {
                    gfModelAnimation* anim = m_bridge->m_modelAnims[0];
                    if (anim != NULL) {
                        anim->setFrame((float)anim->getFrameCount());
                    }
                }
                m_bridge->setMotionRatio(0.0f);
                m_bridgeShadow->setVisibility(1);
                {
                    Matrix identity;
                    fn_27_279FC8(m_bridge, &identity, true);
                    fn_27_279FC8(m_bridgeCrash, &identity, true);
                }
                m_portalSubject.m_state = 1;
                m_bridgeSubject.m_state = 1;
                OLDIN_SAVE(m_bridgeSeq, 0x741, 2);
                break;
        }
    }
}

// The hit box of Lord Bullbo.
void stOldin::setLordBullboAttack(Vec3f* offset) {
    m_lordBullboHit->setAttack(2.0f, offset);
    soCollisionAttackData* attack = m_lordBullboHit->getOverwriteAttackData();
    stOldinAttackBits* bits = reinterpret_cast<stOldinAttackBits*>(reinterpret_cast<u8*>(attack) + 0x30);
    bits->m_nodeIndex = 0;
    attack->m_power = 15;
    bits->m_attribute = 2;
    bits->m_targetSituation = 7;
    bits->m_targetCategory = 0x3FF;
    bits->m_targetLr = 0;
    bits->m_targetPart = 0xF;
    attack->m_vector = 35;
    attack->m_reactionEffect = 40;
    attack->m_reactionFix = 0;
    attack->m_reactionAdd = 80;
    attack->m_size = 6.0f;
    attack->m_offsetPos.m_x = offset->m_x;
    attack->m_offsetPos.m_y = offset->m_y;
    attack->m_offsetPos.m_z = offset->m_z;
    bits->m_setOffKind = 0;
    bits->m_soundLevel = 2;
    bits->m_soundAttribute = 2;
    bits->m_noScale = 0;
    bits->m_isShieldable = 1;
    bits->m_isReflectable = 0;
    bits->m_isAbsorbable = 0;
    bits->m_isDirect = 0;
    bits->m_serialHitFrame = 90;
    bits->m_isInvalidInvincible = 0;
    bits->m_isInvalidXlu = 0;
    bits->m_lrCheck = 3;
    bits->m_isCatch = 0;
    bits->m_noTeam = 0;
    bits->m_noHitStop = 0;
    bits->m_noEffect = 0;
    bits->m_noTransaction = 0;
    bits->m_shapeType = 0;
    m_lordBullboHit->setAttackPreset(grMadein::Attack_Overwrite);
}

// The hit box of the barrel.
void stOldin::setTaruAttack(Vec3f* offset) {
    m_taruAttack->setAttack(40.0f, offset);
    soCollisionAttackData* attack = m_taruAttack->getOverwriteAttackData();
    stOldinAttackBits* bits = reinterpret_cast<stOldinAttackBits*>(reinterpret_cast<u8*>(attack) + 0x30);
    bits->m_nodeIndex = 0;
    attack->m_power = 30;
    bits->m_attribute = 0;
    bits->m_targetSituation = 7;
    bits->m_targetCategory = 0x3FF;
    bits->m_targetLr = 0;
    bits->m_targetPart = 0xF;
    attack->m_vector = 362;
    attack->m_reactionEffect = 50;
    attack->m_reactionFix = 0;
    attack->m_reactionAdd = 80;
    attack->m_size = 40.0f;
    attack->m_offsetPos.m_x = offset->m_x;
    attack->m_offsetPos.m_y = offset->m_y;
    attack->m_offsetPos.m_z = offset->m_z;
    bits->m_setOffKind = 0;
    bits->m_soundLevel = 0;
    bits->m_soundAttribute = 0;
    bits->m_noScale = 0;
    bits->m_isShieldable = 1;
    bits->m_isReflectable = 0;
    bits->m_isAbsorbable = 0;
    bits->m_isDirect = 0;
    bits->m_serialHitFrame = 60;
    bits->m_isInvalidInvincible = 0;
    bits->m_isInvalidXlu = 0;
    bits->m_lrCheck = 0;
    bits->m_isCatch = 0;
    bits->m_noTeam = 0;
    bits->m_noHitStop = 0;
    bits->m_noEffect = 0;
    bits->m_noTransaction = 0;
    bits->m_shapeType = 1;
    m_taruAttack->setAttackPreset(grMadein::Attack_Overwrite);
}

// The hit box the bridge has while it slides back in.
void stOldin::setBridgeAttack() {
    Vec3f offset(200.0f, 0.0f, 0.0f);
    m_bridgeAttack->setAttack(40.0f, &offset);
    soCollisionAttackData* attack = m_bridgeAttack->getOverwriteAttackData();
    stOldinAttackBits* bits = reinterpret_cast<stOldinAttackBits*>(reinterpret_cast<u8*>(attack) + 0x30);
    bits->m_nodeIndex = 0;
    attack->m_power = 0;
    bits->m_attribute = 0;
    bits->m_targetSituation = 7;
    bits->m_targetCategory = 0x3FF;
    bits->m_targetLr = 0;
    bits->m_targetPart = 0xF;
    attack->m_vector = 90;
    attack->m_reactionEffect = 50;
    attack->m_reactionFix = 100;
    attack->m_reactionAdd = 80;
    attack->m_size = 18.0f;
    attack->m_offsetPos.m_x = offset.m_x;
    attack->m_offsetPos.m_y = offset.m_y;
    attack->m_offsetPos.m_z = offset.m_z;
    bits->m_setOffKind = 0;
    bits->m_soundLevel = 0;
    bits->m_soundAttribute = 0;
    bits->m_noScale = 0;
    bits->m_isShieldable = 0;
    bits->m_isReflectable = 0;
    bits->m_isAbsorbable = 0;
    bits->m_isDirect = 0;
    bits->m_serialHitFrame = 60;
    bits->m_isInvalidInvincible = 1;
    bits->m_isInvalidXlu = 1;
    bits->m_lrCheck = 0;
    bits->m_isCatch = 0;
    bits->m_noTeam = 0;
    bits->m_noHitStop = 0;
    bits->m_noEffect = 0;
    bits->m_noTransaction = 0;
    bits->m_shapeType = 1;
    m_bridgeAttack->setAttackPreset(grMadein::Attack_Overwrite);
}

// While the bridge is broken the ledges of its collision cannot be grabbed (`hang` false: they can).
void stOldin::setLRHang(bool hang) {
    if (m_bridge != NULL) {
        grCollision* collision = m_bridge->m_collision;
        if (collision != NULL) {
            u32 jointLen = (u16)collision->m_jointLen;
            for (u32 i = 0; i != jointLen; i++) {
                grCollisionJoint* joint = collision->getJoint(i);
                if (joint != NULL) {
                    if (hang == 1) {
                        joint->m_0x52 = 0;
                    } else {
                        joint->m_0x52 = 0x6000;
                    }
                }
            }
        }
    }
}

grOldin* grOldin::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grOldin* ground = new (Heaps::StageInstance) grOldin(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->m_heapType = Heaps::StageInstance;
        ground->makeCalcuCallback(1, Heaps::StageInstance);
        ground->setCalcuCallbackRoot(7);
    }
    return ground;
}

grOldin::~grOldin() {
}
