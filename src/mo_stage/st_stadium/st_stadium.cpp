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

#include <st_stadium/st_stadium.h>

// HYPOTHESIS: bit 0 of the byte at +0xF of the melee init data is set while the big screen shows Jigglypuff.
struct stStadiumMeleeFlags {
    u8 pad : 7;
    u8 jigglypuff : 1;
};
// MATCH-ONLY: the original reads the first two bytes of a player's init data (character kind, state) as bytes.
struct stStadiumPlayerBytes {
    u8 kind;
    u8 state;
};
// HYPOTHESIS: the stage parameter file (m_stageData): timings of the terrain changes.
struct stStadiumParam {
    char _0[0x2C];
    float m_pickMin;         // 0x2C
    float m_pickMax;         // 0x30
    char _34[8];
    float m_transformFrames; // 0x3C: frames the terrain needs to grow / shrink
};

static inline void stStadiumSwap(u8& x, u8& y) {
    u8 tmp = x;
    x = y;
    y = tmp;
}

// MATCH-ONLY: the original scales vectors with paired singles (inline asm in the shared vector code).
static inline void stStadiumVec3Scale(register Vec3f* pOut, register const Vec3f* v, register float c) {
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

// MATCH-ONLY: the original computes max(a, b) with a paired-single subtract and select.
static inline float stStadiumPsMax(register float a, register float b) {
    register float diff, result;
    // clang-format off
    asm {
        ps_sub diff, a, b
        ps_sel result, diff, a, b
    }
    // clang-format on
    return result;
}

// The big screen is ground 0 and always a grStadiumVision.
static inline grStadiumVision* stadiumVision(stStadium* stage) {
    return static_cast<grStadiumVision*>(stage->getGround(0));
}

stClassInfoImpl<Stages::Stadium, stStadium> stStadium::bss_loc_14;

stStadium* stStadium::create() {
    return new (Heaps::StageInstance) stStadium;
}

stStadium::stStadium() : stMelee("stStadium", Stages::Stadium) {
    m_seHandleA = -1;
    m_seHandleB = -1;
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
        stStadiumSwap(m_pickOrder[a], m_pickOrder[b]);
    }
    m_unk1D8 = 0;
    m_terrainScale = 0.0f;
    m_seQuake = -1;
    m_normalSymbol = 0;
    m_visionActive = 0;
    m_visionZoomTarget = 1.0f;
    m_visionNext = 0;
    m_visionCount = 0;
    m_lastChoice = 0xFF;
    m_visionZoom = 1.0f;
    m_visionPosA.m_x = 0.0f;
    m_visionPosA.m_y = 0.0f;
    m_visionPosA.m_z = 0.0f;
    m_visionPosB.m_x = 0.0f;
    m_visionPosB.m_y = 0.0f;
    m_visionPosB.m_z = 0.0f;
    m_visionMin.m_x = 0.0f;
    m_visionMin.m_y = 0.0f;
    m_visionMax.m_x = 1.0f;
    m_visionMax.m_y = 1.0f;
    m_flyZ[0] = 0.0f;
    m_flyZ[1] = 0.0f;
    m_flyZ[2] = 0.0f;
    m_grow[0] = 1.0f;
    m_grow[1] = 1.0f;
    m_grow[2] = 1.0f;
    m_unk6A0 = 0;
    m_unk6A4 = 0.0f;
    m_unk6A8 = 0;
    m_beltDataA = static_cast<grGimmickBeltConveyorData*>(operator new(sizeof(grGimmickBeltConveyorData), Heaps::StageInstance));
    m_beltDataB = static_cast<grGimmickBeltConveyorData*>(operator new(sizeof(grGimmickBeltConveyorData), Heaps::StageInstance));
}

stStadium::~stStadium() {
    operator delete(m_beltDataA);
    operator delete(m_beltDataB);
    releaseArchive();
    reinterpret_cast<stStadiumMeleeFlags*>(reinterpret_cast<u8*>(g_GameGlobal->m_modeMelee) + 0xF)->jigglypuff = 0;
}

bool stStadium::loading() {
    return true;
}

