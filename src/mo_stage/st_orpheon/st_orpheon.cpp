#include <ai/ai_mgr.h>
#include <ft/ft_manager.h>
#include <gf/gf_3d_scene.h>
#include <gf/gf_archive.h>
#include <gm/gm_lib.h>
#include <gr/gr_madein.h>
#include <gr/gr_tengan_event.h>
#include <memory.h>
#include <mt/mt_prng.h>
#include <mt/mt_spline.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <snd/snd_id.h>
#include <st/se_util.h>
#include <st/st_class_info.h>
#include <st/st_melee.h>
#include <types.h>

#include <st_orpheon/st_orpheon.h>

// Sound tables of the stage: the Queen's voice (m_seQueen) and the spin rumble (m_seSpin).
SndID data_loc_ids0[] = {snd_se_stage_Orpheon_Queen_vc};

StSeUtil::UnkStruct data_loc_seq0[] = {
    {snd_se_stage_Orpheon_Queen_vc, 0.0f, 270.0f, 0.0f},
};

SndID data_loc_ids1[] = {snd_se_stage_Orpheon_09};

StSeUtil::UnkStruct data_loc_seq1[] = {
    {snd_se_stage_Orpheon_09, 0.0f, 14.0f, 0.0f},
    {snd_se_stage_Orpheon_09, 0.0f, 64.0f, 0.0f},
    {snd_se_stage_Orpheon_09, 0.0f, 107.0f, 0.0f},
    {snd_se_stage_Orpheon_09, 0.0f, 151.0f, 0.0f},
};

stClassInfoImpl<Stages::Orpheon, stOrpheon> stOrpheon::bss_loc_14;

stOrpheon* stOrpheon::create() {
    return new (Heaps::StageInstance) stOrpheon;
}

stOrpheon::stOrpheon() : stMelee("stOrpheon", Stages::Orpheon) {
    m_spinAngle = 0.0f;
    m_spinTarget = 180.0f;
    m_queenPathT = 0.0f;
    m_queenRoll = 0.0f;
    m_trap1PathT = 0.0f;
    m_trap2APathT = 0.0f;
    m_trap2BPathT = 0.0f;
    m_trap1Wait = 0.0f;
    m_trap2AWait = 0.0f;
    m_trap2BWait = 0.0f;
    m_unk608 = 0.0f;
    m_unk60C = 0.0f;
    m_unk610 = 0.0f;
    m_queenMotion = 0;
    m_isSpinning = true;
    m_isSpun = false;
    m_repeatCount = 0;
    m_lastChoice = 0;
}

stOrpheon::~stOrpheon() {
    releaseArchive();
}

bool stOrpheon::loading() {
    return true;
}

void stOrpheon::createObj() {
    testStageParamInit(m_fileData, 10);
    testStageDataInit(m_fileData, 20, 0x58);
    addGround(grMadein::create(0, "", "Base", Heaps::StageInstance));
    addGround(grMadein::create(1, "", "Bg", Heaps::StageInstance));
    addGround(grMadein::create(2, "", "PQ", Heaps::StageInstance));
    addGround(grMadein::create(3, "", "Move1", Heaps::StageInstance));
    addGround(grMadein::create(4, "", "Move2A", Heaps::StageInstance));
    addGround(grMadein::create(5, "", "Move2B", Heaps::StageInstance));
    Ground* ground;
    for (u32 i = 0, groundNum = getGroundNum(); i != groundNum; i++) {
        ground = getGround(i);
        if (ground != NULL) {
            ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
            ground->setStageData(m_stageData);
        }
    }
    createCollision(m_fileData, 2, NULL);
    getGround(0)->setNodeCollision(true, 0, 1, false);
    getGround(0)->setNodeCollision(false, 0, 8, false);
    getGround(3)->setEnableCollisionStatus(true);
    getGround(4)->setEnableCollisionStatus(false);
    getGround(5)->setEnableCollisionStatus(false);
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
    for (u32 i = 0; i < getGroundNum(); i++) {
        static_cast<grMadein*>(getGround(i))->initializeEntity();
        static_cast<grMadein*>(getGround(i))->startEntityAutoLoop();
    }
    Vec3f scale;
    scale.m_x = 0.9f;
    scale.m_y = 0.9f;
    scale.m_z = 0.9f;
    static_cast<grMadein*>(getGround(2))->setScale(&scale);
    m_eventCall.set(1200.0f, 1800.0f);
    m_eventSpin.set(900.0f, 1200.0f);
    m_eventPower.set(600.0f, 1800.0f);
    m_eventTrap1.set(0.0f, 0.0f);
    m_eventTrap2A.set(0.0f, 0.0f);
    m_eventTrap2B.set(0.0f, 0.0f);
    m_eventCall.start();
    m_eventTrap1.start();
    m_eventTrap2A.start();
    m_eventTrap2B.start();
    loadStageAttrParam(m_fileData, 30);
    initPosPokeTrainer(4, 0);
    createObjPokeTrainer(m_fileData, 101, "PokeTrainer00", m_pokeTrainerPos, NULL);
    createObjPokeTrainer(m_fileData, 102, "PokeTrainer01", m_pokeTrainerPos + 2, NULL);
    createObjPokeTrainer(m_fileData, 103, "PokeTrainer02", m_pokeTrainerPos + 4, NULL);
    createObjPokeTrainer(m_fileData, 104, "PokeTrainer03", m_pokeTrainerPos + 6, NULL);
    m_seQueen.registId(data_loc_ids0, 1);
    m_seQueen.registSeq(0, data_loc_seq0, 1, Heaps::StageInstance);
    m_seSpin.registId(data_loc_ids1, 1);
    m_seSpin.registSeq(0, data_loc_seq1, 4, Heaps::StageInstance);
    playSeBasic(snd_se_stage_Orpheon_06, 0.0f);
    if (!isPokemonTrainer()) {
        getGround(0)->setNodeVisibility(false, 0, "PT_Ashiba01", false, false);
        getGround(0)->setNodeVisibility(false, 0, "PT_Ashiba02", false, false);
    }
}

