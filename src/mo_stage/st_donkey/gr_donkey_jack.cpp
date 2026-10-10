#define YK_STAGE_INLINE // inline members of the shared SO headers that the stage RELs emit themselves
#define YK_STAGE_FULL // ... and the ones of the hit objects with an attack module and an event observer
#define SO_COLLISION_GROUP_ALIGNED // the hit object embeds soArrayVector<soCollisionGroup, 1> with 4-byte aligned elements
#define SO_COLLISION_GROUP_MEMBERWISE // ... and copies the group member by member

#include <gr/gr_calc_world_callback.h>
#include <memory.h>
#include <mt/mt_prng.h>
#include <snd/snd_system.h>
#include <string.h>
#include <yk/yk_no_hit_normal.h>

#include <st_donkey/gr_donkey.h>
#include <st_donkey/gr_donkey_anim.h>

grDonkeyJack* grDonkeyJack::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grDonkeyJack* ground = new (Heaps::StageInstance) grDonkeyJack(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grDonkeyJack::grDonkeyJack(const char* taskName) : grDonkey(taskName) {
    m_firstSpawn = 1;
    m_posWork = NULL;
    m_offset.m_x = 0.0f;
    m_offset.m_y = 0.0f;
    m_offset.m_z = 0.0f;
    m_hasYakumono = 0;
    m_attackEnabled = 0;
    m_attackSide = 0;
    m_work = NULL;
    m_motion = 1;
    m_unk188 = 0.0f;
    m_motionFrames = 0.0f;
    m_seId = -1;
    m_seIdLand = -1;
    m_stateWork = NULL;
    // The splash of the jump: the same sound at frames 1, 29, 59, 89 and 119.
    m_seSeqId = static_cast<SndID>(0x1B87);
    m_seSeqData[0].id = static_cast<SndID>(0x1B87);
    m_seSeqData[0].unk4 = 0.0f;
    m_seSeqData[0].unk8 = 1.0f;
    m_seSeqData[0].unkC = 0.0f;
    m_seSeqData[1].id = static_cast<SndID>(0x1B87);
    m_seSeqData[1].unk4 = 0.0f;
    m_seSeqData[1].unk8 = 29.0f;
    m_seSeqData[1].unkC = 0.0f;
    m_seSeqData[2].id = static_cast<SndID>(0x1B87);
    m_seSeqData[2].unk4 = 0.0f;
    m_seSeqData[2].unk8 = 59.0f;
    m_seSeqData[2].unkC = 0.0f;
    m_seSeqData[3].id = static_cast<SndID>(0x1B87);
    m_seSeqData[3].unk4 = 0.0f;
    m_seSeqData[3].unk8 = 89.0f;
    m_seSeqData[3].unkC = 0.0f;
    m_seSeqData[4].id = static_cast<SndID>(0x1B87);
    m_seSeqData[4].unk4 = 0.0f;
    m_seSeqData[4].unk8 = 119.0f;
    m_seSeqData[4].unkC = 0.0f;
    m_seSeq.registId(&m_seSeqId, 1);
    m_seSeq.registSeq(0, m_seSeqData, 5, Heaps::StageInstance);
    m_seSeq.m_sndGenerator = &m_snd;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_nodeCallbackDatas[0].m_flags |= 1;
}

grDonkeyJack::~grDonkeyJack() {
    if (m_work != NULL) {
        delete m_work;
    }
    m_work = NULL;
}

void grDonkeyJack::update(float deltaFrame) {
    if (m_isUpdate) {
        updateYakumono();
        updateScaleBase(deltaFrame);
        updateMove(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

// While he is in the air (state 7) the hit object follows the model; the attack is switched off otherwise.
void grDonkeyJack::updateYakumono() {
    if (m_hasYakumono == 1) {
        switch (m_state) {
        case 7: {
            Vec3f pos;
            getNodePosition(&pos, 0, m_nodeIndex);
            setPos(&pos);
            m_snd.setPos(&pos);
            setAttack();
            break;
        }
        default:
            if (m_attackEnabled == 1) {
                disableAttack(0);
                m_attackEnabled = 0;
            }
            break;
        }
    } else {
        setHit();
        if (m_yakumono != NULL) {
            m_hasYakumono = 1;
        }
    }
}

// The jump of a Jack: 0 hides him, 1 waits for the order of the stage (state 3 of the stage data), then he jumps (state 7)
// with a random offset to the side and the splashes play. After the motion he is back at 0 and tells the stage (6).
// The stage data holds the half width of the random offset at index 15.
void grDonkeyJack::updateMove(float deltaFrame) {
    float* data = static_cast<float*>(getStageData());
    if (data != NULL) {
        m_timer -= deltaFrame;
        if (m_timer < 0.0f) {
            m_timer = 0.0f;
        }
        switch (m_state) {
        case 0:
            setMotion(1, false, true, NULL);
            setVisibility(0);
            disableAttack(0);
            m_attackEnabled = 0;
            m_state = 1;
            // HYPOTHESIS: falls through, the waiting is checked right away
        case 1:
            if (*m_stateWork == 3) {
                setMotion(0, false, true, &m_motionFrames);
                m_offset.m_x = -data[15] + data[15] * 2.0f * randf();
                m_seSeq.playFrame(0, getMotionFrame(0), 0.0f);
                m_seIdLand = -1;
                m_state = 7;
            }
            break;
        case 7:
            if (!m_isVisible) {
                setVisibility(1);
            }
            if (getMotionFrame(0) >= m_motionFrames) {
                *m_stateWork = 6;
                m_state = 0;
                return;
            }
            if (getMotionFrame(0) > 150.0f) {
                if (m_seId != -1) {
                    m_snd.stopSE(m_seId, 5);
                    m_seId = -1;
                }
                if (m_seIdLand != -1) {
                    return;
                }
                m_seIdLand = m_snd.playSE(SndID(0x1B88), 0, 0, -1);
            } else {
                m_seSeq.playFrame(0, getMotionFrame(0));
            }
            break;
        }
    }
}

void grDonkeyJack::updateCallBack(float deltaFrame) {
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
            grNodeCallbackData* data = calcWorldCallBack->m_nodeCallbackDatas;
            data->m_offsetPos.m_x = m_offset.m_x;
            data->m_offsetPos.m_y = m_offset.m_y;
            data->m_offsetPos.m_z = m_offset.m_z;
            Vec3f scale;
            donkeyVec3Scale(&scale, &m_scaleBase, 0.9f);
            data = calcWorldCallBack->m_nodeCallbackDatas;
            data->m_scale.m_x = scale.m_x;
            data->m_scale.m_y = scale.m_y;
            data->m_scale.m_z = scale.m_z;
        }
    }
}

// Builds the hit object: one attack part (his body) and one collision group, no hit module.
void grDonkeyJack::setHit() {
    m_work = new (Heaps::StageInstance) grDonkeyWork;
    m_work->unk0 = 0;
    m_work->unk4 = 0;

    ykInitInfo info = {NULL, NULL, 0x10, NULL, NULL};
    info.m_ground = this;
    nw4r::g3d::ScnMdl* model = ykDynamicCastScnMdl(m_sceneModels[0]);
    info.m_node = model;
    Vec3f pos = getPos();
    info.m_pos = &pos;
    info.m_work = m_work;

    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Gimmick, 1, 0, soCollisionAttackModuleImpl, 1, false, true> Config;
    ykNoHitNormal<Config>* yakumono = new (Heaps::StageInstance) ykNoHitNormal<Config>(&info);
    setYakumono(yakumono);
}

// Swings down for the first 90 frames of the jump and to the side after that.
void grDonkeyJack::setAttack() {
    if (getMotionFrame(0) < 90.0f) {
        setAttackDown();
        m_attackSide = 0;
    } else {
        if (m_attackSide == 0) {
            disableAttack(0);
            m_attackEnabled = 0;
        }
        setAttackSide();
        m_attackSide = 1;
    }
}

// Hits for 20 percent and sends fighters at 20 degrees, away from where he is facing.
void grDonkeyJack::setAttackDown() {
    if (m_attackEnabled != 1) {
        soCollisionAttackData attack(1.0f);
        Vec3f offset;
        getNodePosition(&offset, 0, m_nodeIndex);
        offset.m_z = -offset.m_z;
        offset.m_x = 0.0f;
        offset.m_y = 8.0f;

        setAttackGimmickDetails(&attack, 10.0f, 1.0f, 1.0f, 1.0f,
            20, &offset, 20, 42, 0, 90, m_nodeIndex,
            0x3FF, 7, false, 15,
            soCollisionAttackData::Attribute_Normal, soCollisionAttackData::Sound_Level_Large,
            soCollisionAttackData::Sound_Attribute_Kick,
            false, false, false, true, false, false, 0, 0,
            false, false, false, soCollisionAttackData::Lr_Check_Forward,
            false, false, false, false, false, soCollisionAttackData::Region_None, false);
        m_yakumono->setAttack(0, 0, &attack);
        m_attackEnabled = 1;
    }
}

// The same hit, but the fighters are only sent away from the hit object.
void grDonkeyJack::setAttackSide() {
    if (m_attackEnabled != 1) {
        soCollisionAttackData attack(1.0f);
        Vec3f offset;
        getNodePosition(&offset, 0, m_nodeIndex);
        offset.m_z = -offset.m_z;
        offset.m_x = 0.0f;
        offset.m_y = 8.0f;

        setAttackGimmickDetails(&attack, 10.0f, 1.0f, 1.0f, 1.0f,
            20, &offset, 20, 50, 0, 90, m_nodeIndex,
            0x3FF, 7, false, 15,
            soCollisionAttackData::Attribute_Normal, soCollisionAttackData::Sound_Level_Large,
            soCollisionAttackData::Sound_Attribute_Kick,
            false, false, false, true, false, false, 0, 0,
            false, false, false, soCollisionAttackData::Lr_Check_Pos,
            false, false, false, false, false, soCollisionAttackData::Region_None, false);
        m_yakumono->setAttack(0, 0, &attack);
        m_attackEnabled = 1;
    }
}

// Only one animation is played (0, the jump); 1 is the idle pose without an animation.
void grDonkeyJack::setMotion(u32 animId, bool shouldLoop, bool force, float* frameCount) {
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
        grDonkeySetChrAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmVisNumEntries() > animId);
    if (result) {
        grDonkeySetVisibilityAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmTexPatNumEntries() > animId);
    if (result) {
        grDonkeySetTexPatAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmTexSrtNumEntries() > animId);
    if (result) {
        grDonkeySetTexSrtAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmClrNumEntries() > animId);
    if (result) {
        grDonkeySetColorAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    gfModelAnimation::bind(sceneMdl, modelAnim);
    modelAnim->setFrame(0.0);
    modelAnim->setUpdateRate(1.0);
    modelAnim->setLoop(shouldLoop);

    if (frameCount != NULL) {
        *frameCount = modelAnim->getFrameCount();
    }
}