void stStadium::createObj() {
    testStageParamInit(m_fileData, 10);
    testStageDataInit(m_fileData, 0x14, 0x70);
    addGround(grStadiumVision::create(3, "", "grStadiumAuroraVision"));
    addGround(grMadein::create(0x14, "OVDenki", "", Heaps::StageInstance));
    addGround(grMadein::create(0x16, "OVTuchi", "", Heaps::StageInstance));
    addGround(grMadein::create(0x15, "OVKoori", "", Heaps::StageInstance));
    addGround(grMadein::create(0x17, "OVHikou", "", Heaps::StageInstance));
    addGround(grMadein::create(0x18, "OVNormal", "", Heaps::StageInstance));
    addGround(grMadein::create(4, "", "grStadiumSearchLight", Heaps::StageInstance));
    addGround(grMadein::create(0, "", "grStadiumMainBg", Heaps::StageInstance));
    addGround(grMadein::create(1, "StgStadium00AshibaL", "grStadiumAshibaL", Heaps::StageInstance));
    addGround(grMadein::create(2, "StgStadium00AshibaR", "grStadiumAshibaR", Heaps::StageInstance));
    addGround(grMadein::create(5, "StgStadium00Denki", "grStadiumDenki", Heaps::StageInstance));
    addGround(grMadein::create(6, "StgStadium00DenkiRing", "grStadiumDenkiRing", Heaps::StageInstance));
    addGround(grMadein::create(7, "StgStadium00Koori", "grStadiumKoori", Heaps::StageInstance));
    addGround(grMadein::create(8, "StgStadium00Tuchi", "grStadiumTuchi", Heaps::StageInstance));
    addGround(grMadein::create(9, "StgStadium00Hikou", "grStadiumHikou", Heaps::StageInstance));
    addGround(grMadein::create(0xA, "Airmd", "", Heaps::StageInstance));
    addGround(grMadein::create(0xB, "Dugtrio", "", Heaps::StageInstance));
    addGround(grMadein::create(0xC, "Elekible", "", Heaps::StageInstance));
    addGround(grMadein::create(0xD, "Fuwante", "", Heaps::StageInstance));
    addGround(grMadein::create(0xE, "Hanecco", "", Heaps::StageInstance));
    addGround(grMadein::create(0xF, "Jibacoil", "", Heaps::StageInstance));
    addGround(grMadein::create(0x10, "Karakara", "", Heaps::StageInstance));
    addGround(grMadein::create(0x11, "Yukikaburi", "", Heaps::StageInstance));
    addGround(grMadein::create(0x12, "Yukiwarashi", "", Heaps::StageInstance));
    addGround(grMadein::create(0x13, "Conveyor", "", Heaps::StageInstance));
    addGround(grMadein::create(0xC8, "DenkiAshiba", "", Heaps::StageInstance));
    addGround(grMadein::create(0xC9, "KooriAshiba", "", Heaps::StageInstance));
    u32 i = 0;
    u32 groundNum = getGroundNum();
    Ground* ground;
    for (; i != groundNum; i++) {
        ground = getGround(i);
        if (ground != NULL) {
            ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
            ground->setStageData(m_stageData);
        }
    }
    createCollision(m_fileData, 2, NULL);
    createCollision(m_fileData, 3, NULL);
    createCollision(m_fileData, 4, NULL);
    createCollision(m_fileData, 5, getGround(0xD));
    createCollision(m_fileData, 6, getGround(0xE));
    for (u32 i = 1; i < getGroundNum(); i++) {
        static_cast<grMadein*>(getGround(i))->initializeEntity();
    }
    Vec3f beltPos;
    {
        stTrigger* trigger;
        grGimmickBeltConveyorData* belt = m_beltDataA;
        beltPos.m_x = -65.0f;
        beltPos.m_y = 0.0f;
        beltPos.m_z = 0.0f;
        memset(belt, 0, sizeof(grGimmickBeltConveyorData));
        belt->m_pos = beltPos;
        belt->m_speed = 0.7f;
        belt->m_isRight = false;
        belt->m_areaData.m_offsetPos.m_x = 0.0f;
        belt->m_areaData.m_offsetPos.m_y = 0.0f;
        belt->m_areaData.m_range.m_x = 75.0f;
        belt->m_areaData.m_range.m_y = 10.0f;
        trigger = g_stTriggerMng->createTrigger(Gimmick::Area_BeltConveyor, -1);
        trigger->setBeltConveyorTrigger(belt);
        m_beltTriggerA = trigger;
    }
    {
        stTrigger* trigger;
        grGimmickBeltConveyorData* belt = m_beltDataB;
        beltPos.m_x = 65.0f;
        beltPos.m_y = 0.0f;
        beltPos.m_z = 0.0f;
        memset(belt, 0, sizeof(grGimmickBeltConveyorData));
        belt->m_pos = beltPos;
        belt->m_speed = 0.7f;
        belt->m_isRight = true;
        belt->m_areaData.m_offsetPos.m_x = 0.0f;
        belt->m_areaData.m_offsetPos.m_y = 0.0f;
        belt->m_areaData.m_range.m_x = 75.0f;
        belt->m_areaData.m_range.m_y = 10.0f;
        trigger = g_stTriggerMng->createTrigger(Gimmick::Area_BeltConveyor, -1);
        trigger->setBeltConveyorTrigger(belt);
        m_beltTriggerB = trigger;
    }
    m_beltTriggerB->setAreaSleep(true);
    m_beltTriggerA->setAreaSleep(true);
    static_cast<grMadein*>(getGround(6))->startEntityAutoLoop();
    static_cast<grMadein*>(getGround(7))->startEntityAutoLoop();
    static_cast<grMadein*>(getGround(8))->startEntityAutoLoop();
    static_cast<grMadein*>(getGround(9))->startEntityAutoLoop();
    getGround(0x19)->setEnableCollisionStatus(false);
    getGround(0x1A)->setEnableCollisionStatus(false);
    getGround(0xA)->setEnableCollisionStatus(false);
    getGround(0xC)->setEnableCollisionStatus(false);
    getGround(0xD)->setEnableCollisionStatus(false);
    getGround(0xE)->setEnableCollisionStatus(false);
    initCameraParam();
    void* posData = m_fileData->getData(Data_Type_Model, 100, 0xFFFE);
    if (posData) {
        nw4r::g3d::ResFile posFile(posData);
        createStagePositions(&posFile);
    } else {
        createStagePositions();
    }
    createWind2ndOnly();
    if (m_wind2ndTrigger != NULL) {
        m_wind2ndTrigger->setAreaSleep(true);
    }
    stStadiumParam* param = static_cast<stStadiumParam*>(m_stageData);
    m_eventTransform.set(0.0f, 0.0f);
    m_eventPick.set(param->m_pickMin, param->m_pickMax);
    m_eventGrow.set(1200.0f, 1800.0f);
    m_eventVision.set(600.0f, 600.0f);
    m_eventScreen.set(600.0f, 600.0f);
    m_eventScreenHold.set(300.0f, 300.0f);
    m_eventQuake.set(200.0f, 200.0f);
    m_eventHazard.set(400.0f, 800.0f);
    m_eventPick.start();
    m_eventVision.start();
    registScnAnim(static_cast<nw4r::g3d::ResFileData*>(m_fileData->getData(Data_Type_Scene, 0, 0xFFFE)), 0);
    initPosPokeTrainer(1, 0);
    createObjPokeTrainer(m_fileData, 110, "PokeTrainer00", m_pokeTrainerPos, NULL);
    loadStageAttrParam(m_fileData, 30);
    stadiumVision(this)->m_screenKind = 4;
    void* miscData = m_fileData->getData(Data_Type_Misc, 0x28, 0xFFFE);
    stadiumVision(this)->m_miscData = miscData;
    m_eventVision.end();
    m_visionActive = 0;
    getGround(0)->setNodeVisibility(false, 0, "AuroraVision", false, false);
    getGround(0)->setNodeVisibility(false, 0, "AuroraVision9monitor", false, false);
    stadiumVision(this)->setDisplay(false);
    playSeBasic(snd_se_stage_Stadium_02, 0.0f);
}

