#define YK_STAGE_INLINE // inline members of the shared SO headers that the stage RELs emit themselves
#define SO_COLLISION_GROUP_ALIGNED // the hit object embeds soArrayVector<soCollisionGroup, 1> with 4-byte aligned elements

#include <gf/gf_model.h>
#include <gr/collision/gr_collision.h>
#include <gr/collision/gr_collision_joint.h>
#include <gr/gr_calc_world_callback.h>
#include <memory.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <types.h>
#include <yk/yk_no_hit_normal.h>

#include <st_metalgear/gr_metalgear.h>

// MATCH-ONLY: the flag word of a collision joint (the stage sets the collision kind in its second byte)
struct metalgearJointView {
    u8 _pad[0x48];
    u32 _unused0 : 8;
    u32 m_kind : 8;
    u32 _unused1 : 16;
};

grMetalgearAttack::grMetalgearAttack(const char* taskName) : grMetalgear(taskName) {
    m_posWork = NULL;
    m_posLimitWork = NULL;
    m_rotWork = NULL;
    m_pos.m_x = 0.0f;
    m_pos.m_y = 0.0f;
    m_pos.m_z = 0.0f;
    m_stateWork = NULL;
    m_type = 9;
    m_hasYakumono = 0;
    m_attackEnabled = 0;
    m_work = NULL;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_numNodeCallbackData = 1;
    callback->initialize(false, Heaps::StageInstance);
    callback->m_nodeCallbackDatas[0].m_flags |= 1;
}

grMetalgearAttack* grMetalgearAttack::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grMetalgearAttack* ground = new (Heaps::StageInstance) grMetalgearAttack(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grMetalgearAttack::~grMetalgearAttack() {
    if (m_work != NULL) {
        delete m_work;
    }
    m_work = NULL;
}

void grMetalgearAttack::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateYakumono(deltaFrame);
        updateCollision(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

void grMetalgearAttack::updateYakumono(float deltaFrame) {
    if (m_hasYakumono != 1) {
        setHit();
        if (m_yakumono != NULL) {
            m_hasYakumono = 1;
        }
    }
}

// The collision line of the wall tower follows the tower while it breaks (the wall gives how far it is) and the attack is on
// while the tower is in its movement.
void grMetalgearAttack::updateCollision(float deltaFrame) {
    if (m_collision != NULL) {
        grCollisionJoint* joint = m_collision->getJoint(0);
        if (joint != NULL) {
            grCollisionLine* line = joint->getLine(0);
            if (line != NULL) {
                reinterpret_cast<metalgearJointView*>(joint)->m_kind = 3;
                grCollData::VtxData* vtxA = &joint->m_vtxDatas[(u16)line->m_point0Index];
                grCollData::VtxData* vtxB = &joint->m_vtxDatas[(u16)(line->m_point0Index + 1)];
                if (m_type == 8) {
                    float x = m_posWork[0].m_x;
                    x = x + (1.0f - m_posWork[0].m_z) * (m_posLimitWork[1].m_x - x);
                    m_pos.m_x = x;
                    vtxA->m_pos.m_x = x;
                    vtxA->m_pos.m_y = m_posLimitWork[1].m_z;
                    vtxB->m_pos.m_x = m_pos.m_x;
                    vtxB->m_pos.m_y = m_posLimitWork[0].m_y;
                } else if (m_type < 8 && 6 < m_type) {
                    float x = m_posWork[0].m_x;
                    x = x - (1.0f - m_posWork[0].m_z) * (x - m_posLimitWork[0].m_x);
                    m_pos.m_x = x;
                    vtxA->m_pos.m_x = x;
                    vtxA->m_pos.m_y = m_posLimitWork[0].m_y;
                    vtxB->m_pos.m_x = m_pos.m_x;
                    vtxB->m_pos.m_y = m_posLimitWork[1].m_z;
                }
                bool active;
                if (m_posWork[0].m_z == 0.0f) {
                    active = false;
                } else {
                    active = m_posWork[0].m_y != 1.0f;
                }
                if (active != m_isEnableCollisionStatus) {
                    if (active == true) {
                        setEnableCollisionStatus(true);
                    } else {
                        setEnableCollisionStatus(false);
                    }
                }
            }
        }
    }
}

void grMetalgearAttack::updateCallBack(float deltaFrame) {
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
            calcWorldCallBack->m_nodeCallbackDatas[0].m_pos.m_x = m_pos.m_x;
            calcWorldCallBack->m_nodeCallbackDatas[0].m_pos.m_y = 0.0f;
            calcWorldCallBack->m_nodeCallbackDatas[0].m_pos.m_z = 0.0f;
            calcWorldCallBack->m_nodeCallbackDatas[0].m_rot.m_z = *m_rotWork;
        }
    }
}

// The attack's hit object: one attack part and one collision group (the Yakumono has no hit module).
void grMetalgearAttack::setHit() {
    m_work = new (Heaps::StageInstance) u32[2];
    m_work[0] = 0;
    m_work[1] = 0;

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

// A hit box that hurts the fighters a bit (power 0, knockback 135 degrees) while the roof is moving.
void grMetalgearAttack::setAttack() {
    if (m_attackEnabled != 1) {
        soCollisionAttackData attack(1.0f);
        Vec3f offset;
        offset.m_x = 0.0f;
        offset.m_y = 150.0f;
        offset.m_z = 0.0f;
        setAttackGimmickDetails(&attack, 17.0f, 1.0f, 1.0f, 1.0f,
            0, &offset, 135, 55, 100, 70, 0,
            0x3FF, 7, false, 15,
            soCollisionAttackData::Attribute_Normal, soCollisionAttackData::Sound_Level_Medium,
            soCollisionAttackData::Sound_Attribute_Punch,
            false, false, false, false, false, false, 0, 60,
            false, true, true, soCollisionAttackData::Lr_Check_Pos,
            false, true, true, true, true, soCollisionAttackData::Region_None, true);
        m_yakumono->setAttack(0, 0, &attack);
        m_attackEnabled = 1;
    }
}
