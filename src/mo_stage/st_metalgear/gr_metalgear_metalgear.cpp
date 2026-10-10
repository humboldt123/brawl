#include <gf/gf_model.h>
#include <gr/gr_calc_world_callback.h>
#include <math.h>
#include <memory.h>
#include <mt/mt_prng.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <nw4r/math/math_arithmetic.h>
#include <nw4r/math/math_triangular.h>
#include <snd/snd_system.h>
#include <types.h>

#include <st_metalgear/gr_metalgear.h>
#include <st_metalgear/gr_metalgear_anim.h>

// sndSystem::setVol (main, unnamed): the volume of a sound that plays, changed over the given frames
extern "C" void fn_800777DC(sndSystem* sndSystem, int handle, int frames, float volume);

grMetalgearMetalgear::grMetalgearMetalgear(const char* taskName) : grMetalgear(taskName) {
    m_posWork = NULL;
    m_stateWork = NULL;
    m_pos.m_x = 0.0f;
    m_pos.m_y = 0.0f;
    m_pos.m_z = 0.0f;
    m_rot.m_x = 0.0f;
    m_rot.m_y = 0.0f;
    m_rot.m_z = 0.0f;
    m_rotNext.m_x = 0.0f;
    m_rotNext.m_y = 0.0f;
    m_rotNext.m_z = 0.0f;
    m_turns = 0;
    m_type = 9;
    m_number = 0;
    m_motion = 0x80;
    m_frame = 0.0f;
    m_frameCount = 0.0f;
    m_motionEnd = 0;
    m_loop = 0;
    m_seIndex = -1;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_numNodeCallbackData = 1;
    callback->initialize(false, Heaps::StageInstance);
    callback->m_nodeCallbackDatas[0].m_flags |= 1;
    callback->m_nodeCallbackDatas[0].m_flags |= 2;
}

grMetalgearMetalgear::~grMetalgearMetalgear() {
}

