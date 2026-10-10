#define YK_STAGE_INLINE // inline members of the shared SO headers that the stage RELs emit themselves
#define YK_STAGE_FULL // ... and the ones of the hit objects with an attack module and an event observer
#define YK_DATA_INLINE_CTOR // the ykData / ykDataGroup constructors are inline in this REL
#define SO_COLLISION_GROUP_ALIGNED // the hit object embeds soArrayVector<soCollisionGroup, 1> with 4-byte aligned elements
#define SO_COLLISION_GROUP_MEMBERWISE // ... and copies the group member by member

#include <gr/gr_calc_world_callback.h>
#include <memory.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <types.h>
#include <yk/yk_no_hit_normal.h>

#include <st_pictchat/gr_pictchat.h>

// The attack of the spikes of the picture 7 (one for each spike): a sphere on the node of the ground.
grPictchatAttack::grPictchatAttack(const char* taskName) : grPictchat(taskName) {
    m_posWork = NULL;
    m_stateWork = NULL;
    m_pictIDWork = NULL;
    m_pictID = 0;
    m_yakumonoSet = 0;
    m_attackOn = 0;
    m_data = NULL;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback != NULL) {
        callback->m_numNodeCallbackData = 1;
        callback->initialize(false, Heaps::StageInstance);
        callback->m_nodeCallbackDatas[0].m_flags |= 1;
    }
}

grPictchatAttack::~grPictchatAttack() {
    if (m_data != NULL) {
        delete m_data;
    }
    m_data = NULL;
}

void grPictchatAttack::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateYakumono(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

// The attack is on while the stage says that the spikes are (the state 4) and the picture of them is drawn.
void grPictchatAttack::updateYakumono(float deltaFrame) {
    if (m_yakumonoSet == 1) {
        if (*m_pictIDWork == m_pictID && *m_stateWork == 4) {
            setAttack();
        } else if (m_attackOn == 1) {
            disableAttack(0);
            m_attackOn = 0;
        }
    } else {
        setHit();
        if (m_yakumono != NULL) {
            m_yakumonoSet = 1;
        }
    }
}

// The place of the attack is the place the stage gives.
void grPictchatAttack::updateCallBack(float deltaFrame) {
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
            Vec3f* pos = m_posWork;
            grNodeCallbackData* data = calcWorldCallBack->m_nodeCallbackDatas;
            data->m_pos.m_x = pos->m_x;
            data->m_pos.m_y = pos->m_y;
            data->m_pos.m_z = pos->m_z;
        }
    }
}

// The hit object of the attack: one attack part and no hit module.
void grPictchatAttack::setHit() {
    m_data = new (Heaps::StageInstance) ykData;
    m_data->m_dataGroupNum = 0;
    m_data->m_dataGroups = NULL;
    ykInitInfo info = { 0, 0, 0x10, 0, 0 };
    info.m_ground = this;
    info.m_node = ykDynamicCastScnMdl(m_sceneModels[0]);
    Vec3f pos = getPos();
    info.m_pos = &pos;
    info.m_work = m_data;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Gimmick, 1, 0, soCollisionAttackModuleImpl, 1, false, true> Config;
    ykNoHitNormal<Config>* yakumono = new (Heaps::StageInstance) ykNoHitNormal<Config>(&info);
    setYakumono(yakumono);
}

void grPictchatAttack::setAttack() {
}

inline grPictchatAttack007::grPictchatAttack007(const char* taskName) : grPictchatAttack(taskName) {
    m_index = 0;
}

grPictchatAttack007* grPictchatAttack007::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grPictchatAttack007* ground = new (Heaps::StageInstance) grPictchatAttack007(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grPictchatAttack007::~grPictchatAttack007() {
}

// The spikes on the left (0 - 2) throw the fighters to the right and the ones on the right (3 - 5) to the left.
void grPictchatAttack007::setAttack() {
    if (m_yakumono != NULL) {
        soCollisionAttackData attack(1.0f);
        Vec3f offset;
        int vector;
        switch (m_index) {
        case 0:
            vector = 0;
            offset.m_y = 0.0f;
            offset.m_x = 20.0f;
            offset.m_z = 0.0f;
            break;
        case 1:
            vector = 0;
            offset.m_y = 0.0f;
            offset.m_x = 20.0f;
            offset.m_z = 0.0f;
            break;
        case 2:
            vector = 0;
            offset.m_y = 0.0f;
            offset.m_x = 20.0f;
            offset.m_z = 0.0f;
            break;
        case 3:
            vector = 180;
            offset.m_y = 0.0f;
            offset.m_x = -20.0f;
            offset.m_z = 0.0f;
            break;
        case 4:
            vector = 180;
            offset.m_y = 0.0f;
            offset.m_x = -20.0f;
            offset.m_z = 0.0f;
            break;
        case 5:
            vector = 180;
            offset.m_y = 0.0f;
            offset.m_x = -20.0f;
            offset.m_z = 0.0f;
            break;
        default:
            vector = 0;
            offset.m_y = 0.0f;
            offset.m_x = 0.0f;
            offset.m_z = 0.0f;
            break;
        }
        setAttackGimmickDetails(&attack, 7.0f, 1.0f, 1.0f, 1.0f, 20, &offset, vector, 100, 70, 70, m_nodeIndex, 0x3FF, 7, false, 15,
                                soCollisionAttackData::Attribute_Cutup, soCollisionAttackData::Sound_Level_Small,
                                soCollisionAttackData::Sound_Attribute_Cutup, false, false, false, true, false, false, 0, 60, false,
                                false, false, soCollisionAttackData::Lr_Check_Forward, false, false, false, false, false,
                                soCollisionAttackData::Region_None, true);
        m_yakumono->setAttack(0, 0, &attack);
        m_attackOn = 1;
    }
}