// The stage rolls over: after a quiet period the Queen shakes the ship, the whole stage rotates by 180 degrees (the
// floors swap which side is solid) and the Queen's head follows.
void stOrpheon::eventSpinStage(float deltaFrame) {
    if (m_eventSpin.isEvent()) {
        switch (m_eventSpin.getPhase()) {
            case 0:
                if (m_eventSpin.isReadyEnd()) {
                    registScnAnim(static_cast<nw4r::g3d::ResFileData*>(m_fileData->getData(Data_Type_Scene, 4, 0xFFFE)), 0);
                    m_eventSpin.setPhase(m_eventSpin.getPhase() + 1);
                    m_eventSpin.m_manualFramesLeft = 0.0f;
                }
                break;
            case 1:
                m_seSpin.playFrame(0, m_eventSpin.m_manualFramesLeft);
                m_eventSpin.m_manualFramesLeft += deltaFrame;
                if (m_eventSpin.m_manualFramesLeft >= 180.0f) {
                    m_eventSpin.setPhase(m_eventSpin.getPhase() + 1);
                    zoomOutCamera(400.0f, 440.0f);
                    setJointCliff(false);
                    m_eventSpin.m_manualFramesLeft = 0.0f;
                    registScnAnim(static_cast<nw4r::g3d::ResFileData*>(m_fileData->getData(Data_Type_Scene, 0, 0xFFFE)), 0);
                }
                break;
            case 2:
                m_eventSpin.setPhase(m_eventSpin.getPhase() + 1);
                break;
            case 3:
                m_eventSpin.m_manualFramesLeft += deltaFrame;
                if (m_eventSpin.m_manualFramesLeft >= 30.0f) {
                    m_eventSpin.setPhase(m_eventSpin.getPhase() + 1);
                    m_eventSpin.m_manualFramesLeft = 0.0f;
                    m_unk608 = 0.0f;
                    m_unk60C = 0.0f;
                    m_unk610 = 0.0f;
                    playSeBasic(snd_se_stage_Orpheon_04, 0.0f);
                    m_isSpinning = false;
                    setVisibilityTrainer(false);
                }
                break;
            case 4: {
                float diff = m_spinTarget - m_spinAngle;
                if ((float)fabs(diff) < 90.0f && !m_isSpinning) {
                    m_isSpinning = true;
                    if (!m_isSpun) {
                        getGround(0)->setNodeCollision(false, 0, 1, false);
                        getGround(0)->setNodeCollision(true, 0, 8, false);
                        getGround(3)->setEnableCollisionStatus(false);
                        getGround(4)->setEnableCollisionStatus(true);
                        getGround(5)->setEnableCollisionStatus(true);
                        m_isSpun = true;
                        void* posData = m_fileData->getData(Data_Type_Model, 0x69, 0xFFFE);
                        if (posData) {
                            nw4r::g3d::ResFile posFile(posData);
                            m_stagePositions->loadPositionData(&posFile);
                        }
                        updateStagePositions();
                    } else {
                        getGround(0)->setNodeCollision(true, 0, 1, false);
                        getGround(0)->setNodeCollision(false, 0, 8, false);
                        getGround(3)->setEnableCollisionStatus(true);
                        getGround(4)->setEnableCollisionStatus(false);
                        getGround(5)->setEnableCollisionStatus(false);
                        m_isSpun = false;
                        void* posData = m_fileData->getData(Data_Type_Model, 0x64, 0xFFFE);
                        if (posData) {
                            nw4r::g3d::ResFile posFile(posData);
                            m_stagePositions->loadPositionData(&posFile);
                        }
                        updateStagePositions();
                    }
                }
                if ((float)fabs(diff) > 0.1f) {
                    m_spinAngle += diff / 10.0f * deltaFrame;
                } else {
                    m_spinAngle = m_spinTarget;
                    m_eventSpin.setPhase(m_eventSpin.getPhase() + 1);
                    if (m_spinTarget == 0.0f) {
                        m_spinTarget = 180.0f;
                        u32 flip = randi(2);
                        if (flip >= 1) {
                            flip = 1;
                        }
                        if (flip == 0) {
                            m_spinTarget *= -1.0f;
                        }
                    } else {
                        m_spinTarget = 0.0f;
                    }
                    zoomInCamera();
                    setJointCliff(true);
                    setVisibilityTrainer(true);
                    m_eventSpin.m_manualFramesLeft = 0.0f;
                }
                break;
            }
            case 5:
                m_eventSpin.m_manualFramesLeft += deltaFrame;
                if (m_eventSpin.m_manualFramesLeft >= 30.0f) {
                    m_eventSpin.setPhase(m_eventSpin.getPhase() + 1);
                }
                break;
            case 6:
                if (m_spinTarget == 0.0f) {
                    m_queenRoll += 6.0f * deltaFrame;
                    if (m_queenRoll >= 180.0f) {
                        m_queenRoll = 180.0f;
                        m_eventSpin.setPhase(m_eventSpin.getPhase() + 1);
                        m_queenMotion = 1;
                        static_cast<grMadein*>(getGround(2))->setMotion(m_queenMotion);
                        static_cast<grMadein*>(getGround(2))->startEntity();
                        playSeBasic(snd_se_stage_Orpheon_05, 0.0f);
                    }
                } else {
                    m_queenRoll -= 6.0f * deltaFrame;
                    if (m_queenRoll < 0.0f) {
                        m_queenRoll = 0.0f;
                        m_eventSpin.setPhase(m_eventSpin.getPhase() + 1);
                        m_queenMotion = 1;
                        static_cast<grMadein*>(getGround(2))->setMotion(m_queenMotion);
                        static_cast<grMadein*>(getGround(2))->startEntity();
                    }
                }
                break;
            case 7:
                if (m_spinTarget == 0.0f) {
                    m_queenPathT += 0.01f * deltaFrame;
                    if (m_queenPathT >= 1.0f) {
                        m_queenPathT = 1.0f;
                        if (static_cast<grMadein*>(getGround(2))->isEndEntity()) {
                            m_eventSpin.setPhase(m_eventSpin.getPhase() + 1);
                            m_queenMotion = 0;
                            static_cast<grMadein*>(getGround(2))->setMotion(m_queenMotion);
                            static_cast<grMadein*>(getGround(2))->startEntityAutoLoop();
                            playSeBasic(snd_se_stage_Orpheon_Queen_stop, 0.0f);
                            m_seQueen.playFrame(m_queenMotion, 0.0f, 0.0f);
                        }
                    }
                } else {
                    m_queenPathT -= 0.01f * deltaFrame;
                    if (m_queenPathT < 0.0f) {
                        m_queenPathT = 0.0f;
                        if (static_cast<grMadein*>(getGround(2))->isEndEntity()) {
                            m_eventSpin.setPhase(m_eventSpin.getPhase() + 1);
                            m_queenMotion = 0;
                            static_cast<grMadein*>(getGround(2))->setMotion(m_queenMotion);
                            static_cast<grMadein*>(getGround(2))->startEntityAutoLoop();
                            playSeBasic(snd_se_stage_Orpheon_Queen_stop, 0.0f);
                            m_seQueen.playFrame(m_queenMotion, 0.0f, 0.0f);
                        }
                    }
                }
                break;
            case 8:
                m_eventSpin.end();
                break;
        }
        m_eventSpin.update(deltaFrame);
    }
}

