#include <ec/ec_mgr.h>
#include <ft/fighter.h>
#include <ft/ft_manager.h>
#include <gr/gr_calc_world_callback.h>
#include <memory.h>
#include <mt/mt_prng.h>
#include <nw4r/math/math_triangular.h>

#include <st_halberd/gr_halberd.h>
#include <st_halberd/gr_halberd_anim.h>

grHalberdCannon* grHalberdCannon::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grHalberdCannon* ground = new (Heaps::StageInstance) grHalberdCannon(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grHalberdCannon::grHalberdCannon(const char* taskName) : grHalberd(taskName) {
    m_waitTimer = 0.0f;
    m_mtxWork = NULL;
    m_mtxGimmickWork = NULL;
    m_posTgtWork = NULL;
    m_tgtWork = NULL;
    m_speed = 0.0f;
    m_rotStart.m_x = 0.0f;
    m_rotStart.m_y = 0.0f;
    m_rotStart.m_z = 0.0f;
    m_rotGoal.m_x = 0.0f;
    m_rotGoal.m_y = 0.0f;
    m_rotGoal.m_z = 0.0f;
    m_rot.m_x = 0.0f;
    m_rot.m_y = 0.0f;
    m_rot.m_z = 0.0f;
    m_motion = 5;
    m_motionTimer = 0.0f;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_numNodeCallbackData = 1;
    callback->initialize(false, Heaps::StageInstance);
    callback->m_nodeCallbackDatas[0].m_flags |= 8;
    m_seId[0] = snd_se_stage_Halberd_Laser_cover_open;
    m_seId[1] = snd_se_stage_Halberd_Laser_cover_stop;
    m_seData0[0].id = snd_se_stage_Halberd_Laser_cover_open;
    m_seData0[0].unk4 = 0.0f;
    m_seData0[0].unk8 = 100.0f;
    m_seData0[0].unkC = 157.0f;
    m_seData0[1].id = snd_se_stage_Halberd_Laser_cover_stop;
    m_seData0[1].unk4 = 0.0f;
    m_seData0[1].unk8 = 160.0f;
    m_seData0[1].unkC = 0.0f;
    m_seData0[2].id = snd_se_stage_Halberd_Laser_cover_open;
    m_seData0[2].unk4 = 0.0f;
    m_seData0[2].unk8 = 0.0f;
    m_seData0[2].unkC = 60.0f;
    m_seData0[3].id = snd_se_stage_Halberd_Laser_cover_stop;
    m_seData0[3].unk4 = 0.0f;
    m_seData0[3].unk8 = 60.0f;
    m_seData0[3].unkC = 0.0f;
    m_seSeq.registId(m_seId, 2);
    m_seSeq.registSeq(0, m_seData0, 2, Heaps::StageInstance);
    m_seSeq.registSeq(1, m_seData1, 2, Heaps::StageInstance);
    m_seSeq.m_sndGenerator = &m_snd;
    m_seIdEngine = -1;
    m_seIdCharge = -1;
}

grHalberdCannon::~grHalberdCannon() {
}

void grHalberdCannon::processAnim() {
}

void grHalberdCannon::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateMotion(deltaFrame);
        updateRot();
        updateCallBack(deltaFrame);
        updateG3dProcCalcWorld();
        m_hasUpdatedG3dCalcWorld = false;
        if (m_mtxGimmickWork != NULL) {
            getNodeMatrix(&m_mtxGimmickWork[5], 0, "laserPosition");
            getNodeMatrix(&m_mtxGimmickWork[6], 0, "armPosition");
            getNodeMatrix(&m_mtxGimmickWork[8], 0, "hero2Position");
        }
    }
}

