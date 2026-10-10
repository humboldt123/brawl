#define YK_STAGE_INLINE // inline members of the shared SO headers that the stage RELs emit themselves
#define YK_STAGE_FULL // ... and the ones of the hit objects with an attack module and an event observer
#define YK_DATA_INLINE_CTOR // the ykData / ykDataGroup constructors are inline in this REL
#define SO_COLLISION_GROUP_ALIGNED // the hit object embeds soArrayVector<soCollisionGroup, 1> with 4-byte aligned elements
#define SO_COLLISION_GROUP_MEMBERWISE // ... and copies the group member by member

#include <ef/ef_id.h>
#include <memory.h>
#include <mt/mt_prng.h>
#include <mt/mt_trig.h>
#include <types.h>
#include <yk/yk_normal.h>

#include <st_jungle/gr_jungle.h>
#include <st_jungle/gr_jungle_anim.h>

// Matrix::translate (main, unnamed): moves the matrix by the offset
extern "C" void fn_8003F074(Matrix* mtx, float x, float y, float z);

grJungleN* grJungleN::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grJungleN* ground = new (Heaps::StageInstance) grJungleN(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grJungleN::grJungleN(const char* taskName) : grJungle(taskName), m_snd() {
    m_mtxWork = NULL;
    m_stateWork = NULL;
    m_enableWork = NULL;
    m_hp = 0.0f;
    m_yakumonoMade = 0;
    m_shakeTimer = 0.0f;
    m_shake.m_x = 0.0f;
    m_shake.m_y = 0.0f;
    m_shake.m_z = 0.0f;
    m_hitData = NULL;
    m_hitSimple = NULL;
    m_hitSet = NULL;
    m_dataGroup = NULL;
    m_data = NULL;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback != NULL) {
        callback->m_numNodeCallbackData = 1;
        callback->initialize(false, Heaps::StageInstance);
        callback->m_nodeCallbackDatas->m_flags |= 8;
    }
}

