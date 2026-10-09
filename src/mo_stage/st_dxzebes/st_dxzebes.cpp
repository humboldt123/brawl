#include <cm/cm_camera_controller.h>
#include <cm/cm_quake.h>
#include <ec/ec_mgr.h>
#include <ef/ef_screen.h>
#include <gf/gf_3d_scene.h>
#include <gf/gf_archive.h>
#include <gm/gm_global.h>
#include <gm/gm_lib.h>
#include <gr/collision/gr_collision.h>
#include <gr/gr_madein.h>
#include <memory.h>
#include <mt/mt_common.h>
#include <mt/mt_prng.h>
#include <nw4r/math/math_arithmetic.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <snd/snd_id.h>
#include <st/st_class_info.h>
#include <st/st_melee.h>
#include <string.h>
#include <types.h>

#include <st_dxzebes/st_dxzebes.h>

// Unnamed helpers of other modules the stage calls directly.
// efScreen: ends a fill requested with requestFill (HYPOTHESIS: fades it out over `frames`).
extern "C" void fn_8005E3C8(efScreen* screen, efScreenHandle* handle, float frames);
// stCollisionWork: releases the collision lines the work owns (HYPOTHESIS).
extern "C" void fn_27_239F6C(stCollisionWork* work);
// Stage: HYPOTHESIS: true while the stage is the active one (not the one used for the menu preview).
extern "C" int fn_27_2249E0(Stage* stage);
// grMadein: HYPOTHESIS: switches the looping flag of the ground's motion.
extern "C" void fn_27_279228(grMadein* ground, int loop);

// MATCH-ONLY: the original reads the stage the match is played on (6 bits at +0x8 of the melee data) and its number.
struct stDxZebesMeleeView {
    char _0[8];
    u8 mode : 6;
    u8 pad : 2;
    char _9[7];
    u8 stage;
};

// MATCH-ONLY: the flag byte of a collision joint at +0x54 (the shared header only names the bits of the word at +0x48).
struct stDxZebesJointView {
    char _0[0x54];
    bool m_bit80 : 1;
    bool m_bit40 : 1;
    bool m_bit20 : 1;
    bool m_bit10 : 1;
    u8 _rest : 4;
};

