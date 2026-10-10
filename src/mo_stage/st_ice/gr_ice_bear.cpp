#include <cm/cm_quake.h>
#include <gf/gf_model.h>
#include <gr/gr_calc_world_callback.h>
#include <memory.h>
#include <mt/mt_prng.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <nw4r/math/math_triangular.h>
#include <snd/snd_system.h>
#include <types.h>

#include <st_ice/gr_ice.h>
#include <st_ice/gr_ice_anim.h>

// One step of the walk: the bear puts its foot down (it moves down by a part of 35 units following a sine) and shakes the
// camera when the foot is on the ground.
#define ICE_BEAR_STEP(base, SET_STATE)                                                          \
    {                                                                                           \
        int index = (int)((1.0f - (frame - (base)) / 10.0f) * 16384.0f);                        \
        float sine = nw4r::math::SinIdx((u16)index);                                            \
        if (sine < 0.0f) {                                                                      \
            sine = 0.0f;                                                                        \
        }                                                                                       \
        if (1.0f < sine) {                                                                      \
            sine = 1.0f;                                                                        \
        }                                                                                       \
        m_pos.m_y = m_pos.m_y + (35.0f - sine * 35.0f);                                         \
        if (sine == 1.0f && m_quakeSet == 0) {                                                  \
            Vec3f quake;                                                                        \
            quake.m_x = 0.0f;                                                                   \
            quake.m_y = 0.0f;                                                                   \
            quake.m_z = 0.0f;                                                                   \
            cmReqQuake(cmQuake::Amplitude_Large, &quake);                                       \
            SET_STATE                                                                           \
            m_quakeSet = 1;                                                                     \
        }                                                                                       \
    }

// The bear walks to the left or to the right along its way (the angle says where): the ratio goes up or down and the walk
// animation stops at the end.
#define ICE_BEAR_WALK                                                                           \
    if (m_rot.m_y == 0.0f) {                                                                    \
        m_ratio = m_ratio + deltaFrame * 0.005f;                                                \
    } else {                                                                                    \
        m_ratio = m_ratio - deltaFrame * 0.005f;                                                \
    }                                                                                           \
    if (m_ratio < 0.0f) {                                                                       \
        m_ratio = 0.0f;                                                                         \
    }                                                                                           \
    if (1.0f < m_ratio) {                                                                       \
        m_ratio = 1.0f;                                                                         \
    }                                                                                           \
    if ((m_rot.m_y == 0.0f && m_ratio == 1.0f) || (m_rot.m_y == 180.0f && m_ratio == 0.0f)) {   \
        setMotion(2, false, true, &m_motionTimer);                                              \
    }


grIceBear::grIceBear(const char* taskName) : grIce(taskName), m_snd() {
    m_posWork = NULL;
    m_limitWork = NULL;
    m_stateWork = NULL;
    m_pos.m_x = 0.0f;
    m_pos.m_y = 0.0f;
    m_pos.m_z = 0.0f;
    m_rot.m_x = 0.0f;
    m_rot.m_y = 0.0f;
    m_rot.m_z = 0.0f;
    m_fallAccel = 0.0f;
    m_speedY = 0.0f;
    m_ratio = 0.0f;
    m_bounce = 0;
    m_quakeSet = 0;
    m_motion = 5;
    m_motionTimer = 0.0f;
    m_motionLength = 0.0f;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_numNodeCallbackData = 1;
    callback->initialize(false, Heaps::StageInstance);
    callback->m_nodeCallbackDatas[0].m_flags |= 1;
    callback->m_nodeCallbackDatas[0].m_flags |= 2;
}