grJungleN::~grJungleN() {
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

// The block is whole while the state of the stage is 0 or 1; its collision is on while the stage says it is on the screen.
void grJungleN::update(float deltaFrame) {
    grJungle::update(deltaFrame);
    if (m_isUpdate) {
        if (m_enableWork != NULL) {
            switch (*m_enableWork) {
            case 0:
                if (m_isVisible == 1) {
                    setVisibility(0);
                }
                if (m_isEnableCollisionStatus == 1) {
                    setEnableCollisionStatus(false);
                }
                setDisableYakumono();
                break;
            case 1:
                if (*m_stateWork <= 1) {
                    if (!m_isVisible) {
                        setVisibility(1);
                    }
                    if (*m_stateWork != 0) {
                        if (!m_isEnableCollisionStatus) {
                            setEnableCollisionStatus(true);
                            updateG3dProcCalcWorld();
                            m_hasUpdatedG3dCalcWorld = false;
                        }
                        setEnableYakumono();
                    }
                }
                break;
            }
        }
        updateBreak(deltaFrame);
        updateYakumono(deltaFrame);
        updateShake(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

void grJungleN::updateBreak(float deltaFrame) {
    if (m_stateWork != NULL) {
        stJungleData* data = static_cast<stJungleData*>(getStageData());
        if (data != NULL) {
            u8 state = m_state;
            if (state != 1) {
                if (state == 0) {
                    setVisibility(1);
                    m_hp = data->unk14;
                    m_state = 1;
                } else if (state == 9) {
                    if (*m_stateWork == 4) {
                        *m_stateWork = 0;
                        m_state = 0;
                    }
                }
            }
        }
    }
}

// The block follows the matrix the stage gives, moved a little down and by the shake.
void grJungleN::updateCallBack(float deltaFrame) {
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
            if (m_mtxWork != NULL) {
                Vec3f offset;
                offset.m_x = 0.0f;
                offset.m_y = -0.4f;
                offset.m_z = 0.0f;
                Matrix mtx = *m_mtxWork;
                fn_8003F074(&mtx, offset.m_x, offset.m_y, offset.m_z);
                fn_8003F074(&mtx, m_shake.m_x, m_shake.m_y, m_shake.m_z);
                calcWorldCallBack->m_nodeCallbackDatas[0].m_matrix = mtx;
            }
            Vec3f pos;
            pos.m_x = calcWorldCallBack->m_nodeCallbackDatas[0].m_matrix(0, 3);
            pos.m_y = calcWorldCallBack->m_nodeCallbackDatas[0].m_matrix(1, 3);
            pos.m_z = calcWorldCallBack->m_nodeCallbackDatas[0].m_matrix(2, 3);
            m_snd.setPos(&pos);
        }
    }
}

void grJungleN::updateYakumono(float deltaFrame) {
    if (m_stateWork != NULL && m_yakumonoMade != 1) {
        setHit();
        if (m_yakumono != NULL) {
            m_yakumonoMade = 1;
        }
    }
}

// The block shakes for a while after it was hit (every third frame it is moved by a little in a random direction).
void grJungleN::updateShake(float deltaFrame) {
    m_shakeTimer = m_shakeTimer - deltaFrame;
    if (m_shakeTimer <= 0.0f) {
        m_shakeTimer = 0.0f;
    }
    if (m_shakeTimer <= 0.0f) {
        m_shake.m_x = 0.0f;
        m_shake.m_y = 0.0f;
        m_shake.m_z = 0.0f;
    } else {
        u32 remainder = static_cast<u32>(m_shakeTimer) % 3;
        if (static_cast<float>(remainder) == 0.0f) {
            float angle;
            if (randf() >= 0.5f) {
                angle = 180.0f * randf();
            } else {
                randf();
                angle = 0.0f;
            }
            float sine;
            float cosine;
            mtSinCosf(angle * 0.017453292f, &sine, &cosine);
            float scale = 0.8f * randf() + 0.5f;
            m_shake.m_x = scale * cosine;
            m_shake.m_z = 0.0f;
            m_shake.m_y = scale * sine;
        }
    }
}

// The hit object of the block: one hit sphere (the wooden block is hit by the fighters).
void grJungleN::setHit() {
    // MATCH-ONLY: the original allocates the hit data as raw storage (no element construction)
    m_hitData = new (Heaps::StageInstance) soCollisionHitData;
    m_hitSimple = new (Heaps::StageInstance) soCollisionHitData::Simple;
    m_hitSet = reinterpret_cast<soSet<soCollisionHitData>*>(new (Heaps::StageInstance) grJungleSetView);
    m_dataGroup = new (Heaps::StageInstance) ykDataGroup;
    m_data = new (Heaps::StageInstance) ykData;

    m_hitData->m_startOffsetPos.m_x = 0.0f;
    m_hitData->m_startOffsetPos.m_y = 10.0f;
    m_hitData->m_startOffsetPos.m_z = 10.0f;
    m_hitData->m_endOffsetPos.m_x = 0.0f;
    m_hitData->m_endOffsetPos.m_y = 10.0f;
    m_hitData->m_endOffsetPos.m_z = 10.0f;
    m_hitData->m_size = 12.0f;
    // MATCH-ONLY: the shape type is the top bit of the byte at 0x1c (it is the whole byte of the simple form)
    reinterpret_cast<grJungleHitByte*>(m_hitData)->m_shape = 1;
    grJungleCopyHit(m_hitSimple, m_hitData);
    m_hitSimple->m_height = soCollisionHitData::Height_Low;
    m_hitSimple->m_nodeIndex = m_nodeIndex;

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

void grJungleN::onDamage(int index, soDamage* damage, soDamageAttackerInfo* attackerInfo) {
    if (m_stateWork != NULL) {
        float amount = damage->unk4;
        fn_27_26399C(m_yakumono);
        if (m_state == 1) {
            m_hp = m_hp - amount;
            if (m_hp < 0.0f) {
                m_hp = 0.0f;
            }
            if (m_hp == 0.0f) {
                setVisibility(0);
                setEnableCollisionStatus(false);
                disableHit(0, 0);
                *m_stateWork = 2;
                m_snd.playSE(static_cast<SndID>(0x1B94), 0, 0, -1);
                m_state = 9;
            } else {
                m_snd.playSE(static_cast<SndID>(0x1B93), 0, 0, -1);
            }
        }
        m_shakeTimer = 2.5f * randf() + 2.5f;
    }
}

void grJungleN::setEnableYakumono() {
    if (m_stateWork != NULL && *m_stateWork == 1) {
        enableHit(0, 0);
    }
}

void grJungleN::setDisableYakumono() {
    disableHit(0, 0);
}
