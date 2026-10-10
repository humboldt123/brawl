#define YK_STAGE_INLINE // inline members of the shared SO headers that the stage RELs emit themselves
#define YK_STAGE_FULL // ... and the ones of the hit objects with an attack module and an event observer
#define YK_DATA_INLINE_CTOR // the ykData / ykDataGroup constructors are inline in this REL
#define SO_COLLISION_GROUP_ALIGNED // the hit object embeds soArrayVector<soCollisionGroup, 1> with 4-byte aligned elements
#define SO_COLLISION_GROUP_MEMBERWISE // ... and copies the group member by member

#include <ai/ai_mgr.h>
#include <gf/gf_camera.h>
#include <gf/gf_model.h>
#include <gr/gr_calc_world_callback.h>
#include <math.h>
#include <memory.h>
#include <mt/mt_prng.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <snd/snd_system.h>
#include <types.h>
#include <yk/yk_no_hit_normal.h>

#include <st_norfair/gr_norfair.h>


// A row of the table of the lava (the file of the stage).
struct stNorfairMagmaRow {
    float m_level;      // the height the lava goes to
    float m_waitMin;    // frames it waits there
    float m_waitMax;
};

grNorfairMagma::grNorfairMagma(const char* taskName) : grNorfair(taskName) {
    m_tblIndex = 1;
    m_accel = 0.0f;
    m_accelPrev = 0.0f;
    m_offset = 0.0f;
    m_goal = 0.0f;
    m_unk170 = 0.0f;
    m_height = 0.0f;
    m_unk178 = 0.0f;
    m_tblLevelAcc = NULL;
    m_posMagmaWork = NULL;
    m_posLimitWork = NULL;
    memset(m_nodeColl, 0, sizeof(m_nodeColl));
    m_hasHit = 0;
    m_attackSet = 0;
    m_attackWork = NULL;
    m_dangerZone = -1;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_numNodeCallbackData = 1;
    callback->initialize(false, Heaps::StageInstance);
    callback->m_nodeCallbackDatas[0].m_flags |= 0x10;
}

grNorfairMagma* grNorfairMagma::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grNorfairMagma* ground = new (Heaps::StageInstance) grNorfairMagma(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grNorfairMagma::~grNorfairMagma() {
    if (m_attackWork != NULL) {
        delete m_attackWork;
    }
    m_attackWork = NULL;
}

void grNorfairMagma::processAnim() {
    Ground::processAnim();
    if (m_posMagmaWork != NULL) {
        getNodePosition(m_posMagmaWork, 0, m_nodeIndex);
    }
}

void grNorfairMagma::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateYakumono(deltaFrame);
        updateAI(deltaFrame);
        updateLevel(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

// The hit object is made the first time this is called, then the attacks of the lava are set.
void grNorfairMagma::updateYakumono(float deltaFrame) {
    if (m_hasHit == 1) {
        setAttack();
    } else {
        setHit();
        if (m_yakumono != NULL) {
            m_hasHit = 1;
        }
    }
}

// The AI keeps away from the lava (its top is the average height of two nodes of the collision).
void grNorfairMagma::updateAI(float deltaFrame) {
    if (*m_eventIDWork == 4) {
        if (m_dangerZone != -1) {
            g_aiMgr->delDangerZone(m_dangerZone);
            m_dangerZone = -1;
        }
    } else {
        Vec3f limitB;
        Vec3f limitA;
        Vec3f nodePosA;
        Vec3f nodePosB;
        Vec2f zoneA;
        Vec2f zoneB;
        getNodePosition(&nodePosA, 0, m_nodeColl[8]);
        getNodePosition(&nodePosB, 0, m_nodeColl[9]);
        limitA.m_x = m_posLimitWork[0].m_x;
        limitB.m_x = m_posLimitWork[1].m_x;
        limitA.m_y = (nodePosA.m_y + nodePosB.m_y) * 0.5f + 40.0f;
        limitB.m_y = m_posLimitWork[1].m_y;
        if (limitA.m_y > 40.0f) {
            limitA.m_y = 40.0f;
        }
        if (limitA.m_y < limitB.m_y) {
            limitA.m_y = limitB.m_y;
        }
        zoneB.m_x = limitB.m_x;
        zoneB.m_y = limitB.m_y;
        zoneA.m_x = limitA.m_x;
        zoneA.m_y = limitA.m_y;
        m_dangerZone = g_aiMgr->setDangerZone(&zoneA, &zoneB, m_dangerZone, false, false);
    }
}

// The lava rises and falls: the table of the lava gives the height and the wait of each step, the speed grows up to a
// limit of the stage data, and the sound of a rise or a fall is played when a step starts.
void grNorfairMagma::updateLevel(float deltaFrame) {
    stNorfairData* data = static_cast<stNorfairData*>(getStageData());
    if (data != NULL) {
        m_timer = m_timer - deltaFrame;
        if (m_timer < 0.0f) {
            m_timer = 0.0f;
        }
        stNorfairMagmaRow* row = reinterpret_cast<stNorfairMagmaRow*>(m_tblLevelAcc->getData(m_tblIndex));
        if (row == NULL) {
            m_tblIndex = 1;
        } else {
            if (*m_eventIDWork == 4) {
                m_timer = 0.0f;
                if (m_tblIndex != 0) {
                    m_state = 1;
                }
            }
            switch (m_state) {
            case 0:
                m_height = row->m_level;
                m_timer = row->m_waitMin + (row->m_waitMax - row->m_waitMin) * randf();
                m_state = 1;
                break;
            case 1:
                if (m_timer == 0.0f) {
                    if (*m_eventIDWork == 4) {
                        m_tblIndex = 0;
                    } else {
                        m_tblIndex++;
                        m_offset = data->unk08 * randf();
                    }
                    if (row->m_level + m_offset > m_height) {
                        g_sndSystem->playSE(static_cast<SndID>(0x1BC4), 0, 0, 0, -1);
                    }
                    if (row->m_level + m_offset < m_height) {
                        g_sndSystem->playSE(static_cast<SndID>(0x1BC5), 0, 0, 0, -1);
                    }
                    m_state = 2;
                    m_accel = 0.0f;
                    m_accelPrev = 0.0f;
                }
                break;
            case 2:
                m_goal = row->m_level + m_offset;
                if (m_goal > m_height) {
                    m_accel += data->unk0C * deltaFrame;
                }
                if (m_goal < m_height) {
                    m_accel -= data->unk0C * deltaFrame;
                }
                if (fabs(m_accel) > data->unk10) {
                    if (m_goal > m_height) {
                        m_accel = data->unk10 * deltaFrame;
                    }
                    if (m_goal < m_height) {
                        m_accel = data->unk10 * deltaFrame * -1.0f;
                    }
                }
                m_height += m_accel * deltaFrame;
                if (m_accelPrev != 0.0f) {
                    bool stopped = false;
                    if (m_accelPrev > 0.0f && m_accel < 0.0f) {
                        stopped = true;
                    }
                    if (m_accelPrev < 0.0f && m_accel > 0.0f) {
                        stopped = true;
                    }
                    if (m_accel == 0.0f) {
                        stopped = true;
                    }
                    if (stopped == true) {
                        m_timer = row->m_waitMin + (row->m_waitMax - row->m_waitMin) * randf();
                        m_state = 1;
                    }
                }
                m_accelPrev = m_accel;
                break;
            }
        }
    }
}

void grNorfairMagma::updateCallBack(float deltaFrame) {
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
            calcWorldCallBack->m_nodeCallbackDatas->m_offsetPos.m_y = m_height;
        }
    }
}

