#include <cm/cm_quake.h>
#include <cm/cm_subject.h>
#include <ec/ec_mgr.h>
#include <ef/ef_id.h>
#include <gf/gf_archive.h>
#include <gf/gf_camera.h>
#include <gf/gf_copyefb.h>
#include <gm/gm_global.h>
#include <gm/gm_lib.h>
#include <gr/gr_gimmick.h>
#include <gr/gr_madein.h>
#include <gr/gr_stadium_vision.h>
#include <gr/gr_tengan_event.h>
#include <memory.h>
#include <mt/mt_prng.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <nw4r/g3d/g3d_resmat.h>
#include <nw4r/g3d/g3d_resmdl.h>
#include <snd/snd_id.h>
#include <st/st_class_info.h>
#include <st/st_melee.h>
#include <st/st_positions.h>
#include <st/st_trigger.h>
#include <types.h>

#include <st_dxpstadium/st_dxpstadium.h>

stClassInfoImpl<Stages::DxPStadium, stDxPStadium> stDxPStadium::bss_loc_14;

// HYPOTHESIS: bit 0 of the byte at +0xF of the melee init data is set while the big screen shows Jigglypuff.
struct stDxPStadiumMeleeFlags {
    u8 pad : 7;
    u8 jigglypuff : 1;
};
// MATCH-ONLY: the original reads the first two bytes of a player's init data (character kind, state) as bytes.
struct stDxPStadiumPlayerBytes {
    u8 kind;
    u8 state;
};

static inline void stDxPStadiumSetShort(s16* dst, s16 value) {
    *dst = value;
}

static inline void stDxPStadiumSwap(u8& x, u8& y) {
    u8 tmp = x;
    x = y;
    y = tmp;
}

// MATCH-ONLY: the original scales vectors with paired singles (inline asm in the shared vector code).
static inline void stDxPStadiumVec3Scale(register Vec3f* pOut, register const Vec3f* v, register float c) {
    register float fr0, fr1;
    // clang-format off
    asm {
        psq_l    fr0, Vec3f.m_x(v), 0, 0
        psq_l    fr1, Vec3f.m_z(v), 1, 0
        ps_muls0 fr0, fr0, c
        ps_muls0 fr1, fr1, c
        psq_st   fr0, Vec3f.m_x(pOut), 0, 0
        psq_st   fr1, Vec3f.m_z(pOut), 1, 0
    }
    // clang-format on
}

// The big screen is ground 0 and always a grStadiumVision.
static inline grStadiumVision* stadiumVision(stDxPStadium* stage) {
    return static_cast<grStadiumVision*>(stage->getGround(0));
}

stDxPStadium* stDxPStadium::create() {
    return new (Heaps::StageInstance) stDxPStadium;
}

stDxPStadium::stDxPStadium() : stMelee("stDxPStadium", Stages::DxPStadium) {
    m_terrainScale = 0.0f;
    m_unk5EC = -1;
    m_unk604 = -1;
    m_unk608 = -1;
    m_unk614 = 0;
    m_pickIndex = 0;
    m_pickOrder[0] = 0;
    m_pickOrder[1] = 1;
    m_pickOrder[2] = 2;
    m_pickOrder[3] = 3;
    u32 a, b;
    for (int i = 0; i < 32; i++) {
        a = randi(4);
        if (a >= 3) {
            a = 3;
        }
        b = randi(4);
        if (b >= 3) {
            b = 3;
        }
        stDxPStadiumSwap(m_pickOrder[a], m_pickOrder[b]);
    }
    m_unk6F8 = false;
    m_visionActive = false;
    m_visionZoomTarget = 1.0f;
    m_visionNext = 0;
    m_visionCount = 0;
    m_visionZoom = 1.0f;
    m_visionPosA.m_x = 0.0f;
    m_visionPosA.m_y = 0.0f;
    m_visionPosA.m_z = 0.0f;
    m_visionPosB.m_x = 0.0f;
    m_visionPosB.m_y = 0.0f;
    m_visionPosB.m_z = 0.0f;
    m_visionLeft = 0.0f;
    m_visionTop = 0.0f;
    m_visionRight = 1.0f;
    m_visionBottom = 1.0f;
    m_beltTrigger = NULL;
    m_beltData = NULL;
}

stDxPStadium::~stDxPStadium() {
    if (m_beltData != NULL) {
        delete m_beltData;
    }
    releaseArchive();
    // HYPOTHESIS: the flag marks that the big screen shows Jigglypuff (see startPlayerVision)
    reinterpret_cast<stDxPStadiumMeleeFlags*>(reinterpret_cast<u8*>(g_GameGlobal->m_modeMelee) + 0xF)->jigglypuff = 0;
}

bool stDxPStadium::loading() {
    return true;
}