// "Move1": a trap floor that slides out along a path, waits at the half-way point, slides on, and back again.
void stOrpheon::eventTrapFloor1(float deltaFrame) {
    switch (m_eventTrap1.getPhase()) {
        case 0:
            m_trap1PathT = 0.0f;
            m_eventTrap1.setPhase(m_eventTrap1.getPhase() + 1);
            break;
        case 1:
            m_trap1PathT += 0.005f * deltaFrame;
            if (m_trap1PathT >= 0.505f) {
                m_trap1PathT = 0.505f;
                m_eventTrap1.setPhase(m_eventTrap1.getPhase() + 1);
                m_eventTrap1.m_manualFramesLeft = 0.0f;
                m_trap1Wait = 120.0f + __fabsf(180.0f - 360.0f * randf());
            }
            break;
        case 2:
            m_eventTrap1.m_manualFramesLeft += deltaFrame;
            if (m_eventTrap1.m_manualFramesLeft >= m_trap1Wait) {
                m_eventTrap1.setPhase(m_eventTrap1.getPhase() + 1);
            }
            break;
        case 3:
            m_trap1PathT += 0.005f * deltaFrame;
            if (m_trap1PathT >= 1.0f) {
                m_trap1PathT = 1.0f;
                m_eventTrap1.setPhase(m_eventTrap1.getPhase() + 1);
                m_eventTrap1.m_manualFramesLeft = 0.0f;
                m_trap1Wait = 120.0f + __fabsf(180.0f - 360.0f * randf());
            }
            break;
        case 4:
            m_eventTrap1.m_manualFramesLeft += deltaFrame;
            if (m_eventTrap1.m_manualFramesLeft >= m_trap1Wait) {
                m_eventTrap1.setPhase(m_eventTrap1.getPhase() + 1);
            }
            break;
        case 5:
            m_trap1PathT -= 0.005f * deltaFrame;
            if (m_trap1PathT < 0.505f) {
                m_trap1PathT = 0.505f;
                m_eventTrap1.m_manualFramesLeft = 0.0f;
                m_eventTrap1.setPhase(m_eventTrap1.getPhase() + 1);
                m_trap1Wait = 120.0f + __fabsf(180.0f - 360.0f * randf());
            }
            break;
        case 6:
            m_eventTrap1.m_manualFramesLeft += deltaFrame;
            if (m_eventTrap1.m_manualFramesLeft >= m_trap1Wait) {
                m_eventTrap1.setPhase(m_eventTrap1.getPhase() + 1);
            }
            break;
        case 7:
            m_trap1PathT -= 0.005f * deltaFrame;
            if (m_trap1PathT < 0.0f) {
                m_eventTrap1.m_manualFramesLeft = 0.0f;
                m_trap1PathT = 0.0f;
                m_eventTrap1.setPhase(m_eventTrap1.getPhase() + 1);
                m_trap1Wait = 120.0f + __fabsf(180.0f - 360.0f * randf());
            }
            break;
        case 8:
            m_eventTrap1.m_manualFramesLeft += deltaFrame;
            if (m_eventTrap1.m_manualFramesLeft >= m_trap1Wait) {
                m_eventTrap1.end();
                m_eventTrap1.start();
            }
            break;
    }
    m_eventTrap1.update(deltaFrame);
}

