#define YK_STAGE_INLINE // inline members of the shared SO headers that the stage RELs emit themselves
#define YK_STAGE_FULL // ... and the ones of the hit objects with an attack module and an event observer
#define SO_COLLISION_GROUP_ALIGNED // the hit object embeds soArrayVector<soCollisionGroup, 1> with 4-byte aligned elements
#define SO_COLLISION_GROUP_MEMBERWISE // ... and copies the group member by member

#include <ai/ai_mgr.h>
#include <gr/gr_calc_world_callback.h>
#include <memory.h>
#include <mt/mt_prng.h>
#include <snd/snd_system.h>
#include <string.h>
#include <yk/yk_no_hit_normal.h>

#include <st_donkey/gr_donkey.h>
#include <st_donkey/gr_donkey_anim.h>

grDonkeyKong* grDonkeyKong::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grDonkeyKong* ground = new (Heaps::StageInstance) grDonkeyKong(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grDonkeyKong::grDonkeyKong(const char* taskName) : grDonkey(taskName) {
    m_firstThrow = 1;
    m_posWork = NULL;
    m_offset.m_x = 0.0f;
    m_offset.m_y = 0.0f;
    m_offset.m_z = 0.0f;
    m_work = NULL;
    m_hasYakumono = 0;
    m_attackEnabled = 0;
    m_seTimer = 0.0f;
    m_seId = -1;
    m_seIdRoar = -1;
    m_dangerZone = -1;
    m_stateWork = NULL;
    m_stateJackWork = NULL;
    m_motion = 2;
    m_motionFrames = 0.0f;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_nodeCallbackDatas[0].m_flags |= 1;
}

grDonkeyKong::~grDonkeyKong() {
    if (m_work != NULL) {
        delete m_work;
    }
    m_work = NULL;
}

void grDonkeyKong::update(float deltaFrame) {
    if (m_isUpdate) {
        updateYakumono();
        updateScaleBase(deltaFrame);
        updateMove(deltaFrame);
        updateColor();
        updateCallBack(deltaFrame);
    }
}

// Creates the hit object once the model is there; while he stands at his throwing position (state 4) the AI is warned of
// the area around him, and the attack is switched off otherwise.
void grDonkeyKong::updateYakumono() {
    if (m_hasYakumono == 1) {
        switch (m_state) {
        case 4: {
            Vec2f zoneMin;
            Vec2f zoneMax;
            Vec3f pos;
            setAttack();
            pos = *m_posWork + m_offset;
            zoneMin.m_x = pos.m_x - 40.0f;
            zoneMin.m_y = pos.m_y - 10.0f;
            zoneMax.m_y = pos.m_y + 70.0f;
            zoneMax.m_x = pos.m_x + 40.0f;
            m_dangerZone = g_aiMgr->setDangerZone(&zoneMin, &zoneMax, m_dangerZone, false, false);
            break;
        }
        default:
            if (m_attackEnabled == 1) {
                disableAttack(0);
                m_attackEnabled = 0;
            }
            if (m_dangerZone != -1) {
                g_aiMgr->delDangerZone(m_dangerZone);
                m_dangerZone = -1;
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

// The walk of Donkey Kong: 0 sets up the start, 1 waits, 2 comes in (320 frames, with a sound), 3 shakes in to the throwing
// position (the model sinks 27 units into the girder and comes up), 4 throws (the attack is on), 5 sinks again.
// The stage data holds the waits and the chance to appear.
void grDonkeyKong::updateMove(float deltaFrame) {
    float* data = static_cast<float*>(getStageData());
    if (data != NULL) {
        m_timer -= deltaFrame;
        if (m_timer < 0.0f) {
            m_timer = 0.0f;
        }
        m_seTimer -= deltaFrame;
        if (m_seTimer < 0.0f) {
            m_seTimer = 0.0f;
        }
        switch (m_state) {
        case 0:
            setMotion(1, true, false, &m_motionFrames);
            if (m_firstThrow == 1) {
                m_timer = data[0];
            } else {
                m_timer = data[1];
            }
            m_firstThrow = 0;
            m_seId = -1;
            m_seIdRoar = -1;
            m_offset.m_x = 0.0f;
            m_offset.m_y = 0.0f;
            m_offset.m_z = -27.0f;
            m_state = 1;
            break;
        case 1:
            if (m_timer == 0.0f) {
                if (randf() >= data[2]) {
                    m_state = 0;
                } else {
                    m_seId = g_sndSystem->playSE(SndID(0x1B85), 0, 0, 0, -1);
                    m_timer = 320.0f;
                    m_seTimer = 340.0f;
                    m_state = 2;
                }
            }
            break;
        case 2:
            if (m_timer == 0.0f) {
                m_timer = data[3];
                *m_stateWork = 3;
                m_state = 3;
            }
            break;
        case 3:
            if (m_timer == 0.0f) {
                m_offset.m_x = 0.0f;
                m_offset.m_y = 0.0f;
                m_offset.m_z = 0.0f;
                m_state = 4;
                m_timer = data[4] + (data[5] - data[4]) * randf();
            } else {
                m_offset.m_x = 0.0f;
                m_offset.m_y = 0.0f;
                m_offset.m_z = (m_timer / 27.0f) * -27.0f;
            }
            break;
        case 4:
            if (m_timer == 0.0f) {
                m_timer = data[3];
                *m_stateWork = 6;
                m_state = 5;
            } else if (m_stateJackWork[0] == 3 || m_stateJackWork[1] == 3) {
                setMotion(0, true, false, &m_motionFrames);
                if (m_modelAnims[0] == NULL) {
                    return;
                }
                float frame = m_modelAnims[0]->getFrame();
                if (frame == 0.0f || frame == 30.0f || frame == 60.0f || frame == 75.0f) {
                    m_seIdRoar = m_snd.playSE(SndID(0x1B86), 0, 0, -1);
                }
            } else {
                setMotion(1, true, false, &m_motionFrames);
                if (m_seIdRoar != -1) {
                    m_snd.stopSE(m_seIdRoar, 5);
                    m_seIdRoar = -1;
                }
            }
            break;
        case 5:
            if (m_timer == 0.0f) {
                m_state = 0;
            } else {
                m_offset.m_x = 0.0f;
                m_offset.m_y = 0.0f;
                m_offset.m_z = (1.0f - m_timer / 27.0f) * -27.0f;
            }
            break;
        }
        if (m_seTimer == 0.0f && m_seId != -1) {
            g_sndSystem->stopSE(m_seId, 0);
            m_seId = -1;
        }
    }
}

// Darkens the model while it sinks into the girder (the tev colour of the first material goes from white to grey).
void grDonkeyKong::updateColor() {
    double rate;
    switch (m_state) {
    case 4:
        rate = 0.0;
        break;
    case 3:
        rate = m_timer / 27.0f;
        break;
    case 5:
        rate = 1.0f - m_timer / 27.0f;
        break;
    default:
        rate = 1.0;
        break;
    }
    nw4r::g3d::ResMatTevColor tevColor;
    nw4r::g3d::ResMat mat;
    GXColor color = { 0xFF, 0xFF, 0xFF, 0xFF };
    nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[0];
    if (scnMdl != NULL) {
        nw4r::g3d::ResMdl model = scnMdl->m_resMdl;
        if (model.IsValid()) {
            mat = model.GetResMat("donkey");
            if (mat.IsValid()) {
                nw4r::g3d::ScnMdl::CopiedMatAccess access(scnMdl, mat.ptr()->m_id);
                tevColor = access.GetResMatTevColor(0);
                if (tevColor.IsValid()) {
                    u8 shade = (int)(128.0 - (float)(rate * 64.0));
                    color.r = 0xFF;
                    color.g = shade;
                    color.b = shade;
                    color.a = shade;
                    tevColor.GXSetTevColor(GX_TEVREG1, color);
                    tevColor.DCStore(false);
                    mat.DCStore(false);
                }
            }
        }
    }
}

void grDonkeyKong::updateCallBack(float deltaFrame) {
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
            if (posWork != NULL) {
                Vec3f pos;
                pos = *posWork + m_offset;
                grNodeCallbackData* data = calcWorldCallBack->m_nodeCallbackDatas;
                data->m_pos.m_x = pos.m_x;
                data->m_pos.m_y = pos.m_y;
                data->m_pos.m_z = pos.m_z;
                Vec3f scale;
                donkeyVec3Scale(&scale, &m_scaleBase, 0.9f);
                data = calcWorldCallBack->m_nodeCallbackDatas;
                data->m_scale.m_x = scale.m_x;
                data->m_scale.m_y = scale.m_y;
                data->m_scale.m_z = scale.m_z;
                m_snd.setPos(&pos);
            }
        }
    }
}

// Builds the hit object: one attack part (his body) and one collision group, no hit module.
void grDonkeyKong::setHit() {
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

// His body hits for 20 percent and knocks fighters away at 361 degrees (the Sakurai angle).
void grDonkeyKong::setAttack() {
    if (m_attackEnabled == 1) {
        return;
    }

    soCollisionAttackData attack(1.0f);
    Vec3f offset;
    getNodePosition(&offset, 0, m_nodeIndex);
    offset.m_z = -offset.m_z;
    offset.m_x = 0.0f;
    offset.m_y = 20.0f;

    setAttackGimmickDetails(&attack, 20.0f, 1.0f, 1.0f, 1.0f,
        20, &offset, 361, 50, 0, 78, m_nodeIndex,
        0x3FF, 7, false, 15,
        soCollisionAttackData::Attribute_Normal, soCollisionAttackData::Sound_Level_Medium,
        soCollisionAttackData::Sound_Attribute_Kick,
        false, false, false, true, false, false, 0, 60,
        false, false, false, soCollisionAttackData::Lr_Check_Pos,
        false, false, false, false, false, soCollisionAttackData::Region_None, false);
    m_yakumono->setAttack(0, 0, &attack);
    m_attackEnabled = 1;
}

// Two animations: 0 beats his chest, 1 stands.
void grDonkeyKong::setMotion(u32 animId, bool shouldLoop, bool force, float* frameCount) {
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

    if (animId >= 2) {
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
