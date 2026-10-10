#include <cm/cm_camera_controller.h>
#include <gf/gf_3d_scene.h>
#include <gf/gf_archive.h>
#include <gm/gm_global.h>
#include <gr/collision/gr_collision.h>
#include <memory.h>
#include <mt/mt_matrix.h>
#include <mt/mt_prng.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <nw4r/math/math_triangular.h>
#include <snd/snd_system.h>
#include <st/st_class_info.h>
#include <st/st_melee.h>
#include <st/st_positions.h>
#include <st/st_utility.h>
#include <string.h>
#include <types.h>

#include <st_halberd/gr_halberd.h>
#include <st_halberd/st_halberd.h>

stClassInfoImpl<Stages::Halberd, stHalberd> stHalberd::bss_loc_14;

stHalberd* stHalberd::create() {
    return new (Heaps::StageInstance) stHalberd;
}

stHalberd::stHalberd() : stMelee("stHalberd", Stages::Halberd) {
    m_state = 0;
    m_frame = 0.0f;
    m_frameNext = 0.0f;
    m_motion = 0x15;
    for (u8 i = 0; i < 10; i++) {
        mtx(&m_mtx[i])->setIdentity();
    }
    m_posState = 0;
    m_shotTimer = 0.0f;
    m_shotKind = 0x15;
    m_shotQueue[0] = 0x15;
    m_shotQueue[1] = 0x15;
    m_shotQueue[2] = 0x15;
    m_shotSide = 0;
    m_shotPos.m_x = 0.0f;
    m_shotPos.m_y = 0.0f;
    m_shotPos.m_z = 0.0f;
    m_target = -1;
    m_targetPos.m_x = 0.0f;
    m_targetPos.m_y = 0.0f;
    m_targetPos.m_z = 0.0f;
    m_subject.clear();
    m_subject.m_state = 1;
    for (u8 i = 0; i < 13; i++) {
        mtx(&m_mtxBone[i])->setIdentity();
    }
    for (u8 i = 0; i < 12; i++) {
        mtx(&m_mtxJoint[i])->setIdentity();
    }
    mtx(&m_mtxHand[0])->setIdentity();
    m_armState = 0;
    m_armTimer = 0.0f;
    m_armTimerMax = 0.0f;
    m_armWait = 0.0f;
    m_armPos.m_x = 0.0f;
    m_armPos.m_y = 0.0f;
    m_armPos.m_z = 0.0f;
    m_armStart.m_x = 0.0f;
    m_armStart.m_y = 0.0f;
    m_armStart.m_z = 0.0f;
    m_armGoal.m_x = 0.0f;
    m_armGoal.m_y = 0.0f;
    m_armGoal.m_z = 0.0f;
    m_armUnk4AC = -1;
    m_armUnk4B0[0] = 0.0f;
    m_armUnk4B0[1] = 0.0f;
    m_armUnk4B0[2] = 0.0f;
    m_armVel.m_x = 0.0f;
    m_armVel.m_y = 0.0f;
    m_armVel.m_z = 0.0f;
    m_armSpeed = 0.0f;
    m_armOffset.m_x = 0.0f;
    m_armOffset.m_y = 0.0f;
    m_armOffset.m_z = 275.0f;
    m_armMotion = 0x15;
    m_armMiss = 0;
    m_armUnk9BC[0] = 0.0f;
    m_armUnk9BC[1] = 0.0f;
    m_armUnk9BC[2] = 0.0f;
    m_warning = 0x15;
}

stHalberd::~stHalberd() {
    releaseArchive();
    __dt__9cmSubjectFv(&m_subject, -1);
}

bool stHalberd::loading() {
    return true;
}

void stHalberd::createObj() {
    testStageParamInit(m_fileData, 10);
    testStageDataInit(m_fileData, 0x14, 0xB0);
    initPosPokeTrainer(4, 1);
    createObjEnkei(0);
    createObjBg(1);
    createObjDome(2);
    createObjStage(3);
    createCollision(m_fileData, 2, NULL);
    createObjCannon(4);
    createObjLaser(5);
    createObjTarget(6);
    createObjHero2Hou(7);
    createObjHero2Dan(8);
    createObjWarning(9);
    createObjArm();
    initCameraParam();
    loadStageAttrParam(m_fileData, 0x1E);
    void* posData = m_fileData->getData(Data_Type_Model, 100, 0xFFFE);
    if (posData == NULL) {
        createStagePositions();
    } else {
        nw4r::g3d::ResFile posFile(posData);
        createStagePositions(&posFile);
    }
    createWind2ndOnly();
    registScnAnim(static_cast<nw4r::g3d::ResFileData*>(m_fileData->getData(Data_Type_Scene, 0, 0xFFFE)), 0);
}

void stHalberd::createObjBg(int index) {
    grHalberdBg* ground;
    switch (index) {
    case 1:
        ground = grHalberdBg::create(0, "StgHalberdChikei", "grHalberdMainBg");
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setMtxGimmickWork(mtx(&m_mtx[0]));
        ground->setPosTrainerWork(m_pokeTrainerPos);
        ground->setFrameWork(&m_frame);
        ground->setStateWork(&m_motion);
    }
}

