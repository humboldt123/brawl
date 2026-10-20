#include <gr/gr_calc_world_callback.h>
#include <memory.h>
#include <mt/mt_prng.h>
#include <nw4r/math/math_triangular.h>
#include <types.h>

#include <st_mansion/gr_mansion_floor.h>
#include <st_mansion/gr_mansion_util.h>

// Ground::isCollisionStatusOwnerTask (sora_melee): the symbol takes an int pointer (the header declares another type)
extern "C" bool isCollisionStatusOwnerTask__6GroundFP12grCollStatusPi(Ground* ground, grCollStatus* collStatus, int* flags);

// HYPOTHESIS: the flags that tell which kind of owner the collision status has (a fighter) for the floor
static int sOwnerFlags[2] = { 1, 0 };

grMansionFloor* grMansionFloor::create(int modelIndex, const char* nodeName, const char* taskName) {
    grMansionFloor* ground = new (Heaps::StageInstance) grMansionFloor(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(modelIndex);
        ground->setTgtNode(nodeName);
    }
    return ground;
}

grMansionFloor::grMansionFloor(const char* taskName) : grMansion(taskName) {
    unk158 = 0;
    unk159 = 0;
    unk15C = 0.0f;
    unk160 = 0.0f;
    unk164 = 0.0f;
    unk168 = NULL;
    unk16C.m_x = 0.0f;
    unk16C.m_y = 0.0f;
    unk16C.m_z = 0.0f;
    unk178 = NULL;
    unk17C = NULL;
    unk184 = 0.0f;
    unk188 = 0.0f;
    unk18C = 0.0f;
    unk190 = 1;
    unk191 = 0;
    unk194 = 0.0f;
    unk198 = 1;
    unk19C = 0.0f;
    unk1A0 = 0.0f;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->setup(1, 1);
}

grMansionFloor::~grMansionFloor() { }