void stDxPStadium::createObj() {
    testStageParamInit(m_fileData, 10);
    addGround(grStadiumVision::create(0, "", ""));
    for (s16 i = 1; i < 12; i++) {
        addGround(grMadein::create(i, "", "", Heaps::StageInstance));
    }
    Ground* ground;
    for (u32 i = 0, groundNum = getGroundNum(); i != groundNum; i++) {
        ground = getGround(i);
        if (ground != NULL) {
            ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
            ground->setStageData(m_stageData);
        }
    }
    createCollision(m_fileData, 0, NULL);
    createCollision(m_fileData, 1, getGround(1));
    createCollision(m_fileData, 2, getGround(2));
    createCollision(m_fileData, 3, getGround(4));
    createCollision(m_fileData, 4, getGround(7));
    createCollision(m_fileData, 5, getGround(9));
    float visionValue = 0.45f;
    stadiumVision(this)->m_unk174 = visionValue;
    stadiumVision(this)->m_unk178 = visionValue;
    visionValue = 18.0f;
    stadiumVision(this)->m_unk17C = visionValue;
    visionValue = 15.0f;
    stadiumVision(this)->m_unk180 = visionValue;
    stDxPStadiumSetShort(&stadiumVision(this)->m_unk190, -0x55);
    stDxPStadiumSetShort(&stadiumVision(this)->m_unk192, 0x55);
    grStadiumVision* vision = stadiumVision(this);
    s16 visionShort = -0x37;
    stDxPStadiumSetShort(&vision->m_unk194, visionShort);
    stDxPStadiumSetShort(&vision->m_unk196, visionShort);
    stDxPStadiumSetShort(&stadiumVision(this)->m_unk19A, -0xF);
    stDxPStadiumSetShort(&stadiumVision(this)->m_unk19C, -5);
    visionValue = 0.5f;
    stadiumVision(this)->m_unk184 = visionValue;
    float visionSource = stadiumVision(this)->m_unk180;
    visionValue = 4.5f * visionSource;
    stadiumVision(this)->m_unk188 = visionValue;
    visionSource = stadiumVision(this)->m_unk174;
    visionValue = 1.6f * visionSource;
    stadiumVision(this)->m_unk18C = visionValue;
    for (u32 i = 1; i < getGroundNum(); i++) {
        static_cast<grMadein*>(getGround(i))->initializeEntity();
    }
    static_cast<grMadein*>(getGround(8))->startEntityAutoLoop();
    static_cast<grMadein*>(getGround(3))->startEntityAutoLoop();
    getGround(1)->setEnableCollisionStatus(false);
    getGround(2)->setEnableCollisionStatus(false);
    getGround(4)->setEnableCollisionStatus(false);
    getGround(7)->setEnableCollisionStatus(false);
    getGround(9)->setEnableCollisionStatus(false);
    initCameraParam();
    void* posData = m_fileData->getData(Data_Type_Model, 100, 0xFFFE);
    if (posData) {
        nw4r::g3d::ResFile posFile(posData);
        createStagePositions(&posFile);
    } else {
        createStagePositions();
    }
    createWind2ndOnly();
    m_eventQuake.set(200.0f, 200.0f);
    m_eventTransform.set(0.0f, 0.0f);
    m_eventPick.set(2400.0f, 3000.0f);
    m_eventGrow.set(1200.0f, 1800.0f);
    m_eventVision.set(600.0f, 600.0f);
    m_eventScreen.set(500.0f, 500.0f);
    m_eventScreenHold.set(300.0f, 300.0f);
    m_eventPick.start();
    m_eventVision.start();
    g_ecMgr->setEffect(ef_ptc_stg_dx_pstadium_hanabi);
    loadStageAttrParam(m_fileData, 30);
    registScnAnim(static_cast<nw4r::g3d::ResFileData*>(m_fileData->getData(Data_Type_Scene, 0, 0xFFFE)), 0);
    initPosPokeTrainer(1, 0);
    createObjPokeTrainer(m_fileData, 101, "PokeTrainer00", m_pokeTrainerPos, NULL);
    stadiumVision(this)->m_screenKind = 4;
    void* miscData = m_fileData->getData(Data_Type_Misc, 0x28, 0xFFFE);
    stadiumVision(this)->m_miscData = miscData;
    getGround(0)->setNodeVisibility(false, 0, "Normal", false, false);
    getGround(0)->setNodeVisibility(false, 0, "Fire", false, false);
    getGround(0)->setNodeVisibility(false, 0, "Grass", false, false);
    getGround(0)->setNodeVisibility(false, 0, "Rock", false, false);
    getGround(0)->setNodeVisibility(false, 0, "Water", false, false);
    getGround(0)->setNodeVisibility(true, 0, "Black", false, false);
    m_eventVision.end();
    m_visionActive = false;
    getGround(0)->setNodeVisibility(false, 0, "Dummy", false, false);
    stadiumVision(this)->setDisplay(false);
    m_beltData = new (Heaps::StageResource) grGimmickBeltConveyorData();
    if (m_beltData != NULL) {
        memset(m_beltData, 0, sizeof(grGimmickBeltConveyorData));
        m_beltData->m_pos.m_x = -52.0f;
        m_beltData->m_pos.m_y = 0.0f;
        m_beltData->m_pos.m_z = 0.0f;
        m_beltData->m_speed = 6.0f;
        m_beltData->m_isRight = false;
        m_beltData->m_areaData.m_offsetPos.m_x = 0.0f;
        m_beltData->m_areaData.m_offsetPos.m_y = 0.0f;
        m_beltData->m_areaData.m_range.m_x = 41.4f;
        m_beltData->m_areaData.m_range.m_y = 10.0f;
        m_beltData->m_areaData.m_shapeType = gfArea::Shape_Rectangle;
        m_beltTrigger = g_stTriggerMng->createTrigger(Gimmick::Area_BeltConveyor, -1);
        m_beltTrigger->setBeltConveyorTrigger(m_beltData);
        m_beltTrigger->setAreaSleep(true);
    }
}

