#define YK_STAGE_INLINE // inline members of the shared SO headers that the stage RELs emit themselves
#define YK_STAGE_FULL // ... and the ones of the hit objects with an attack module and an event observer
#define SO_COLLISION_GROUP_ALIGNED // the hit object embeds soArrayVector<soCollisionGroup, 1> with 4-byte aligned elements
#define SO_COLLISION_GROUP_MEMBERWISE // ... and copies the group member by member

#include <ai/ai_mgr.h>
#include <gr/gr_calc_world_callback.h>
#include <math.h>
#include <memory.h>
#include <mt/mt_prng.h>
#include <string.h>
#include <yk/yk_no_hit_normal.h>

#include <st_donkey/gr_donkey.h>

grDonkeyFireBall* grDonkeyFireBall::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grDonkeyFireBall* ground = new (Heaps::StageInstance) grDonkeyFireBall(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grDonkeyFireBall::grDonkeyFireBall(const char* taskName) : grDonkey(taskName) {
    m_posWork = NULL;
    m_stateWork = NULL;
    m_pos.m_x = 0.0f;
    m_pos.m_y = 0.0f;
    m_pos.m_z = 0.0f;
    m_posTgt.m_x = 0.0f;
    m_posTgt.m_y = 0.0f;
    m_posTgt.m_z = 0.0f;
    m_rot.m_x = 0.0f;
    m_rot.m_y = 0.0f;
    m_rot.m_z = 0.0f;
    m_dir = 6;
    m_type = 8;
    m_waypoint = 0;
    m_hasYakumono = 0;
    m_attackEnabled = 0;
    m_work = NULL;
    m_dangerZone = -1;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_nodeCallbackDatas[0].m_flags |= 1;
    callback->m_nodeCallbackDatas[0].m_flags |= 2;
}

grDonkeyFireBall::~grDonkeyFireBall() {
    if (m_work != NULL) {
        delete m_work;
    }
    m_work = NULL;
}

void grDonkeyFireBall::update(float deltaFrame) {
    if (m_isUpdate) {
        updateYakumono();
        updateScaleBase(deltaFrame);
        updateActive(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

// Creates the hit object once the model is there and switches the attack on.
void grDonkeyFireBall::updateYakumono() {
    if (m_hasYakumono != 1) {
        setHit();
        if (m_yakumono != NULL) {
            m_hasYakumono = 1;
            setAttack();
        }
    }
}

static inline bool donkeyNearZero(float v) {
    bool result = false;
    if ((float)fabs(v) < 1e-5f) {
        result = true;
    }
    return result;
}

// The life of a fire ball: 0 waits for the position of the stage and the order to go (state 3 of the stage data), 1 picks
// the next waypoint, 8 moves to it (sideways first when it goes to the right, then up or down) and waits for the pause.
// The stage data holds the speeds at 17 (left, right) and 18, the chance of going to the right at 19/20 and the pause at 22.
void grDonkeyFireBall::updateActive(float deltaFrame) {
    float* data = static_cast<float*>(getStageData());
    if (data != NULL) {
        m_timer -= deltaFrame;
        if (m_timer < 0.0f) {
            m_timer = 0.0f;
        }
        switch (m_state) {
        case 0: {
            Vec3f* posWork = m_posWork;
            bool unset = false;
            m_pos.m_x = posWork->m_x;
            m_pos.m_y = posWork->m_y;
            m_pos.m_z = posWork->m_z;
            if (donkeyNearZero(m_pos.m_x) && donkeyNearZero(m_pos.m_y) && donkeyNearZero(m_pos.m_z)) {
                unset = true;
            }
            if (!unset && *m_stateWork == 3) {
                m_state = 1;
            }
            break;
        }
        case 1:
            selectPosTgt();
            m_timer = data[22];
            m_state = 8;
            break;
        case 8:
            switch (m_dir) {
            case 1:
                updateMoveX(deltaFrame);
                if (m_pos.m_x == m_posTgt.m_x) {
                    updateMoveY(deltaFrame);
                    if (m_pos.m_y == m_posTgt.m_y && m_timer == 0.0f) {
                        m_state = 1;
                    }
                }
                break;
            case 0:
                updateMoveX(deltaFrame);
                if (m_timer == 0.0f) {
                    m_state = 1;
                }
                break;
            }
            break;
        }
    }
}

// Moves sideways towards the target at the speed of the stage data; the ball faces the way it goes.
void grDonkeyFireBall::updateMoveX(float deltaFrame) {
    float* data = static_cast<float*>(getStageData());
    if (data != NULL) {
        Vec3f diff;
        diff = m_posTgt - m_pos;
        float speed = diff.m_x;
        if (speed != 0.0f) {
            switch (m_dir) {
            case 0:
            case 1:
                speed = data[17];
                break;
            case 2:
                speed = data[18];
                break;
            }
            if (diff.m_x > 0.0f && speed < diff.m_x) {
                diff.m_x = speed * deltaFrame;
            }
            if (diff.m_x < 0.0f && diff.m_x < -speed) {
                diff.m_x = -speed * deltaFrame;
            }
            m_pos.m_x += diff.m_x;
            if (diff.m_x > 0.0f) {
                m_rot.m_x = 0.0f;
                m_rot.m_z = 0.0f;
                m_rot.m_y = 180.0f;
            }
            if (diff.m_x < 0.0f) {
                m_rot.m_x = 0.0f;
                m_rot.m_y = 0.0f;
                m_rot.m_z = 0.0f;
            }
        }
    }
}

// Moves up or down towards the target. The speed starts out as the sideways difference (the unused first value of the
// vector), as in the sideways move.
void grDonkeyFireBall::updateMoveY(float deltaFrame) {
    float* data = static_cast<float*>(getStageData());
    if (data != NULL) {
        Vec3f diff;
        diff = m_posTgt - m_pos;
        float speed = diff.m_x;
        if (diff.m_y != 0.0f) {
            switch (m_dir) {
            case 0:
            case 1:
                speed = data[17];
                break;
            case 2:
                speed = data[18];
                break;
            }
            if (diff.m_y > 0.0f && speed < diff.m_y) {
                diff.m_y = speed * deltaFrame;
            }
            if (diff.m_y < 0.0f && diff.m_y < -speed) {
                diff.m_y = -speed * deltaFrame;
            }
            m_pos.m_y += diff.m_y;
        }
    }
}

void grDonkeyFireBall::updateCallBack(float deltaFrame) {
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
            grNodeCallbackData* data = calcWorldCallBack->m_nodeCallbackDatas;
            data->m_pos.m_x = m_pos.m_x;
            data->m_pos.m_y = m_pos.m_y;
            data->m_pos.m_z = m_pos.m_z;
            data = calcWorldCallBack->m_nodeCallbackDatas;
            data->m_rot.m_x = m_rot.m_x;
            data->m_rot.m_y = m_rot.m_y;
            data->m_rot.m_z = m_rot.m_z;
            Vec3f scale;
            donkeyVec3Scale(&scale, &m_scaleBase, 0.9f);
            data = calcWorldCallBack->m_nodeCallbackDatas;
            data->m_scale.m_x = scale.m_x;
            data->m_scale.m_y = scale.m_y;
            data->m_scale.m_z = scale.m_z;
            // The AI is warned of the area around the ball.
            Vec2f zoneMin;
            Vec2f zoneMax;
            zoneMin.m_y = m_pos.m_y - 20.0f;
            zoneMin.m_x = m_pos.m_x - 20.0f;
            zoneMax.m_y = m_pos.m_y + 20.0f;
            zoneMax.m_x = m_pos.m_x + 20.0f;
            m_dangerZone = g_aiMgr->setDangerZone(&zoneMin, &zoneMax, m_dangerZone, false, false);
        }
    }
}

// Builds the hit object: one attack part (the ball) and one collision group, no hit module.
void grDonkeyFireBall::setHit() {
    m_work = new (Heaps::StageInstance) grDonkeyWork;
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
}

// The ball burns: 10 percent, fire, knocks fighters away at 20 degrees.
void grDonkeyFireBall::setAttack() {
    if (m_attackEnabled != 1) {
        soCollisionAttackData attack(1.0f);
        Vec3f offset;
        offset.m_x = 0.0f;
        offset.m_y = 5.0f;
        offset.m_z = 0.0f;

        setAttackGimmickDetails(&attack, 3.5f, 1.0f, 1.0f, 1.0f,
            10, &offset, 20, 28, 0, 90, m_nodeIndex,
            0x3FF, 7, false, 15,
            soCollisionAttackData::Attribute_Fire, soCollisionAttackData::Sound_Level_Medium,
            soCollisionAttackData::Sound_Attribute_Fire,
            false, false, false, true, false, false, 0, 60,
            false, false, false, soCollisionAttackData::Lr_Check_Pos,
            false, false, false, true, false, soCollisionAttackData::Region_None, false);
        m_yakumono->setAttack(0, 0, &attack);
        m_attackEnabled = 1;
    }
}

// Picks the waypoint to go to next from the one it is at; the stage data gives the chance (19 against 20) of going to the
// right, which the type of the ball (0 or 1, the two sides it comes from) turns into different routes between the five points.
void grDonkeyFireBall::selectPosTgt() {
    float* data = static_cast<float*>(getStageData());
    if (data != NULL) {
        int index = 0;
        float sum = data[19] + data[20];
        float chance = randf();
        bool toRight = false;
        if ((float)(data[19] / sum) < chance) {
            toRight = true;
        }
        switch (m_type) {
        case 0:
            switch (m_waypoint) {
            case 0:
                index = 3;
                if (toRight) {
                    index = 1;
                }
                break;
            case 1:
                index = 2;
                if (toRight) {
                    index = 0;
                }
                break;
            case 2:
                index = 1;
                if (toRight) {
                    index = 3;
                }
                break;
            case 3:
                if (randf() >= 0.5f) {
                    index = 4;
                } else {
                    index = 0;
                }
                if (toRight) {
                    index = 2;
                }
                break;
            case 4:
                index = 3;
                break;
            }
            break;
        case 1:
            switch (m_waypoint) {
            case 0:
                index = 1;
                break;
            case 1:
                index = 0;
                if (toRight) {
                    index = 3;
                }
                break;
            case 2:
                index = 3;
                break;
            case 3:
                if (randf() >= 0.5f) {
                    index = 4;
                } else {
                    index = 2;
                }
                if (toRight) {
                    index = 3;
                }
                break;
            case 4:
                index = 3;
                if (toRight) {
                    index = 6;
                }
                break;
            case 5:
                index = 6;
                break;
            case 6:
                index = 5;
                if (toRight) {
                    index = 4;
                }
                break;
            }
            break;
        }
        Vec3f* target = &m_posWork[index];
        m_posTgt.m_x = target->m_x;
        m_posTgt.m_y = target->m_y;
        m_posTgt.m_z = target->m_z;
        m_waypoint = index;
        if (toRight) {
            m_dir = 1;
        } else {
            m_dir = 0;
        }
    }
}
