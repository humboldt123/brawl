#define YK_STAGE_INLINE // inline members of the shared SO headers that the stage RELs emit themselves
#define YK_STAGE_FULL // ... and the ones of the hit objects with an attack module and an event observer
#define SO_COLLISION_GROUP_ALIGNED // the hit object embeds soArrayVector<soCollisionGroup, 1> with 4-byte aligned elements
#define SO_COLLISION_GROUP_MEMBERWISE // ... and copies the group member by member

#include <memory.h>
#include <types.h>
#include <yk/yk_no_hit_normal.h>

#include <st_jungle/gr_jungle.h>

grJungleAttack::grJungleAttack(const char* taskName) : grJungle(taskName) {
    m_posWork = NULL;
    m_posLimitWork = NULL;
    m_enableWork = NULL;
    m_offset.m_x = 0.0f;
    m_offset.m_y = 0.0f;
    m_offset.m_z = 0.0f;
    m_yakumonoMade = 0;
    m_attackOn = 0;
    m_work = NULL;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback != NULL) {
        callback->m_numNodeCallbackData = 1;
        callback->initialize(false, Heaps::StageInstance);
        callback->m_nodeCallbackDatas->m_flags |= 1;
        callback->m_nodeCallbackDatas->m_flags |= 0x10;
    }
}

void grJungleAttack::update(float deltaFrame) {
    grJungle::update(deltaFrame);
    if (m_isUpdate) {
        updateYakumono(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

// The attack is on while the stage says so (the state of the enable flag is 1) and is taken away when it is 0.
void grJungleAttack::updateYakumono(float deltaFrame) {
    if (m_yakumonoMade == 1) {
        switch (*m_enableWork) {
        case 0:
            if (m_attackOn == 1) {
                disableAttack(0);
            }
            m_attackOn = 0;
            break;
        case 1:
            setAttack();
            break;
        }
    } else {
        setHit();
        if (m_yakumono != NULL) {
            m_yakumonoMade = 1;
        }
    }
}

void grJungleAttack::updateCallBack(float deltaFrame) {
    grCalcWorldCallBack* calcWorldCallBack = &m_calcWorldCallBack;
    if (calcWorldCallBack != NULL) {
        nw4r::g3d::ScnMdl* scnMdl = *m_sceneModels;
        if (scnMdl != NULL) {
            if (scnMdl->m_calcWorldCallBack == NULL) {
                calcWorldCallBack->m_index = 0;
                calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex = m_nodeIndex;
                scnMdl->m_calcWorldCallBack = calcWorldCallBack;
                scnMdl->EnableScnMdlCallbackTiming(1);
                scnMdl->m_nodeIndex = calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex;
            }
            grNodeCallbackData* data = calcWorldCallBack->m_nodeCallbackDatas;
            data->m_pos.m_x = m_posWork->m_x;
            data->m_pos.m_y = m_posWork->m_y;
            data->m_pos.m_z = m_posWork->m_z;
            data = calcWorldCallBack->m_nodeCallbackDatas;
            data->m_offsetPos.m_x = m_offset.m_x;
            data->m_offsetPos.m_y = m_offset.m_y;
            data->m_offsetPos.m_z = m_offset.m_z;
        }
    }
}

// The hit object has an attack and no hit spheres.
void grJungleAttack::setHit() {
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

void grJungleAttack::setAttack() {
}

// (inline: the original has no constructor of its own)
inline grJungleAttack03::grJungleAttack03(const char* taskName) : grJungleAttack(taskName) {
}

grJungleAttack03* grJungleAttack03::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grJungleAttack03* ground = new (Heaps::StageInstance) grJungleAttack03(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grJungleAttack03::~grJungleAttack03() {
}

// The attack of the spikes: 6 percent, cuts, knocks fighters away to the right.
void grJungleAttack03::setAttack() {
    if (m_yakumono != NULL) {
        soCollisionAttackData attack(1.0f);
        Vec3f offset;
        offset.m_x = 2.5f;
        offset.m_y = -2.5f;
        offset.m_z = 0.0f;
        setAttackGimmickDetails(&attack, 6.0f, 1.0f, 1.0f, 1.0f, 15, &offset, 315, 100, 0, 200, 0, 0x3FF, 7, false, 15,
                                soCollisionAttackData::Attribute_Cutup, soCollisionAttackData::Sound_Level_Medium,
                                soCollisionAttackData::Sound_Attribute_Cutup, false, false, false, true, false, false, 0, 60, false,
                                false, false, soCollisionAttackData::Lr_Check_Forward, false, false, false, false, false,
                                soCollisionAttackData::Region_None, false);
        m_yakumono->setLr(1.0f);
        m_yakumono->setAttack(0, 0, &attack);
        m_attackOn = 1;
    }
}

// (inline: the original has no constructor of its own)
inline grJungleAttack08::grJungleAttack08(const char* taskName) : grJungleAttack(taskName) {
    m_offset.m_x = -12.5f;
    m_offset.m_y = 0.0f;
    m_offset.m_z = 0.0f;
}

grJungleAttack08* grJungleAttack08::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grJungleAttack08* ground = new (Heaps::StageInstance) grJungleAttack08(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grJungleAttack08::~grJungleAttack08() {
}

// The attack of the stone: 10 percent, cuts, knocks fighters away upwards.
void grJungleAttack08::setAttack() {
    if (m_yakumono != NULL) {
        soCollisionAttackData attack(1.0f);
        Vec3f offset;
        offset.m_x = 25.0f;
        offset.m_y = 0.0f;
        offset.m_z = 0.0f;
        setAttackGimmickDetails(&attack, 3.5f, 1.0f, 1.0f, 1.0f, 10, &offset, 90, 80, 0, 80, 0, 0x3FF, 7, false, 15,
                                soCollisionAttackData::Attribute_Cutup, soCollisionAttackData::Sound_Level_Small,
                                soCollisionAttackData::Sound_Attribute_Cutup, false, false, false, true, false, false, 0, 60, false,
                                false, false, soCollisionAttackData::Lr_Check_Pos, false, false, false, false, false,
                                soCollisionAttackData::Region_None, true);
        m_yakumono->setAttack(0, 0, &attack);
        m_attackOn = 1;
    }
}