bool grNorfairMagma::setNode() {
    bool result = grGimmick::setNode();
    getNodeIndex(&m_nodeColl[0], 0, "Collision_left1");
    getNodeIndex(&m_nodeColl[1], 0, "Collision_left2");
    getNodeIndex(&m_nodeColl[2], 0, "Collision_left3");
    getNodeIndex(&m_nodeColl[3], 0, "Collision_left4");
    getNodeIndex(&m_nodeColl[4], 0, "Collision_left5");
    getNodeIndex(&m_nodeColl[5], 0, "Collision_left6");
    getNodeIndex(&m_nodeColl[6], 0, "Collision_center1");
    getNodeIndex(&m_nodeColl[7], 0, "Collision_center2");
    getNodeIndex(&m_nodeColl[8], 0, "Collision_center3");
    getNodeIndex(&m_nodeColl[9], 0, "Collision_center4");
    getNodeIndex(&m_nodeColl[10], 0, "Collision_center5");
    getNodeIndex(&m_nodeColl[11], 0, "Collision_center6");
    getNodeIndex(&m_nodeColl[12], 0, "Collision_right1");
    getNodeIndex(&m_nodeColl[13], 0, "Collision_right2");
    getNodeIndex(&m_nodeColl[14], 0, "Collision_right3");
    getNodeIndex(&m_nodeColl[15], 0, "Collision_right4");
    getNodeIndex(&m_nodeColl[16], 0, "Collision_right5");
    getNodeIndex(&m_nodeColl[17], 0, "Collision_right6");
    return result;
}

// The hit object of the lava: an attack module with eighteen attack parts (one per node of the collision) and no hit module.
void grNorfairMagma::setHit() {
    m_attackWork = new (Heaps::StageInstance) ykData;
    m_attackWork->m_dataGroupNum = 0;
    m_attackWork->m_dataGroups = NULL;

    ykInitInfo info = { 0, 0, 0x10, 0, 0 };
    info.m_ground = this;
    info.m_node = ykDynamicCastScnMdl(m_sceneModels[0]);
    Vec3f pos = getPos();
    info.m_pos = &pos;
    info.m_work = m_attackWork;

    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Gimmick, 18, 0, soCollisionAttackModuleImpl, 1, false, true> Config;
    ykNoHitNormal<Config>* yakumono = new (Heaps::StageInstance) ykNoHitNormal<Config>(&info);
    setYakumono(yakumono);
}

void grNorfairMagma::setAttack() {
    if (m_attackSet != 1) {
        for (u8 i = 0; i < 18; i++) {
            Vec3f offset;
            offset.m_x = 0.0f;
            offset.m_y = -30.0f;
            offset.m_z = 0.0f;
            setAttackDetails(i, m_nodeColl[i], &offset);
        }
        m_attackSet = 1;
    }
}

// One hit sphere of the lava (size 30): it burns (power 14).
void grNorfairMagma::setAttackDetails(u8 index, u32 nodeIndex, Vec3f* offset) {
    soCollisionAttackData attack(1.0f);
    setAttackGimmickDetails(&attack, 30.0f, 1.0f, 1.0f, 1.0f,
        14, offset, 90, 50, 0, 80, nodeIndex,
        0x3FF, 7, false, 15,
        soCollisionAttackData::Attribute_Fire, soCollisionAttackData::Sound_Level_Large,
        soCollisionAttackData::Sound_Attribute_Fire,
        false, false, false, true, false, false, 0, 60,
        false, false, false, soCollisionAttackData::Lr_Check_Lr,
        false, false, true, false, false, soCollisionAttackData::Region_None, false);
    m_yakumono->setAttack(index, 0, &attack);
}
