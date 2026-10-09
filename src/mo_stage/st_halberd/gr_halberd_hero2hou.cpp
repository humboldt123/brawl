#include <gr/gr_calc_world_callback.h>
#include <memory.h>
#include <nw4r/math/math_triangular.h>

#include <st_halberd/gr_halberd.h>
#include <st_halberd/gr_halberd_anim.h>

grHalberdHero2Hou* grHalberdHero2Hou::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grHalberdHero2Hou* ground = new (Heaps::StageInstance) grHalberdHero2Hou(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grHalberdHero2Hou::grHalberdHero2Hou(const char* taskName) : grHalberd(taskName) {
    m_mtxWork = NULL;
    m_mtxGimmickWork = NULL;
    m_posTgtWork = NULL;
    m_unk16c.m_x = 0.0f;
    m_unk16c.m_y = 0.0f;
    m_unk16c.m_z = 0.0f;
    m_rotStart.m_x = 0.0f;
    m_rotStart.m_y = 0.0f;
    m_rotStart.m_z = 0.0f;
    m_rotGoal.m_x = 0.0f;
    m_rotGoal.m_y = 0.0f;
    m_rotGoal.m_z = 0.0f;
    m_rot.m_x = 0.0f;
    m_rot.m_y = 0.0f;
    m_rot.m_z = 0.0f;
    m_motion = 4;
    m_lastFrame = 0.0f;
    m_motionTimer = 0.0f;
    m_soundPhase = 0;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_numNodeCallbackData = 1;
    callback->initialize(false, Heaps::StageInstance);
    callback->m_nodeCallbackDatas[0].m_flags |= 8;
}

grHalberdHero2Hou::~grHalberdHero2Hou() {
}

void grHalberdHero2Hou::processAnim() {
    Ground::processAnim();
    if (m_mtxGimmickWork != NULL) {
        getNodeMatrix(&m_mtxGimmickWork[9], 0, "hero2ShotPosition");
    }
}

void grHalberdHero2Hou::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateMotion(deltaFrame);
        updateRot();
        updateCallBack(deltaFrame);
    }
}