// The transformation of the stage (m_eventTransform), one phase per step:
//   0 wait for the quake to end, then start the terrain announcement on the big screen
//   1 once it has been shown the old terrain shakes and starts shrinking (quake + sound)
//   2 shrink the old terrain away, bring the new one in, set up its effects and its positions
//   3 grow the new terrain; when it has grown the stage settles (camera zooms in)
//   4-7 the same thing in reverse once the terrain has been around for a while
void stDxPStadium::updateSpecialStage(float deltaFrame) {
    if (m_eventTransform.isEvent()) {
        switch (m_eventTransform.getPhase()) {
            case 0:
                if (!m_eventQuake.isEvent()) {
                    m_eventVision.end();
                    m_visionActive = false;
                    getGround(0)->setNodeVisibility(false, 0, "Dummy", false, false);
                    stadiumVision(this)->setDisplay(false);
                    m_eventScreen.start();
                    m_eventScreenHold.start();
                    m_eventTransform.setPhase(m_eventTransform.getPhase() + 1);
                    m_unk6F8 = false;
                }
                break;
            case 1:
                if (m_eventScreenHold.isReadyEnd() == true) {
                    m_eventScreenHold.end();
                    m_terrainScale = 1.0f;
                    m_eventTransform.setPhase(m_eventTransform.getPhase() + 1);
                    g_ecMgr->setEffect(ef_ptc_stg_dx_pstadium_kemuri);
                    m_eventQuake.start();
                    playSeBasic(snd_se_stage_Pstadium_change, 0.0f);
                    zoomOutCamera(300.0f, 310.0f);
                    m_effectHandles[0] = -1;
                    m_effectHandles[1] = -1;
                    m_effectHandles[2] = -1;
                    m_effectHandles[3] = -1;
                    m_effectHandles[4] = -1;
                    m_unk5E8 = 0.0f;
                }
                break;
            case 2:
                m_terrainScale -= 0.01f * deltaFrame;
                if (m_terrainScale <= 0.0f) {
                    m_terrainScale = 0.0001f;
                    static_cast<grMadein*>(getGround(3))->endEntity();
                    getGround(3)->setEnableCollisionStatus(false);
                    if (m_terrain == 4) {
                        static_cast<grMadein*>(getGround(m_terrain))->startEntity();
                    } else {
                        static_cast<grMadein*>(getGround(m_terrain))->startEntityAutoLoop();
                    }
                    getGround(m_terrain)->setEnableCollisionStatus(true);
                    switch (m_terrain) {
                        case 2: {
                            static_cast<grMadein*>(getGround(11))->startEntityAutoLoop();
                            void* posData = m_fileData->getData(Data_Type_Model, 0x67, 0xFFFE);
                            if (posData) {
                                nw4r::g3d::ResFile posFile(posData);
                                m_stagePositions->loadPositionData(&posFile);
                            }
                            break;
                        }
                        case 1: {
                            m_effectHandles[0] = g_ecMgr->setEffect(ef_ptc_stg_dx_pstadium_fire);
                            void* posData = m_fileData->getData(Data_Type_Model, 0x66, 0xFFFE);
                            if (posData) {
                                nw4r::g3d::ResFile posFile(posData);
                                m_stagePositions->loadPositionData(&posFile);
                            }
                            break;
                        }
                        case 4: {
                            m_effectHandles[0] = g_ecMgr->setEffect(ef_ptc_stg_dx_pstadium_rock);
                            void* posData = m_fileData->getData(Data_Type_Model, 0x68, 0xFFFE);
                            if (posData) {
                                nw4r::g3d::ResFile posFile(posData);
                                m_stagePositions->loadPositionData(&posFile);
                            }
                            break;
                        }
                        case 7: {
                            m_effectHandles[0] = g_ecMgr->setEffect(ef_ptc_stg_dx_pstadium_water_01);
                            m_effectHandles[1] = g_ecMgr->setEffect(ef_ptc_stg_dx_pstadium_water_02);
                            m_effectHandles[2] = g_ecMgr->setEffect(ef_ptc_stg_dx_pstadium_water_02);
                            m_effectHandles[3] = g_ecMgr->setEffect(ef_ptc_stg_dx_pstadium_water_03);
                            m_effectHandles[4] = g_ecMgr->setEffect(ef_ptc_stg_dx_pstadium_water_03);
                            static_cast<grMadein*>(getGround(5))->startEntityAutoLoop();
                            static_cast<grMadein*>(getGround(6))->startEntityAutoLoop();
                            static_cast<grMadein*>(getGround(9))->startEntityAutoLoop();
                            static_cast<grMadein*>(getGround(10))->startEntityAutoLoop();
                            g_ecMgr->setParent(m_effectHandles[1], getGround(m_terrain)->m_sceneModels[0], "PtclPoint", false);
                            g_ecMgr->setParent(m_effectHandles[2], getGround(m_terrain)->m_sceneModels[0], "PtclPoint_1", false);
                            g_ecMgr->setParent(m_effectHandles[3], getGround(m_terrain)->m_sceneModels[0], "FunsuiAN", false);
                            g_ecMgr->setParent(m_effectHandles[4], getGround(m_terrain)->m_sceneModels[0], "FunsuiBN", false);
                            void* posData = m_fileData->getData(Data_Type_Model, 0x69, 0xFFFE);
                            if (posData) {
                                nw4r::g3d::ResFile posFile(posData);
                                m_stagePositions->loadPositionData(&posFile);
                            }
                            break;
                        }
                    }
                    updateStagePositions();
                    Vec3f scale;
                    scale.m_y = m_terrainScale;
                    scale.m_x = 1.0f;
                    scale.m_z = 1.0f;
                    static_cast<grGimmick*>(getGround(m_terrain))->setScale(&scale);
                    static_cast<grGimmick*>(getGround(5))->setScale(&scale);
                    static_cast<grGimmick*>(getGround(6))->setScale(&scale);
                    static_cast<grGimmick*>(getGround(9))->setScale(&scale);
                    static_cast<grGimmick*>(getGround(10))->setScale(&scale);
                    static_cast<grGimmick*>(getGround(11))->setScale(&scale);
                    if (m_effectHandles[0] != -1) {
                        g_ecMgr->setScl(m_effectHandles[0], &scale);
                    }
                    if (m_effectHandles[1] != -1) {
                        g_ecMgr->setScl(m_effectHandles[1], &scale);
                    }
                    if (m_effectHandles[2] != -1) {
                        g_ecMgr->setScl(m_effectHandles[2], &scale);
                    }
                    if (m_effectHandles[3] != -1) {
                        g_ecMgr->setScl(m_effectHandles[3], &scale);
                    }
                    if (m_effectHandles[4] != -1) {
                        g_ecMgr->setScl(m_effectHandles[4], &scale);
                    }
                    m_eventTransform.setPhase(m_eventTransform.getPhase() + 1);
                }
                {
                    Vec3f scale;
                    scale.m_y = m_terrainScale;
                    scale.m_x = 1.0f;
                    scale.m_z = 1.0f;
                    static_cast<grGimmick*>(getGround(3))->setScale(&scale);
                }
                break;
            case 3:
                m_terrainScale += 0.01f * deltaFrame;
                if (m_terrainScale >= 1.0f) {
                    m_terrainScale = 1.0f;
                    m_eventTransform.setPhase(m_eventTransform.getPhase() + 1);
                    m_eventGrow.start();
                    zoomInCamera();
                    if (m_terrain == 7) {
                        getGround(9)->setEnableCollisionStatus(true);
                    }
                    if (m_terrain == 4) {
                        if (m_beltTrigger != NULL) {
                            m_beltTrigger->setAreaSleep(false);
                        }
                    }
                }
                {
                    Vec3f scale;
                    scale.m_y = m_terrainScale;
                    scale.m_x = 1.0f;
                    scale.m_z = 1.0f;
                    static_cast<grGimmick*>(getGround(m_terrain))->setScale(&scale);
                    static_cast<grGimmick*>(getGround(5))->setScale(&scale);
                    static_cast<grGimmick*>(getGround(6))->setScale(&scale);
                    static_cast<grGimmick*>(getGround(9))->setScale(&scale);
                    static_cast<grGimmick*>(getGround(10))->setScale(&scale);
                    static_cast<grGimmick*>(getGround(11))->setScale(&scale);
                    if (m_effectHandles[0] != -1) {
                        g_ecMgr->setScl(m_effectHandles[0], &scale);
                    }
                    if (m_effectHandles[2] != -1) {
                        g_ecMgr->setScl(m_effectHandles[2], &scale);
                    }
                    if (m_effectHandles[3] != -1) {
                        g_ecMgr->setScl(m_effectHandles[3], &scale);
                    }
                    if (m_effectHandles[4] != -1) {
                        g_ecMgr->setScl(m_effectHandles[4], &scale);
                    }
                }
                break;
            case 4:
                if (m_eventGrow.isReadyEnd() == true) {
                    if (!m_eventQuake.isEvent()) {
                        m_eventVision.end();
                        m_visionActive = false;
                        getGround(0)->setNodeVisibility(false, 0, "Dummy", false, false);
                        stadiumVision(this)->setDisplay(false);
                        m_eventScreen.start();
                        m_eventScreenHold.start();
                        m_eventTransform.setPhase(m_eventTransform.getPhase() + 1);
                        m_unk6F8 = true;
                    }
                }
                break;
            case 5:
                if (m_eventScreenHold.isReadyEnd() == true) {
                    m_eventScreenHold.end();
                    m_eventGrow.end();
                    m_eventTransform.setPhase(m_eventTransform.getPhase() + 1);
                    g_ecMgr->setEffect(ef_ptc_stg_dx_pstadium_kemuri);
                    m_eventQuake.start();
                    playSeBasic(snd_se_stage_Pstadium_jisin, 0.0f);
                    m_unk604 = -1;
                    m_unk608 = -1;
                    zoomOutCamera(300.0f, 310.0f);
                    if (m_terrain == 7) {
                        getGround(9)->setEnableCollisionStatus(false);
                    }
                    if (m_terrain == 4) {
                        if (m_beltTrigger != NULL) {
                            m_beltTrigger->setAreaSleep(true);
                        }
                    }
                }
                break;
            case 6:
                m_terrainScale -= 0.01f * deltaFrame;
                if (m_terrainScale <= 0.0f) {
                    m_terrainScale = 0.0001f;
                    m_eventTransform.setPhase(m_eventTransform.getPhase() + 1);
                    static_cast<grMadein*>(getGround(m_terrain))->endEntity();
                    getGround(m_terrain)->setEnableCollisionStatus(false);
                    static_cast<grMadein*>(getGround(3))->startEntityAutoLoop();
                    getGround(3)->setEnableCollisionStatus(true);
                    if (m_terrain == 2) {
                        static_cast<grMadein*>(getGround(11))->endEntity();
                    }
                    if (m_terrain == 7) {
                        static_cast<grMadein*>(getGround(5))->endEntity();
                        static_cast<grMadein*>(getGround(6))->endEntity();
                        static_cast<grMadein*>(getGround(9))->endEntity();
                        static_cast<grMadein*>(getGround(10))->endEntity();
                    }
                    void* posData = m_fileData->getData(Data_Type_Model, 100, 0xFFFE);
                    if (posData) {
                        nw4r::g3d::ResFile posFile(posData);
                        m_stagePositions->loadPositionData(&posFile);
                    }
                    updateStagePositions();
                    m_eventTransform.m_manualFramesLeft = 0.0f;
                    Vec3f scale;
                    scale.m_y = m_terrainScale;
                    scale.m_x = 1.0f;
                    scale.m_z = 1.0f;
                    static_cast<grGimmick*>(getGround(3))->setScale(&scale);
                    if (m_effectHandles[0] != -1) {
                        g_ecMgr->endEffect(m_effectHandles[0]);
                    }
                    if (m_effectHandles[1] != -1) {
                        g_ecMgr->endEffect(m_effectHandles[1]);
                    }
                    if (m_effectHandles[2] != -1) {
                        g_ecMgr->endEffect(m_effectHandles[2]);
                    }
                    if (m_effectHandles[3] != -1) {
                        g_ecMgr->endEffect(m_effectHandles[3]);
                    }
                    if (m_effectHandles[4] != -1) {
                        g_ecMgr->endEffect(m_effectHandles[4]);
                    }
                    m_effectHandles[0] = -1;
                    m_effectHandles[1] = -1;
                    m_effectHandles[2] = -1;
                    m_effectHandles[3] = -1;
                    m_effectHandles[4] = -1;
                    zoomInCamera();
                }
                {
                    Vec3f scale;
                    scale.m_y = m_terrainScale;
                    scale.m_x = 1.0f;
                    scale.m_z = 1.0f;
                    static_cast<grGimmick*>(getGround(m_terrain))->setScale(&scale);
                    static_cast<grGimmick*>(getGround(5))->setScale(&scale);
                    static_cast<grGimmick*>(getGround(6))->setScale(&scale);
                    static_cast<grGimmick*>(getGround(9))->setScale(&scale);
                    static_cast<grGimmick*>(getGround(10))->setScale(&scale);
                    static_cast<grGimmick*>(getGround(11))->setScale(&scale);
                    if (m_effectHandles[0] != -1) {
                        g_ecMgr->setScl(m_effectHandles[0], &scale);
                    }
                    if (m_effectHandles[2] != -1) {
                        g_ecMgr->setScl(m_effectHandles[2], &scale);
                    }
                    if (m_effectHandles[3] != -1) {
                        g_ecMgr->setScl(m_effectHandles[3], &scale);
                    }
                    if (m_effectHandles[4] != -1) {
                        g_ecMgr->setScl(m_effectHandles[4], &scale);
                    }
                }
                break;
            case 7:
                m_eventTransform.m_manualFramesLeft += deltaFrame;
                if (m_eventTransform.m_manualFramesLeft >= 60.0f) {
                    m_terrainScale += 0.01f * deltaFrame;
                    if (m_terrainScale >= 1.0f) {
                        m_terrainScale = 1.0f;
                        m_eventTransform.end();
                        m_eventPick.end();
                        m_eventPick.start();
                    }
                }
                {
                    Vec3f scale;
                    scale.m_y = m_terrainScale;
                    scale.m_x = 1.0f;
                    scale.m_z = 1.0f;
                    static_cast<grGimmick*>(getGround(3))->setScale(&scale);
                }
                break;
        }
        if (m_terrain == 7) {
            Vec3f rot;
            Vec3f pos;
            rot.m_z = m_unk5E8;
            rot.m_x = 0.0f;
            rot.m_y = 0.0f;
            pos.m_x = 0.0f;
            pos.m_y = 0.0f;
            pos.m_z = 0.0f;
            getGround(m_terrain)->getNodePosition(&pos, 0, "FunsuiAN");
            static_cast<grGimmick*>(getGround(5))->setPos(&pos);
            getGround(m_terrain)->getNodePosition(&pos, 0, "FunsuiBN");
            static_cast<grGimmick*>(getGround(6))->setPos(&pos);
            getGround(m_terrain)->getNodePosition(&pos, 0, "WingN");
            static_cast<grGimmick*>(getGround(9))->setRot(&rot);
            static_cast<grGimmick*>(getGround(9))->setPos(&pos);
            m_unk5E8 -= 0.5f;
        }
    }
}