void stHalberd::createObjDome(int index) {
    grHalberdDome* ground;
    switch (index) {
    case 2:
        ground = grHalberdDome::create(1, "StgHalberdDome", "grHalberdDome");
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setMtxWork(mtx(&m_mtx[1]));
        ground->setMtxGimmickWork(mtx(&m_mtx[0]));
        ground->setPosTrainerWork(m_pokeTrainerPos);
        ground->setFrameWork(&m_frame);
        ground->setStateWork(&m_motion);
    }
}

void stHalberd::createObjEnkei(int index) {
    grHalberdEnkei* ground;
    switch (index) {
    case 0:
        ground = grHalberdEnkei::create(2, "StgHalberdEnkei", "grHalberdEnkei");
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setMtxWork(mtx(&m_mtx[0]));
        ground->setFrameWork(&m_frame);
        ground->setStateWork(&m_motion);
    }
}

void stHalberd::createObjStage(int index) {
    grHalberdStage* ground;
    switch (index) {
    case 3:
        ground = grHalberdStage::create(3, "StgHalberdStage", "grHalberdStage");
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setFrameWork(&m_frame);
        ground->setStateWork(&m_motion);
    }
}

void stHalberd::createObjCannon(int index) {
    grHalberdCannon* ground;
    switch (index) {
    case 4:
        ground = grHalberdCannon::create(9, "StgHalberd2ren", "grHalberdCannon");
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setMtxWork(mtx(&m_mtx[4]));
        ground->setMtxGimmickWork(mtx(&m_mtx[0]));
        ground->setPosTgtWork(&m_targetPos);
        ground->setTgtWork(&m_target);
        ground->setFrameWork(&m_frame);
        ground->setStateWork(&m_shotKind);
    }
}

void stHalberd::createObjLaser(int index) {
    grHalberdLaser* ground;
    switch (index) {
    case 5:
        ground = grHalberdLaser::create(6, "StgHalberdLaser", "grHalberdLaser");
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setMtxWork(mtx(&m_mtx[5]));
        ground->setPosTgtWork(&m_targetPos);
        ground->setFrameWork(&m_frame);
        ground->setStateWork(&m_shotKind);
    }
}

void stHalberd::createObjTarget(int index) {
    grHalberdTarget* ground;
    switch (index) {
    case 6:
        ground = grHalberdTarget::create(5, "StgHalBerdTarget", "grHalberdTarget");
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setPosWork(&m_targetPos);
        ground->setFrameWork(&m_frame);
        ground->setStateWork(&m_shotKind);
    }
}

void stHalberd::createObjHero2Hou(int index) {
    grHalberdHero2Hou* ground;
    switch (index) {
    case 7:
        ground = grHalberdHero2Hou::create(7, "StgHalberdHero2Hou", "grHalberdHero2Hou");
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setMtxWork(mtx(&m_mtx[8]));
        ground->setMtxGimmickWork(mtx(&m_mtx[0]));
        ground->setPosTgtWork(&m_targetPos);
        ground->setFrameWork(&m_frame);
        ground->setStateWork(&m_shotKind);
    }
}

void stHalberd::createObjHero2Dan(int index) {
    grHalberdHero2Dan* ground;
    switch (index) {
    case 8:
        ground = grHalberdHero2Dan::create(8, "Hero2Dan", "grHalberdHero2Dan");
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setMtxWork(mtx(&m_mtx[0]));
        ground->setPosTgtWork(&m_targetPos);
        ground->setFrameWork(&m_frame);
        ground->setStateWork(&m_shotKind);
    }
}

void stHalberd::createObjWarning(int index) {
    grHalberdWarning* ground;
    switch (index) {
    case 9:
        ground = grHalberdWarning::create(0x14, "gr2_StgHalberdWarning", "grHalberdWarning");
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setMtxWork(mtx(&m_mtx[0]));
        ground->setFrameWork(&m_frame);
        ground->setStateWork(&m_warning);
    }
}

// The arm is made of 13 bones, 12 joints and a hand; each is a ground of its own that follows its matrix.
void stHalberd::createObjArm() {
    for (u8 i = 0; i < 13; i++) {
        createObjArmBone(i);
    }
    for (u8 i = 0; i < 12; i++) {
        createObjArmJoint(i);
    }
    createObjArmHand(0);
}

void stHalberd::createObjArmBone(int index) {
    grHalberdArm* ground = grHalberdArm::create(0xB, "StgHalberdArmBone", "grHalberdArmBone");
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setMtxWork(mtx(&m_mtxBone[index]));
        ground->setType(0);
        ground->setFrameWork(&m_frame);
        ground->setStateWork(&m_armMotion);
        ground->setStateAttackWork(&m_shotKind);
    }
}

void stHalberd::createObjArmJoint(int index) {
    grHalberdArm* ground = grHalberdArm::create(10, "StgHalberdArmJoint", "grHalberdArmJoint");
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setMtxWork(mtx(&m_mtxJoint[index]));
        ground->setType(1);
        ground->setFrameWork(&m_frame);
        ground->setStateWork(&m_armMotion);
        ground->setStateAttackWork(&m_shotKind);
    }
}

void stHalberd::createObjArmHand(int index) {
    grHalberdArm* ground = grHalberdArm::create(0xC, "StgHalberdArmHand", "grHalberdArmHand");
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setMtxWork(mtx(&m_mtxHand[index]));
        ground->setType(2);
        ground->setFrameWork(&m_frame);
        ground->setStateWork(&m_armMotion);
        ground->setStateAttackWork(&m_shotKind);
    }
}