// "Move2A": the second trap floor, one pass out and one pass back.
void stOrpheon::eventTrapFloor2A(float deltaFrame) {
    switch (m_eventTrap2A.getPhase()) {
        case 0:
            m_trap2APathT = 0.0f;
            m_eventTrap2A.setPhase(m_eventTrap2A.getPhase() + 1);
            break;
        case 1:
            m_trap2APathT += 0.005f * deltaFrame;
            if (m_trap2APathT >= 1.0f) {
                m_trap2APathT = 1.0f;
                m_eventTrap2A.setPhase(m_eventTrap2A.getPhase() + 1);
                m_eventTrap2A.m_manualFramesLeft = 0.0f;
                float randOffset = 240.0f * randf();
                float extraWait = 120.0f - randOffset;
                m_trap2AWait = 120.0f + extraWait;
            }
            break;
        case 2:
            m_eventTrap2A.m_manualFramesLeft += deltaFrame;
            if (m_eventTrap2A.m_manualFramesLeft >= m_trap2AWait) {
                m_eventTrap2A.setPhase(m_eventTrap2A.getPhase() + 1);
            }
            break;
        case 3:
            m_trap2APathT -= 0.005f * deltaFrame;
            if (m_trap2APathT < 0.0f) {
                m_eventTrap2A.m_manualFramesLeft = 0.0f;
                m_trap2APathT = 0.0f;
                m_eventTrap2A.setPhase(m_eventTrap2A.getPhase() + 1);
                m_trap2AWait = 60.0f + (120.0f - 240.0f * randf());
            }
            break;
        case 4:
            m_eventTrap2A.m_manualFramesLeft += deltaFrame;
            if (m_eventTrap2A.m_manualFramesLeft >= m_trap2AWait) {
                m_eventTrap2A.end();
                m_eventTrap2A.start();
            }
            break;
    }
    m_eventTrap2A.update(deltaFrame);
}

