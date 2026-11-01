#define MT_VEC3F_IMPLICIT_ASSIGN // BYTES: this unit copies Vec3f as an aggregate (word moves), see mt_vector.h
#define YK_STAGE_INLINE // inline members of the shared SO headers that the stage RELs emit themselves
#define YK_CLTARGET_4_FIRST // the clTarget constructor writes the second member first in this REL
#define YK_STAGE_FULL // ... and the ones of the hit objects with an attack module and an event observer
#define YK_DATA_INLINE_CTOR // the ykData / ykDataGroup constructors are inline in this REL
#define SO_COLLISION_GROUP_ALIGNED // the hit object embeds soArrayVector<soCollisionGroup, 1> with 4-byte aligned elements
#define SO_COLLISION_GROUP_MEMBERWISE // ... and copies the group member by member

#include <cm/cm_quake.h>
#include <ft/fighter.h>
#include <ft/ft_manager.h>
#include <gf/gf_model.h>
#include <gr/collision/gr_collision.h>
#include <gr/collision/gr_collision_joint.h>
#include <gr/gr_calc_world_callback.h>
#include <memory.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <snd/snd_system.h>
#include <types.h>
#include <yk/yk_normal.h>

#include <st_famicom/gr_famicom.h>
#include <st_famicom/gr_famicom_anim.h>

// MATCH-ONLY: a flag byte of a line of a collision joint (the top bit makes it a line without ledge)
struct famicomLineView {
    u8 _pad[0x10];
    u8 m_flags;
};

grFamicomPow::grFamicomPow(const char* taskName) : grFamicom(taskName), m_snd() {
    m_posWork = NULL;
    m_blink = 0.0f;
    m_stateWork = NULL;
    m_hits = 0;
    m_nodeColl[0] = 0;
    m_nodeColl[1] = 0;
    memset(m_cooldown, 0, sizeof(m_cooldown));
    m_hasHit = 0;
    m_hitData = NULL;
    m_hitSimple = NULL;
    m_hitSet = NULL;
    m_dataGroup = NULL;
    m_data = NULL;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_numNodeCallbackData = 1;
    callback->initialize(false, Heaps::StageInstance);
    callback->m_nodeCallbackDatas[0].m_flags |= 1;
    callback->m_nodeCallbackDatas[0].m_flags |= 4;
}

