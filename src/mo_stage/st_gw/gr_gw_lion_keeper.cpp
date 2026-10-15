#define YK_STAGE_INLINE // inline members of the shared SO headers that the stage RELs emit themselves
#define YK_STAGE_FULL // ... and the ones of the hit objects with an attack module and an event observer
#define SO_COLLISION_GROUP_ALIGNED // the hit object embeds soArrayVector<soCollisionGroup, 1> with 4-byte aligned elements
#define SO_COLLISION_GROUP_MEMBERWISE // ... and copies the group member by member

#include <gr/gr_calc_world_callback.h>
#include <memory.h>
#include <mt/mt_prng.h>
#include <types.h>
#include <yk/yk_no_hit_normal.h>

#include <st_gw/gr_gw_item.h>
#include <st_gw/gr_gw_lion_keeper.h>

static grGWMarker sMarkerA(0xFF, 0);
static grGWMarker sMarkerB(0xFF, 1);

grGWLionKeeper::grGWLionKeeper(const char* taskName) : grGW(taskName), m_snd() {
    m_posWork = NULL;
    m_sceneWork = NULL;
    m_sceneNo = 4;
    m_posIndex = 0;
    m_type = 0x10;
    m_played = 0;
    m_yakumonoMade = 0;
    m_attackOn = 0;
    m_work = NULL;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_nodeCallbackDatas[0].m_flags |= 1;
}