grIceBear* grIceBear::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grIceBear* ground = new (Heaps::StageInstance) grIceBear(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grIceBear::~grIceBear() {
}

void grIceBear::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateMove(deltaFrame);
        updateMotion(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

// The bear falls from above the mountain when the stage says so, bounces on the ground, walks along the places of the
// stage (the ratio is how far it is on the way), and leaves on the right side.
void grIceBear::updateMove(float deltaFrame) {
    stIceData* data = static_cast<stIceData*>(getStageData());
    if (data == NULL) {
        return;
    }
    float timer = m_timer - deltaFrame;
    m_timer = timer;
    if (timer < 0.0f) {
        m_timer = 0.0f;
    }
    u8 state = m_state;
    if (state == 8) {
        if (m_bounce < 5) {
            float speed = m_speedY + m_fallAccel * deltaFrame;
            m_speedY = speed;
            m_pos.m_y = m_pos.m_y + speed * deltaFrame;
            if (m_rot.m_y == 0.0f) {
                m_ratio = m_ratio + deltaFrame * 0.00575f;
            } else {
                m_ratio = m_ratio - deltaFrame * 0.00575f;
            }
            if (m_ratio < 0.0f) {
                m_ratio = 0.0f;
            }
            if (1.0f < m_ratio) {
                m_ratio = 1.0f;
            }
            Vec3f ground;
            calcPos(&ground);
            m_pos.m_x = ground.m_x;
            m_pos.m_z = ground.m_z;
            if (0.0f <= m_speedY || ground.m_y <= m_pos.m_y) {
                if (m_motion != 4) {
                    setMotion(4, true, false, &m_motionTimer);
                    float random = randf();
                    m_bounce = 0;
                    m_motionTimer = m_motionTimer * (random * 5.0f + 3.0f);
                }
            } else {
                m_speedY = m_speedY * -0.35f;
                m_fallAccel = m_fallAccel * 0.85f;
                m_bounce = m_bounce + 1;
            }
        } else {
            float random = randf();
            m_state = 9;
            m_timer = data->unk90 + (data->unk94 - data->unk90) * random;
        }
    } else if (state < 8) {
        if (state == 1) {
            if (*m_stateWork == 5) {
                m_ratio = 0.0f;
                m_rot.m_y = 0.0f;
                calcPos(&m_pos);
                float y = m_limitWork[0].m_y;
                m_fallAccel = -0.1f;
                m_pos.m_y = y + 35.0f;
                m_speedY = 0.0f;
                setMotion(3, true, true, &m_motionTimer);
                setVisibility(1);
                m_state = 8;
            }
        } else if (state == 0) {
            setMotion(5, false, true, NULL);
            setVisibility(0);
            m_bounce = 0;
            m_rot.m_x = 0.0f;
            m_rot.m_y = 0.0f;
            m_rot.m_z = 0.0f;
            m_pos.m_x = 0.0f;
            m_pos.m_y = 0.0f;
            m_pos.m_z = 0.0f;
            *m_stateWork = 0xE;
            m_state = 1;
        }
    } else if (state == 10) {
        if (m_timer == 0.0f) {
            float angle = m_rot.m_z + deltaFrame * 5.0f;
            m_rot.m_z = angle;
            if (360.0f < angle) {
                m_rot.m_z = angle - 360.0f;
            }
            m_pos.m_x = m_pos.m_x + deltaFrame * 2.0f;
            m_pos.m_y = m_pos.m_y + deltaFrame * 1.5f;
        } else {
            m_rot.m_y = 180.0f;
            float ratio = m_ratio - deltaFrame * 0.002f;
            m_ratio = ratio;
            if (ratio < 0.0f) {
                m_ratio = 0.0f;
            }
            if (1.0f < m_ratio) {
                m_ratio = 1.0f;
            }
            calcPos(&m_pos);
        }
        if (m_limitWork[1].m_x < m_pos.m_x) {
            m_state = 0;
        }
    } else if (state < 10) {
        u8 stage = *m_stateWork;
        if (stage == 5) {
            if (m_motionTimer == 0.0f) {
                float random = randf();
                m_timer = data->unk90 + (data->unk94 - data->unk90) * random + m_motionTimer;
                if (m_motion == 4 || m_motion == 3) {
                    if (data->unk98 <= randf()) {
                        setMotion(0, false, true, &m_motionTimer);
                        m_motionLength = m_motionTimer;
                    } else {
                        setMotion(1, false, true, &m_motionTimer);
                        m_motionLength = m_motionTimer;
                    }
                }
            } else {
                u8 motion = m_motion;
                if (motion == 1) {
                    calcPos(&m_pos);
                    float length = m_motionLength;
                    float frame = m_motionTimer;
                    if (frame <= length - 44.0f) {
                        if (frame <= length - 50.0f) {
                            if (frame < length - 60.0f) {
                                if (frame <= length - 74.0f) {
                                    if (frame <= length - 80.0f) {
                                        if (length - 90.0f <= frame) {
                                            ICE_BEAR_STEP(length - 90.0f, )
                                        }
                                    } else {
                                        m_pos.m_y = m_pos.m_y + (1.0f - (frame - (length - 80.0f)) / 6.0f) * 35.0f;
                                    }
                                } else {
                                    m_quakeSet = 0;
                                }
                            } else {
                                ICE_BEAR_STEP(length - 60.0f, *m_stateWork = 7;)
                            }
                        } else {
                            m_pos.m_y = m_pos.m_y + (1.0f - (frame - (length - 50.0f)) / 6.0f) * 35.0f;
                        }
                    } else {
                        m_quakeSet = 0;
                    }
                } else if (motion == 0) {
                    calcPos(&m_pos);
                    float length = m_motionLength;
                    float frame = m_motionTimer;
                    if (frame <= length - 44.0f) {
                        if (frame <= length - 50.0f) {
                            if (length - 60.0f <= frame) {
                                ICE_BEAR_STEP(length - 60.0f, *m_stateWork = 6;)
                            }
                        } else {
                            m_pos.m_y = m_pos.m_y + (1.0f - (frame - (length - 50.0f)) / 6.0f) * 35.0f;
                        }
                    } else {
                        m_quakeSet = 0;
                    }
                } else if (motion == 4) {
                    ICE_BEAR_WALK
                    calcPos(&m_pos);
                } else {
                    calcPos(&m_pos);
                }
            }
        } else if (stage < 5) {
            if (stage == 0) {
                if (m_motion == 4) {
                    ICE_BEAR_WALK
                }
                calcPos(&m_pos);
            }
        }
    } else if (state == 0xC) {
        setMotion(4, true, false, &m_motionTimer);
        m_state = 10;
        m_timer = 270.0f;
    }
}

// The animation of the bear: it looks around, turns and sits down by timers (the numbers are the animations 0 - 4).
void grIceBear::updateMotion(float deltaFrame) {
    float timer = m_motionTimer - deltaFrame;
    m_motionTimer = timer;
    if (timer < 0.0f) {
        m_motionTimer = 0.0f;
    }
    if (m_state == 9) {
        u8 motion = m_motion;
        if (motion == 2) {
            if (m_motionTimer == 0.0f) {
                setMotion(4, true, false, &m_motionTimer);
                float random = randf();
                m_motionTimer = m_motionTimer * (random * 5.0f + 3.0f);
                if (m_rot.m_y == 0.0f) {
                    m_rot.m_y = 180.0f;
                } else {
                    m_rot.m_y = 0.0f;
                }
            }
        } else if (motion < 2) {
            if (motion == 0) {
                if (m_motionTimer == 0.0f) {
                    m_snd.playSE(static_cast<SndID>(0x1C87), 0, 0, -1);
                    setMotion(3, true, false, &m_motionTimer);
                    float random = randf();
                    m_motionTimer = m_motionTimer * (random * 8.0f + 3.0f);
                }
            } else if (m_motionTimer == 0.0f) {
                m_snd.playSE(static_cast<SndID>(0x1C87), 0, 0, -1);
                setMotion(3, true, false, &m_motionTimer);
                float random = randf();
                m_motionTimer = m_motionTimer * (random * 12.0f + 5.0f);
            }
        } else if (motion == 4) {
            if (m_motionTimer == 0.0f) {
                setMotion(3, true, false, &m_motionTimer);
                float random = randf();
                m_motionTimer = m_motionTimer * (random * 5.0f + 3.0f);
            }
        } else if (motion < 4 && m_motionTimer == 0.0f) {
            setMotion(4, true, false, &m_motionTimer);
            float random = randf();
            m_motionTimer = m_motionTimer * (random * 5.0f + 3.0f);
        }
    }
}

void grIceBear::updateCallBack(float deltaFrame) {
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
            grNodeCallbackData* data = calcWorldCallBack->m_nodeCallbackDatas;
            data->m_pos.m_x = m_pos.m_x;
            data->m_pos.m_y = m_pos.m_y;
            data->m_pos.m_z = m_pos.m_z;
            data = calcWorldCallBack->m_nodeCallbackDatas;
            data->m_rot.m_x = m_rot.m_x;
            data->m_rot.m_y = m_rot.m_y;
            data->m_rot.m_z = m_rot.m_z;
            m_snd.setPos(&m_pos);
        }
    }
}

