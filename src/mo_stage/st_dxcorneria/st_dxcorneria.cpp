#include <cm/cm_camera_controller.h>
#include <ec/ec_mgr.h>
#include <gf/gf_3d_scene.h>
#include <gf/gf_archive.h>
#include <gf/gf_model.h>
#include <gm/gm_global.h>
#include <gm/gm_lib.h>
#include <gr/gr_madein.h>
#include <if/if_smash_appear.h>
#include <memory.h>
#include <mt/mt_matrix.h>
#include <mt/mt_prng.h>
#include <nw4r/math/math_arithmetic.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <snd/snd_3d_generator.h>
#include <snd/snd_id.h>
#include <st/st_class_info.h>
#include <st/st_melee.h>
#include <st/st_positions.h>
#include <st/st_utility.h>
#include <stdio.h>
#include <string.h>
#include <types.h>

#include <st_dxcorneria/st_dxcorneria.h>

// Unnamed helpers the stage calls directly (see st_newpork.cpp for the grMadein ones).
// grMadein::setMatrix(Matrix*, bool): lets the ground (and optionally its attack) follow the matrix.
extern "C" void fn_27_279FC8(grMadein* ground, Matrix* matrix, bool followAttack);
// grMadein::switchToMatrix
extern "C" void fn_27_279FA4(grMadein* ground);
// Stage: HYPOTHESIS: true while the stage is the active one (not the one used for the menu preview).
extern "C" int fn_27_2249E0(Stage* stage);
// Matrix helpers of sora/mt (see mt_matrix.cpp).
extern "C" void fn_8003E5B4(Matrix* mtx);
extern "C" void fn_8003E3BC(const Matrix* mtx, const Vec3f* src, Vec3f* dst);
extern "C" void fn_8003E87C(Matrix* mtx, float angle);
extern "C" void fn_8003EBF4(Matrix* mtx, float angle);
extern "C" void fn_8003F074(Matrix* mtx, float x, float y, float z);

stClassInfoImpl<Stages::DxCorneria, stDxCorneria> stDxCorneria::bss_loc_14;

// The values the stage is tuned with, preceded by the word the coroutine tag of the falling Great Fox starts with.
struct stDxCorneriaStatics {
    int m_noTag;
    stDxCorneriaParam m_param;
};
static stDxCorneriaStatics sStatics = {
    0,
    {30.0f,  180.0f, 0.002f, 0.1f, 20.0f,  20.0f,   1200.0f, 1200.0f, 180.0f, 0.5f,   160.0f, 160.0f,
     8,      12,     100.0f, 600.0f, 600.0f, 960.0f, 1800.0f, 0.15f,   {4, 2, 1, 1, 1, 1}, 120.0f,
     0.2f,   0.5f,   16,     8,      8,      4,      0,       2.0f},
};

// The number of motions of the Arwing (0) and of the Wolfen (1).
static int sMotionCount[2] = {11, 4};

// The coroutine markers of the falling Great Fox (see NEWPORK_SAVE in st_newpork.cpp).
#define CORNERIA_SAVE(seq, lineNumber)       \
    do {                                     \
        static int sTag = 1;                 \
        (seq).m_tag = sTag;                  \
        (seq).m_line = (lineNumber);         \
    } while (0)

// The power of a shot of the Great Fox: the big one (charged) and the small one.
static const int sShotPower[2] = {48, 5};

// HYPOTHESIS: the tuning of the sway of the Pokemon Trainer's base: shortest/longest wait, acceleration, speed limit.
static float sBaseParams[14] = {30.0f, 180.0f, 0.005f, 0.4f, 0.0f, -100.0f, -100.0f, 20.0f, -10.0f, 100.0f, 100.0f, 70.0f, 20.0f, 0.0f};

static Vec3f sBeamAPos(-44.0f, 0.0f, 0.0f);
static Vec3f sBeamGPos(-104.0f, 0.0f, 0.0f);
static Vec3f sCanonPos(16.0f, 0.0f, 0.0f);
static Vec3f sCanonPosSpare;
static snd3DGenerator sBeamSound;

// MATCH-ONLY: the original scales vectors in place with paired singles (inline asm in the shared vector code).
static inline void corneriaScaleVec3f(register Vec3f* v, register float c) {
    register float fr1, fr0;
    // clang-format off
    asm {
        psq_l    fr0, Vec3f.m_x(v), 0, 0
        psq_l    fr1, Vec3f.m_z(v), 1, 0
        ps_muls0 fr0, fr0, c
        ps_muls0 fr1, fr1, c
        psq_st   fr0, Vec3f.m_x(v), 0, 0
        psq_st   fr1, Vec3f.m_z(v), 1, 0
    }
    // clang-format on
}

// Which of the three stretches the terrain's animation (and so the stage) is in, `offset` frames ahead.
static inline int terrainPhase(grDxCorneria* terrain, int offset) {
    if (terrain != NULL) {
        gfModelAnimation* anim = terrain->m_modelAnims[0];
        if (anim != NULL) {
            int frame = (int)anim->getFrame() + offset;
            if (frame >= 450 && frame < 1450) {
                return 0;
            }
            if (frame >= 1450 && frame < 2450) {
                return 2;
            }
        }
    }
    return 1;
}

// HYPOTHESIS: inline fsel based min / max helpers (the same pattern the glide statuses use).
static inline float corneriaMin(float value, float hi) {
    return nw4r::math::FSelect(value - hi, hi, value);
}
static inline float corneriaMax(float value, float lo) {
    return nw4r::math::FSelect(value - lo, value, lo);
}

// The constructor is inlined into create() (and into the class info's create()).
inline stDxCorneria::stDxCorneria() : stMelee("stDxCorneria", Stages::DxCorneria), m_seq(sStatics.m_noTag) {
    m_appearTask = NULL;
    m_appearing = 0;
    m_appearKind = 0;
}

stDxCorneria* stDxCorneria::create() {
    return new (Heaps::StageInstance) stDxCorneria;
}

stDxCorneria::~stDxCorneria() {
    releaseArchive();
    if (m_appearTask != NULL) {
        m_appearTask->exit();
    }
}

bool stDxCorneria::loading() {
    return true;
}

void stDxCorneria::setAppearKind(u8 kind) {
    m_appearKind = kind;
}

bool stDxCorneria::startAppear() {
    int index = randi(5);
    if ((u32)index >= 4) {
        index = 4;
    }
    IfSmashAppearTask* task = m_appearTask;
    int arg = m_appearKind * 5 + index;
    if (task != NULL) {
        task->start(arg);
    }
    m_appearing = 1;
    return true;
}

void stDxCorneria::endAppear() {
    if (isAppear()) {
        if (m_appearTask != NULL) {
            m_appearTask->stop(0);
        }
    }
}

bool stDxCorneria::isStartAppearTimming() {
    return m_appearing != 1;
}

bool stDxCorneria::isAppear() {
    return m_appearTask->isPlaying();
}

void stDxCorneria::forceStopAppear() {
    if (m_appearTask != NULL) {
        m_appearTask->stop(1);
    }
}

IfSmashAppearTask* stDxCorneria::getAppearTask() {
    return m_appearTask;
}

#define CORNERIA_GROUND(index) static_cast<grMadein*>(getGround(index))

// Creates a ground the stage keeps a pointer to: it is registered, started up with the stage's data and set up.
#define CORNERIA_SETUP(ground)                                                \
    addGround(ground);                                                        \
    (ground)->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);              \
    (ground)->setStageData(m_param)