void stDxPStadium::update(float deltaFrame) {
    g_ecMgr->setDrawPrio(1);
    if (m_eventPick.isReadyEnd() == true && !m_eventTransform.isEvent()) {
        switch (m_pickOrder[m_pickIndex]) {
            case 0:
                m_terrain = 1;
                break;
            case 1:
                m_terrain = 2;
                break;
            case 2:
                m_terrain = 4;
                break;
            case 3:
                m_terrain = 7;
                break;
        }
        m_eventTransform.start();
        m_pickIndex += 1;
        if (m_pickIndex >= 4) {
            m_pickIndex = 0;
            m_pickOrder[0] = 0;
            m_pickOrder[1] = 1;
            m_pickOrder[2] = 2;
            m_pickOrder[3] = 3;
            u32 a, b;
            for (int i = 0; i < 32; i++) {
                a = randi(4);
                if (a >= 3) {
                    a = 3;
                }
                b = randi(4);
                if (b >= 3) {
                    b = 3;
                }
                stDxPStadiumSwap(m_pickOrder[a], m_pickOrder[b]);
            }
        }
    }
    m_eventPick.update(deltaFrame);
    m_eventTransform.update(deltaFrame);
    m_eventGrow.update(deltaFrame);
    m_eventQuake.update(deltaFrame);
    m_eventScreen.update(deltaFrame);
    m_eventScreenHold.update(deltaFrame);
    m_eventVision.update(deltaFrame);
    if (m_eventVision.isEvent() == true) {
        switch (m_eventVision.getPhase()) {
            case 1:
                if (m_eventVision.isReadyEnd()) {
                    m_eventVision.end();
                    m_visionActive = false;
                    getGround(0)->setNodeVisibility(false, 0, "Dummy", false, false);
                    stadiumVision(this)->setDisplay(false);
                } else {
                    break;
                }
                // FALL-THROUGH
            case 0: {
                u32 choice = randi(2);
                if (choice >= 1) {
                    choice = 1;
                }
                m_visionCount += 1;
                m_eventVision.set(600.0f, 1200.0f);
                if (m_visionCount >= 7) {
                    m_visionCount = 0;
                    m_eventVision.set(300.0f, 300.0f);
                    choice = 2;
                }
                m_eventVision.end();
                m_visionActive = false;
                getGround(0)->setNodeVisibility(false, 0, "Dummy", false, false);
                stadiumVision(this)->setDisplay(false);
                m_eventVision.start();
                switch (choice) {
                    case 0:
                        m_visionZoomTarget = 0.6f + 0.9f * randf();
                        startPlayerVision();
                        getGround(0)->setNodeVisibility(false, 0, "Black", false, false);
                        break;
                    case 1:
                        stadiumVision(this)->m_screenMode = 0xE;
                        stadiumVision(this)->setDisplay(true);
                        getGround(0)->setNodeVisibility(true, 0, "Black", false, false);
                        break;
                    case 2:
                        stadiumVision(this)->m_screenMode = 0xD;
                        stadiumVision(this)->setDisplay(true);
                        getGround(0)->setNodeVisibility(true, 0, "Black", false, false);
                        break;
                }
                m_eventVision.setPhase(1);
                break;
            }
        }
    }
    updateVisionScreen();
    updateSpecialStage(deltaFrame);
    if (m_eventQuake.isEvent()) {
        if (m_eventQuake.isReadyEnd() == true) {
            cmRemoveQuake(1);
            m_eventQuake.end();
        } else {
            Vec3f offset;
            offset.m_x = 0.0f;
            offset.m_y = 0.0f;
            offset.m_z = 0.0f;
            cmReqQuake(cmQuake::Amplitude_M, &offset);
        }
    }
    updateVisionTerrain(deltaFrame);
    g_ecMgr->setDrawPrio(-1);
}

