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
#include <mt/mt_trig.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <snd/snd_system.h>
#include <types.h>
#include <yk/yk_normal.h>

#include <st_ice/gr_ice.h>
#include <st_ice/gr_ice_hit.h>

// Yakumono::getDamage (sora_melee, unnamed)
extern "C" void fn_27_26399C(Yakumono* yakumono);
// translate a matrix (main, unnamed)
extern "C" void fn_8003F074(float x, float y, float z, Matrix* matrix);

grIceBreak::grIceBreak(const char* taskName) : grIce(taskName), m_snd() {
    m_mtxWork = NULL;
    m_preBuild = 0.0f;
    m_hitTimer = 0.0f;
    m_hp = 0.0f;
    m_shakeTimer = 0.0f;
    m_shake.m_x = 0.0f;
    m_shake.m_y = 0.0f;
    m_shake.m_z = 0.0f;
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
    callback->m_nodeCallbackDatas[0].m_flags |= 8;
}

grIceBreak* grIceBreak::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grIceBreak* ground = new (Heaps::StageInstance) grIceBreak(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grIceBreak::~grIceBreak() {
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

void grIceBreak::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    float timer = m_timer;
    m_timer = timer - deltaFrame;
    if (timer - deltaFrame < 0.0f) {
        m_timer = 0.0f;
    }
    timer = m_hitTimer;
    m_hitTimer = timer - deltaFrame;
    if (timer - deltaFrame < 0.0f) {
        m_hitTimer = 0.0f;
    }
    if (m_isUpdate) {
        updateYakumono(deltaFrame);
        updatePreBuild(deltaFrame);
        updateShake(deltaFrame);
        updateCallBack(deltaFrame);
        m_hasUpdatedG3dCalcWorld = false;
        updateG3dProcCalcWorld();
    }
}

// The hit object is made the first time this is called; after that the floor is built up again when it is broken and the
// time of the stage data went by.
void grIceBreak::updateYakumono(float deltaFrame) {
    if (m_hasHit == 1) {
        if (m_hp == 0.0f && m_timer == 0.0f) {
            stIceData* data = static_cast<stIceData*>(getStageData());
            if (data != NULL) {
                m_hp = data->unk0C;
                setVisibility(1);
                setEnableCollisionStatus(1);
                enableHit(0, 0);
            }
        }
    } else {
        setHit();
        if (m_yakumono != NULL) {
            m_hasHit = 1;
        }
    }
}

// The floor blinks while it builds up (in the last second).
void grIceBreak::updatePreBuild(float deltaFrame) {
    if (m_hp == 0.0f) {
        if (m_timer <= 60.0f) {
            float timer = m_preBuild - deltaFrame;
            m_preBuild = timer;
            if (timer < 0.0f) {
                m_preBuild = 0.0f;
            }
            if (m_preBuild <= 2.0f) {
                setVisibility(0);
            } else {
                setVisibility(1);
            }
            if (m_preBuild == 0.0f) {
                m_preBuild = 4.0f;
            }
        } else {
            m_preBuild = 4.0f;
        }
    }
}

// The floor shakes a bit in a random direction every third frame while the shake timer runs.
void grIceBreak::updateShake(float deltaFrame) {
    float timer = m_shakeTimer - deltaFrame;
    m_shakeTimer = timer;
    if (timer <= 0.0f) {
        m_shakeTimer = 0.0f;
    }
    if (m_shakeTimer <= 0.0f) {
        m_shake.m_x = 0.0f;
        m_shake.m_y = 0.0f;
        m_shake.m_z = 0.0f;
    } else {
        u32 frame = (u32)m_shakeTimer;
        if ((float)(frame % 3) == 0.0f) {
            float sine;
            float cosine;
            mtSinCosf((randf() * 360.0f) * 0.017453292f, &sine, &cosine);
            float scale = randf() * 0.8f + 0.5f;
            m_shake.m_x = scale * cosine;
            m_shake.m_z = 0.0f;
            m_shake.m_y = scale * sine;
        }
    }
}

void grIceBreak::updateCallBack(float deltaFrame) {
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
            if (m_mtxWork != NULL) {
                Matrix matrix = *m_mtxWork;
                fn_8003F074(m_shake.m_x, m_shake.m_y, m_shake.m_z, &matrix);
                calcWorldCallBack->m_nodeCallbackDatas[0].m_matrix = matrix;
                Vec3f pos;
                pos.m_x = matrix.m[0][3];
                pos.m_y = matrix.m[1][3];
                pos.m_z = matrix.m[2][3];
                m_snd.setPos(&pos);
            }
        }
    }
}

// The hit object of the floor: one hit sphere (a box that starts at (5, -2.5) and ends at (20, -7.5), size 6) and the damage
// module. It has no attack.
void grIceBreak::setHit() {
    m_hitData = new (Heaps::StageInstance) soCollisionHitData;
    m_hitSimple = new (Heaps::StageInstance) soCollisionHitData::Simple;
    m_hitSet = reinterpret_cast<soSet<soCollisionHitData>*>(new (Heaps::StageInstance) grIceSetView);
    m_dataGroup = new (Heaps::StageInstance) ykDataGroup;
    m_data = new (Heaps::StageInstance) ykData;

    m_hitData->m_startOffsetPos.m_x = 5.0f;
    m_hitData->m_startOffsetPos.m_y = -2.5f;
    m_hitData->m_startOffsetPos.m_z = 0.0f;
    m_hitData->m_endOffsetPos.m_x = 20.0f;
    m_hitData->m_endOffsetPos.m_y = -7.5f;
    m_hitData->m_endOffsetPos.m_z = 0.0f;
    m_hitData->m_size = 6.0f;
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
    typedef ykNormal<soCollisionAttackModuleBuildConfigNull,
                     soCollisionHitModuleBuildConfig<soCollision::Category_Wall, 1, 1, soCollisionHitModuleImpl, 0x3FF, true> >
        Config;
    Config* yakumono = new (Heaps::StageInstance) Config(&info);
    yakumono->postInitialize();
    yakumono->activate(&pos, -1.0f, 0.0f);
    setYakumono(yakumono);
}

// The floor was hit: its hit points go down (by the damage, multiplied by the factor of the stage data). When the last one is
// gone it breaks (the effect, the sound, no collision) for the time of the stage data.
void grIceBreak::onDamage(int index, soDamage* damage, soDamageAttackerInfo* attackerInfo) {
    stIceData* data = static_cast<stIceData*>(getStageData());
    if (data != NULL) {
        float amount = damage->unk4;
        fn_27_26399C(m_yakumono);
        float hp = m_hp - amount * data->unk10;
        m_hp = hp;
        if (hp < 0.0f) {
            m_hp = 0.0f;
        }
        if (m_hp == 0.0f) {
            u32 effect = g_ecMgr->setEffect(static_cast<EfID>(0x480002));
            g_ecMgr->setParent(effect, m_sceneModels[0], "kowareyuka", 0);
            m_snd.playSE(static_cast<SndID>(0x1C81), 0, 0, -1);
            setVisibility(0);
            setEnableCollisionStatus(0);
            disableHit(0, 0);
            m_timer = data->unk24;
        } else {
            if (m_hitTimer == 0.0f) {
                u32 effect = g_ecMgr->setEffect(static_cast<EfID>(0x480004));
                g_ecMgr->setParent(effect, m_sceneModels[0], "kowareyuka", 0);
                m_hitTimer = randf() * 5.0f + 10.0f;
            }
            m_snd.playSE(static_cast<SndID>(0x1C80), 0, 0, -1);
            m_shakeTimer = randf() * 2.5f + 2.5f;
        }
    }
}
