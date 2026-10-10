#define YK_STAGE_INLINE // inline members of the shared SO headers that the stage RELs emit themselves
#define SO_COLLISION_GROUP_ALIGNED // the hit object embeds soArrayVector<soCollisionGroup, 1> with 4-byte aligned elements
#include <ec/ec_mgr.h>
#include <ef/ef_id.h>
#include <gr/gr_calc_world_callback.h>
#include <mt/mt_prng.h>
#include <snd/snd_id.h>
#include <st_dxonett/gr_dxonett.h>
#include <yk/yk_no_hit_normal.h>
#include <gr/gr_calc_world_callback.h>

grDxOnettCar* grDxOnettCar::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grDxOnettCar* ground = new (Heaps::StageInstance) grDxOnettCar(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grDxOnettCar::grDxOnettCar(const char* taskName) : grDxOnett(taskName) {
    setPos(0.0f, 0.0f, 0.0f);
    setRot(0.0f, 0.0f, 0.0f);
    m_flipped = 0;
    m_moveSpeed = 0.0f;
    m_posLimitWork = NULL;
    m_curWork = NULL;
    m_revWork = NULL;
    m_stateWork = NULL;
    m_type = 4;
    m_soundA = 0;
    m_soundB = 0;
    makeCalcuCallback(1, Heaps::StageInstance);
    setCalcuCallbackRoot(3);
}

grDxOnettCar::~grDxOnettCar() { }

void grDxOnettCar::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateMove(deltaFrame);
        updateCallback(0);
    }
}

// The car's engine noise when it comes into view (one of six).
static inline void carPlayComing(snd3DGenerator* snd) {
    float rnd = randf();
    if (rnd < 1.0f / 6.0f) {
        snd->playSE(static_cast<SndID>(0x1DD3), 0, 0, -1);
    } else if (rnd < 1.0f / 3.0f) {
        snd->playSE(static_cast<SndID>(0x1DD4), 0, 0, -1);
    } else if (rnd < 0.5f) {
        snd->playSE(static_cast<SndID>(0x1DD5), 0, 0, -1);
    } else if (rnd < 2.0f / 3.0f) {
        snd->playSE(static_cast<SndID>(0x1DD6), 0, 0, -1);
    } else if (rnd < 5.0f / 6.0f) {
        snd->playSE(static_cast<SndID>(0x1DD7), 0, 0, -1);
    } else {
        snd->playSE(static_cast<SndID>(0x1DD8), 0, 0, -1);
    }
}

// The rotation of a car turned further around the vertical axis.
static inline Vec3f carTurn(const Vec3f& rot, float add) {
    return Vec3f(rot.m_x, rot.m_y + add, rot.m_z);
}