void stDxCorneria::createObj() {
    CameraController* camera = CameraController::getInstance();
    reinterpret_cast<u8*>(camera)[0x44] |= 0x2;
    *reinterpret_cast<float*>(reinterpret_cast<u8*>(camera) + 0x190) = 1.308997f;
    testStageParamInit(m_fileData, 10);
    testStageDataInit(m_fileData, 0x14, 0x8C);
    m_param = &sStatics.m_param;

    m_terrain = grDxCorneria::create(0, "", "grDxCorneriaTerrain");
    if (m_terrain != NULL) {
        CORNERIA_SETUP(m_terrain);
        m_terrain->initializeEntity();
        m_terrain->startEntityAutoLoop();
        m_waterHighNode = m_terrain->getNodeIndex(0, "WaterHightN");
    }

    m_greatFox = grDxCorneria::create(1, "", "grDxCorneriaGreatFox");
    if (m_greatFox != NULL) {
        CORNERIA_SETUP(m_greatFox);
        m_greatFox->initializeEntity();
        m_greatFox->startEntityAutoLoop();
        fn_27_279FA4(m_greatFox);
        m_greatFox->m_calcWorldCallBack.m_nodeCallbackDatas[0].m_matrix.setIdentity();
        createCollision(m_fileData, 2, m_greatFox);
        m_gunNormalNode = m_greatFox->getNodeIndex(0, "GunNormalN");
        m_gunCrashNode = m_greatFox->getNodeIndex(0, "GunCrashN");
        m_chargeNode[0] = m_greatFox->getNodeIndex(0, "JyudenLN");
        m_chargeNode[1] = m_greatFox->getNodeIndex(0, "JyudenRN");
        m_splashANode[2] = m_greatFox->getNodeIndex(0, "ShibukiA0N");
        m_splashANode[1] = m_greatFox->getNodeIndex(0, "ShibukiA1N");
        m_splashANode[0] = m_greatFox->getNodeIndex(0, "ShibukiA2N");
        m_splashBNode[1] = m_greatFox->getNodeIndex(0, "ShibukiB0N");
        m_splashBNode[0] = m_greatFox->getNodeIndex(0, "ShibukiB1N");
        m_nullNode = m_greatFox->getNodeIndex(0, "null255");
        m_greatFox->setNodeVisibility(true, 0, m_gunNormalNode, false, false);
        m_greatFox->setNodeCollision(true, 0, (u16)m_gunNormalNode, false);
        m_greatFox->setNodeVisibility(false, 0, m_gunCrashNode, false, false);
    }

    m_greatFoxHit = grDxCorneria::create(9, "", "grDxCorneriaGreatFoxHit");
    if (m_greatFoxHit != NULL) {
        CORNERIA_SETUP(m_greatFoxHit);
        Vec3f hitStart(6.0f, 0.0f, 0.0f);
        Vec3f hitEnd(10.0f, 0.0f, 0.0f);
        m_greatFoxHit->setHitPoint(7.0f, &hitStart, &hitEnd, true, 0);
        m_greatFoxHit->initializeEntity();
        m_greatFoxHit->startEntity();
    }

    m_greatFoxAttack = grDxCorneria::create(9, "", "grDxCorneriaGreatFoxHit");
    if (m_greatFoxAttack != NULL) {
        CORNERIA_SETUP(m_greatFoxAttack);
        setGFoxCanonAttack(m_greatFoxAttack);
        m_greatFoxAttack->initializeEntity();
    }

    m_arwing = grDxCorneria::create(3, "", "grDxCorneriaArwing");
    if (m_arwing != NULL) {
        CORNERIA_SETUP(m_arwing);
        m_arwing->initializeEntity();
        m_arwing->setEnableCollisionStatus(false);
        fn_27_279FA4(m_arwing);
        createCollision(m_fileData, 3, m_arwing);
        m_arwingBeamNode[0] = m_arwing->getNodeIndex(0, "BeamL");
        m_arwingBeamNode[1] = m_arwing->getNodeIndex(0, "BeamR");
    }

    m_wolfen = grDxCorneria::create(4, "", "grDxCorneriaWolfen");
    if (m_wolfen != NULL) {
        CORNERIA_SETUP(m_wolfen);
        m_wolfen->initializeEntity();
        m_wolfen->setEnableCollisionStatus(false);
        fn_27_279FA4(m_wolfen);
        createCollision(m_fileData, 4, m_wolfen);
        m_wolfenBeamNode[0] = m_wolfenBeamNode[1] = m_wolfen->getNodeIndex(0, "WolfenBeamN");
    }

    m_arwingMotionA = grDxCorneria::create(5, "", "grDxCorneriaArwingMotionA");
    if (m_arwingMotionA != NULL) {
        CORNERIA_SETUP(m_arwingMotionA);
        m_arwingMotionA->initializeEntity();
        m_arwingMoveNode[0] = m_arwingMotionA->getNodeIndex(0, "ArwingMoveAN");
    }

    m_arwingMotionB = grDxCorneria::create(6, "", "grDxCorneriaArwingMotionB");
    if (m_arwingMotionB != NULL) {
        CORNERIA_SETUP(m_arwingMotionB);
        m_arwingMotionB->initializeEntity();
        m_arwingMoveNode[1] = m_arwingMotionB->getNodeIndex(0, "ArwingMoveBN");
    }

    // The beams of the Arwings: a mesh and a hit box each, parked far away until they are used.
    m_beamACount = 8;
    for (int i = 0; i < m_beamACount; i++) {
        grDxCorneria* mesh = grDxCorneria::create(7, "", "grDxCorneriaBeamA");
        if (mesh != NULL) {
            CORNERIA_SETUP(mesh);
            mesh->initializeEntity();
            if (i == 0) {
                m_beamAFrontNode = mesh->getNodeIndex(0, "BeamFrontAN");
                m_beamARearNode = mesh->getNodeIndex(0, "BeamRearAN");
            }
            grDxCorneriaBeam* hit = grDxCorneriaBeam::create(9, "", "grDxCorneriaBeamAAttack");
            if (hit != NULL) {
                CORNERIA_SETUP(hit);
                setBeamAAttack(hit);
                hit->initializeEntity();
            }
            m_beamA[i].m_ground = mesh;
            m_beamA[i].m_hit = hit;
            if (mesh != NULL) {
                mesh->endEntity();
                mesh->setVisibility(0);
                mesh->setEnableCollisionStatus(false);
                fn_27_279FA4(mesh);
                grNodeCallbackData* data = mesh->m_calcWorldCallBack.m_nodeCallbackDatas;
                data->m_matrix.setIdentity();
                Vec3f parked(-1000.0f, 0.0f, 0.0f);
                data->m_matrix.m[0][3] = parked.m_x;
                data->m_matrix.m[1][3] = parked.m_y;
                data->m_matrix.m[2][3] = parked.m_z;
                hit->endEntity();
                fn_27_279FA4(hit);
                data = hit->m_calcWorldCallBack.m_nodeCallbackDatas;
                data->m_matrix.setIdentity();
                data->m_matrix.m[0][3] = parked.m_x;
                data->m_matrix.m[1][3] = parked.m_y;
                data->m_matrix.m[2][3] = parked.m_z;
            }
            m_beamA[i].m_state = 0;
        }
    }

    // The shots of the Great Fox.
    m_beamGCount = (m_param->m_cannonShotsMax > m_param->m_cannonShotsMin) ? m_param->m_cannonShotsMax : m_param->m_cannonShotsMin;
    if (m_beamGCount > 0x20) {
        m_beamGCount = 0x20;
    }
    for (int i = 0; i < m_beamGCount; i++) {
        grDxCorneria* mesh = grDxCorneria::create(8, "", "grDxCorneriaBeamG");
        if (mesh != NULL) {
            CORNERIA_SETUP(mesh);
            mesh->initializeEntity();
            if (i == 0) {
                m_beamGFrontNode = mesh->getNodeIndex(0, "BeamFrontGN");
                m_beamGRearNode = mesh->getNodeIndex(0, "BeamRearGN");
            }
            grDxCorneriaBeam* hit = grDxCorneriaBeam::create(9, "", "grDxCorneriaBeamAAttack");
            if (hit != NULL) {
                CORNERIA_SETUP(hit);
                setBeamGAttack(hit);
                hit->initializeEntity();
            }
            m_beamG[i].m_ground = mesh;
            m_beamG[i].m_hit = hit;
            if (mesh != NULL) {
                mesh->endEntity();
                mesh->setVisibility(0);
                mesh->setEnableCollisionStatus(false);
                fn_27_279FA4(mesh);
                grNodeCallbackData* data = mesh->m_calcWorldCallBack.m_nodeCallbackDatas;
                data->m_matrix.setIdentity();
                Vec3f parked(-1000.0f, 0.0f, 0.0f);
                data->m_matrix.m[0][3] = parked.m_x;
                data->m_matrix.m[1][3] = parked.m_y;
                data->m_matrix.m[2][3] = parked.m_z;
                hit->endEntity();
                fn_27_279FA4(hit);
                data = hit->m_calcWorldCallBack.m_nodeCallbackDatas;
                data->m_matrix.setIdentity();
                data->m_matrix.m[0][3] = parked.m_x;
                data->m_matrix.m[1][3] = parked.m_y;
                data->m_matrix.m[2][3] = parked.m_z;
            }
            m_beamG[i].m_state = 0;
        }
    }

    // The base the Pokemon Trainer stands on only exists when the stage is the active one.
    if (fn_27_2249E0(this)) {
        m_trainerBase = grDxCorneria::create(0xA, "", "grDxCorneriaTPBase");
        if (m_trainerBase != NULL) {
            CORNERIA_SETUP(m_trainerBase);
            m_trainerBase->initializeEntity();
            m_trainerBase->startEntityAutoLoop();
            fn_27_279FA4(m_trainerBase);
            m_trainerBase->m_calcWorldCallBack.m_nodeCallbackDatas[0].m_matrix.setIdentity();
            m_trainerNode[0] = m_trainerBase->getNodeIndex(0, "PokemonTrainer1N");
            m_trainerNode[1] = m_trainerBase->getNodeIndex(0, "PokemonTrainer2N");
            m_trainerNode[2] = m_trainerBase->getNodeIndex(0, "PokemonTrainer3N");
            m_trainerNode[3] = m_trainerBase->getNodeIndex(0, "PokemonTrainer4N");
        }
    } else {
        m_trainerBase = NULL;
    }

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

    m_cannonHp = m_param->m_cannonHp;
    m_greatFox->setNodeVisibility(true, 0, m_gunNormalNode, false, false);
    m_greatFox->setNodeCollision(true, 0, (u16)m_gunNormalNode, false);
    m_greatFox->setNodeVisibility(false, 0, m_gunCrashNode, false, false);
    m_cannonState = 0;
    m_cannonSide = 0;
    m_cannonTimer = m_param->m_cannonWaitMin + (m_param->m_cannonWaitMax - m_param->m_cannonWaitMin) * (1.0f * randf());
    m_arwingIsWolfen = 0;
    m_arwingActive = NULL;
    m_arwingTimer = m_param->m_arwingFirstMin + (m_param->m_arwingFirstMax - m_param->m_arwingFirstMin) * (1.0f * randf());
    m_splashAActive = false;
    m_splashBActive = false;
    m_arwingRoll = 0.0f;
    m_splashAEffect = 0;
    m_splashBEffect = 0;
    m_seq.m_tag = sStatics.m_noTag;
    m_seq.m_line = 0;
    m_fallTime = 0.0f;
    m_fallSpeed = 0.0f;
    m_fallHeight = 0.0f;
    m_swayTimer = 0.0f;
    m_swayAccelX = 0.0f;
    m_swayAccelY = 0.0f;
    m_swaySpeedX = 0.0f;
    m_swaySpeedY = 0.0f;
    m_swayX = 0.0f;
    m_swayY = 0.0f;
    m_soundBaseHandle = 0;
    m_baseTimer = 0.0f;
    m_baseAccelX = 0.0f;
    m_baseAccelY = 0.0f;
    m_baseSpeedX = 0.0f;
    m_baseSpeedY = 0.0f;
    m_baseX = 0.0f;
    m_baseY = 50.0f;
    m_startCounter = 0;
    initPosPokeTrainer(4, 1);
    loadStageAttrParam(m_fileData, 30);
    m_appearTask = IfFoxSmashAppearTask::create(m_fileData);
}

