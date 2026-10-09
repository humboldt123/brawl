#include <OS/OSError.h>
#include <ai/ai_mgr.h>
#include <cm/cm_camera_controller.h>
#include <cm/cm_controller_ai.h>
#include <ec/ec_mgr.h>
#include <gf/gf_archive.h>
#include <gf/gf_model.h>
#include <gm/gm_global.h>
#include <gm/gm_lib.h>
#include <gr/gr_gimmick.h>
#include <gr/gr_gimmick_catapult.h>
#include <gr/gr_gimmick_spring.h>
#include <gr/gr_madein.h>
#include <it/it_manager.h>
#include <math.h>
#include <memory.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <snd/snd_id.h>
#include <snd/snd_system.h>
#include <st/st_class_info.h>
#include <st/st_collision_attr_param.h>
#include <st/st_melee.h>
#include <st/st_positions.h>
#include <st/st_trigger.h>
#include <stdio.h>
#include <string.h>
#include <types.h>

#include <st_tbreak/st_tbreak.h>

// Unnamed helpers of sora_melee the stage calls directly (see st_newpork.cpp).
// grMadein::setMatrix(Matrix*, bool): lets the ground (and optionally its attack) follow the matrix.
extern "C" void fn_27_279FC8(grMadein* ground, Matrix* matrix, bool followAttack);
// grMadein::switchToMatrix
extern "C" void fn_27_279FA4(grMadein* ground);

// MATCH-ONLY: the original reads the level number from a bit field of the melee init data (3 bits at +0xE).
struct stTargetBreakMeleeView {
    char _0[0xE];
    u8 level : 3;
    u8 pad : 5;
};

// The level table: a first word that the coroutine tag starts with, then the five levels (the last number of the
// belts, 100, is the same everywhere). HYPOTHESIS: only the use of the leading fields is known.
struct stTargetBreakStatics {
    int m_noTag;
    stTargetBreakLevel m_levels[5];
};
static stTargetBreakStatics sStatics = {
    0,
    {
        {1.0f, 9.6f, 20.0f, 5.0f, 0x0, true, {0, 0, 0}, -1, -1,
         {-1, -1, 0, -1, 0, -1, 0},
         {{361, 10, 50, 0, 100}, {361, 10, 50, 0, 100}, {361, 10, 50, 0, 100}, {361, 10, 50, 0, 100}}},
        {1.5f, 8.4f, 20.0f, 5.0f, 0x0, true, {0, 0, 0}, -1, -1,
         {-1, -1, 0, -1, 0, -1, 0},
         {{361, 10, 50, 0, 100}, {361, 10, 50, 0, 100}, {361, 10, 50, 0, 100}, {361, 10, 50, 0, 100}}},
        {2.0f, 7.2f, 20.0f, 5.0f, 0x2, true, {0, 0, 0}, 30, 32,
         {3, -1, 0, -1, 0, -1, 0},
         {{270, 10, 50, 0, 100}, {270, 10, 50, 0, 100}, {90, 10, 50, 0, 100}, {361, 10, 50, 0, 100}}},
        {2.5f, 6.0f, 20.0f, 5.0f, 0x1F2, false, {0, 0, 0}, -1, -1,
         {-1, -1, 0, -1, 0, -1, 0},
         {{361, 10, 50, 0, 100}, {361, 10, 50, 0, 100}, {361, 10, 50, 0, 100}, {361, 10, 50, 0, 100}}},
        {3.0f, 4.8f, 20.0f, 5.0f, 0x0, true, {0, 0, 0}, -1, -1,
         {-1, -1, 0, -1, 0, -1, 0},
         {{90, 10, 50, 0, 100}, {90, 10, 50, 0, 100}, {90, 10, 50, 0, 100}, {90, 10, 50, 0, 100}}},
    },
};

