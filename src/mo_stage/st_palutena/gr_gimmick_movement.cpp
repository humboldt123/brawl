#include <gr/gr_calc_world_callback.h>
#include <math.h>
#include <memory.h>
#include <mt/mt_matrix.h>
#include <nw4r/g3d/g3d_scnmdlsmpl.h>
#include <nw4r/math/math_triangular.h>

#include <st_palutena/gr_palutena.h>

grGimmickMovement::grGimmickMovement(const char* taskName) : grGimmick(taskName) {
    m_state = 0;
    m_timer = 0.0f;
    m_dist = 0.0f;
}

grGimmickMovement::~grGimmickMovement() {
}

void grGimmickMovement::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateMove(deltaFrame);
        grGimmick::updateCallback(0);
    }
}

void grGimmickMovement::startup(gfArchive* data, u32 unk1, gfSceneRoot::LayerType layerType) {
    grGimmick::startup(data, unk1, layerType);
    m_data = static_cast<grGimmickMovementData*>(getGimmickData());
    makeCalcuCallback(1, Heaps::StageInstance);
    setCalcuCallbackRoot(1);
    Vec3f pos(m_data->m_start.m_x, m_data->m_start.m_y, m_data->m_start.m_z);
    setPos(&pos);
}

// Moves the gimmick from the start to the goal point: either with a speed (state 2: the speed grows by the acceleration) or
// in a given time (state 3: the position follows a sine or a cosine curve); when it arrives (state 4) it rests again.
void grGimmickMovement::updateMove(float deltaFrame) {
    float previous = m_timer;
    m_timer = previous - deltaFrame;
    if (previous - deltaFrame < 0.0f) {
        m_timer = 0.0f;
    }
    switch (m_state) {
    case 2: {
        float step = m_data->m_accel * deltaFrame;
        float moved = m_dist * deltaFrame + step;
        m_dist = m_dist + step;
        Vec3f current = getPos();
        Vec3f diff = m_data->m_goal - current;
        if (diff.m_z + (diff.m_x + diff.m_y) == 0.0f) {
            m_state = 4;
        } else {
            float remaining = 0.0f;
            float lengthSq = diff.m_z * diff.m_z + (diff.m_x * diff.m_x + diff.m_y * diff.m_y);
            if (!((float)fabs(lengthSq) <= 1.17549435e-38f)) {
                remaining = lengthSq * rsqrtf(lengthSq);
            }
            diff.normalize();
            diff.m_x *= moved;
            diff.m_y *= moved;
            diff.m_z *= moved;
            Vec3f base = getPos();
            Vec3f next = base + diff;
            if (diff.m_z + (diff.m_x + diff.m_y) == 0.0f) {
                m_state = 4;
            } else {
                float travelled = 0.0f;
                float travelSq = diff.m_z * diff.m_z + (diff.m_x * diff.m_x + diff.m_y * diff.m_y);
                if (!((float)fabs(travelSq) <= 1.17549435e-38f)) {
                    travelled = travelSq * rsqrtf(travelSq);
                }
                if (travelled <= remaining) {
                    setPos(&next);
                } else {
                    setPos(&m_data->m_goal);
                    m_state = 4;
                }
            }
        }
        break;
    }
    case 3: {
        float rate = 1.0f - m_timer / m_data->m_time;
        if (rate < 0.0f) {
            rate = 0.0f;
        }
        if (1.0f < rate) {
            rate = 1.0f;
        }
        u8 ease = m_data->m_ease;
        if (ease == 1) {
            rate = nw4r::math::SinFIdx(NW4R_MATH_IDX_TO_FIDX(nw4r::math::U16ToF32((u16)(int)(rate * 16384.0f))));
        } else if (ease != 0 && ease < 3) {
            rate = 1.0f - nw4r::math::CosFIdx(NW4R_MATH_IDX_TO_FIDX(nw4r::math::U16ToF32((u16)(int)(rate * 16384.0f))));
        }
        Vec3f diff = m_data->m_goal - m_data->m_start;
        if (diff.m_z + (diff.m_x + diff.m_y) == 0.0f) {
            m_state = 4;
        } else {
            float length = 0.0f;
            float lengthSq = diff.m_z * diff.m_z + (diff.m_x * diff.m_x + diff.m_y * diff.m_y);
            if (!((float)fabs(lengthSq) <= 1.17549435e-38f)) {
                length = lengthSq * rsqrtf(lengthSq);
            }
            diff.normalize();
            float distance = length * rate;
            diff.m_x *= distance;
            diff.m_y *= distance;
            diff.m_z *= distance;
            Vec3f pos = m_data->m_start + diff;
            setPos(&pos);
            if (rate == 1.0f) {
                m_state = 4;
            }
        }
        break;
    }
    case 4:
        m_state = 0;
        m_timer = 0.0f;
        m_dist = 0.0f;
        break;
    }
}

// The position of a node of the model (the origin of the node moved by its world matrix).
void grGimmickMovement::getPosNode(Vec3f* out, u32 mdlIndex, u32 nodeIndex) {
    if (out != NULL) {
        out->m_x = 0.0f;
        out->m_y = 0.0f;
        out->m_z = 0.0f;
        nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[mdlIndex];
        if (scnMdl != NULL) {
            nw4r::math::MTX34 mtx;
            if (scnMdl->GetScnMtxPos(&mtx, nw4r::g3d::ScnObj::MTX_WORLD, nodeIndex)) {
                Vec3f origin(0.0f, 0.0f, 0.0f);
                Vec3f result;
                reinterpret_cast<Matrix*>(&mtx)->mulPos(&origin, &result);
                out->m_x = result.m_x;
                out->m_y = result.m_y;
                out->m_z = result.m_z;
            }
        }
    }
}

// Starts the movement: with a time (state 3) when the data has one, with the speed of the data (state 2) otherwise.
void grGimmickMovement::startMove() {
    grGimmickMovementData* data = m_data;
    if (data->m_time == 0.0f) {
        m_dist = data->m_speed;
        m_timer = 0.0f;
        m_state = 2;
    } else {
        m_dist = 0.0f;
        m_timer = data->m_time;
        m_state = 3;
    }
    grCalcWorldCallBack* calcWorldCallBack = &m_calcWorldCallBack;
    grNodeCallbackData* nodeData = calcWorldCallBack->m_nodeCallbackDatas;
    if (nodeData == NULL) {
        return;
    }
    nodeData->m_pos.m_x = data->m_start.m_x;
    nodeData->m_pos.m_y = data->m_start.m_y;
    nodeData->m_pos.m_z = data->m_start.m_z;
}