// The announcement of the terrain on the big screen: the screen first flashes the name of the terrain, holds it and
// then goes dark again (m_eventScreen phases 0-2).
void stDxPStadium::updateVisionTerrain(float deltaFrame) {
    if (m_eventScreen.isEvent()) {
        int screenKind = 0;
        int terrain = m_terrain;
        if (m_unk6F8 == true) {
            terrain = 3;
        }
        switch (terrain) {
            case 1:
                screenKind = 9;
                break;
            case 2:
                screenKind = 8;
                break;
            case 3:
                screenKind = 4;
                break;
            case 4:
                screenKind = 0xB;
                break;
            case 7:
                screenKind = 0xA;
                break;
        }
        stadiumVision(this)->m_screenKind = screenKind;
        if (terrain != 0) {
            switch (m_eventScreen.getPhase()) {
                case 0:
                    m_eventScreen.m_manualFramesLeft = 50.0f;
                    getGround(0)->setNodeVisibility(false, 0, "Black", false, false);
                    switch (terrain) {
                        case 3:
                            getGround(0)->setNodeVisibility(true, 0, "Normal", false, false);
                            break;
                        case 1:
                            getGround(0)->setNodeVisibility(true, 0, "Fire", false, false);
                            break;
                        case 2:
                            getGround(0)->setNodeVisibility(true, 0, "Grass", false, false);
                            break;
                        case 4:
                            getGround(0)->setNodeVisibility(true, 0, "Rock", false, false);
                            break;
                        case 7:
                            getGround(0)->setNodeVisibility(true, 0, "Water", false, false);
                            break;
                    }
                    m_eventScreen.setPhase(1);
                    playSeBasic(snd_se_state_aurora_vision, 0.0f);
                    break;
                case 1:
                    m_eventScreen.m_manualFramesLeft -= deltaFrame;
                    if (m_eventScreen.m_manualFramesLeft < 0.0f) {
                        m_eventScreen.m_manualFramesLeft = 0.0f;
                    }
                    if (0.0f == m_eventScreen.m_manualFramesLeft) {
                        getGround(0)->setNodeVisibility(true, 0, "Black", false, false);
                        switch (terrain) {
                            case 3:
                                getGround(0)->setNodeVisibility(false, 0, "Normal", false, false);
                                break;
                            case 1:
                                getGround(0)->setNodeVisibility(false, 0, "Fire", false, false);
                                break;
                            case 2:
                                getGround(0)->setNodeVisibility(false, 0, "Grass", false, false);
                                break;
                            case 4:
                                getGround(0)->setNodeVisibility(false, 0, "Rock", false, false);
                                break;
                            case 7:
                                getGround(0)->setNodeVisibility(false, 0, "Water", false, false);
                                break;
                        }
                        m_eventScreen.m_manualFramesLeft = 50.0f;
                        m_eventScreen.setPhase(2);
                        if (m_eventScreen.isReadyEnd() == true) {
                            m_eventScreen.end();
                            m_eventVision.end();
                            m_visionActive = false;
                            getGround(0)->setNodeVisibility(false, 0, "Dummy", false, false);
                            stadiumVision(this)->setDisplay(false);
                            m_eventVision.start();
                        }
                    }
                    break;
                case 2:
                    m_eventScreen.m_manualFramesLeft -= deltaFrame;
                    if (m_eventScreen.m_manualFramesLeft < 0.0f) {
                        m_eventScreen.m_manualFramesLeft = 0.0f;
                    }
                    if (0.0f == m_eventScreen.m_manualFramesLeft) {
                        m_eventScreen.setPhase(0);
                    }
                    break;
            }
        }
    }
}

