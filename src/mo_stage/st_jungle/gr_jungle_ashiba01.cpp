#define YK_STAGE_INLINE // inline members of the shared SO headers that the stage RELs emit themselves
#define YK_STAGE_FULL // ... and the ones of the hit objects with an attack module and an event observer
#define YK_DATA_INLINE_CTOR // the ykData / ykDataGroup constructors are inline in this REL
#define SO_COLLISION_GROUP_ALIGNED // the hit object embeds soArrayVector<soCollisionGroup, 1> with 4-byte aligned elements
#define SO_COLLISION_GROUP_MEMBERWISE // ... and copies the group member by member

#include <memory.h>
#include <mt/mt_trig.h>
#include <types.h>
#include <yk/yk_normal.h>

#include <st_jungle/gr_jungle.h>

// (inline: the original has no constructor of its own)
inline grJungleAshiba01::grJungleAshiba01(const char* taskName) : grJungleAshiba(taskName) {
    m_attackOn = 0;
    m_node = 0;
}

grJungleAshiba01* grJungleAshiba01::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grJungleAshiba01* ground = new (Heaps::StageInstance) grJungleAshiba01(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grJungleAshiba01::~grJungleAshiba01() {
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

void grJungleAshiba01::update(float deltaFrame) {
    grJungleAshiba::update(deltaFrame);
    if (m_isUpdate) {
        updateYakumono(deltaFrame);
    }
}

void grJungleAshiba01::updateYakumono(float deltaFrame) {
    if (m_yakumonoMade == 1) {
        setPos(m_posWork->m_x, m_posWork->m_y, m_posWork->m_z);
    } else {
        setHit();
        if (m_yakumono != NULL) {
            m_yakumonoMade = 1;
        }
    }
}

bool grJungleAshiba01::setNode() {
    bool result = grGimmick::setNode();
    getNodeIndex(&m_node, 0, "L1togetoge");
    return result;
}

// The hit object of the spikes: one hit sphere and the attack that hurts.
void grJungleAshiba01::setHit() {
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
    m_hitData->m_size = 1.0f;
    // MATCH-ONLY: the shape type is the top bit of the byte at 0x1c (it is the whole byte of the simple form)
    reinterpret_cast<grJungleHitByte*>(m_hitData)->m_shape = 1;
    grJungleCopyHit(m_hitSimple, m_hitData);
    m_hitSimple->m_height = soCollisionHitData::Height_Low;
    m_hitSimple->m_nodeIndex = m_node;

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

// The spikes hurt: 15 percent, cuts, knocks fighters away downwards.
void grJungleAshiba01::setAttack() {
    if (m_yakumono != NULL) {
        soCollisionAttackData attack(1.0f);
        Vec3f offset;
        offset.m_x = 0.0f;
        offset.m_y = -2.5f;
        offset.m_z = 0.0f;
        setAttackGimmickDetails(&attack, 3.0f, 1.0f, 1.0f, 1.0f, 15, &offset, -90, 100, 0, 200, m_node, 0x3FF, 7, false, 15,
                                soCollisionAttackData::Attribute_Cutup, soCollisionAttackData::Sound_Level_Small,
                                soCollisionAttackData::Sound_Attribute_Cutup, false, false, false, true, false, false, 0, 60, false,
                                false, false, soCollisionAttackData::Lr_Check_Pos, false, false, false, false, false,
                                soCollisionAttackData::Region_None, true);
        m_yakumono->setAttack(0, 0, &attack);
        m_attackOn = 1;
    }
}

void grJungleAshiba01::setEnableYakumono() {
    enableHit(0, 0);
    if (m_attackOn == 0) {
        setAttack();
    }
}

void grJungleAshiba01::setDisableYakumono() {
    disableHit(0, 0);
    if (m_attackOn == 1) {
        disableAttack(0);
    }
    m_attackOn = 0;
}