// MATCH-ONLY: the original copies the stage's damage floor from a constant image (a grGimmickDamageFloor: damage 10,
// vector 361, reaction 50 / 0 / 100, cut-up attribute, air and ground, 15 serial hit frames, medium sound level,
// cut-up sound, first index, any team).
struct stTargetBreakWords {
    u32 m_words[25];
};
static const u32 sDamageFloorImage[25] = {0x41200000, 0, 0, 0, 0, 0x169, 0x32, 0, 0x64, 0, 2, 0x101, 0,
                                          0, 0, 0xF, 1, 3, 0, 0, 0, 0xA, 0, 1, 0xFFFFFFFF};

// The items that are loaded with the stage and handed out at the nodes of the item model.
static stTargetBreakItem sItems[16] = {
    {"ItemNodeChewingBomb", (itKind)31, 0},   {"ItemNodeBeamSword", (itKind)4, 0},
    {"ItemNodePasaran", (itKind)40, 0},       {"ItemNodeSuperScope", (itKind)61, 0},
    {"ItemNodeUnira", (itKind)67, 0},         {"ItemNodeCrackerLauncher", (itKind)13, 0},
    {"ItemNodeSuperMushroom", (itKind)38, 0}, {"ItemNodePoisonousMushroom", (itKind)37, 0},
    {"ItemNodeBox", (itKind)7, 0},            {"ItemNodeRayGun", (itKind)44, 0},
    {"ItemNodeSmartBomb", (itKind)54, 0},     {"ItemNodeExplosiveBox", (itKind)43, 0},
    {"ItemNodeCarrierBox", (itKind)10, 0},    {"ItemNodeHomeRunBat", (itKind)33, 0},
    {"ItemNodeDeku", (itKind)18, 0},          {"ItemNodeCurry", (itKind)16, 0},
};