void stHalberd::update(float deltaFrame) {
    if (m_isDevil == true) {
        setCameraLimitRange(-250.0f, 250.0f, 200.0f, 45.0f);
    } else {
        resetCameraLimitRange();
    }
    updateActive(deltaFrame);
    updateArm(deltaFrame);
    updateArmPos();
    updateHeroHero();
}

// The story of the ship: state 0 starts it, 2 waits until the warning comes, 3 plays the fly-by and the first shots, 4
// fires the cannons until the animation ends, 5 and 6 let the ship leave and come back, and the story starts again.
void stHalberd::updateActive(float deltaFrame) {
    stHalberdData* data = static_cast<stHalberdData*>(m_stageData);
    if (data != NULL) {
        m_frame += deltaFrame;
        switch (m_state) {
        case 0:
            m_motion = 2;
            m_frame = 0.0f;
            m_state = 2;
            break;
        case 2:
            if (720.0f - data->unk1C <= m_frame) {
                m_warning = 0;
                m_state = 3;
            }
            break;
        case 3: {
            if (720.0f <= m_frame && m_frame <= 850.0f) {
                if (m_stageParam == NULL) {
                    return;
                }
                float rate = (m_frame - 720.0f) / 130.0f;
                if (rate < 0.0f) {
                    rate = 0.0f;
                }
                if (rate > 1.0f) {
                    rate = 1.0f;
                }
                float sine = nw4r::math::SinFIdx(static_cast<float>(static_cast<s16>(static_cast<int>(rate * 16384.0f))) * (1.0f / 256.0f));
                g_sndSystem->setEffectVol(0, 1, m_stageParam->m_effectVol * 0.01f * (1.0f - sine));
            }
            if (m_frame < 5100.0f) {
                if (5030.0f <= m_frame && m_posState == 0) {
                    void* posData = m_fileData->getData(Data_Type_Model, 0x65, 0xFFFE);
                    if (posData != NULL) {
                        nw4r::g3d::ResFile posFile(posData);
                        m_stagePositions->loadPositionData(&posFile);
                    }
                    updateStagePositions();
                    m_posState = 1;
                }
            } else {
                m_frame -= 5100.0f;
                m_motion = 3;
                m_frameNext = data->unk24 + (data->unk28 - data->unk24) * randf();
                if (m_shotSide == 1) {
                    m_shotTimer = data->unk00 + (data->unk04 - data->unk00) * randf();
                    m_shotSide = 0;
                } else {
                    m_shotTimer = data->unk08 + (data->unk0C - data->unk08) * randf();
                }
                m_state = 4;
            }
            break;
        }
        case 4:
            if (m_shotKind == 0x15) {
                if (m_frame < m_frameNext) {
                    float timer = m_shotTimer;
                    m_shotTimer -= deltaFrame;
                    if (m_shotTimer < 0.0f) {
                        m_shotTimer = 0.0f;
                    }
                    m_subject.m_state = 1;
                    if (m_shotTimer == 0.0f) {
                        if (m_shotQueue[1] == 0x15 && m_shotQueue[2] == 0x15) {
                            float roll = randf();
                            if (roll < 1.0f / 6.0f) {
                                m_shotQueue[0] = 6;
                                m_shotQueue[1] = 5;
                                m_shotQueue[2] = 7;
                            } else if (roll < 2.0f / 6.0f) {
                                m_shotQueue[0] = 5;
                                m_shotQueue[1] = 6;
                                m_shotQueue[2] = 7;
                            } else if (roll < 3.0f / 6.0f) {
                                m_shotQueue[0] = 5;
                                m_shotQueue[1] = 6;
                                m_shotQueue[2] = 7;
                            } else if (roll < 4.0f / 6.0f) {
                                m_shotQueue[0] = 5;
                                m_shotQueue[1] = 7;
                                m_shotQueue[2] = 6;
                            } else if (roll < 5.0f / 6.0f) {
                                m_shotQueue[0] = 7;
                                m_shotQueue[1] = 6;
                                m_shotQueue[2] = 5;
                            } else {
                                m_shotQueue[0] = 7;
                                m_shotQueue[1] = 5;
                                m_shotQueue[2] = 6;
                            }
                            if (m_shotQueue[0] == m_shotKind) {
                                m_shotQueue[0] = m_shotQueue[1];
                                m_shotQueue[1] = m_shotQueue[2];
                                m_shotQueue[2] = m_shotKind;
                            }
                        } else {
                            m_shotQueue[0] = m_shotQueue[1];
                            m_shotQueue[1] = m_shotQueue[2];
                            m_shotQueue[2] = 0x15;
                        }
                        m_shotKind = m_shotQueue[0];
                        m_shotTimer = data->unk08 + (data->unk0C - data->unk08) * randf();
                        halberdDisableSubject(&m_subject);
                        Rect2D range;
                        range.m_left = -25.0f;
                        range.m_right = 25.0f;
                        range.m_up = 50.0f;
                        range.m_down = 0.0f;
                        Vec3f pos(0.0f, 50.0f, 0.0f);
                        m_subject.setPos(&pos);
                        m_subject.m_range = range;
                    }
                    (void)timer;
                } else {
                    m_motion = 4;
                    m_frame = 0.0f;
                    m_warning = 1;
                    registScnAnim(static_cast<nw4r::g3d::ResFileData*>(m_fileData->getData(Data_Type_Scene, 0, 0xFFFE)), 1);
                    g_gfSceneRoot->setCurrentFrame(m_frame);
                    if (m_posState == 1) {
                        void* posData = m_fileData->getData(Data_Type_Model, 0x66, 0xFFFE);
                        if (posData != NULL) {
                            nw4r::g3d::ResFile posFile(posData);
                            m_stagePositions->loadPositionData(&posFile);
                        }
                        updateStagePositions();
                        m_posState = 2;
                    }
                    m_state = 5;
                }
            }
            break;
        case 5:
            if (1200.0f - data->unk1C <= m_frame) {
                m_state = 6;
            }
            break;
        case 6:
            if (1200.0f <= m_frame) {
                m_warning = 0x15;
                m_motion = 2;
                m_frame = (m_frame - 1200.0f) + 1121.0f;
                registScnAnim(static_cast<nw4r::g3d::ResFileData*>(m_fileData->getData(Data_Type_Scene, 0, 0xFFFE)), 0);
                g_gfSceneRoot->setCurrentFrame(m_frame);
                if (m_posState == 2) {
                    void* posData = m_fileData->getData(Data_Type_Model, 100, 0xFFFE);
                    if (posData != NULL) {
                        nw4r::g3d::ResFile posFile(posData);
                        m_stagePositions->loadPositionData(&posFile);
                    }
                    updateStagePositions();
                    m_posState = 0;
                }
                m_state = 3;
            }
            break;
        }
    }
}