// The place of the bear on its way: the ratio (0 - 1) is split in four parts, one for each pair of places.
void grIceBear::calcPos(Vec3f* pos) {
    if (pos == NULL) {
        return;
    }
    float ratio = m_ratio;
    float part;
    Vec3f start;
    Vec3f end;
    if (0.25f <= ratio) {
        if (0.5f <= ratio) {
            if (0.75f <= ratio) {
                Vec3f* places = m_posWork;
                start.m_x = places[3].m_x;
                part = (ratio - 0.75f) / 0.25f;
                start.m_y = places[3].m_y;
                start.m_z = places[3].m_z;
                end.m_x = places[4].m_x;
                end.m_y = places[4].m_y;
                end.m_z = places[4].m_z;
            } else {
                Vec3f* places = m_posWork;
                start.m_x = places[2].m_x;
                part = (ratio - 0.5f) / 0.25f;
                start.m_y = places[2].m_y;
                start.m_z = places[2].m_z;
                end.m_x = places[3].m_x;
                end.m_y = places[3].m_y;
                end.m_z = places[3].m_z;
            }
        } else {
            Vec3f* places = m_posWork;
            start.m_x = places[1].m_x;
            part = (ratio - 0.25f) / 0.25f;
            start.m_y = places[1].m_y;
            start.m_z = places[1].m_z;
            end.m_x = places[2].m_x;
            end.m_y = places[2].m_y;
            end.m_z = places[2].m_z;
        }
    } else {
        part = ratio / 0.25f;
        Vec3f* places = m_posWork;
        start.m_x = places[0].m_x;
        start.m_y = places[0].m_y;
        start.m_z = places[0].m_z;
        end.m_x = places[1].m_x;
        end.m_y = places[1].m_y;
        end.m_z = places[1].m_z;
    }
    Vec3f dir;
    dir.m_x = end.m_x - start.m_x;
    dir.m_y = end.m_y - start.m_y;
    dir.m_z = end.m_z - start.m_z;
    float length = grIceVecLength(&dir);
    dir.normalize();
    float distance = length * part;
    pos->m_x = start.m_x + dir.m_x * distance;
    pos->m_y = start.m_y + dir.m_y * distance;
    pos->m_z = start.m_z + dir.m_z * distance;
}

