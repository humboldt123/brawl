#include <ef/ef_id.h>
#include <memory.h>
#include <mt/mt_trig.h>
#include <types.h>

#include <st_jungle/gr_jungle.h>
#include <st_jungle/gr_jungle_anim.h>

// (inline: the original has no constructor of its own)
inline grJungleAshiba10::grJungleAshiba10(const char* taskName) : grJungleAshiba(taskName), m_snd() {
    m_stateWork = NULL;
    m_nodeLeft = 0;
    m_nodeRight = 0;
    m_seHandle = -1;
    m_unused = 0;
}

grJungleAshiba10* grJungleAshiba10::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grJungleAshiba10* ground = new (Heaps::StageInstance) grJungleAshiba10(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grJungleAshiba10::~grJungleAshiba10() {
}

void grJungleAshiba10::update(float deltaFrame) {
    grJungleAshiba::update(deltaFrame);
    updateSE();
    updateG3dProcCalcWorld();
    m_hasUpdatedG3dCalcWorld = false;
    if (m_posGimmickWork != NULL) {
        getNodeMatrix(m_mtxGimmickWork, 0, m_nodeLeft);
        getNodeMatrix(&m_mtxGimmickWork[1], 0, m_nodeRight);
    }
}

// The platform is there once the stage has scrolled far enough (4927 frames); the loads on it follow its flags.
void grJungleAshiba10::updateBaseActive() {
    if (m_stateWork != NULL && m_enableWork != NULL) {
        switch (*m_enableWork) {
        case 0:
            if (m_isVisible == 1) {
                setVisibility(0);
            }
            if (m_isEnableCollisionStatus == 1) {
                setEnableCollisionStatus(false);
            }
            setDisableYakumono();
            break;
        case 1:
            if (!m_isVisible) {
                setVisibility(1);
            }
            break;
        }
        if (getMotionFrame(0) > 4927.0f) {
            if (!m_isEnableCollisionStatus) {
                setEnableCollisionStatus(true);
                if (m_collision != NULL) {
                    m_collision->setEnable();
                }
                if (m_stateWork[0] == 0) {
                    m_stateWork[0] = 1;
                }
                if (m_stateWork[1] == 0) {
                    m_stateWork[1] = 1;
                }
                setEnableYakumono();
            }
        } else if (m_isEnableCollisionStatus == 1) {
            setEnableCollisionStatus(false);
            if (m_stateWork[0] == 1) {
                m_stateWork[0] = 0;
            }
            if (m_stateWork[1] == 1) {
                m_stateWork[1] = 0;
            }
            setDisableYakumono();
        }
    }
}

// The sound of the airplane plays once it is far enough in the stage (4670 frames) and near the screen.
void grJungleAshiba10::updateSE() {
    Vec3f pos;
    getNodePosition(&pos, 0, "AirPlaneN");
    m_snd.setPos(&pos);
    if (getMotionFrame(0) >= 4670.0f) {
        if (pos.m_y < m_posLimitWork[1].m_y - 30.0f) {
            if (m_seHandle != -1) {
                m_snd.stopSE(m_seHandle, 0x78);
                m_seHandle = -1;
            }
        } else if (m_seHandle == -1) {
            m_seHandle = m_snd.playSE(static_cast<SndID>(0x1B8F), 0, 0x78, -1);
        }
    } else {
        m_seHandle = -1;
    }
}

bool grJungleAshiba10::setNode() {
    bool result = grGimmick::setNode();
    getNodeIndex(&m_nodeLeft, 0, "L10nimotsuLN");
    getNodeIndex(&m_nodeRight, 0, "L10nimotsuRN");
    return result;
}

void grJungleAshiba10::resetYakumono() {
    u8* state = m_stateWork;
    if (state != NULL) {
        if (state[0] > 1) {
            state[0] = 4;
        }
        if (m_stateWork[1] >= 2) {
            m_stateWork[1] = 4;
        }
    }
}

void grJungleAshiba10::setStateWork(u8* stateWork) {
    m_stateWork = stateWork;
}