// The cannon: it opens its cover (2), turns towards a fighter and follows it (5), charges (6), fires (8) and closes again.
void grHalberdCannon::updateMotion(float deltaFrame) {
    if (m_stateWork == NULL) {
        return;
    }
    stHalberdData* data = static_cast<stHalberdData*>(getStageData());
    if (data == NULL) {
        return;
    }
    m_timer -= deltaFrame;
    if (m_timer < 0.0f) {
        m_timer = 0.0f;
    }
    m_motionTimer -= deltaFrame;
    if (m_motionTimer < 0.0f) {
        m_motionTimer = 0.0f;
    }
    m_waitTimer -= deltaFrame;
    if (m_waitTimer < 0.0f) {
        m_waitTimer = 0.0f;
    }
    switch (m_state) {
    case 0:
        setMotion(4, 0, 1, NULL);
        m_state = 2;
        m_timer = 90.0f;
        // fall through
    case 2:
        if (*m_stateWork == 5) {
            setMotion(2, 0, 1, &m_motionTimer);
            m_seIdEngine = m_snd.playSE(snd_se_stage_Halberd_Laser_eng, 0, 0, -1);
            m_posTgtWork->m_x = 0.0f;
            m_posTgtWork->m_y = 100.0f;
            m_posTgtWork->m_z = 0.0f;
            *m_tgtWork = -1;
            m_speed = 0.0f;
            m_state = 5;
            m_timer = data->unk78 + (data->unk7C - data->unk78) * randf();
        } else {
            float rate = 1.0f - m_timer / 90.0f;
            if (rate < 0.0f) {
                rate = 0.0f;
            }
            if (rate > 1.0f) {
                rate = 1.0f;
            }
            float sine = nw4r::math::SinFIdx(static_cast<float>(static_cast<s16>(static_cast<int>(rate * 16384.0f))) * (1.0f / 256.0f));
            Vec3f diff(0.0f - m_rotStart.m_x, 0.0f - m_rotStart.m_y, 0.0f - m_rotStart.m_z);
            m_rotGoal.m_z = 0.0f;
            m_rotGoal.m_x = m_rotStart.m_x + diff.m_x * sine;
            m_rotGoal.m_y = m_rotStart.m_y + diff.m_y * sine;
        }
        break;
    case 5:
        if (m_timer == 0.0f) {
            m_timer = data->unk94 + (data->unk98 - data->unk94) * randf();
            m_seIdCharge = m_snd.playSE(snd_se_stage_Halberd_Laser_charge, 0, 0, -1);
            *m_stateWork = 9;
            m_state = 6;
        } else if (*m_tgtWork == -1) {
            int player = static_cast<int>(randf() * 4.0f);
            Vec3f pos;
            if (getPlayerPosition(player, &pos)) {
                *m_tgtWork = player;
                *m_stateWork = 8;
                m_waitTimer = data->unk84;
            }
        } else {
            Vec3f pos;
            if (getPlayerPosition(*m_tgtWork, &pos)) {
                m_speed += data->unk88 * deltaFrame;
                if (m_speed > data->unk8C) {
                    m_speed = data->unk8C;
                }
                Vec3f* target = m_posTgtWork;
                Vec3f diff(pos.m_x - target->m_x, pos.m_y - target->m_y, pos.m_z - target->m_z);
                bool zero = false;
                if (fabsf(diff.m_x) < 1e-5f && fabsf(diff.m_y) < 1e-5f && fabsf(diff.m_z) < 1e-5f) {
                    zero = true;
                }
                if (!zero) {
                    float step = diff.length();
                    if (m_speed < step) {
                        step = m_speed;
                    }
                    diff.normalize();
                    diff.m_x *= step;
                    diff.m_y *= step;
                    diff.m_z *= step;
                    target = m_posTgtWork;
                    target->m_x += diff.m_x;
                    target->m_y += diff.m_y;
                    target->m_z += diff.m_z;
                }
                if (m_waitTimer == 0.0f) {
                    m_waitTimer = data->unk84;
                    if (data->unk80 < randf()) {
                        break;
                    }
                    Fighter* fighter = g_ftManager->searchNearFighter(0.0f, 20.0f, m_posTgtWork, -1, 0);
                    if (fighter == NULL) {
                        break;
                    }
                    *m_tgtWork = g_ftManager->getPlayerNo(fighter->m_entryId);
                }
                Matrix* aim = &m_mtxGimmickWork[5];
                target = m_posTgtWork;
                Vec3f dir(target->m_x - aim->m[0][3], target->m_y - aim->m[1][3], target->m_z - aim->m[2][3]);
                dir.normalize();
                m_rotGoal.m_x = nw4r::math::Atan2FIdx(dir.m_z, dir.m_y) * 1.40625f - 90.0f;
                m_rotGoal.m_z = 0.0f;
                m_rotGoal.m_y = nw4r::math::Atan2FIdx(dir.m_x, dir.m_z) * 1.40625f;
            }
        }
        break;
    case 6:
        if (m_timer == 0.0f) {
            setMotion(3, 0, 1, &m_motionTimer);
            if (m_seIdCharge != -1) {
                m_snd.stopSE(m_seIdCharge, 0);
            }
            m_seIdCharge = -1;
            m_snd.playSE(snd_se_stage_Halberd_Laser_shot, 0, 0, -1);
            *m_stateWork = 10;
            m_state = 8;
        }
        break;
    case 8:
        if (*m_stateWork == 11) {
            setMotion(0, 0, 1, &m_motionTimer);
            *m_stateWork = 0x15;
        }
        break;
    }
    u8 motion = m_motion;
    if (motion != 1) {
        if (motion == 0) {
            if (m_motionTimer == 0.0f) {
                m_rotStart.m_x = m_rot.m_x;
                m_rotStart.m_y = m_rot.m_y;
                m_rotStart.m_z = m_rot.m_z;
                if (m_seIdEngine != -1) {
                    m_snd.stopSE(m_seIdEngine, 0);
                }
                m_seIdEngine = -1;
                m_snd.playSE(snd_se_stage_Halberd_finish, 0, 0, -1);
                m_state = 0;
            } else {
                m_seSeq.playFrame(1, getMotionFrame(0));
            }
        } else if (motion < 3) {
            if (m_motionTimer == 0.0f) {
                setMotion(1, 1, 1, NULL);
            } else {
                m_seSeq.playFrame(0, getMotionFrame(0));
            }
        }
    }
}

