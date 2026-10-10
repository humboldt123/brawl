#define YK_STAGE_INLINE // inline members of the shared SO headers that the stage RELs emit themselves
#define YK_STAGE_FULL // ... and the ones of the hit objects with an attack module and an event observer
#define YK_DATA_INLINE_CTOR // the ykData / ykDataGroup constructors are inline in this REL
#define SO_COLLISION_GROUP_ALIGNED // the hit object embeds soArrayVector<soCollisionGroup, 1> with 4-byte aligned elements
#define SO_COLLISION_GROUP_MEMBERWISE // ... and copies the group member by member

#include <ef/ef_id.h>
#include <memory.h>
#include <mt/mt_trig.h>
#include <types.h>
#include <yk/yk_normal.h>

#include <st_jungle/gr_jungle.h>
#include <st_jungle/gr_jungle_anim.h>

// (inline: the original has no constructor of its own)
inline grJungleAshiba02::grJungleAshiba02(const char* taskName) : grJungleAshiba(taskName), m_snd() {
    m_stateWork = NULL;
    m_scale.m_x = 1.0f;
    m_scale.m_y = 1.0f;
    m_scale.m_z = 1.0f;
    m_scaleRate = 1.0f;
    m_nodeButton = 0;
    m_nodeTrap = 0;
}