// The bear has all its animations (visibility, character, texture pattern, texture SRT and material colour) at the same index.
void grIceBear::setMotion(u32 animId, bool loop, bool force, float* frameCount) {
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

    if (animId < 5) {
        bool result = (modelAnim->m_resFile.GetResAnmVisNumEntries() > animId);
        if (result) {
            grIceSetVisibilityAnim2(animId, model, modelAnim, Heaps::StageInstance);
        }

        result = (modelAnim->m_resFile.GetResAnmChrNumEntries() > animId);
        if (result) {
            grIceSetChrAnim2(animId, model, modelAnim, Heaps::StageInstance);
        }

        result = (modelAnim->m_resFile.GetResAnmTexPatNumEntries() > animId);
        if (result) {
            grIceSetTexPatAnim2(animId, model, modelAnim, Heaps::StageInstance);
        }

        result = (modelAnim->m_resFile.GetResAnmTexSrtNumEntries() > animId);
        if (result) {
            grIceSetTexSrtAnim2(animId, model, modelAnim, Heaps::StageInstance);
        }

        result = (modelAnim->m_resFile.GetResAnmClrNumEntries() > animId);
        if (result) {
            grIceSetColorAnim2(animId, model, modelAnim, Heaps::StageInstance);
        }

        gfModelAnimation::bind(sceneMdl, modelAnim);
        modelAnim->setFrame(0.0);
        modelAnim->setUpdateRate(1.0);
        modelAnim->setLoop(loop);

        if (frameCount != NULL) {
            *frameCount = modelAnim->getFrameCount();
        }
    }
}
