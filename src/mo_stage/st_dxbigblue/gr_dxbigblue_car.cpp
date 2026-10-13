#define MT_VEC3F_BIKE_UTILITY_OPERATORS // the stage uses the operators with the register order of the native Utility code
#include <cm/cm_camera_controller.h>
#include <st_dxbigblue/gr_dxbigblue_car.h>
#include <st_dxbigblue/gr_dxbigblue_util.h>
#include <st_dxbigblue/st_dxbigblue_data.h>

grDxBigBlueCar* grDxBigBlueCar::create(int mdlIndex, const char* nodeName, const char* taskName) {
    grDxBigBlueCar* ground = new (Heaps::StageInstance) grDxBigBlueCar(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(nodeName);
    }
    return ground;
}

grDxBigBlueCar::grDxBigBlueCar(const char* taskName) : grDxBigBlue(taskName) {
    m_posLimitWork = NULL;
    m_posBgWork = NULL;
    m_speed = 0.0f;
    unk164 = 0;
    unk168 = 0.0f;
    m_index = 0;
    m_indexTopWork = NULL;
    m_tblWork = NULL;
    m_carDataWork = NULL;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_numNodeCallbackData = 1;
    callback->initialize(false, Heaps::StageInstance);
    callback->m_nodeCallbackDatas[0].m_flags |= 1;
    callback->m_nodeCallbackDatas[0].m_flags |= 2;
}

grDxBigBlueCar::~grDxBigBlueCar() { }

