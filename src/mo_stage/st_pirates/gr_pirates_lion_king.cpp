#include <gr/collision/gr_collision.h>
#include <gr/gr_calc_world_callback.h>
#include <st_pirates/gr_pirates_lion_king.h>
#include <st_pirates/gr_pirates_util.h>

grPiratesLionKing::grPiratesLionKing(const char* taskName) : grPirates(taskName) {
    unk160 = 0;
    unk161 = 0;
    m_mtxWork = NULL;
    m_eventWork = NULL;
    m_stateWork = NULL;
    m_showFlgWork = NULL;
    unk174 = 0;
    unk178 = NULL;
    unk17C = NULL;
    unk180 = 0x11;
    unk184 = 0.0f;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_numNodeCallbackData = 1;
    callback->initialize(false, Heaps::StageInstance);
    callback->m_nodeCallbackDatas[0].m_flags |= 8;
}

grPiratesLionKing* grPiratesLionKing::create(int mdlIndex, const char* nodeName, const char* taskName) {
    grPiratesLionKing* ground = new (Heaps::StageInstance) grPiratesLionKing(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(nodeName);
    }
    return ground;
}

grPiratesLionKing::~grPiratesLionKing() { }

void grPiratesLionKing::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        unk160 = 0;
        if (m_timer < *m_ctrlFrame) {
            unk160 = 1;
        }
        m_timer = *m_ctrlFrame;
        updateJoint(deltaFrame);
        updateMotion(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

// Finds the joints of the ship and of the lion the first time they exist; after that the joint of the ship counts only while
// the lion is up (steps 5 and 6) and the joint of the lion while it is shown.
void grPiratesLionKing::updateJoint(float deltaFrame) {
    if (unk178 != NULL && unk17C != NULL) {
        int state = *m_stateWork;
        if (state >= 5 && state < 7) {
            unk178->m_0x55_3 = true;
        } else {
            unk178->m_0x55_3 = false;
        }
        switch (*m_stateWork) {
        case 6:
            if (m_showFlgWork != NULL && *m_showFlgWork == 1) {
                if (*static_cast<float*>(unk15C) < 940.0f) {
                    piratesJointDisable(unk17C);
                } else {
                    piratesJointEnable(unk17C);
                }
            }
            break;
        default:
            if (m_showFlgWork != NULL && *m_showFlgWork == 1) {
                piratesJointEnable(unk17C);
            } else {
                piratesJointDisable(unk17C);
            }
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
                    u16 nodeIndex = joint->m_nodeIndex;
                    if (nodeIndex == getNodeIndex(0, "B_RED_SHIP")) {
                        unk178 = joint;
                    } else {
                        u16 nodeIndex2 = joint->m_nodeIndex;
                        if (nodeIndex2 == getNodeIndex(0, "RedRotateN")) {
                            unk17C = joint;
                            joint->m_0x56_7 = true;
                        }
                    }
                    if (unk178 != NULL && unk17C != NULL) {
                        break;
                    }
                }
            }
        }
    }
}

// The lion follows the weather of the stage (like the sea and the sky, with its own steps for the bow of the ship), and it is
// shown when the stage says so.
void grPiratesLionKing::updateMotion(float deltaFrame) {
    unk184 -= deltaFrame;
    if (unk184 < 0.0f) {
        unk184 = 0.0f;
    }
    switch (m_state) {
    case 0:
        setMotion(1, true, true, NULL);
        m_state = 1;
        break;
    case 1:
        if (unk160 == 1) {
            u32 next = 0x11;
            bool loop = true;
            switch (unk180) {
            case 7:
            case 10:
            case 0xd:
                next = 0;
                break;
            case 5:
            case 8:
            case 0xf:
                next = 2;
                break;
            case 6:
            case 9:
            case 0xc:
                next = 3;
                break;
            case 4:
            case 0xb:
            case 0xe:
                next = 1;
                break;
            case 0x10:
                if (--unk161 == 0) {
                    next = 0;
                }
                break;
            }
            u8 event = *m_eventWork;
            if (event != unk174) {
                u8 base = next;
                switch (event) {
                case 1:
                    switch (unk180) {
                    case 0:
                        next = 4;
                        break;
                    case 1:
                        break;
                    case 2:
                        next = 0xb;
                        break;
                    case 3:
                        next = 0xe;
                        break;
                    case 0x10:
                        next = 4;
                        break;
                    }
                    break;
                case 2:
                    switch (unk180) {
                    case 0:
                        next = 5;
                        break;
                    case 1:
                        next = 8;
                        break;
                    case 3:
                        next = 0xf;
                        break;
                    case 0x10:
                        next = 4;
                        break;
                    }
                    break;
                case 3:
                    switch (unk180) {
                    case 0:
                        next = 6;
                        break;
                    case 1:
                        next = 9;
                        break;
                    case 2:
                        next = 0xc;
                        break;
                    case 0x10:
                        next = 4;
                        break;
                    }
                    break;
                case 4:
                    break;
                case 5:
                    switch (unk180) {
                    case 1:
                        next = 7;
                        break;
                    case 2:
                        next = 10;
                        break;
                    case 3:
                        next = 0xd;
                        break;
                    }
                    break;
                case 6:
                    unk161 = 3;
                    next = 0x10;
                    break;
                }
                if (next != base) {
                    loop = false;
                }
                unk174 = *m_eventWork;
            }
            if (m_showFlgWork != NULL) {
                if (*m_showFlgWork == 1) {
                    if (!m_isVisible) {
                        setNodeVisibilityAll(true, 0);
                        setVisibility(1);
                        setEnableCollisionStatus(true);
                        if (unk17C != NULL) {
                            piratesJointEnable(unk17C);
                        }
                    }
                } else if (m_isVisible) {
                    setVisibility(0);
                    setEnableCollisionStatus(false);
                    if (unk17C != NULL) {
                        piratesJointDisable(unk17C);
                    }
                }
            }
            if (next != 0x11) {
                setMotion(next, loop, true, &unk184);
                setMotionFrame(480.0f - *m_ctrlFrame, 0);
            }
        }
        break;
    }
}

// The model follows the first matrix of the scene.
void grPiratesLionKing::updateCallBack(float deltaFrame) {
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
            }
        }
    }
}

void grPiratesLionKing::setMotion(u32 animId, bool loop, bool force, float* frameCount) {
    PIRATES_SET_MOTION_CASES(unk180, 0x11, animId, loop, force, frameCount,
        case 0x10:
            PIRATES_BIND_VIS(0, model, modelAnim)
        default:
            PIRATES_BIND_CHR(animId, model, modelAnim)
            PIRATES_BIND_TEXPAT(animId, model, modelAnim)
            PIRATES_BIND_TEXSRT(animId, model, modelAnim)
            PIRATES_BIND_COLOR(animId, model, modelAnim)
            break;,
        )
}