// "Move2B": same as Move2A with the absolute value of the random wait.
void stOrpheon::eventTrapFloor2B(float deltaFrame) {
    switch (m_eventTrap2B.getPhase()) {
        case 0:
            m_trap2BPathT = 0.0f;
            m_eventTrap2B.setPhase(m_eventTrap2B.getPhase() + 1);
            break;
        case 1:
            m_trap2BPathT += 0.005f * deltaFrame;
            if (m_trap2BPathT >= 1.0f) {
                m_trap2BPathT = 1.0f;
                m_eventTrap2B.setPhase(m_eventTrap2B.getPhase() + 1);
                m_eventTrap2B.m_manualFramesLeft = 0.0f;
                float randOffset = 240.0f * randf();
                float extraWait = 120.0f - randOffset;
                extraWait = __fabsf(extraWait);
                m_trap2BWait = 120.0f + extraWait;
            }
            break;
        case 2:
            m_eventTrap2B.m_manualFramesLeft += deltaFrame;
            if (m_eventTrap2B.m_manualFramesLeft >= m_trap2BWait) {
                m_eventTrap2B.setPhase(m_eventTrap2B.getPhase() + 1);
            }
            break;
        case 3:
            m_trap2BPathT -= 0.005f * deltaFrame;
            if (m_trap2BPathT < 0.0f) {
                m_eventTrap2B.m_manualFramesLeft = 0.0f;
                m_trap2BPathT = 0.0f;
                m_eventTrap2B.setPhase(m_eventTrap2B.getPhase() + 1);
                m_trap2BWait = 60.0f + __fabsf(120.0f - 240.0f * randf());
            }
            break;
        case 4:
            m_eventTrap2B.m_manualFramesLeft += deltaFrame;
            if (m_eventTrap2B.m_manualFramesLeft >= m_trap2BWait) {
                m_eventTrap2B.end();
                m_eventTrap2B.start();
            }
            break;
    }
    m_eventTrap2B.update(deltaFrame);
}