void stDxCorneria::update(float deltaFrame) {
    GFoxUpdate(deltaFrame);
    PTBaseUpdate(deltaFrame);
    GFoxFallUpdate(deltaFrame);
    Vec3f pos(0.0f, -250.0f, 0.0f);
    pos.m_y = -250.0f - m_fallHeight;
    pos.m_y = pos.m_y - m_swayY;
    pos.m_x = 0.0f - m_swayX;
    m_terrain->setPos(&pos);
    if (m_startCounter > 10) {
        GFoxCanonUpdate(deltaFrame);
        ArwingAttackUpdate(deltaFrame);
        for (int i = 0; i < m_beamACount; i++) {
            m_beamA[i].update(deltaFrame, m_arwingActive);
        }
        for (int i = 0; i < m_beamGCount; i++) {
            m_beamG[i].update(deltaFrame, m_greatFox);
        }
        WaterSplashUpdate();
    } else {
        m_startCounter++;
    }
}

// The cannon of the Great Fox: it waits, charges at both sides, fires a series of shots and starts over; once its
// armor was shot to pieces it blows up.
void stDxCorneria::GFoxCanonUpdate(float deltaFrame) {
    Matrix nodeMtx(true);
    m_greatFox->getNodeMatrix(&nodeMtx, 0, m_chargeNode[0]);
    Vec3f cannonPos(nodeMtx.m[0][3], nodeMtx.m[1][3], nodeMtx.m[2][3]);
    if (m_cannonState < 5) {
        Vec3f hitPos = cannonPos;
        hitPos.m_y += 4.0f;
        hitPos.m_z = 0.0f;
        m_greatFoxHit->setPos(&hitPos);
        m_greatFoxAttack->setPos(&hitPos);
        if (m_cannonState == 0) {
            if (m_greatFoxHit->isHit() == 1) {
                u8* hitInfo = *reinterpret_cast<u8**>(reinterpret_cast<u8*>(m_greatFoxHit) + 0x160);
                m_cannonHp -= *reinterpret_cast<float*>(hitInfo + 0x28);
                // MATCH-ONLY: the hit flag is cleared with a byte access like the original.
                reinterpret_cast<u8*>(m_greatFoxHit)[0x16C] &= ~0x10;
                if (m_cannonHp <= 0.0f) {
                    m_cannonState = 5;
                }
            }
        }
    }
    m_soundCannon.setPos(&cannonPos);
    m_cannonTimer -= deltaFrame;
    switch (m_cannonState) {
        case 0:
            if (m_cannonTimer <= 0.0f) {
                m_cannonState = 1;
                int side = 1;
                if (1.0f * randf() <= m_param->m_cannonSideChance) {
                    side = 0;
                }
                m_cannonSide = side;
                if (side == 0) {
                    m_cannonTimer = m_param->m_cannonTimeA;
                    m_cannonSpeed = 80.0f;
                    m_cannonShots = 1;
                    m_soundCannon.playSE(snd_se_stage_Corneria_gfox_tame_s, -1, 0, -1);
                } else {
                    m_cannonTimer = m_param->m_cannonTimeB;
                    m_cannonSpeed = 10.0f;
                    int range = m_param->m_cannonShotsMax - m_param->m_cannonShotsMin;
                    int extra = randi(range + 1);
                    if ((u32)extra >= (u32)range) {
                        extra = range;
                    }
                    m_cannonShots = m_param->m_cannonShotsMin + extra;
                    m_soundCannon.playSE(snd_se_stage_Corneria_gfox_tame_l, -1, 0, -1);
                }
                u32 effect = g_ecMgr->setEffect(ef_ptc_stg_dx_corneria_syukou);
                g_ecMgr->setParent(effect, m_greatFox->m_sceneModels[0], (u16)m_chargeNode[0], false);
                m_chargeEffect[0] = effect;
                effect = g_ecMgr->setEffect(ef_ptc_stg_dx_corneria_syukou);
                g_ecMgr->setParent(effect, m_greatFox->m_sceneModels[0], (u16)m_chargeNode[1], false);
                m_chargeEffect[1] = effect;
                m_greatFoxAttack->startEntity();
            }
            break;
        case 1:
            if (m_cannonTimer <= 0.0f) {
                m_cannonState = 2;
                m_cannonTimer = m_param->m_cannonChargeTime;
                m_greatFoxAttack->endEntity();
                g_ecMgr->endEffect(m_chargeEffect[0]);
                g_ecMgr->endEffect(m_chargeEffect[1]);
            }
            break;
        case 2: {
            if (m_cannonTimer <= 0.0f) {
                m_cannonState = 3;
            }
            Vec3f rectA(cannonPos.m_x - 200.0f, cannonPos.m_y + 10.0f, -cannonPos.m_z);
            Vec3f rectB(cannonPos.m_x + 1.0f, cannonPos.m_y - 3.0f, cannonPos.m_z);
            if (checkRectInAnyPlayer(&rectB, &rectA)) {
                m_cannonState = 3;
            }
            break;
        }
        case 3: {
            m_cannonShots--;
            int shooter = m_cannonShots & 1;
            int power = sShotPower[m_cannonSide];
            u32 node = m_chargeNode[shooter];
            stDxCorneriaBeam* slot = m_beamG;
            for (int i = 0; i < m_beamGCount; i++, slot++) {
                if (slot->m_state == 0) {
                    slot->m_state = 1;
                    slot->m_node = node;
                    slot->m_power = power;
                    slot->m_aimed = false;
                    slot->m_fromArwing = true;
                    slot->m_reflectable = true;
                    slot->m_homing = true;
                    break;
                }
            }
            u32 effect = g_ecMgr->setEffect(ef_ptc_stg_dx_corneria_hassya);
            g_ecMgr->setParent(effect, m_greatFox->m_sceneModels[0], (u16)node, false);
            m_soundCannon.playSE((SndID)(snd_se_stage_Corneria_shot2 - (m_cannonSide == 0)), -1, 0, -1);
            if (m_cannonShots > 0) {
                m_cannonState = 4;
                m_cannonTimer = 8.0f;
            } else {
                m_cannonState = 0;
                m_cannonSide = 0;
                m_cannonTimer = m_param->m_cannonWaitMin + (m_param->m_cannonWaitMax - m_param->m_cannonWaitMin) * (1.0f * randf());
            }
            break;
        }
        case 4:
            if (m_cannonTimer <= 0.0f) {
                m_cannonState = 3;
            }
            break;
        case 5: {
            m_greatFox->setNodeVisibility(false, 0, m_gunNormalNode, false, false);
            m_greatFox->setNodeCollision(false, 0, (u16)m_gunNormalNode, false);
            m_greatFox->setNodeVisibility(true, 0, m_gunCrashNode, false, false);
            m_greatFoxHit->endEntity();
            m_cannonState = 6;
            u32 effect = g_ecMgr->setEffect(ef_ptc_stg_dx_corneria_bakuhatu);
            g_ecMgr->setParent(effect, m_greatFox->m_sceneModels[0], (u16)m_chargeNode[0], false);
            effect = g_ecMgr->setEffect(ef_ptc_stg_dx_corneria_bakuhatu);
            g_ecMgr->setParent(effect, m_greatFox->m_sceneModels[0], (u16)m_chargeNode[1], false);
            m_soundCannon.playSE((SndID)0x51, -1, 0, -1);
            break;
        }
    }
}

