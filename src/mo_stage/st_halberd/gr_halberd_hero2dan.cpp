#define YK_STAGE_INLINE // inline members of the shared SO headers that the stage RELs emit themselves
#define YK_STAGE_FULL // ... and the ones of the hit objects with an attack module and an event observer
#define SO_COLLISION_GROUP_ALIGNED // the hit object embeds soArrayVector<soCollisionGroup, 1> with 4-byte aligned elements
#define SO_COLLISION_GROUP_MEMBERWISE // ... and copies the group member by member

#include <ai/ai_mgr.h>
#include <cm/cm_quake.h>
#include <ec/ec_mgr.h>
#include <gr/gr_calc_world_callback.h>
#include <math.h>
#include <memory.h>
#include <nw4r/math/math_triangular.h>
#include <snd/snd_system.h>
#include <string.h>
#include <yk/yk_no_hit_normal.h>

#include <st_halberd/gr_halberd.h>
#include <st_halberd/gr_halberd_anim.h>

// HYPOTHESIS: sets the pitch of the sounds of the sound system (an unnamed function of the main binary).
extern "C" void fn_800778CC(sndSystem* system, float pitch);

grHalberdHero2Dan* grHalberdHero2Dan::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grHalberdHero2Dan* ground = new (Heaps::StageInstance) grHalberdHero2Dan(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grHalberdHero2Dan::grHalberdHero2Dan(const char* taskName) : grHalberd(taskName), m_subject(0, 1) {
    m_mtxWork = NULL;
    reinterpret_cast<Matrix*>(&m_mtx0)->setIdentity();
    reinterpret_cast<Matrix*>(&m_mtx1)->setIdentity();
    m_posTgtWork = NULL;
    m_pos.m_x = 0.0f;
    m_pos.m_y = 0.0f;
    m_pos.m_z = 0.0f;
    m_motion = 1;
    m_flightTimer = 0.0f;
    m_yakumonoMade = 0;
    m_unk1dd = 0;
    m_work = NULL;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_numNodeCallbackData = 1;
    callback->initialize(false, Heaps::StageInstance);
    callback->m_nodeCallbackDatas[0].m_flags |= 1;
    m_subject.clear();
    m_seId = -1;
    m_subject.m_state = 1;
    m_dangerZone = -1;
}

grHalberdHero2Dan::~grHalberdHero2Dan() {
    if (m_work != NULL) {
        delete m_work;
    }
    m_work = NULL;
}

void grHalberdHero2Dan::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateYakumono(deltaFrame);
        updateMotion(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

void grHalberdHero2Dan::updateYakumono(float deltaFrame) {
    if (m_yakumonoMade != 1) {
        setHit();
        if (m_yakumono != NULL) {
            m_yakumonoMade = 1;
        }
    }
}

// The shell of the rear gun: it waits in the gun (state 2), flies to the target in an arc (8) and bombs there (9).
void grHalberdHero2Dan::updateMotion(float deltaFrame) {
    if (m_stateWork == NULL) {
        return;
    }
    stHalberdData* data = static_cast<stHalberdData*>(getStageData());
    if (data == NULL) {
        return;
    }
    m_timer -= deltaFrame;
    if (m_timer < 0.0f) {
        m_timer = 0.0f;
    }
    m_flightTimer -= deltaFrame;
    if (m_flightTimer < 0.0f) {
        m_flightTimer = 0.0f;
    }
    u8 state = m_state;
    if (state != 2) {
        if (state > 1) {
            if (state == 9) {
                if (m_timer != 0.0f) {
                    return;
                }
                disableAttack(1);
                m_state = 0;
                return;
            }
            if (state > 8) {
                return;
            }
            if (state < 8) {
                return;
            }
            float rate = 1.0f - m_timer / data->unkAC;
            if (rate < 0.0f) {
                rate = 0.0f;
            }
            if (rate > 1.0f) {
                rate = 1.0f;
            }
            Vec3f start(m_mtx1.m[0][3], m_mtx1.m[1][3], m_mtx1.m[2][3]);
            Vec3f origin(m_mtx0.m[0][3], m_mtx0.m[1][3], m_mtx0.m[2][3]);
            Vec3f* target = m_posTgtWork;
            Vec3f dir(target->m_x - start.m_x, target->m_y - start.m_y, target->m_z - start.m_z);
            float distance = grHalberdLength(dir.m_z, dir.m_x, dir.m_y);
            float rise = dir.m_y;
            dir.normalize();
            float step = distance * rate;
            m_pos.m_x = start.m_x + dir.m_x * step;
            m_pos.m_y = start.m_y + dir.m_y * step;
            m_pos.m_z = start.m_z + dir.m_z * step;
            Vec3f toTarget(target->m_x - origin.m_x, target->m_y - origin.m_y, target->m_z - origin.m_z);
            float reach = grHalberdLength(toTarget.m_z, toTarget.m_x, toTarget.m_y);
            Vec3f toStart(start.m_x - origin.m_x, start.m_y - origin.m_y, start.m_z - origin.m_z);
            toStart.normalize();
            float angle = nw4r::math::Atan2FIdx(toStart.m_z, toStart.m_y) * 1.40625f - 90.0f;
            if (angle > 180.0f) {
                angle = angle - 360.0f;
            }
            float bend = fabsf(angle) / data->unkA0;
            if (bend < 0.0f) {
                bend = 0.0f;
            }
            if (bend > 1.0f) {
                bend = 1.0f;
            }
            float arc = (reach * 0.35f) * bend;
            float sine = nw4r::math::SinFIdx(static_cast<float>(static_cast<s16>(static_cast<int>(rate * 32768.0f))) * (1.0f / 256.0f));
            m_pos.m_y = start.m_y + arc * sine + rise * rate;
            m_snd.setPos(&m_pos);
            if (m_seId != -1) {
                fn_800778CC(g_sndSystem, 1.0f - rate * 0.1f);
            }
            Vec2f zoneMin(m_posTgtWork->m_x - 30.0f, m_posTgtWork->m_y + 30.0f);
            Vec2f zoneMax(m_posTgtWork->m_x + 30.0f, m_posTgtWork->m_y - 30.0f);
            m_dangerZone = g_aiMgr->setDangerZone(&zoneMin, &zoneMax, m_dangerZone, 0, 0);
            if (rate != 1.0f) {
                return;
            }
            bomb();
            m_state = 9;
            return;
        }
        if (state != 0) {
            return;
        }
        setMotion(1, 0, 1, NULL);
        setVisibility(0);
        m_state = 2;
    }
    if (*m_stateWork == 0x10) {
        setMotion(0, 1, 1, NULL);
        setVisibility(1);
        m_seId = m_snd.playSE(snd_se_stage_Halberd_Hero_shot, 0, 0, -1);
        setAttack(0);
        stHalberdMtx* mtxs = reinterpret_cast<stHalberdMtx*>(m_mtxWork);
        m_mtx0 = mtxs[8];
        m_mtx1 = mtxs[9];
        halberdDisableSubject(&m_subject);
        m_timer = data->unkAC;
        m_state = 8;
    } else {
        stHalberdMtx* mtxs = reinterpret_cast<stHalberdMtx*>(m_mtxWork);
        m_pos.m_x = mtxs[9].m[0][3];
        m_pos.m_y = mtxs[9].m[1][3];
        m_pos.m_z = mtxs[9].m[2][3];
    }
}

void grHalberdHero2Dan::updateCallBack(float deltaFrame) {
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
            bool zero = false;
            data->m_pos.m_x = m_pos.m_x;
            data->m_pos.m_y = m_pos.m_y;
            data->m_pos.m_z = m_pos.m_z;
            Vec3f camera(m_subject.m_pos.m_x, m_subject.m_pos.m_y, m_subject.m_pos.m_z);
            Vec3f diff(m_pos.m_x - camera.m_x, m_pos.m_y - camera.m_y, m_pos.m_z - camera.m_z);
            if (fabsf(diff.m_x) < 1e-5f && fabsf(diff.m_y) < 1e-5f && fabsf(diff.m_z) < 1e-5f) {
                zero = true;
            }
            if (!zero) {
                float length = grHalberdLength(diff.m_z, diff.m_x, diff.m_y) * 0.25f;
                diff.normalize();
                diff.m_x *= length;
                diff.m_y *= length;
                diff.m_z *= length;
                camera.m_x += diff.m_x;
                camera.m_y += diff.m_y;
                camera.m_z += diff.m_z;
                m_subject.setPos(&camera);
                m_subject.m_stateB = 0;
            }
        }
    }
}

