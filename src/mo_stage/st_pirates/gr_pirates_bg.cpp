#include <memory.h>
#include <mt/mt_prng.h>
#include <st_pirates/gr_pirates_bg.h>
#include <st_pirates/gr_pirates_util.h>
#include <st_pirates/st_pirates.h>

grPiratesBg* grPiratesBg::create(int mdlIndex, const char* nodeName, const char* taskName) {
    grPiratesBg* ground = new (Heaps::StageInstance) grPiratesBg(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(nodeName);
    }
    return ground;
}

grPiratesBg::~grPiratesBg() { }

void grPiratesBg::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    unk160 = 0;
    if (m_timer < *m_ctrlFrame) {
        unk160 = 1;
    }
    m_timer = *m_ctrlFrame;
    updateMotion(deltaFrame);
    if (m_mtxGimmickWork != NULL) {
        getNodeMatrix(m_mtxGimmickWork + 0, 0, "SeaN");
        getNodeMatrix(m_mtxGimmickWork + 1, 0, "KaizokuN");
        getNodeMatrix(m_mtxGimmickWork + 2, 0, "RedN");
        getNodeMatrix(m_mtxGimmickWork + 3, 0, "TatsumakiSkyN");
        getNodeMatrix(m_mtxGimmickWork + 4, 0, "TScreenN");
    }
}

// Follows the weather of the stage: the sea and the sky play one of five animations (0 = fine, 1 = clouds come, 2 = clouds,
// 3 = ..., 4 = storm), the next one is chosen when the loop starts again.
void grPiratesBg::updateMotion(float deltaFrame) {
    stPiratesData* data = static_cast<stPiratesData*>(getStageData());
    if (data == NULL) {
        return;
    }
    unk178 -= deltaFrame;
    if (unk178 < 0.0f) {
        unk178 = 0.0f;
    }
    unk180 -= deltaFrame;
    if (unk180 < 0.0f) {
        unk180 = 0.0f;
    }
    switch (m_state) {
    case 0:
        setMotion(0, true, true, &unk180);
        unk178 = data->unk24;
        m_state = 1;
        break;
    case 1:
        if (unk160 == 1) {
            u32 next = 5;
            bool loop = true;
            switch (unk17C) {
            case 0:
                if (unk178 == 0.0f) {
                    if (*m_eventWork == 9) {
                        return;
                    }
                    if (*m_eventWork == 7) {
                        return;
                    }
                    next = 1;
                    loop = false;
                }
                break;
            case 2:
                if (unk178 == 0.0f) {
                    next = 3;
                    loop = false;
                }
                break;
            case 1: {
                next = 2;
                float range;
                PIRATES_RANDF_RANGE(range, data->unk34, data->unk30);
                unk178 = range;
                break;
            }
            case 3: {
                next = 0;
                float range;
                PIRATES_RANDF_RANGE(range, data->unk2C, data->unk28);
                unk178 = range;
                break;
            }
            case 4:
                if (--unk161 == 0) {
                    next = 0;
                    float range;
                    PIRATES_RANDF_RANGE(range, data->unk2C, data->unk28);
                    unk178 = range;
                }
                break;
            }
            u8 event = *m_eventWork;
            if (event != unk174) {
                u8 base = next;
                switch (event) {
                case 6:
                    unk161 = 3;
                    next = 4;
                    break;
                case 7:
                case 9:
                    if (*m_showFlgLionKingWork != 0 && *m_stateLionKingWork != 5) {
                        next = 3;
                    }
                    break;
                default:
                    break;
                }
                if (next != base) {
                    loop = false;
                }
                unk174 = *m_eventWork;
            }
            if (next != 5) {
                setMotion(next, loop, true, &unk180);
                setMotionFrame(480.0f - *m_ctrlFrame, 0);
                switch (unk17C) {
                case 0:
                    *m_showFlgLionKingWork = 0;
                    *m_stateLionKingWork = 7;
                    break;
                case 1:
                    *m_showFlgLionKingWork = 1;
                    *m_stateLionKingWork = 3;
                    break;
                case 2:
                    *m_showFlgLionKingWork = 1;
                    *m_stateLionKingWork = 4;
                    break;
                case 3:
                    *m_showFlgLionKingWork = 1;
                    *m_stateLionKingWork = 5;
                    break;
                case 4: {
                    u8* flag = m_showFlgLionKingWork;
                    if (*flag == 0) {
                        *flag = 0;
                        *m_stateLionKingWork = 7;
                    } else if (*m_stateLionKingWork != 3 && *m_stateLionKingWork != 4) {
                        *flag = 0;
                        *m_stateLionKingWork = 7;
                    } else {
                        *flag = 1;
                        *m_stateLionKingWork = 6;
                    }
                    break;
                }
                }
            }
        }
        break;
    }
}

void grPiratesBg::setMotion(u32 animId, bool loop, bool force, float* frameCount) {
    PIRATES_SET_MOTION(unk17C, 5, animId, loop, force, frameCount)
}
