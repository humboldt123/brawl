#include <gr/gr_calc_world_callback.h>
#include <math.h>
#include <mt/mt_prng.h>
#include <mt/mt_trig.h>
#include <nw4r/math/math_triangular.h>
#include <st_pirates/gr_pirates_cannon.h>
#include <st_pirates/gr_pirates_util.h>
#include <st_pirates/st_pirates.h>

static inline bool piratesIsNearZero(float value) {
    bool result = false;
    if ((float)fabs(value) < 1e-5f) {
        result = true;
    }
    return result;
}

grPiratesCannon::grPiratesCannon(const char* taskName) : grPirates(taskName) {
    m_id = 0;
    m_mtxWork = NULL;
    m_eventWork = NULL;
    m_fireWork = NULL;
    m_idActiveWork = NULL;
    m_posTgtWork = NULL;
    unk178.m_x = 0.0f;
    unk178.m_y = 0.0f;
    unk178.m_z = 0.0f;
    unk184 = 0.0f;
    unk188 = 0.0f;
    unk18C = 0.0f;
    unk190.m_x = 0.0f;
    unk190.m_y = 0.0f;
    unk190.m_z = 0.0f;
    unk19C = 0.0f;
    unk1A0 = 0.0f;
    unk1A4 = 0.0f;
    unk1A8 = 0;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_numNodeCallbackData = 1;
    callback->initialize(false, Heaps::StageInstance);
    callback->m_nodeCallbackDatas[0].m_flags |= 8;
}

grPiratesCannon* grPiratesCannon::create(int mdlIndex, const char* nodeName, const char* taskName) {
    grPiratesCannon* ground = new (Heaps::StageInstance) grPiratesCannon(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(nodeName);
    }
    return ground;
}

grPiratesCannon::~grPiratesCannon() { }

void grPiratesCannon::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateCannon(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

// The cannons are shown while the bombs rain. The cannon that fires turns to the target of the bomb, the others turn at
// random; the target is taken when the shot starts.
void grPiratesCannon::updateCannon(float deltaFrame) {
    stPiratesData* data = static_cast<stPiratesData*>(getStageData());
    if (data != NULL) {
        m_timer -= deltaFrame;
        if (m_timer < 0.0f) {
            m_timer = 0.0f;
        }
        switch (*m_eventWork) {
        case 7: {
            setVisibility(1);
            switch (*m_fireWork) {
            case 0:
                if (unk1A8 != 1) {
                    Vec3f pos;
                    Vec3f rot;
                    u32 same = 0;
                    if (piratesIsNearZero(unk190.m_x) && piratesIsNearZero(unk190.m_y) && piratesIsNearZero(unk190.m_z)) {
                        same = 1;
                    }
                    if (same == 1) {
                        Matrix* mtx = m_mtxWork;
                        float posZ = mtx->m[2][3];
                        float posY = mtx->m[1][3];
                        float posX = mtx->m[0][3];
                        pos.m_x = posX;
                        pos.m_y = posY;
                        pos.m_z = posZ;
                        mtx->getRotate(&rot);
                        float sine;
                        float cosine;
                        mtSinCosf(rot.m_y, &sine, &cosine);
                        unk178.m_y = pos.m_y;
                        float x = pos.m_x + sine * 500.0f;
                        unk178.m_x = x;
                        unk190.m_x = x;
                        unk190.m_y = pos.m_y;
                        float z = pos.m_z + cosine * 500.0f;
                        unk178.m_z = z;
                        unk190.m_z = z;
                    } else {
                        float* target = m_posTgtWork;
                        unk190.m_x = target[0];
                        unk190.m_y = target[1];
                        unk190.m_z = target[2];
                    }
                    float* target = m_posTgtWork;
                    unk178.m_x = target[0];
                    unk178.m_y = target[1];
                    unk178.m_z = target[2];
                    m_mtxWork->getRotate(&rot);
                    unk184 = 0.0f;
                    float rnd = randf();
                    unk18C = 0.0f;
                    unk188 = (-12.0f + 24.0f * rnd) * 0.017453292f;
                    m_timer = data->unk6C;
                    unk1A8 = 1;
                }
                break;
            default:
                unk1A8 = 0;
                break;
            }
            float rate = 1.0f - m_timer / data->unk6C;
            if (rate < 0.0f) {
                rate = 0.0f;
            }
            if (rate > 1.0f) {
                rate = 1.0f;
            }
            float sine = nw4r::math::SinFIdx(NW4R_MATH_IDX_TO_FIDX(nw4r::math::U16ToF32((u16)(int)(rate * 16384.0f))));
            if (m_id == *m_idActiveWork) {
                Vec3f dir;
                Vec3f pos;
                Vec3f cur;
                Vec3f rot;
                Matrix* mtx = m_mtxWork;
                float curX = unk190.m_x + sine * (unk178.m_x - unk190.m_x);
                float posZ = mtx->m[2][3];
                float curZ = unk190.m_z + sine * (unk178.m_z - unk190.m_z);
                float posY = mtx->m[1][3];
                float curY = unk190.m_y + sine * (unk178.m_y - unk190.m_y);
                float posX = mtx->m[0][3];
                cur.m_x = curX;
                cur.m_y = curY;
                cur.m_z = curZ;
                pos.m_x = posX;
                pos.m_y = posY;
                pos.m_z = posZ;
                dir = cur - pos;
                dir.normalize();
                m_mtxWork->getRotate(&rot);
                float angle = nw4r::math::Atan2FIdx(dir.m_x, dir.m_z) * 1.40625f;
                unk1A0 += ((angle - rot.m_y * 57.29578f) - unk1A0) * 0.012f;
            } else {
                unk1A0 += (unk188 * 57.29578f - unk1A0) * 0.012f;
            }
            break;
        }
        default:
            setVisibility(0);
            unk1A8 = 0;
            unk190.m_x = 0.0f;
            unk190.m_y = 0.0f;
            unk190.m_z = 0.0f;
            break;
        }
    }
}

// The model follows its matrix, turned by the angle of the cannon.
void grPiratesCannon::updateCallBack(float deltaFrame) {
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
            if (m_mtxWork != NULL) {
                Vec3f scale(1.1764706f, 1.1764706f, 1.1764706f);
                Matrix mtx = *m_mtxWork;
                mtx.rotY(unk1A0 * 0.017453292f);
                fn_8003E828(&mtx, &scale, &mtx);
                calcWorldCallBack->m_nodeCallbackDatas[0].m_matrix = mtx;
            }
        }
    }
}