// MATCH-ONLY: the original scales the vector in place with paired singles (inline asm in the shared vector code).
static inline void tbreakScaleVec3f(register Vec3f* v, register float c) {
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

// MATCH-ONLY: passing the offset by value reproduces the original's temporary copies (same idiom as grGreenhillBreak).
static inline void tbreakAssignVec2f(Vec2f& dst, Vec2f src) {
    dst = src;
}

// MATCH-ONLY: the original builds this vector in a temporary and copies it.
struct stTargetBreakRaw3 {
    float x, y, z;
};
static inline void tbreakAssignVec3f(Vec3f& dst, float x, float y, float z) {
    stTargetBreakRaw3 tmp = {x, y, z};
    *reinterpret_cast<stTargetBreakRaw3*>(&dst) = tmp;
}

// Registers sound effect number `slot` of a gimmick: played at once, at the gimmick's origin, without a repeat.
#define TBREAK_SOUND(gimmick, slot, soundId)                                                   \
    (gimmick)->m_soundEffects[slot].m_id = (soundId);                                          \
    (gimmick)->m_soundEffects[slot].m_repeatFrame = 0;                                         \
    (gimmick)->m_soundEffects[slot].m_nodeIndex = 0;                                           \
    (gimmick)->m_soundEffects[slot].m_endFrame = 0;                                            \
    tbreakAssignVec2f((gimmick)->m_soundEffects[slot].m_offsetPos, Vec2f(0.0f, 0.0f))

// HYPOTHESIS: the coroutine markers of the level (see NEWPORK_SAVE in st_newpork.cpp): the stage saves the number of
// the source line to resume at together with a marker whose value is always 1.
#define TBREAK_SAVE(seq, lineNumber)         \
    do {                                     \
        static int sTag = 1;                 \
        (seq).m_tag = sTag;                  \
        (seq).m_line = (lineNumber);         \
    } while (0)

// HYPOTHESIS: two small constant objects (an 0xFF marker with an index) that the unit's headers instantiate.
struct stTargetBreakMarker {
    int m_a;
    int m_b;
    stTargetBreakMarker(int a, int b) : m_a(a), m_b(b) { }
};
static stTargetBreakMarker sMarkerA(0xFF, 0);
static stTargetBreakMarker sMarkerB(0xFF, 1);

stClassInfoImpl<Stages::TargetBreak, stTargetBreak> stTargetBreak::bss_loc_14;

stTargetBreak* stTargetBreak::create() {
    return new (Heaps::StageInstance) stTargetBreak;
}

stTargetBreak::stTargetBreak() : stMelee("stTargetBreak", Stages::TargetBreak) {
    m_seq.m_tag = sStatics.m_noTag;
    m_seq.m_line = 0;
    for (int i = 0; i < 16; i++) {
        itManager::getInstance()->preloadItemKindArchive(sItems[i].m_kind, sItems[i].m_variant, (itArchive::Type)0, true);
    }
    for (int i = 0; i < 4; i++) {
        m_beltData[i] = NULL;
    }
    m_catapultData = NULL;
    m_springData = NULL;
    setPlayerPositionIndexSerial();
}

stTargetBreak::~stTargetBreak() {
    for (int i = 0; i < 4; i++) {
        if (m_beltData[i] != NULL) {
            delete m_beltData[i];
        }
    }
    if (m_springData != NULL) {
        delete m_springData;
    }
    if (m_catapultData != NULL) {
        delete m_catapultData;
    }
    releaseArchive();
}

bool stTargetBreak::loading() {
    return true;
}

void stTargetBreak::createObj() {
    // The level (0-4) comes from the rules of the match.
    int levelRaw = reinterpret_cast<stTargetBreakMeleeView*>(g_GameGlobal->m_modeMelee)->level;
    int levelNumber = (levelRaw > 4) ? 4 : levelRaw;
    m_level = (levelNumber < 0) ? 0 : levelNumber;
    testStageParamInit(m_fileData, 10);
    const stTargetBreakLevel* level = &sStatics.m_levels[m_level];

    m_field = grTargetBreak::create(0, "", "grTargetBreakGround");
    if (m_field != NULL) {
        addGround(m_field);
        m_field->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        m_field->setStageData(m_stageData);
        m_field->initializeEntity();
        m_field->startEntityAutoLoop();
    }

    m_nodeModel = grTargetBreak::create(2, "", "grTargetNode");
    if (m_nodeModel != NULL) {
        addGround(m_nodeModel);
        m_nodeModel->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        m_nodeModel->setStageData(m_stageData);
        m_nodeModel->initializeEntity();
        m_nodeModel->startEntity();
    }

    if (m_level == 3) {
        m_iceField = grTargetBreak::create(3, "", "grTargetBreakGroundIce");
        if (m_iceField != NULL) {
            addGround(m_iceField);
            m_iceField->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
            m_iceField->setStageData(m_stageData);
            m_iceField->initializeEntity();
            m_iceField->startEntityAutoLoop();
        }
    }

    // The damage the floor does (to fighters that fall out of the level, HYPOTHESIS): power 10, angle 361.
    stTargetBreakWords damageFloorWords = *reinterpret_cast<const stTargetBreakWords*>(sDamageFloorImage);
    grGimmickDamageFloor* damageFloor = reinterpret_cast<grGimmickDamageFloor*>(&damageFloorWords);
    tbreakAssignVec3f(damageFloor->m_attackData.m_offsetPos, 0.0f, 0.0f, 0.0f);
    setStageAttackData(damageFloor, 0);

    // The conveyor belts of the level are marked by two nodes each.
    for (int i = 0; i < 4; i++) {
        m_beltData[i] = NULL;
        m_beltTrigger[i] = NULL;
        if (m_field != NULL) {
            char name[32];
            sprintf(name, "BeltConveyerNode%02dS", i + 1);
            m_beltNodeStart[i] = m_field->getNodeIndex(0, name);
            sprintf(name, "BeltConveyerNode%02dE", i + 1);
            m_beltNodeEnd[i] = m_field->getNodeIndex(0, name);
            if (m_beltNodeStart[i] != 0 && m_beltNodeEnd[i] != 0) {
                OSReport("Find BeltConveyerNode%02d\n", i + 1);
                m_beltData[i] = new (Heaps::StageInstance) grGimmickBeltConveyorData;
                memset(m_beltData[i], 0, sizeof(grGimmickBeltConveyorData));
            }
        } else {
            m_beltNodeStart[i] = 0;
            m_beltNodeEnd[i] = 0;
        }
    }

    // The ten targets.
    for (int i = 0; i < 10; i++) {
        grTargetBreak* target = grTargetBreak::create(1, "", "grTarget");
        if (target != NULL) {
            m_targets[i] = target;
            addGround(target);
            target->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
            target->setStageData(m_stageData);
            Vec3f hitStart(0.0f, 0.0f, -10.0f);
            Vec3f hitEnd(0.0f, 0.0f, 20.0f);
            target->setHitPoint(level->m_targetSize, &hitStart, &hitEnd, true, 0);
            target->markEnemy();
            target->initializeEntity();
            target->startEntity();
            fn_27_279FA4(target);
            target->thisIsTarget();
            m_targetHitBy[i] = -1;
        }
        m_targetTimer[i] = -1.0f;
    }
    m_chainRadiusSq = level->m_chainRadius * level->m_chainRadius;

    // The model with the nodes the items appear at (not in every level).
    if (level->m_hasItemNode) {
        m_itemNodeModel = grTargetBreak::create(3, "", "grItemNode");
        addGround(m_itemNodeModel);
        m_itemNodeModel->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        m_itemNodeModel->setStageData(m_stageData);
        m_itemNodeModel->initializeEntity();
        m_itemNodeModel->startEntity();
        m_springNode = m_itemNodeModel->getNodeIndex(0, "GimmickNodeSpring");
        OSReport("GimmickNodeSpring is %d\n", m_springNode);
    } else {
        m_itemNodeModel = NULL;
    }

    // Targets that move have a model of their own.
    for (int i = 0; i < 10; i++) {
        int modelIndex = 0x28 + i;
        if ((level->m_movingTargets & (1 << i)) && m_fileData->getData(Data_Type_Model, modelIndex, 0xFFFE)) {
            OSReport("create move target node %d\n", i);
            m_movingTargetNodes[i] = grTargetBreak::create((s16)modelIndex, "", "grMoveTargetNode");
            if (m_movingTargetNodes[i] != NULL) {
                addGround(m_movingTargetNodes[i]);
                m_movingTargetNodes[i]->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
                m_movingTargetNodes[i]->setStageData(m_stageData);
                m_movingTargetNodes[i]->initializeEntity();
                m_movingTargetNodes[i]->startEntityAutoLoop();
            }
        } else {
            m_movingTargetNodes[i] = NULL;
        }
    }

    // The steps that move along locators.
    if (m_fileData->getData(Data_Type_Model, 9, 0xFFFE)) {
        for (int i = 0; i < 12; i++) {
            int locatorIndex = 0xA + i;
            if (m_fileData->getData(Data_Type_Model, locatorIndex, 0xFFFE)) {
                OSReport("create locater%d\n", i);
                m_stepLocators[i] = grTargetBreak::create((s16)locatorIndex, "", "grStep");
                if (m_stepLocators[i] != NULL) {
                    addGround(m_stepLocators[i]);
                    m_stepLocators[i]->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
                    m_stepLocators[i]->setStageData(m_stageData);
                    m_stepLocators[i]->initializeEntity();
                    m_stepLocators[i]->startEntityAutoLoop();
                    char name[32];
                    sprintf(name, "MoveLocator%02d", i + 1);
                    m_stepLocatorNode[i] = m_stepLocators[i]->getNodeIndex(0, name);
                    OSReport("create step%d\n", i);
                    m_steps[i] = grTargetBreak::create(9, "", "grStep");
                    if (m_steps[i] != NULL) {
                        addGround(m_steps[i]);
                        m_steps[i]->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
                        m_steps[i]->setStageData(m_stageData);
                        m_steps[i]->initializeEntity();
                        m_steps[i]->startEntity();
                        fn_27_279FA4(m_steps[i]);
                        createCollision(m_fileData, 3, m_steps[i]);
                    } else {
                        m_steps[i] = NULL;
                    }
                } else {
                    m_steps[i] = NULL;
                }
            } else {
                m_steps[i] = NULL;
                m_stepLocators[i] = NULL;
            }
        }
    } else {
        for (int i = 0; i < 12; i++) {
            int stepIndex = 0xA + i;
            if (m_fileData->getData(Data_Type_Model, stepIndex, 0xFFFE)) {
                OSReport("create step%d\n", i);
                m_steps[i] = grTargetBreak::create((s16)stepIndex, "", "grStep");
                if (m_steps[i] != NULL) {
                    addGround(m_steps[i]);
                    m_steps[i]->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
                    m_steps[i]->setStageData(m_stageData);
                    m_steps[i]->initializeEntity();
                    m_steps[i]->startEntityAutoLoop();
                }
                m_stepLocators[i] = NULL;
            } else {
                m_steps[i] = NULL;
                m_stepLocators[i] = NULL;
            }
        }
        OSReport("create step end\n");
    }

    // The catapult (only in some levels).
    if (level->m_catapult >= 0) {
        grGimmickCatapultData* catapultData = new (Heaps::StageInstance) grGimmickCatapultData;
        m_catapultData = catapultData;
        memset(catapultData, 0, sizeof(grGimmickCatapultData));
        catapultData->m_motionPathData.m_motionRatio = 5.4f;
        catapultData->m_motionPathData.m_index = 0;
        catapultData->m_motionPathData.m_mdlIndex = level->m_catapult + 1;
        catapultData->m_motionPathData.m_7 = 0;
        catapultData->m_areaData.m_offsetPos.m_x = 0.0f;
        catapultData->m_areaData.m_offsetPos.m_y = 3.0f;
        catapultData->m_areaData.m_range.m_x = 10.0f;
        catapultData->m_areaData.m_range.m_y = 5.0f;
        catapultData->m_startMoveFrame = 30.0f;
        catapultData->m_52 = 60.0f;
        catapultData->m_56 = 1.0f;
        catapultData->m_vector = 165.0f;
        catapultData->m_mdlIndex = level->m_catapult;
        catapultData->m_isFaceLeft = true;
        catapultData->m_useNoHelperWarp = true;
        m_catapult = createGimmickCatapult(catapultData, m_fileData);
        TBREAK_SOUND(m_catapult, 0, snd_se_Target_Catapult_on);
        TBREAK_SOUND(m_catapult, 1, snd_se_Target_Catapult_start);
        TBREAK_SOUND(m_catapult, 2, snd_se_Target_Catapult_shoot);
    }

    // The trampoline (only in some levels).
    if (level->m_spring >= 0) {
        grGimmickSpringData* springData = new (Heaps::StageInstance) grGimmickSpringData;
        m_springData = springData;
        memset(springData, 0, sizeof(grGimmickSpringData));
        springData->m_areaData.m_offsetPos.m_x = 0.0f;
        springData->m_areaData.m_offsetPos.m_y = 0.0f;
        springData->m_areaData.m_range.m_x = 10.0f;
        springData->m_areaData.m_range.m_y = 6.0f;
        springData->m_pos.m_x = 0.0f;
        springData->m_pos.m_y = 0.0f;
        springData->m_rot = 0.0f;
        springData->m_bounce = 5.0f;
        springData->m_mdlIndex = level->m_spring;
        springData->m_collIndex = level->m_unk20[0];
        grGimmickTargetBreakSpring* spring = new (Heaps::StageInstance) grGimmickTargetBreakSpring("TBSprintg");
        spring->setMdlIndex(level->m_spring);
        m_spring = spring;
        addGround(spring);
        spring->setGimmickData(springData);
        spring->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        createGimmickCollision(springData->m_collIndex, spring, m_fileData);
    }

    // The last level has a moving block.
    if (m_level == 4) {
        m_block = grTargetBreak::create(8, "", "grMoveBlock");
        if (m_block != NULL) {
            addGround(m_block);
            m_block->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
            m_block->setStageData(m_stageData);
            m_block->initializeEntity();
            m_block->startEntityAutoLoop();
        }
    } else {
        m_block = NULL;
    }

    createCollision(m_fileData, 2, NULL);
    initCameraParam();
    void* posData = m_fileData->getData(Data_Type_Model, 100, 0xFFFE);
    if (posData) {
        nw4r::g3d::ResFile posFile(posData);
        createStagePositions(&posFile);
    } else {
        createStagePositions();
    }
    createWind2ndOnly();
    loadStageAttrParam(m_fileData, 50);
    initPosPokeTrainer(1, 0);
    createObjPokeTrainer(m_fileData, 101, "PokeTrainer00", m_pokeTrainerPos, NULL);
    registScnAnim(static_cast<nw4r::g3d::ResFileData*>(m_fileData->getData(Data_Type_Scene, 0, 0xFFFE)), 0);
    m_hitCount = 0;
    m_targetsLeft = 10;
    m_damageTotal = 0.0f;
    m_hitsBy[0] = 0;
    m_hitsBy[1] = 0;
    m_seq.m_tag = sStatics.m_noTag;
    m_seq.m_line = 0;
    *((u8*)g_cmAIController + 0x94) &= ~8; // HYPOTHESIS: clears a camera flag of the AI controller
}

// Break the targets one after the other: a target that was hit (or whose chain-reaction timer ran out) breaks, adds the
// damage to the total and sets the timer of every target within the chain radius.
void stTargetBreak::update(float deltaFrame) {
    const stTargetBreakLevel* level = &sStatics.m_levels[m_level];
    for (int i = 0; i < 10; i++) {
        if (m_targets[i] == NULL) {
            continue;
        }
        if (0.0f <= m_targetTimer[i]) {
            m_targetTimer[i] -= deltaFrame;
            if (m_targetTimer[i] < 0.0f) {
                m_targetTimer[i] = 0.0f;
            }
            OSReport("%d _nChainCrush=%f\n", i, m_targetTimer[i]);
        }
        if (m_targets[i]->wasHit() || m_targetTimer[i] == 0.0f) {
            OSReport("Target%d is broken.\n", i);
            if (m_targets[i]->wasHit()) {
                m_damageTotal += m_targets[i]->getLastDamage();
                m_targetHitBy[i] = m_targets[i]->getLastAttacker();
            } else {
                m_damageTotal += 30.0f;
            }
            if (m_targetHitBy[i] <= 1) {
                m_hitsBy[m_targetHitBy[i]] += 1;
            }
            Vec3f breakPos;
            m_targets[i]->m_calcWorldCallBack.m_nodeCallbackDatas[0].m_matrix.getPosition(&breakPos);
            m_targets[i]->setVisibility(0);
            m_targets[i]->deleteHitPoint();
            m_targets[i]->targetBroken();
            g_ecMgr->setEffect((EfID)(0x12B0001 + m_level), &breakPos);
            m_hitCount += 1;
            m_targetsLeft -= 1;
            m_targets[i] = NULL;
            // Chain reaction: every target that is still whole and close enough breaks after a while.
            for (int j = 0; j < 10; j++) {
                if (m_targets[j] != NULL && m_targetTimer[j] < 0.0f) {
                    OSReport("  ChainCheck %d", j);
                    Vec3f otherPos;
                    m_targets[j]->m_calcWorldCallBack.m_nodeCallbackDatas[0].m_matrix.getPosition(&otherPos);
                    Vec3f diff = otherPos - breakPos;
                    float distSq = diff.m_x * diff.m_x + diff.m_y * diff.m_y + diff.m_z * diff.m_z;
                    if (distSq <= m_chainRadiusSq) {
                        m_targetTimer[j] = level->m_chainDelay;
                        m_targetHitBy[j] = m_targetHitBy[i];
                        OSReport("  HIT!!!");
                    }
                    OSReport("\n");
                }
            }
        } else {
            // The target follows its node of the level (or of its own moving node model).
            char nodeName[32];
            sprintf(nodeName, "TargetNode%02d", i + 1);
            if (level->m_movingTargets & (1 << i)) {
                if (m_movingTargetNodes[i] != NULL) {
                    Matrix nodeMtx(true);
                    m_movingTargetNodes[i]->getNodeMatrix(&nodeMtx, 0, nodeName);
                    nodeMtx(2, 3) = -10.0f;
                    fn_27_279FC8(m_targets[i], &nodeMtx, true);
                }
            } else if (m_nodeModel != NULL) {
                Matrix nodeMtx(true);
                m_nodeModel->getNodeMatrix(&nodeMtx, 0, nodeName);
                nodeMtx(2, 3) = -10.0f;
                fn_27_279FC8(m_targets[i], &nodeMtx, true);
            }
        }
    }

    // The steps follow their locators.
    for (int i = 0; i < 12; i++) {
        if (m_stepLocators[i] != NULL) {
            Matrix locatorMtx(true);
            m_stepLocators[i]->getNodeMatrix(&locatorMtx, 0, m_stepLocatorNode[i]);
            fn_27_279FC8(m_steps[i], &locatorMtx, true);
        }
    }

    // The level's coroutine: set up the belts, wait, hand out the items.
    switch (m_seq.m_line) {
        case 0:
            TBREAK_SAVE(m_seq, 0x3bb);
            // FALL-THROUGH
        case 0x3bb:
            TBREAK_SAVE(m_seq, 0x3bd);
            break;
        case 0x3bd:
            TBREAK_SAVE(m_seq, 0x3be);
            break;
        case 0x3be: {
            for (int i = 0; i < 4; i++) {
                if (m_field != NULL && m_beltData[i] != NULL) {
                    Matrix nodeMtx(true);
                    m_field->getNodeMatrix(&nodeMtx, 0, m_beltNodeStart[i]);
                    Vec3f startPos;
                    nodeMtx.getPosition(&startPos);
                    m_field->getNodeMatrix(&nodeMtx, 0, m_beltNodeEnd[i]);
                    Vec3f endPos;
                    nodeMtx.getPosition(&endPos);
                    Vec3f center = startPos + endPos;
                    tbreakScaleVec3f(&center, 0.5f);
                    Vec3f length = endPos - startPos;
                    m_beltData[i]->m_pos.m_x = center.m_x;
                    m_beltData[i]->m_pos.m_y = center.m_y;
                    m_beltData[i]->m_pos.m_z = center.m_z;
                    m_beltData[i]->m_speed = 1.0f;
                    m_beltData[i]->m_isRight = true;
                    m_beltData[i]->m_areaData.m_offsetPos.m_x = 0.0f;
                    m_beltData[i]->m_areaData.m_offsetPos.m_y = 0.0f;
                    m_beltData[i]->m_areaData.m_range.m_x = (float)fabs(length.m_x);
                    m_beltData[i]->m_areaData.m_range.m_y = (m_level == 0) ? (float)fabs(length.m_y) : 10.0f;
                    m_beltTrigger[i] = g_stTriggerMng->createTrigger(Gimmick::Area_BeltConveyor, -1);
                    m_beltTrigger[i]->setBeltConveyorTrigger(m_beltData[i]);
                }
            }
            if (level->m_spring >= 0) {
                Matrix springMtx(true);
                m_itemNodeModel->getNodeMatrix(&springMtx, 0, m_springNode);
                Vec3f springPos;
                springMtx.getPosition(&springPos);
                m_spring->setPos(&springPos);
            }
            TBREAK_SAVE(m_seq, 0x419);
            break;
        }
        case 0x419:
            for (int i = 0; i < 4; i++) {
                if (m_field != NULL && m_beltData[i] != NULL) {
                    m_beltTrigger[i]->setAreaSleep(false);
                }
            }
            // FALL-THROUGH
        case 0x42a:
            if (putItem()) {
                m_itemNodeModel = NULL;
                TBREAK_SAVE(m_seq, 0x42d);
            } else {
                TBREAK_SAVE(m_seq, 0x42a);
            }
            break;
    }
}

// Waits until all the items are loaded, then puts every one of them at its node of the item model. Returns true when
// they are out (or when the level has none).
bool stTargetBreak::putItem() {
    if (m_itemNodeModel != NULL) {
        for (int i = 0; i < 16; i++) {
            if (!itManager::getInstance()->isCompItemKindArchive(sItems[i].m_kind, sItems[i].m_variant, true)) {
                return false;
            }
        }
        for (int i = 0; i < 16; i++) {
            Matrix nodeMtx(true);
            if (m_itemNodeModel->getNodeMatrix(&nodeMtx, 0, sItems[i].m_nodeName)) {
                BaseItem* item = itManager::getInstance()->createItem(sItems[i].m_kind, sItems[i].m_variant);
                if (item != NULL) {
                    Vec3f pos;
                    nodeMtx.getPosition(&pos);
                    item->warp(&pos);
                    item->setVanishMode(false);
                }
            }
        }
    }
    return true;
}

grGimmickTargetBreakSpring::~grGimmickTargetBreakSpring() { }

void grGimmickTargetBreakSpring::setMotionOff() {
    m_modelAnims[0]->unbindShapeAnim(m_sceneModels[0]);
    changeNodeAnim(0, 0);
    changeShapeAnim(0, 0);
    g_sndSystem->playSE(snd_se_Target_Spring, -1, 0, 0, -1);
    // MATCH-ONLY: the original stores the state with a plain byte store instead of a bitfield insert.
    reinterpret_cast<u8*>(this)[0x158] = State_Off;
    m_animFrame = 0.0f;
}

grTargetBreak* grTargetBreak::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grTargetBreak* ground = new (Heaps::StageInstance) grTargetBreak(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->m_heapType = Heaps::StageInstance;
        ground->makeCalcuCallback(1, Heaps::StageInstance);
        ground->setCalcuCallbackRoot(7);
    }
    return ground;
}

grTargetBreak::~grTargetBreak() { }

void grTargetBreak::thisIsTarget() {
    createSoundWork(1, 1);
    TBREAK_SOUND(this, 0, snd_se_Target_Break);
}

void grTargetBreak::targetBroken() {
    startGimmickSE(0);
}

// HYPOTHESIS: the hit is only written down for the stage: the attacker is unknown, the damage, the side and the
// hit position are kept, and the hit flag is set.
void grTargetBreak::onDamage(int index, soDamage* damage, soDamageAttackerInfo* attackerInfo) {
    m_hitPointInfo->m_lastPlayerHit = -1;
    m_hitPointInfo->m_lastDamageTaken = damage->m_damageAdd;
    m_hitPointInfo->m_lastSide = damage->m_lr;
    *reinterpret_cast<Vec3f*>(reinterpret_cast<u8*>(m_hitPointInfo) + 0x18) = damage->m_pos;
    *reinterpret_cast<Vec2f*>(reinterpret_cast<u8*>(m_hitPointInfo) + 0x30) = damage->m_speed;
    m_isHit = true;
}