// How long (in frames) each picture on the big screen stays up, by kind.
void stDxPStadium::setVision(u8 kind) {
    u16 frames[20] = {0, 120, 120, 120, 120, 120, 120, 60, 120, 255, 255, 255, 255, 255, 255, 120, 120, 120, 120, 120};
    stadiumVision(this)->setDisplay(false);
    m_visionActive = false;
    getGround(0)->setNodeVisibility(false, 0, "Dummy", false, false);
    stadiumVision(this)->m_screenMode = kind;
    stadiumVision(this)->setDisplay(true);
    m_eventVision.end();
    m_eventVision.set(frames[kind], frames[kind]);
    m_eventVision.start();
    m_eventVision.setPhase(1);
    getGround(0)->setNodeVisibility(true, 0, "Black", false, false);
}

// Puts one of the fighters in play on the big screen (round robin).
void stDxPStadium::startPlayerVision() {
    Vec3f pos;
    stadiumVision(this)->setDisplay(false);
    getGround(0)->setNodeVisibility(true, 0, "Dummy", false, false);
    int count = 0;
    int players[4] = {count, count, count, count};
    int* next = players;
    for (int i = 0; i < 4; i++) {
        if (getPlayerPosition(i, &pos) == true) {
            *next++ = i;
            count++;
        }
    }
    if (m_visionNext >= count) {
        m_visionNext = 0;
    }
    m_visionActive = true;
    m_visionPlayer = players[m_visionNext++];
    stDxPStadiumPlayerBytes* player = reinterpret_cast<stDxPStadiumPlayerBytes*>(&g_GameGlobal->m_modeMelee->m_playersInitData[m_visionPlayer]);
    stDxPStadiumMeleeFlags* flags = reinterpret_cast<stDxPStadiumMeleeFlags*>(reinterpret_cast<u8*>(g_GameGlobal->m_modeMelee) + 0xF);
    if (player->kind == Character_Jigglypuff && player->state == 3) {
        flags->jigglypuff = 1;
    } else {
        flags->jigglypuff = 0;
    }
}