// The animation of the shell (2 animations; only the first one is played).
void grHalberdHero2Dan::setMotion(u32 animId, bool shouldLoop, bool force, float* frameCount) {
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

void grHalberdHero2Dan::setHit() {
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

// The attack 0 is the shell flying, the attack 1 the explosion.
void grHalberdHero2Dan::setAttack(int index) {
    soCollisionAttackData attack(1.0f);
    Vec3f offset;
    offset.m_x = 0.0f;
    offset.m_y = 0.0f;
    offset.m_z = 0.0f;
    if (index == 1) {
        setAttackGimmickDetails(&attack, 30.0f, 1.0f, 1.0f, 1.0f,
            20, &offset, 361, 50, 0, 78, m_nodeIndex,
            0x3FF, 7, false, 15,
            soCollisionAttackData::Attribute_Fire, soCollisionAttackData::Sound_Level_Large,
            soCollisionAttackData::Sound_Attribute_Fire,
            false, false, false, true, false, false, 0, 60,
            false, false, false, soCollisionAttackData::Lr_Check_Pos,
            false, false, false, false, false, soCollisionAttackData::Region_None, false);
    } else if (index < 1 && index >= 0) {
        setAttackGimmickDetails(&attack, 15.0f, 1.0f, 1.0f, 1.0f,
            10, &offset, 45, 60, 10, 78, m_nodeIndex,
            0x3FF, 7, false, 15,
            soCollisionAttackData::Attribute_Normal, soCollisionAttackData::Sound_Level_Large,
            soCollisionAttackData::Sound_Attribute_None,
            false, false, false, true, false, false, 0, 15,
            false, false, false, soCollisionAttackData::Lr_Check_Pos,
            false, false, false, false, false, soCollisionAttackData::Region_None, false);
    }
    m_yakumono->setAttack(index, 0, &attack);
}

// The shell explodes: the hit sets the fire off, the camera shakes and the danger zone of the AI is removed.
void grHalberdHero2Dan::bomb() {
    *m_stateWork = 0x11;
    disableAttack(0);
    setAttack(1);
    m_timer = 10.0f;
    if (m_seId != -1) {
        m_snd.stopSE(m_seId, 0);
    }
    m_seId = -1;
    g_ecMgr->setEffect(static_cast<EfID>(0x3F0003), &m_pos);
    m_snd.playSE(static_cast<SndID>(0x51), 0, 0, -1);
    Vec3f shake(0.0f, 0.0f, 0.0f);
    cmReqQuake(cmQuake::Amplitude_M, &shake);
    m_subject.m_state = 1;
    if (m_dangerZone != -1) {
        g_aiMgr->delDangerZone(m_dangerZone);
        m_dangerZone = -1;
    }
    setVisibility(0);
}

void grHalberdHero2Dan::onInflict(soCollisionLog* collisionLog, u32 unk2, float power) {
    if (m_state == 8) {
        bomb();
        m_state = 9;
    }
}