void grMetalgearMetalgear::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateActive(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

void grMetalgearMetalgear::updateActive(float deltaFrame) {
}

void grMetalgearMetalgear::updateCallBack(float deltaFrame) {
    grCalcWorldCallBack* calcWorldCallBack = &m_calcWorldCallBack;
    if (calcWorldCallBack != NULL) {
        nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[m_unk1];
        if (scnMdl != NULL) {
            if (scnMdl->m_calcWorldCallBack == NULL) {
                calcWorldCallBack->m_index = 0;
                calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex = m_nodeIndex;
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
        }
    }
}

void grMetalgearMetalgear::setMotion(u32 animId, bool loop, bool force, float* frameCount) {
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

    if (animId >= 0x80) {
        return;
    }

    bool result = (modelAnim->m_resFile.GetResAnmChrNumEntries() > animId);
    if (result) {
        grMetalgearSetChrAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmTexPatNumEntries() > animId);
    if (result) {
        grMetalgearSetTexPatAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmTexSrtNumEntries() > animId);
    if (result) {
        grMetalgearSetTexSrtAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmClrNumEntries() > animId);
    if (result) {
        grMetalgearSetColorAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmVisNumEntries() > animId);
    if (result) {
        grMetalgearSetVisibilityAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    gfModelAnimation::bind(sceneMdl, modelAnim);
    modelAnim->setFrame(0.0);
    modelAnim->setUpdateRate(1.0);
    modelAnim->setLoop(loop);
    m_loop = loop;

    if (frameCount != NULL) {
        *frameCount = modelAnim->getFrameCount();
    }
}

// HYPOTHESIS: the clamp helper that the other stages use as well.
static inline float metalgearClamp(float value, float lo, float hi) {
    value = nw4r::math::FSelect(value - lo, value, lo);
    return nw4r::math::FSelect(value - hi, hi, value);
}


// The sounds of a sequence: the id, the frame it ends at and a second frame (nothing is known about that one).
static inline void metalgearSetSeData(StSeUtil::UnkStruct* data, SndID id, float end, float unkC) {
    data->id = id;
    data->unk4 = 0.0f;
    data->unk8 = end;
    data->unkC = unkC;
}

grMetalgearGekko::grMetalgearGekko(const char* taskName) : grMetalgearMetalgear(taskName) {
    m_posStart.m_x = 0.0f;
    m_posStart.m_y = 0.0f;
    m_posStart.m_z = 0.0f;
    m_posEnd.m_x = 0.0f;
    m_posEnd.m_y = 0.0f;
    m_posEnd.m_z = 0.0f;
    m_isWall = false;
    m_seHandle = -1;
    m_seIds[0] = static_cast<SndID>(0x1D12);
    m_seIds[1] = static_cast<SndID>(0x1D13);
    m_seIds[2] = static_cast<SndID>(0x1D14);
    m_seIds[3] = static_cast<SndID>(0x1D15);
    m_seIds[4] = static_cast<SndID>(0x1D10);
    metalgearSetSeData(&m_seData[0], static_cast<SndID>(0x1D12), 43.0f, 0.0f);
    metalgearSetSeData(&m_seData[1], static_cast<SndID>(0x1D13), 125.0f, 0.0f);
    metalgearSetSeData(&m_seData[2], static_cast<SndID>(0x1D10), 201.0f, 0.0f);
    metalgearSetSeData(&m_seData[3], static_cast<SndID>(0x1D12), 221.0f, 0.0f);
    metalgearSetSeData(&m_seData[4], static_cast<SndID>(0x1D13), 268.0f, 0.0f);
    metalgearSetSeData(&m_seData[5], static_cast<SndID>(0x1D14), 308.0f, 0.0f);
    metalgearSetSeData(&m_seData[6], static_cast<SndID>(0x1D15), 335.0f, 0.0f);
    metalgearSetSeData(&m_seData[7], static_cast<SndID>(0x1D14), 32.0f, 0.0f);
    metalgearSetSeData(&m_seData[8], static_cast<SndID>(0x1D15), 69.0f, 0.0f);
    metalgearSetSeData(&m_seData[9], static_cast<SndID>(0x1D12), 148.0f, 0.0f);
    metalgearSetSeData(&m_seData[10], static_cast<SndID>(0x1D13), 197.0f, 0.0f);
    metalgearSetSeData(&m_seData[11], static_cast<SndID>(0x1D13), 10.0f, 0.0f);
    metalgearSetSeData(&m_seData[12], static_cast<SndID>(0x1D14), 22.0f, 0.0f);
    metalgearSetSeData(&m_seData[13], static_cast<SndID>(0x1D15), 65.0f, 0.0f);
    metalgearSetSeData(&m_seData[14], static_cast<SndID>(0x1D12), 30.0f, 0.0f);
    metalgearSetSeData(&m_seData[15], static_cast<SndID>(0x1D13), 80.0f, 0.0f);
    metalgearSetSeData(&m_seData[16], static_cast<SndID>(0x1D12), 122.0f, 0.0f);
    metalgearSetSeData(&m_seData[17], static_cast<SndID>(0x1D13), 162.0f, 0.0f);
    metalgearSetSeData(&m_seData[18], static_cast<SndID>(0x1D15), 171.0f, 0.0f);
    metalgearSetSeData(&m_seData[19], static_cast<SndID>(0x1D12), 260.0f, 0.0f);
    metalgearSetSeData(&m_seData[20], static_cast<SndID>(0x1D12), 35.0f, 0.0f);
    metalgearSetSeData(&m_seData[21], static_cast<SndID>(0x1D13), 95.0f, 0.0f);
    m_sePlayer.registId(m_seIds, 5);
    m_sePlayer.registSeq(0, &m_seData[0], 7, Heaps::StageInstance);
    m_sePlayer.registSeq(1, &m_seData[7], 4, Heaps::StageInstance);
    m_sePlayer.registSeq(2, &m_seData[11], 3, Heaps::StageInstance);
    m_sePlayer.registSeq(3, &m_seData[14], 6, Heaps::StageInstance);
    m_sePlayer.registSeq(4, &m_seData[20], 2, Heaps::StageInstance);
}

grMetalgearGekko* grMetalgearGekko::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grMetalgearGekko* ground = new (Heaps::StageInstance) grMetalgearGekko(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grMetalgearGekko::~grMetalgearGekko() {
}

// The Gekko comes through the wall: it appears, jumps (motion 2) to a place in front of the stage, walks to places (the
// motions 0, 1, 3) and leaves.
void grMetalgearGekko::updateActive(float deltaFrame) {
    if (getStageData() == NULL) {
        return;
    }
    m_timer = m_timer - deltaFrame;
    if (m_timer < 0.0f) {
        m_timer = 0.0f;
    }
    switch (m_state) {
    case 0:
        setMotion(5, false, true, NULL);
        setVisibility(0);
        switch (m_number) {
        case 0: {
            Vec3f* pos = m_posWork;
            float x = pos->m_x;
            m_pos.m_x = x;
            float y = pos->m_y;
            m_pos.m_y = y;
            float z = pos->m_z;
            m_posStart.m_x = x;
            m_posStart.m_y = y;
            m_pos.m_z = z - 100.0f;
            m_posStart.m_z = z - 100.0f;
            m_posEnd.m_x = pos->m_x;
            m_posEnd.m_y = pos->m_y;
            m_posEnd.m_z = pos->m_z + 100.0f;
            break;
        }
        case 1: {
            Vec3f* pos = m_posWork;
            m_pos.m_x = pos->m_x;
            m_pos.m_y = pos->m_y;
            m_pos.m_z = pos->m_z;
            if (randf() < 0.5f) {
                m_pos.m_x = m_pos.m_x + 100.0f;
            } else {
                m_pos.m_x = m_pos.m_x - 100.0f;
            }
            m_posStart.m_y = m_pos.m_y;
            pos = m_posWork;
            m_posStart.m_x = m_pos.m_x;
            m_posStart.m_z = m_pos.m_z;
            Vec3f dir;
            dir = *pos - m_pos;
            dir.normalize();
            float length = randf() * 100.0f + 100.0f;
            dir.m_x = dir.m_x * length;
            dir.m_y = dir.m_y * length;
            dir.m_z = dir.m_z * length;
            m_posEnd = m_pos + dir;
            break;
        }
        }
        m_state = 1;
        break;
    case 1:
        if (*m_stateWork == 4) {
            setMotion(2, false, true, &m_frameCount);
            m_seIndex = 2;
            m_sePlayer.playFrame(m_seIndex, getMotionFrame(0), 0.0f);
            setVisibility(1);
            m_state = 6;
        }
        break;
    case 6:
        m_motionEnd = 0;
        if (m_loop == 0) {
            if (getMotionFrame(0) >= m_frameCount) {
                m_motionEnd = 1;
            }
        } else {
            if (getMotionFrame(0) < m_frame) {
                m_motionEnd = 1;
            } else {
                m_frame = getMotionFrame(0);
            }
        }
        switch (m_motion) {
        case 3:
            break;
        case 1:
            if (m_seHandle == -1 && getMotionFrame(0) >= 28.0f) {
                m_seHandle = g_sndSystem->playSE(snd_se_stage_Metalgear_09, 0, 0, 0, -1);
                if (m_seHandle != -1) {
                    fn_800777DC(g_sndSystem, m_seHandle, 0, 0.6f);
                }
            }
        case 2: {
            float rate = metalgearClamp(getMotionFrame(0) / 60.0f, 0.0f, 1.0f);
            float value = 1.0f - nw4r::math::CosFIdx(NW4R_MATH_IDX_TO_FIDX(nw4r::math::U16ToF32((u16)(int)(rate * 16384.0f))));
            Vec3f dir;
            dir = m_posEnd - m_posStart;
            float length = metalgearLength(dir);
            dir.normalize();
            length = length * value;
            dir.m_x = dir.m_x * length;
            dir.m_y = dir.m_y * length;
            dir.m_z = dir.m_z * length;
            m_pos = m_posStart + dir;
            m_rot.m_y = nw4r::math::Atan2FIdx(dir.m_x, dir.m_z) * 1.40625f;
            if (value == 1.0f) {
                m_pos.m_x = m_posEnd.m_x;
                m_pos.m_y = m_posEnd.m_y;
                m_pos.m_z = m_posEnd.m_z;
            }
            break;
        }
        case 4: {
            Vec3f dir;
            dir = m_posEnd - m_posStart;
            float angle = nw4r::math::Atan2FIdx(dir.m_x, dir.m_z) * 1.40625f;
            float diff = angle - m_rot.m_y;
            if ((float)fabs(diff) <= 1.25f) {
                if (m_motionEnd == 1) {
                    m_rot.m_y = angle;
                    if (isWall() == 1) {
                        u32 next;
                        if (randf() >= 0.5f) {
                            next = 3;
                        } else {
                            next = 0;
                        }
                        setMotion(next, false, true, &m_frameCount);
                        switch (next) {
                        case 3:
                            m_seIndex = 3;
                            break;
                        case 0:
                            m_seIndex = 0;
                            break;
                        }
                        if (m_seIndex != -1) {
                            m_sePlayer.playFrame(m_seIndex, getMotionFrame(0), 0.0f);
                        }
                        m_posStart.m_x = m_pos.m_x;
                        m_posStart.m_y = m_pos.m_y;
                        m_posStart.m_z = m_pos.m_z;
                        m_posEnd.m_x = m_pos.m_x;
                        m_posEnd.m_y = m_pos.m_y;
                        m_posEnd.m_z = m_pos.m_z;
                        m_rot.m_y = 0.0f;
                    } else {
                        float length;
                        bool near = false;
                        if (metalgearIsNearZero(dir.m_x) && metalgearIsNearZero(dir.m_y) && metalgearIsNearZero(dir.m_z)) {
                            near = true;
                        }
                        if (near) {
                            length = 0.0f;
                        } else {
                            length = metalgearLength(dir);
                        }
                        if (isEnableTgt() == 0) {
                            m_posEnd.m_x = m_pos.m_x;
                            length = 0.0f;
                            m_posEnd.m_y = m_pos.m_y;
                            m_posEnd.m_z = m_pos.m_z;
                            m_posStart.m_x = m_pos.m_x;
                            m_posStart.m_y = m_pos.m_y;
                            m_posStart.m_z = m_pos.m_z;
                        }
                        u32 next;
                        if (length >= 50.0f) {
                            next = 2;
                        } else {
                            next = 1;
                            m_seHandle = -1;
                        }
                        setMotion(next, false, true, &m_frameCount);
                        switch (next) {
                        case 2:
                            m_seIndex = 2;
                            break;
                        case 1:
                            m_seIndex = 1;
                            break;
                        }
                        if (m_seIndex != -1) {
                            m_sePlayer.playFrame(m_seIndex, getMotionFrame(0), 0.0f);
                        }
                    }
                    m_motionEnd = 0;
                }
            } else {
                if (diff > 0.0f) {
                    m_rot.m_y = m_rot.m_y + 1.25f;
                }
                if (angle - m_rot.m_y < 0.0f) {
                    m_rot.m_y = m_rot.m_y - 1.25f;
                }
            }
            break;
        }
        }
        break;
    }
    if (m_motionEnd == 1) {
        switch (m_motion) {
        case 3: {
            m_posStart.m_y = m_pos.m_y;
            Vec3f* pos = m_posWork;
            m_posStart.m_x = m_pos.m_x;
            m_posStart.m_z = m_pos.m_z;
            m_posEnd.m_x = pos->m_x;
            m_posEnd.m_y = pos->m_y;
            float dx = m_posEnd.m_x - m_posStart.m_x;
            float dy = m_posEnd.m_y - m_posStart.m_y;
            m_posEnd.m_z = pos->m_z - 100.0f;
            float dz = m_posEnd.m_z - m_posStart.m_z;
            m_rot.m_y = nw4r::math::Atan2FIdx(dx, dz) * 1.40625f;
            setMotion(2, false, true, &m_frameCount);
            m_seIndex = 2;
            m_sePlayer.playFrame(m_seIndex, getMotionFrame(0), 0.0f);
            break;
        }
        case 0:
            selectTgt();
            break;
        case 2:
        case 1:
            if (isWall() == 1) {
                u32 next;
                if (randf() >= 0.5f) {
                    next = 3;
                } else {
                    next = 0;
                }
                setMotion(next, false, true, &m_frameCount);
                switch (next) {
                case 3:
                    m_seIndex = 3;
                    break;
                case 0:
                    m_seIndex = 0;
                    break;
                }
                if (m_seIndex != -1) {
                    m_sePlayer.playFrame(m_seIndex, getMotionFrame(0), 0.0f);
                }
                m_posStart.m_x = m_pos.m_x;
                m_posStart.m_y = m_pos.m_y;
                m_posStart.m_z = m_pos.m_z;
                m_posEnd.m_x = m_pos.m_x;
                m_posEnd.m_y = m_pos.m_y;
                m_posEnd.m_z = m_pos.m_z;
                m_rot.m_y = 0.0f;
            } else {
                selectTgt();
            }
            break;
        }
    }
    if (m_seIndex != -1) {
        m_sePlayer.playFrame(m_seIndex, getMotionFrame(0));
    }
    if (m_number == 1) {
        Vec3f* pos = m_posWork;
        pos[8].m_x = m_posEnd.m_x;
        pos[8].m_y = m_posEnd.m_y;
        pos[8].m_z = m_posEnd.m_z;
        pos[9].m_x = m_pos.m_x;
        pos[9].m_y = m_pos.m_y;
        pos[9].m_z = m_pos.m_z;
    } else if (m_number == 0) {
        Vec3f* pos = m_posWork;
        pos[6].m_x = m_posEnd.m_x;
        pos[6].m_y = m_posEnd.m_y;
        pos[6].m_z = m_posEnd.m_z;
        pos[7].m_x = m_pos.m_x;
        pos[7].m_y = m_pos.m_y;
        pos[7].m_z = m_pos.m_z;
    }
}

// The Gekko walks to a new place in front of the wall: any place near the player side that is not in the wall.
void grMetalgearGekko::selectTgt() {
    setMotion(4, true, true, &m_frameCount);
    m_seIndex = 4;
    m_frame = 0.0f;
    m_sePlayer.playFrame(m_seIndex, getMotionFrame(0), 0.0f);
    m_posStart.m_x = m_pos.m_x;
    m_posStart.m_y = m_pos.m_y;
    m_posStart.m_z = m_pos.m_z;
    if (m_motion != 0 && randf() < 0.35f) {
        Vec3f* pos = m_posWork;
        m_posEnd.m_x = pos->m_x;
        m_posEnd.m_y = pos->m_y;
        m_posEnd.m_z = pos->m_z + 100.0f;
        if (isEnableTgt() == 1) {
            return;
        }
    }
    do {
        Vec3f* pos = m_posWork;
        Vec3f dir;
        dir.m_x = pos->m_x - m_posEnd.m_x;
        dir.m_y = pos->m_y - m_posEnd.m_y;
        dir.m_z = pos->m_z - m_posEnd.m_z;
        Vec3f dirNorm = dir;
        dirNorm.normalize();
        float angle = nw4r::math::Atan2FIdx(dirNorm.m_x, dirNorm.m_z) * 1.40625f;
        float sine;
        float cosine;
        nw4r::math::SinCosFIdx(&sine, &cosine, 0.7111111f * ((angle - 45.0f) + 90.0f * randf()));
        float length = 100.0f + 100.0f * randf();
        bool near = false;
        pos = m_posWork;
        m_posEnd.m_x = m_posEnd.m_x + length * sine;
        m_posEnd.m_z = m_posEnd.m_z + length * cosine;
        Vec3f diff;
        diff.m_x = m_posEnd.m_x - pos->m_x;
        diff.m_y = m_posEnd.m_y - pos->m_y;
        diff.m_z = m_posEnd.m_z - pos->m_z;
        if (metalgearIsNearZero(diff.m_x) && metalgearIsNearZero(diff.m_y) && metalgearIsNearZero(diff.m_z)) {
            near = true;
        }
        dirNorm = diff;
        if (!near) {
            if (metalgearLength(diff) > 100.0f) {
                dirNorm.normalize();
                dirNorm.m_x = dirNorm.m_x * 100.0f;
                dirNorm.m_y = dirNorm.m_y * 100.0f;
                dirNorm.m_z = dirNorm.m_z * 100.0f;
                pos = m_posWork;
                Vec3f end;
                end.m_x = pos->m_x + dirNorm.m_x;
                end.m_y = pos->m_y + dirNorm.m_y;
                end.m_z = pos->m_z + dirNorm.m_z;
                m_posEnd.m_x = end.m_x;
                m_posEnd.m_y = end.m_y;
                m_posEnd.m_z = end.m_z;
            }
        }
    } while (isEnableTgt() == 0);
}

// The Gekko is at the wall when it looks to the front (the rotation is small) and stands at the place of the wall.
bool grMetalgearGekko::isWall() {
    m_isWall = false;
    if (m_rot.m_y == 0.0f) {
        m_isWall = true;
    }
    if (m_rot.m_y < 0.0f && -15.0f < m_rot.m_y) {
        m_isWall = true;
    }
    if (0.0f < m_rot.m_y && m_rot.m_y < 15.0f) {
        m_isWall = true;
    }
    if (m_isWall == false) {
        return false;
    }
    bool same = false;
    Vec3f* pos = m_posWork;
    m_isWall = false;
    if (m_pos.m_x == pos[1].m_x && m_pos.m_y == pos[1].m_y && m_pos.m_z == pos[1].m_z) {
        same = true;
    }
    if (same) {
        m_isWall = true;
    }
    return m_isWall;
}

// The place is good when both of the markers of this Gekko's side are at least 60 units away from it.
bool grMetalgearGekko::isEnableTgt() {
    Vec3f a;
    Vec3f b;
    switch (m_number) {
    case 0:
        a = m_posWork[8] - m_posEnd;
        b = m_posWork[9] - m_posEnd;
        break;
    case 1:
        a = m_posWork[6] - m_posEnd;
        b = m_posWork[7] - m_posEnd;
        break;
    default:
        return false;
    }
    bool near = false;
    if (metalgearIsNearZero(a.m_x) && metalgearIsNearZero(a.m_y) && metalgearIsNearZero(a.m_z)) {
        near = true;
    }
    if (near == true) {
        return false;
    }
    near = false;
    if (metalgearIsNearZero(b.m_x) && metalgearIsNearZero(b.m_y) && metalgearIsNearZero(b.m_z)) {
        near = true;
    }
    if (near == true) {
        return false;
    }
    if (metalgearLength(a) < 60.0f) {
        return false;
    }
    if (metalgearLength(b) < 60.0f) {
        return false;
    }
    return true;
}

grMetalgearRay::grMetalgearRay(const char* taskName) : grMetalgearMetalgear(taskName) {
    m_seIds[0] = static_cast<SndID>(0x1D0D);
    metalgearSetSeData(&m_seData[0], static_cast<SndID>(0x1D0D), 98.0f, 0.0f);
    metalgearSetSeData(&m_seData[1], static_cast<SndID>(0x1D0D), 98.0f, 0.0f);
    m_sePlayer.registId(m_seIds, 1);
    m_sePlayer.registSeq(0, &m_seData[0], 1, Heaps::StageInstance);
    m_sePlayer.registSeq(1, &m_seData[1], 1, Heaps::StageInstance);
}

grMetalgearRay* grMetalgearRay::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grMetalgearRay* ground = new (Heaps::StageInstance) grMetalgearRay(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grMetalgearRay::~grMetalgearRay() {
}

// Ray swims in front of the back of the stage: it turns its head (the rotation goes between -60 and 60) in steps of 15 degrees.
void grMetalgearRay::updateActive(float deltaFrame) {
    stMetalgearData* data = static_cast<stMetalgearData*>(getStageData());
    if (data != NULL) {
        m_timer = m_timer - deltaFrame;
        if (m_timer < 0.0f) {
            m_timer = 0.0f;
        }
        float frame = getMotionFrame(0);
        switch (m_state) {
        case 0:
            setMotion(6, false, true, NULL);
            setVisibility(0);
            m_pos.m_x = m_posWork->m_x;
            m_pos.m_y = m_posWork->m_y;
            m_pos.m_z = m_posWork->m_z;
            m_state = 1;
            break;
        case 1:
            if (*m_stateWork == 4) {
                setMotion(0, false, true, &m_frameCount);
                frame = 0.0f;
                m_seIndex = 0;
                m_frame = 0.0f;
                m_sePlayer.playFrame(m_seIndex, getMotionFrame(0), frame);
                setVisibility(1);
                m_state = 6;
            }
            break;
        case 6:
            if (m_timer == 0.0f) {
                m_motionEnd = 1;
            }
            break;
        }
        if (frame >= m_frameCount || frame < m_frame) {
            u8 motion = m_motion;
            switch (motion) {
            case 0: {
                setMotion(3, true, false, &m_frameCount);
                frame = 0.0f;
                float r = randf();
                m_motionEnd = 0;
                m_seIndex = -1;
                m_timer = data->unk58 + (data->unk5C - data->unk58) * r;
                break;
            }
            case 1:
                m_rot.m_y = m_rot.m_y + 15.0f;
                if (m_rotNext.m_y + (float)m_turns * 15.0f <= m_rot.m_y || 60.0f <= m_rot.m_y) {
                    m_rot.m_y = m_rotNext.m_y + (float)m_turns * 15.0f;
                    if (m_rot.m_y > 60.0f) {
                        m_rot.m_y = 60.0f;
                    }
                    setMotion(3, true, false, &m_frameCount);
                    frame = 0.0f;
                    m_seIndex = -1;
                }
                break;
            case 2:
                m_rot.m_y = m_rot.m_y - 15.0f;
                if (m_rot.m_y <= m_rotNext.m_y - (float)m_turns * 15.0f || m_rot.m_y <= -60.0f) {
                    m_rot.m_y = m_rotNext.m_y - (float)m_turns * 15.0f;
                    if (m_rot.m_y < -60.0f) {
                        m_rot.m_y = -60.0f;
                    }
                    setMotion(3, true, false, &m_frameCount);
                    frame = 0.0f;
                    m_seIndex = -1;
                }
                break;
            case 3:
            case 4:
            case 5:
                if (m_motionEnd == 1) {
                    u8 next;
                    if (data->unk60 <= randf()) {
                        switch (m_motion) {
                        case 4:
                            if (randf() < 0.3f) {
                                next = 5;
                            } else {
                                next = 3;
                            }
                            break;
                        case 3:
                            if (randf() < 0.7f) {
                                next = 4;
                            } else {
                                next = 5;
                            }
                            break;
                        case 5:
                            if (randf() < 0.5f) {
                                next = 3;
                            } else {
                                next = 4;
                            }
                            break;
                        }
                    } else {
                        if (m_rot.m_y < 60.0f) {
                            if (-60.0f < m_rot.m_y) {
                                if (randf() < data->unk64) {
                                    next = 2;
                                } else {
                                    next = 1;
                                }
                            } else {
                                next = 1;
                            }
                        } else {
                            next = 2;
                        }
                        m_rotNext.m_x = m_rot.m_x;
                        m_rotNext.m_y = m_rot.m_y;
                        m_rotNext.m_z = m_rot.m_z;
                        m_turns = (int)(randf() * 2.0f + 2.0f);
                    }
                    setMotion(next, true, true, &m_frameCount);
                    frame = 0.0f;
                    switch (next) {
                    case 3:
                        m_seIndex = -1;
                        break;
                    case 1:
                        m_seIndex = -1;
                        break;
                    case 0:
                        m_seIndex = 0;
                        break;
                    case 2:
                        m_seIndex = -1;
                        break;
                    case 5:
                        m_seIndex = 1;
                        break;
                    case 4:
                        m_seIndex = -1;
                        break;
                    }
                    if (m_seIndex != -1) {
                        m_sePlayer.playFrame(m_seIndex, getMotionFrame(0), 0.0f);
                    }
                    float r = randf();
                    m_motionEnd = 0;
                    m_timer = data->unk58 + (data->unk5C - data->unk58) * r;
                }
                break;
            }
        }
        if (m_seIndex != -1) {
            m_sePlayer.playFrame(m_seIndex, getMotionFrame(0));
        }
        m_frame = frame;
    }
}

grMetalgearRex::grMetalgearRex(const char* taskName) : grMetalgearMetalgear(taskName) {
    m_seIds[0] = static_cast<SndID>(0x1D05);
    m_seIds[1] = static_cast<SndID>(0x1D06);
    m_seIds[2] = static_cast<SndID>(0x1D07);
    m_seIds[3] = static_cast<SndID>(0x1D08);
    m_seIds[4] = static_cast<SndID>(0x1D09);
    m_seIds[5] = static_cast<SndID>(0x1D0A);
    m_seIds[6] = static_cast<SndID>(0x1D0B);
    m_seIds[7] = static_cast<SndID>(0x1D0C);
    m_seIds[8] = static_cast<SndID>(0x1D0F);
    m_seIds[9] = static_cast<SndID>(0x1D04);
    metalgearSetSeData(&m_seData[0], static_cast<SndID>(0x1D07), 10.0f, 0.0f);
    metalgearSetSeData(&m_seData[1], static_cast<SndID>(0x1D04), 30.0f, 0.0f);
    metalgearSetSeData(&m_seData[2], static_cast<SndID>(0x1D05), 123.0f, 0.0f);
    metalgearSetSeData(&m_seData[3], static_cast<SndID>(0x1D05), 1.0f, 0.0f);
    metalgearSetSeData(&m_seData[4], static_cast<SndID>(0x1D06), 29.0f, 0.0f);
    metalgearSetSeData(&m_seData[5], static_cast<SndID>(0x1D05), 72.0f, 0.0f);
    metalgearSetSeData(&m_seData[6], static_cast<SndID>(0x1D06), 98.0f, 0.0f);
    metalgearSetSeData(&m_seData[7], static_cast<SndID>(0x1D05), 1.0f, 0.0f);
    metalgearSetSeData(&m_seData[8], static_cast<SndID>(0x1D06), 29.0f, 0.0f);
    metalgearSetSeData(&m_seData[9], static_cast<SndID>(0x1D05), 72.0f, 0.0f);
    metalgearSetSeData(&m_seData[10], static_cast<SndID>(0x1D06), 100.0f, 0.0f);
    metalgearSetSeData(&m_seData[11], static_cast<SndID>(0x1D0F), 114.0f, 0.0f);
    metalgearSetSeData(&m_seData[12], static_cast<SndID>(0x1D05), 201.0f, 0.0f);
    metalgearSetSeData(&m_seData[13], static_cast<SndID>(0x1D06), 229.0f, 0.0f);
    metalgearSetSeData(&m_seData[14], static_cast<SndID>(0x1D05), 272.0f, 0.0f);
    metalgearSetSeData(&m_seData[15], static_cast<SndID>(0x1D06), 300.0f, 0.0f);
    metalgearSetSeData(&m_seData[16], static_cast<SndID>(0x1D05), 1.0f, 0.0f);
    metalgearSetSeData(&m_seData[17], static_cast<SndID>(0x1D06), 30.0f, 0.0f);
    metalgearSetSeData(&m_seData[18], static_cast<SndID>(0x1D07), 61.0f, 0.0f);
    metalgearSetSeData(&m_seData[19], static_cast<SndID>(0x1D08), 145.0f, 0.0f);
    metalgearSetSeData(&m_seData[20], static_cast<SndID>(0x1D07), 216.0f, 0.0f);
    metalgearSetSeData(&m_seData[21], static_cast<SndID>(0x1D05), 272.0f, 0.0f);
    metalgearSetSeData(&m_seData[22], static_cast<SndID>(0x1D06), 300.0f, 0.0f);
    metalgearSetSeData(&m_seData[23], static_cast<SndID>(0x1D05), 1.0f, 0.0f);
    metalgearSetSeData(&m_seData[24], static_cast<SndID>(0x1D06), 30.0f, 0.0f);
    metalgearSetSeData(&m_seData[25], static_cast<SndID>(0x1D0A), 34.0f, 0.0f);
    metalgearSetSeData(&m_seData[26], static_cast<SndID>(0x1D09), 43.0f, 0.0f);
    metalgearSetSeData(&m_seData[27], static_cast<SndID>(0x1D0B), 76.0f, 0.0f);
    metalgearSetSeData(&m_seData[28], static_cast<SndID>(0x1D0A), 87.0f, 0.0f);
    metalgearSetSeData(&m_seData[29], static_cast<SndID>(0x1D0C), 106.0f, 270.0f);
    metalgearSetSeData(&m_seData[30], static_cast<SndID>(0x1D09), 275.0f, 0.0f);
    metalgearSetSeData(&m_seData[31], static_cast<SndID>(0x1D05), 301.0f, 0.0f);
    metalgearSetSeData(&m_seData[32], static_cast<SndID>(0x1D06), 330.0f, 0.0f);
    m_sePlayer.registId(m_seIds, 10);
    m_sePlayer.registSeq(0, &m_seData[0], 3, Heaps::StageInstance);
    m_sePlayer.registSeq(1, &m_seData[3], 4, Heaps::StageInstance);
    m_sePlayer.registSeq(2, &m_seData[7], 9, Heaps::StageInstance);
    m_sePlayer.registSeq(3, &m_seData[16], 7, Heaps::StageInstance);
    m_sePlayer.registSeq(4, &m_seData[23], 10, Heaps::StageInstance);
}

grMetalgearRex* grMetalgearRex::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grMetalgearRex* ground = new (Heaps::StageInstance) grMetalgearRex(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grMetalgearRex::~grMetalgearRex() {
}

// Rex turns its head and shoots (the same steps as Ray, with the sounds of the railgun).
void grMetalgearRex::updateActive(float deltaFrame) {
    stMetalgearData* data = static_cast<stMetalgearData*>(getStageData());
    if (data != NULL) {
        m_timer = m_timer - deltaFrame;
        if (m_timer < 0.0f) {
            m_timer = 0.0f;
        }
        float frame = getMotionFrame(0);
        switch (m_state) {
        case 0:
            setMotion(6, false, true, NULL);
            setVisibility(0);
            m_pos.m_x = m_posWork->m_x;
            m_pos.m_y = m_posWork->m_y;
            m_pos.m_z = m_posWork->m_z;
            m_state = 1;
            break;
        case 1:
            if (*m_stateWork == 4) {
                setMotion(0, false, true, &m_frameCount);
                frame = 0.0f;
                m_frame = 0.0f;
                setVisibility(1);
                m_seIndex = 0;
                m_sePlayer.playFrame(m_seIndex, getMotionFrame(0), frame);
                m_state = 6;
            }
            break;
        case 6:
            if (m_timer == 0.0f) {
                m_motionEnd = 1;
            }
            break;
        }
        if (frame >= m_frameCount || frame < m_frame) {
            u8 motion = m_motion;
            switch (motion) {
            case 0: {
                setMotion(3, true, false, &m_frameCount);
                frame = 0.0f;
                m_seIndex = 2;
                m_sePlayer.playFrame(m_seIndex, getMotionFrame(0), frame);
                float r = randf();
                m_motionEnd = 0;
                m_timer = data->unk58 + (data->unk5C - data->unk58) * r;
                break;
            }
            case 1:
                m_rot.m_y = m_rot.m_y + 15.0f;
                if (m_rotNext.m_y + (float)m_turns * 15.0f <= m_rot.m_y || 60.0f <= m_rot.m_y) {
                    m_rot.m_y = m_rotNext.m_y + (float)m_turns * 15.0f;
                    if (m_rot.m_y > 60.0f) {
                        m_rot.m_y = 60.0f;
                    }
                    setMotion(3, true, false, &m_frameCount);
                    frame = 0.0f;
                    m_seIndex = 2;
                    m_sePlayer.playFrame(m_seIndex, getMotionFrame(0), frame);
                }
                break;
            case 2:
                m_rot.m_y = m_rot.m_y - 15.0f;
                if (m_rot.m_y <= m_rotNext.m_y - (float)m_turns * 15.0f || m_rot.m_y <= -60.0f) {
                    m_rot.m_y = m_rotNext.m_y - (float)m_turns * 15.0f;
                    if (m_rot.m_y < -60.0f) {
                        m_rot.m_y = -60.0f;
                    }
                    setMotion(3, true, false, &m_frameCount);
                    frame = 0.0f;
                    m_seIndex = 2;
                    m_sePlayer.playFrame(m_seIndex, getMotionFrame(0), frame);
                }
                break;
            case 3:
            case 4:
            case 5:
                if (m_motionEnd == 1) {
                    u8 next;
                    if (data->unk60 <= randf()) {
                        switch (m_motion) {
                        case 4:
                            if (randf() < 0.5f) {
                                next = 5;
                            } else {
                                next = 3;
                            }
                            break;
                        case 3:
                            if (randf() < 0.5f) {
                                next = 4;
                            } else {
                                next = 5;
                            }
                            break;
                        case 5:
                            if (randf() < 0.5f) {
                                next = 3;
                            } else {
                                next = 4;
                            }
                            break;
                        }
                    } else {
                        if (m_rot.m_y < 60.0f) {
                            if (-60.0f < m_rot.m_y) {
                                if (randf() < data->unk64) {
                                    next = 2;
                                } else {
                                    next = 1;
                                }
                            } else {
                                next = 1;
                            }
                        } else {
                            next = 2;
                        }
                        m_rotNext.m_x = m_rot.m_x;
                        m_rotNext.m_y = m_rot.m_y;
                        m_rotNext.m_z = m_rot.m_z;
                        m_turns = (int)(randf() * 2.0f + 2.0f);
                    }
                    setMotion(next, true, true, &m_frameCount);
                    frame = 0.0f;
                    switch (next) {
                    case 3:
                        m_seIndex = 2;
                        break;
                    case 1:
                        m_seIndex = 1;
                        break;
                    case 0:
                        m_seIndex = 0;
                        break;
                    case 2:
                        m_seIndex = 1;
                        break;
                    case 5:
                        m_seIndex = 4;
                        break;
                    case 4:
                        m_seIndex = 3;
                        break;
                    }
                    if (m_seIndex != -1) {
                        m_sePlayer.playFrame(m_seIndex, getMotionFrame(0), 0.0f);
                    }
                    float r = randf();
                    m_motionEnd = 0;
                    m_timer = data->unk58 + (data->unk5C - data->unk58) * r;
                }
                break;
            }
        }
        if (m_seIndex != -1) {
            m_sePlayer.playFrame(m_seIndex, getMotionFrame(0));
        }
        m_frame = frame;
    }
}