void stStadium::createObjDetails() { }


// The transformation of the stage (m_eventTransform), one phase per step:
//   0 wait for the quake to end, then start the terrain symbol on the big screen
//   1 once it has been shown the old terrain shakes (quake + sound)
//   2 shrink the old terrain, bring the new one in with its Pokemon
//   3 grow the new terrain; when it has grown the stage settles (camera zooms in)
//   4 the Pokemon grow in one group after the other and play their motions
//   5-7 the same thing in reverse once the terrain has been around for a while
void stStadium::updateSpecialStage(float deltaFrame) {
    stStadiumParam* param = static_cast<stStadiumParam*>(m_stageData);
    if (m_eventTransform.isEvent()) {
        switch (m_eventTransform.getPhase()) {
            case 0:
                if (!m_eventQuake.isEvent()) {
                    m_eventVision.end();
                    m_visionActive = 0;
                    getGround(0)->setNodeVisibility(false, 0, "AuroraVision", false, false);
                    getGround(0)->setNodeVisibility(false, 0, "AuroraVision9monitor", false, false);
                    stadiumVision(this)->setDisplay(false);
                    m_eventScreen.start();
                    m_eventScreenHold.start();
                    m_eventTransform.setPhase(m_eventTransform.getPhase() + 1);
                    m_normalSymbol = 0;
                    float z0 = 30.0f + 50.0f * randf();
                    float z1 = 20.0f + 50.0f * randf();
                    float z2 = 10.0f + 50.0f * randf();
                    m_flyZ[1] = z1;
                    m_flyZ[2] = z0;
                    m_grow[0] = 0.001f;
                    m_flyZ[0] = z2;
                    m_grow[1] = 0.001f;
                    m_grow[2] = 0.001f;
                }
                break;
            case 1:
                if (m_eventScreenHold.isReadyEnd() == true) {
                    m_eventScreenHold.end();
                    m_terrainScale = 1.0f;
                    m_eventTransform.setPhase(m_eventTransform.getPhase() + 1);
                    g_ecMgr->setEffect(ef_ptc_stg_stadium_kemuri);
                    m_eventQuake.start();
                    m_seQuake = playSeBasic(snd_se_stage_Stadium_01, 0.0f);
                    zoomOutCamera(300.0f, 310.0f);
                }
                break;
            case 2:
                m_terrainScale -= (1.0f / param->m_transformFrames) * deltaFrame;
                if (m_terrainScale <= 0.0f) {
                    m_terrainScale = 0.0001f;
                    m_eventTransform.setPhase(m_eventTransform.getPhase() + 1);
                    static_cast<grMadein*>(getGround(8))->endEntity();
                    static_cast<grMadein*>(getGround(9))->endEntity();
                    getGround(8)->setEnableCollisionStatus(false);
                    getGround(9)->setEnableCollisionStatus(false);
                    static_cast<grMadein*>(getGround(m_terrain))->startEntityAutoLoop();
                    getGround(m_terrain)->setEnableCollisionStatus(true);
                    Vec3f scale;
                    scale.m_y = m_terrainScale;
                    scale.m_x = 1.0f;
                    scale.m_z = 1.0f;
                    static_cast<grGimmick*>(getGround(m_terrain))->setScale(&scale);
                    if (m_terrain == 0xA) {
                        static_cast<grGimmick*>(getGround(0xB))->setScale(&scale);
                        static_cast<grGimmick*>(getGround(0x18))->setScale(&scale);
                        static_cast<grGimmick*>(getGround(0x19))->setScale(&scale);
                        getGround(0x19)->setEnableCollisionStatus(true);
                    }
                    if (m_terrain == 0xC) {
                        static_cast<grGimmick*>(getGround(0x1A))->setScale(&scale);
                        getGround(0x1A)->setEnableCollisionStatus(true);
                    }
                    if (m_terrain == 0xA) {
                        static_cast<grMadein*>(getGround(0xB))->startEntityAutoLoop();
                        static_cast<grMadein*>(getGround(0x18))->startEntityAutoLoop();
                        static_cast<grMadein*>(getGround(0x19))->startEntityAutoLoop();
                    }
                    if (m_terrain == 0xC) {
                        static_cast<grMadein*>(getGround(0x1A))->startEntityAutoLoop();
                    }
                    if (m_terrain == 0xE) {
                        static_cast<grGimmick*>(getGround(0xE))->setPos(0.0f, -0.5f, 0.0f);
                    }
                    switch (m_terrain) {
                        case 0xA: {
                            m_eventHazard.end();
                            m_eventHazard.start();
                            playSeBasic(snd_se_stage_Stadium_05, 0.0f);
                            static_cast<grMadein*>(getGround(0x14))->setMotion(0);
                            static_cast<grMadein*>(getGround(0x14))->startEntity();
                            u32 pick = randi(2);
                            if (pick >= 1) {
                                pick = 1;
                            }
                            static_cast<grMadein*>(getGround(0x11))->setMotion((u8)pick);
                            static_cast<grMadein*>(getGround(0x11))->startEntity();
                            break;
                        }
                        case 0xC: {
                            playSeBasic(snd_se_stage_Stadium_04, 0.0f);
                            u32 pick = randi(3);
                            if (pick >= 2) {
                                pick = 2;
                            }
                            static_cast<grMadein*>(getGround(0x16))->setMotion((u8)pick);
                            static_cast<grMadein*>(getGround(0x16))->startEntity();
                            pick = randi(2);
                            if (pick >= 1) {
                                pick = 1;
                            }
                            static_cast<grMadein*>(getGround(0x17))->setMotion((u8)pick);
                            static_cast<grMadein*>(getGround(0x17))->startEntity();
                            break;
                        }
                        case 0xD: {
                            playSeBasic(snd_se_stage_Stadium_03, 0.0f);
                            u32 pick = randi(3);
                            if (pick >= 2) {
                                pick = 2;
                            }
                            static_cast<grMadein*>(getGround(0x15))->setMotion((u8)pick);
                            static_cast<grMadein*>(getGround(0x15))->startEntity();
                            pick = randi(2);
                            if (pick >= 1) {
                                pick = 1;
                            }
                            static_cast<grMadein*>(getGround(0x10))->setMotion((u8)(pick + 2));
                            static_cast<grMadein*>(getGround(0x10))->startEntity();
                            break;
                        }
                        case 0xE: {
                            Vec3f rot;
                            rot.m_x = 0.0f;
                            rot.m_y = 30.0f;
                            rot.m_z = 0.0f;
                            m_seHandleA = playSeBasic(snd_se_stage_Stadium_08, 0.0f);
                            static_cast<grGimmick*>(getGround(0xF))->setRot(&rot);
                            static_cast<grMadein*>(getGround(0xF))->startEntityAutoLoop();
                            u32 pick = randi(2);
                            if (pick >= 1) {
                                pick = 1;
                            }
                            static_cast<grMadein*>(getGround(0x12))->setMotion((u8)pick);
                            static_cast<grMadein*>(getGround(0x12))->startEntity();
                            static_cast<grMadein*>(getGround(0x13))->startEntityAutoLoop();
                            break;
                        }
                    }
                    zoomInCamera();
                }
                {
                    Vec3f scale;
                    scale.m_y = m_terrainScale;
                    scale.m_x = 1.0f;
                    scale.m_z = 1.0f;
                    static_cast<grGimmick*>(getGround(8))->setScale(&scale);
                    static_cast<grGimmick*>(getGround(9))->setScale(&scale);
                }
                break;
            case 3:
                m_terrainScale += (1.0f / param->m_transformFrames) * deltaFrame;
                if (m_terrainScale >= 1.0f) {
                    m_terrainScale = 1.0f;
                    m_eventTransform.setPhase(m_eventTransform.getPhase() + 1);
                    m_eventGrow.start();
                    switch (m_terrain) {
                        case 0xA:
                            m_seHandleA = playSeBasic(snd_se_stage_Stadium_06, 0.0f);
                            m_seHandleB = playSeBasic(snd_se_stage_Stadium_07, 0.0f);
                            m_beltTriggerA->setAreaSleep(false);
                            m_beltTriggerB->setAreaSleep(false);
                            playSeBasic(snd_se_stage_Stadium_electro_finish, 0.0f);
                            {
                                void* posData = m_fileData->getData(Data_Type_Model, 0x65, 0xFFFE);
                                if (posData) {
                                    nw4r::g3d::ResFile posFile(posData);
                                    m_stagePositions->loadPositionData(&posFile);
                                }
                            }
                            break;
                        case 0xE:
                            setGravityHalf();
                            static_cast<grGimmick*>(getGround(0xE))->setPos(0.0f, 0.0f, 0.0f);
                            if (m_wind2ndTrigger != NULL) {
                                m_wind2ndTrigger->setAreaSleep(false);
                            }
                            {
                                void* posData = m_fileData->getData(Data_Type_Model, 0x68, 0xFFFE);
                                if (posData) {
                                    nw4r::g3d::ResFile posFile(posData);
                                    m_stagePositions->loadPositionData(&posFile);
                                }
                            }
                            break;
                        case 0xD:
                            playSeBasic(snd_se_stage_Stadium_ground_finish, 0.0f);
                            {
                                void* posData = m_fileData->getData(Data_Type_Model, 0x67, 0xFFFE);
                                if (posData) {
                                    nw4r::g3d::ResFile posFile(posData);
                                    m_stagePositions->loadPositionData(&posFile);
                                }
                            }
                            break;
                        case 0xC:
                            playSeBasic(snd_se_stage_Stadium_ice_finish, 0.0f);
                            {
                                void* posData = m_fileData->getData(Data_Type_Model, 0x66, 0xFFFE);
                                if (posData) {
                                    nw4r::g3d::ResFile posFile(posData);
                                    m_stagePositions->loadPositionData(&posFile);
                                }
                            }
                            break;
                    }
                    updateStagePositions();
                }
                {
                    Vec3f scale;
                    scale.m_y = m_terrainScale;
                    scale.m_x = 1.0f;
                    scale.m_z = 1.0f;
                    static_cast<grGimmick*>(getGround(m_terrain))->setScale(&scale);
                    if (m_terrain == 0xA) {
                        static_cast<grGimmick*>(getGround(0xB))->setScale(&scale);
                        static_cast<grGimmick*>(getGround(0x18))->setScale(&scale);
                        static_cast<grGimmick*>(getGround(0x19))->setScale(&scale);
                    }
                    if (m_terrain == 0xC) {
                        static_cast<grGimmick*>(getGround(0x1A))->setScale(&scale);
                    }
                    if (m_terrain == 0xE) {
                        static_cast<grGimmick*>(getGround(0xE))->setPos(0.0f, -0.5f + 0.5f * m_terrainScale, 0.0f);
                    }
                }
                break;
            case 4:
                m_grow[0] += 0.08f;
                if (m_grow[0] >= 1.0f) {
                    m_grow[0] = 1.0f;
                }
                if (1.0f == m_grow[0]) {
                    m_grow[1] += 0.04f;
                    if (m_grow[1] >= 1.0f) {
                        m_grow[1] = 1.0f;
                    }
                }
                if (1.0f == m_grow[1]) {
                    m_grow[2] += 0.02f;
                    if (m_grow[2] >= 1.0f) {
                        m_grow[2] = 1.0f;
                    }
                }
                switch (m_terrain) {
                    case 0xA: {
                        grMadein* pokemon = static_cast<grMadein*>(getGround(0x14));
                        if (pokemon != NULL && pokemon->isEndEntity()) {
                            u32 pick = randi(2);
                            if (pick >= 1) {
                                pick = 1;
                            }
                            pokemon->setMotion((u8)pick);
                            pokemon->startEntity();
                        }
                        pokemon = static_cast<grMadein*>(getGround(0x11));
                        if (pokemon != NULL && pokemon->isEndEntity()) {
                            u32 pick = randi(2);
                            if (pick >= 1) {
                                pick = 1;
                            }
                            pokemon->setMotion((u8)pick);
                            pokemon->startEntity();
                        }
                        break;
                    }
                    case 0xC: {
                        grMadein* pokemon = static_cast<grMadein*>(getGround(0x16));
                        if (pokemon != NULL && pokemon->isEndEntity()) {
                            u32 pick = randi(3);
                            if (pick >= 2) {
                                pick = 2;
                            }
                            pokemon->setMotion((u8)pick);
                            pokemon->startEntity();
                        }
                        pokemon = static_cast<grMadein*>(getGround(0x17));
                        if (pokemon != NULL && pokemon->isEndEntity()) {
                            u32 pick = randi(2);
                            if (pick >= 1) {
                                pick = 1;
                            }
                            pokemon->setMotion((u8)pick);
                            pokemon->startEntity();
                        }
                        break;
                    }
                    case 0xD: {
                        grMadein* pokemon = static_cast<grMadein*>(getGround(0x15));
                        if (pokemon != NULL && pokemon->isEndEntity()) {
                            u32 pick = randi(3);
                            if (pick >= 2) {
                                pick = 2;
                            }
                            pokemon->setMotion((u8)pick);
                            pokemon->startEntity();
                        }
                        pokemon = static_cast<grMadein*>(getGround(0x10));
                        if (pokemon != NULL && pokemon->isEndEntity()) {
                            u32 pick = randi(2);
                            if (pick >= 1) {
                                pick = 1;
                            }
                            pokemon->setMotion((u8)(pick + 2));
                            pokemon->startEntity();
                        }
                        break;
                    }
                    case 0xE: {
                        grMadein* pokemon = static_cast<grMadein*>(getGround(0x12));
                        if (pokemon != NULL && pokemon->isEndEntity()) {
                            u32 pick = randi(2);
                            if (pick >= 1) {
                                pick = 1;
                            }
                            pokemon->setMotion((u8)pick);
                            pokemon->startEntity();
                        }
                        break;
                    }
                }
                if (m_eventGrow.isReadyEnd() == true) {
                    if (!m_eventQuake.isEvent()) {
                        if (1.0f == m_grow[0] && 1.0f == m_grow[1] && 1.0f == m_grow[2]) {
                            m_eventVision.end();
                            m_visionActive = 0;
                            getGround(0)->setNodeVisibility(false, 0, "AuroraVision", false, false);
                            getGround(0)->setNodeVisibility(false, 0, "AuroraVision9monitor", false, false);
                            stadiumVision(this)->setDisplay(false);
                            m_eventScreen.start();
                            m_eventScreenHold.start();
                            m_eventTransform.setPhase(m_eventTransform.getPhase() + 1);
                            m_normalSymbol = 1;
                        }
                    }
                }
                break;
            case 5:
                m_grow[2] -= 0.08f;
                if (m_grow[2] < 0.001f) {
                    m_grow[2] = 0.001f;
                }
                if (0.001f == m_grow[2]) {
                    m_grow[1] -= 0.04f;
                    if (m_grow[1] < 0.001f) {
                        m_grow[1] = 0.001f;
                    }
                }
                if (0.001f == m_grow[1]) {
                    m_grow[0] -= 0.02f;
                    if (m_grow[0] < 0.001f) {
                        m_grow[0] = 0.001f;
                    }
                }
                if (m_eventScreenHold.isReadyEnd() == true) {
                    if (0.001f == m_grow[0] && 0.001f == m_grow[1] && 0.001f == m_grow[2]) {
                        m_eventScreenHold.end();
                        m_eventGrow.end();
                        m_eventTransform.setPhase(m_eventTransform.getPhase() + 1);
                        g_ecMgr->setEffect(ef_ptc_stg_stadium_kemuri);
                        m_eventQuake.start();
                        m_seQuake = playSeBasic(snd_se_stage_Stadium_01, 0.0f);
                        stopSeBasic(m_seHandleA, 4.0f);
                        stopSeBasic(m_seHandleB, 4.0f);
                        m_seHandleA = -1;
                        m_seHandleB = -1;
                        zoomOutCamera(300.0f, 310.0f);
                        setGravityNormal();
                    }
                }
                break;
            case 6:
                m_terrainScale -= (1.0f / param->m_transformFrames) * deltaFrame;
                if (m_terrainScale <= 0.0f) {
                    m_terrainScale = 0.0001f;
                    m_eventTransform.setPhase(m_eventTransform.getPhase() + 1);
                    static_cast<grMadein*>(getGround(m_terrain))->endEntity();
                    getGround(m_terrain)->setEnableCollisionStatus(false);
                    if (m_wind2ndTrigger != NULL) {
                        m_wind2ndTrigger->setAreaSleep(true);
                    }
                    if (m_terrain == 0xA) {
                        m_beltTriggerA->setAreaSleep(true);
                        m_beltTriggerB->setAreaSleep(true);
                    }
                    if (m_terrain == 0xA) {
                        static_cast<grMadein*>(getGround(0xB))->endEntity();
                        static_cast<grMadein*>(getGround(0x18))->endEntity();
                        static_cast<grMadein*>(getGround(0x19))->endEntity();
                        getGround(0x19)->setEnableCollisionStatus(false);
                    }
                    if (m_terrain == 0xC) {
                        static_cast<grMadein*>(getGround(0x1A))->endEntity();
                        getGround(0x1A)->setEnableCollisionStatus(false);
                    }
                    static_cast<grMadein*>(getGround(8))->startEntityAutoLoop();
                    static_cast<grMadein*>(getGround(9))->startEntityAutoLoop();
                    getGround(8)->setEnableCollisionStatus(true);
                    getGround(9)->setEnableCollisionStatus(true);
                    {
                        Vec3f scale;
                        scale.m_y = m_terrainScale;
                        scale.m_x = 1.0f;
                        scale.m_z = 1.0f;
                        static_cast<grGimmick*>(getGround(8))->setScale(&scale);
                        static_cast<grGimmick*>(getGround(9))->setScale(&scale);
                    }
                    static_cast<grMadein*>(getGround(0xF))->endEntity();
                    static_cast<grMadein*>(getGround(0x10))->endEntity();
                    static_cast<grMadein*>(getGround(0x11))->endEntity();
                    static_cast<grMadein*>(getGround(0x12))->endEntity();
                    static_cast<grMadein*>(getGround(0x13))->endEntity();
                    static_cast<grMadein*>(getGround(0x14))->endEntity();
                    static_cast<grMadein*>(getGround(0x15))->endEntity();
                    static_cast<grMadein*>(getGround(0x16))->endEntity();
                    static_cast<grMadein*>(getGround(0x17))->endEntity();
                    zoomInCamera();
                }
                {
                    Vec3f scale;
                    scale.m_y = m_terrainScale;
                    scale.m_x = 1.0f;
                    scale.m_z = 1.0f;
                    static_cast<grGimmick*>(getGround(m_terrain))->setScale(&scale);
                    if (m_terrain == 0xA) {
                        static_cast<grGimmick*>(getGround(0xB))->setScale(&scale);
                        static_cast<grGimmick*>(getGround(0x18))->setScale(&scale);
                        static_cast<grGimmick*>(getGround(0x19))->setScale(&scale);
                    }
                    if (m_terrain == 0xC) {
                        static_cast<grGimmick*>(getGround(0x1A))->setScale(&scale);
                    }
                }
                break;
            case 7:
                m_terrainScale += (1.0f / param->m_transformFrames) * deltaFrame;
                if (m_terrainScale >= 1.0f) {
                    m_terrainScale = 1.0f;
                    m_eventTransform.end();
                    m_eventPick.end();
                    m_eventPick.start();
                    playSeBasic(snd_se_stage_Stadium_02, 0.0f);
                    void* posData = m_fileData->getData(Data_Type_Model, 100, 0xFFFE);
                    if (posData) {
                        nw4r::g3d::ResFile posFile(posData);
                        m_stagePositions->loadPositionData(&posFile);
                    }
                    updateStagePositions();
                }
                {
                    Vec3f scale;
                    scale.m_y = m_terrainScale;
                    scale.m_x = 1.0f;
                    scale.m_z = 1.0f;
                    static_cast<grGimmick*>(getGround(8))->setScale(&scale);
                    static_cast<grGimmick*>(getGround(9))->setScale(&scale);
                }
                break;
        }
    }
    // The Pokemon of the current terrain follow their nodes in the field model, scaled by their grow factor.
    Vec3f posA, posB, posC;
    Vec3f scaleA, scaleB, scaleC;
    switch (m_terrain) {
        case 0xA: {
            float growA = 1.1f * m_grow[0];
            scaleA.m_x = growA;
            scaleA.m_y = growA;
            scaleA.m_z = growA;
            float growB = 1.4f * m_grow[1];
            scaleB.m_x = growB;
            scaleB.m_y = growB;
            scaleB.m_z = growB;
            getGround(m_terrain)->getNodePosition(&posA, 0, "Jibacoil1");
            getGround(m_terrain)->getNodePosition(&posB, 0, "Elekible1");
            static_cast<grGimmick*>(getGround(0x14))->setPos(&posA);
            static_cast<grGimmick*>(getGround(0x11))->setPos(&posB);
            static_cast<grGimmick*>(getGround(0x14))->setScale(&scaleA);
            static_cast<grGimmick*>(getGround(0x11))->setScale(&scaleB);
            break;
        }
        case 0xC: {
            float growA = 1.4f * m_grow[0];
            scaleA.m_x = growA;
            scaleA.m_y = growA;
            scaleA.m_z = growA;
            float growB = 1.6f * m_grow[1];
            scaleB.m_x = growB;
            scaleB.m_y = growB;
            scaleB.m_z = growB;
            getGround(m_terrain)->getNodePosition(&posA, 0, "Yukikaburi1");
            getGround(m_terrain)->getNodePosition(&posB, 0, "Yukiwarashi1");
            static_cast<grGimmick*>(getGround(0x16))->setPos(&posA);
            static_cast<grGimmick*>(getGround(0x17))->setPos(&posB);
            static_cast<grGimmick*>(getGround(0x16))->setScale(&scaleA);
            static_cast<grGimmick*>(getGround(0x17))->setScale(&scaleB);
            break;
        }
        case 0xD: {
            float growA = 1.85f * m_grow[0];
            scaleA.m_x = growA;
            scaleA.m_y = growA;
            scaleA.m_z = growA;
            float growB = 1.6f * m_grow[1];
            scaleB.m_x = growB;
            scaleB.m_y = growB;
            scaleB.m_z = growB;
            getGround(m_terrain)->getNodePosition(&posA, 0, "Karakara1");
            getGround(m_terrain)->getNodePosition(&posB, 0, "Dugtorio1");
            static_cast<grGimmick*>(getGround(0x15))->setPos(&posA);
            static_cast<grGimmick*>(getGround(0x10))->setPos(&posB);
            static_cast<grGimmick*>(getGround(0x15))->setScale(&scaleA);
            static_cast<grGimmick*>(getGround(0x10))->setScale(&scaleB);
            break;
        }
        case 0xE: {
            float growA = 1.6f * m_grow[0];
            scaleA.m_x = growA;
            scaleA.m_y = growA;
            scaleA.m_z = growA;
            float growB = 2.5f * m_grow[1];
            scaleB.m_x = growB;
            scaleB.m_y = growB;
            scaleB.m_z = growB;
            float growC = 2.1f * m_grow[2];
            scaleC.m_x = growC;
            scaleC.m_y = growC;
            scaleC.m_z = growC;
            getGround(m_terrain)->getNodePosition(&posA, 0, "Airmd1");
            getGround(m_terrain)->getNodePosition(&posB, 0, "Fuwante1");
            getGround(m_terrain)->getNodePosition(&posC, 0, "Hanecco1");
            posA.m_z = m_flyZ[0];
            posB.m_z = m_flyZ[1];
            posC.m_z = m_flyZ[2];
            static_cast<grGimmick*>(getGround(0xF))->setPos(&posA);
            static_cast<grGimmick*>(getGround(0x12))->setPos(&posB);
            static_cast<grGimmick*>(getGround(0x13))->setPos(&posC);
            static_cast<grGimmick*>(getGround(0xF))->setScale(&scaleA);
            static_cast<grGimmick*>(getGround(0x12))->setScale(&scaleB);
            static_cast<grGimmick*>(getGround(0x13))->setScale(&scaleC);
            break;
        }
    }
}