void grMansionFloor::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateActive(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

// The floor is hidden for a while, shows up and moves around (to the side and up and down) for a while, then goes back.
void grMansionFloor::updateActive(float deltaFrame) {
    float* data = static_cast<float*>(getStageData());
    if (data != NULL) {
        unk154 -= deltaFrame;
        if (unk154 < 0.0f) {
            unk154 = 0.0f;
        }
        unk164 -= deltaFrame;
        if (unk164 < 0.0f) {
            unk164 = 0.0f;
        }
        unk194 -= deltaFrame;
        if (unk194 < 0.0f) {
            unk194 = 0.0f;
        }
        switch (unk150) {
        case 0:
            setMotion(1, false, true, NULL);
            setVisibility(0);
            setEnableCollisionStatus(false);
            if (unk190 == 1) {
                unk154 = data[10];
            } else {
                unk154 = data[11] + (data[12] - data[11]) * randf();
            }
            unk158 = 0;
            unk159 = 0;
            unk15C = 0.0f;
            unk160 = 0.0f;
            unk164 = 0.0f;
            unk184 = 0.0f;
            unk188 = 0.0f;
            unk18C = 0.0f;
            unk191 = 0;
            unk150 = 2;
            break;
        case 2:
            if (unk154 == 0.0f) {
                if (unk190 == 1) {
                    getNodePosition(&unk16C, 0, m_nodeIndex);
                    Vec3f* pos = unk168;
                    pos->m_x = unk16C.m_x;
                    pos->m_y = unk16C.m_y;
                    pos->m_z = unk16C.m_z;
                    unk190 = 0;
                }
                setMotion(0, true, true, &unk1A0);
                setMotionFrame(unk1A0, 0);
                unk19C = unk1A0;
                if (m_motionRatio > 0.0f) {
                    m_motionRatio *= -1.0f;
                }
                unk154 = data[13] + (data[14] - data[13]) * randf();
                unk150 = 7;
            }
            break;
        case 7:
            if (unk154 == 0.0f) {
                setMotion(0, false, true, &unk1A0);
                if (m_motionRatio < 0.0f) {
                    m_motionRatio *= -1.0f;
                }
                unk150 = 8;
            } else {
                switch (unk180) {
                case 4:
                    updateMoveLeft(deltaFrame);
                    break;
                case 5:
                    updateMoveRight(deltaFrame);
                    break;
                }
                updateMoveUpDown(deltaFrame);
                if (getMotionFrame(0) > unk19C) {
                    setMotion(1, false, true, NULL);
                } else {
                    if (!m_isVisible) {
                        setVisibility(1);
                        setEnableCollisionStatus(true);
                    }
                    unk19C = getMotionFrame(0);
                }
            }
            break;
        case 8:
            if (getMotionFrame(0) >= unk1A0) {
                unk150 = 0;
            }
            break;
        }
    }
}

// The left floor drifts to the left and back (the right floor to the right and back): it speeds up for a while, keeps going
// until the end of its time or 15 units from where it started, then goes the other way.
void grMansionFloor::updateMoveLeft(float deltaFrame) {
    float* data = static_cast<float*>(getStageData());
    if (data != NULL) {
        unk15C -= deltaFrame;
        if (unk15C < 0.0f) {
            unk15C = 0.0f;
        }
        switch (unk158) {
        case 0:
            unk158 = 2;
            unk15C = 5.0f;
            break;
        case 2:
            if (unk15C == 0.0f) {
                float random = randf();
                unk158 = 9;
                unk15C = random * 60.0f + 30.0f;
            }
            break;
        case 9:
            if (unk15C == 0.0f || unk168->m_x < unk16C.m_x - 15.0f) {
                float random = randf();
                unk158 = 10;
                unk15C = random * 60.0f + 60.0f;
            } else {
                unk184 -= data[15] * deltaFrame;
                if (unk184 < -data[16]) {
                    unk184 = -data[16];
                }
                unk168->m_x += unk184 * deltaFrame;
            }
            break;
        case 10:
            if (unk15C == 0.0f || unk168->m_x > unk16C.m_x) {
                float random = randf();
                unk158 = 9;
                unk15C = random * 60.0f + 60.0f;
            } else {
                unk184 += data[15] * deltaFrame;
                if (unk184 > data[16]) {
                    unk184 = data[16];
                }
                unk168->m_x += unk184 * deltaFrame;
            }
            break;
        }
    }
}

void grMansionFloor::updateMoveRight(float deltaFrame) {
    float* data = static_cast<float*>(getStageData());
    if (data != NULL) {
        unk15C -= deltaFrame;
        if (unk15C < 0.0f) {
            unk15C = 0.0f;
        }
        switch (unk158) {
        case 0:
            unk158 = 2;
            unk15C = 5.0f;
            break;
        case 2:
            if (unk15C == 0.0f) {
                float random = randf();
                unk158 = 9;
                unk15C = random * 60.0f + 30.0f;
            }
            break;
        case 9:
            if (unk15C == 0.0f || unk168->m_x < unk16C.m_x) {
                float random = randf();
                unk158 = 10;
                unk15C = random * 60.0f + 60.0f;
            } else {
                unk184 -= data[15] * deltaFrame;
                if (unk184 < -data[16]) {
                    unk184 = -data[16];
                }
                unk168->m_x += unk184 * deltaFrame;
            }
            break;
        case 10:
            if (unk15C == 0.0f || unk168->m_x > unk16C.m_x + 15.0f) {
                float random = randf();
                unk158 = 9;
                unk15C = random * 60.0f + 60.0f;
            } else {
                unk184 += data[15] * deltaFrame;
                if (unk184 > data[16]) {
                    unk184 = data[16];
                }
                unk168->m_x += unk184 * deltaFrame;
            }
            break;
        }
    }
}

void grMansionFloor::updateMoveUpDown(float deltaFrame) {
    float* data = static_cast<float*>(getStageData());
    if (data != NULL) {
        unk160 -= deltaFrame;
        if (unk160 < 0.0f) {
            unk160 = 0.0f;
        }
        switch (unk159) {
        case 0:
            unk159 = 2;
            unk160 = 6.5f;
            break;
        case 2:
            if (unk160 == 0.0f) {
                float random = randf();
                unk159 = 0xB;
                unk160 = random * 60.0f + 120.0f;
            }
            break;
        case 0xB:
            if (unk160 == 0.0f || unk168->m_y > unk16C.m_y + 30.0f) {
                float random = randf();
                unk159 = 0xC;
                unk160 = random * 60.0f + 50.0f;
            } else {
                unk188 += data[15] * deltaFrame;
                if (unk188 > data[16]) {
                    unk188 = data[16];
                }
                unk168->m_y += unk188 * deltaFrame;
            }
            break;
        case 0xC:
            if (unk160 == 0.0f || unk168->m_y < unk16C.m_y) {
                float random = randf();
                unk159 = 0xB;
                unk160 = random * 60.0f + 60.0f;
            } else {
                unk188 -= data[15] * deltaFrame;
                if (unk188 < -data[16]) {
                    unk188 = -data[16];
                }
                unk168->m_y += unk188 * deltaFrame;
            }
            break;
        }
    }
}

// While the floor is out its node follows the place the stage keeps (and sinks a bit after a landing).
void grMansionFloor::updateCallBack(float deltaFrame) {
    if (unk150 == 7) {
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
                Vec3f* source = unk168;
                if (source != NULL) {
                    Vec3f pos;
                    pos.m_x = source->m_x;
                    pos.m_y = source->m_y;
                    pos.m_z = source->m_z;
                    float sink = 1.0f - unk164 / 30.0f;
                    if (sink < 0.0f) {
                        sink = 0.0f;
                    }
                    if (sink > 1.0f) {
                        sink = 1.0f;
                    }
                    float wave = nw4r::math::SinIdx(static_cast<u16>(static_cast<int>(sink * 32768.0f)));
                    pos.m_y = pos.m_y - wave * 5.0f;
                    grNodeCallbackData* data = calcWorldCallBack->m_nodeCallbackDatas;
                    data->m_pos.m_x = pos.m_x;
                    data->m_pos.m_y = pos.m_y;
                    data->m_pos.m_z = pos.m_z;
                }
            }
        }
    }
}

void grMansionFloor::setMotion(u32 animId, bool loop, bool force, float* frameCount) {
    MANSION_SET_MOTION_VIS(unk198, animId, loop, force, frameCount)
}

// A fighter landed on the floor: it sinks for a moment, and the next move may make it fall (by chance).
void grMansionFloor::receiveCollMsg_Landing(grCollStatus* collStatus, grCollisionJoint* collisionJoint, bool isFirstContact) {
    float* data = static_cast<float*>(getStageData());
    if (data != NULL && isCollisionStatusOwnerTask__6GroundFP12grCollStatusPi(this, collStatus, sOwnerFlags)) {
        if (unk194 == 0.0f) {
            if (randf() < data[18]) {
                unk191 = 1;
            } else {
                unk191 = 0;
            }
        }
        unk194 = 5.0f;
        if (isFirstContact == 1 && unk164 == 0.0f) {
            unk164 = 30.0f;
        }
    }
}