// The rear gun: it opens (2), pops up (5), turns towards the target (6), fires (8) and closes again.
void grHalberdHero2Hou::updateMotion(float deltaFrame) {
    if (m_stateWork != NULL) {
        stHalberdData* data = static_cast<stHalberdData*>(getStageData());
        if (data != NULL) {
            m_timer -= deltaFrame;
            if (m_timer < 0.0f) {
                m_timer = 0.0f;
            }
            switch (m_state) {
            case 0:
                setMotion(2, 0, 1, NULL);
                m_state = 2;
                m_timer = 90.0f;
                // fall through
            case 2:
                if (*m_stateWork == 7) {
                    setMotion(0, 1, 1, &m_motionTimer);
                    m_lastFrame = 0.0f;
                    m_snd.playSE(snd_se_stage_Halberd_Hero_Up_01, 0, 0, -1);
                    m_soundPhase = 1;
                    *m_stateWork = 0xC;
                    m_state = 5;
                    m_timer = m_motionTimer * 2.0f;
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
            case 5: {
                u8 phase = m_soundPhase;
                if (phase == 1) {
                    if (getMotionFrame(0) >= 40.0f) {
                        m_snd.playSE(snd_se_stage_Halberd_Hero_Up_02, 0, 0, -1);
                        m_soundPhase = 2;
                    }
                } else if (phase == 0) {
                    if (getMotionFrame(0) < m_lastFrame) {
                        m_snd.playSE(snd_se_stage_Halberd_Hero_Up_01, 0, 0, -1);
                        m_soundPhase = 1;
                    }
                } else if (phase < 3 && getMotionFrame(0) >= 80.0f) {
                    m_snd.playSE(snd_se_stage_Halberd_Hero_Down, 0, 0, -1);
                    m_soundPhase = 0;
                }
                m_lastFrame = getMotionFrame(0);
                if (m_timer == 0.0f && *m_stateWork == 0xD) {
                    setMotion(3, 1, 1, NULL);
                    m_snd.playSE(snd_se_stage_Halberd_Hero_set, 0, 0, -1);
                    m_rotStart.m_x = m_rot.m_x;
                    m_rotStart.m_y = m_rot.m_y;
                    m_rotStart.m_z = m_rot.m_z;
                    Matrix* shot = &m_mtxGimmickWork[9];
                    Vec3f* target = m_posTgtWork;
                    Vec3f diff(target->m_x - shot->m[0][3], target->m_y - shot->m[1][3], target->m_z - shot->m[2][3]);
                    float length = 0.0f;
                    bool zero = false;
                    if (fabsf(diff.m_x) < 1e-5f && fabsf(diff.m_y) < 1e-5f && fabsf(diff.m_z) < 1e-5f) {
                        zero = true;
                    }
                    if (!zero) {
                        length = grHalberdLength(diff.m_z, diff.m_x, diff.m_y);
                    }
                    float height = shot->m[1][3] + length * 0.5f;
                    Vec3f dir(target->m_x - shot->m[0][3], height - shot->m[1][3], target->m_z - shot->m[2][3]);
                    dir.normalize();
                    m_rotGoal.m_x = nw4r::math::Atan2FIdx(dir.m_z, dir.m_y) * 1.40625f - 90.0f;
                    m_rotGoal.m_y = nw4r::math::Atan2FIdx(dir.m_x, dir.m_z) * 1.40625f;
                    m_state = 6;
                    m_rotGoal.m_z = 0.0f;
                }
                break;
            }
            case 6:
                if (m_rotGoal.m_x == m_rot.m_x && m_rotGoal.m_y == m_rot.m_y) {
                    setMotion(1, 0, 1, &m_motionTimer);
                    m_snd.playSE(snd_se_stage_Halberd_Hero_Up_02, 0, 0, -1);
                    m_snd.playSE(snd_se_stage_Halberd_Hero_fire, 0, 0, -1);
                    *m_stateWork = 0xF;
                    m_state = 8;
                }
                break;
            case 8:
                if (*m_stateWork == 0x11) {
                    *m_stateWork = 0x15;
                    m_rotStart.m_x = m_rot.m_x;
                    m_rotStart.m_y = m_rot.m_y;
                    m_rotStart.m_z = m_rot.m_z;
                    m_snd.playSE(snd_se_stage_Halberd_Hero_Down, 0, 0, -1);
                    m_state = 0;
                }
                break;
            }
            if (m_motionTimer <= getMotionFrame(0) && m_motion == 1) {
                setMotion(2, 0, 1, NULL);
                *m_stateWork = 0x10;
            }
        }
    }
}

// Turns the barrel towards the angles of the goal, no faster than the stage allows.
void grHalberdHero2Hou::updateRot() {
    stHalberdData* data = static_cast<stHalberdData*>(getStageData());
    if (data != NULL) {
        float dx = m_rotGoal.m_x - m_rot.m_x;
        float dy = m_rotGoal.m_y - m_rot.m_y;
        float limit = data->unkA8;
        if (0.0f < dx && limit < dx) {
            dx = limit;
        }
        if (dx < 0.0f && dx < -limit) {
            dx = -limit;
        }
        if (0.0f < dy && limit < dy) {
            dy = limit;
        }
        if (dy < 0.0f && dy < -limit) {
            dy = -limit;
        }
        dy = m_rot.m_y + dy;
        m_rot.m_x = m_rot.m_x + dx;
        m_rot.m_y = dy;
        m_rot.m_z = m_rot.m_z + (m_rotGoal.m_z - m_rot.m_z);
        m_mtxWork->rotY(dy * 0.017453292f);
        fn_8003EA9C(m_mtxWork, m_rot.m_x * 0.017453292f);
        fn_8003EBF4(m_mtxWork, m_rot.m_z * 0.017453292f);
    }
}

void grHalberdHero2Hou::updateCallBack(float deltaFrame) {
    grCalcWorldCallBack* calcWorldCallBack = &m_calcWorldCallBack;
    if (calcWorldCallBack != NULL) {
        nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[0];
        if (scnMdl != NULL) {
            if (scnMdl->m_calcWorldCallBack == NULL) {
                calcWorldCallBack->m_index = 0;
                calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex = m_nodeIndex;
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

// The animation of the rear gun (4 animations).
void grHalberdHero2Hou::setMotion(u32 animId, bool shouldLoop, bool force, float* frameCount) {
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

    if (animId >= 4) {
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