grFamicomPow* grFamicomPow::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grFamicomPow* ground = new (Heaps::StageInstance) grFamicomPow(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grFamicomPow::~grFamicomPow() {
    if (m_hitData != NULL) {
        delete[] reinterpret_cast<u8*>(m_hitData); // MATCH-ONLY: raw storage (no element destructors)
    }
    m_hitData = NULL;
    if (m_hitSimple != NULL) {
        delete[] reinterpret_cast<u8*>(m_hitSimple);
    }
    m_hitSimple = NULL;
    if (m_hitSet != NULL) {
        delete m_hitSet;
    }
    m_hitSet = NULL;
    if (m_dataGroup != NULL) {
        delete m_dataGroup;
    }
    m_dataGroup = NULL;
    if (m_data != NULL) {
        delete m_data;
    }
    m_data = NULL;
}

bool grFamicomPow::setNode() {
    bool result = grGimmick::setNode();
    getNodeIndex(&m_nodeColl[0], 0, "PowCol01");
    getNodeIndex(&m_nodeColl[1], 0, "PowCol02");
    return result;
}

void grFamicomPow::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateYakumono(deltaFrame);
        updatePow(deltaFrame);
        updatePreBuild(deltaFrame);
        updateColl(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

// The hit object is made the first time this is called.
void grFamicomPow::updateYakumono(float deltaFrame) {
    if (m_hasHit != 1) {
        setHit();
        if (m_yakumono != NULL) {
            m_hasHit = 1;
        }
    }
}

// The block comes back (state 1), can be hit (2) until it was hit three times, then it is gone (3) for the time of the
// stage data.
void grFamicomPow::updatePow(float deltaFrame) {
    m_timer = m_timer - deltaFrame;
    if (m_timer < 0.0f) {
        m_timer = 0.0f;
    }
    switch (m_state) {
    case 0:
        m_state = 1;
        break;
    case 1:
        if (m_timer == 0.0f) {
            setVisibility(1);
            setEnableCollisionStatus(true);
            enableHit(0, 0);
            m_state = 2;
        }
        break;
    case 2:
        for (u8 i = 0; i < 7; i++) {
            m_cooldown[i] = m_cooldown[i] - deltaFrame;
            if (m_cooldown[i] < 0.0f) {
                m_cooldown[i] = 0.0f;
            }
        }
        break;
    case 3:
        if (m_timer < 60.0f) {
            m_hits = 0;
            m_state = 1;
        }
        break;
    }
}

// The block blinks while it comes back.
void grFamicomPow::updatePreBuild(float deltaFrame) {
    if (m_state == 2 || (m_hits == 3 && m_timer > 60.0f)) {
        m_blink = 4.0f;
    } else {
        m_blink = m_blink - deltaFrame;
        if (m_blink < 0.0f) {
            m_blink = 0.0f;
        }
        if (m_blink > 2.0f) {
            setVisibility(1);
        } else {
            setVisibility(0);
        }
        if (m_blink == 0.0f) {
            m_blink = 4.0f;
        }
    }
}

// The line under the block follows its nodes and gets lower with every hit (the block is hit from below, so it gets
// flatter).
void grFamicomPow::updateColl(float deltaFrame) {
    if (m_collision != NULL) {
        grCollisionJoint* joint = m_collision->getJoint(0);
        if (joint != NULL) {
            grCollData::VtxData* vtx = joint->m_vtxDatas;
            if (vtx != NULL) {
                joint->m_0x49 = 3;
                famicomLineView* line = reinterpret_cast<famicomLineView*>(joint->getLine(1));
                if (line != NULL) {
                    line->m_flags |= 0x80;
                }
                line = reinterpret_cast<famicomLineView*>(joint->getLine(3));
                if (line != NULL) {
                    line->m_flags |= 0x80;
                }
                u16 bottom;
                for (u16 i = 0; i < 2; i++) {
                    Vec3f pos;
                    getNodePosition(&pos, 0, m_nodeColl[i]);
                    vtx[i].m_pos.m_x = pos.m_x;
                    vtx[i].m_pos.m_y = pos.m_y;
                    if (i == 0) {
                        bottom = 3;
                    }
                    if (i == 1) {
                        bottom = 1;
                    }
                    float scale;
                    switch (m_hits) {
                    case 0:
                        scale = 1.0f;
                        break;
                    case 1:
                        scale = 0.7f;
                        break;
                    case 2:
                        scale = 0.35f;
                        break;
                    default:
                        scale = 0.001f;
                        break;
                    }
                    float eleven = 11.0f;
                    vtx[bottom].m_pos.m_x = pos.m_x;
                    vtx[bottom].m_pos.m_y = pos.m_y - (float)((double)eleven * (double)scale);
                }
            }
        }
    }
}

void grFamicomPow::updateCallBack(float deltaFrame) {
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
            Vec3f* posWork = m_posWork;
            if (posWork != NULL) {
                float scale;
                switch (m_hits) {
                case 0:
                    scale = 1.0f;
                    break;
                case 1:
                    scale = 0.7f;
                    break;
                case 2:
                    scale = 0.35f;
                    break;
                default:
                    scale = 0.001f;
                    break;
                }
                Vec3f pos = *posWork;
                pos.m_y = pos.m_y + (1.0f - scale) * 5.5f;
                grNodeCallbackData* data = calcWorldCallBack->m_nodeCallbackDatas;
                data->m_pos.m_x = pos.m_x;
                data->m_pos.m_y = pos.m_y;
                data->m_pos.m_z = pos.m_z;
                data->m_scale.m_x = 1.0f;
                data->m_scale.m_y = scale;
                data->m_scale.m_z = 1.0f;
            }
        }
    }
}