// The power failure: the lights go out in four steps (scene animations 1, 2, 3, back to 0) while the Queen's nose
// pokes in, but never during the stage roll.
void stOrpheon::eventPowerFailure(float deltaFrame) {
    if (m_eventPower.isEvent()) {
        switch (m_eventPower.getPhase()) {
            case 0:
                if (!m_eventSpin.isEvent() && m_eventPower.isReadyEnd()) {
                    g_gfSceneRoot->removeResAnmScn();
                    registScnAnim(static_cast<nw4r::g3d::ResFileData*>(m_fileData->getData(Data_Type_Scene, 1, 0xFFFE)), 0);
                    static_cast<grMadein*>(getGround(1))->setMotionDetails(0, 0, 0, 0, 1);
                    static_cast<grMadein*>(getGround(1))->startEntity();
                    m_eventPower.setPhase(m_eventPower.getPhase() + 1);
                    playSeBasic(snd_se_stage_Orpheon_01, 0.0f);
                    m_eventPower.m_manualFramesLeft = 0.0f;
                }
                break;
            case 1:
                m_eventPower.m_manualFramesLeft += deltaFrame;
                if (m_eventPower.m_manualFramesLeft >= 180.0f) {
                    playSeBasic(snd_se_stage_Orpheon_02, 0.0f);
                    m_eventPower.m_manualFramesLeft = 0.0f;
                    m_eventPower.setPhase(m_eventPower.getPhase() + 1);
                }
                break;
            case 2:
                if (static_cast<grMadein*>(getGround(1))->isEndEntity() == true) {
                    g_gfSceneRoot->removeResAnmScn();
                    registScnAnim(static_cast<nw4r::g3d::ResFileData*>(m_fileData->getData(Data_Type_Scene, 2, 0xFFFE)), 0);
                    static_cast<grMadein*>(getGround(1))->setMotionDetails(0, 0, 0, 0, 2);
                    static_cast<grMadein*>(getGround(1))->startEntity();
                    m_eventPower.setPhase(m_eventPower.getPhase() + 1);
                }
                break;
            case 3:
                if (static_cast<grMadein*>(getGround(1))->isEndEntity() == true) {
                    g_gfSceneRoot->removeResAnmScn();
                    registScnAnim(static_cast<nw4r::g3d::ResFileData*>(m_fileData->getData(Data_Type_Scene, 3, 0xFFFE)), 0);
                    static_cast<grMadein*>(getGround(1))->setMotionDetails(0, 0, 0, 0, 3);
                    static_cast<grMadein*>(getGround(1))->startEntity();
                    m_eventPower.setPhase(m_eventPower.getPhase() + 1);
                    playSeBasic(snd_se_stage_Orpheon_03, 0.0f);
                }
                break;
            case 4:
                if (static_cast<grMadein*>(getGround(1))->isEndEntity() == true) {
                    g_gfSceneRoot->removeResAnmScn();
                    registScnAnim(static_cast<nw4r::g3d::ResFileData*>(m_fileData->getData(Data_Type_Scene, 0, 0xFFFE)), 0);
                    static_cast<grMadein*>(getGround(1))->setMotionDetails(0, 0, 0, 0, 0);
                    static_cast<grMadein*>(getGround(1))->startEntityAutoLoop();
                    m_eventPower.end();
                }
                break;
        }
        m_eventPower.update(deltaFrame);
    }
}

// The scheduler: once the call timer is up and neither big event runs, start the power failure (30%) or the spin
// (70%), but never the same event more than twice in a row.
void stOrpheon::eventCall(float deltaFrame) {
    switch (m_eventCall.getPhase()) {
    case 0:
        if (m_eventCall.isReadyEnd() && !m_eventPower.isEvent() && !m_eventSpin.isEvent()) {
            int choice = 0;
            if (__fabsf(1.0f - 2.0f * randf()) < 0.3f) {
                choice = 1;
            }
            if (choice == m_lastChoice) {
                m_repeatCount++;
                if ((s8)m_repeatCount >= 2) {
                    choice = !choice;
                    m_repeatCount = 0;
                }
            } else {
                m_repeatCount = 0;
            }
            m_lastChoice = choice;
            if (choice) {
                m_eventPower.start();
            } else {
                m_eventSpin.start();
            }
            m_eventCall.end();
            m_eventCall.start();
        }
        break;
    }
    m_eventCall.update(deltaFrame);
}