grGWLionKeeper* grGWLionKeeper::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grGWLionKeeper* ground = new (Heaps::StageInstance) grGWLionKeeper(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grGWLionKeeper::~grGWLionKeeper() {
    if (m_work != NULL) {
        delete m_work;
    }
    m_work = NULL;
}

void grGWLionKeeper::update(float deltaFrame) {
    grGW::update(deltaFrame);
    if (m_isUpdate) {
        updateYakumono(deltaFrame);
        updateScaleBase(deltaFrame);
        updateActive(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

// The hit object is made when the keeper is first updated; after that it follows the hit node of its side.
void grGWLionKeeper::updateYakumono(float deltaFrame) {
    if (m_yakumonoMade == 1) {
        Vec3f pos;
        switch (m_type) {
        case 4:
            getNodePosition(&pos, 0, "_KEEPER_LEFT_HIT");
            break;
        case 5:
            getNodePosition(&pos, 0, "_KEEPER_RIGHT_HIT");
            break;
        }
        setPos(&pos);
    } else {
        setHit();
        if (m_yakumono != NULL) {
            m_yakumonoMade = 1;
        }
    }
}

void grGWLionKeeper::updateActive(float deltaFrame) {
    float* data = static_cast<float*>(getStageData());
    if (data != NULL) {
        m_timer -= deltaFrame;
        if (m_timer < 0.0f) {
            m_timer = 0.0f;
        }
        switch (m_state) {
        case 0:
            setMotion(1, false, true, NULL);
            setVisibility(0);
            if (m_attackOn == 1) {
                disableAttack(0);
            }
            m_attackOn = 0;
            m_state = 1;
            break;
        case 1:
            if (*m_sceneWork == m_sceneNo) {
                setMotion(0, true, true, &m_frameCount);
                setMotionFrame(m_frameCount, 0);
                m_frameLast = m_frameCount;
                if (m_motionRatio > 0.0f) {
                    m_motionRatio *= -1.0f;
                }
                m_played = 0;
                m_state = 3;
            }
            break;
        case 3:
            if (*m_sceneWork != m_sceneNo) {
                setMotion(0, false, true, &m_frameCount);
                if (m_motionRatio < 0.0f) {
                    m_motionRatio *= -1.0f;
                }
                disableAttack(0);
                m_attackOn = 0;
                m_state = 4;
            } else {
                if (!m_isVisible) {
                    setVisibility(1);
                }
                if (m_played == 0) {
                    if (getMotionFrame(0) > m_frameLast) {
                        setMotion(1, false, false, NULL);
                        m_played = 1;
                        setAttack();
                    } else {
                        m_frameLast = getMotionFrame(0);
                    }
                }
                if (m_timer == 0.0f) {
                    float r = randf();
                    if (r < 1.0f / 3.0f) {
                        m_posIndex = 0;
                    } else if (r < 2.0f / 3.0f) {
                        m_posIndex = 1;
                    } else {
                        m_posIndex = 2;
                    }
                    m_timer = data[21] + (data[22] - data[21]) * randf();
                }
            }
            break;
        case 4:
            if (getMotionFrame(0) >= m_frameCount) {
                m_state = 0;
            }
            break;
        }
    }
}

void grGWLionKeeper::updateCallBack(float deltaFrame) {
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
            if (m_posWork != NULL) {
                grNodeCallbackData* data = calcWorldCallBack->m_nodeCallbackDatas;
                Vec3f* pos = &m_posWork[m_posIndex];
                data->m_pos.m_x = pos->m_x;
                data->m_pos.m_y = pos->m_y;
                data->m_pos.m_z = pos->m_z;
            }
            Vec3f scale;
            gwVec3Scale(&scale, &m_scale, 0.9f);
            grNodeCallbackData* data = calcWorldCallBack->m_nodeCallbackDatas;
            data->m_scale.m_x = scale.m_x;
            data->m_scale.m_y = scale.m_y;
            data->m_scale.m_z = scale.m_z;
        }
    }
}

// The hit object has no hit spheres of its own, only an attack module with one part (the keeper hurts fighters).
void grGWLionKeeper::setHit() {
    m_work = new (Heaps::StageInstance) ykData;
    ykInitInfo info = { 0, 0, 0x10, 0, 0 };
    info.m_ground = this;
    info.m_node = ykDynamicCastScnMdl(m_sceneModels[0]);
    Vec3f pos = getPos();
    info.m_pos = &pos;
    info.m_work = m_work;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Gimmick, 1, 0, soCollisionAttackModuleImpl, 1, false, true> Config;
    ykNoHitNormal<Config>* yakumono = new (Heaps::StageInstance) ykNoHitNormal<Config>(&info);
    setYakumono(yakumono);
}

// The keeper hurts with its hand: 10 percent, from the hit node of its side, knocking fighters away from the keeper.
void grGWLionKeeper::setAttack() {
    if (m_attackOn != 1) {
        soCollisionAttackData attack(1.0f);
        u32 node;
        switch (m_type) {
        case 5:
            node = getNodeIndex(0, "_KEEPER_RIGHT_HIT");
            break;
        case 4:
            node = getNodeIndex(0, "_KEEPER_LEFT_HIT");
            break;
        default:
            return;
        }
        Vec3f offset;
        offset.m_x = 0.0f;
        offset.m_y = 0.0f;
        offset.m_z = 0.0f;
        setAttackGimmickDetails(&attack, 10.0f, 1.0f, 1.0f, 1.0f, 12, &offset, 0, 60, 0, 60, node, 0x3FF, 7, false, 15,
                                soCollisionAttackData::Attribute_Normal, soCollisionAttackData::Sound_Level_Small,
                                soCollisionAttackData::Sound_Attribute_Punch, false, false, false, true, false, false, 0, 60,
                                false, false, false, soCollisionAttackData::Lr_Check_Forward, false, false, false, false,
                                false, soCollisionAttackData::Region_None, true);
        switch (m_type) {
        case 5:
            m_yakumono->setLr(-1.0f);
            break;
        case 4:
            m_yakumono->setLr(1.0f);
            break;
        }
        m_yakumono->setAttack(0, 0, &attack);
        m_attackOn = 1;
    }
}

// A fighter was hurt: the keeper's sound plays at the place of the keeper.
void grGWLionKeeper::onInflict(soCollisionLog* log, u32 unk, float damage) {
    m_snd.playSE(static_cast<SndID>(0x1C93), 0, 0, -1);
    m_snd.setPos(&m_posWork[m_posIndex]);
}