// The hand of the arm: it flies in (state 1, 7, 8), follows a player or the middle of the stage (9), grabs (10, 11) and
// leaves again (12). m_armOffset is where the hand is relative to the shoulder.
void stHalberd::updateArm(float deltaFrame) {
    stHalberdData* data = static_cast<stHalberdData*>(m_stageData);
    if (data == NULL) {
        return;
    }
    m_armTimer -= deltaFrame;
    if (m_armTimer < 0.0f) {
        m_armTimer = 0.0f;
    }
    m_armWait -= deltaFrame;
    if (m_armWait < 0.0f) {
        m_armWait = 0.0f;
    }
    Vec3f dir;
    Vec3f playerPos;
    switch (m_armState) {
    case 0:
        m_armState = 1;
        m_armTimer = 210.0f;
        m_armTimerMax = 210.0f;
        break;
    case 1:
        if (m_shotKind == 6) {
            m_armTimer = 300.0f;
            m_armTimerMax = 240.0f;
            m_armStart.m_x = mtx(&m_mtxHand[0])->m[0][3];
            m_armStart.m_y = mtx(&m_mtxHand[0])->m[1][3];
            m_armStart.m_z = mtx(&m_mtxHand[0])->m[2][3];
            m_armGoal.m_z = mtx(&m_mtx[6])->m[2][3] + 41.25f;
            m_armGoal.m_x = mtx(&m_mtx[6])->m[0][3] + 0.0f;
            m_armGoal.m_y = mtx(&m_mtx[6])->m[1][3] + 0.0f;
            m_armGoal.m_x = m_armGoal.m_x - (randf() * 5.0f + 5.0f);
            m_armPos.m_x = m_armStart.m_x;
            m_armPos.m_y = m_armStart.m_y;
            m_armPos.m_z = m_armStart.m_z;
            m_armGoal.m_y = m_armGoal.m_y + randf() * 5.0f + 25.0f;
            m_armMotion = 0x12;
            m_armState = 7;
        } else {
            float rate = 1.0f - m_armTimer / m_armTimerMax;
            if (rate < 0.0f) {
                rate = 0.0f;
            }
            if (rate > 1.0f) {
                rate = 1.0f;
            }
            float cosine = nw4r::math::CosFIdx(static_cast<float>(static_cast<s16>(static_cast<int>(rate * 65535.0f))) * (1.0f / 256.0f));
            float sine = nw4r::math::SinFIdx(static_cast<float>(static_cast<s16>(static_cast<int>(rate * 65535.0f))) * (1.0f / 256.0f));
            if (m_armTimer <= 0.0f) {
                m_armTimer = m_armTimerMax;
            }
            float len = sine * 2.5f + 137.5f;
            m_armOffset.m_x = len * 0.0f;
            m_armOffset.m_y = cosine * 10.0f + 15.0f;
            m_armOffset.m_z = len * 1.0f;
            m_armMotion = 0x15;
        }
        break;
    case 7: {
        float rate = 1.0f - m_armTimer / m_armTimerMax;
        if (rate < 0.0f) {
            rate = 0.0f;
        }
        if (rate > 1.0f) {
            rate = 1.0f;
        }
        float sine = nw4r::math::SinFIdx(static_cast<float>(static_cast<s16>(static_cast<int>(rate * 16384.0f))) * (1.0f / 256.0f));
        dir.m_x = m_armGoal.m_x - m_armStart.m_x;
        dir.m_y = m_armGoal.m_y - m_armStart.m_y;
        dir.m_z = m_armGoal.m_z - m_armStart.m_z;
        float dist = dir.length();
        dir.normalize();
        float step = dist * sine;
        m_armPos.m_z = m_armStart.m_z + dir.m_z * step;
        m_armPos.m_x = m_armStart.m_x + dir.m_x * step;
        m_armPos.m_y = m_armStart.m_y + dir.m_y * step;
        m_armOffset.m_z = m_armPos.m_z - mtx(&m_mtx[6])->m[2][3];
        m_armOffset.m_x = m_armPos.m_x - mtx(&m_mtx[6])->m[0][3];
        m_armOffset.m_y = m_armPos.m_y - mtx(&m_mtx[6])->m[1][3];
        if (sine == 1.0f) {
            m_armTimer = 60.0f;
            m_armTimerMax = 60.0f;
            m_armStart.m_x = mtx(&m_mtxHand[0])->m[0][3];
            m_armStart.m_y = mtx(&m_mtxHand[0])->m[1][3];
            m_armStart.m_z = mtx(&m_mtxHand[0])->m[2][3];
            m_armGoal.m_z = mtx(&m_mtx[6])->m[2][3] + 192.5f;
            m_armGoal.m_x = mtx(&m_mtx[6])->m[0][3] + 0.0f;
            m_armGoal.m_y = mtx(&m_mtx[6])->m[1][3] + 0.0f;
            m_armGoal.m_x = m_armGoal.m_x + randf() * 10.0f + 20.0f;
            m_armPos.m_x = m_armStart.m_x;
            m_armPos.m_y = m_armStart.m_y;
            m_armPos.m_z = m_armStart.m_z;
            m_armGoal.m_y = m_armGoal.m_y + randf() * 5.0f + 45.0f;
            m_armSpeed = data->unk40;
            m_armState = 8;
        }
        break;
    }
    case 8: {
        dir.m_x = m_armGoal.m_x - m_armStart.m_x;
        dir.m_y = m_armGoal.m_y - m_armStart.m_y;
        dir.m_z = m_armGoal.m_z - m_armStart.m_z;
        dir.normalize();
        dir.m_x *= m_armSpeed;
        dir.m_z *= m_armSpeed;
        dir.m_y *= m_armSpeed;
        m_armPos.m_z += dir.m_z;
        m_armPos.m_x += dir.m_x;
        m_armPos.m_y += dir.m_y;
        m_armOffset.m_z = m_armPos.m_z - mtx(&m_mtx[6])->m[2][3];
        m_armOffset.m_x = m_armPos.m_x - mtx(&m_mtx[6])->m[0][3];
        m_armOffset.m_y = m_armPos.m_y - mtx(&m_mtx[6])->m[1][3];
        if (m_armOffset.length() > 137.5f) {
            int player = static_cast<int>(randf() * 4.0f);
            if (stMelee::getPlayerPosition(player, &playerPos) == 1) {
                m_target = player;
                m_targetPos.m_x = playerPos.m_x;
                m_targetPos.m_y = playerPos.m_y;
                m_targetPos.m_z = playerPos.m_z;
                m_targetPos.m_y = playerPos.m_y + 50.0f;
                m_armWait = data->unk4C;
            } else {
                m_target = -1;
                m_targetPos.m_x = 0.0f;
                m_targetPos.m_y = 100.0f;
                m_targetPos.m_z = 0.0f;
                m_armWait = data->unk3C;
            }
            m_armGoal.m_x = m_targetPos.m_x;
            m_armGoal.m_y = m_targetPos.m_y;
            m_armTimer = data->unk44 + (data->unk48 - data->unk44);
            m_armGoal.m_z = m_targetPos.m_z;
            m_armVel.m_x = 0.0f;
            m_armVel.m_y = 0.0f;
            m_armVel.m_z = 0.0f;
            m_armState = 9;
        }
        break;
    }
    case 9:
        if (m_target >= 0) {
            if (stMelee::getPlayerPosition(m_target, &playerPos) == 1) {
                m_targetPos.m_x = playerPos.m_x;
                m_targetPos.m_y = playerPos.m_y;
                m_targetPos.m_z = playerPos.m_z;
                m_targetPos.m_y = playerPos.m_y + 50.0f;
            }
            float x = m_targetPos.m_x;
            if (x < -140.0f) {
                x = -140.0f;
            }
            float y = m_targetPos.m_y;
            if (y < 47.5f) {
                y = 47.5f;
            }
            if (x > 140.0f) {
                x = 140.0f;
            }
            if (y > 120.0f) {
                y = 120.0f;
            }
            float z = m_targetPos.m_z - mtx(&m_mtx[6])->m[2][3];
            m_targetPos.m_x = x;
            m_targetPos.m_y = y;
            dir.m_x = m_targetPos.m_x - mtx(&m_mtx[6])->m[0][3];
            dir.m_y = m_targetPos.m_y - mtx(&m_mtx[6])->m[1][3];
            dir.m_z = z;
            dir.normalize();
            m_armGoal.m_x = mtx(&m_mtx[6])->m[0][3] + dir.m_x * 192.5f;
            m_armGoal.m_y = mtx(&m_mtx[6])->m[1][3] + dir.m_y * 192.5f;
            m_armGoal.m_z = mtx(&m_mtx[6])->m[2][3] + z * 192.5f;
        }
        m_armPos.m_x += m_armVel.m_x;
        m_armPos.m_y += m_armVel.m_y;
        m_armPos.m_z += m_armVel.m_z;
        if (m_armPos.m_x > m_armGoal.m_x) {
            m_armVel.m_x -= deltaFrame * 0.125f;
        }
        if (m_armPos.m_x < m_armGoal.m_x) {
            m_armVel.m_x += deltaFrame * 0.125f;
        }
        if (m_armPos.m_y > m_armGoal.m_y) {
            m_armVel.m_y -= deltaFrame * 0.125f;
        }
        if (m_armPos.m_y < m_armGoal.m_y) {
            m_armVel.m_y += deltaFrame * 0.125f;
        }
        if (m_armVel.m_x < -3.0f) {
            m_armVel.m_x = deltaFrame * -3.0f;
        }
        if (m_armVel.m_x > 3.0f) {
            m_armVel.m_x = deltaFrame * 3.0f;
        }
        if (m_armVel.m_y < -3.0f) {
            m_armVel.m_y = deltaFrame * -3.0f;
        }
        if (m_armVel.m_y > 3.0f) {
            m_armVel.m_y = deltaFrame * 3.0f;
        }
        dir.m_x = m_armPos.m_x - mtx(&m_mtx[6])->m[0][3];
        dir.m_y = m_armPos.m_y - mtx(&m_mtx[6])->m[1][3];
        dir.m_z = m_armPos.m_z - mtx(&m_mtx[6])->m[2][3];
        if (dir.length() >= 192.5f) {
            m_armPos.m_z -= deltaFrame * 0.125f;
        } else {
            m_armPos.m_z += deltaFrame * 0.125f;
        }
        m_armOffset.m_x = m_armPos.m_x - mtx(&m_mtx[6])->m[0][3];
        m_armOffset.m_y = m_armPos.m_y - mtx(&m_mtx[6])->m[1][3];
        m_armOffset.m_z = m_armPos.m_z - mtx(&m_mtx[6])->m[2][3];
        if (m_armTimer == 0.0f) {
            if (m_armMiss > 9 || m_target != -1) {
                m_armTimer = data->unk54;
                m_armMotion = 0x13;
                m_armState = 10;
                return;
            }
            m_armTimer = data->unk44 + (data->unk48 - data->unk44);
        }
        if (m_armWait == 0.0f) {
            m_armWait = data->unk4C;
            if (randf() <= data->unk50) {
                int player = static_cast<int>(randf() * 4.0f);
                if (stMelee::getPlayerPosition(player, &playerPos) == 0) {
                    m_armMiss++;
                    m_armWait = data->unk3C;
                    if (m_armMiss < 10) {
                        return;
                    }
                    player = -1;
                    playerPos.m_x = 0.0f;
                    playerPos.m_y = 0.0f;
                    playerPos.m_z = 0.0f;
                }
                m_target = player;
                m_targetPos.m_x = playerPos.m_x;
                m_targetPos.m_y = playerPos.m_y;
                m_targetPos.m_z = playerPos.m_z;
                m_targetPos.m_y = playerPos.m_y + 50.0f;
            }
        }
        break;
    case 10:
        if (m_armTimer == 0.0f) {
            if (stMelee::getPlayerPosition(m_target, &playerPos) == 1) {
                m_targetPos.m_x = playerPos.m_x;
                m_targetPos.m_y = playerPos.m_y;
                m_targetPos.m_z = playerPos.m_z;
            } else {
                m_targetPos.m_x = 0.0f;
                m_targetPos.m_y = 0.0f;
                m_targetPos.m_z = 0.0f;
            }
            float x = m_targetPos.m_x;
            if (x < -140.0f) {
                x = -140.0f;
            }
            float y = m_targetPos.m_y;
            if (y < 47.5f) {
                y = 47.5f;
            }
            m_armStart.m_x = m_armPos.m_x;
            if (x > 140.0f) {
                x = 140.0f;
            }
            m_armStart.m_y = m_armPos.m_y;
            if (y > 120.0f) {
                y = 120.0f;
            }
            m_targetPos.m_x = x;
            m_targetPos.m_y = y;
            m_armStart.m_z = m_armPos.m_z;
            m_armGoal.m_x = x;
            m_armGoal.m_y = y;
            m_armGoal.m_z = m_targetPos.m_z;
            m_armPos.m_x += randf() * 5.0f + -2.5f;
            m_armPos.m_y += randf() * 5.0f + -2.5f;
            m_armSpeed = 0.0f;
            m_armMotion = 0x14;
            m_armState = 11;
            m_armPos.m_z += randf() * 5.0f + -2.5f;
        }
        break;
    case 0xB:
        m_armSpeed += data->unk58 * deltaFrame;
        if (m_armSpeed > data->unk5C) {
            m_armSpeed = data->unk5C;
        }
        if (fabsf(m_armGoal.m_z - m_armStart.m_z) > 50.0f) {
            if (stMelee::getPlayerPosition(m_target, &playerPos) == 1) {
                m_targetPos.m_x = playerPos.m_x;
                m_targetPos.m_y = playerPos.m_y;
                m_targetPos.m_z = playerPos.m_z;
            }
            float x = m_targetPos.m_x;
            if (x < -140.0f) {
                x = -140.0f;
            }
            float y = m_targetPos.m_y;
            if (y < 47.5f) {
                y = 47.5f;
            }
            if (x > 140.0f) {
                x = 140.0f;
            }
            if (y > 120.0f) {
                y = 120.0f;
            }
            m_targetPos.m_x = x;
            m_targetPos.m_y = y;
        }
        m_armStart.m_z = m_armPos.m_z;
        m_armGoal.m_z = m_targetPos.m_z;
        m_armStart.m_x = m_armPos.m_x;
        float dz = m_armGoal.m_z - m_armStart.m_z;
        m_armStart.m_y = m_armPos.m_y;
        m_armGoal.m_x = m_targetPos.m_x;
        m_armGoal.m_y = m_targetPos.m_y;
        dir.m_x = m_armGoal.m_x - m_armStart.m_x;
        dir.m_y = m_armGoal.m_y - m_armStart.m_y;
        dir.normalize();
        m_armPos.m_z += dz * m_armSpeed;
        m_armPos.m_y += dir.m_y * m_armSpeed;
        m_armPos.m_x += dir.m_x * m_armSpeed;
        m_armOffset.m_x = m_armPos.m_x - mtx(&m_mtx[6])->m[0][3];
        m_armOffset.m_y = m_armPos.m_y - mtx(&m_mtx[6])->m[1][3];
        m_armOffset.m_z = m_armPos.m_z - mtx(&m_mtx[6])->m[2][3];
        if (fabsf(m_armGoal.m_z - m_armStart.m_z) < 5.0f) {
            m_armTimer = 180.0f;
            m_armTimerMax = 180.0f;
            m_armStart.m_x = mtx(&m_mtxHand[0])->m[0][3];
            m_armStart.m_y = mtx(&m_mtxHand[0])->m[1][3];
            m_armStart.m_z = mtx(&m_mtxHand[0])->m[2][3];
            m_armGoal.m_z = mtx(&m_mtx[6])->m[2][3] + 137.5f;
            m_armGoal.m_x = mtx(&m_mtx[6])->m[0][3] + 0.0f;
            m_armGoal.m_y = mtx(&m_mtx[6])->m[1][3] + 0.0f + 25.0f;
            m_armPos.m_x = m_armStart.m_x;
            m_armPos.m_y = m_armStart.m_y;
            m_armPos.m_z = m_armStart.m_z;
            m_shotKind = 0x15;
            m_armMotion = 0x15;
            m_armState = 12;
        }
        break;
    case 0xC: {
        float rate = 1.0f - m_armTimer / m_armTimerMax;
        if (rate < 0.0f) {
            rate = 0.0f;
        }
        if (rate > 1.0f) {
            rate = 1.0f;
        }
        float sine = nw4r::math::SinFIdx(static_cast<float>(static_cast<s16>(static_cast<int>(rate * 16384.0f))) * (1.0f / 256.0f));
        dir.m_x = m_armGoal.m_x - m_armStart.m_x;
        dir.m_y = m_armGoal.m_y - m_armStart.m_y;
        dir.m_z = m_armGoal.m_z - m_armStart.m_z;
        float dist = dir.length();
        dir.normalize();
        float step = dist * sine;
        m_armPos.m_z = m_armStart.m_z + dir.m_z * step;
        m_armPos.m_x = m_armStart.m_x + dir.m_x * step;
        m_armPos.m_y = m_armStart.m_y + dir.m_y * step;
        m_armOffset.m_z = m_armPos.m_z - mtx(&m_mtx[6])->m[2][3];
        m_armOffset.m_x = m_armPos.m_x - mtx(&m_mtx[6])->m[0][3];
        m_armOffset.m_y = m_armPos.m_y - mtx(&m_mtx[6])->m[1][3];
        if (sine == 1.0f) {
            m_armState = 0;
        }
        break;
    }
    }
}