// Turns the barrel towards the angles of the goal, no faster than the stage allows.
void grHalberdCannon::updateRot() {
    stHalberdData* data = static_cast<stHalberdData*>(getStageData());
    if (data != NULL) {
        Vec3f rotNow = m_rot;
        Vec3f diff = m_rotGoal - rotNow;
        float limit = data->unk90;
        if (0.0f < diff.m_x && limit < diff.m_x) {
            diff.m_x = limit;
        }
        if (diff.m_x < 0.0f && diff.m_x < -limit) {
            diff.m_x = -limit;
        }
        if (0.0f < diff.m_y && limit < diff.m_y) {
            diff.m_y = limit;
        }
        if (diff.m_y < 0.0f && diff.m_y < -limit) {
            diff.m_y = -limit;
        }
        m_rot.m_x = m_rot.m_x + diff.m_x;
        m_rot.m_y = m_rot.m_y + diff.m_y;
        m_rot.m_z = m_rot.m_z + diff.m_z;
        if (m_rot.m_x > 5.0f) {
            m_rot.m_x = 5.0f;
        }
        if (m_rot.m_x < -5.0f) {
            m_rot.m_x = -5.0f;
        }
        if (m_rot.m_y > 20.0f) {
            m_rot.m_y = 20.0f;
        }
        if (m_rot.m_y < -20.0f) {
            m_rot.m_y = -20.0f;
        }
        m_mtxWork->rotY(m_rot.m_y * 0.017453292f);
        fn_8003EA9C(m_mtxWork, m_rot.m_x * 0.017453292f);
        fn_8003EBF4(m_mtxWork, m_rot.m_z * 0.017453292f);
    }
}

void grHalberdCannon::updateCallBack(float deltaFrame) {
    grCalcWorldCallBack* calcWorldCallBack = &m_calcWorldCallBack;
    if (calcWorldCallBack != NULL) {
        nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[0];
        if (scnMdl != NULL) {
            if (scnMdl->m_calcWorldCallBack == NULL) {
                calcWorldCallBack->m_index = 0;
                scnMdl->m_calcWorldCallBack = calcWorldCallBack;
                scnMdl->EnableScnMdlCallbackTiming(1);
                scnMdl->m_nodeIndex = calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex;
            }
            if (m_mtxWork != NULL) {
                calcWorldCallBack->m_nodeCallbackDatas[0].m_matrix = *m_mtxWork;
                Vec3f pos(m_mtxWork->m[0][3], m_mtxWork->m[1][3], m_mtxWork->m[2][3]);
                m_snd.setPos(&pos);
            }
        }
    }
}

// The animation of the cannon (5 animations).
void grHalberdCannon::setMotion(u32 animId, bool shouldLoop, bool force, float* frameCount) {
    if (m_motion == animId && force == 0) {
        return;
    }

    nw4r::g3d::ScnMdl* sceneMdl = *m_sceneModels;
    if (sceneMdl == NULL) {
        return;
    }

    gfModelAnimation* modelAnim = *m_modelAnims;
    if (modelAnim == NULL) {
        return;
    }

    nw4r::g3d::ResMdl model = sceneMdl->m_resMdl;
    if (!model.IsValid()) {
        return;
    }

    modelAnim->unbindNodeAnim(sceneMdl);
    modelAnim->unbindVisibleAnim(sceneMdl);
    modelAnim->unbindTexAnim(sceneMdl);
    modelAnim->unbindTexSrtAnim(sceneMdl);
    modelAnim->unbindMatColAnim(sceneMdl);
    m_motion = animId;

    if (animId >= 5) {
        return;
    }

    bool result = (modelAnim->m_resFile.GetResAnmChrNumEntries() > animId);
    if (result) {
        grHalberdSetChrAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmVisNumEntries() > animId);
    if (result) {
        grHalberdSetVisibilityAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmTexPatNumEntries() > animId);
    if (result) {
        grHalberdSetTexPatAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmTexSrtNumEntries() > animId);
    if (result) {
        grHalberdSetTexSrtAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmClrNumEntries() > animId);
    if (result) {
        grHalberdSetColorAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    gfModelAnimation::bind(sceneMdl, modelAnim);
    modelAnim->setFrame(0.0);
    modelAnim->setUpdateRate(1.0);
    modelAnim->setLoop(shouldLoop);

    if (frameCount != NULL) {
        *frameCount = modelAnim->getFrameCount();
    }
}

bool grHalberdCannon::getPlayerPosition(int playerNo, Vec3f* pos) {
    return stMelee::getPlayerPosition(playerNo, pos);
}
