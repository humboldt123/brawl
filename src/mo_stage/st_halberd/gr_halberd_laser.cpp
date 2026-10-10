#define YK_STAGE_INLINE // inline members of the shared SO headers that the stage RELs emit themselves
#define YK_STAGE_FULL // ... and the ones of the hit objects with an attack module and an event observer
#define SO_COLLISION_GROUP_ALIGNED // the hit object embeds soArrayVector<soCollisionGroup, 1> with 4-byte aligned elements
#define SO_COLLISION_GROUP_MEMBERWISE // ... and copies the group member by member

#include <ai/ai_mgr.h>
#include <cm/cm_quake.h>
#include <gr/gr_calc_world_callback.h>
#include <math.h>
#include <memory.h>
#include <string.h>
#include <yk/yk_no_hit_normal.h>

#include <st_halberd/gr_halberd.h>
#include <st_halberd/gr_halberd_anim.h>

grHalberdLaser* grHalberdLaser::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grHalberdLaser* ground = new (Heaps::StageInstance) grHalberdLaser(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grHalberdLaser::grHalberdLaser(const char* taskName) : grHalberd(taskName) {
    m_mtxWork = NULL;
    m_posTgtWork = NULL;
    m_motion = 3;
    m_timer2 = 0.0f;
    m_length = 0.0f;
    m_yakumonoMade = 0;
    m_attackOn = 0;
    m_work = NULL;
    m_dangerZone = -1;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_numNodeCallbackData = 1;
    callback->initialize(false, Heaps::StageInstance);
    callback->m_nodeCallbackDatas[0].m_flags |= 8;
}

grHalberdLaser::~grHalberdLaser() {
    if (m_work != NULL) {
        delete m_work;
    }
    m_work = NULL;
}

void grHalberdLaser::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateYakumono(deltaFrame);
        updateMotion(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

// The beam hurts only in the last frames of its motion: it is switched on a while after the laser fires and the final
// shot (a bigger one) is made 2 frames before the beam goes out. The AI is told where the beam hits.
void grHalberdLaser::updateYakumono(float deltaFrame) {
    if (m_yakumonoMade == 1) {
        if (m_motion == 1) {
            float elapsed = m_length - m_timer2;
            if (elapsed > 137.0f) {
                disableAttack(0);
                m_attackOn = 0;
                if (m_dangerZone != -1) {
                    g_aiMgr->delDangerZone(m_dangerZone);
                    m_dangerZone = -1;
                }
            } else if (elapsed > 135.0f) {
                disableAttack(0);
                m_attackOn = 0;
                setAttackFinal();
            } else if (elapsed > 10.0f) {
                setAttack();
                float dx = m_posTgtWork->m_x - m_mtxWork->m[0][3];
                float dy = m_posTgtWork->m_y - m_mtxWork->m[1][3];
                float dz = m_posTgtWork->m_z - m_mtxWork->m[2][3];
                float length = grHalberdLength(dz, dx, dy);
                Vec3f end;
                end.m_x = length * 0.0f;
                end.m_z = length * 1.0f;
                end.m_y = end.m_x;
                m_mtxWork->mulPos(&end, &end);
                Vec2f zoneMin(end.m_x - 20.0f, end.m_y + 20.0f);
                Vec2f zoneMax(end.m_x + 20.0f, end.m_y - 20.0f);
                m_dangerZone = g_aiMgr->setDangerZone(&zoneMin, &zoneMax, m_dangerZone, 0, 0);
            }
        }
    } else {
        setHit();
        if (m_yakumono != NULL) {
            m_yakumonoMade = 1;
        }
    }
}

void grHalberdLaser::updateMotion(float deltaFrame) {
    if (m_stateWork == NULL) {
        return;
    }
    m_timer2 -= deltaFrame;
    if (m_timer2 < 0.0f) {
        m_timer2 = 0.0f;
    }
    switch (m_state) {
    case 8: {
        Vec3f offset(0.0f, 0.0f, 0.0f);
        cmReqQuake(cmQuake::Amplitude_S, &offset);
        break;
    }
    case 7:
        if (*m_stateWork == 10) {
            setMotion(1, 0, 1, &m_timer2);
            m_state = 8;
            m_length = m_timer2;
        }
        break;
    case 0:
        setMotion(3, 0, 1, NULL);
        setVisibility(0);
        m_state = 2;
        // fall through
    case 2:
        if (*m_stateWork == 9) {
            setMotion(2, 0, 1, &m_timer2);
            setVisibility(1);
            m_state = 7;
        }
        break;
    }
    if (m_timer2 == 0.0f) {
        u8 motion = m_motion;
        if (motion == 2) {
            setMotion(0, 1, 1, NULL);
        } else if (motion < 2 && motion != 0) {
            *m_stateWork = 0xB;
            m_state = 0;
        }
    }
}

void grHalberdLaser::updateCallBack(float deltaFrame) {
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
                calcWorldCallBack->m_nodeCallbackDatas[0].m_matrix = *m_mtxWork;
            }
        }
    }
}