// Lays the bones, the joints and the hand of the arm along a curve from the shoulder (the matrix of the arm) to the hand.
void stHalberd::updateArmPos() {
    if (m_armOffset.m_x != 0.0f || m_armOffset.m_y != 0.0f || m_armOffset.m_z != 0.0f) {
        float len = m_armOffset.length();
        Vec3f dir(m_armOffset.m_x, m_armOffset.m_y, m_armOffset.m_z);
        Vec3f shoulder(0.0f, 0.0f, 0.0f);
        dir.normalize();
        dir.m_x *= len;
        dir.m_y *= len;
        dir.m_z *= len;
        Vec3f hand(shoulder.m_x + dir.m_x, shoulder.m_y + dir.m_y, shoulder.m_z + dir.m_z);
        mtx(&m_mtxHand[0])->setIdentity();
        fn_8003F03C(mtx(&m_mtxHand[0]), hand.m_x, hand.m_y, hand.m_z);
        Vec3f whole(hand.m_x - shoulder.m_x, hand.m_y - shoulder.m_y, hand.m_z - shoulder.m_z);
        float dist = whole.length();
        float step = (dist - 10.0f) / 13.0f;
        float sag = 275.0f - (dist - 10.0f);
        if (sag < 0.0f) {
            sag = 0.0f;
        }
        if (sag > 0.0f) {
            sag = sag * 0.25f;
        }
        Vec3f prev(shoulder.m_x, shoulder.m_y, shoulder.m_z);
        for (u8 i = 0; i < 12; i++) {
            int n = i + 1;
            Vec3f along(hand.m_x - shoulder.m_x, hand.m_y - shoulder.m_y, hand.m_z - shoulder.m_z);
            float reach = step * n;
            along.normalize();
            along.m_x *= reach;
            along.m_y *= reach;
            along.m_z *= reach;
            float frac = static_cast<float>(n) / 13.0f;
            int idx = static_cast<int>(32768.0f * frac);
            Vec3f point(shoulder.m_x + along.m_x, shoulder.m_y + along.m_y, shoulder.m_z + along.m_z);
            float sine = nw4r::math::SinFIdx(static_cast<float>(static_cast<s16>(idx)) * (1.0f / 256.0f));
            point.m_y = (shoulder.m_y + sag * sine) + (hand.m_y - shoulder.m_y) * frac;
            Matrix* joint = mtx(&m_mtxJoint[i]);
            joint->setIdentity();
            fn_8003F03C(joint, point.m_x, point.m_y, point.m_z);
            mtx(&m_mtx[6])->mul(joint, joint);
            Vec3f seg(point.m_x - prev.m_x, point.m_y - prev.m_y, point.m_z - prev.m_z);
            float segLen = seg.length();
            Vec3f scale(1.0f, 1.0f, segLen / 22.0f);
            seg.normalize();
            float angleX = nw4r::math::Atan2FIdx(seg.m_z, seg.m_y) * 1.40625f - 90.0f;
            float angleY = nw4r::math::Atan2FIdx(seg.m_x, seg.m_z) * 1.40625f;
            float angleZ = 0.0f;
            float half = 0.5f * segLen;
            seg.m_y *= half;
            seg.m_x *= half;
            seg.m_z *= half;
            Matrix* bone = mtx(&m_mtxBone[i]);
            bone->setIdentity();
            fn_8003F074(bone, prev.m_x + seg.m_x, prev.m_y + seg.m_y, prev.m_z + seg.m_z);
            bone->rotY(angleY * 0.017453292f);
            fn_8003EA9C(bone, angleX * 0.017453292f);
            fn_8003EBF4(bone, angleZ * 0.017453292f);
            fn_8003E828(bone, &scale, bone);
            mtx(&m_mtx[6])->mul(bone, bone);
            prev = point;
        }
        Vec3f seg(hand.m_x - prev.m_x, hand.m_y - prev.m_y, hand.m_z - prev.m_z);
        float segLen = seg.length();
        Vec3f scale(1.0f, 1.0f, segLen / 22.0f);
        seg.normalize();
        float angleX = nw4r::math::Atan2FIdx(seg.m_z, seg.m_y) * 1.40625f - 90.0f;
        float angleY = nw4r::math::Atan2FIdx(seg.m_x, seg.m_z) * 1.40625f;
        float angleZ = 0.0f;
        float part = segLen * 0.35f;
        seg.m_x *= part;
        seg.m_y *= part;
        seg.m_z *= part;
        Matrix* bone = mtx(&m_mtxBone[12]);
        bone->setIdentity();
        fn_8003F074(bone, prev.m_x + seg.m_x, prev.m_y + seg.m_y, prev.m_z + seg.m_z);
        bone->rotY(angleY * 0.017453292f);
        fn_8003EA9C(bone, angleX * 0.017453292f);
        fn_8003EBF4(bone, angleZ * 0.017453292f);
        fn_8003E828(bone, &scale, bone);
        mtx(&m_mtx[6])->mul(bone, bone);
        fn_8003EA9C(mtx(&m_mtxHand[0]), angleX * 0.017453292f);
        mtx(&m_mtxHand[0])->rotY(angleY * 0.017453292f);
        fn_8003EBF4(mtx(&m_mtxHand[0]), angleZ * 0.017453292f);
        mtx(&m_mtx[6])->mul(mtx(&m_mtxHand[0]), mtx(&m_mtxHand[0]));
    }
}

// While the cannon shoots the second kind of shot (12), the stage looks for the place on the floor where the shell lands.
void stHalberd::updateHeroHero() {
    stHalberdData* data = static_cast<stHalberdData*>(m_stageData);
    if (data != NULL && m_shotKind == 0xC) {
        Vec3f from(0.0f, 500.0f, 0.0f);
        Vec3f dir(0.0f, -1000.0f, 0.0f);
        float spread = -data->unk9C + data->unk9C * 2.0f * randf();
        Vec3f hit;
        Vec3f normal;
        if (stRayCheck(&from, &dir, &hit, &normal, true, NULL, false, 1)) {
            m_targetPos.m_x = spread;
            m_targetPos.m_y = hit.m_y;
            m_targetPos.m_z = 0.0f;
            m_shotKind = 0xD;
        }
    }
}

bool stHalberd::isEventEnd(int param1, int* eventState, int* eventDecision) {
    if (m_state > 3) {
        *eventState = 6;
        *eventDecision = 3;
        return true;
    }
    return false;
}

GXColor stHalberd::getFinalTechniqColor() {
    u32 packed = 0x14000496;
    return *reinterpret_cast<GXColor*>(&packed);
}