// Picks which flyer comes next (an Arwing or, once in a while, a Wolfen), which flight it takes and starts it.
void stDxCorneria::ArwingAttackStart() {
    m_arwingIsWolfen = (1.0f * randf() <= m_param->m_wolfenChance);
    m_arwingActive = m_arwingIsWolfen ? m_wolfen : m_arwing;
    int phase = terrainPhase(m_terrain, 300);
    if (phase == 1) {
        m_arwingKind = 0;
    } else {
        m_arwingKind = (1.0f * randf() <= 0.29411766f);
    }
    int count = sMotionCount[m_arwingKind];
    int motion = randi(count + 1);
    if ((u32)motion >= (u32)count) {
        motion = count;
    }
    m_arwingMotion = motion;
    m_arwingMotionGround = (&m_arwingMotionA)[m_arwingKind];
    m_arwingMotionNode = m_arwingMoveNode[m_arwingKind];
    m_arwingMotionGround->setMotion((u8)motion);
    m_arwingMotionGround->startEntity();
    m_arwingRoll = 0.0f;
    m_arwingTimer = (float)m_param->m_arwingFlightTime;
    m_soundArwingA = 0;
    m_soundArwingB = 0;
    m_arwingRollTimer = 2.0f;
}

// Fills the first free slot of a beam list.
static inline void startBeam(stDxCorneriaBeam* slots, int count, u32 node, int power, Vec3f* target, bool fromArwing, bool reflectable) {
    for (int i = 0; i < count; i++, slots++) {
        if (slots->m_state == 0) {
            slots->m_state = 1;
            slots->m_node = node;
            slots->m_power = power;
            if (target != NULL) {
                slots->m_target = *target;
                slots->m_aimed = true;
            } else {
                slots->m_aimed = false;
            }
            slots->m_fromArwing = fromArwing;
            slots->m_reflectable = reflectable;
            slots->m_homing = true;
            break;
        }
    }
}

