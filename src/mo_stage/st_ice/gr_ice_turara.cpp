#define YK_STAGE_INLINE // inline members of the shared SO headers that the stage RELs emit themselves
#define YK_STAGE_FULL // ... and the ones of the hit objects with an attack module and an event observer
#define YK_DATA_INLINE_CTOR // the ykData / ykDataGroup constructors are inline in this REL
#define SO_COLLISION_GROUP_ALIGNED // the hit object embeds soArrayVector<soCollisionGroup, 1> with 4-byte aligned elements
#define SO_COLLISION_GROUP_MEMBERWISE // ... and copies the group member by member

#include <ec/ec_mgr.h>
#include <gf/gf_model.h>
#include <gr/gr_calc_world_callback.h>
#include <memory.h>
#include <mt/mt_prng.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <snd/snd_system.h>
#include <types.h>
#include <yk/yk_normal.h>

#include <st_ice/gr_ice.h>
#include <st_ice/gr_ice_hit.h>

// Yakumono::getDamage (sora_melee, unnamed)
extern "C" void fn_27_26399C(Yakumono* yakumono);

// The icicle of the stage is shown, and its hit sphere is on, while the stage says the state of it (8 - 10, one for each
// of the three).
#define ICE_TURARA_IS_MINE(state) \
    ((state == 8 && m_type == 0) || (state == 9 && m_type == 1) || (state == 10 && m_type == 2))

grIceTurara::grIceTurara(const char* taskName) : grIce(taskName), m_snd() {
    m_mtxWork = NULL;
    m_posLimitWork = NULL;
    m_stateWork = NULL;
    m_pos.m_x = 0.0f;
    m_pos.m_y = 0.0f;
    m_pos.m_z = 0.0f;
    m_rot.m_x = 0.0f;
    m_rot.m_y = 0.0f;
    m_rot.m_z = 0.0f;
    m_type = 3;
    m_hpWork = NULL;
    m_fallSpeed = 0.0f;
    m_hitTimer = 0.0f;
    m_hasHit = 0;
    m_attackSet = 0;
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
    callback->m_nodeCallbackDatas[0].m_flags |= 2;
}

grIceTurara* grIceTurara::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grIceTurara* ground = new (Heaps::StageInstance) grIceTurara(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grIceTurara::~grIceTurara() {
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

void grIceTurara::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateYakumono(deltaFrame);
        updateActive(deltaFrame);
        updateCallBack(deltaFrame);
        m_hasUpdatedG3dCalcWorld = false;
        updateG3dProcCalcWorld();
    }
}

// The hit object is made the first time this is called.
void grIceTurara::updateYakumono(float deltaFrame) {
    if (m_hasHit != 1) {
        setHit();
        if (m_yakumono != NULL) {
            m_hasHit = 1;
        }
    }
}

// The icicle is hit: the hit points of the icicles go down by the damage (they are shared by the three), and a shard flies
// off the place that was hit.
void grIceTurara::onDamage(int index, soDamage* damage, soDamageAttackerInfo* attackerInfo) {
    stIceData* data = static_cast<stIceData*>(getStageData());
    if (data != NULL) {
        float amount = damage->unk4;
        fn_27_26399C(m_yakumono);
        *m_hpWork = *m_hpWork - amount;
        if (*m_hpWork < 0.0f) {
            *m_hpWork = 0.0f;
        }
        if (m_hitTimer == 0.0f) {
            u32 effect = g_ecMgr->setEffect(static_cast<EfID>(0x480005));
            if (*m_stateWork == 8 && m_type == 0) {
                g_ecMgr->setParent(effect, m_sceneModels[0], "Turara1", 0);
            }
            if (*m_stateWork == 9 && m_type == 1) {
                g_ecMgr->setParent(effect, m_sceneModels[0], "Turara2", 0);
            }
            if (*m_stateWork == 10 && m_type == 2) {
                g_ecMgr->setParent(effect, m_sceneModels[0], "Turara3", 0);
            }
            m_hitTimer = randf() * 5.0f + 10.0f;
        }
    }
}

// The icicle waits (hidden), is shown when the stage says its state, falls (the state 0x13) and goes away below the
// bottom of the screen. The icicle with the type 2 is the one that hurts a fighter it falls on.
void grIceTurara::updateActive(float deltaFrame) {
    float timer = m_timer - deltaFrame;
    m_timer = timer;
    if (timer < 0.0f) {
        m_timer = 0.0f;
    }
    timer = m_hitTimer - deltaFrame;
    m_hitTimer = timer;
    if (timer < 0.0f) {
        m_hitTimer = 0.0f;
    }
    u8 state = m_state;
    if (state == 7) {
        u8 stage = *m_stateWork;
        if (stage == 0xB) {
            m_fallSpeed = 0.0f;
            disableHit(0, 0);
            if (m_type == 2) {
                setAttack();
                m_snd.playSE(static_cast<SndID>(0x1C8A), 0, 0, -1);
            }
            m_state = 0x13;
        } else if (stage == 0xE) {
            m_state = 0;
        } else if (ICE_TURARA_IS_MINE(stage)) {
            setVisibility(1);
            enableHit(0, 0);
        } else {
            setVisibility(0);
            disableHit(0, 0);
        }
    } else if (state < 7) {
        if (state == 1) {
            u8 stage = *m_stateWork;
            if (stage != 0xE) {
                if (ICE_TURARA_IS_MINE(stage)) {
                    setVisibility(1);
                    enableHit(0, 0);
                }
                m_state = 7;
            }
        } else if (state == 0) {
            setVisibility(0);
            disableHit(0, 0);
            disableAttack(0);
            m_attackSet = 0;
            m_state = 1;
        }
    } else if (state == 0x13 && m_pos.m_y < m_posLimitWork[1].m_y) {
        *m_stateWork = 0xE;
        setVisibility(0);
        if (m_type == 2) {
            disableAttack(0);
            m_attackSet = 0;
        }
        m_state = 0;
    }
}