void stOrpheon::update(float deltaFrame) {
    g_aiMgr->clearDangerZone(0);
    eventCall(deltaFrame);
    eventPowerFailure(deltaFrame);
    eventSpinStage(deltaFrame);
    eventTrapFloor1(deltaFrame);
    eventTrapFloor2A(deltaFrame);
    eventTrapFloor2B(deltaFrame);
    m_seQueen.playFrame(m_queenMotion, static_cast<grMadein*>(getGround(2))->getEntityFrame());
    Vec3f rot;
    rot.m_z = m_spinAngle;
    rot.m_x = 0.0f;
    rot.m_y = 0.0f;
    static_cast<grMadein*>(getGround(0))->setRot(&rot);
    static_cast<grMadein*>(getGround(1))->setRot(&rot);
    static_cast<grMadein*>(getGround(3))->setRot(&rot);
    static_cast<grMadein*>(getGround(4))->setRot(&rot);
    static_cast<grMadein*>(getGround(5))->setRot(&rot);
    rot.m_x = 0.0f;
    rot.m_y = 0.0f;
    rot.m_z = m_spinAngle + m_queenRoll;
    static_cast<grMadein*>(getGround(2))->setRot(&rot);

    Vec3f spline[4];
    Vec3f pos;
    getGround(1)->getNodePosition(&spline[0], 0, "PQueenPositionA");
    getGround(1)->getNodePosition(&spline[2], 0, "PQueenPositionB");
    spline[1] = spline[0];
    spline[3] = spline[2];
    mtBezierCurve(m_queenPathT, spline, &pos);
    static_cast<grMadein*>(getGround(2))->setPos(&pos);

    getGround(0)->getNodePosition(&spline[0], 0, "C_TRAP_MOVE_01_PositionUp");
    getGround(0)->getNodePosition(&spline[3], 0, "C_TRAP_MOVE_01_PositionDown");
    spline[1] = spline[0];
    spline[2] = spline[3];
    mtBezierCurve(m_trap1PathT, spline, &pos);
    static_cast<grMadein*>(getGround(3))->setPos(&pos);

    getGround(0)->getNodePosition(&spline[0], 0, "C_TRAP_MOVE_02_01_Position");
    getGround(0)->getNodePosition(&spline[3], 0, "C_TRAP_MOVE_02_01_PositionRight");
    spline[1] = spline[0];
    spline[2] = spline[3];
    mtBezierCurve(m_trap2APathT, spline, &pos);
    static_cast<grMadein*>(getGround(4))->setPos(&pos);
    if (m_trap2APathT >= 0.3f) {
        Vec2f min;
        Vec2f max;
        Vec2f lo;
        Vec2f hi;
        lo.m_x = -50.0f;
        lo.m_y = -5.0f;
        min.m_x = pos.m_x + lo.m_x;
        min.m_y = pos.m_y + lo.m_y;
        hi.m_x = 50.0f;
        hi.m_y = 5.0f;
        max.m_x = pos.m_x + hi.m_x;
        max.m_y = pos.m_y + hi.m_y;
        g_aiMgr->setDangerZone(&min, &max, -1, false, false);
    }

    getGround(0)->getNodePosition(&spline[0], 0, "C_TRAP_MOVE_02_02_PositionLeft");
    getGround(0)->getNodePosition(&spline[3], 0, "C_TRAP_MOVE_02_02_Position");
    spline[1] = spline[0];
    spline[2] = spline[3];
    mtBezierCurve(m_trap2BPathT, spline, &pos);
    static_cast<grMadein*>(getGround(5))->setPos(&pos);
    if (m_trap2BPathT < 0.8f) {
        Vec2f min;
        Vec2f max;
        Vec2f lo;
        Vec2f hi;
        lo.m_x = -50.0f;
        lo.m_y = -5.0f;
        min.m_x = pos.m_x + lo.m_x;
        min.m_y = pos.m_y + lo.m_y;
        hi.m_x = 50.0f;
        hi.m_y = 5.0f;
        max.m_x = pos.m_x + hi.m_x;
        max.m_y = pos.m_y + hi.m_y;
        g_aiMgr->setDangerZone(&min, &max, -1, false, false);
    }
}

// The trainer's Pokemon (Squirtle, Ivysaur, Charizard) hide while the stage rolls over.
// MATCH-ONLY: the original keeps this out of line.
#pragma push
#pragma dont_inline on
void stOrpheon::setVisibilityTrainer(bool visible) {
    ftManager* manager = g_ftManager;
    for (int i = 0; i < 4; i++) {
        int entryId = manager->getEntryId(i);
        if (entryId != -1) {
            if (manager->isFighterActivate(entryId, -1)) {
                int kind = manager->getFighterGmKind(entryId);
                if (kind == 0x1D || kind == 0x1F || kind == 0x21) {
                    manager->setVisibilityTrainer(entryId, visible);
                }
            }
        }
    }
}
#pragma pop

// While the stage rolls the cliff-grab flags of every collision joint are cleared; they come back afterwards.
void stOrpheon::setJointCliff(bool enable) {
    u32 collisionNum = getCollisionNum();
    for (u32 i = 0; i != collisionNum; i++) {
        grCollision* collision = getCollision(i);
        if (collision != NULL) {
            u16 flags = 0;
            if (!enable) {
                flags = 0x6000;
            }
            u16 jointNum = collision->m_jointLen;
            for (u32 j = 0; j != jointNum; j++) {
                grCollisionJoint* joint = collision->getJoint(j);
                if (joint != NULL) {
                    joint->m_0x52 = flags;
                }
            }
        }
    }
}