// The car drives along the road. Which of the four cars drives is chosen by the stage; the car starts at one end of the road
// and drives to the other (state 1 = wait for the start, 3 = drives left, 4 = drives right, 5 = ends).
void grDxOnettCar::updateMove(float deltaFrame) {
    float* data = static_cast<float*>(getStageData());
    if (data == NULL) {
        return;
    }
    m_timer -= deltaFrame;
    if (m_timer < 0.0f) {
        m_timer = 0.0f;
    }
    switch (m_state) {
    case 0: {
        setVisibility(false);
        m_flipped = 0;
        m_soundA = 0;
        m_soundB = 0;
        Vec3f zero;
        zero.m_x = 0.0f;
        zero.m_y = 0.0f;
        zero.m_z = 0.0f;
        m_snd.setPos(&zero);
        m_state = 1;
        break;
    }
    case 1: {
        setPos(m_posGimmickWork[2].m_x, m_posGimmickWork[2].m_y, m_posGimmickWork[2].m_z);
        if (*m_curWork == m_type) {
            setPos(m_posGimmickWork[2].m_x, m_posGimmickWork[2].m_y, m_posGimmickWork[2].m_z);
            setRot(0.0f, 0.0f, 0.0f);
            setVisibility(true);
            float rnd = randf();
            m_moveSpeed = data[7] + data[8] * rnd;
            m_state = 3;
        }
        if (*m_revWork == m_type) {
            setPos(m_posGimmickWork[1].m_x, m_posGimmickWork[1].m_y, m_posGimmickWork[1].m_z);
            setRot(0.0f, 180.0f, 0.0f);
            setVisibility(true);
            float rnd = randf();
            m_moveSpeed = data[7] + data[8] * rnd;
            m_state = 4;
        }
        break;
    }
    case 3: {
        Vec3f moved;
        Vec3f dir;
        dir = m_posGimmickWork[0] - m_posGimmickWork[2];
        dir.normalize();
        float step = m_moveSpeed * deltaFrame;
        dir.m_x = dir.m_x * step;
        dir.m_y = dir.m_y * step;
        dir.m_z = dir.m_z * step;
        moved = getPos();
        moved = moved + dir;
        setPos(moved.m_x, moved.m_y, moved.m_z);
        m_posGimmickWork[7].m_x = moved.m_x;
        m_posGimmickWork[7].m_y = moved.m_y;
        m_posGimmickWork[7].m_z = moved.m_z;
        if (moved.m_x < m_posGimmickWork[0].m_x) {
            m_state = 5;
        }
        if (m_soundA == 0 && moved.m_x < m_posLimitWork[1].m_x) {
            carPlayComing(&m_snd);
            m_soundA = 1;
        }
        if (m_soundB == 0 && moved.m_x < data[18]) {
            if (*m_stateWork == 0) {
                float rnd = randf();
                if (rnd < 1.0f / 3.0f) {
                    m_snd.playSE(static_cast<SndID>(0x1DCD), 0, 0, -1);
                } else if (rnd < 2.0f / 3.0f) {
                    m_snd.playSE(static_cast<SndID>(0x1DCE), 0, 0, -1);
                } else {
                    m_snd.playSE(static_cast<SndID>(0x1DCF), 0, 0, -1);
                }
                m_soundB = 1;
            }
            m_soundB = 1;
        }
        if (m_flipped == 1) {
            Vec3f turned = carTurn(getRot(), data[16]);
            if (360.0f < turned.m_y) {
                turned.m_y -= 360.0f;
            }
            if (turned.m_y < 0.0f) {
                turned.m_y += 360.0f;
            }
            setRot(turned.m_x, turned.m_y, turned.m_z);
        } else if (*m_stateAttackWork == 1) {
            if (randf() < data[14]) {
                float factor = data[13];
                m_flipped = 1;
                m_moveSpeed = m_moveSpeed * factor;
                u32 effect = g_ecMgr->setEffect(ef_ptc_stg_dx_onett_carspin);
                switch (m_type) {
                case 2:
                    g_ecMgr->setParent(effect, m_sceneModels[0], "Particle", true);
                    break;
                case 0:
                    g_ecMgr->setParent(effect, m_sceneModels[0], "Particle1", true);
                    break;
                case 1:
                    g_ecMgr->setParent(effect, m_sceneModels[0], "Particle2", true);
                    break;
                case 3:
                    g_ecMgr->setParent(effect, m_sceneModels[0], "Particle3", true);
                    break;
                }
            }
            if (randf() < 0.5f) {
                m_snd.playSE(static_cast<SndID>(0x1DD9), 0, 0, -1);
            } else {
                m_snd.playSE(static_cast<SndID>(0x1DDA), 0, 0, -1);
            }
            *m_stateAttackWork = 2;
        }
        break;
    }
    case 4: {
        Vec3f next;
        Vec3f dir;
        dir = m_posGimmickWork[3] - m_posGimmickWork[1];
        dir.normalize();
        float step = m_moveSpeed * deltaFrame;
        dir.m_x = dir.m_x * step;
        dir.m_y = dir.m_y * step;
        dir.m_z = dir.m_z * step;
        next = getPos();
        next = next + dir;
        setPos(next.m_x, next.m_y, next.m_z);
        if (m_soundA == 0 && m_posLimitWork[0].m_x < next.m_x) {
            carPlayComing(&m_snd);
            m_soundA = 1;
        }
        if (m_posGimmickWork[3].m_x < next.m_x) {
            m_state = 5;
        }
        break;
    }
    case 5:
        if (*m_curWork == m_type) {
            *m_curWork = 4;
        }
        if (*m_revWork == m_type) {
            *m_revWork = 4;
        }
        m_state = 0;
        break;
    }
}

// ---- Attack ----

// Yakumono::setGlobalOffset(u8, u32): forwards to the hit module (HYPOTHESIS: the arguments are a flag and a group index).
extern "C" void fn_27_263800(Yakumono* yakumono, u8 flag, u32 groupIndex);

