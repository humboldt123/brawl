#define YK_STAGE_INLINE // inline members of the shared SO headers that the stage RELs emit themselves
#define YK_STAGE_FULL // ... and the ones of the hit objects with an attack module and an event observer
#define SO_COLLISION_GROUP_ALIGNED // the hit object embeds soArrayVector<soCollisionGroup, 1> with 4-byte aligned elements
#define SO_COLLISION_GROUP_MEMBERWISE // ... and copies the group member by member

#include <ai/ai_mgr.h>
#include <gr/gr_calc_world_callback.h>
#include <math.h>
#include <memory.h>
#include <string.h>
#include <yk/yk_no_hit_normal.h>

#include <st_halberd/gr_halberd.h>
#include <st_halberd/gr_halberd_anim.h>

grHalberdArm* grHalberdArm::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grHalberdArm* ground = new (Heaps::StageInstance) grHalberdArm(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grHalberdArm::grHalberdArm(const char* taskName) : grHalberd(taskName) {
    m_mtxWork = NULL;
    m_stateAttackWork = NULL;
    m_type = 3;
    m_motion = 3;
    m_unk16c = 0.0f;
    m_motionTimer = 0.0f;
    m_yakumonoMade = 0;
    m_attackOn = 0;
    m_work = NULL;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_numNodeCallbackData = 1;
    callback->initialize(false, Heaps::StageInstance);
    callback->m_nodeCallbackDatas[0].m_flags |= 8;
}

grHalberdArm::~grHalberdArm() {
    if (m_work != NULL) {
        delete m_work;
    }
    m_work = NULL;
}

void grHalberdArm::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateYakumono(deltaFrame);
        updateActive(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

// Only the hand hurts: its attack is on while the shot of the stage is the grab (6).
void grHalberdArm::updateYakumono(float deltaFrame) {
    if (m_yakumonoMade == 1) {
        Vec3f pos(m_mtxWork->m[0][3], m_mtxWork->m[1][3], m_mtxWork->m[2][3]);
        setPos(&pos);
        if (*m_stateAttackWork == 6) {
            if (m_type == 2) {
                setAttack();
            }
        } else if (m_type == 2) {
            disableAttack(0);
            m_attackOn = 0;
        }
    } else {
        setHit();
        if (m_yakumono != NULL) {
            m_yakumonoMade = 1;
        }
    }
}

// Plays the animation of the hand (only the hand has one) with the sounds of the arm.
void grHalberdArm::updateActive(float deltaFrame) {
    if (m_type == 2) {
        u8 state = m_state;
        if (state == 2) {
            if (*m_stateWork == 0x13) {
                setMotion(1, 0, 1, &m_motionTimer);
                m_snd.playSE(snd_se_stage_Halberd_Arm_stop, 0, 0, -1);
                m_state = 0xD;
            }
        } else if (state < 2) {
            if (state == 0) {
                setMotion(0, 1, 1, &m_motionTimer);
                m_state = 1;
            }
            if (*m_stateWork == 0x12) {
                m_snd.playSE(snd_se_stage_Halberd_Arm_start, 0, 0, -1);
                m_state = 2;
            }
        } else if (state == 0xE) {
            if (*m_stateWork == 0x15) {
                m_state = 0;
            }
        } else if (state < 0xE && state > 0xC && *m_stateWork == 0x14) {
            setMotion(2, 0, 1, &m_motionTimer);
            m_snd.playSE(snd_se_stage_Halberd_Arm_attack, 0, 0, -1);
            m_state = 0xE;
        }
    }
}

void grHalberdArm::updateCallBack(float deltaFrame) {
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
            if (m_mtxWork != NULL) {
                Matrix mtx = *m_mtxWork;
                calcWorldCallBack->m_nodeCallbackDatas[0].m_matrix = mtx;
                Vec3f pos(mtx.m[0][3], mtx.m[1][3], mtx.m[2][3]);
                m_snd.setPos(&pos);
            }
        }
    }
}

// The animation of the hand (3 animations).
void grHalberdArm::setMotion(u32 animId, bool shouldLoop, bool force, float* frameCount) {
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

    if (animId >= 3) {
        return;
    }

    bool result = (modelAnim->m_resFile.GetResAnmChrNumEntries() > animId);
    if (result) {
        grHalberdSetChrAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmVisNumEntries() > animId);
    if (result) {
        grHalberdSetVisibilityAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmTexPatNumEntries() > animId);
    if (result) {
        grHalberdSetTexPatAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmTexSrtNumEntries() > animId);
    if (result) {
        grHalberdSetTexSrtAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmClrNumEntries() > animId);
    if (result) {
        grHalberdSetColorAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    gfModelAnimation::bind(sceneMdl, modelAnim);
    modelAnim->setFrame(0.0);
    modelAnim->setUpdateRate(1.0);
    modelAnim->setLoop(shouldLoop);

    if (frameCount != NULL) {
        *frameCount = modelAnim->getFrameCount();
    }
}

void grHalberdArm::setHit() {
    m_work = new (Heaps::StageInstance) grHalberdWork;
    m_work->unk0 = 0;
    m_work->unk4 = 0;

    ykInitInfo info = {NULL, NULL, 0x10, NULL, NULL};
    info.m_ground = this;
    nw4r::g3d::ScnMdl* model = ykDynamicCastScnMdl(m_sceneModels[0]);
    info.m_node = model;
    Vec3f pos = getPos();
    info.m_pos = &pos;
    info.m_work = m_work;

    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Gimmick, 2, 0, soCollisionAttackModuleImpl, 1, false, true> Config;
    ykNoHitNormal<Config>* yakumono = new (Heaps::StageInstance) ykNoHitNormal<Config>(&info);
    setYakumono(yakumono);
}

void grHalberdArm::setAttack() {
    if (m_attackOn != 1) {
        soCollisionAttackData attack(1.0f);
        Vec3f offset;
        offset.m_x = 0.0f;
        offset.m_y = 0.0f;
        offset.m_z = 0.0f;

        setAttackGimmickDetails(&attack, 10.0f, 1.0f, 1.0f, 1.0f,
            10, &offset, 361, 80, 0, 70, m_nodeIndex,
            0x3FF, 7, false, 15,
            soCollisionAttackData::Attribute_Normal, soCollisionAttackData::Sound_Level_Medium,
            soCollisionAttackData::Sound_Attribute_Punch,
            false, false, false, true, false, false, 0, 60,
            false, false, false, soCollisionAttackData::Lr_Check_Pos,
            false, false, false, false, false, soCollisionAttackData::Region_None, false);
        m_yakumono->setAttack(0, 0, &attack);
        m_attackOn = 1;
    }
}
