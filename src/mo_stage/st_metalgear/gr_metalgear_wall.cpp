#define YK_STAGE_INLINE // inline members of the shared SO headers that the stage RELs emit themselves
#define YK_STAGE_FULL // ... and the ones of the hit objects with an attack module and an event observer
#define YK_DATA_INLINE_CTOR // the ykData / ykDataGroup constructors are inline in this REL
#define SO_COLLISION_GROUP_ALIGNED // the hit object embeds soArrayVector<soCollisionGroup, 1> with 4-byte aligned elements
#define SO_COLLISION_GROUP_MEMBERWISE // ... and copies the group member by member

#include <cm/cm_quake.h>
#include <ec/ec_mgr.h>
#include <gf/gf_model.h>
#include <gr/collision/gr_collision.h>
#include <gr/collision/gr_collision_joint.h>
#include <gr/gr_calc_world_callback.h>
#include <math.h>
#include <memory.h>
#include <mt/mt_prng.h>
#include <mt/mt_trig.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <nw4r/math/math_arithmetic.h>
#include <nw4r/math/math_triangular.h>
#include <snd/snd_system.h>
#include <yk/yk_normal.h>

#include <st_metalgear/gr_metalgear.h>
#include <st_metalgear/gr_metalgear_anim.h>

// MATCH-ONLY: the members of soSet are private
struct grMetalgearSetView {
    void* m_elements;
    u32 m_size;
};

// MATCH-ONLY: the flag word of a collision joint (the stage sets the collision kind in its second byte)
struct metalgearJointView {
    u8 _pad[0x48];
    u32 _unused0 : 8;
    u32 m_kind : 8;
    u32 _unused1 : 16;
};

// HYPOTHESIS: an inline helper of the original (degrees to radians)
static inline float metalgearDegToRad(float deg) {
    return deg * 0.017453292f;
}

// HYPOTHESIS: the clamp helper that the other stages use as well.
static inline float metalgearClamp(float value, float lo, float hi) {
    value = nw4r::math::FSelect(value - lo, value, lo);
    return nw4r::math::FSelect(value - hi, hi, value);
}

grMetalgearWall::grMetalgearWall(const char* taskName) : grMetalgear(taskName), m_snd() {
    unk158 = 0.0f;
    unk15C = 0.0f;
    unk160 = 0.0f;
    m_pos.m_x = 0.0f;
    m_pos.m_y = 0.0f;
    m_pos.m_z = 0.0f;
    m_posGimmickWork = NULL;
    m_rotZWork = NULL;
    m_life = 0.0f;
    m_motionFrame = 0.0f;
    m_shakeTimer = 0.0f;
    m_shake.m_x = 0.0f;
    m_shake.m_y = 0.0f;
    m_shake.m_z = 0.0f;
    m_stateWork = NULL;
    m_type = 9;
    m_collPoint[0] = 0.0f;
    m_collPoint[1] = 0.0f;
    m_collPoint[2] = 0.0f;
    m_collPoint[3] = 0.0f;
    m_nodeEffect = 0;
    m_effectTimer = 0.0f;
    m_hasHit = 0;
    m_hitData = NULL;
    m_hitSimple = NULL;
    m_hitSet = NULL;
    m_dataGroup = NULL;
    m_data = NULL;
    m_motion = 1;
    m_frameCount = 0.0f;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_numNodeCallbackData = 1;
    callback->initialize(false, Heaps::StageInstance);
    callback->m_nodeCallbackDatas[0].m_flags |= 0x10;
    m_joint = NULL;
    m_vtxIndex = -1;
}