// Follows the fighter shown on the big screen: smooths the corners of the fighter's camera box, projects them onto
// the screen and keeps the resulting rectangle (m_vision*) inside the 0-1 range with the aspect ratio of the screen.
void stDxPStadium::updateVisionRect() {
    cmSubject* subject = cmSubjectList::getSubjectByPlayerNo(m_visionPlayer);
    if (subject != NULL) {
        m_visionZoom += (m_visionZoomTarget - m_visionZoom) / 10.0f;
        Vec3f cornerA(subject->m_range.m_left * m_visionZoom, subject->m_range.m_down * m_visionZoom, 0.0f);
        Vec3f cornerB(subject->m_range.m_right * m_visionZoom, subject->m_range.m_up * m_visionZoom, 0.0f);
        Vec3fAdd(&cornerA, &cornerA, &subject->m_pos);
        Vec3fAdd(&cornerB, &cornerB, &subject->m_pos);
        float quarter = 4.0f;
        Vec3f deltaA;
        Vec3f scaledA;
        Vec3fSub(&deltaA, &cornerA, &m_visionPosA);
        stDxPStadiumVec3Scale(&scaledA, &deltaA, 1.0f / quarter);
        Vec3fAdd(&m_visionPosA, &m_visionPosA, &scaledA);
        Vec3f deltaB;
        Vec3f scaledB;
        Vec3fSub(&deltaB, &cornerB, &m_visionPosB);
        stDxPStadiumVec3Scale(&scaledB, &deltaB, 1.0f / quarter);
        Vec3fAdd(&m_visionPosB, &m_visionPosB, &scaledB);
        cornerA = m_visionPosA;
        cornerB = m_visionPosB;
        Vec2f screenA;
        Vec2f screenB;
        gfCamera* camera = &gfCameraManager::getManager()->m_cameras[0];
        camera->calcProjection3Dto2D(&cornerA, &screenA);
        camera->calcProjection3Dto2D(&cornerB, &screenB);
        screenA.m_x = screenA.m_x * (1.0f / 640.0f);
        screenA.m_y = 1.0f - (1.0f / 480.0f) * screenA.m_y;
        screenB.m_x = screenB.m_x * (1.0f / 640.0f);
        screenB.m_y = 1.0f - (1.0f / 480.0f) * screenB.m_y;
        if (screenA.m_x > screenB.m_x) {
            float tmp = screenA.m_x;
            screenA.m_x = screenB.m_x;
            screenB.m_x = tmp;
        }
        if (screenA.m_y > screenB.m_y) {
            float tmp = screenA.m_y;
            screenA.m_y = screenB.m_y;
            screenB.m_y = tmp;
        }
        float width = screenB.m_x - screenA.m_x;
        float height = screenB.m_y - screenA.m_y;
        width = __fsel(width - 0.08f, width, 0.08f);
        height = __fsel(height - 0.08f, height, 0.08f);
        float aspect = 1.0f;
        if (g_GameGlobal->getGlobalRecordMenuDatap()->m_isWidescreen) {
            aspect = 4.0f / 3.0f;
        }
        if (width / height > 11.0f / 7.0f) {
            height = width * aspect / (11.0f / 7.0f);
            if (height > 1.0f) {
                width = width / height;
                height = 1.0f;
            }
            if (width > 1.0f) {
                height = height / width;
                width = 1.0f;
            }
        } else {
            width = height * (11.0f / 7.0f) / aspect;
            if (width > 1.0f) {
                height = height / width;
                width = 1.0f;
            }
            if (height > 1.0f) {
                width = width / height;
                height = 1.0f;
            }
        }
        Vec2f center = (screenA + screenB) * 0.5f;
        Vec2f rectMin(center.m_x - 0.5f * width, center.m_y - 0.5f * height);
        Vec2f rectMax(center.m_x + 0.5f * width, center.m_y + 0.5f * height);
        if (rectMin.m_x < 0.0f) {
            rectMin.m_x = 0.0f;
            rectMax.m_x = width;
        }
        if (rectMin.m_y < 0.0f) {
            rectMin.m_y = 0.0f;
            rectMax.m_y = height;
        }
        if (rectMax.m_x > 1.0f) {
            rectMax.m_x = 1.0f;
            rectMin.m_x = 1.0f - width;
        }
        if (rectMax.m_y > 1.0f) {
            rectMax.m_y = 1.0f;
            rectMin.m_y = 1.0f - height;
        }
        m_visionLeft = rectMin.m_x;
        m_visionTop = rectMin.m_y;
        m_visionRight = rectMax.m_x;
        m_visionBottom = rectMax.m_y;
    }
}

