#include <ec/ec_mgr.h>
#include <ef/ef_id.h>
#include <gr/gr_calc_world_callback.h>
#include <math.h>
#include <st_dxonett/gr_dxonett.h>

// MATCH-ONLY: Ground::isCollisionStatusOwnerTask takes the category flags as an int pointer in the original (the symbol
// of sora_melee is ...FP12grCollStatusPi).
extern "C" bool isCollisionStatusOwnerTask__6GroundFP12grCollStatusPi(Ground* ground, grCollStatus* collStatus, int* flag);

// MATCH-ONLY: passing the offset by value reproduces the original's temporary copies (same idiom as grGreenhillBreak).
static inline void assignVec2f(Vec2f& dst, Vec2f src) {
    dst = src;
}

static int sLandCategoryFighter = 1;
static int sLandCategoryItem = 4;

grDxOnettAshiba* grDxOnettAshiba::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grDxOnettAshiba* ground = new (Heaps::StageInstance) grDxOnettAshiba(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grDxOnettAshiba::grDxOnettAshiba(const char* taskName) : grDxOnett(taskName) {
    m_type = 2;
    m_offsetX = 0.0f;
    m_offsetY = 0.0f;
    m_offsetZ = 0.0f;
    m_speed = 0.0f;
    m_landed = 0;
    m_push = 0.0f;
    m_target = 0.0f;
    m_landCount = 0;
    m_landCountPrev = 0;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_numNodeCallbackData = 1;
    callback->initialize(false, Heaps::StageInstance);
    callback->m_nodeCallbackDatas[0].m_flags |= 0x10;
    createSoundWork(1, 1);
    m_soundEffects[0].m_id = static_cast<SndID>(0x1DD2);
    m_soundEffects[0].m_repeatFrame = 0;
    m_soundEffects[0].m_nodeIndex = 0;
    m_soundEffects[0].m_endFrame = 0;
    assignVec2f(m_soundEffects[0].m_offsetPos, Vec2f(0.0f, 0.0f));
}

grDxOnettAshiba::~grDxOnettAshiba() { }

void grDxOnettAshiba::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateLanding(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

// The tree is a spring: fighters that land on it push it down (by at most 7 units) and it swings back. A hard landing
// plays a sound and shakes leaves off.
void grDxOnettAshiba::updateLanding(float deltaFrame) {
    float timer = m_timer;
    m_timer = timer - deltaFrame;
    if (timer - deltaFrame < 0.0f) {
        m_timer = 0.0f;
    }

    if (m_push < -7.0f) {
        m_push = -7.0f;
    }
    float diff = m_push - m_offsetY;
    m_target = m_push;
    float ratio = __fabsf(diff) / 7.0f;
    float base = 0.02f + m_speed;
    float speed;
    if (diff < 0.0f) {
        speed = base - ratio * 0.5f;
    } else {
        speed = base + ratio * 0.5f;
    }
    speed = (speed - 0.02f) * 0.95f;
    if (speed > 2.0f) {
        speed = 2.0f;
    } else if (speed < -2.0f) {
        speed = -2.0f;
    }

    if (__fabsf(m_offsetY) - m_target < 0.2f && __fabsf(speed) < 0.2f) {
        m_offsetY = m_target;
        m_speed = 0.0f;
    } else {
        float next = m_offsetY + speed;
        m_speed = speed;
        m_offsetY = next;
        if (next > 7.0f) {
            m_offsetY = 7.0f;
        } else if (next < -7.0f) {
            m_offsetY = -7.0f;
        }
    }

    if (m_landCount != m_landCountPrev || m_landed == 1) {
        Vec3f pos;
        getNodePosition(&pos, 0, m_nodeIndex);
        g_ecMgr->setEffect(ef_ptc_stg_dx_onett_ha, &pos);
        startGimmickSE(0);
    }
    m_push = 0.0f;
    m_landCountPrev = m_landCount;
    m_landCount = 0;
    m_landed = 0;
}

void grDxOnettAshiba::updateCallBack(float deltaFrame) {
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
            grNodeCallbackData* data = calcWorldCallBack->m_nodeCallbackDatas;
            data->m_offsetPos.m_x = m_offsetX;
            data->m_offsetPos.m_y = m_offsetY;
            data->m_offsetPos.m_z = m_offsetZ;
        }
    }
}

void grDxOnettAshiba::receiveCollMsg_Landing(grCollStatus* collStatus, grCollisionJoint* joint, bool isStartCollision) {
    if (isCollisionStatusOwnerTask__6GroundFP12grCollStatusPi(this, collStatus, &sLandCategoryFighter) == 1
        || isCollisionStatusOwnerTask__6GroundFP12grCollStatusPi(this, collStatus, &sLandCategoryItem) == 1) {
        if (isStartCollision == 1) {
            m_landed = 1;
            m_speed = -1.5f;
        }
        m_landCount++;
        m_push += -2.0f;
    }
}