grJungleAshiba02* grJungleAshiba02::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grJungleAshiba02* ground = new (Heaps::StageInstance) grJungleAshiba02(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grJungleAshiba02::~grJungleAshiba02() {
    if (m_hitData != NULL) {
        delete m_hitData;
    }
    m_hitData = NULL;
    if (m_hitSimple != NULL) {
        delete m_hitSimple;
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

void grJungleAshiba02::processAnim() {
    Ground::processAnim();
    if (m_isUpdate && m_posWork != NULL) {
        getNodePosition(m_posGimmickWork, 0, m_nodeTrap);
    }
}

void grJungleAshiba02::update(float deltaFrame) {
    grJungleAshiba::update(deltaFrame);
    if (m_isUpdate) {
        updateYakumono(deltaFrame);
        updateActive(deltaFrame);
    }
}

void grJungleAshiba02::updateYakumono(float deltaFrame) {
    if (m_yakumonoMade == 1) {
        setPos(m_posWork->m_x, m_posWork->m_y, m_posWork->m_z);
    } else {
        setHit();
        if (m_yakumono != NULL) {
            m_yakumonoMade = 1;
            getNodeScale(&m_scale, 0, m_nodeButton);
        }
    }
}

// The state of the button of the trap: when the trap is hit the stage state goes to 1, the trap folds away (the scale of the
// node goes from 1 to 0 in six frames) and, when the stage clears the state again, comes back.
void grJungleAshiba02::updateActive(float deltaFrame) {
    m_timer -= deltaFrame;
    if (m_timer < 0.0f) {
        m_timer = 0.0f;
    }
    switch (m_state) {
    case 0:
        m_state = 1;
        break;
    case 1:
        if (*m_stateWork != 0) {
            m_state = 3;
            m_timer = 6.0f;
        }
        break;
    case 3: {
        float c = grJungleClamp(1.0f - m_timer / 6.0f, 0.001f, 1.0f);
        m_scaleRate = 1.0f - c;
        if (c == 1.0f) {
            m_state = 4;
        }
        break;
    }
    case 4:
        if (*m_stateWork == 0) {
            m_state = 5;
            m_timer = 6.0f;
        }
        break;
    case 5: {
        float c = grJungleClamp(1.0f - m_timer / 6.0f, 0.001f, 1.0f);
        m_scaleRate = c;
        if (c == 1.0f) {
            m_state = 0;
        }
        break;
    }
    }
}

void grJungleAshiba02::updateCallBack(float deltaFrame) {
    grJungleAshiba::updateCallBack(deltaFrame);
    grCalcWorldCallBack* calcWorldCallBack = &m_calcWorldCallBack;
    if (calcWorldCallBack != NULL) {
        grNodeCallbackData* data = calcWorldCallBack->m_nodeCallbackDatas;
        data[1].m_flags |= 4;
        data[1].m_nodeIndex = m_nodeButton;
        data[1].m_scale.m_x = m_scale.m_x;
        data[1].m_scale.m_y = m_scale.m_y;
        data[1].m_scale.m_z = m_scale.m_z;
        data[1].m_scale.m_y = data[1].m_scale.m_y * m_scaleRate;
        Vec3f pos;
        getNodePosition(&pos, 0, m_nodeButton);
        m_snd.setPos(&pos);
    }
}

bool grJungleAshiba02::setNode() {
    bool result = grGimmick::setNode();
    getNodeIndex(&m_nodeButton, 0, "L2ButtonClick01");
    getNodeIndex(&m_nodeTrap, 0, "L2trapN");
    return result;
}

// The hit object of the button: one hit sphere (the button is pushed when something hits it).
void grJungleAshiba02::setHit() {
    // MATCH-ONLY: the original allocates the hit data as raw storage (no element construction)
    m_hitData = new (Heaps::StageInstance) soCollisionHitData;
    m_hitSimple = new (Heaps::StageInstance) soCollisionHitData::Simple;
    m_hitSet = reinterpret_cast<soSet<soCollisionHitData>*>(new (Heaps::StageInstance) grJungleSetView);
    m_dataGroup = new (Heaps::StageInstance) ykDataGroup;
    m_data = new (Heaps::StageInstance) ykData;

    m_hitData->m_startOffsetPos.m_x = 0.0f;
    m_hitData->m_startOffsetPos.m_y = 0.0f;
    m_hitData->m_startOffsetPos.m_z = 0.0f;
    m_hitData->m_endOffsetPos.m_x = 0.0f;
    m_hitData->m_endOffsetPos.m_y = 0.0f;
    m_hitData->m_endOffsetPos.m_z = 0.0f;
    m_hitData->m_size = 4.5f;
    // MATCH-ONLY: the shape type is the top bit of the byte at 0x1c (it is the whole byte of the simple form)
    reinterpret_cast<grJungleHitByte*>(m_hitData)->m_shape = 1;
    grJungleCopyHit(m_hitSimple, m_hitData);
    m_hitSimple->m_height = soCollisionHitData::Height_Low;
    m_hitSimple->m_nodeIndex = m_nodeButton;

    // MATCH-ONLY: the members of soSet are private
    grJungleSetView* set = reinterpret_cast<grJungleSetView*>(m_hitSet);
    set->m_elements = m_hitSimple;
    set->m_size = 1;
    m_dataGroup->m_hitDataSimpleSet = reinterpret_cast<soSet<soCollisionHitData::Simple>*>(m_hitSet);
    m_dataGroup->m_hitGroupIndex = 0;
    m_data->m_dataGroups = m_dataGroup;
    m_data->m_dataGroupNum = 1;

    ykInitInfo info = { 0, 0, 0x10, 0, 0 };
    info.m_ground = this;
    info.m_node = ykDynamicCastScnMdl(m_sceneModels[0]);
    Vec3f pos = getPos();
    info.m_pos = &pos;
    info.m_work = m_data;
    typedef ykNormal<soCollisionAttackModuleBuildConfig<soCollision::Category_Gimmick, 1, 0, soCollisionAttackModuleImpl, 1, false, true>,
                     soCollisionHitModuleBuildConfig<soCollision::Category_Gimmick, 1, 1, soCollisionHitModuleImpl, 0x3FF, true> >
        Config;
    Config* yakumono = new (Heaps::StageInstance) Config(&info);
    yakumono->postInitialize();
    yakumono->activate(&pos, -1.0f, 0.0f);
    setYakumono(yakumono);
}

void grJungleAshiba02::onDamage(int index, soDamage* damage, soDamageAttackerInfo* attackerInfo) {
    if (m_stateWork != NULL) {
        fn_27_26399C(m_yakumono);
        *m_stateWork = 1;
        disableHit(0, 0);
        m_snd.playSE(static_cast<SndID>(0x1B8C), 0, 0, -1);
        m_snd.playSE(static_cast<SndID>(0x1B8D), 0, 0, -1);
    }
}

void grJungleAshiba02::setEnableYakumono() {
    if (m_stateWork != NULL && *m_stateWork == 0 && m_state == 1) {
        enableHit(0, 0);
    }
}

void grJungleAshiba02::setDisableYakumono() {
    disableHit(0, 0);
}

void grJungleAshiba02::setStateWork(u8* stateWork) {
    m_stateWork = stateWork;
}

int grJungleAshiba02::getCallBackNodeCount() {
    return 2;
}