// MATCH-ONLY: a word based view of the bit fields of soCollisionAttackData (from +0x30). The shared header declares its
// single-bit flags as bool, which the compiler accesses one byte at a time; the original sets all of them with one
// read-modify-write per word.
struct stDxZebesAttackBits {
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

// The frames the acid stays at one level before it moves to the next one (see grZebesWaterProc): the base, a spare,
// the random extra and the height the level is around. A line of zeros ends the table. HYPOTHESIS: names follow their
// use in grZebesWaterInit.
struct stDxZebesAcidLevel {
    s16 m_wait;
    s16 _2;
    s16 m_waitRandom;
    s16 m_height;
};
static stDxZebesAcidLevel sAcidLevels[19] = {
    {300, 0, 800, -140},   {180, 0, 180, -10},    {90, 0, 180, -66},    {60, 60, 70, 60},   {240, 0, 30, -120},
    {60, 0, 30, 86},       {40, 0, 0, 0},         {300, 0, 180, -200},  {100, 0, 200, -60}, {300, 0, 180, -128},
    {200, 0, 100, 38},     {120, 0, 0, 48},       {300, 0, 120, -140},  {120, 0, 0, 64},    {40, 0, 0, 40},
    {40, 0, 0, 20},        {40, 0, 0, 0},         {160, 0, 0, -250},    {0, 0, 0, 0},
};

// HYPOTHESIS: inline clamp helper (fsel based), the same one the glide statuses use.
static inline float zebesClamp(float value, float lo, float hi) {
    value = nw4r::math::FSelect(value - lo, value, lo);
    return nw4r::math::FSelect(value - hi, hi, value);
}

stDxZebes* stDxZebes::create() {
    return new (Heaps::StageInstance) stDxZebes;
}

stDxZebes::stDxZebes() : stMelee("stDxZebes", Stages::DxZebes) {
    reinterpret_cast<s8&>(m_screenHandle) = -1;
    memset(m_cells, 0, sizeof(m_cells));
    memset(m_shaft, 0, sizeof(m_shaft));
    memset(&m_acidStep, 0, 0x18);
    memset(&m_mapState, 0, 0x14);
    memset(&m_bgState, 0, 2);
    m_flagStart = false;
    m_flagFade = true;
    m_motionToggle = 0;
    m_fadeTimer = 500.0f;
    reinterpret_cast<s8&>(m_screenHandle) = -1;
    m_sourceA[0].m_x = -40.5f;
    m_sourceA[0].m_y = -10.0f;
    m_sourceA[0].m_z = 13.0f;
    m_sourceA[1].m_x = -40.5f;
    m_sourceA[1].m_y = -24.0f;
    m_sourceA[1].m_z = 13.0f;
    m_sourceA[2].m_x = -5.5f;
    m_sourceA[2].m_y = -13.0f;
    m_sourceA[2].m_z = 23.5f;
    m_sourceA[3].m_x = -5.5f;
    m_sourceA[3].m_y = -27.0f;
    m_sourceA[3].m_z = 23.5f;
    m_sourceA[4].m_x = -5.5f;
    m_sourceA[4].m_y = 0.5f;
    m_sourceA[4].m_z = 0.0f;
    m_sourceA[5].m_x = -5.5f;
    m_sourceA[5].m_y = -10.0f;
    m_sourceA[5].m_z = 0.0f;
    m_sourceA[6].m_x = -8.0f;
    m_sourceA[6].m_y = 4.5f;
    m_sourceA[6].m_z = 32.0f;
    m_sourceA[7].m_x = -8.0f;
    m_sourceA[7].m_y = -14.5f;
    m_sourceA[7].m_z = 32.0f;
    m_sourceA[8].m_x = -34.0f;
    m_sourceA[8].m_y = 0.5f;
    m_sourceA[8].m_z = 24.0f;
    m_sourceA[9].m_x = -34.0f;
    m_sourceA[9].m_y = -17.5f;
    m_sourceA[9].m_z = 24.0f;
    m_sourceA[10].m_x = -23.5f;
    m_sourceA[10].m_y = -12.0f;
    m_sourceA[10].m_z = 5.5f;
    m_sourceA[11].m_x = -23.5f;
    m_sourceA[11].m_y = -23.0f;
    m_sourceA[11].m_z = 5.5f;
    m_sourceA[12].m_x = 6.5f;
    m_sourceA[12].m_y = 4.0f;
    m_sourceA[12].m_z = 18.0f;
    m_sourceA[13].m_x = 6.5f;
    m_sourceA[13].m_y = -10.5f;
    m_sourceA[13].m_z = 18.0f;
    m_sourceA[14].m_x = 10.0f;
    m_sourceA[14].m_y = 0.5f;
    m_sourceA[14].m_z = 9.5f;
    m_sourceA[15].m_x = 10.0f;
    m_sourceA[15].m_y = -10.0f;
    m_sourceA[15].m_z = 9.5f;
    m_sourceA[16].m_x = -21.5f;
    m_sourceA[16].m_y = 2.0f;
    m_sourceA[16].m_z = 15.5f;
    m_sourceA[17].m_x = -21.5f;
    m_sourceA[17].m_y = -16.0f;
    m_sourceA[17].m_z = 15.5f;
    m_sourceA[18].m_x = -59.0f;
    m_sourceA[18].m_y = 27.5f;
    m_sourceA[18].m_z = 6.0f;
    m_sourceA[19].m_x = -59.0f;
    m_sourceA[19].m_y = 17.0f;
    m_sourceA[19].m_z = 6.0f;
    m_sourceB[0].m_x = 42.5f;
    m_sourceB[0].m_y = -11.0f;
    m_sourceB[0].m_z = 0.0f;
    m_sourceB[1].m_x = 42.5f;
    m_sourceB[1].m_y = -25.0f;
    m_sourceB[1].m_z = 0.0f;
    m_sourceB[2].m_x = 30.0f;
    m_sourceB[2].m_y = 1.0f;
    m_sourceB[2].m_z = 11.0f;
    m_sourceB[3].m_x = 30.0f;
    m_sourceB[3].m_y = -17.0f;
    m_sourceB[3].m_z = 11.0f;
    m_sourceB[4].m_x = 45.0f;
    m_sourceB[4].m_y = -5.0f;
    m_sourceB[4].m_z = 7.5f;
    m_sourceB[5].m_x = 45.0f;
    m_sourceB[5].m_y = -14.0f;
    m_sourceB[5].m_z = 7.5f;
    m_sourceB[6].m_x = 56.0f;
    m_sourceB[6].m_y = 15.0f;
    m_sourceB[6].m_z = 6.0f;
    m_sourceB[7].m_x = 56.0f;
    m_sourceB[7].m_y = 4.5f;
    m_sourceB[7].m_z = 6.0f;
    m_cameraMinY = 0.0f;
    m_cameraInit = 1;
}

stDxZebes::~stDxZebes() {
    releaseArchive();
    fn_27_239F6C(&m_collision);
    if (m_screenHandle.isValid() == 1) {
        efScreenHandle handle = m_screenHandle;
        fn_8005E3C8(g_efScreen, &handle, 1.0f);
        reinterpret_cast<s8&>(m_screenHandle) = -1;
    }
}

bool stDxZebes::loading() {
    return true;
}

#define ZEBES_GROUND(index) static_cast<grMadein*>(getGround(index))

void stDxZebes::createObj() {
    testStageParamInit(m_fileData, 10);
    stDxZebesMeleeView* melee = reinterpret_cast<stDxZebesMeleeView*>(g_GameGlobal->m_modeMelee);
    if (melee->mode == 7 && melee->stage == 0x2A) {
        m_flagStart = true;
    }
    m_collision.initialize();
    m_collision.m_vtxLen = 6;
    m_collision.m_isClosed = false;

    addGround(grMadein::create(0, "", "acid", Heaps::StageInstance));
    addGround(grMadein::create(1, "", "ashital1", Heaps::StageInstance));
    addGround(grMadein::create(2, "", "ashital2", Heaps::StageInstance));
    addGround(grMadein::create(3, "", "ashitar1", Heaps::StageInstance));
    addGround(grMadein::create(4, "", "ashitar2", Heaps::StageInstance));
    addGround(grMadein::create(5, "", "ashitmove", Heaps::StageInstance));
    addGround(grMadein::create(6, "", "birdman", Heaps::StageInstance));
    addGround(grMadein::create(8, "", "enkei", Heaps::StageInstance));
    addGround(grMadein::create(9, "", "topfloor", Heaps::StageInstance));
    addGround(grMadein::create(0xC, "", "ashital3", Heaps::StageInstance));
    addGround(grMadein::create(0xD, "", "ashitar3", Heaps::StageInstance));

    // The acid hurts fighters that touch it.
    Vec3f attackPos;
    attackPos.m_y = 0.0f;
    attackPos.m_x = 600.0f;
    attackPos.m_z = 0.0f;
    grMadein* acidAttack = grMadein::create(0xF, "", "acid attack", Heaps::StageInstance);
    addGround(acidAttack);
    acidAttack->setAttack(15.0f, &attackPos);
    acidAttack->setAttackPreset(grMadein::Attack_Overwrite);
    soCollisionAttackData* attack = acidAttack->getOverwriteAttackData();
    attack->m_power = 0xE;
    attack->m_size = 16.0f;
    attack->m_vector = 90;
    attack->m_reactionEffect = 0x23;
    attack->m_reactionFix = 0;
    attack->m_reactionAdd = 110;
    attack->m_offsetPos.m_x = attackPos.m_x;
    attack->m_offsetPos.m_y = attackPos.m_y;
    attack->m_offsetPos.m_z = attackPos.m_z;
    stDxZebesAttackBits* bits = reinterpret_cast<stDxZebesAttackBits*>(reinterpret_cast<u8*>(attack) + 0x30);
    bits->m_nodeIndex = 0;
    bits->m_attribute = soCollisionAttackData::Attribute_Fire;
    bits->m_targetSituation = 7;
    bits->m_targetCategory = 0x3FF;
    bits->m_targetLr = 0;
    bits->m_targetPart = 0xF;
    bits->m_setOffKind = soCollisionAttackData::SetOff_Off;
    bits->m_noScale = 0;
    bits->m_soundLevel = soCollisionAttackData::Sound_Level_Medium;
    bits->m_soundAttribute = soCollisionAttackData::Sound_Attribute_Fire;
    bits->m_isShieldable = 0;
    bits->m_isReflectable = 0;
    bits->m_isAbsorbable = 0;
    bits->m_isDirect = 0;
    bits->m_serialHitFrame = 60;
    bits->m_isInvalidInvincible = 0;
    bits->m_isInvalidXlu = 0;
    bits->m_lrCheck = soCollisionAttackData::Lr_Check_Lr;
    bits->m_isCatch = 0;
    bits->m_noTeam = 0;
    bits->m_noHitStop = 0;
    bits->m_noEffect = 0;
    bits->m_noTransaction = 0;
    bits->m_shapeType = soCollision::Shape_Capsule;

    addGround(grMadein::create(0xF, "", "ShaftHitL", Heaps::StageInstance));
    addGround(grMadein::create(0xF, "", "ShaftHitR", Heaps::StageInstance));
    addGround(grMadein::create(0xF, "nodeIndex", "nodeIndex", Heaps::StageInstance));
    addGround(grMadein::create(0xE, "", "PTashiba", Heaps::StageInstance));
    createCollisionSelf(&m_collision, getGround(0xE), "nodeIndex", "nodeIndex", 0x408);

    // The cells of the organism.
    for (int i = 0; i < 20; i++) {
        m_cells[i].m_ground = grMadein::create(7, "", "cell", Heaps::StageInstance);
        addGround(m_cells[i].m_ground);
        Vec3f hitPos(0.0f, 0.0f, 0.0f);
        if (i != 0 && i != 6) {
            m_cells[i].m_ground->setHitPoint(3.0f, &hitPos, &hitPos, true, 0);
        }
    }

    // The two shafts on the sides are hit with a bar.
    Vec3f hitStart(-51.4f, 18.0f, 0.0f);
    Vec3f hitEnd(-51.4f, 0.0f, 0.0f);
    Vec3f zero(0.0f, 0.0f, 0.0f);
    ZEBES_GROUND(0xC)->setHitPoint(1.5f, &hitStart, &hitEnd, true, 0);
    ZEBES_GROUND(0xC)->setPos(&zero);
    ZEBES_GROUND(9)->makeCalcuCallback(3, Heaps::StageInstance);
    {
        grMadein* shaft = ZEBES_GROUND(9);
        shaft->m_calcWorldCallBack.m_nodeCallbackDatas[0].m_nodeIndex = 0;
        shaft->m_calcWorldCallBack.m_nodeCallbackDatas[0].m_flags = 1;
        shaft->m_calcWorldCallBack.m_nodeCallbackDatas[1].m_nodeIndex = 3;
        shaft->m_calcWorldCallBack.m_nodeCallbackDatas[1].m_flags = 0x10;
        shaft->m_calcWorldCallBack.m_nodeCallbackDatas[2].m_nodeIndex = 5;
        shaft->m_calcWorldCallBack.m_nodeCallbackDatas[2].m_flags = 0x10;
    }
    hitStart.m_x = 51.4f;
    hitStart.m_y = 14.0f;
    hitStart.m_z = 0.0f;
    hitEnd.m_x = 51.4f;
    hitEnd.m_y = -6.0f;
    hitEnd.m_z = 0.0f;
    zero.m_x = 0.0f;
    zero.m_y = 0.0f;
    zero.m_z = 0.0f;
    ZEBES_GROUND(0xD)->setHitPoint(1.5f, &hitStart, &hitEnd, true, 0);
    ZEBES_GROUND(0xD)->setPos(&zero);
    ZEBES_GROUND(0xA)->makeCalcuCallback(3, Heaps::StageInstance);
    {
        grMadein* shaft = ZEBES_GROUND(0xA);
        shaft->m_calcWorldCallBack.m_nodeCallbackDatas[0].m_nodeIndex = 0;
        shaft->m_calcWorldCallBack.m_nodeCallbackDatas[0].m_flags = 1;
        shaft->m_calcWorldCallBack.m_nodeCallbackDatas[1].m_nodeIndex = 3;
        shaft->m_calcWorldCallBack.m_nodeCallbackDatas[1].m_flags = 0x10;
        shaft->m_calcWorldCallBack.m_nodeCallbackDatas[2].m_nodeIndex = 5;
        shaft->m_calcWorldCallBack.m_nodeCallbackDatas[2].m_flags = 0x10;
    }

    u32 groundNum = getGroundNum();
    for (u32 i = 0; i != groundNum; i++) {
        Ground* ground = getGround(i);
        if (ground != NULL) {
            ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
            static_cast<grMadein*>(ground)->initializeEntity();
        }
    }
    createCollision(m_fileData, 0, NULL);
    createCollision(m_fileData, 1, NULL);
    createCollision(m_fileData, 2, NULL);
    createCollision(m_fileData, 3, NULL);
    createCollision(m_fileData, 4, NULL);
    for (int i = 0; i < 16; i++) {
        if (i == 0 || i == 1 || i == 3 || (u32)(i - 7) <= 1) {
            ZEBES_GROUND(i)->startEntityAutoLoop();
        } else {
            ZEBES_GROUND(i)->startEntity();
        }
    }

    Vec3f platformPos(10.7f, 10.7f, -150.0f);
    m_cameraFloor = platformPos.m_x;
    ZEBES_GROUND(0xF)->setPos(&platformPos);
    if (fn_27_2249E0(this) == 0) {
        ZEBES_GROUND(0xF)->endEntity();
    }
    initCameraParam();
    void* posData = m_fileData->getData(Data_Type_Model, 10, 0xFFFE);
    if (posData) {
        nw4r::g3d::ResFile posFile(posData);
        createStagePositions(&posFile);
    } else {
        createStagePositions();
    }
    createWind2ndOnly();
    registScnAnim(static_cast<nw4r::g3d::ResFileData*>(m_fileData->getData(Data_Type_Scene, 0, 0xFFFE)), 0);

    m_shaft[0].m_state = 0xFF;
    m_shaft[0].m_nextState = 0;
    m_shaft[0].m_timer = 0;
    m_shaft[0].m_base = 0.0f;
    m_shaft[0].m_offset = 0.0f;
    m_shaft[0].m_velocity = 0.0f;
    m_shaft[0].m_strength = 0.0f;
    m_shaft[1].m_state = 0xFF;
    m_shaft[1].m_nextState = 0;
    m_shaft[1].m_timer = 0;
    m_shaft[1].m_base = 0.0f;
    m_shaft[1].m_offset = 0.0f;
    m_shaft[1].m_velocity = 0.0f;
    m_shaft[1].m_strength = 0.0f;
    m_mapState = 0;
    m_lastShaftState = 0;
    m_cellRespawnTimer = 0;
    m_bgState = 0;
    grZebesCellInit();
    grZebesWaterInit();
    m_eventShaft.set(1200.0f, 2400.0f);
    m_eventPhase.set(3000.0f, 3600.0f);
    m_eventPhase.start();
    m_lineA.m_source = m_sourceA;
    m_lineA.m_count = 10;
    for (int i = 0; i < 10; i++) {
        m_lineA.m_effects[i] = -1;
    }
    m_lineB.m_source = m_sourceB;
    m_lineB.m_count = 4;
    for (int i = 0; i < 10; i++) {
        m_lineB.m_effects[i] = -1;
    }
    loadStageAttrParam(m_fileData, 30);
    initPosPokeTrainer(4, 1);
}

void stDxZebes::update(float deltaFrame) {
    grZebesMapProc();
    grZebesWaterProc();
    grZebesBGProc();
    if (m_flagStart) {
        m_fadeTimer -= deltaFrame;
        if (m_fadeTimer < 0.0f) {
            m_fadeTimer = 0.0f;
        }
        if (m_fadeTimer == 0.0f) {
            if (m_flagFade == 1) {
                GXColor black;
                black.r = 0;
                black.g = 0;
                black.b = 0;
                black.a = 0xFF;
                m_flagFade = false;
                m_fadeTimer = 480.0f;
                GXColor fill = black;
                int handle = g_efScreen->requestFill(60.0f, 0, 0, &fill);
                reinterpret_cast<u8&>(m_screenHandle) = (u32)handle >> 24;
            } else {
                m_flagFade = true;
                m_fadeTimer = 1200.0f;
                efScreenHandle handle = m_screenHandle;
                fn_8005E3C8(g_efScreen, &handle, 60.0f);
                reinterpret_cast<s8&>(m_screenHandle) = -1;
            }
        }
    }

    // The cells follow their ground models; a cell that was hit pops.
    for (int i = 0; i < 20; i++) {
        stDxZebesCell* cell = &m_cells[i];
        Vec3f scale;
        Vec3f pos;
        pos.m_x = cell->m_x;
        pos.m_y = cell->m_y;
        pos.m_z = 0.0f;
        float size = 0.85f * cell->m_size;
        scale.m_x = size;
        scale.m_y = size;
        scale.m_z = size;
        cell->m_ground->setScale(&scale);
        cell->m_ground->setPos(&pos);
        if (cell->m_ground->isHit() == 1 && cell->m_state == 1 && i != 0 && i != 6) {
            m_cells[i].m_state = 2;
            m_cellDelay = 120;
            m_cells[i].m_targetSize = 0.001f;
            playSeBasic((SndID)0x3B, 0.0f);
            break;
        }
    }

    // The shafts take the damage the grounds were hit with.
    for (int i = 0; i < 2; i++) {
        if (ZEBES_GROUND(0xC + i)->isHit()) {
            u8* hitInfo = *reinterpret_cast<u8**>(reinterpret_cast<u8*>(ZEBES_GROUND(0xC + i)) + 0x160);
            float hitX = *reinterpret_cast<float*>(hitInfo + 0x30);
            float hitY = *reinterpret_cast<float*>(hitInfo + 0x34);
            u8* hitInfo2 = *reinterpret_cast<u8**>(reinterpret_cast<u8*>(ZEBES_GROUND(0xC + i)) + 0x160);
            m_shaft[i].m_strength += *reinterpret_cast<float*>(hitInfo2 + 0x28);
            if (hitX > 0.0f) {
                m_shaft[i].m_velocity = 0.5f + hitX;
            } else {
                m_shaft[i].m_velocity = -0.5f + hitX;
            }
        }
    }
    m_eventShaft.update(deltaFrame);
    m_eventPhase.update(deltaFrame);
    updateBirdman(deltaFrame);

    // The platform with the acid attack follows the acid.
    Vec3f acidPos = ZEBES_GROUND(0)->getPos();
    Vec3f attackPos(-300.0f, acidPos.m_y - 75.0f, acidPos.m_z);
    ZEBES_GROUND(0xB)->setPos(&attackPos);

    Vec3f nodePos(0.0f, 0.0f, 0.0f);
    ZEBES_GROUND(5)->getNodePosition(&nodePos, 0, "XMoveLN");
    grWEUpdate(m_acidHeight - 58.85f, &m_lineA, &nodePos);
    ZEBES_GROUND(5)->getNodePosition(&nodePos, 0, "XMoveRN");
    grWEUpdate(m_acidHeight - 58.85f, &m_lineB, &nodePos);

    // The camera is not allowed to see below the acid.
    CameraController* camera = CameraController::getInstance();
    Rect2D range = *reinterpret_cast<Rect2D*>(reinterpret_cast<u8*>(camera) + 0x148);
    if (m_cameraInit == 1) {
        m_cameraMinY = range.m_down;
        m_cameraInit = 0;
    }
    range.m_down = getAcidHeight();
    if (range.m_down < m_cameraMinY) {
        range.m_down = m_cameraMinY;
    }
    CameraController::getInstance()->setCameraRange(&range);

    if (fn_27_2249E0(this) == 1) {
        if (getAcidHeight() > m_cameraFloor - 30.0f) {
            m_cameraFloor = 30.0f + getAcidHeight();
        } else {
            m_cameraFloor -= 0.125f;
            if (m_cameraFloor < 10.0f) {
                m_cameraFloor = 10.7f;
            }
        }
        Vec3f floorPos(10.0f, m_cameraFloor, -150.0f);
        ZEBES_GROUND(0xF)->setPos(&floorPos);
        ZEBES_GROUND(0xF)->getNodePosition(&m_pokeTrainerPos[0], 0, "PokemonTrainer1N");
        ZEBES_GROUND(0xF)->getNodePosition(&m_pokeTrainerPos[1], 0, "PokemonTrainer2N");
        ZEBES_GROUND(0xF)->getNodePosition(&m_pokeTrainerPos[2], 0, "PokemonTrainer3N");
        ZEBES_GROUND(0xF)->getNodePosition(&m_pokeTrainerPos[3], 0, "PokemonTrainer4N");
    }
}

// The statue in the background (ground "birdman"): its two events take turns, every time the second one runs out it
// plays the next of its two motions.
void stDxZebes::updateBirdman(float deltaFrame) {
    switch (m_eventPhase.getPhase()) {
        case 0:
            fn_27_279228(ZEBES_GROUND(6), 1);
            if (m_eventPhase.isReadyEnd() == 1) {
                m_eventPhase.setPhase(1);
                m_eventShaft.start();
                fn_27_279228(ZEBES_GROUND(6), 0);
                ZEBES_GROUND(6)->setMotion(m_motionToggle);
                ZEBES_GROUND(6)->startEntity();
            }
            break;
        case 1:
            if (m_eventShaft.isReadyEnd() == 1) {
                m_eventShaft.end();
                m_eventShaft.start();
                if (ZEBES_GROUND(6)->isEndEntity()) {
                    m_motionToggle ^= 1;
                    ZEBES_GROUND(6)->setMotion(m_motionToggle);
                    ZEBES_GROUND(6)->startEntity();
                }
            }
            break;
    }
}

// Adds a cell at (x, y) unless the slot is in use.
void stDxZebes::grZebesCellAdd(int index, float x, float y, float size, int state) {
    stDxZebesCell* cell = &m_cells[index];
    if (cell->m_state == 0) {
        cell->m_x = x;
        cell->m_y = y;
        cell->m_velX = 0.0f;
        cell->m_velY = 0.0f;
        cell->m_targetSize = size;
        cell->m_size = size;
        Vec3f pos(x, y, 0.0f);
        cell->m_ground->setPos(&pos);
        pos.m_x = size;
        pos.m_y = size;
        pos.m_z = size;
        cell->m_ground->setScale(&pos);
        cell->m_state = state;
        cell->m_ground->startEntityAutoLoop();
        cell->m_ground->setMotionRatio(0.0f);
        cell->m_ground->setMotionFrame(0.0f, 0);
    }
}

// Lets a cell grow or shrink to its size, counts down the popping cells and moves a popped cell's neighbour over.
bool stDxZebes::grZebesCellUpdateOne(int index) {
    bool result = false;
    stDxZebesCell* cell = &m_cells[index];
    if (cell->m_state != 0 && cell->m_ground != NULL) {
        float diff = cell->m_targetSize - cell->m_size;
        if (diff > 0.05f) {
            cell->m_size += 0.05f;
        } else if (diff < -0.05f) {
            cell->m_size -= 0.05f;
        } else {
            cell->m_size = cell->m_targetSize;
        }
        switch (cell->m_state) {
            case 3:
                if (cell->m_timer-- < 0) {
                    Vec3f breakPos(cell->m_x, cell->m_y, 4.0f);
                    playSeBasic(snd_se_stage_Zebes_cellbreak, 0.0f);
                    cell->m_state = 0;
                    cell->m_ground->endEntity();
                }
                break;
            case 2: {
                Vec3f popPos(cell->m_x, cell->m_y, 4.0f);
                cell->m_state = 0;
                cell->m_ground->endEntity();
                u32 effect = g_ecMgr->setEffect(ef_ptc_stg_dx_zebes_haretu);
                g_ecMgr->setParent(effect, cell->m_ground->m_sceneModels[0], "Model", false);
                if (index < 7 && index != 0 && index != 6) {
                    int nearest = -1;
                    float nearestDistance = 64.0f;
                    for (int i = 7; i < 20; i++) {
                        if (m_cells[i].m_state == 1) {
                            float dx = m_cells[i].m_x - cell->m_x;
                            float dy = m_cells[i].m_y - cell->m_y;
                            float distance = dx * dx + dy * dy;
                            if (distance < nearestDistance) {
                                nearest = i;
                                nearestDistance = distance;
                            }
                        }
                    }
                    if (nearest != -1) {
                        stDxZebesCell* other = &m_cells[nearest];
                        cell->m_state = other->m_state;
                        cell->m_size = other->m_size;
                        cell->m_targetSize = other->m_targetSize;
                        cell->m_ground->setMotionRatio(0.0f);
                        cell->m_ground->setMotionFrame(0.0f, 0);
                        cell->m_ground->startEntityAutoLoop();
                        other->m_state = 0;
                        other->m_ground->endEntity();
                    } else {
                        for (int i = 7; i < 20; i++) {
                            if (i != 0 && i != 6 && m_cells[i].m_state == 1) {
                                m_cells[i].m_state = 3;
                                m_cells[i].m_timer = randi(60) + 10;
                            }
                        }
                        result = true;
                    }
                }
                break;
            }
        }
    }
    return result;
}

// Moves all cells: the chain keeps its links together, the floating ones push each other apart.
bool stDxZebes::grZebesCellUpdate() {
    bool result = false;
    for (int i = 0; i < 20; i++) {
        if (m_cells[i].m_state != 0) {
            if (grZebesCellUpdateOne(i)) {
                result = true;
            }
        }
    }
    for (int i = 0; i < 20; i++) {
        if (m_cells[i].m_state != 0) {
            m_cells[i].m_velX = 0.0f;
            m_cells[i].m_velY = -0.1f;
        }
    }

    // The chain: every link is pulled towards its neighbours.
    for (int i = 0; i < 7; i++) {
        stDxZebesCell* cell = &m_cells[i];
        if (cell->m_state == 1 || cell->m_state == 4) {
            int links = 0;
            if (i != 0 && i != 6) {
                float pullX = 0.0f;
                float pullY = 0.0f;
                stDxZebesCell* previous = &m_cells[i - 1];
                if (previous->m_state != 0) {
                    float dx = previous->m_x - cell->m_x;
                    float dy = previous->m_y - cell->m_y;
                    float distance = mtSqrtf(dx * dx + dy * dy);
                    if (distance > 3.5f) {
                        float directionX = dx / distance;
                        float excess = distance - 3.5f;
                        float directionY = dy / distance;
                        if (excess > 0.4f) {
                            excess = 0.4f;
                        }
                        pullX += directionX * excess;
                        pullY += directionY * excess;
                    }
                    links = 1;
                }
                stDxZebesCell* next = &m_cells[i + 1];
                if (next->m_state != 0) {
                    float dx = next->m_x - cell->m_x;
                    float dy = next->m_y - cell->m_y;
                    float distance = mtSqrtf(dx * dx + dy * dy);
                    if (distance > 3.5f) {
                        float directionX = dx / distance;
                        float excess = distance - 3.5f;
                        float directionY = dy / distance;
                        if (excess > 0.4f) {
                            excess = 0.4f;
                        }
                        pullX += directionX * excess;
                        pullY += directionY * excess;
                    }
                    links++;
                }
                cell->m_x += pullX;
                cell->m_y += pullY;
                if (cell->m_state != 4 && links != 2) {
                    cell->m_state = 3;
                    cell->m_timer = randi(60) + 10;
                }
            }
        }
    }

    // Cells push each other away when they are too close.
    for (int i = 0; i < 19; i++) {
        if (m_cells[i].m_state != 0) {
            for (int j = i + 1; j < 20; j++) {
                if (m_cells[j].m_state != 0) {
                    float dx = m_cells[j].m_x - m_cells[i].m_x;
                    float dy = m_cells[j].m_y - m_cells[i].m_y;
                    float distance = mtSqrtf(dx * dx + dy * dy);
                    float sizes = m_cells[i].m_size + m_cells[j].m_size;
                    float near = 1.75f * sizes;
                    float mid = 2.0f * sizes;
                    float far = 3.0f * sizes;
                    if (distance < 0.001f) {
                        dy = 0.001f;
                        dx = 0.0f;
                        distance = 0.001f;
                    }
                    if (distance < near) {
                        float push = ((near / distance) - 1.0f) * 0.5f;
                        push *= 0.9f;
                        if (i >= 7) {
                            float pushY = -dy * push;
                            float pushX = -dx * push;
                            if (pushY < 0.0f) {
                                pushY = 0.0f;
                            }
                            m_cells[i].m_velX += pushX;
                            m_cells[i].m_velY += pushY;
                        }
                        if (j >= 7) {
                            float pushY = dy * push;
                            float pushX = dx * push;
                            if (pushY < 0.0f) {
                                pushY = 0.0f;
                            }
                            m_cells[j].m_velX += pushX;
                            m_cells[j].m_velY += pushY;
                        }
                    } else if (!(distance < mid) && distance < far) {
                        float pull = 0.02f / distance;
                        if (i >= 7) {
                            m_cells[i].m_velX += dx * pull;
                            m_cells[i].m_velY += dy * pull;
                        }
                        if (j >= 7) {
                            m_cells[j].m_velX += -dx * pull;
                            m_cells[j].m_velY += -dy * pull;
                        }
                    }
                }
            }
        }
    }

    // Move the cells, keep them inside the box and slow them down.
    for (int i = 0; i < 20; i++) {
        stDxZebesCell* cell = &m_cells[i];
        if (cell->m_state != 0) {
            if (i == 0) {
                cell->m_velX = 0.0f;
                cell->m_velY = 0.0f;
                cell->m_x = m_bounds[2].m_x;
                cell->m_y = m_bounds[2].m_y;
            } else if (i == 6) {
                cell->m_velX = 0.0f;
                cell->m_velY = 0.0f;
                cell->m_x = m_bounds[3].m_x;
                cell->m_y = m_bounds[3].m_y;
            } else {
                float newY = cell->m_y + cell->m_velY;
                float newX = cell->m_x + cell->m_velX;
                if (newY < m_bounds[0].m_y && newX < m_bounds[0].m_x) {
                    if (cell->m_y >= m_bounds[0].m_y) {
                        newY = m_bounds[0].m_y;
                    } else if (cell->m_x >= m_bounds[0].m_x) {
                        newX = m_bounds[0].m_x;
                    }
                }
                if (newY < m_bounds[1].m_y && newX > m_bounds[1].m_x) {
                    if (cell->m_y >= m_bounds[1].m_y) {
                        newY = m_bounds[1].m_y;
                    } else if (cell->m_x <= m_bounds[1].m_x) {
                        newX = m_bounds[1].m_x;
                    }
                }
                cell->m_x = newX;
                cell->m_y = newY;
                if (cell->m_velX > 1.2f) {
                    cell->m_velX = 1.2f;
                } else if (cell->m_velX < -1.2f) {
                    cell->m_velX = -1.2f;
                }
                if (cell->m_velY > 1.2f) {
                    cell->m_velY = 1.2f;
                } else if (cell->m_velY < -1.2f) {
                    cell->m_velY = -1.2f;
                }
            }
        }
    }
    return result;
}

// Lets a new floating cell grow in from time to time while the organism has fewer than 15.
void stDxZebes::grZebesCellRebirth() {
    if (m_mapState == 0) {
        if (m_cellDelay != 0) {
            m_cellDelay--;
        } else if (--m_cellRespawnTimer < 0) {
            int used = 0;
            int free = -1;
            for (int i = 0; i < 20; i++) {
                if (m_cells[i].m_state != 0) {
                    used++;
                } else if (free == -1) {
                    free = i;
                }
            }
            if (used < 15) {
                float left = m_bounds[2].m_x;
                float width = m_bounds[1].m_x - left;
                float bottom = m_bounds[2].m_y;
                float height = m_bounds[1].m_y - bottom;
                float size = 1.0f + 0.5f * randf();
                float y = bottom + height * randf();
                float x = (3.5f + left) + (width - 7.0f) * randf();
                grZebesCellAdd(free, x, y, size, 1);
                m_cells[free].m_size = 0.001f;
                playSeBasic(snd_se_stage_Zebes_cellbirth, 0.0f);
            }
            m_cellRespawnTimer = randi(30) + 20;
        }
    }
}

// Sets up the box the cells live in and the first 16 cells, then lets them settle.
void stDxZebes::grZebesCellInit() {
    m_cellDelay = 0;
    m_mapState = 0;
    m_cellRespawnTimer = 0;
    for (int i = 0; i < 20; i++) {
        m_cells[i].m_state = 0;
    }
    float sizes[7] = {1.0f, 1.1f, 1.0f, 1.2f, 1.1f, 1.0f, 1.0f};
    Vec3f p0(7.59f, 2.5f, 0.0f);
    Vec3f p1(24.05f, 2.2f, 0.0f);
    Vec3f p2(8.2f, -4.55f, 0.0f);
    Vec3f p3(24.1f, -4.6f, 0.0f);
    m_bounds[2] = p2;
    m_bounds[3] = p3;
    m_bounds[0] = p0;
    m_bounds[1] = p1;
    m_bounds[2].m_x = p2.m_x * 1.07f;
    m_bounds[2].m_y = p2.m_y * 1.07f;
    m_bounds[3].m_x = p3.m_x * 1.07f;
    m_bounds[3].m_y = p3.m_y * 1.07f;
    m_bounds[0].m_x = p0.m_x * 1.07f;
    m_bounds[0].m_y = p0.m_y * 1.07f;
    m_bounds[1].m_x = p1.m_x * 1.07f;
    m_bounds[1].m_y = p1.m_y * 1.07f;
    float stepX = (m_bounds[3].m_x - m_bounds[2].m_x) / 6.0f;
    float stepY = (m_bounds[3].m_y - m_bounds[2].m_y) / 6.0f;
    for (int i = 0; i < 7; i++) {
        grZebesCellAdd(i, m_bounds[2].m_x + stepX * (float)i, m_bounds[2].m_y + stepY * (float)i, sizes[i], 1);
        if (i == 0 || i == 6) {
            m_cells[i].m_ground->setVisibility(0);
        }
    }
    float left = m_bounds[2].m_x;
    float width = m_bounds[1].m_x - left;
    float bottom = m_bounds[2].m_y;
    float height = m_bounds[1].m_y - bottom;
    float innerX = 3.5f + left;
    float innerW = width - 7.0f;
    grZebesCellAdd(7, innerX + 0.5f * innerW, bottom + 0.5f * height, 1.0f, 1);
    grZebesCellAdd(8, innerX + 0.2f * innerW, bottom + 0.8f * height, 1.2f, 1);
    grZebesCellAdd(9, innerX + 0.8f * innerW, bottom + 0.2f * height, 1.1f, 1);
    grZebesCellAdd(10, innerX + 0.8f * innerW, bottom + 0.8f * height, 1.1f, 1);
    grZebesCellAdd(11, innerX + 0.5f * innerW, bottom + 0.2f * height, 1.2f, 1);
    grZebesCellAdd(12, innerX + 0.5f * innerW, bottom + 0.9f * height, 1.3f, 1);
    grZebesCellAdd(13, innerX + 0.5f * innerW, bottom + 0.9f * height, 1.3f, 1);
    grZebesCellAdd(14, innerX + 0.6f * innerW, bottom + height, 1.1f, 1);
    grZebesCellAdd(15, innerX + 0.2f * innerW, bottom + height, 1.0f, 1);
    for (int i = 0; i < 100; i++) {
        grZebesCellUpdate();
    }
}

// The state machine of one shaft (a platform that sinks when it is hit): see stDxZebesShaft.
void stDxZebes::grZebesShaftUpdate(int groundA, stDxZebesShaft* shaft, int groundB, int groundC) {
    if (shaft->m_state != shaft->m_nextState) {
        shaft->m_state = shaft->m_nextState;
        switch (shaft->m_nextState) {
            case 0:
                ZEBES_GROUND(groundA)->setMotion(0);
                ZEBES_GROUND(groundA)->setMotionRatio(0.0f);
                ZEBES_GROUND(groundA)->setMotionFrame(0.0f, 0);
                ZEBES_GROUND(groundA)->startEntity();
                ZEBES_GROUND(groundB)->setMotion(0);
                ZEBES_GROUND(groundB)->startEntityAutoLoop();
                ZEBES_GROUND(groundC)->startEntity();
                shaft->m_timer = 0;
                shaft->m_offset = 0.0f;
                shaft->m_velocity = 0.0f;
                break;
            case 1:
                ZEBES_GROUND(groundA)->setMotion(1);
                ZEBES_GROUND(groundA)->setMotionRatio(1.0f);
                ZEBES_GROUND(groundA)->setMotionFrame(0.0f, 0);
                ZEBES_GROUND(groundA)->startEntity();
                ZEBES_GROUND(groundB)->setMotion(1);
                ZEBES_GROUND(groundB)->startEntity();
                ZEBES_GROUND(groundC)->endEntity();
                break;
            case 2:
                shaft->m_timer = (s16)(1200.0f + 600.0f * randf());
                break;
            case 3:
                ZEBES_GROUND(groundA)->setMotion(2);
                ZEBES_GROUND(groundA)->setMotionRatio(1.0f);
                ZEBES_GROUND(groundA)->setMotionFrame(0.0f, 0);
                ZEBES_GROUND(groundA)->startEntity();
                ZEBES_GROUND(groundB)->setMotion(2);
                ZEBES_GROUND(groundB)->startEntity();
                shaft->m_strength = 0.0f;
                break;
        }
    }
    switch (shaft->m_state) {
        case 0: {
            if (shaft->m_timer++ > 20) {
                shaft->m_timer = 0;
                shaft->m_strength -= 1.0f;
                if (shaft->m_strength < 0.0f) {
                    shaft->m_strength = 0.0f;
                }
            }
            if (shaft->m_offset > 0.15f) {
                shaft->m_velocity -= 0.1f;
            } else if (shaft->m_offset < -0.15f) {
                shaft->m_velocity += 0.1f;
            }
            shaft->m_velocity *= 0.9f;
            if (shaft->m_velocity > 2.0f) {
                shaft->m_velocity = 2.0f;
            } else if (shaft->m_velocity < -2.0f) {
                shaft->m_velocity = -2.0f;
            }
            shaft->m_offset += shaft->m_velocity;
            if (shaft->m_offset > 6.0f) {
                shaft->m_offset = 6.0f;
            } else if (shaft->m_offset < -6.0f) {
                shaft->m_offset = -6.0f;
            }
            if (__fabsf(shaft->m_offset) < 0.15f && __fabsf(shaft->m_velocity) < 0.15f) {
                shaft->m_offset = 0.0f;
                shaft->m_velocity = 0.0f;
            }
            grNodeCallbackData* data = ZEBES_GROUND(groundA)->m_calcWorldCallBack.m_nodeCallbackDatas;
            data[1].m_offsetPos.m_x = shaft->m_base + shaft->m_offset;
            data[1].m_offsetPos.m_y = 0.0f;
            data[1].m_offsetPos.m_z = 0.0f;
            data = ZEBES_GROUND(groundA)->m_calcWorldCallBack.m_nodeCallbackDatas;
            data[2].m_offsetPos.m_x = shaft->m_base + shaft->m_offset;
            data[2].m_offsetPos.m_y = 0.0f;
            data[2].m_offsetPos.m_z = 0.0f;
            float frame = 30.0f;
            if (shaft->m_strength > frame) {
                shaft->m_nextState = 1;
                playSeBasic(snd_se_stage_Zebes_pole_break, 0.0f);
            } else {
                frame = (frame * shaft->m_strength) / frame;
            }
            ZEBES_GROUND(groundA)->setMotionFrame(frame, 0);
            break;
        }
        case 1:
            if (ZEBES_GROUND(groundA)->isEndEntity()) {
                shaft->m_nextState = 2;
            }
            break;
        case 2:
            if (--shaft->m_timer < 0) {
                shaft->m_nextState = 3;
            }
            break;
        case 3:
            if (ZEBES_GROUND(groundA)->isEndEntity()) {
                shaft->m_nextState = 0;
            }
            break;
    }
}

// Starts the acid at the first level of the table.
void stDxZebes::grZebesWaterInit() {
    m_acidStep = 0;
    s16 range = sAcidLevels[0].m_waitRandom;
    s32 random = randi(range + 1);
    if ((u32)random >= (u32)range) {
        random = range;
    }
    m_acidTimer = sAcidLevels[m_acidStep].m_wait + random;
    m_acidState = 1;
    m_acidRate = 0.0f;
    m_acidSpeed = 0.0f;
    stDxZebesAcidLevel* level = sAcidLevels;
    int last = 0;
    while (last < 18 && (level[1].m_wait != 0 || level[1]._2 != 0 || level[1].m_waitRandom != 0 || level[1].m_height != 0)) {
        level++;
        last++;
    }
    float height = (float)sAcidLevels[last].m_height + (7.0f - 14.0f * randf());
    m_acidTarget = height;
    m_acidHeight = height;
}

// The acid: waits, then rises or falls to the next level of the table, shaking the camera while it does.
void stDxZebes::grZebesWaterProc() {
    switch (m_acidState) {
        case 0:
            break;
        case 1: {
            if (--m_acidTimer < 120) {
                Vec3f quake(0.0f, 0.0f, 0.0f);
                cmReqQuake(cmQuake::Amplitude_Middle, &quake);
            }
            if (m_acidTimer < 0) {
                float target = (float)sAcidLevels[m_acidStep].m_height + (7.0f - 14.0f * randf());
                m_acidTarget = target;
                if (target > 90.0f) {
                    m_acidTarget = 90.0f;
                } else if (target < -320.0f) {
                    m_acidTarget = -320.0f;
                }
                m_acidSpeed = 0.0f;
                m_acidState = 2;
                m_acidTimer = 0;
                float distance = __fabsf(m_acidTarget - m_acidHeight);
                float discriminant = 57600.0f - (4.0f * distance) / 0.03f;
                if (discriminant < 0.0f) {
                    m_acidRate = 999.0f;
                } else {
                    float root = mtSqrtf(discriminant);
                    m_acidRate = 0.5f * (0.03f * (240.0f - root));
                    if (m_acidRate < 0.0f) {
                        m_acidRate = 999.0f;
                    }
                }
                if (m_acidTarget > m_acidHeight) {
                    playSeBasic(snd_se_stage_Zebes_asidsea_up, 0.0f);
                } else {
                    playSeBasic(snd_se_stage_Zebes_asdsea_down, 0.0f);
                }
            }
            break;
        }
        case 2:
            m_acidState = 3;
            break;
        case 3: {
            if (++m_acidTimer < 60) {
                Vec3f quake(0.0f, 0.0f, 0.0f);
                cmReqQuake(cmQuake::Amplitude_Middle, &quake);
            }
            float speed = m_acidSpeed;
            float time = speed / 0.03f;
            float remaining = m_acidTarget - m_acidHeight;
            float brake = speed * time - 0.015f * (time * time);
            if (__fabsf(remaining) < brake || __fabsf(remaining) < 0.03f) {
                m_acidState = 4;
            } else {
                m_acidSpeed = speed + 0.03f;
                if (m_acidSpeed > m_acidRate) {
                    m_acidSpeed = m_acidRate;
                }
            }
            if (m_acidTarget > m_acidHeight) {
                m_acidHeight = m_acidHeight + m_acidSpeed;
            } else {
                m_acidHeight = m_acidHeight - m_acidSpeed;
            }
            break;
        }
        case 4: {
            m_acidSpeed -= 0.03f;
            float remaining = m_acidTarget - m_acidHeight;
            if (m_acidSpeed < 0.0f || __fabsf(remaining) < m_acidSpeed) {
                m_acidState = 1;
                m_acidSpeed = 0.0f;
                m_acidStep++;
                if (m_acidStep == 19 ||
                    (sAcidLevels[m_acidStep].m_wait == 0 && sAcidLevels[m_acidStep]._2 == 0 &&
                     sAcidLevels[m_acidStep].m_waitRandom == 0 && sAcidLevels[m_acidStep].m_height == 0)) {
                    m_acidStep = 0;
                }
                s32 random = randi(sAcidLevels[m_acidStep].m_waitRandom + 1);
                if ((u32)random >= (u32)sAcidLevels[m_acidStep].m_waitRandom) {
                    random = sAcidLevels[m_acidStep].m_waitRandom;
                }
                m_acidTimer = sAcidLevels[m_acidStep].m_wait + random;
            }
            if (m_acidTarget > m_acidHeight) {
                m_acidHeight = m_acidHeight + m_acidSpeed;
            } else {
                m_acidHeight = m_acidHeight - m_acidSpeed;
            }
            break;
        }
    }
    float ratio = zebesClamp((m_acidHeight - -320.0f) / 410.0f, 0.0f, 1.0f);
    g_gfSceneRoot->setCurrentFrame(120.0f * ratio);
    Vec3f acidPos;
    acidPos.m_y = m_acidHeight;
    acidPos.m_x = 0.0f;
    acidPos.m_z = 0.0f;
    ZEBES_GROUND(0)->setPos(&acidPos);
}

// Moves the platforms that follow the moving platform's node.
#define ZEBES_FOLLOW_NODES(pos)                                         \
    ZEBES_GROUND(5)->getNodePosition(&pos, 0, "XMoveLN");               \
    ZEBES_GROUND(1)->setPos(&pos);                                      \
    ZEBES_GROUND(2)->setPos(&pos);                                      \
    ZEBES_GROUND(6)->setPos(&pos);                                      \
    ZEBES_GROUND(9)->setPos(&pos);                                      \
    ZEBES_GROUND(0xC)->setPos(&pos);                                    \
    ZEBES_GROUND(5)->getNodePosition(&pos, 0, "XMoveRN");               \
    ZEBES_GROUND(3)->setPos(&pos);                                      \
    ZEBES_GROUND(4)->setPos(&pos);                                      \
    ZEBES_GROUND(0xA)->setPos(&pos);                                    \
    ZEBES_GROUND(0xD)->setPos(&pos)

// The shafts on both sides, the organism in the middle and the moving platform that carries the side platforms.
void stDxZebes::grZebesMapProc() {
    grZebesShaftUpdate(9, &m_shaft[0], 2, 0xC);
    grZebesShaftUpdate(0xA, &m_shaft[1], 4, 0xD);
    int shaftState = m_shaft[0].m_state;
    if (m_lastShaftState != shaftState) {
        m_lastShaftState = shaftState;
        if (shaftState == 1) {
            ZEBES_GROUND(9)->setMotion(1);
            ZEBES_GROUND(9)->setMotionRatio(1.0f);
            ZEBES_GROUND(9)->setMotionFrame(0.0f, 0);
            ZEBES_GROUND(9)->startEntity();
            ZEBES_GROUND(2)->setMotion(1);
            ZEBES_GROUND(2)->startEntity();
        } else if (shaftState == 3) {
            ZEBES_GROUND(9)->setMotion(2);
            ZEBES_GROUND(9)->setMotionRatio(1.0f);
            ZEBES_GROUND(9)->setMotionFrame(0.0f, 0);
            ZEBES_GROUND(9)->startEntity();
            ZEBES_GROUND(2)->setMotion(2);
            ZEBES_GROUND(2)->startEntity();
        }
    }
    if (m_cellSpawnTimer-- < 0) {
        m_cellSpawnTimer = (s16)(200.0f + 100.0f * randf());
    }
    bool merged = grZebesCellUpdate();
    grZebesCellRebirth();

    switch (m_mapState) {
        case 0:
            if (merged) {
                m_mapState = 1;
                ZEBES_GROUND(5)->endEntity();
                ZEBES_GROUND(5)->setMotion(0);
                ZEBES_GROUND(5)->startEntity();
            }
            break;
        case 1: {
            Vec3f followPos;
            ZEBES_FOLLOW_NODES(followPos);
            if (ZEBES_GROUND(5)->isEndEntity()) {
                m_mapState = 2;
                m_mapTimer = (s16)(600.0f + randf() * 0.0f);
            }
            break;
        }
        case 2:
            if (--m_mapTimer < 0) {
                m_mapState = 3;
                ZEBES_GROUND(5)->endEntity();
                ZEBES_GROUND(5)->setMotion(1);
                ZEBES_GROUND(5)->startEntity();
                m_cellCounter = 0;
                m_cellDelay = 0;
            }
            break;
        case 3: {
            s16 counter = ++m_cellCounter;
            if (counter % 15 == 0) {
                int a = counter / 15;
                int b = 6 - a;
                if (a < b) {
                    grZebesCellAdd(a, m_cells[a - 1].m_x, m_cells[a - 1].m_y, 1.0f + 0.5f * randf(), 4);
                }
                if (a <= b) {
                    grZebesCellAdd(b, m_cells[b + 1].m_x, m_cells[b + 1].m_y, 1.0f + 0.5f * randf(), 4);
                }
            }
            Vec3f followPos;
            ZEBES_FOLLOW_NODES(followPos);
            if (ZEBES_GROUND(5)->isEndEntity()) {
                m_mapState = 0;
                for (int i = 0; i < 20; i++) {
                    if (m_cells[i].m_state == 4) {
                        m_cells[i].m_state = 1;
                    }
                }
            }
            break;
        }
    }

    // While the organism is whole its body is solid: the top of the cells makes the surface of the collision.
    if (m_mapState == 0) {
        float cellWidth = (m_bounds[1].m_x - m_bounds[0].m_x) / 5.0f;
        float top[6];
        float edge[6];
        for (int i = 0; i < 6; i++) {
            top[i] = -9999.0f;
        }
        for (int i = 0; i < 20; i++) {
            stDxZebesCell* cell = &m_cells[i];
            if (cell->m_state == 1) {
                float relative = cell->m_x - m_bounds[0].m_x;
                float surface = cell->m_y + 1.8f * cell->m_size;
                float lower = relative - 0.9f;
                float upper = 0.9f + relative;
                int bucket = (int)(0.5f + lower / cellWidth);
                if (bucket > 5) {
                    bucket = 5;
                } else if (bucket < 0) {
                    bucket = 0;
                }
                edge[bucket] = m_bounds[0].m_x + (float)bucket * cellWidth;
                if (surface > top[bucket]) {
                    top[bucket] = surface;
                }
                bucket = (int)(0.5f + upper / cellWidth);
                if (bucket > 5) {
                    bucket = 5;
                } else if (bucket < 0) {
                    bucket = 0;
                }
                edge[bucket] = m_bounds[0].m_x + (float)bucket * cellWidth;
                if (surface > top[bucket]) {
                    top[bucket] = surface;
                }
            }
        }
        edge[0] = 9.19023f;
        top[0] = 1.3000501f;
        edge[5] = 24.77157f;
        top[5] = 1.2914901f;
        for (int i = 0; i < 5; i++) {
            if (i != 0) {
                if (top[i] < top[i + 1] - cellWidth) {
                    top[i] = top[i + 1] - cellWidth;
                }
            }
            if (i != 5) {
                if (top[i + 1] < top[i] - cellWidth) {
                    top[i + 1] = top[i] - cellWidth;
                }
            }
        }
        ZEBES_GROUND(0xE)->setEnableCollisionStatus(true);
        grCollision* collision = ZEBES_GROUND(0xE)->m_collision;
        stDxZebesJointView* joint = reinterpret_cast<stDxZebesJointView*>(collision->getJoint(0));
        joint->m_bit40 = joint->m_bit10 = true;
        float* vertices = reinterpret_cast<float*>(*reinterpret_cast<u8**>(reinterpret_cast<u8*>(collision->getJoint(0)) + 0x38));
        for (int i = 0; i < 6; i++) {
            vertices[i * 2] = edge[i];
            vertices[i * 2 + 1] = top[i];
        }
    } else {
        stDxZebesJointView* joint = reinterpret_cast<stDxZebesJointView*>(ZEBES_GROUND(0xE)->m_collision->getJoint(0));
        joint->m_bit40 = joint->m_bit10 = joint->m_bit80 = false;
    }
}

// The background: when the acid has risen high enough the core in the background moves.
void stDxZebes::grZebesBGProc() {
    switch (m_bgState) {
        case 0:
            if (m_acidHeight > 5.0f) {
                playSeBasic(snd_se_stage_Zebes_coremove, 0.0f);
                m_bgState = 1;
                ZEBES_GROUND(7)->setMotion(2);
                ZEBES_GROUND(7)->startEntity();
            } else if (m_acidHeight > -20.0f) {
                playSeBasic(snd_se_stage_Zebes_coremove, 0.0f);
                ZEBES_GROUND(7)->setMotion(1);
                ZEBES_GROUND(7)->startEntity();
                m_bgState = 1;
            }
            break;
        case 1:
            if (ZEBES_GROUND(7)->isEndEntity()) {
                ZEBES_GROUND(7)->setMotion(0);
                ZEBES_GROUND(7)->startEntityAutoLoop();
                m_bgState = 0;
            }
            break;
    }
}

// Places the effects of a line where its segments cross the height of the acid; returns how many do.
int stDxZebes::grWEUpdate(float height, stDxZebesAcidLine* line, Vec3f* offset) {
    int active = 0;
    Vec3f* source = line->m_source;
    Vec3f* destination = line->m_points;
    for (int i = 0; i < line->m_count; i++) {
        destination[0] = source[0] + *offset;
        destination[1] = source[1] + *offset;
        source += 2;
        destination += 2;
    }
    for (int i = 0; i < line->m_count; i++) {
        Vec3f* a = &line->m_points[i * 2];
        Vec3f* b = &line->m_points[i * 2 + 1];
        if (!((a->m_y > height && b->m_y > height) || (a->m_y < height && b->m_y < height))) {
            Vec3f pos;
            float delta = b->m_y - a->m_y;
            if (__fabsf(delta) > 0.0001f) {
                float t = (height - a->m_y) / delta;
                pos.m_y = height;
                pos.m_x = a->m_x + t * (b->m_x - a->m_x);
                pos.m_z = a->m_z + t * (b->m_z - a->m_z);
            } else {
                float t = randf();
                pos.m_y = height;
                pos.m_x = a->m_x + t * (b->m_x - a->m_x);
                pos.m_z = a->m_z + t * (b->m_z - a->m_z);
            }
            if (line->m_effects[i] == (u32)-1) {
                g_ecMgr->setDrawPrio(1);
                line->m_effects[i] = g_ecMgr->setEffect(ef_ptc_stg_dx_zebes_yuge);
                g_ecMgr->setDrawPrio(-1);
                g_ecMgr->setPos(line->m_effects[i], &pos);
            } else {
                g_ecMgr->setPos(line->m_effects[i], &pos);
            }
            active++;
        } else if (line->m_effects[i] != (u32)-1) {
            g_ecMgr->killEffect(line->m_effects[i], 1, 1);
            line->m_effects[i] = -1;
        }
    }
    return active;
}

float stDxZebes::getAcidHeight() {
    return m_acidHeight - 55.0f;
}

stClassInfoImpl<Stages::DxZebes, stDxZebes> stDxZebes::bss_loc_14;
