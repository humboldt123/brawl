#include <gf/gf_model.h>
#include <memory.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <types.h>

#include <st_earth/gr_earth.h>
#include <st_earth/gr_earth_anim.h>

grEarthSun* grEarthSun::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grEarthSun* ground = new (Heaps::StageInstance) grEarthSun(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grEarthSun::~grEarthSun() {
}

// The sun: it turns to the fine weather (0xA - 0xC) or to the rain (0xD - 0xF); the animation of each is played, then looped
// until the stage asks for the other weather.
void grEarthSun::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    m_timer = m_timer - deltaFrame;
    if (m_timer < 0.0f) {
        m_timer = 0.0f;
    }
    switch (m_state) {
    case 0:
        m_state = 1;
        break;
    case 0xA:
        setMotion(0, false, &m_timer);
        m_request = 0;
        m_savedFrame = 0.0f;
        m_state = 0xB;
        break;
    case 0xB:
        if (m_timer == 0.0f) {
            if (m_request == 1) {
                m_savedFrame = 0.0f;
                m_state = 0xD;
            } else {
                setMotion(1, true, &m_timer);
                m_state = 0xC;
                m_savedFrame = m_timer;
            }
        }
        break;
    case 0xC:
        if (m_request == 1) {
            gfModelAnimation* modelAnim = *m_modelAnims;
            if (modelAnim != NULL) {
                modelAnim->setLoop(false);
                if (m_savedFrame == modelAnim->getFrame()) {
                    m_state = 0xD;
                }
            }
        }
        break;
    case 0xD:
        setMotion(2, false, &m_timer);
        m_request = 0;
        m_savedFrame = 0.0f;
        m_state = 0xE;
        break;
    case 0xE:
        if (m_timer == 0.0f) {
            if (m_request == 1) {
                m_savedFrame = 0.0f;
                m_state = 0xA;
            } else {
                setMotion(3, true, &m_timer);
                m_state = 0xF;
                m_savedFrame = m_timer;
            }
        }
        break;
    case 0xF:
        if (m_request == 1) {
            gfModelAnimation* modelAnim = *m_modelAnims;
            if (modelAnim != NULL) {
                modelAnim->setLoop(false);
                if (m_savedFrame == modelAnim->getFrame()) {
                    m_state = 0xA;
                }
            }
        }
        break;
    }
}

void grEarthSun::requestToFine() {
    u8 state = m_state;
    if (0xC < state) {
        if (state < 0x10) {
            m_request = 1;
            return;
        }
        return;
    }
    if (state < 2) {
        m_state = 0xA;
        return;
    }
}

void grEarthSun::requestToRain() {
    u8 state = m_state;
    if (9 < state) {
        if (state < 0xD) {
            m_request = 1;
            return;
        }
        return;
    }
    if (state < 2) {
        m_state = 0xD;
        return;
    }
}

// The animations of the sun are bound with all their parts: the one of the index is looked for in the file, in every kind of
// animation.
void grEarthSun::setMotion(u32 animId, bool loop, float* frameCount) {
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
                if (animId < 4) {
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

// The weather has changed far enough when the animation is at 85 percent of the saved frame.
bool grEarthSun::isChangeWeather() {
    bool result;
    if (m_savedFrame == 0.0f) {
        result = true;
    } else if (*m_modelAnims == NULL) {
        result = false;
    } else {
        result = (m_savedFrame * 0.85f <= (*m_modelAnims)->getFrame());
    }
    return result;
}