grMetalgearWall* grMetalgearWall::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grMetalgearWall* ground = new (Heaps::StageInstance) grMetalgearWall(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grMetalgearWall::~grMetalgearWall() {
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

// The places of the two top walls are given to the stage (the fighters can stand on the platforms they make).
void grMetalgearWall::processAnim() {
    Ground::processAnim();
    if (m_posGimmickWork != NULL) {
        switch (m_type) {
        case 0:
            if (m_nodeEffect == 0) {
                getNodeIndex(&m_nodeEffect, 0, "StgMetalgear_L_TOP");
            }
            getNodePosition(&m_posGimmickWork[11], 0, m_nodeEffect);
            break;
        case 2:
            if (m_nodeEffect == 0) {
                getNodeIndex(&m_nodeEffect, 0, "StgMetalgear_R_TOP");
            }
            getNodePosition(&m_posGimmickWork[12], 0, m_nodeEffect);
            break;
        }
    }
}

void grMetalgearWall::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateYakumono(deltaFrame);
        updateBreak(deltaFrame);
        updateShake(deltaFrame);
        updateCollision(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

void grMetalgearWall::updateYakumono(float deltaFrame) {
    if (m_hasHit != 1) {
        setHit();
        if (m_yakumono != NULL) {
            m_hasHit = 1;
        }
    }
}

// The wall stands (state 0, 1), breaks when the life is gone (2), stays broken (3, 4) and rises again (5).
void grMetalgearWall::updateBreak(float deltaFrame) {
    stMetalgearData* data = static_cast<stMetalgearData*>(getStageData());
    if (data == NULL) {
        return;
    }
    m_timer = m_timer - deltaFrame;
    if (m_timer < 0.0f) {
        m_timer = 0.0f;
    }
    m_effectTimer = m_effectTimer - deltaFrame;
    if (m_effectTimer < 0.0f) {
        m_effectTimer = 0.0f;
    }
    switch (m_state) {
    case 0:
        setMotion(0, false, true, &m_frameCount);
        switch (m_type) {
        case 2:
        case 0:
            m_life = data->unk10;
            break;
        case 1:
        case 3:
            m_life = data->unk14;
            break;
        }
        enableHit(0, 0);
        *m_stateWork = 6;
        m_state = 1;
        return;
    case 1:
        if (*m_stateWork < 2) {
            setEnableCollisionStatus(false);
            disableHit(0, 0);
            Vec3f offset(0.0f, 0.0f, 0.0f);
            cmReqQuake(cmQuake::Amplitude_XL, &offset);
            int sndId = -1;
            float pan;
            switch (m_type) {
            case 2:
                if (*m_stateWork == 0) {
                    sndId = snd_se_stage_Metalgear_01;
                }
                pan = 0.75f;
                break;
            case 0:
                if (*m_stateWork == 0) {
                    sndId = snd_se_stage_Metalgear_01;
                }
                pan = -0.75f;
                break;
            case 1:
                if (*m_stateWork == 0) {
                    sndId = snd_se_stage_Metalgear_01;
                }
                if (*m_stateWork == 1) {
                    sndId = snd_se_stage_Metalgear_wall_all;
                }
                pan = -0.75f;
                break;
            case 3:
                if (*m_stateWork == 0) {
                    sndId = snd_se_stage_Metalgear_01;
                }
                if (*m_stateWork == 1) {
                    sndId = snd_se_stage_Metalgear_wall_all;
                }
                pan = 0.75f;
                break;
            }
            if (sndId != -1) {
                fn_27_224DB8(sndId, pan);
            }
            u32 handle;
            switch (m_type) {
            case 2:
                handle = g_ecMgr->setEffect(ef_ptc_stg_metalgear_tower_rtcrash);
                g_ecMgr->setParent(handle, m_sceneModels[0], "StgMetalgear_R_TOP", false);
                break;
            case 0:
                handle = g_ecMgr->setEffect(ef_ptc_stg_metalgear_tower_ltcrash);
                g_ecMgr->setParent(handle, m_sceneModels[0], "StgMetalgear_L_TOP", false);
                break;
            case 1:
                handle = g_ecMgr->setEffect(ef_ptc_stg_metalgear_tower_lbcrash);
                g_ecMgr->setParent(handle, m_sceneModels[0], "StgMetalgear_L_UNDER", false);
                break;
            case 3:
                handle = g_ecMgr->setEffect(ef_ptc_stg_metalgear_tower_rbcrash);
                g_ecMgr->setParent(handle, m_sceneModels[0], "StgMetalgear_R_UNDER", false);
                break;
            }
            m_state = 2;
            return;
        }
        {
            float base;
            switch (m_type) {
            case 2:
            case 0:
                base = data->unk10;
                break;
            case 1:
            case 3:
                base = data->unk14;
                break;
            }
            float frame;
            if (base * data->unk28 <= m_life) {
                if (base * data->unk24 <= m_life) {
                    frame = 0.0f;
                } else {
                    frame = 1.0f;
                }
            } else {
                frame = 2.0f;
            }
            setMotionFrame(frame, 0);
            if (m_motionFrame != frame) {
                if (m_type < 2) {
                    fn_27_224DB8(snd_se_stage_Metalgear_wall_clack, -0.75f);
                } else if (m_type < 4) {
                    fn_27_224DB8(snd_se_stage_Metalgear_wall_clack, 0.75f);
                }
            }
            m_motionFrame = frame;
        }
        return;
    case 2: {
        if (getMotionFrame(0) <= 150.0f) {
            Vec3f offset(0.0f, 0.0f, 0.0f);
            cmReqQuake(cmQuake::Amplitude_M, &offset);
        } else {
            setVisibility(0);
            setMotionFrame(150.0f, 0);
        }
        if (*m_stateWork != 2) {
            return;
        }
        m_state = 4;
        return;
    }
    case 3:
        return;
    case 4: {
        if (getMotionFrame(0) > 150.0f) {
            setVisibility(0);
            setMotionFrame(150.0f, 0);
        }
        if (*m_stateWork != 3) {
            return;
        }
        if (m_type < 2) {
            fn_27_224DB8(snd_se_stage_Metalgear_02, -0.75f);
        } else if (m_type < 4) {
            fn_27_224DB8(snd_se_stage_Metalgear_02, 0.75f);
        }
        setVisibility(1);
        Vec3f offset(0.0f, 0.0f, 0.0f);
        cmReqQuake(cmQuake::Amplitude_S, &offset);
        m_state = 5;
        return;
    }
    case 5:
        if (getMotionFrame(0) >= m_frameCount) {
            m_state = 0;
            return;
        }
        if (m_isEnableCollisionStatus) {
            return;
        }
        setEnableCollisionStatus(true);
        return;
    }
}

// The wall shakes after a hit (the position of the model moves by a small random step every third frame).
void grMetalgearWall::updateShake(float deltaFrame) {
    m_shakeTimer = m_shakeTimer - deltaFrame;
    if (m_shakeTimer <= 0.0f) {
        m_shakeTimer = 0.0f;
    }
    if (m_shakeTimer > 0.0f) {
        u32 step = (u32)m_shakeTimer % 3;
        if ((float)step == 0.0f) {
            float sine;
            float cosine;
            mtSinCosf(metalgearDegToRad(90.0f), &sine, &cosine);
            float scale = randf() * 0.8f + 0.5f;
            m_shake.m_x = scale * cosine;
            m_shake.m_z = 0.0f;
            m_shake.m_y = scale * sine;
        }
    } else {
        m_shake.m_x = 0.0f;
        m_shake.m_y = 0.0f;
        m_shake.m_z = 0.0f;
    }
}

// The collision of the wall follows its animation: the vertices are moved over the angle of the wall (the stage knows it).
void grMetalgearWall::updateCollision(float deltaFrame) {
    switch (m_type) {
    case 0:
        break;
    case 1:
        return;
    case 2:
        break;
    default:
        return;
    }
    if (m_joint == NULL) {
        grCollision* collision = m_collision;
        if (collision == NULL) {
            return;
        }
        u16 count = collision->m_jointLen;
        u32 i = 0;
        grCollisionJoint* joint;
        for (; i != count; i++) {
            joint = collision->getJoint(i);
            if (joint != NULL && joint->m_ground == this) {
                break;
            }
        }
        if (i == count) {
            return;
        }
        m_joint = joint;
        reinterpret_cast<metalgearJointView*>(joint)->m_kind = 3;
        u16* line = reinterpret_cast<u16*>(joint->getLine(0));
        int index = 0;
        for (u32 n = joint->m_vtxLen; n != 0; n--) {
            Vec2f vtx = joint->m_vtxDatas[(u16)(*line + index)].m_pos;
            if (130.0f < vtx.m_y) {
                m_vtxIndex = index;
                m_collPoint[0] = vtx.m_x;
                m_collPoint[1] = 43.0f;
                Vec2f next = joint->m_vtxDatas[(u16)(*line + index + 1)].m_pos;
                m_collPoint[2] = next.m_x;
                m_collPoint[3] = 43.0f;
                break;
            }
            index++;
        }
    }
    if (m_joint != NULL && m_vtxIndex != -1) {
        u16* line = reinterpret_cast<u16*>(m_joint->getLine(0));
        if (line != NULL) {
            float reach;
            float reachNext;
            if (m_state == 5) {
                reach = metalgearClamp((getMotionFrame(0) - 150.0f) / (m_frameCount - 150.0f), 0.0f, 1.0f);
                reachNext = metalgearClamp((getMotionFrame(0) - 150.0f) / ((m_frameCount - 150.0f) - 60.0f), 0.0f, 1.0f);
            } else {
                reach = 1.0f;
                reachNext = reach;
            }
            int base = *line + m_vtxIndex;
            grCollData::VtxData* vtxA = &m_joint->m_vtxDatas[(u16)base];
            grCollData::VtxData* vtxB = &m_joint->m_vtxDatas[(u16)(base + 1)];
            float sine;
            float cosine;
            if (m_type != 1) {
                if (m_type == 0) {
                    nw4r::math::SinCosFIdx(&sine, &cosine, (180.0f - reach * 90.0f) * 0.7111111f);
                    if (m_rotZWork != NULL) {
                        *m_rotZWork = 180.0f - reach * 90.0f;
                    }
                    m_posGimmickWork[11].m_x = -70.0f;
                    m_posGimmickWork[11].m_y = reach;
                    m_posGimmickWork[11].m_z = reachNext;
                    if (reach != 1.0f) {
                        nw4r::math::SinCosFIdx(&sine, &cosine, 128.0f);
                    }
                    vtxA->m_pos.m_x = m_collPoint[0] + cosine * 200.0f;
                    vtxA->m_pos.m_y = m_collPoint[1] + sine * 200.0f;
                    vtxB->m_pos.m_x = m_collPoint[2] + cosine * 200.0f;
                    vtxB->m_pos.m_y = m_collPoint[3] + sine * 200.0f;
                } else if (m_type < 3) {
                    nw4r::math::SinCosFIdx(&sine, &cosine, (reach * 90.0f) * 0.7111111f);
                    if (m_rotZWork != NULL) {
                        *m_rotZWork = reach * 90.0f;
                    }
                    m_posGimmickWork[12].m_x = 70.0f;
                    m_posGimmickWork[12].m_y = reach;
                    m_posGimmickWork[12].m_z = reachNext;
                    if (reach != 1.0f) {
                        nw4r::math::SinCosFIdx(&sine, &cosine, 0.0f);
                    }
                    vtxA->m_pos.m_x = m_collPoint[0] + cosine * 200.0f;
                    vtxA->m_pos.m_y = m_collPoint[1] + sine * 200.0f;
                    vtxB->m_pos.m_x = m_collPoint[2] + cosine * 200.0f;
                    vtxB->m_pos.m_y = m_collPoint[3] + sine * 200.0f;
                }
            }
        }
    }
}

void grMetalgearWall::updateCallBack(float deltaFrame) {
    grCalcWorldCallBack* calcWorldCallBack = &m_calcWorldCallBack;
    if (calcWorldCallBack != NULL) {
        nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[m_unk1];
        if (scnMdl != NULL) {
            if (scnMdl->m_calcWorldCallBack == NULL) {
                calcWorldCallBack->m_index = 0;
                calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex = m_nodeIndex;
                scnMdl->m_calcWorldCallBack = calcWorldCallBack;
                scnMdl->EnableScnMdlCallbackTiming(1);
                scnMdl->m_nodeIndex = calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex;
            }
            grNodeCallbackData* data = calcWorldCallBack->m_nodeCallbackDatas;
            data->m_offsetPos = m_pos + m_shake;
        }
    }
}

// The hit object of the wall: one hit sphere (size 10) over the part of the stage the wall covers.
void grMetalgearWall::setHit() {
    Vec3f nodePos;
    u32 rootNode = 0;
    getNodePosition(&nodePos, 0, rootNode);
    // MATCH-ONLY: the original allocates the hit data as raw storage (no element construction)
    m_hitData = reinterpret_cast<soCollisionHitData*>(new (Heaps::StageInstance) u8[sizeof(soCollisionHitData)]);
    m_hitSimple = reinterpret_cast<soCollisionHitData::Simple*>(new (Heaps::StageInstance) u8[sizeof(soCollisionHitData::Simple)]);
    m_hitSet = reinterpret_cast<soSet<soCollisionHitData>*>(new (Heaps::StageInstance) u8[sizeof(grMetalgearSetView)]);
    m_dataGroup = new (Heaps::StageInstance) ykDataGroup;
    m_data = new (Heaps::StageInstance) ykData;

    m_hitData->m_startOffsetPos.m_x = 0.0f;
    m_hitData->m_startOffsetPos.m_y = -30.0f;
    m_hitData->m_startOffsetPos.m_z = -nodePos.m_z;
    m_hitData->m_endOffsetPos.m_x = 0.0f;
    m_hitData->m_endOffsetPos.m_y = 18.0f;
    m_hitData->m_endOffsetPos.m_z = -nodePos.m_z;
    m_hitData->m_size = 10.0f;
    // MATCH-ONLY: the shape type is the top bit of the byte at 0x1c (it is the whole byte of the simple form)
    reinterpret_cast<grMetalgearHitByte*>(m_hitData)->m_shape = 1;
    u32* srcWords = reinterpret_cast<u32*>(m_hitData);
    u32* dstWords = reinterpret_cast<u32*>(m_hitSimple);
    for (int w = 0; w < 6; w += 3) {
        u32 w0 = srcWords[w];
        u32 w1 = srcWords[w + 1];
        dstWords[w] = w0;
        dstWords[w + 1] = w1;
        dstWords[w + 2] = srcWords[w + 2];
    }
    m_hitSimple->m_size = m_hitData->m_size;
    reinterpret_cast<u8*>(m_hitSimple)[0x1C] = reinterpret_cast<u8*>(m_hitData)[0x1C];
    m_hitSimple->m_height = soCollisionHitData::Height_Low;
    m_hitSimple->m_nodeIndex = 0;
    // MATCH-ONLY: the members of soSet are private
    grMetalgearSetView* set = reinterpret_cast<grMetalgearSetView*>(m_hitSet);
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
                     soCollisionHitModuleBuildConfig<soCollision::Category_Gimmick, 1, 1, soCollisionHitModuleImpl, 0x3FF, true> >
        Config;
    Config* yakumono = new (Heaps::StageInstance) Config(&info);
    yakumono->postInitialize();
    yakumono->activate(&pos, -1.0f, 0.0f);
    setYakumono(yakumono);
}

// The animation of the wall (only motion 0 exists).
void grMetalgearWall::setMotion(u32 animId, bool loop, bool force, float* frameCount) {
    if (m_motion == animId && force == 0) {
        return;
    }

    nw4r::g3d::ScnMdl* sceneMdl = *m_sceneModels;
    if (sceneMdl == NULL) {
        return;
    }

    gfModelAnimation* modelAnim = *m_modelAnims;
    if (modelAnim == NULL) {
        return;
    }

    nw4r::g3d::ResMdl model = sceneMdl->m_resMdl;
    if (!model.IsValid()) {
        return;
    }

    modelAnim->unbindNodeAnim(sceneMdl);
    modelAnim->unbindVisibleAnim(sceneMdl);
    modelAnim->unbindTexAnim(sceneMdl);
    modelAnim->unbindTexSrtAnim(sceneMdl);
    modelAnim->unbindMatColAnim(sceneMdl);
    m_motion = animId;

    if (animId != 0) {
        return;
    }

    bool result = (modelAnim->m_resFile.GetResAnmChrNumEntries() > animId);
    if (result) {
        grMetalgearSetChrAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmTexPatNumEntries() > animId);
    if (result) {
        grMetalgearSetTexPatAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmTexSrtNumEntries() > animId);
    if (result) {
        grMetalgearSetTexSrtAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmClrNumEntries() > animId);
    if (result) {
        grMetalgearSetColorAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmVisNumEntries() > animId);
    if (result) {
        grMetalgearSetVisibilityAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    gfModelAnimation::bind(sceneMdl, modelAnim);
    modelAnim->setFrame(0.0);
    modelAnim->setUpdateRate(1.0);
    modelAnim->setLoop(loop);

    if (frameCount != NULL) {
        *frameCount = modelAnim->getFrameCount();
    }
}

// A fighter hit the wall: the life shrinks, the wall shakes and sparks.
void grMetalgearWall::onDamage(int index, soDamage* damage, soDamageAttackerInfo* attackerInfo) {
    float amount = damage->unk4;
    fn_27_26399C(m_yakumono);
    m_life = m_life - amount;
    if (m_life < 0.0f) {
        m_life = 0.0f;
        *m_stateWork = 0;
    }
    m_shakeTimer = randf() * 2.5f + 2.5f;
    if (m_effectTimer == 0.0f) {
        u32 handle = g_ecMgr->setEffect(ef_ptc_stg_metalgear_tower_damage);
        switch (m_type) {
        case 2:
            g_ecMgr->setParent(handle, m_sceneModels[0], "StgMetalgear_R_TOP", false);
            break;
        case 0:
            g_ecMgr->setParent(handle, m_sceneModels[0], "StgMetalgear_L_TOP", false);
            break;
        case 1:
            g_ecMgr->setParent(handle, m_sceneModels[0], "StgMetalgear_L_UNDER", false);
            break;
        case 3:
            g_ecMgr->setParent(handle, m_sceneModels[0], "StgMetalgear_R_UNDER", false);
            break;
        }
        m_effectTimer = randf() * 5.0f + 10.0f;
    }
    if (m_type < 2) {
        fn_27_224DB8(snd_se_stage_Metalgear_wall_dmg, -0.75f);
    } else if (m_type < 4) {
        fn_27_224DB8(snd_se_stage_Metalgear_wall_dmg, 0.75f);
    }
}