// Maps the rectangle found by updateVisionRect onto the big screen: the "MDummy" material shows the screen-copy of
// the fight, scaled and shifted so that only the rectangle around the fighter is visible.
void stDxPStadium::updateVisionScreen() {
    if (m_visionActive) {
        Vec2f sum;
        sum.m_x = m_visionLeft + m_visionRight;
        sum.m_y = m_visionTop + m_visionBottom;
        float width = m_visionRight - m_visionLeft;
        Vec2f center = sum * 0.5f;
        float height = m_visionBottom - m_visionTop;
        nw4r::g3d::ResMdl resMdl = getGround(0)->m_sceneModels[0]->m_resMdl;
        nw4r::g3d::ResMat resMat = resMdl.GetResMat("MDummy");
        nw4r::g3d::ResTexSrt texSrt(NULL);
        nw4r::g3d::ResTexObj texObj(reinterpret_cast<u8*>(resMat.ptr()) + 0x3C);
        GXTexObj* screenTex = texObj.GetTexObj(GX_TEXMAP0);
        if (gfCopyEFBMgr::getInstance()->isValid(0) == true) {
            *screenTex = *gfCopyEFBMgr::getInstance()->getCopyEFBTex(0);
        }
        texSrt = nw4r::g3d::ResTexSrt(reinterpret_cast<u8*>(resMat.ptr()) + 0x1A4);
        texSrt.SetMapMode(0, 0, -1, -1);
        nw4r::g3d::ResTexSrtData* srt = texSrt.ptr();
        srt->m_range.m_x = width;
        srt->m_range.m_y = height;
        srt->m_pos.m_x = -(center.m_x - 0.5f * width) / width;
        srt->m_pos.m_y = -(center.m_y - 0.5f * height) / height;
        texSrt->m_flags = (texSrt->m_flags & ~0xF) | 5;
        updateVisionRect();
    }
}