grDxOnettAttack* grDxOnettAttack::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grDxOnettAttack* ground = new (Heaps::StageInstance) grDxOnettAttack(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grDxOnettAttack::grDxOnettAttack(const char* taskName) : grDxOnett(taskName) {
    m_curWork = NULL;
    m_stateWork = NULL;
    m_hasYakumono = 0;
    m_attackSet = 0;
    m_work = NULL;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_numNodeCallbackData = 1;
    callback->initialize(false, Heaps::StageInstance);
    callback->m_nodeCallbackDatas[0].m_flags |= 1;
}

grDxOnettAttack::~grDxOnettAttack() {
    if (m_work != NULL) {
        delete m_work;
    }
    m_work = NULL;
}

void grDxOnettAttack::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateYakumono(deltaFrame);
        updateMove(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

// Creates the hit object once the ground exists.
void grDxOnettAttack::updateYakumono(float deltaFrame) {
    if (m_hasYakumono != 1) {
        setHit();
        if (m_yakumono != NULL) {
            m_hasYakumono = 1;
        }
    }
}

// The attack is on while a car is on the road (the stage says which one with m_curWork, 4 = none); the position of the
// attack is the position the car publishes in the stage (entry 7).
void grDxOnettAttack::updateMove(float deltaFrame) {
    void* data = getStageData();
    if (data == NULL) {
        return;
    }
    switch (m_state) {
    case 0:
        disableHit(0, 0);
        disableAttack(0);
        m_attackSet = 0;
        m_state = 1;
        // fall through
    case 1:
        if (*m_curWork != 4) {
            m_state = 3;
        } else {
            Vec3f* pos = m_posGimmickWork;
            pos[7].m_x = pos[2].m_x;
            pos[7].m_y = pos[2].m_y;
            pos[7].m_z = pos[2].m_z;
        }
        break;
    case 2:
        break;
    case 3:
        if (*m_curWork == 4) {
            disableHit(0, 0);
            disableAttack(0);
            m_attackSet = 0;
            *m_stateWork = 2;
            m_state = 1;
        } else if (m_attackSet == 0) {
            enableHit(0, 0);
            setAttack();
        }
        break;
    }
}

void grDxOnettAttack::updateCallBack(float deltaFrame) {
    grCalcWorldCallBack* calcWorldCallBack = &m_calcWorldCallBack;
    if (calcWorldCallBack != NULL) {
        nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[m_unk1];
        if (scnMdl != NULL) {
            if (scnMdl->m_calcWorldCallBack == NULL) {
                calcWorldCallBack->m_index = 0;
                calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex = m_nodeIndex;
                scnMdl->m_calcWorldCallBack = calcWorldCallBack;
                scnMdl->EnableScnMdlCallbackTiming(1);
                scnMdl->m_nodeIndex = calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex;
            }
            grNodeCallbackData* data = calcWorldCallBack->m_nodeCallbackDatas;
            Vec3f* pos = m_posGimmickWork;
            data[0].m_pos.m_x = pos[7].m_x;
            data[0].m_pos.m_y = pos[7].m_y;
            data[0].m_pos.m_z = pos[7].m_z;
            calcWorldCallBack->m_nodeCallbackDatas[0].m_pos.m_y += 5.0f;
            calcWorldCallBack->m_nodeCallbackDatas[0].m_pos.m_z = 0.0f;
        }
    }
}

// Builds the attack's hit object: one attack part, one collision group and no hit module (a ykNoHitNormal).
void grDxOnettAttack::setHit() {
    m_work = new (Heaps::StageInstance) grDxOnettAttackWork;
    m_work->unk0 = 0;
    m_work->unk4 = 0;

    ykInitInfo info = {NULL, NULL, 0x10, NULL, NULL};
    info.m_ground = this;
    nw4r::g3d::ScnMdl* model = ykDynamicCastScnMdl(m_sceneModels[0]);
    info.m_node = model;
    Vec3f pos = getPos();
    info.m_pos = &pos;
    info.m_work = m_work;

    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Gimmick, 1, 0, soCollisionAttackModuleImpl, 1, false, true> Config;
    ykNoHitNormal<Config>* yakumono = new (Heaps::StageInstance) ykNoHitNormal<Config>(&info);
    setYakumono(yakumono);
    fn_27_263800(yakumono, 1, 0);
}

// A box of 5 units around the node; it throws the fighters it hits.
void grDxOnettAttack::setAttack() {
    if (m_attackSet == 1) {
        return;
    }

    soCollisionAttackData attack(1.0f);
    Vec3f offset;
    offset.m_x = 0.0f;
    offset.m_y = 0.0f;
    offset.m_z = 0.0f;

    setAttackGimmickDetails(&attack, 5.0f, 1.0f, 1.0f, 1.0f,
        30, &offset, 45, 80, 150, 0, 0,
        0x3FF, 7, false, 15,
        soCollisionAttackData::Attribute_Normal, soCollisionAttackData::Sound_Level_Small,
        soCollisionAttackData::Sound_Attribute_None,
        false, false, false, true, false, false, 0, 60,
        false, false, false, soCollisionAttackData::Lr_Check_Pos,
        false, false, false, false, false, soCollisionAttackData::Region_None, false);
    m_yakumono->setAttack(0, 0, &attack);
    m_attackSet = 1;
}

// A fighter was hit: the stage learns about it (HYPOTHESIS: 1 = a car hit a fighter).
void grDxOnettAttack::onInflict(soCollisionLog* collisionLog, u32 flags, float power) {
    *m_stateWork = 1;
}