// The flight of the flyer: follows the motion's node, plays its sounds, rolls and shoots.
void stDxCorneria::ArwingAttackUpdate(float deltaFrame) {
    m_arwingTimer -= deltaFrame;
    if (m_arwingTimer <= 0.0f && m_arwingActive == NULL) {
        ArwingAttackStart();
    }
    if (m_arwingActive != NULL && m_arwingMotionGround != NULL) {
        if (m_arwingMotionGround->isEndEntity()) {
            m_arwingMotionGround->endEntity();
            m_arwingMotionGround = NULL;
            m_arwingActive->endEntity();
            m_arwingActive = NULL;
            m_arwingTimer = m_param->m_arwingWaitMin + (m_param->m_arwingWaitMax - m_param->m_arwingWaitMin) * (1.0f * randf());
        } else {
            Matrix nodeMtx(true);
            m_arwingMotionGround->getNodeMatrix(&nodeMtx, 0, m_arwingMotionNode);
            Vec3f soundPos(nodeMtx.m[0][3], nodeMtx.m[1][3] + 10.0f, nodeMtx.m[2][3]);
            nodeMtx.m[1][3] = soundPos.m_y;
            m_soundArwing.setPos(&soundPos);
            if (m_soundArwingA == 0) {
                m_soundArwingA = m_soundArwing.playSE((SndID)(snd_se_stage_Corneria_arwincome2 - (m_arwingKind == 0)), -1, 0, -1);
            }
            if (m_soundArwingB == 0) {
                gfModelAnimation* anim = m_arwingMotionGround->m_modelAnims[0];
                if (anim != NULL) {
                    float last = (float)anim->getFrameCount();
                    if (120.0f + anim->getFrame() >= last) {
                        m_soundArwingB = m_soundArwing.playSE(snd_se_stage_Corneria_arwinbye, -1, 0, -1);
                    }
                }
            }
            if (!m_arwingActive->m_isVisible) {
                if (m_arwingRollTimer <= 0.0f) {
                    m_arwingActive->startEntityAutoLoop();
                } else {
                    m_arwingRollTimer -= deltaFrame;
                }
            }
            if (m_arwingRoll != 0.0f) {
                if (m_arwingRoll < 0.0f) {
                    float next = m_arwingRoll + 0.31415927f * deltaFrame;
                    m_arwingRoll = nw4r::math::FSelect(0.0f - next, next, 0.0f);
                } else {
                    float next = m_arwingRoll - 0.31415927f * deltaFrame;
                    m_arwingRoll = nw4r::math::FSelect(next - 0.0f, next, 0.0f);
                }
                Matrix rotation(true);
                rotation.setIdentity();
                fn_8003E87C(&rotation, m_arwingRoll);
                nodeMtx.mul(&rotation, &nodeMtx);
                m_arwingTimer = (float)m_param->m_arwingFlightTime;
                m_arwingActive->setEnableCollisionStatus(false);
            } else {
                m_arwingActive->setEnableCollisionStatus(m_arwingKind == 0);
            }
            fn_27_279FC8(m_arwingActive, &nodeMtx, false);
            if (m_arwingRoll == 0.0f && m_arwingTimer <= 0.0f) {
                m_arwingTimer = (float)m_param->m_arwingFlightTime;
                int shots = m_param->m_arwingShotChance - 1;
                int pick = randi(shots + 1);
                if ((u32)pick >= (u32)shots) {
                    pick = shots;
                }
                if (pick == 1) {
                    bool fired = false;
                    Vec3f target;
                    Vec3f* targetPtr = NULL;
                    if (m_arwingKind == 1) {
                        if (GetArwingTarget(&target)) {
                            targetPtr = &target;
                            m_soundArwing.playSE(snd_se_stage_Corneria_arwin_shot_swish, -1, 0, -1);
                            fired = true;
                        }
                    } else {
                        m_soundArwing.playSE(snd_se_stage_Corneria_arwin_shot, -1, 0, -1);
                        fired = true;
                    }
                    if (fired) {
                        if (m_arwingIsWolfen == 0) {
                            startBeam(m_beamA, m_beamACount, m_arwingBeamNode[0], 6, targetPtr, m_arwingKind == 0, true);
                            startBeam(m_beamA, m_beamACount, m_arwingBeamNode[1], 6, targetPtr, m_arwingKind == 0, m_arwingKind == 1);
                        } else {
                            startBeam(m_beamA, m_beamACount, m_wolfenBeamNode[0], 6, targetPtr, m_arwingKind == 0, true);
                        }
                    }
                } else if (m_arwingKind == 0) {
                    int roll = randi(0x10);
                    if ((u32)roll >= 0xF) {
                        roll = 0xF;
                    }
                    if (roll == 1) {
                        int direction = randi(8);
                        if ((u32)direction >= 7) {
                            direction = 7;
                        }
                        m_arwingRoll = (direction & 1) ? 6.2831855f : -6.2831855f;
                    }
                }
            }
        }
    }
}

// Looks for a fighter the Wolfen can aim at: it has to be in front of it and close enough.
bool stDxCorneria::GetArwingTarget(Vec3f* target) {
    Matrix mtx = m_arwingActive->m_calcWorldCallBack.m_nodeCallbackDatas[0].m_matrix;
    Vec3f origin(mtx.m[0][3], mtx.m[1][3], mtx.m[2][3]);
    Vec3f forward(-1.0f, 0.0f, 0.0f);
    fn_8003E5B4(&mtx);
    Vec3f local;
    fn_8003E3BC(&mtx, &forward, &local);
    bool found = false;
    float bestDot = 0.99f;
    for (u32 i = 0; i < 4; i++) {
        Vec3f playerPos;
        if (getPlayerPosition(i, &playerPos)) {
            playerPos.m_y += 10.0f;
            Vec3f offset = playerPos - origin;
            float distanceSq = offset.m_x * offset.m_x + offset.m_y * offset.m_y + offset.m_z * offset.m_z;
            if (distanceSq <= 10000.0f) {
                offset.normalize();
                float dot = local.m_x * offset.m_x + local.m_y * offset.m_y + local.m_z * offset.m_z;
                if (dot >= bestDot) {
                    bestDot = dot;
                    found = true;
                    corneriaScaleVec3f(&offset, 1000.0f);
                    *target = playerPos + offset;
                }
            }
        }
    }
    return found;
}

// The splashes at both sides of the Great Fox: an effect appears where the line between two nodes crosses the water.
void stDxCorneria::WaterSplashUpdate() {
    Matrix mtx(true);
    m_terrain->getNodeMatrix(&mtx, 0, m_waterHighNode);
    Vec3f water(mtx.m[0][3], mtx.m[1][3], mtx.m[2][3]);
    Vec3f pointsA[3];
    Vec3f pointsB[2];
    for (int i = 0; i < 3; i++) {
        m_greatFox->getNodeMatrix(&mtx, 0, m_splashANode[i]);
        pointsA[i].m_x = mtx.m[0][3];
        pointsA[i].m_y = mtx.m[1][3];
        pointsA[i].m_z = mtx.m[2][3];
    }
    for (int i = 0; i < 2; i++) {
        m_greatFox->getNodeMatrix(&mtx, 0, m_splashBNode[i]);
        pointsB[i].m_x = mtx.m[0][3];
        pointsB[i].m_y = mtx.m[1][3];
        pointsB[i].m_z = mtx.m[2][3];
    }

    bool inWater;
    Vec3f splash;
    if (pointsA[0].m_y < water.m_y) {
        inWater = true;
        splash = pointsA[0];
    } else {
        inWater = false;
        for (int i = 0; i < 2; i++) {
            if (pointsA[i + 1].m_y < water.m_y) {
                inWater = true;
                float t = (water.m_y - pointsA[i].m_y) / (pointsA[i + 1].m_y - pointsA[i].m_y);
                Vec3f delta = pointsA[i + 1] - pointsA[i];
                corneriaScaleVec3f(&delta, t);
                splash = delta + pointsA[i];
                break;
            }
        }
    }
    if (m_splashAActive) {
        if (!inWater) {
            g_ecMgr->endEffect(m_splashAEffect);
            m_splashAEffect = 0;
        } else {
            g_ecMgr->setPos(m_splashAEffect, &splash);
        }
    } else if (inWater) {
        m_splashAEffect = g_ecMgr->setEffect(ef_ptc_stg_dx_corneria_shibuki);
        g_ecMgr->setPos(m_splashAEffect, &splash);
    }
    m_splashAActive = inWater;

    if (pointsB[0].m_y < water.m_y) {
        inWater = true;
        splash = pointsB[0];
    } else if (pointsB[1].m_y < water.m_y) {
        inWater = true;
        float t = (water.m_y - pointsB[0].m_y) / (pointsB[1].m_y - pointsB[0].m_y);
        Vec3f delta = pointsB[1] - pointsB[0];
        corneriaScaleVec3f(&delta, t);
        splash = delta + pointsB[0];
    } else {
        inWater = false;
    }
    if (m_splashBActive) {
        if (!inWater) {
            g_ecMgr->endEffect(m_splashBEffect);
            m_splashBEffect = 0;
            if (m_soundSplashHandle >= 0) {
                m_soundSplash.stopSE(m_soundSplashHandle, 20);
            }
            m_soundSplashHandle = -1;
        } else {
            g_ecMgr->setPos(m_splashBEffect, &splash);
            m_soundSplash.setPos(&splash);
        }
    } else if (inWater) {
        m_splashBEffect = g_ecMgr->setEffect(ef_ptc_stg_dx_corneria_shibuki);
        g_ecMgr->setPos(m_splashBEffect, &splash);
        m_soundSplash.setPos(&splash);
        m_soundSplashHandle = m_soundSplash.playSE(snd_se_stage_Corneria_hikouon2, -1, 0, -1);
    }
    m_splashBActive = inWater;
}

