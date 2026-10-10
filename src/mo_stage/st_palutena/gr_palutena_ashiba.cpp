#define YK_STAGE_INLINE // inline members of the shared SO headers that the stage RELs emit themselves
#define YK_STAGE_FULL // ... and the ones of the hit objects with an attack module and an event observer
#define YK_DATA_INLINE_CTOR // the ykData / ykDataGroup constructors are inline in this REL
#define SO_COLLISION_GROUP_ALIGNED // the hit object embeds soArrayVector<soCollisionGroup, 1> with 4-byte aligned elements
#define SO_COLLISION_GROUP_MEMBERWISE // ... and copies the group member by member

#include <ec/ec_mgr.h>
#include <gm/gm_global.h>
#include <gr/gr_calc_world_callback.h>
#include <math.h>
#include <memory.h>
#include <mt/mt_prng.h>
#include <mt/mt_trig.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <nw4r/math/math_arithmetic.h>
#include <nw4r/math/math_triangular.h>
#include <snd/snd_system.h>
#include <string.h>
#include <yk/yk_no_hit_normal.h>
#include <yk/yk_normal.h>

#include <st_palutena/gr_palutena.h>
#include <st_palutena/gr_palutena_anim.h>

// HYPOTHESIS: the same fsel based clamp helper the glide statuses use.
static inline float palutenaClamp(float value, float lo, float hi) {
    value = nw4r::math::FSelect(value - lo, value, lo);
    return nw4r::math::FSelect(value - hi, hi, value);
}