// The hit object of the block: a hit module with one hit sphere (size 6) and a damage module that reports the hits to onDamage.
void grFamicomPow::setHit() {
    // MATCH-ONLY: the original allocates the hit data as raw storage (no element construction)
    m_hitData = reinterpret_cast<soCollisionHitData*>(new (Heaps::StageInstance) u8[sizeof(soCollisionHitData)]);
    m_hitSimple = reinterpret_cast<soCollisionHitData::Simple*>(new (Heaps::StageInstance) u8[sizeof(soCollisionHitData::Simple)]);
    m_hitSet = new (Heaps::StageInstance) soSet<soCollisionHitData::Simple>;
    m_dataGroup = new (Heaps::StageInstance) ykDataGroup;
    m_data = new (Heaps::StageInstance) ykData;

    m_hitData->m_startOffsetPos.m_x = 0.0f;
    m_hitData->m_startOffsetPos.m_y = 0.0f;
    m_hitData->m_startOffsetPos.m_z = 0.0f;
    m_hitData->m_endOffsetPos.m_x = 0.0f;
    m_hitData->m_endOffsetPos.m_y = 0.0f;
    m_hitData->m_endOffsetPos.m_z = 0.0f;
    m_hitData->m_size = 6.0f;
    m_hitData->m_shapeType = soCollision::Shape_Capsule;
    *m_hitSimple = *m_hitData;
    m_hitSimple->m_height = soCollisionHitData::Height_Low;
    m_hitSimple->m_nodeIndex = m_nodeIndex;
    soSet<soCollisionHitData::Simple>* set = m_hitSet;
    set->m_elements = m_hitSimple;
    set->m_size = 1;
    m_dataGroup->m_hitDataSimpleSet = m_hitSet;
    m_dataGroup->m_hitGroupIndex = 0;
    m_data->m_dataGroups = m_dataGroup;
    m_data->m_dataGroupNum = 1;

    ykInitInfo info = { 0, 0, 0x10, 0, 0 };
    info.m_ground = this;
    info.m_node = ykDynamicCastScnMdl(m_sceneModels[0]);
    Vec3f pos = getPos();
    info.m_pos = &pos;
    info.m_work = m_data;
    typedef ykNormal<soCollisionAttackModuleBuildConfigNull,
                     soCollisionHitModuleBuildConfig<soCollision::Category_Wall, 1, 1, soCollisionHitModuleImpl, 0x3FF, true> >
        Config;
    Config* yakumono = new (Heaps::StageInstance) Config(&info);
    yakumono->postInitialize();
    yakumono->activate(&pos, -1.0f, 0.0f);
    setYakumono(yakumono);
}

// A fighter hit the block: the stage shakes, and the third hit uses the block up.
void grFamicomPow::pow() {
    switch (m_state) {
    case 2:
        if (*m_stateWork != 8) {
            stFamicomData* data = static_cast<stFamicomData*>(getStageData());
            if (data != NULL) {
                *m_stateWork = 8;
                Vec3f quakePos;
                quakePos.m_x = 0.0f;
                quakePos.m_y = 0.0f;
                quakePos.m_z = 0.0f;
                cmReqQuake(cmQuake::Amplitude_Large, &quakePos);
                m_snd.playSE(static_cast<SndID>(0x1CE2), 0, 0, -1);
                m_hits++;
                if (m_hits >= 3) {
                    m_hits = 3;
                }
                if (m_hits == 3) {
                    m_timer = data->powBlockRespawnFrames;
                    setVisibility(0);
                    setEnableCollisionStatus(false);
                    disableHit(0, 0);
                    m_state = 3;
                }
            }
        }
        break;
    }
}

void grFamicomPow::onDamage(int index, soDamage* damage, soDamageAttackerInfo* attackerInfo) {
    pow();
}

void grFamicomPow::receiveCollMsg_Heading(grCollStatus* collStatus, grCollisionJoint* collisionJoint, bool isFirstContact) {
    static u32 sOwnerFlags = 1;
    if (isCollisionStatusOwnerTask(collStatus, reinterpret_cast<CollCategoryFlag*>(&sOwnerFlags)) &&
        collStatus->m_unk9 == 2) {
        Fighter* fighter = static_cast<Fighter*>(gfTask::getTask(collStatus->m_taskId));
        if (fighter != NULL) {
            int playerNo = g_ftManager->getPlayerNo(fighter->m_entryId);
            if (playerNo >= 0 && playerNo <= 7) {
                if (m_cooldown[playerNo] == 0.0f) {
                    pow();
                }
                m_cooldown[playerNo] = 5.0f;
            }
        }
    }
}