// One slot of the beams: starts the beam at its node, lets it fly along its direction and ends it.
void stDxCorneriaBeam::update(float deltaFrame, grMadein* source) {
    switch (m_state) {
        case 1:
            m_ground->startEntity();
            m_ground->setVisibility(1);
            m_hit->resetBeam();
            m_ground->setMotion(0);
            {
                gfModelAnimation* anim = m_ground->m_modelAnims[0];
                if (anim != NULL) {
                    anim->setFrame(0.0f);
                    anim->setLoop(false);
                }
            }
            m_state = 2;
            // FALL-THROUGH
        case 2:
            if (!m_ground->isEndEntity()) {
                if (source != NULL) {
                    Matrix mtx(true);
                    source->getNodeMatrix(&mtx, 0, m_node);
                    fn_27_279FC8(m_ground, &mtx, false);
                    if (m_fromArwing) {
                        mtx.m[2][3] = 0.0f;
                    }
                    if (m_hit->isEndEntity()) {
                        int power = m_power;
                        m_hit->getOverwriteAttackData()->m_power = power;
                        m_hit->startEntity();
                    }
                    fn_27_279FC8(m_hit, &mtx, false);
                }
                break;
            }
            m_state = 3;
            m_timer = 0.0f;
            goto fly;
        case 4: {
        fly:
            m_timer += deltaFrame;
            grNodeCallbackData* data = m_ground->m_calcWorldCallBack.m_nodeCallbackDatas;
            if (m_homing) {
                Vec3f pos(data->m_matrix.m[0][3], data->m_matrix.m[1][3], data->m_matrix.m[2][3]);
                if (-20.0f <= pos.m_z && pos.m_z <= 20.0f) {
                    Matrix inverse = data->m_matrix;
                    fn_8003E5B4(&inverse);
                    Vec3f direction;
                    fn_8003E3BC(&inverse, &sBeamAPos, &direction);
                    Vec3f hitPos;
                    Vec3f hitNormal;
                    if (stRayCheck(&pos, &direction, &hitPos, &hitNormal, false, NULL, false, 1)) {
                        u32 effect = g_ecMgr->setEffect((EfID)0x18);
                        g_ecMgr->setPos(effect, &hitPos);
                        m_timer = 10000.0f;
                        sBeamSound.setPos(&hitPos);
                        sBeamSound.playSE((SndID)0x50, -1, 0, -1);
                    }
                }
            }
            if (m_timer <= 300.0f && m_hit->m_hit) {
                fn_8003F074(&data->m_matrix, m_hit->m_angle, 0.0f, 0.0f);
                Vec3f effectPos(data->m_matrix.m[0][3], data->m_matrix.m[1][3], 0.0f);
                u32 effect = g_ecMgr->setEffect((EfID)0x18);
                g_ecMgr->setPos(effect, &effectPos);
                m_timer = 10000.0f;
                sBeamSound.setPos(&effectPos);
                sBeamSound.playSE((SndID)0x50, -1, 0, -1);
            }
            if (300.0f < m_timer) {
                m_ground->endEntity();
                m_ground->setVisibility(0);
                m_state = 0;
                Vec3f parked(-1000.0f, 0.0f, 0.0f);
                data = m_ground->m_calcWorldCallBack.m_nodeCallbackDatas;
                data->m_matrix.m[0][3] = parked.m_x;
                data->m_matrix.m[1][3] = parked.m_y;
                data->m_matrix.m[2][3] = parked.m_z;
                m_hit->endEntity();
                data = m_hit->m_calcWorldCallBack.m_nodeCallbackDatas;
                data->m_matrix.m[0][3] = parked.m_x;
                data->m_matrix.m[1][3] = parked.m_y;
                data->m_matrix.m[2][3] = parked.m_z;
                break;
            }
            if (m_fromArwing) {
                if (m_hit->m_reflected) {
                    m_hit->m_reflected = false;
                    fn_8003F074(&m_ground->m_calcWorldCallBack.m_nodeCallbackDatas->m_matrix, m_hit->m_angle, 0.0f, 0.0f);
                    fn_8003EBF4(&m_ground->m_calcWorldCallBack.m_nodeCallbackDatas->m_matrix, 3.1415927f);
                    m_timer = 0.0f;
                } else {
                    fn_8003F074(&m_ground->m_calcWorldCallBack.m_nodeCallbackDatas->m_matrix, -6.0f, 0.0f, 0.0f);
                }
                Matrix* groundMtx = &m_ground->m_calcWorldCallBack.m_nodeCallbackDatas->m_matrix;
                Matrix* hitMtx = &m_hit->m_calcWorldCallBack.m_nodeCallbackDatas->m_matrix;
                float x = groundMtx->m[0][3];
                float y = groundMtx->m[1][3];
                hitMtx->m[0][3] = x;
                hitMtx->m[1][3] = y;
                hitMtx->m[2][3] = 0.0f;
            } else {
                if (m_state == 3) {
                    Matrix inverse(true);
                    if (m_aimed) {
                        Vec3f pos(data->m_matrix.m[0][3], data->m_matrix.m[1][3], data->m_matrix.m[2][3]);
                        m_direction = m_target - pos;
                        if (1.0f < m_direction.lengthSq()) {
                            m_direction.normalize();
                            corneriaScaleVec3f(&m_direction, 6.0f);
                        } else {
                            inverse = data->m_matrix;
                            fn_8003E5B4(&inverse);
                            Vec3f forward(-6.0f, 0.0f, 0.0f);
                            fn_8003E3BC(&inverse, &forward, &m_direction);
                        }
                    } else {
                        inverse = data->m_matrix;
                        fn_8003E5B4(&inverse);
                        Vec3f forward(-6.0f, 0.0f, 0.0f);
                        fn_8003E3BC(&inverse, &forward, &m_direction);
                    }
                }
                Vec3f step(m_direction.m_x, m_direction.m_y, m_direction.m_z);
                corneriaScaleVec3f(&step, deltaFrame);
                step.m_x *= m_hit->m_scale;
                Vec3f pos(data->m_matrix.m[0][3], data->m_matrix.m[1][3], data->m_matrix.m[2][3]);
                pos += step;
                data->m_matrix.m[0][3] = pos.m_x;
                data->m_matrix.m[1][3] = pos.m_y;
                data->m_matrix.m[2][3] = pos.m_z;
                data = m_hit->m_calcWorldCallBack.m_nodeCallbackDatas;
                data->m_matrix.m[0][3] = pos.m_x;
                data->m_matrix.m[1][3] = pos.m_y;
                data->m_matrix.m[2][3] = pos.m_z;
            }
            m_state = 4;
            break;
        }
    }
}

// True if a fighter stands inside the box spanned by the two corners.
bool stDxCorneria::checkRectInAnyPlayer(Vec3f* a, Vec3f* b) {
    Vec3f low;
    Vec3f high;
    float diffX = a->m_x - b->m_x;
    high.m_x = nw4r::math::FSelect(diffX, a->m_x, b->m_x);
    low.m_x = nw4r::math::FSelect(diffX, b->m_x, a->m_x);
    float diffY = a->m_y - b->m_y;
    high.m_y = nw4r::math::FSelect(diffY, a->m_y, b->m_y);
    low.m_y = nw4r::math::FSelect(diffY, b->m_y, a->m_y);
    float diffZ = a->m_z - b->m_z;
    high.m_z = nw4r::math::FSelect(diffZ, a->m_z, b->m_z);
    low.m_z = nw4r::math::FSelect(diffZ, b->m_z, a->m_z);
    for (u32 i = 0; i < 4; i++) {
        Vec3f playerPos;
        if (getPlayerPosition(i, &playerPos)) {
            if (low.m_x <= playerPos.m_x && playerPos.m_x <= high.m_x && low.m_y <= playerPos.m_y &&
                playerPos.m_y <= high.m_y && low.m_z <= playerPos.m_z && playerPos.m_z <= high.m_z) {
                return true;
            }
        }
    }
    return false;
}

