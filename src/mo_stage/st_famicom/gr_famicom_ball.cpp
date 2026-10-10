#define YK_STAGE_INLINE // inline members of the shared SO headers that the stage RELs emit themselves
#define YK_STAGE_FULL // ... and the ones of the hit objects with an attack module and an event observer
#define YK_DATA_INLINE_CTOR // the ykData / ykDataGroup constructors are inline in this REL
#define SO_COLLISION_GROUP_ALIGNED // the hit object embeds soArrayVector<soCollisionGroup, 1> with 4-byte aligned elements
#define SO_COLLISION_GROUP_MEMBERWISE // ... and copies the group member by member

#include <ai/ai_mgr.h>
#include <gf/gf_model.h>
#include <gr/gr_calc_world_callback.h>
#include <memory.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <snd/snd_system.h>
#include <types.h>
#include <yk/yk_no_hit_normal.h>

#include <st_famicom/gr_famicom.h>
#include <st_famicom/gr_famicom_anim.h>

grFamicomBall::grFamicomBall(const char* taskName) : grFamicom(taskName), m_snd() {
    m_posWork = NULL;
    m_ltoRWork = NULL;
    m_posX = 0.0f;
    m_posXPrev = 0.0f;
    m_nodeBall = 0;
    m_motion = 1;
    m_prevFrame = 0.0f;
    m_hasHit = 0;
    m_attackSet = 0;
    m_attackWork = NULL;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_numNodeCallbackData = 1;
    callback->initialize(false, Heaps::StageInstance);
    callback->m_nodeCallbackDatas[0].m_flags |= 0x10;
    m_seHandle = -1;
    m_dangerZone = -1;
}

grFamicomBall* grFamicomBall::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grFamicomBall* ground = new (Heaps::StageInstance) grFamicomBall(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grFamicomBall::~grFamicomBall() {
    if (m_attackWork != NULL) {
        delete m_attackWork;
    }
    m_attackWork = NULL;
}

void grFamicomBall::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateYakumono(deltaFrame);
        updateMove(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

// The hit object is made the first time this is called; after that the sound follows the ball.
void grFamicomBall::updateYakumono(float deltaFrame) {
    if (m_hasHit == 1) {
        Vec3f pos;
        getNodePosition(&pos, 0, m_nodeIndex);
        setPos(&pos);
        m_snd.setPos(&pos);
    } else {
        setHit();
        if (m_yakumono != NULL) {
            m_hasHit = 1;
        }
    }
}

// The ball waits for the stage (the state of the ball is 0xD when there is none), then rolls over the stage in the animation
// (from the left to the right or the other way round) with a sound, and hurts the fighters it touches while it moves.
void grFamicomBall::updateMove(float deltaFrame) {
    switch (m_state) {
    case 0:
        setMotion(1, false, true, NULL);
        setVisibility(0);
        disableHit(0, 0);
        disableAttack(0);
        m_attackSet = 0;
        m_state = 1;
        // fall through
    case 1:
        if (*m_stateWork != 0xD) {
            if (*m_ltoRWork == 1) {
                setMotion(0, false, true, &m_prevFrame);
                if (m_motionRatio < 0.0f) {
                    m_motionRatio *= -1.0f;
                }
            } else {
                setMotion(0, true, true, &m_prevFrame);
                if (m_motionRatio > 0.0f) {
                    m_motionRatio *= -1.0f;
                }
            }
            m_seHandle = m_snd.playSE(static_cast<SndID>(0x1CE3), 0, 0, -1);
            m_state = 6;
        }
        break;
    case 6: {
        if (!m_isVisible) {
            setVisibility(1);
            enableHit(0, 0);
            Vec3f pos;
            getNodePosition(&pos, 0, m_nodeBall);
            m_posX = pos.m_x;
            m_posXPrev = pos.m_x;
        }
        bool finished = false;
        if (*m_ltoRWork == 1) {
            if (getMotionFrame(0) >= m_prevFrame) {
                finished = true;
            }
        } else {
            if (getMotionFrame(0) <= m_prevFrame) {
                m_prevFrame = getMotionFrame(0);
            } else {
                finished = true;
            }
        }
        if (finished) {
            if (m_seHandle != -1) {
                m_snd.stopSE(m_seHandle, 0);
            }
            setVisibility(0);
            disableHit(0, 0);
            if (m_dangerZone != -1) {
                g_aiMgr->delDangerZone(m_dangerZone);
                m_dangerZone = -1;
            }
            *m_stateWork = 0xD;
            m_state = 0;
            return;
        }
        Vec3f pos;
        getNodePosition(&pos, 0, m_nodeBall);
        m_posX = pos.m_x;
        if (m_posX == m_posXPrev) {
            disableAttack(0);
            m_attackSet = 0;
        } else {
            setAttack();
        }
        m_posXPrev = m_posX;
        break;
    }
    }
}

void grFamicomBall::updateCallBack(float deltaFrame) {
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
            Vec3f* posWork = m_posWork;
            if (posWork != NULL && m_stateWork != NULL) {
                Vec3f pos;
                switch (*m_stateWork) {
                case 9:
                    pos = posWork[0];
                    break;
                case 10:
                    pos = posWork[1];
                    break;
                case 0xB:
                    pos = posWork[2];
                    break;
                case 0xC:
                    pos = posWork[3];
                    break;
                default:
                    return;
                }
                grNodeCallbackData* data = calcWorldCallBack->m_nodeCallbackDatas;
                data->m_offsetPos.m_x = 0.0f;
                data->m_offsetPos.m_y = pos.m_y;
                data->m_offsetPos.m_z = 0.0f;
            }
        }
    }
}