grPalutenaAshiba* grPalutenaAshiba::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grPalutenaAshiba* ground = new (Heaps::StageInstance) grPalutenaAshiba(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grPalutenaAshiba::grPalutenaAshiba(const char* taskName) : grPalutena(taskName) {
    unk158 = 1;
    m_hpWork = NULL;
    m_hpPrev = 0.0f;
    unk164 = 0;
    unk168 = 0.0f;
    m_lastLevel = 0;
    m_shakeTimer = 0.0f;
    m_shakePos.m_x = 0.0f;
    m_shakePos.m_y = 0.0f;
    m_shakePos.m_z = 0.0f;
    m_offset.m_x = 0.0f;
    m_offset.m_y = 0.0f;
    m_offset.m_z = 0.0f;
    m_scale.m_x = 1.0f;
    m_scale.m_y = 1.0f;
    m_scale.m_z = 1.0f;
    m_type = 10;
    m_level = 0;
    m_effectTimer = 0.0f;
    m_effectDone = 0;
    m_modelAnim = NULL;
    m_animState = 0;
    m_motion = 1;
    unk1AC = 0.0f;
    m_motionSet = 0;
    m_hasYakumono = 0;
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
    callback->m_nodeCallbackDatas[0].m_flags |= 0x10;
    callback->m_nodeCallbackDatas[0].m_flags |= 4;
    m_event = 0;
    if (g_GameGlobal->m_modeMelee == NULL) {
        return;
    }
    if (palutenaMeleeBytes()->m_mode != 7) {
        return;
    }
    if (palutenaMeleeBytes()->m_event == 3) {
        m_event = 1;
    }
}

grPalutenaAshiba::~grPalutenaAshiba() {
    grPalutenaFreeModelAnim(m_modelAnim);
    m_modelAnim = NULL;
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

void grPalutenaAshiba::processAnim() {
    Ground::processAnim();
    switch (m_animState) {
    case 0:
        setMotionCommon(0, true, true, NULL);
        m_animState = 1;
        // fall through
    case 1:
        m_animState = 4;
        break;
    case 4:
        break;
    }
}

void grPalutenaAshiba::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateYakumono(deltaFrame);
        updatePreBuild(deltaFrame);
        updateToBuild(deltaFrame);
        updateShake(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

// Shows the model of the break level the platform is in (the hit object is made first), shakes the platform and makes
// the dust when the platform loses life.
void grPalutenaAshiba::updateYakumono(float deltaFrame) {
    if (m_hasYakumono == 1) {
        switch (getBreakLevel()) {
        default:
            setVisibility(0);
            if (m_level == 0) {
                disableHit(0, 0);
            }
            break;
        case 3:
            if (m_level != 2) {
                setVisibility(0);
            } else {
                setVisibility(1);
            }
            break;
        case 1:
        case 2:
            if (m_level != 1) {
                setVisibility(0);
            } else {
                setVisibility(1);
            }
            break;
        case 0:
            if (m_level != 0) {
                setVisibility(0);
            } else {
                setVisibility(1);
            }
            if (m_level == 0) {
                enableHit(0, 0);
            }
            break;
        }
        m_effectTimer = m_effectTimer - deltaFrame;
        if (m_effectTimer < 0.0f) {
            m_effectTimer = 0.0f;
        }
        if (*m_hpWork < m_hpPrev && m_isVisible == 1) {
            m_shakeTimer = randf() * 5.0f + 5.0f;
            m_snd.playSE(static_cast<SndID>(0x1CDE), 0, 0, -1);
            u8 currentLevel = getBreakLevel();
            if (m_lastLevel != currentLevel && currentLevel != 0) {
                m_snd.playSE(static_cast<SndID>(0x1CDD), 0, 0, -1);
            }
            m_lastLevel = currentLevel;
            if (m_effectTimer == 0.0f) {
                Vec3f nodePos;
                getNodePosition(&nodePos, m_unk1, m_nodeIndex);
                Vec3f effectPos(0.0f, 0.0f, 0.0f);
                u32 effect = g_ecMgr->setEffect(static_cast<EfID>(0x4F0001), &effectPos);
                u32 kind;
                switch (m_type) {
                case 4:
                    kind = 2;
                    break;
                default:
                    kind = 1;
                    break;
                }
                g_ecMgr->setParent(effect, m_sceneModels[m_unk1], kind, 0);
                m_effectTimer = randf() * 5.0f + 10.0f;
            }
        }
        m_hpPrev = *m_hpWork;
    } else if (m_level != 0) {
        m_hasYakumono = 1;
    } else {
        setHit();
        if (m_yakumono != NULL) {
            m_hasYakumono = 1;
        }
    }
}

// While the platform is being built (the last frames of its waiting time) it grows from the floor and the dust is made.
void grPalutenaAshiba::updatePreBuild(float deltaFrame) {
    stPalutenaData* data = static_cast<stPalutenaData*>(getStageData());
    if (data != NULL && !(unk154 < data->unk28 - (data->unk2C + 4.0f))) {
        if (m_effectDone == 0) {
            u32 effect = g_ecMgr->setEffect(static_cast<EfID>(0x4F0004));
            u32 kind;
            switch (m_type) {
            case 4:
                kind = 2;
                break;
            default:
                kind = 1;
                break;
            }
            g_ecMgr->setParent(effect, m_sceneModels[0], kind, 0);
            m_effectDone = 1;
        }
        float start = data->unk28 - data->unk2C;
        if (!(unk154 < start)) {
            float rate = (unk154 - start) / data->unk2C;
            m_offset.m_x = 0.0f;
            m_offset.m_z = 0.0f;
            m_scale.m_x = 1.0f;
            m_scale.m_z = 1.0f;
            float grown = palutenaClamp(rate, 0.001f, 1.0f);
            m_scale.m_y = grown;
            m_offset.m_y = grown * 6.0f + -6.0f;
            *m_hpWork = data->m_hp;
            if (m_level == 0) {
                if (m_isVisible != 1) {
                    setVisibility(1);
                }
                if (!m_isEnableCollisionStatus) {
                    setEnableCollisionStatus(true);
                }
            }
        }
    }
}

// Waits until the platform is built again (m_state 2) and starts the building (0 -> 1); the platform breaks at level 4.
void grPalutenaAshiba::updateToBuild(float deltaFrame) {
    stPalutenaData* data = static_cast<stPalutenaData*>(getStageData());
    if (data == NULL) {
        return;
    }
    switch (m_state) {
    case 0:
        *m_hpWork = data->m_hp;
        unk154 = 0.0f;
        m_state = 1;
        // fall through
    case 1:
        if (getBreakLevel() == 4) {
            m_snd.playSE(static_cast<SndID>(0x1CDF), 0, 0, -1);
            Vec3f pos;
            getNodePosition(&pos, 0, 1);
            pos.m_y = pos.m_y - 2.5f;
            fn_8005F800(g_ecMgr, static_cast<EfID>(0x4F0002), &pos);
            m_state = 2;
        }
        break;
    case 2:
        if (m_event == 1) {
            return;
        }
        unk154 = unk154 + deltaFrame;
        if (data->unk28 < unk154) {
            unk154 = data->unk28;
        }
        if (unk154 < data->unk28) {
            return;
        }
        m_state = 0;
        break;
    }
}

// The platform shakes sideways for a few frames after it was hit.
void grPalutenaAshiba::updateShake(float deltaFrame) {
    m_shakeTimer = m_shakeTimer - deltaFrame;
    if (m_shakeTimer <= 0.0f) {
        m_shakeTimer = 0.0f;
    }
    if (m_shakeTimer > 0.0f) {
        if ((float)((u32)m_shakeTimer % 3) == 0.0f) {
            float sine;
            float cosine;
            float angle = 90.0f;
            mtSinCosf(0.017453292f * angle, &sine, &cosine);
            float amount = 0.3f + 0.4f * randf();
            m_shakePos.m_x = 0.75f * (amount * cosine);
            m_shakePos.m_y = amount * sine;
            m_shakePos.m_z = 0.0f;
        }
    } else {
        m_shakePos.m_x = 0.0f;
        m_shakePos.m_y = 0.0f;
        m_shakePos.m_z = 0.0f;
    }
}

void grPalutenaAshiba::updateCallBack(float deltaFrame) {
    Vec3f pos;
    grCalcWorldCallBack* calcWorldCallBack = &m_calcWorldCallBack;
    if (calcWorldCallBack != NULL) {
        nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[0];
        if (scnMdl != NULL) {
            if (scnMdl->m_calcWorldCallBack == NULL) {
                calcWorldCallBack->m_index = 0;
                scnMdl->m_calcWorldCallBack = calcWorldCallBack;
                scnMdl->EnableScnMdlCallbackTiming(1);
                scnMdl->m_nodeIndex = m_nodeIndex;
            }
            grNodeCallbackData* data = &calcWorldCallBack->m_nodeCallbackDatas[0];
            Vec3f offset = m_offset + m_shakePos;
            data->m_offsetPos.m_x = offset.m_x;
            data->m_offsetPos.m_y = offset.m_y;
            data->m_offsetPos.m_z = offset.m_z;
            data = &calcWorldCallBack->m_nodeCallbackDatas[0];
            data->m_scale.m_x = m_scale.m_x;
            data->m_scale.m_y = m_scale.m_y;
            data->m_scale.m_z = m_scale.m_z;
            getNodePosition(&pos, 0, m_nodeIndex);
            m_snd.setPos(&pos);
        }
    }
}

// Builds the hit object of the platform: one hit capsule along its top (the size depends on the platform), made once.
void grPalutenaAshiba::setHit() {
    u32 nodeIndex = 1;
    if (m_type == 4) {
        nodeIndex = 4;
    }
    Vec3f extent;
    u32 rootNode = 0;
    getNodePosition(&extent, 0, rootNode);
    switch (m_type) {
    case 0:
        extent.m_x = 10.0f;
        break;
    case 1:
        extent.m_x = 10.0f;
        break;
    case 2:
        extent.m_x = 15.0f;
        break;
    case 3:
        extent.m_x = 15.0f;
        break;
    case 4:
        extent.m_x = 12.0f;
        break;
    case 5:
        extent.m_x = 14.0f;
        break;
    case 6:
        extent.m_x = 14.0f;
        break;
    case 7:
        extent.m_x = 14.0f;
        break;
    case 8:
        extent.m_x = 15.0f;
        break;
    }
    switch (m_type) {
    case 0:
        extent.m_y = 2.5f;
        break;
    case 1:
        extent.m_y = 2.5f;
        break;
    case 2:
        extent.m_y = 2.3f;
        break;
    case 3:
        extent.m_y = 2.3f;
        break;
    case 4:
        extent.m_y = 2.2f;
        break;
    case 5:
        extent.m_y = 2.2f;
        break;
    case 6:
        extent.m_y = 2.2f;
        break;
    case 7:
        extent.m_y = 2.2f;
        break;
    case 8:
        extent.m_y = 2.5f;
        break;
    }
    // MATCH-ONLY: the original allocates the hit data as raw storage (no element construction)
    m_hitData = reinterpret_cast<soCollisionHitData*>(new (Heaps::StageInstance) u8[sizeof(soCollisionHitData)]);
    m_hitSimple = reinterpret_cast<soCollisionHitData::Simple*>(new (Heaps::StageInstance) u8[sizeof(soCollisionHitData::Simple)]);
    m_hitSet = reinterpret_cast<soSet<soCollisionHitData::Simple>*>(new (Heaps::StageInstance) u8[sizeof(grPalutenaSetView)]);
    m_dataGroup = new (Heaps::StageInstance) ykDataGroup;
    m_data = new (Heaps::StageInstance) ykData;

    m_hitData->m_startOffsetPos.m_x = -extent.m_x;
    m_hitData->m_startOffsetPos.m_y = -extent.m_y;
    m_hitData->m_startOffsetPos.m_z = -extent.m_z;
    m_hitData->m_endOffsetPos.m_x = extent.m_x;
    m_hitData->m_endOffsetPos.m_y = -extent.m_y;
    m_hitData->m_endOffsetPos.m_z = -extent.m_z;
    m_hitData->m_size = 2.5f;
    // MATCH-ONLY: the shape type is the top bit of the byte at 0x1c (it is the whole byte of the simple form)
    reinterpret_cast<grPalutenaHitByte*>(m_hitData)->m_shape = 1;
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
    m_hitSimple->m_nodeIndex = nodeIndex;
    // MATCH-ONLY: the members of soSet are private
    grPalutenaSetView* set = reinterpret_cast<grPalutenaSetView*>(m_hitSet);
    set->m_elements = m_hitSimple;
    set->m_size = 1;
    m_dataGroup->m_hitDataSimpleSet = m_hitSet;
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
    yakumono->setCollisionHitSelfCatagory(9);
    setYakumono(yakumono);
}

// How broken the platform is, from its life: 0 = whole, 1 / 2 = damaged, 3 = nearly broken, 4 = broken.
u8 grPalutenaAshiba::getBreakLevel() {
    stPalutenaData* data = static_cast<stPalutenaData*>(getStageData());
    if (data == NULL) {
        return 0;
    }
    float hp = *m_hpWork;
    if (hp <= 0.0f) {
        return 4;
    }
    if (hp <= data->m_hp * data->unk18) {
        return 3;
    }
    if (hp <= data->m_hp * data->unk14) {
        return 2;
    }
    return hp <= data->m_hp * data->unk10;
}

void grPalutenaAshiba::requestBuild() {
    if (m_state == 1) {
        if (!m_isEnableCollisionStatus) {
            setEnableCollisionStatus(true);
        }
        setVisibility(1);
        enableHit(0, 0);
        m_scale.m_x = 1.0f;
        m_scale.m_y = 1.0f;
        m_scale.m_z = 1.0f;
        m_offset.m_x = 0.0f;
        m_offset.m_y = 0.0f;
        m_offset.m_z = 0.0f;
    }
}

void grPalutenaAshiba::requestBreak() {
    if (m_isEnableCollisionStatus == 1) {
        setEnableCollisionStatus(false);
    }
    setVisibility(0);
    disableHit(0, 0);
    m_effectDone = 0;
    m_scale.m_x = 1.0f;
    m_scale.m_y = 0.001f;
    m_scale.m_z = 1.0f;
}

// Makes the animation of the platform from the file of the stage (one for all platforms).
void grPalutenaAshiba::setResCommon(nw4r::g3d::ResFile* resFile) {
    nw4r::g3d::ScnMdl* sceneMdl = *m_sceneModels;
    if (sceneMdl != NULL) {
        m_modelAnim = grPalutenaNewModelAnim(resFile);
        if (m_modelAnim != NULL) {
            m_modelAnim->_spacer[0] = 0;
            gfModelAnimation::bind(sceneMdl, m_modelAnim);
        }
    }
}

// The animation of the platform (only the first one is bound).
void grPalutenaAshiba::setMotionCommon(u32 animId, bool loop, bool force, float* frameCount) {
    if (m_motion == animId && force == 0) {
        return;
    }

    nw4r::g3d::ScnMdl* sceneMdl = *m_sceneModels;
    if (sceneMdl == NULL) {
        return;
    }

    gfModelAnimation* modelAnim = m_modelAnim;
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

    bool result = (modelAnim->m_resFile.GetResAnmVisNumEntries() > animId);
    if (result) {
        grPalutenaSetVisibilityAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmChrNumEntries() > animId);
    if (result) {
        grPalutenaSetChrAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmTexPatNumEntries() > animId);
    if (result) {
        grPalutenaSetTexPatAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmTexSrtNumEntries() > animId);
    if (result) {
        grPalutenaSetTexSrtAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmClrNumEntries() > animId);
    if (result) {
        grPalutenaSetColorAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    gfModelAnimation::bind(sceneMdl, modelAnim);
    modelAnim->setFrame(0.0);
    modelAnim->setUpdateRate(1.0);
    modelAnim->setLoop(loop);

    if (frameCount != NULL) {
        *frameCount = modelAnim->getFrameCount();
    }
    m_motionSet = 1;
}

// A fighter hit the platform: it loses life.
void grPalutenaAshiba::onDamage(int index, soDamage* damage, soDamageAttackerInfo* attackerInfo) {
    float amount = damage->unk4;
    fn_27_26399C(m_yakumono);
    *m_hpWork = *m_hpWork - amount;
    if (*m_hpWork < 0.0f) {
        *m_hpWork = 0.0f;
    }
}

void grPalutenaAshiba::getPos_Shake(Vec3f* out) {
    out->m_x = m_shakePos.m_x;
    out->m_y = m_shakePos.m_y;
    out->m_z = m_shakePos.m_z;
}
