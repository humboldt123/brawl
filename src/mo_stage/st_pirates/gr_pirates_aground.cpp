#include <gr/collision/gr_collision.h>
#include <st_pirates/gr_pirates_aground.h>
#include <st_pirates/gr_pirates_util.h>

grPiratesAground::grPiratesAground(const char* taskName) : grPirates(taskName) {
    m_mtxGimmickWork = NULL;
    unk164 = 0;
    unk165 = 0;
    m_eventWork = NULL;
    unk16C = 0;
    m_countAground = NULL;
    unk174 = 3;
    unk178 = 0.0f;
    unk17C = NULL;
    unk180 = NULL;
    unk184 = NULL;
    unk190 = 0;
}

grPiratesAground* grPiratesAground::create(int mdlIndex, const char* nodeName, const char* taskName) {
    grPiratesAground* ground = new (Heaps::StageInstance) grPiratesAground(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(nodeName);
    }
    return ground;
}

grPiratesAground::~grPiratesAground() { }

void grPiratesAground::processAnim() {
    Ground::processAnim();
    if (m_isUpdate && m_mtxGimmickWork != NULL) {
        getNodeMatrix(m_mtxGimmickWork + 17, 0, "iwamoveN");
    }
}

void grPiratesAground::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        unk164 = 0;
        if (m_timer < *m_ctrlFrame) {
            unk164 = 1;
        }
        m_timer = *m_ctrlFrame;
        updateJoint(deltaFrame);
        updateMotion(deltaFrame);
        updateSE(deltaFrame);
    }
}

// Finds the three collision joints of the reef the first time they exist (the shadow, the reef and the moving reef); after
// that the joints of the reef only count while the ship has run aground.
void grPiratesAground::updateJoint(float deltaFrame) {
    if (unk17C != NULL && unk180 != NULL) {
        switch (unk174) {
        case 0:
        case 2:
            unk17C->m_0x55_3 = true;
            unk180->m_0x55_3 = true;
            break;
        default:
            unk17C->m_0x55_3 = false;
            unk180->m_0x55_3 = false;
            break;
        }
    } else {
        grCollision* collision = m_collision;
        if (collision != NULL) {
            u32 jointLen = (u16)collision->m_jointLen;
            for (u32 i = 0; i != jointLen; i++) {
                grCollisionJoint* joint = collision->getJoint(i);
                if (joint == NULL) {
                    break;
                }
                if (joint->m_ground == this && joint->_0x4E == 0) {
                    u16 nodeIndexA = joint->m_nodeIndex;
                    if (nodeIndexA == getNodeIndex(0, "NShadow1")) {
                        joint->m_0x56_7 = true;
                        piratesJointDisable(joint);
                        unk17C = joint;
                    }
                    u16 nodeIndexB = joint->m_nodeIndex;
                    if (nodeIndexB == getNodeIndex(0, "iwa")) {
                        unk180 = joint;
                    }
                    u16 nodeIndexC = joint->m_nodeIndex;
                    if (nodeIndexC == getNodeIndex(0, "iwamoveN")) {
                        joint->m_0x56_7 = true;
                        piratesJointDisable(joint);
                        unk184 = joint;
                    }
                }
                if (unk17C != NULL && unk180 != NULL && unk184 != NULL) {
                    break;
                }
            }
        }
    }
}

// The reef rises (animation 0), stays while the ship is stuck (1) and sinks (2); the collision of the reef is on only
// while it is up.
void grPiratesAground::updateMotion(float deltaFrame) {
    if (m_eventWork == NULL) {
        return;
    }
    unk178 -= deltaFrame;
    if (unk178 < 0.0f) {
        unk178 = 0.0f;
    }
    switch (m_state) {
    case 0:
        setMotion(3, false, true, NULL);
        setVisibility(0);
        setEnableCollisionStatus(false);
        if (m_collision != NULL) {
            m_collision->setDisable();
        }
        m_state = 1;
    case 1:
        if (unk164 == 1) {
            switch (unk174) {
            case 0:
                if (--unk165 == 0) {
                    setMotion(1, true, true, &unk178);
                    setMotionFrame(480.0f - *m_ctrlFrame, 0);
                    unk165 = *m_countAground;
                    unk190 = 0;
                }
                break;
            case 1:
                if (--unk165 == 0) {
                    setMotion(2, false, true, &unk178);
                    setMotionFrame(480.0f - *m_ctrlFrame, 0);
                    unk165 = 1;
                    unk190 = 0;
                }
                break;
            case 2:
                if (--unk165 == 0) {
                    m_state = 0;
                }
                break;
            }
            if (*m_eventWork != unk16C) {
                switch (*m_eventWork) {
                case 9:
                    setMotion(0, false, true, &unk178);
                    setMotionFrame(480.0f - *m_ctrlFrame, 0);
                    unk165 = 3;
                    setVisibility(1);
                    if (!m_isEnableCollisionStatus) {
                        setEnableCollisionStatus(true);
                        if (m_collision != NULL) {
                            m_collision->setEnable();
                        }
                        if (unk17C != NULL) {
                            piratesJointEnable(unk17C);
                        }
                    }
                    unk190 = 0;
                    break;
                }
                unk16C = *m_eventWork;
            }
        } else {
            switch (unk174) {
            case 0:
                if (getMotionFrame(0) >= 1380.0f && unk184 != NULL) {
                    piratesJointEnable(unk184);
                }
                break;
            case 1:
                break;
            case 2:
                if (unk184 != NULL) {
                    piratesJointDisable(unk184);
                }
                if (getMotionFrame(0) >= 210.0f) {
                    setEnableCollisionStatus(false);
                    if (m_collision != NULL) {
                        m_collision->setDisable();
                    }
                } else {
                    if (getMotionFrame(0) >= 120.0f && unk17C != NULL) {
                        piratesJointDisable(unk17C);
                    }
                }
                break;
            }
        }
        break;
    }
}

// The sounds of the reef: the ship scrapes along it, hits it and breaks free.
void grPiratesAground::updateSE(float deltaFrame) {
    Vec3f pos(0.0f, 0.0f, 0.0f);
    m_sndGenerator.setPos(&pos);
    switch (unk174) {
    case 0:
        switch (unk190) {
        case 0:
            if (getMotionFrame(0) < 900.0f) {
                break;
            }
            m_sndGenerator.playSE(snd_se_stage_Pirates_21, 0, 0, -1);
            unk190++;
            break;
        case 1:
            if (getMotionFrame(0) < 1240.0f) {
                break;
            }
            m_sndGenerator.playSE(snd_se_stage_Pirates_22, 0, 0, -1);
            unk190++;
            break;
        }
        break;
    case 1:
        break;
    case 2:
        switch (unk190) {
        case 0:
            m_sndGenerator.playSE(snd_se_stage_Pirates_23, 0, 0, -1);
            unk190++;
            break;
        case 1:
            if (getMotionFrame(0) < 180.0f) {
                break;
            }
            m_sndGenerator.playSE(snd_se_stage_Pirates_24, 0, 0, -1);
            unk190++;
            break;
        }
        break;
    }
}

void grPiratesAground::setMotion(u32 animId, bool loop, bool force, float* frameCount) {
    PIRATES_SET_MOTION(unk174, 3, animId, loop, force, frameCount)
}