void grDxBigBlueCar::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateActive(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

// The car goes through its steps in the line: it waits (not shown), comes in (picks a place in line), stays, leaves and is
// free again. The state of the car (the record) says which of the steps it is in.
void grDxBigBlueCar::updateActive(float deltaFrame) {
    if (m_carDataWork == NULL) {
        return;
    }
    stDxBigBlueData* data = static_cast<stDxBigBlueData*>(getStageData());
    if (data == NULL) {
        return;
    }
    bool drives = false;
    u8 carIndex = m_tblWork[m_index];
    stDxBigBlueCarData* car = &m_carDataWork[carIndex];
    m_timer -= deltaFrame;
    if (m_timer < 0.0f) {
        m_timer = 0.0f;
    }
    car->unk34 -= deltaFrame;
    if (car->unk34 < 0.0f) {
        car->unk34 = 0.0f;
    }
    switch (m_state) {
        case 0:
            setVisibility(0);
            setEnableCollisionStatus(false);
            unk164 = 0;
            m_speed = 0.0f;
            unk168 = 0.0f;
            m_state = 1;
        case 1:
            if (car->m_state != 10) {
                m_timer = data->unk5C + (data->unk60 - data->unk5C) * randf();
                m_state = 2;
            }
            break;
        case 2:
            if (!m_isVisible) {
                setVisibility(1);
                setEnableCollisionStatus(true);
            }
            switch (car->m_state) {
                case 4:
                    updateActiveIn(deltaFrame);
                    break;
                case 5:
                    updateActiveStay(deltaFrame);
                    if (car->m_state == 6) {
                        m_state = 3;
                    }
                    break;
                case 7:
                    updateActiveOut(deltaFrame);
                    break;
            }
            drives = true;
            break;
        case 3:
            switch (car->m_state) {
                case 5:
                case 7:
                    m_timer = data->unk5C + (data->unk60 - data->unk5C) * randf();
                    m_state = 2;
                    break;
            }
            drives = true;
            break;
        case 6:
            if (car->m_state == 9) {
                setVisibility(0);
                setEnableCollisionStatus(false);
                car->m_state = 10;
                car->unk04 = 0.0f;
                car->unk08 = 0.0f;
                car->unk0C = 0.0f;
                car->unk10 = 0.0f;
                car->unk14 = 0.0f;
                car->unk18 = 0.0f;
                car->unk1C = 0.0f;
                car->unk20 = 0.0f;
                car->unk24 = 0.0f;
                car->unk28 = 0.0f;
                car->unk2C = 0.0f;
                car->unk30 = 0;
                car->unk34 = 0.0f;
                car->unk38 = 0;
                m_state = 0;
            }
            break;
    }
    if (drives == 1) {
        if (unk164 == 1) {
            unk168 += data->unk48 * deltaFrame;
            if (unk168 > data->unk4C) {
                unk168 = data->unk4C;
                unk164 = 0;
            }
        } else {
            unk168 -= data->unk48 * deltaFrame;
            if (unk168 < 0.0f) {
                unk168 = 0.0f;
            }
        }
    }
}

// The car comes in from the side (the speed goes up to the top speed) until it is in line with the car in front of it.
void grDxBigBlueCar::updateActiveIn(float deltaFrame) {
    stDxBigBlueData* data = static_cast<stDxBigBlueData*>(getStageData());
    if (data != NULL) {
        bool arrived = false;
        stDxBigBlueCarData* car = &m_carDataWork[m_tblWork[m_index]];
        CameraController* camera = CameraController::getInstance();
        Rect2D* range = reinterpret_cast<Rect2D*>(reinterpret_cast<u8*>(camera) + 0x148);
        Vec3f lo(range->m_left, range->m_up, 0.0f);
        Vec3f hi(range->m_right, range->m_down, 0.0f);
        if (0.0f == m_timer) {
            arrived = true;
        } else if (car->unk38 == 1) {
            m_speed += data->unk28 * deltaFrame;
            if (m_speed > data->unk24) {
                m_speed = data->unk24;
            }
            car->unk04 += m_speed * deltaFrame;
            u32 next = m_index + 1;
            if (next == 30) {
                next = 0;
            }
            if (isActive(next) == 1 && !(car->unk04 < lo.m_x)) {
                if (m_carDataWork[m_tblWork[next]].unk04 - car->unk04 < data->unk20) {
                    arrived = true;
                }
            }
        } else {
            m_speed -= data->unk28 * deltaFrame;
            if (m_speed < -data->unk24) {
                m_speed = -data->unk24;
            }
            u32 prev = 29;
            car->unk04 += m_speed * deltaFrame;
            if (m_index != 0) {
                prev = m_index - 1;
            }
            if (isActive(prev) == 1 && !(car->unk04 > hi.m_x)) {
                if (car->unk04 - m_carDataWork[m_tblWork[prev]].unk04 < data->unk20) {
                    arrived = true;
                }
            }
        }
        if (arrived == 1) {
            m_timer = data->unk5C + (data->unk60 - data->unk5C) * randf();
            car->m_state = 5;
        }
    }
}

// The car stays on screen; it drives slowly and keeps its distance to the cars next to it, until the wait is over.
void grDxBigBlueCar::updateActiveStay(float deltaFrame) {
    stDxBigBlueData* data = static_cast<stDxBigBlueData*>(getStageData());
    if (data == NULL) {
        return;
    }
    u32 index = m_index;
    stDxBigBlueCarData* car = &m_carDataWork[m_tblWork[index]];
    if (0.0f == m_timer) {
        if (car->unk04 > 0.0f) {
            u32 next = index + 1;
            if (next == 30) {
                next = 0;
            }
            if (isActive(next) == 1) {
                m_timer = data->unk5C + (data->unk60 - data->unk5C) * randf();
            } else {
                car->unk38 = 1;
                car->m_state = 6;
            }
        } else if (car->unk04 < 0.0f) {
            u32 prev = 29;
            if (index != 0) {
                prev = index - 1;
            }
            if (isActive(prev) == 1) {
                m_timer = data->unk5C + (data->unk60 - data->unk5C) * randf();
            } else {
                car->unk38 = 0;
                car->m_state = 6;
            }
        } else {
            m_timer = data->unk5C + (data->unk60 - data->unk5C) * randf();
        }
    } else if (car->unk04 > m_posLimitWork[3]) {
        car->m_state = 6;
        car->unk38 = 1;
    } else if (car->unk04 < m_posLimitWork[0]) {
        car->m_state = 6;
        car->unk38 = 0;
    } else {
        CameraController* camera = CameraController::getInstance();
        u32 top = *m_indexTopWork;
        Rect2D* range = reinterpret_cast<Rect2D*>(reinterpret_cast<u8*>(camera) + 0x148);
        Vec3f lo(range->m_left, range->m_up, 0.0f);
        Vec3f hi(range->m_right, range->m_down, 0.0f);
        u32 slot0 = top;
        if (top >= 30) {
            slot0 = top - 30;
        }
        u32 slot1 = top + 1;
        if (slot1 >= 30) {
            slot1 -= 30;
        }
        u32 slot2 = top + 2;
        if (slot2 >= 30) {
            slot2 -= 30;
        }
        u32 slot3 = top + 3;
        if (slot3 >= 30) {
            slot3 -= 30;
        }
        bool inFront = false;
        index = m_index;
        float step = (hi.m_x - lo.m_x) / 12.0f;
        if (index == slot0) {
            if (car->unk04 < lo.m_x + step * 3.0f) {
                inFront = true;
            } else if (car->unk04 > lo.m_x + step * 3.0f) {
                inFront = false;
            }
        } else if (index == slot1) {
            if (car->unk04 < lo.m_x + step * 5.0f) {
                inFront = true;
            } else if (car->unk04 > lo.m_x + step * 5.0f) {
                inFront = false;
            }
        } else if (index == slot2) {
            if (car->unk04 < lo.m_x + step * 7.0f) {
                inFront = true;
            } else if (car->unk04 > lo.m_x + step * 7.0f) {
                inFront = false;
            }
        } else if (index == slot3) {
            if (car->unk04 < lo.m_x + step * 9.0f) {
                inFront = true;
            } else if (car->unk04 > lo.m_x + step * 9.0f) {
                inFront = false;
            }
        }
        if (!inFront) {
            m_speed -= deltaFrame * (data->unk28 * 0.25f);
            float bottom = -data->unk24 * 0.25f;
            if (m_speed < bottom) {
                m_speed = bottom;
            }
            car->unk04 += m_speed * deltaFrame;
            car->unk38 = inFront;
        } else if (inFront == 1) {
            m_speed += deltaFrame * (data->unk28 * 0.25f);
            float limit = data->unk24 * 0.25f;
            if (m_speed > limit) {
                m_speed = limit;
            }
            car->unk04 += m_speed * deltaFrame;
            car->unk38 = inFront;
        }
    }
}

// The car leaves to the side it is driving to; it is gone when it is out of the camera.
void grDxBigBlueCar::updateActiveOut(float deltaFrame) {
    stDxBigBlueData* data = static_cast<stDxBigBlueData*>(getStageData());
    if (data != NULL) {
        stDxBigBlueCarData* car = &m_carDataWork[m_tblWork[m_index]];
        CameraController* camera = CameraController::getInstance();
        Rect2D* range = reinterpret_cast<Rect2D*>(reinterpret_cast<u8*>(camera) + 0x148);
        Vec3f lo(range->m_left, range->m_up, 0.0f);
        Vec3f hi(range->m_right, range->m_down, 0.0f);
        if (car->unk38 == 1) {
            m_speed += data->unk28 * deltaFrame;
            if (m_speed > data->unk24) {
                m_speed = data->unk24;
            }
            car->unk04 += m_speed * deltaFrame;
            u32 next = m_index + 1;
            if (next == 30) {
                next = 0;
            }
            if (isActive(next) == 1) {
                m_timer = data->unk5C + (data->unk60 - data->unk5C) * randf();
                car->m_state = 5;
            }
        } else {
            m_speed -= data->unk28 * deltaFrame;
            if (m_speed < -data->unk24) {
                m_speed = -data->unk24;
            }
            u32 prev = 29;
            car->unk04 += m_speed * deltaFrame;
            if (m_index != 0) {
                prev = m_index - 1;
            }
            if (isActive(prev) == 1) {
                m_timer = data->unk5C + (data->unk60 - data->unk5C) * randf();
                car->m_state = 5;
            }
        }
        if (car->unk04 >= m_posLimitWork[3] + data->unk68 || car->unk04 <= m_posLimitWork[0] - data->unk68) {
            car->m_state = 8;
            setVisibility(0);
            m_state = 6;
        }
    }
}

void grDxBigBlueCar::updateCallBack(float deltaFrame) {
    if (m_carDataWork != NULL) {
        grCalcWorldCallBack* calcWorldCallBack = &m_calcWorldCallBack;
        if (calcWorldCallBack != NULL) {
            nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[0];
            if (scnMdl != NULL) {
                DXBIGBLUE_BIND_CALLBACK(calcWorldCallBack, scnMdl)
                stDxBigBlueData* data = static_cast<stDxBigBlueData*>(getStageData());
                if (data != NULL) {
                    float sink = unk168;
                    stDxBigBlueCarData* car = &m_carDataWork[m_tblWork[m_index]];
                    Vec3f pos(car->unk04, car->unk08, car->unk0C);
                    if (sink > data->unk4C) {
                        pos.m_y = car->unk08 - data->unk4C;
                    } else {
                        pos.m_y = car->unk08 - sink;
                    }
                    grNodeCallbackData* node = &calcWorldCallBack->m_nodeCallbackDatas[0];
                    node->m_pos = pos;
                    node = &calcWorldCallBack->m_nodeCallbackDatas[0];
                    Vec3f rot;
                    float turn = car->unk24 - node->m_rot.m_z;
                    float rate;
                    rot = node->m_rot;
                    if (car->unk30 == 1) {
                        rate = data->unk54;
                    } else {
                        rate = data->unk50;
                    }
                    rot.m_z = rot.m_z + turn * (rate * deltaFrame);
                    node->m_rot = rot;
                }
            }
        }
    }
}

// Is the car of this place in the line on screen (coming in, staying or leaving)?
bool grDxBigBlueCar::isActive(int index) {
    if (m_carDataWork == NULL) {
        return false;
    }
    if (m_tblWork == NULL) {
        return false;
    }
    switch (m_carDataWork[m_tblWork[index]].m_state) {
        case 4:
        case 5:
        case 6:
        case 7:
            return true;
    }
    return false;
}

// A fighter lands on the car: it counts as ridden for five frames, and it sinks when it is the first touch.
void grDxBigBlueCar::receiveCollMsg_Landing(grCollStatus* collStatus, grCollisionJoint* collisionJoint, bool isFirstContact) {
    // HYPOTHESIS: the flags that tell which kind of owner the collision status has (a fighter)
    static int sOwnerFlags[2] = { 1, 0 };
    if (isCollisionStatusOwnerTask__6GroundFP12grCollStatusPi(this, collStatus, sOwnerFlags)) {
        m_carDataWork[m_tblWork[m_index]].unk34 = 5.0f;
        if (isFirstContact == 1) {
            unk164 = 1;
        }
    }
}