// The animation of the laser (3 animations).
void grHalberdLaser::setMotion(u32 animId, bool shouldLoop, bool force, float* frameCount) {
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

void grHalberdLaser::setHit() {
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

// The beam: 1 damage, kept for 125 frames; the final shot hurts 5.
void grHalberdLaser::setAttack() {
    if (m_attackOn != 1) {
        soCollisionAttackData attack(1.0f);
        Vec3f offset;
        offset.m_x = m_posTgtWork->m_x - m_mtxWork->m[0][3];
        offset.m_y = m_posTgtWork->m_y - m_mtxWork->m[1][3];
        offset.m_z = m_posTgtWork->m_z - m_mtxWork->m[2][3];
        float length = grHalberdLength(offset.m_z, offset.m_x, offset.m_y);
        offset.m_z = length * 1.0f;
        offset.m_x = length * 0.0f;
        offset.m_y = offset.m_x;

        setAttackGimmickDetails(&attack, 20.0f, 1.0f, 1.0f, 1.0f,
            1, &offset, 45, 100, 10, 0, m_nodeIndex,
            0x3FF, 7, false, 15,
            soCollisionAttackData::Attribute_Normal, soCollisionAttackData::Sound_Level_Small,
            soCollisionAttackData::Sound_Attribute_Magic,
            false, false, false, true, false, false, 0, 4,
            false, false, false, soCollisionAttackData::Lr_Check_Pos,
            false, false, false, false, false, soCollisionAttackData::Region_None, false);
        m_yakumono->setAttack(0, 0, &attack);
        m_attackOn = 1;
    }
}

void grHalberdLaser::setAttackFinal() {
    if (m_attackOn != 1) {
        soCollisionAttackData attack(1.0f);
        Vec3f offset;
        offset.m_x = m_posTgtWork->m_x - m_mtxWork->m[0][3];
        offset.m_y = m_posTgtWork->m_y - m_mtxWork->m[1][3];
        offset.m_z = m_posTgtWork->m_z - m_mtxWork->m[2][3];
        float length = grHalberdLength(offset.m_z, offset.m_x, offset.m_y);
        offset.m_z = length * 1.0f;
        offset.m_x = length * 0.0f;
        offset.m_y = offset.m_x;

        setAttackGimmickDetails(&attack, 30.0f, 1.0f, 1.0f, 1.0f,
            5, &offset, 45, 150, 0, 80, m_nodeIndex,
            0x3FF, 7, false, 15,
            soCollisionAttackData::Attribute_Normal, soCollisionAttackData::Sound_Level_Small,
            soCollisionAttackData::Sound_Attribute_Magic,
            false, false, false, true, false, false, 0, 60,
            false, false, false, soCollisionAttackData::Lr_Check_Pos,
            false, false, false, false, false, soCollisionAttackData::Region_None, false);
        m_yakumono->setAttack(0, 0, &attack);
        m_attackOn = 1;
    }
}
