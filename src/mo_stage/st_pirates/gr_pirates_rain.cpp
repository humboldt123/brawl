#include <st_pirates/gr_pirates_rain.h>
#include <st_pirates/gr_pirates_util.h>

inline grPiratesRain::grPiratesRain(const char* taskName) : grPirates(taskName) {
    m_state = 0;
    m_timer = 0.0f;
    m_eventWork = NULL;
    unk164 = 3;
    unk168 = 0.0f;
    unk16C = 0.0f;
    unk178 = -1;
}

grPiratesRain* grPiratesRain::create(int mdlIndex, const char* nodeName, const char* taskName) {
    grPiratesRain* ground = new (Heaps::StageInstance) grPiratesRain(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(nodeName);
    }
    return ground;
}

grPiratesRain::~grPiratesRain() { }

void grPiratesRain::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateActive(deltaFrame);
    }
}

// The rain falls while the event 6 (rain) is on: it starts, loops and ends with the sound of the rain. The animations: 0
// starts, 1 loops, 2 ends and 3 is none.
void grPiratesRain::updateActive(float deltaFrame) {
    void* data = getStageData();
    if (data != NULL) {
        m_timer -= deltaFrame;
        if (m_timer < 0.0f) {
            m_timer = 0.0f;
        }
        switch (m_state) {
        case 0:
            setMotion(3, false, true, NULL);
            setVisibility(0);
            m_state = 1;
            break;
        case 1:
            if (*m_eventWork == 6) {
                setMotion(0, false, true, &unk16C);
                setVisibility(1);
                unk178 = m_sndGenerator.playSE(snd_se_stage_Pirates_03, 0, (int)unk16C, -1);
                m_state = 3;
            }
            break;
        case 3:
            if (getMotionFrame(0) >= unk16C) {
                setMotion(1, false, true, &unk16C);
                m_state = 6;
            }
            break;
        case 6:
            if (getMotionFrame(0) >= unk16C) {
                setMotion(2, false, true, &unk16C);
                if (unk178 != -1) {
                    m_sndGenerator.stopSE(unk178, (int)unk16C);
                }
                unk178 = -1;
                m_timer = 300.0f;
                m_state = 7;
            }
            break;
        case 7:
            if (m_timer == 0.0f) {
                setMotion(0, false, true, &unk16C);
                setVisibility(1);
                unk178 = m_sndGenerator.playSE(snd_se_stage_Pirates_03, 0, (int)unk16C, -1);
                m_state = 9;
            }
            break;
        case 9:
            if (getMotionFrame(0) >= unk16C) {
                setMotion(1, true, true, &unk16C);
                m_state = 4;
                unk168 = 0.0f;
            }
            break;
        case 4:
            if (*m_eventWork == 6 || getMotionFrame(0) >= unk168) {
                unk168 = getMotionFrame(0);
            } else {
                setMotion(2, false, true, &unk16C);
                if (unk178 != -1) {
                    m_sndGenerator.stopSE(unk178, (int)unk16C);
                }
                unk178 = -1;
                m_state = 10;
            }
            break;
        case 10:
            if (getMotionFrame(0) >= unk16C) {
                m_state = 0;
            }
            break;
        }
    }
}

void grPiratesRain::setMotion(u32 animId, bool loop, bool force, float* frameCount) {
    PIRATES_SET_MOTION(unk164, 3, animId, loop, force, frameCount)
}