// The Great Fox dives into the water and comes back up: a coroutine like the planks of New Pork City.
void stDxCorneria::GFoxFallUpdate(float deltaFrame) {
    switch (m_seq.m_line) {
        case 0:
            CORNERIA_SAVE(m_seq, 0x742);
            // FALL-THROUGH
        case 0x742:
        case 0x74D:
        again:
            if (terrainPhase(m_terrain, 0) != 1) {
                CORNERIA_SAVE(m_seq, 0x74D);
                return;
            }
            // FALL-THROUGH
        case 0x753:
            if (terrainPhase(m_terrain, 0) != 0) {
                CORNERIA_SAVE(m_seq, 0x753);
                return;
            }
            // FALL-THROUGH
        case 0x759:
            if (terrainPhase(m_terrain, 200) != 2) {
                CORNERIA_SAVE(m_seq, 0x759);
                return;
            }
            if (randi(3) != 0) {
                OSReport("GFox Fall Skip...\n");
                goto again;
            }
            m_fallSpeed = 0.0f;
            m_fallTime = 0.0f;
            // FALL-THROUGH
        case 0x782:
            if (m_fallTime < 900.0f) {
                float t = m_fallSpeed / 0.005f;
                float brake = m_fallSpeed * t + (-0.0025f * t) * t;
                m_fallTime = m_fallTime + deltaFrame;
                if (m_fallHeight - -230.0f > brake) {
                    m_fallSpeed = corneriaMin(m_fallSpeed + 0.005f * deltaFrame, 3.0f);
                } else {
                    m_fallSpeed = corneriaMax(m_fallSpeed - 0.005f * deltaFrame, 0.005f);
                }
                m_fallHeight = corneriaMax(m_fallHeight - m_fallSpeed * deltaFrame, -230.0f);
                CORNERIA_SAVE(m_seq, 0x782);
                return;
            }
            // FALL-THROUGH
        case 0x79A:
            if (0.0f > m_fallHeight) {
                float t = m_fallSpeed / 0.005f;
                float brake = m_fallSpeed * t + (-0.0025f * t) * t;
                if (-m_fallHeight > brake) {
                    m_fallSpeed = corneriaMin(m_fallSpeed + 0.005f * deltaFrame, 3.0f);
                } else {
                    m_fallSpeed = corneriaMax(m_fallSpeed - 0.005f * deltaFrame, 0.005f);
                }
                m_fallHeight = corneriaMin(m_fallHeight + m_fallSpeed * deltaFrame, 0.0f);
                CORNERIA_SAVE(m_seq, 0x79A);
                return;
            }
            goto again;
    }
}

// The Great Fox sways: a random acceleration pushes it around inside a box.
void stDxCorneria::GFoxUpdate(float deltaFrame) {
    if (m_swayTimer <= 0.0f) {
        float range = m_param->m_swayWaitMax - m_param->m_swayWaitMin;
        m_swayTimer = m_param->m_swayWaitMin + range * randf();
        m_swayAccelX = m_param->m_swayAccel * (1.0f - 2.0f * randf());
        m_swayAccelY = m_param->m_swayAccel * (1.0f - 2.0f * randf());
    }
    m_swayTimer -= deltaFrame;
    m_swaySpeedX += m_swayAccelX * deltaFrame;
    m_swaySpeedY += m_swayAccelY * deltaFrame;
    m_swaySpeedX = nw4r::math::FSelect(m_param->m_swaySpeedLimit - m_swaySpeedX, m_swaySpeedX, m_param->m_swaySpeedLimit);
    m_swaySpeedX = nw4r::math::FSelect(-m_param->m_swaySpeedLimit - m_swaySpeedX, -m_param->m_swaySpeedLimit, m_swaySpeedX);
    m_swayX += m_swaySpeedX * deltaFrame;
    m_swaySpeedY = nw4r::math::FSelect(m_param->m_swaySpeedLimit - m_swaySpeedY, m_swaySpeedY, m_param->m_swaySpeedLimit);
    m_swaySpeedY = nw4r::math::FSelect(-m_param->m_swaySpeedLimit - m_swaySpeedY, -m_param->m_swaySpeedLimit, m_swaySpeedY);
    m_swayY += m_swaySpeedY * deltaFrame;
    if (m_swayX > m_param->m_swayLimitX) {
        m_swayAccelX = -m_param->m_swayAccel;
    } else if (m_swayX < -m_param->m_swayLimitX) {
        m_swayAccelX = m_param->m_swayAccel;
    }
    if (m_swayY > m_param->m_swayLimitY) {
        m_swayAccelY = -m_param->m_swayAccel;
    } else if (m_swayY < -m_param->m_swayLimitY) {
        m_swayAccelY = m_param->m_swayAccel;
    }
    Matrix mtx(true);
    m_greatFox->getNodeMatrix(&mtx, 0, m_nullNode);
    Vec3f pos(mtx.m[0][3], mtx.m[1][3], mtx.m[2][3]);
    m_soundBase.setPos(&pos);
    if (m_soundBaseHandle == 0) {
        m_soundBaseHandle = m_soundBase.playSE(snd_se_stage_Corneria_hikouon, -1, 0, -1);
    }
}

// The base of the Pokemon Trainer sways the same way; the trainer positions follow its nodes.
void stDxCorneria::PTBaseUpdate(float deltaFrame) {
    if (m_baseTimer <= 0.0f) {
        float range = sBaseParams[1] - sBaseParams[0];
        m_baseTimer = sBaseParams[0] + range * randf();
        m_baseAccelX = sBaseParams[2] * (1.0f - 2.0f * randf());
        m_baseAccelY = sBaseParams[2] * (1.0f - 2.0f * randf());
    }
    m_baseTimer -= deltaFrame;
    m_baseSpeedX += m_baseAccelX * deltaFrame;
    m_baseSpeedY += m_baseAccelY * deltaFrame;
    m_baseSpeedX = nw4r::math::FSelect(sBaseParams[3] - m_baseSpeedX, m_baseSpeedX, sBaseParams[3]);
    m_baseSpeedX = nw4r::math::FSelect(-sBaseParams[3] - m_baseSpeedX, -sBaseParams[3], m_baseSpeedX);
    m_baseX += m_baseSpeedX * deltaFrame;
    m_baseSpeedY = nw4r::math::FSelect(sBaseParams[3] - m_baseSpeedY, m_baseSpeedY, sBaseParams[3]);
    m_baseSpeedY = nw4r::math::FSelect(-sBaseParams[3] - m_baseSpeedY, -sBaseParams[3], m_baseSpeedY);
    m_baseY += m_baseSpeedY * deltaFrame;
    int mode = 0;
    if (m_arwingKind != 0 && m_arwingKind == 1) {
        mode = 1;
    }
    if (m_baseX > sBaseParams[9 + mode]) {
        m_baseAccelX = -sBaseParams[3];
    } else if (m_baseX < sBaseParams[5 + mode]) {
        m_baseAccelX = sBaseParams[3];
    }
    if (m_baseY > sBaseParams[11 + mode]) {
        m_baseAccelY = -sBaseParams[3];
    } else if (m_baseY < sBaseParams[7 + mode]) {
        m_baseAccelY = sBaseParams[3];
    }
    if (m_trainerBase != NULL) {
        grNodeCallbackData* data = m_trainerBase->m_calcWorldCallBack.m_nodeCallbackDatas;
        data->m_matrix.m[0][3] = m_baseX;
        data->m_matrix.m[1][3] = m_baseY;
        data->m_matrix.m[2][3] = -150.0f;
        for (int i = 0; i < 4; i++) {
            Matrix mtx(true);
            m_trainerBase->getNodeMatrix(&mtx, 0, m_trainerNode[i]);
            m_pokeTrainerPos[i].m_x = mtx.m[0][3];
            m_pokeTrainerPos[i].m_y = mtx.m[1][3];
            m_pokeTrainerPos[i].m_z = mtx.m[2][3];
        }
    }
}