// The symbol of the new terrain on the big screen: it flashes up, holds and goes dark again (m_eventScreen phases 0-2).
void stStadium::updateSymbol(float deltaFrame) {
    if (m_eventScreen.isEvent()) {
        int groundIndex = 0;
        int symbol = 0;
        switch (m_terrain) {
            case 0xA:
                groundIndex = 1;
                symbol = 0;
                break;
            case 0xD:
                groundIndex = 2;
                symbol = 1;
                break;
            case 0xC:
                groundIndex = 3;
                symbol = 2;
                break;
            case 0xE:
                groundIndex = 4;
                symbol = 3;
                break;
        }
        if (m_normalSymbol == 1) {
            groundIndex = 5;
            symbol = 4;
        }
        stadiumVision(this)->m_screenKind = symbol;
        if (groundIndex != 0) {
            switch (m_eventScreen.getPhase()) {
                case 0:
                    m_eventScreen.m_manualFramesLeft = 50.0f;
                    static_cast<grMadein*>(getGround(groundIndex))->startEntity();
                    m_eventScreen.setPhase(1);
                    playSeBasic(snd_se_stage_Stadium_09, 0.0f);
                    break;
                case 1:
                    m_eventScreen.m_manualFramesLeft -= deltaFrame;
                    if (m_eventScreen.m_manualFramesLeft < 0.0f) {
                        m_eventScreen.m_manualFramesLeft = 0.0f;
                    }
                    if (0.0f == m_eventScreen.m_manualFramesLeft) {
                        static_cast<grMadein*>(getGround(groundIndex))->endEntity();
                        m_eventScreen.m_manualFramesLeft = 50.0f;
                        m_eventScreen.setPhase(2);
                        if (m_eventScreen.isReadyEnd() == true) {
                            m_eventScreen.end();
                            m_eventVision.end();
                            m_visionActive = 0;
                            getGround(0)->setNodeVisibility(false, 0, "AuroraVision", false, false);
                            getGround(0)->setNodeVisibility(false, 0, "AuroraVision9monitor", false, false);
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

void stStadium::notifyEventInfoReady() { }

void stStadium::notifyEventInfoGo() { }

// How long (in frames) each picture on the big screen stays up, by kind.
void stStadium::setVision(u8 kind) {
    u16 frames[20] = {0, 120, 120, 120, 120, 120, 120, 60, 120, 255, 255, 255, 255, 255, 255, 120, 120, 120, 120, 120};
    stadiumVision(this)->setDisplay(false);
    m_visionActive = 0;
    getGround(0)->setNodeVisibility(false, 0, "AuroraVision", false, false);
    getGround(0)->setNodeVisibility(false, 0, "AuroraVision9monitor", false, false);
    stadiumVision(this)->m_screenMode = kind;
    stadiumVision(this)->setDisplay(true);
    m_eventVision.end();
    m_eventVision.set(frames[kind], frames[kind]);
    m_eventVision.start();
    m_eventVision.setPhase(1);
}

void stStadium::update(float deltaFrame) {
    if (m_eventPick.isReadyEnd() == true && !m_eventTransform.isEvent()) {
        switch (m_pickOrder[m_pickIndex]) {
            case 0:
                m_terrain = 0xA;
                break;
            case 1:
                m_terrain = 0xD;
                break;
            case 2:
                m_terrain = 0xC;
                break;
            case 3:
                m_terrain = 0xE;
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
                stStadiumSwap(m_pickOrder[a], m_pickOrder[b]);
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
    m_eventHazard.update(deltaFrame);
    if (m_eventVision.isEvent() == true) {
        switch (m_eventVision.getPhase()) {
            case 1:
                if (m_eventVision.isReadyEnd()) {
                    m_eventVision.end();
                    m_visionActive = 0;
                    getGround(0)->setNodeVisibility(false, 0, "AuroraVision", false, false);
                    getGround(0)->setNodeVisibility(false, 0, "AuroraVision9monitor", false, false);
                    stadiumVision(this)->setDisplay(false);
                } else {
                    break;
                }
                // FALL-THROUGH
            case 0: {
                u32 rnd = randi(2);
                if (rnd >= 1) {
                    rnd = 1;
                }
                u32 choice = rnd;
                m_visionCount += 1;
                m_eventVision.set(600.0f, 1200.0f);
                if (m_visionCount >= 7) {
                    m_visionCount = 0;
                    choice = 2;
                    m_eventVision.set(300.0f, 300.0f);
                }
                m_eventVision.end();
                m_visionActive = 0;
                getGround(0)->setNodeVisibility(false, 0, "AuroraVision", false, false);
                getGround(0)->setNodeVisibility(false, 0, "AuroraVision9monitor", false, false);
                stadiumVision(this)->setDisplay(false);
                m_eventVision.start();
                switch (choice) {
                    case 0:
                        m_visionZoomTarget = 0.6f + 0.9f * randf();
                        enableVisionScreen();
                        break;
                    case 1:
                        stadiumVision(this)->m_screenMode = 0xE;
                        stadiumVision(this)->setDisplay(true);
                        if (m_lastChoice != choice) {
                            playSeBasic(snd_se_stage_Stadium_10, 0.0f);
                        }
                        break;
                    case 2:
                        stadiumVision(this)->m_screenMode = 0xD;
                        stadiumVision(this)->setDisplay(true);
                        if (m_lastChoice != choice) {
                            playSeBasic(snd_se_stage_Stadium_10, 0.0f);
                        }
                        break;
                }
                m_lastChoice = choice;
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
            stopSeBasic(m_seQuake, 2.0f);
            m_seQuake = -1;
            m_eventQuake.end();
        } else {
            Vec3f offset;
            offset.m_x = 0.0f;
            offset.m_y = 0.0f;
            offset.m_z = 0.0f;
            cmReqQuake(cmQuake::Amplitude_M, &offset);
        }
    }
    updateSymbol(deltaFrame);
}

// Puts one of the fighters in play on the big screen (round robin).
void stStadium::enableVisionScreen() {
    stadiumVision(this)->setDisplay(false);
    getGround(0)->setNodeVisibility(true, 0, "AuroraVision", false, false);
    getGround(0)->setNodeVisibility(false, 0, "AuroraVision9monitor", false, false);
    Vec3f pos;
    int* next;
    int count = 0;
    int players[4] = {count, count, count, count};
    next = players;
    for (int i = 0; i < 4; i++) {
        if (getPlayerPosition(i, &pos) == true) {
            *next++ = i;
            count++;
        }
    }
    if (m_visionNext >= count) {
        m_visionNext = 0;
    }
    u32 index = m_visionNext;
    m_visionActive = 1;
    m_visionNext = index + 1;
    m_visionPlayer = players[index];
    stStadiumPlayerBytes* player = reinterpret_cast<stStadiumPlayerBytes*>(&g_GameGlobal->m_modeMelee->m_playersInitData[m_visionPlayer]);
    stStadiumMeleeFlags* flags = reinterpret_cast<stStadiumMeleeFlags*>(reinterpret_cast<u8*>(g_GameGlobal->m_modeMelee) + 0xF);
    if (player->kind == Character_Jigglypuff && player->state == 3) {
        flags->jigglypuff = 1;
    } else {
        flags->jigglypuff = 0;
    }
}

// Follows the fighter shown on the big screen: smooths the corners of the fighter's camera box, projects them onto
// the screen and keeps the resulting rectangle (m_vision*) inside the 0-1 range with the aspect ratio of the screen.
void stStadium::updateVisionScreenPos() {
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
        stStadiumVec3Scale(&scaledA, &deltaA, 1.0f / quarter);
        Vec3fAdd(&m_visionPosA, &m_visionPosA, &scaledA);
        Vec3f deltaB;
        Vec3f scaledB;
        Vec3fSub(&deltaB, &cornerB, &m_visionPosB);
        stStadiumVec3Scale(&scaledB, &deltaB, 1.0f / quarter);
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
        width = stStadiumPsMax(width, 0.08f);
        height = stStadiumPsMax(height, 0.08f);
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
        m_visionMin = rectMin;
        m_visionMax = rectMax;
    }
}

// Maps the rectangle found by updateVisionRect onto the big screen: the "MoniterDummy1" material shows the screen-copy of
// the fight, scaled and shifted so that only the rectangle around the fighter is visible.
void stStadium::updateVisionScreen() {
    if (m_visionActive) {
        Vec2f center = (m_visionMin + m_visionMax) * 0.5f;
        float width = m_visionMax.m_x - m_visionMin.m_x;
        float height = m_visionMax.m_y - m_visionMin.m_y;
        nw4r::g3d::ResMdl resMdl = getGround(0)->m_sceneModels[0]->m_resMdl;
        nw4r::g3d::ResMat resMat = resMdl.GetResMat("MoniterDummy1");
        nw4r::g3d::ResTexObj texObj(reinterpret_cast<u8*>(resMat.ptr()) + 0x3C);
        GXTexObj* screenTex = texObj.GetTexObj(GX_TEXMAP0);
        if (gfCopyEFBMgr::getInstance()->isValid(0) == true) {
            *screenTex = *gfCopyEFBMgr::getInstance()->getCopyEFBTex(0);
        }
        nw4r::g3d::ResTexSrt texSrt(reinterpret_cast<u8*>(resMat.ptr()) + 0x1A4);
        texSrt.SetMapMode(0, 0, -1, -1);
        nw4r::g3d::ResTexSrtData* srt = texSrt.ptr();
        srt->m_range.m_x = width;
        srt->m_range.m_y = height;
        srt->m_pos.m_x = -(center.m_x - 0.5f * width) / width;
        srt->m_pos.m_y = -(center.m_y - 0.5f * height) / height;
        texSrt->m_flags = (texSrt->m_flags & ~0xF) | 5;
        updateVisionScreenPos();
    }
}
