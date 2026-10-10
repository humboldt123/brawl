#include <gf/gf_model.h>
#include <memory.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <nw4r/math/math_triangular.h>
#include <snd/snd_system.h>
#include <types.h>

#include <st_earth/gr_earth.h>
#include <st_earth/gr_earth_anim.h>

grEarthRiver* grEarthRiver::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grEarthRiver* ground = new (Heaps::StageInstance) grEarthRiver(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grEarthRiver::~grEarthRiver() {
}

// The river: it starts (2) with the animation of the water and the sound, the speed of it goes up and down with the frames of
// the animation (3), and it loops (4, 5) until the stage says it ends (6, 7).
void grEarthRiver::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    stEarthData* data = static_cast<stEarthData*>(getStageData());
    if (data != NULL) {
        m_timerRiver = m_timerRiver - deltaFrame;
        if (m_timerRiver < 0.0f) {
            m_timerRiver = 0.0f;
        }
        switch (m_riverState) {
        case 0:
            setMotion(3, false, NULL);
            setVisibility(0);
            m_riverState = 1;
            break;
        case 2: {
            setMotion(0, false, &m_frameCount);
            setVisibility(1);
            Vec3f pos;
            pos.m_x = -30.0f;
            pos.m_y = 0.0f;
            pos.m_z = 0.0f;
            m_snd.setPos(&pos);
            m_seHandle = m_snd.playSE(static_cast<SndID>(0x1CDA), 0, 0, -1);
            m_riverState = 3;
            break;
        }
        case 3:
            if (getMotionFrame(0) < 330.0f) {
                if (getMotionFrame(0) < 300.0f) {
                    if (getMotionFrame(0) < 100.0f) {
                        *m_speedWork = 0.0f;
                    } else {
                        float rate = (getMotionFrame(0) - 100.0f) / 200.0f;
                        if (rate < 0.0f) {
                            rate = 0.0f;
                        }
                        if (1.0f < rate) {
                            rate = 1.0f;
                        }
                        float sine = nw4r::math::SinFIdx(static_cast<float>(static_cast<s16>(static_cast<int>(rate * 16384.0f))) * (1.0f / 256.0f));
                        *m_speedWork = (data->unkCC - 0.5f) * sine + 0.5f;
                    }
                } else {
                    float rate = (getMotionFrame(0) - 300.0f) / 30.0f;
                    if (rate < 0.0f) {
                        rate = 0.0f;
                    }
                    if (1.0f < rate) {
                        rate = 1.0f;
                    }
                    float sine = nw4r::math::SinFIdx(static_cast<float>(static_cast<s16>(static_cast<int>(rate * 16384.0f))) * (1.0f / 256.0f));
                    *m_speedWork = (data->unkCC - 0.5f) * sine + 0.5f;
                }
            } else {
                *m_speedWork = data->unkCC;
            }
            if (m_frameCount <= getMotionFrame(0)) {
                if (m_endRequest == 1) {
                    m_riverState = 6;
                } else {
                    m_riverState = 4;
                }
            }
            if (m_seHandle == -1 || !fn_800797A4(&m_snd, m_seHandle)) {
                if (m_seHandle != -1) {
                    m_snd.stopSE(m_seHandle, 0x3C);
                }
                Vec3f pos;
                pos.m_x = -30.0f;
                pos.m_y = 0.0f;
                pos.m_z = 0.0f;
                m_snd.setPos(&pos);
                m_seHandle = m_snd.playSE(static_cast<SndID>(0x1CDB), 0, 0x3C, -1);
            }
            break;
        case 4:
            setMotion(1, true, &m_frameCount);
            m_riverState = 5;
            m_framePrev = 0.0f;
            break;
        case 5:
            if (m_endRequest != 1 || m_framePrev <= getMotionFrame(0)) {
                m_framePrev = getMotionFrame(0);
            } else {
                m_riverState = 6;
            }
            break;
        case 6: {
            setMotion(2, false, &m_frameCount);
            if (m_seHandle != -1) {
                m_snd.stopSE(m_seHandle, 0x3C);
            }
            Vec3f pos;
            pos.m_x = -30.0f;
            pos.m_y = 0.0f;
            pos.m_z = 0.0f;
            m_snd.setPos(&pos);
            m_snd.playSE(static_cast<SndID>(0x1CDC), 0, 0x3C, -1);
            m_endRequest = 0;
            m_riverState = 7;
            break;
        }
        case 7:
            if (getMotionFrame(0) < 320.0f) {
                if (getMotionFrame(0) < 150.0f) {
                    if (getMotionFrame(0) < 80.0f) {
                        *m_speedWork = 2.0f;
                    } else {
                        float rate = (getMotionFrame(0) - 80.0f) / 70.0f;
                        if (rate < 0.0f) {
                            rate = 0.0f;
                        }
                        if (1.0f < rate) {
                            rate = 1.0f;
                        }
                        *m_speedWork = 2.0f - rate * 1.5f;
                    }
                } else {
                    float rate = (getMotionFrame(0) - 150.0f) / 170.0f;
                    if (rate < 0.0f) {
                        rate = 0.0f;
                    }
                    if (1.0f < rate) {
                        rate = 1.0f;
                    }
                    *m_speedWork = 0.5f - rate * 0.5f;
                }
            } else {
                *m_speedWork = 0.0f;
            }
            if (m_frameCount <= getMotionFrame(0)) {
                m_riverState = 0;
            }
            break;
        }
    }
}

void grEarthRiver::startRiver() {
    u8 state = m_riverState;
    if (state < 6) {
        if (state != 1) {
            return;
        }
    } else if (7 < state) {
        return;
    }
    m_riverState = 2;
}

void grEarthRiver::endRiver() {
    if (5 < m_riverState) {
        return;
    }
    if (m_riverState < 2) {
        return;
    }
    m_endRequest = 1;
}

// The animations of the river are bound with all their parts: the one of the index is looked for in the file, in every kind of
// animation.
void grEarthRiver::setMotion(u32 animId, bool loop, float* frameCount) {
    nw4r::g3d::ScnMdl* sceneMdl = *m_sceneModels;
    if (sceneMdl != NULL) {
        gfModelAnimation* modelAnim = *m_modelAnims;
        if (modelAnim != NULL) {
            nw4r::g3d::ResMdl model = sceneMdl->m_resMdl;
            if (model.IsValid()) {
                modelAnim->unbindNodeAnim(sceneMdl);
                modelAnim->unbindVisibleAnim(sceneMdl);
                modelAnim->unbindTexAnim(sceneMdl);
                modelAnim->unbindTexSrtAnim(sceneMdl);
                modelAnim->unbindMatColAnim(sceneMdl);
                if (animId < 3) {
                    GR_EARTH_BIND_ANIMS(animId, model, modelAnim)
                    gfModelAnimation::bind(sceneMdl, modelAnim);
                    modelAnim->setFrame(0.0);
                    modelAnim->setUpdateRate(1.0);
                    modelAnim->setLoop(loop);
                    if (frameCount != NULL) {
                        *frameCount = modelAnim->getFrameCount();
                    }
                }
            }
        }
    }
}