void grIceTurara::updateCallBack(float deltaFrame) {
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
            Matrix* mtx = m_mtxWork;
            if (mtx != NULL) {
                if (m_state == 0x13) {
                    float speed = m_fallSpeed + 0.2f;
                    m_fallSpeed = speed;
                    if (1.0f < speed) {
                        m_fallSpeed = 1.0f;
                    }
                    m_pos.m_y = m_pos.m_y - m_fallSpeed;
                    if (m_rot.m_z != 0.0f) {
                        m_rot.m_z = m_rot.m_z * 0.025f;
                    }
                } else {
                    m_pos.m_x = mtx->m[0][3];
                    m_pos.m_y = mtx->m[1][3];
                    m_pos.m_z = mtx->m[2][3];
                    mtx->getRotate(&m_rot);
                    m_rot.m_x = m_rot.m_x * 57.29578f;
                    m_rot.m_y = m_rot.m_y * 57.29578f;
                    m_rot.m_z = m_rot.m_z * 57.29578f;
                }
                m_pos.m_z = 0.0f;
                grNodeCallbackData* data = calcWorldCallBack->m_nodeCallbackDatas;
                data->m_pos.m_x = m_pos.m_x;
                data->m_pos.m_y = m_pos.m_y;
                data->m_pos.m_z = m_pos.m_z;
                data = calcWorldCallBack->m_nodeCallbackDatas;
                data->m_rot.m_x = m_rot.m_x;
                data->m_rot.m_y = m_rot.m_y;
                data->m_rot.m_z = m_rot.m_z;
                m_snd.setPos(&m_pos);
            }
        }
    }
}

// The hit object of the icicle: one hit sphere (size 5) and the damage module (it reports its damage to onDamage).
void grIceTurara::setHit() {
    // MATCH-ONLY: the original allocates the hit data as raw storage (no element construction)
    m_hitData = new (Heaps::StageInstance) soCollisionHitData;
    m_hitSimple = new (Heaps::StageInstance) soCollisionHitData::Simple;
    m_hitSet = reinterpret_cast<soSet<soCollisionHitData>*>(new (Heaps::StageInstance) grIceSetView);
    m_dataGroup = new (Heaps::StageInstance) ykDataGroup;
    m_data = new (Heaps::StageInstance) ykData;

    m_hitData->m_startOffsetPos.m_x = 0.0f;
    m_hitData->m_startOffsetPos.m_y = 0.0f;
    m_hitData->m_startOffsetPos.m_z = 0.0f;
    m_hitData->m_endOffsetPos.m_x = 0.0f;
    m_hitData->m_endOffsetPos.m_y = 0.0f;
    m_hitData->m_endOffsetPos.m_z = 0.0f;
    m_hitData->m_size = 5.0f;
    // MATCH-ONLY: the shape type is the top bit of the byte at 0x1c (it is the whole byte of the simple form)
    reinterpret_cast<grIceHitByte*>(m_hitData)->m_shape = 1;
    grIceCopyHit(m_hitSimple, m_hitData);
    m_hitSimple->m_height = soCollisionHitData::Height_Low;
    m_hitSimple->m_nodeIndex = m_nodeIndex;

    // MATCH-ONLY: the members of soSet are private
    grIceSetView* set = reinterpret_cast<grIceSetView*>(m_hitSet);
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

// The attack of the falling icicle (power 20, it throws the fighter straight up).
void grIceTurara::setAttack() {
    if (m_attackSet != 1) {
        soCollisionAttackData attack(1.0f);
        Vec3f offset;
        offset.m_x = 0.0f;
        offset.m_y = 0.0f;
        offset.m_z = 0.0f;
        setAttackGimmickDetails(&attack, 6.0f, 1.0f, 1.0f, 1.0f,
            20, &offset, 270, 70, 0, 70, m_nodeIndex,
            0x3FF, 7, false, 15,
            soCollisionAttackData::Attribute_Normal, soCollisionAttackData::Sound_Level_Small, soCollisionAttackData::Sound_Attribute_Cutup,
            false, false, false, true, false, false, 0, 60,
            false, false, false, soCollisionAttackData::Lr_Check_Pos,
            false, false, false, false, false, soCollisionAttackData::Region_None, true);
        m_yakumono->setAttack(0, 0, &attack);
        m_attackSet = 1;
    }
}