bool grFamicomBall::setNode() {
    bool result = grGimmick::setNode();
    getNodeIndex(&m_nodeBall, 0, "greenBall1");
    return result;
}

// The hit object of the ball: an attack module with one attack part and no hit module.
void grFamicomBall::setHit() {
    m_attackWork = new (Heaps::StageInstance) ykData;
    m_attackWork->m_dataGroupNum = 0;
    m_attackWork->m_dataGroups = NULL;

    ykInitInfo info = { 0, 0, 0x10, 0, 0 };
    info.m_ground = this;
    info.m_node = ykDynamicCastScnMdl(m_sceneModels[0]);
    Vec3f pos = getPos();
    info.m_pos = &pos;
    info.m_work = m_attackWork;

    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Gimmick, 1, 0, soCollisionAttackModuleImpl, 1, false, true> Config;
    ykNoHitNormal<Config>* yakumono = new (Heaps::StageInstance) ykNoHitNormal<Config>(&info);
    setYakumono(yakumono);
}

// The ball burns (power 20) the fighters it touches and sends them away from it.
void grFamicomBall::setAttack() {
    if (m_attackSet != 1) {
        soCollisionAttackData attack(1.0f);
        Vec3f offset;
        offset.m_x = 0.0f;
        offset.m_y = 0.0f;
        offset.m_z = 0.0f;
        setAttackGimmickDetails(&attack, 5.0f, 1.0f, 1.0f, 1.0f,
            20, &offset, 30, 50, 0, 70, 0,
            0x2F7, 7, false, 15,
            soCollisionAttackData::Attribute_Fire, soCollisionAttackData::Sound_Level_Large,
            soCollisionAttackData::Sound_Attribute_Fire,
            false, false, false, true, false, false, 0, 90,
            false, false, false, soCollisionAttackData::Lr_Check_Pos,
            false, false, false, false, false, soCollisionAttackData::Region_None, true);
        m_yakumono->setAttack(0, 0, &attack);
        m_attackSet = 1;
    }
}

// The animations of the ball are bound with all their parts, the character animation first (only the animation 0 exists).
void grFamicomBall::setMotion(u32 animId, bool loop, bool force, float* frameCount) {
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

    if (animId >= 1) {
        return;
    }

    bool result = (modelAnim->m_resFile.GetResAnmChrNumEntries() > animId);
    if (result) {
        grFamicomSetChrAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmVisNumEntries() > animId);
    if (result) {
        grFamicomSetVisibilityAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmTexPatNumEntries() > animId);
    if (result) {
        grFamicomSetTexPatAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmTexSrtNumEntries() > animId);
    if (result) {
        grFamicomSetTexSrtAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmClrNumEntries() > animId);
    if (result) {
        grFamicomSetColorAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    gfModelAnimation::bind(sceneMdl, modelAnim);
    modelAnim->setFrame(0.0);
    modelAnim->setUpdateRate(1.0);
    modelAnim->setLoop(loop);

    if (frameCount != NULL) {
        *frameCount = modelAnim->getFrameCount();
    }
}
