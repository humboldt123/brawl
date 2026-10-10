#define YK_STAGE_INLINE // inline members of the shared SO headers that the stage RELs emit themselves
#define SO_COLLISION_GROUP_ALIGNED // the hit object embeds soArrayVector<soCollisionGroup, 1> with 4-byte aligned elements
#include <ai/ai_mgr.h>
#include <ec/ec_mgr.h>
#include <ef/ef_id.h>
#include <gr/gr_calc_world_callback.h>
#include <mt/mt_prng.h>
#include <snd/snd_id.h>
#include <yk/yk_no_hit_normal.h>

#include <st_dxgarden/gr_dxgarden.h>
#include <st_dxgarden/gr_dxgarden_anim.h>

grDxGardenKrap* grDxGardenKrap::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grDxGardenKrap* ground = new (Heaps::StageInstance) grDxGardenKrap(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grDxGardenKrap::grDxGardenKrap(const char* taskName) : grDxGarden(taskName) {
    m_animId = 1;
    m_frames = 0.0f;
    m_biteFrame = 0.0f;
    m_pos.m_x = 0.0f;
    m_pos.m_y = 0.0f;
    m_pos.m_z = 0.0f;
    m_effectHandle = 0;
    m_effectOn = 0;
    m_hasYakumono = 0;
    m_attackSet = 0;
    m_work = NULL;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_numNodeCallbackData = 1;
    callback->initialize(false, Heaps::StageInstance);
    callback->m_nodeCallbackDatas[0].m_flags |= 0x10;
    m_seIds[0] = snd_se_stage_Garden_swim1;
    m_seIds[1] = snd_se_stage_Garden_swim2;
    m_seData[0].id = snd_se_stage_Garden_swim1;
    m_seData[0].unk4 = 0.0f;
    m_seData[0].unk8 = 40.0f;
    m_seData[0].unkC = 0.0f;
    m_seData[1].id = snd_se_stage_Garden_swim2;
    m_seData[1].unk4 = 0.0f;
    m_seData[1].unk8 = 107.0f;
    m_seData[1].unkC = 0.0f;
    m_seData[2].id = snd_se_stage_Garden_swim1;
    m_seData[2].unk4 = 0.0f;
    m_seData[2].unk8 = 140.0f;
    m_seData[2].unkC = 0.0f;
    m_seData[3].id = snd_se_stage_Garden_swim2;
    m_seData[3].unk4 = 0.0f;
    m_seData[3].unk8 = 210.0f;
    m_seData[3].unkC = 0.0f;
    m_sePlayer.registId(m_seIds, 2);
    m_sePlayer.registSeq(0, m_seData, 4, Heaps::StageInstance);
    m_sePlayer.m_sndGenerator = &m_snd;
}

grDxGardenKrap::~grDxGardenKrap() {
    if (m_work != NULL) {
        delete m_work;
    }
    m_work = NULL;
}

void grDxGardenKrap::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    setNode();
    if (m_isUpdate) {
        updateActive(deltaFrame);
        updateYakumono(deltaFrame);
        updateEffect(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

// The crocodile waits (state 0 -> 1), comes up at a random place when its timer is over, bites for part of its animation
// and dives again when the animation is over.
void grDxGardenKrap::updateActive(float deltaFrame) {
    float* data = static_cast<float*>(getStageData());
    if (data != NULL) {
        m_timer -= deltaFrame;
        if (m_timer < 0.0f) {
            m_timer = 0.0f;
        }
        m_frames -= deltaFrame;
        if (m_frames < 0.0f) {
            m_frames = 0.0f;
        }
        switch (m_state) {
        case 0:
            setMotion(1, false, true, NULL);
            disableAttack(0);
            m_attackSet = 0;
            m_timer = 300.0f;
            m_state = 1;
            break;
        case 1:
            if (0.0f == m_timer) {
                setMotion(0, false, true, &m_frames);
                m_sePlayer.playFrame(0, getMotionFrame(0), 0.0f);
                m_biteFrame = 0.0f;
                m_pos.m_x = data[5] + (data[6] - data[5]) * randf();
                m_state = 2;
            }
            break;
        case 2:
            if (0.0f == m_frames) {
                m_state = 0;
            } else {
                m_biteFrame += deltaFrame;
                if (m_biteFrame > 210.0f) {
                    disableAttack(0);
                    m_attackSet = 0;
                } else if (m_biteFrame > 148.0f) {
                    setAttack();
                } else if (m_biteFrame > 110.0f) {
                    disableAttack(0);
                    m_attackSet = 0;
                } else if (m_biteFrame > 38.0f) {
                    setAttack();
                } else {
                    disableAttack(0);
                    m_attackSet = 0;
                }
                m_sePlayer.playFrame(0, getMotionFrame(0));
            }
            break;
        }
    }
}

// Creates the hit object once the crocodile exists.
void grDxGardenKrap::updateYakumono(float deltaFrame) {
    if (m_hasYakumono != 1) {
        setHit();
        if (m_yakumono != NULL) {
            m_hasYakumono = 1;
        }
    }
}

// The spray effect follows the mouth while the attack is on.
void grDxGardenKrap::updateEffect(float deltaFrame) {
    if (m_attackSet == 1) {
        if (m_effectOn == 0) {
            Vec3f pos;
            getNodePosition(&pos, 0, "MouseN");
            m_effectHandle = g_ecMgr->setEffect(ef_ptc_stg_dx_garden_wani, &pos);
            m_effectOn = 1;
        } else {
            Vec3f pos;
            getNodePosition(&pos, 0, "MouseN");
            g_ecMgr->setPos(m_effectHandle, &pos);
        }
    } else if (m_effectOn == 1) {
        g_ecMgr->endEffect(m_effectHandle);
        m_effectOn = 0;
    }
}

void grDxGardenKrap::updateCallBack(float deltaFrame) {
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
            grNodeCallbackData* data = calcWorldCallBack->m_nodeCallbackDatas;
            data[0].m_offsetPos.m_x = m_pos.m_x;
            data[0].m_offsetPos.m_y = m_pos.m_y;
            data[0].m_offsetPos.m_z = m_pos.m_z;
            Vec3f pos;
            getNodePosition(&pos, 0, calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex);
            m_snd.setPos(&pos);
        }
    }
}

// Builds the crocodile's hit object: one attack part, one collision group and no hit module (a ykNoHitNormal), at the position
// the model reports.
void grDxGardenKrap::setHit() {
    m_work = new (Heaps::StageInstance) grDxGardenKrapWork;
    m_work->unk0 = 0;
    m_work->unk4 = 0;

    ykInitInfo info = {NULL, NULL, 0x16, NULL, NULL};
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

// The bite: a box around the mouth that throws the fighters (same attack data as the cars of Mute City, set the same way).
void grDxGardenKrap::setAttack() {
    if (m_attackSet != 1) {
        soCollisionAttackData attack;
        Vec3f offset;
        offset.m_x = 0.0f;
        offset.m_y = 0.0f;
        offset.m_z = 0.0f;
        setAttackGimmickDetails(&attack, 6.0f, 1.0f, 1.0f, 1.0f,
            30, &offset, 245, 100, 0, 10, m_nodeIndex,
            0x3FF, 7, false, 15,
            soCollisionAttackData::Attribute_Cutup, soCollisionAttackData::Sound_Level_Small,
            soCollisionAttackData::Sound_Attribute_Fire,
            false, false, false, true, false, false, 0, 32,
            false, false, false, soCollisionAttackData::Lr_Check_Pos,
            false, false, false, false, false, soCollisionAttackData::Region_None, true);
        m_yakumono->setAttack(0, 0, &attack);
        m_attackSet = 1;
    }
}

void grDxGardenKrap::setMotion(u32 animId, bool shouldLoop, bool force, float* frameCount) {
    if (m_animId == animId && force == 0) {
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
    m_animId = animId;

    if (animId >= 1) {
        return;
    }

    bool result = (modelAnim->m_resFile.GetResAnmChrNumEntries() > animId);
    if (result) {
        grDxGardenSetChrAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmVisNumEntries() > animId);
    if (result) {
        grDxGardenSetVisibilityAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmTexPatNumEntries() > animId);
    if (result) {
        grDxGardenSetTexPatAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmTexSrtNumEntries() > animId);
    if (result) {
        grDxGardenSetTexSrtAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmClrNumEntries() > animId);
    if (result) {
        grDxGardenSetColorAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    gfModelAnimation::bind(sceneMdl, modelAnim);
    modelAnim->setFrame(0.0);
    modelAnim->setUpdateRate(1.0);
    modelAnim->setLoop(shouldLoop);

    if (frameCount != NULL) {
        *frameCount = modelAnim->getFrameCount();
    }
}

// Leaf functions shared with other fighter modules (same module-map names and bodies).
extern "C" {
void fn_77_2FB8() {}
u8 fn_77_2FC4(u8* p) { return *(u8*)(p + 0x44); }
void fn_77_30D0() {}
} // extern "C"