// MATCH-ONLY: a word based view of the bit fields of soCollisionAttackData (from +0x30). The shared header declares its
// single-bit flags as bool, which the compiler accesses one byte at a time; the original sets all of them with one
// read-modify-write per word.
struct stDxCorneriaAttackBits {
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

// The hit box of an Arwing's beam.
void stDxCorneria::setBeamAAttack(grDxCorneriaBeam* beam) {
    beam->setAttack(0.5f, &sBeamAPos);
    beam->m_angle = sBeamAPos.m_x;
    soCollisionAttackData* attack = beam->getOverwriteAttackData();
    stDxCorneriaAttackBits* bits = reinterpret_cast<stDxCorneriaAttackBits*>(reinterpret_cast<u8*>(attack) + 0x30);
    bits->m_nodeIndex = 0;
    attack->m_power = 6;
    bits->m_attribute = 0;
    bits->m_targetSituation = 7;
    bits->m_targetCategory = 0x3FF;
    bits->m_targetLr = 0;
    bits->m_targetPart = 0xF;
    attack->m_vector = 10;
    attack->m_reactionEffect = 25;
    attack->m_reactionFix = 0;
    attack->m_reactionAdd = 10;
    attack->m_size = 0.5f;
    attack->m_offsetPos.m_x = sBeamAPos.m_x;
    attack->m_offsetPos.m_y = sBeamAPos.m_y;
    attack->m_offsetPos.m_z = sBeamAPos.m_z;
    bits->m_setOffKind = 0;
    bits->m_soundLevel = 0;
    bits->m_soundAttribute = 1;
    bits->m_noScale = 0;
    bits->m_isShieldable = 1;
    bits->m_isReflectable = 1;
    bits->m_isAbsorbable = 1;
    bits->m_isDirect = 0;
    bits->m_serialHitFrame = 16;
    bits->m_isInvalidInvincible = 0;
    bits->m_isInvalidXlu = 0;
    bits->m_lrCheck = 0;
    bits->m_isCatch = 0;
    bits->m_noTeam = 0;
    bits->m_noHitStop = 1;
    bits->m_noEffect = 0;
    bits->m_noTransaction = 0;
    bits->m_shapeType = soCollision::Shape_Capsule;
    beam->setAttackPreset(grMadein::Attack_Overwrite);
}

// The hit box of a shot of the Great Fox.
void stDxCorneria::setBeamGAttack(grDxCorneriaBeam* beam) {
    beam->setAttack(2.0f, &sBeamGPos);
    beam->m_reflectable = false;
    beam->m_angle = 1.5f * sBeamGPos.m_x;
    soCollisionAttackData* attack = beam->getOverwriteAttackData();
    stDxCorneriaAttackBits* bits = reinterpret_cast<stDxCorneriaAttackBits*>(reinterpret_cast<u8*>(attack) + 0x30);
    bits->m_nodeIndex = 0;
    bits->m_attribute = soCollisionAttackData::Attribute_Electric;
    bits->m_targetSituation = 7;
    attack->m_power = 30;
    bits->m_targetCategory = 0x3FF;
    bits->m_targetLr = 0;
    bits->m_targetPart = 0xF;
    attack->m_vector = 10;
    attack->m_reactionEffect = 80;
    attack->m_reactionFix = 0;
    attack->m_reactionAdd = 70;
    attack->m_size = 2.0f;
    attack->m_offsetPos.m_x = sBeamGPos.m_x;
    attack->m_offsetPos.m_y = sBeamGPos.m_y;
    attack->m_offsetPos.m_z = sBeamGPos.m_z;
    bits->m_setOffKind = soCollisionAttackData::SetOff_Thru;
    bits->m_soundLevel = soCollisionAttackData::Sound_Level_Large;
    bits->m_soundAttribute = soCollisionAttackData::Sound_Attribute_Fire;
    bits->m_noScale = 0;
    bits->m_isShieldable = 1;
    bits->m_isReflectable = 1;
    bits->m_isAbsorbable = 1;
    bits->m_isDirect = 0;
    bits->m_serialHitFrame = 16;
    bits->m_isInvalidInvincible = 0;
    bits->m_isInvalidXlu = 0;
    bits->m_lrCheck = 0;
    bits->m_isCatch = 0;
    bits->m_noTeam = 0;
    bits->m_noHitStop = 1;
    bits->m_noEffect = 0;
    bits->m_noTransaction = 0;
    bits->m_shapeType = soCollision::Shape_Capsule;
    beam->setAttackPreset(grMadein::Attack_Overwrite);
}

// The hit box of the Great Fox's cannon.
void stDxCorneria::setGFoxCanonAttack(grDxCorneria* ground) {
    ground->setAttack(4.0f, &sCanonPos);
    soCollisionAttackData* attack = ground->getOverwriteAttackData();
    stDxCorneriaAttackBits* bits = reinterpret_cast<stDxCorneriaAttackBits*>(reinterpret_cast<u8*>(attack) + 0x30);
    bits->m_nodeIndex = 0;
    attack->m_power = 30;
    bits->m_attribute = soCollisionAttackData::Attribute_Electric;
    bits->m_targetSituation = 7;
    bits->m_targetCategory = 0x3FF;
    bits->m_targetLr = 0;
    bits->m_targetPart = 0xF;
    attack->m_vector = 10;
    attack->m_reactionEffect = 80;
    attack->m_reactionFix = 0;
    attack->m_reactionAdd = 70;
    attack->m_size = 4.0f;
    attack->m_offsetPos.m_x = sCanonPos.m_x;
    attack->m_offsetPos.m_y = sCanonPos.m_y;
    attack->m_offsetPos.m_z = sCanonPos.m_z;
    bits->m_setOffKind = 0;
    bits->m_soundLevel = soCollisionAttackData::Sound_Level_Large;
    bits->m_soundAttribute = soCollisionAttackData::Sound_Attribute_Fire;
    bits->m_noScale = 0;
    bits->m_isShieldable = 1;
    bits->m_isReflectable = 0;
    bits->m_isAbsorbable = 0;
    bits->m_isDirect = 0;
    bits->m_serialHitFrame = 16;
    bits->m_isInvalidInvincible = 0;
    bits->m_isInvalidXlu = 0;
    bits->m_lrCheck = soCollisionAttackData::Lr_Check_Lr;
    bits->m_isCatch = 0;
    bits->m_noTeam = 0;
    bits->m_noHitStop = 1;
    bits->m_noEffect = 0;
    bits->m_noTransaction = 0;
    bits->m_shapeType = soCollision::Shape_Capsule;
    ground->setAttackPreset(grMadein::Attack_Overwrite);
}

grDxCorneria* grDxCorneria::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grDxCorneria* ground = new (Heaps::StageInstance) grDxCorneria(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->m_heapType = Heaps::StageInstance;
        ground->makeCalcuCallback(1, Heaps::StageInstance);
        ground->setCalcuCallbackRoot(7);
    }
    return ground;
}

grDxCorneria::~grDxCorneria() { }

grDxCorneriaBeam* grDxCorneriaBeam::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grDxCorneriaBeam* ground = new (Heaps::StageInstance) grDxCorneriaBeam(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->m_heapType = Heaps::StageInstance;
        ground->makeCalcuCallback(1, Heaps::StageInstance);
        ground->setCalcuCallbackRoot(7);
    }
    return ground;
}

grDxCorneriaBeam::~grDxCorneriaBeam() { }

// A fighter's hit changes the team of the beam: the beam is reflected back at the one who shot it.
void grDxCorneriaBeam::resetBeam() {
    if (m_yakumono != NULL) {
        m_yakumono->setTeam(16);
    }
    m_reflected = false;
    m_scale = 1.0f;
    m_hit = false;
}

void grDxCorneriaBeam::onInflict(soCollisionLog* collisionLog, u32 flags, float power) {
    Yakumono* yakumono = m_yakumono;
    if (yakumono != NULL) {
        if (flags & 8) {
            int oldTeam = yakumono->getTeam();
            yakumono->setTeam(collisionLog->m_teamNo);
            m_reflected = true;
            m_scale = m_scale * -1.0f;
            OSReport("Beam Reflect Old Team = %d New Team = %d\n", oldTeam, collisionLog->m_teamNo);
        } else if (m_reflectable) {
            m_hit = true;
        }
    }
}
