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
inline grJungleAshiba03::grJungleAshiba03(const char* taskName) : grJungleAshiba(taskName), m_snd() {
    m_stateWork = NULL;
    m_scale.m_x = 1.0f;
    m_scale.m_y = 1.0f;
    m_scale.m_z = 1.0f;
    m_scaleRate = 1.0f;
    m_node[0] = 0;
    m_node[1] = 0;
    m_node[2] = 0;
    m_node[3] = 0;
}

grJungleAshiba03* grJungleAshiba03::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grJungleAshiba03* ground = new (Heaps::StageInstance) grJungleAshiba03(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grJungleAshiba03::~grJungleAshiba03() {
    if (m_hitData != NULL) {
        delete[] m_hitData;
    }
    m_hitData = NULL;
    if (m_hitSimple != NULL) {
        delete[] m_hitSimple;
    }
    m_hitSimple = NULL;
    if (m_hitSet != NULL) {
        delete[] reinterpret_cast<grJungleSetView*>(m_hitSet);
    }
    m_hitSet = NULL;
    if (m_dataGroup != NULL) {
        delete[] m_dataGroup;
    }
    m_dataGroup = NULL;
    if (m_data != NULL) {
        delete m_data;
    }
    m_data = NULL;
}

void grJungleAshiba03::processAnim() {
    Ground::processAnim();
    if (m_isUpdate && m_posWork != NULL) {
        getNodePosition(m_posGimmickWork, 0, m_node[2]);
        getNodePosition(&m_posGimmickWork[1], 0, m_node[3]);
        getNodePosition(&m_posGimmickWork[2], 0, m_node[1]);
    }
}

void grJungleAshiba03::update(float deltaFrame) {
    grJungleAshiba::update(deltaFrame);
    if (m_isUpdate) {
        updateYakumono(deltaFrame);
        updateActive(deltaFrame);
    }
}

void grJungleAshiba03::updateYakumono(float deltaFrame) {
    if (m_yakumonoMade == 1) {
        setPos(m_posWork->m_x, m_posWork->m_y, m_posWork->m_z);
    } else {
        setHit();
        if (m_yakumono != NULL) {
            m_yakumonoMade = 1;
            getNodeScale(&m_scale, 0, m_node[0]);
        }
    }
}

// The state of the button of the trap: when the trap is hit the stage state goes to 1, the trap folds away (the scale of the
// node goes from 1 to 0 in six frames) and, when the stage clears the state again, comes back.
void grJungleAshiba03::updateActive(float deltaFrame) {
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

void grJungleAshiba03::updateCallBack(float deltaFrame) {
    grJungleAshiba::updateCallBack(deltaFrame);
    grCalcWorldCallBack* calcWorldCallBack = &m_calcWorldCallBack;
    if (calcWorldCallBack != NULL) {
        grNodeCallbackData* data = calcWorldCallBack->m_nodeCallbackDatas;
        data[1].m_flags |= 4;
        data[1].m_nodeIndex = m_node[0];
        data[1].m_scale.m_x = m_scale.m_x;
        data[1].m_scale.m_y = m_scale.m_y;
        data[1].m_scale.m_z = m_scale.m_z;
        data[1].m_scale.m_y = data[1].m_scale.m_y * m_scaleRate;
        Vec3f pos;
        getNodePosition(&pos, 0, m_node[0]);
        m_snd.setPos(&pos);
    }
}

bool grJungleAshiba03::setNode() {
    bool result = grGimmick::setNode();
    getNodeIndex(&m_node[0], 0, "L3ButtonClick");
    getNodeIndex(&m_node[1], 0, "L3togetoge");
    getNodeIndex(&m_node[2], 0, "L3moveN");
    getNodeIndex(&m_node[3], 0, "L3stoneN");
    return result;
}

// The hit objects of the platform: two hit spheres (the button and the spikes); there is no attack.
void grJungleAshiba03::setHit() {
    // MATCH-ONLY: the original allocates the hit data as raw storage (no element construction)
    m_hitData = new (Heaps::StageInstance) soCollisionHitData[2];
    m_hitSimple = new (Heaps::StageInstance) soCollisionHitData::Simple[2];
    m_hitSet = reinterpret_cast<soSet<soCollisionHitData>*>(new (Heaps::StageInstance) grJungleSetView[2]);
    m_dataGroup = new (Heaps::StageInstance) ykDataGroup[2];
    m_data = new (Heaps::StageInstance) ykData;

    // MATCH-ONLY: the members of soSet are private
    grJungleSetView* sets = reinterpret_cast<grJungleSetView*>(m_hitSet);
    for (int i = 0; i < 2; i++) {
        m_hitData[i].m_startOffsetPos.m_x = 0.0f;
        m_hitData[i].m_startOffsetPos.m_y = 0.0f;
        m_hitData[i].m_startOffsetPos.m_z = 0.0f;
        m_hitData[i].m_endOffsetPos.m_x = 0.0f;
        m_hitData[i].m_endOffsetPos.m_y = 0.0f;
        m_hitData[i].m_endOffsetPos.m_z = 0.0f;
        m_hitData[i].m_size = 4.5f;
        // MATCH-ONLY: the shape type is the top bit of the byte at 0x1c (it is the whole byte of the simple form)
        reinterpret_cast<grJungleHitByte*>(&m_hitData[i])->m_shape = 1;
        grJungleCopyHit(&m_hitSimple[i], &m_hitData[i]);
        m_hitSimple[i].m_height = soCollisionHitData::Height_Low;
        m_hitSimple[i].m_nodeIndex = m_node[i];
        sets[i].m_elements = &m_hitSimple[i];
        sets[i].m_size = 1;
        m_dataGroup[i].m_hitDataSimpleSet = reinterpret_cast<soSet<soCollisionHitData::Simple>*>(&sets[i]);
        m_dataGroup[i].m_hitGroupIndex = i;
    }
    m_data->m_dataGroups = m_dataGroup;
    m_data->m_dataGroupNum = 2;

    ykInitInfo info = { 0, 0, 0x10, 0, 0 };
    info.m_ground = this;
    info.m_node = ykDynamicCastScnMdl(m_sceneModels[0]);
    Vec3f pos = getPos();
    info.m_pos = &pos;
    info.m_work = m_data;
    typedef ykNormal<soCollisionAttackModuleBuildConfigNull,
                     soCollisionHitModuleBuildConfig<soCollision::Category_Gimmick, 2, 2, soCollisionHitModuleImpl, 0x3FF, true> >
        Config;
    Config* yakumono = new (Heaps::StageInstance) Config(&info);
    yakumono->postInitialize();
    yakumono->activate(&pos, -1.0f, 0.0f);
    setYakumono(yakumono);
}

void grJungleAshiba03::resetYakumono() {
    if (m_stateWork != NULL) {
        if (m_stateWork[1] >= 2) {
            m_stateWork[1] = 4;
        }
    }
}

void grJungleAshiba03::onDamage(int index, soDamage* damage, soDamageAttackerInfo* attackerInfo) {
    if (m_stateWork != NULL) {
        fn_27_26399C(m_yakumono);
        if (index == 0) {
            *m_stateWork = 1;
            disableHit(0, 0);
            m_snd.playSE(static_cast<SndID>(0x1B8C), 0, 0, -1);
        }
    }
}

void grJungleAshiba03::setEnableYakumono() {
    if (m_stateWork != NULL) {
        if (*m_stateWork == 0 && m_state == 1) {
            enableHit(0, 0);
        }
        enableHit(1, 0);
    }
}

void grJungleAshiba03::setDisableYakumono() {
    disableHit(0, 0);
    disableHit(1, 0);
}

void grJungleAshiba03::setStateWork(u8* stateWork) {
    m_stateWork = stateWork;
}

int grJungleAshiba03::getCallBackNodeCount() {
    return 2;
}
