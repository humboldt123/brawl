#define WN_MARIO_FIREBALL_UNIT
#define SO_COLLISION_GROUP_ALIGNED
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_STATUS_NO_TAIL
#define WN_TRANSITION_CTOR_INLINE
#define WN_KINETIC_MEDIATOR_INLINE_DTOR
#define FT_POOL_ROOT_NONPOLYMORPHIC // BYTES: without it the hierarchy destructor is never_inline
#define WN_PARAM_CUSTOMIZE_INLINE_DTOR // BYTES: the builder destructors expand the empty param customize module destructor
#define WN_PARAM_ACCESSER_INLINE_DTOR // BYTES: the empty soParamAccesser base destructor is inlined in sora_melee
#define MT_VEC3F_IMPLICIT_ASSIGN // BYTES: activate copies the Vec3f position as an aggregate (word moves), see mt_vector.h
#define FT_REL_LINK_EXTERN
#define SO_NULLABLE_INLINE_CTOR
#define WN_ANIMCMD_RESET_MODE_BOX // BYTES: word-aligned reset-mode temporaries (frame size of the anim cmd sub builder)
#define WN_ANIMCMD_PRESENTER_LOCAL_STORE // BYTES: the interpreters' presenters keep the looked-up list in a local (98.3% vs 97.2%)
#include <wn/wn_module_accesser_builder.h>
#include <wn/mario/wn_mario_fireball.h>
#include <wn/wn_class_info_impl.h>
#include <so/so_external_value_accesser.h>
#include <so/so_module_accesser.h>
#include <so/model/so_model_module_impl.h>
#include <gf/gf_task_scheduler.h>
#include <wn/wn_activate_desc.h>
#include <wn/wn_kinetic_energy_normal.h>
#include <ac/ac_anim_cmd_impl.h>
#include <so/status/so_status_module_impl.h>
#include <math.h>

// The base of the mediators: an empty destructor that sora_melee always expands (it has no out-of-line copy there).
inline soArticleMediator::~soArticleMediator() { }
// BYTES: the same for the empty team, sound generator accesser and resource id accesser destructors: the builder
// destructors expand them to nothing instead of calling the weak copies.
inline soTeam::~soTeam() { }
inline soSound3dGeneratorAccesserImpl::~soSound3dGeneratorAccesserImpl() { }
inline soResourceIdAccesser::~soResourceIdAccesser() { }

wnMarioFireball::wnMarioFireball(s32 articleId, const wnArticleBuildInfo& info, const wnMarioFireballData* data) :
    wnWeaponBuilder<wnMarioFireballModuleAccesserBuildConfig>(
        "MarioFireball", articleId, info,
        wnModuleAccesserBuildData(0, data, info.getHeapModule()->getInstanceHeap(),
            info.getHeapModule()->getNWModelInstanceHeap(),
            info.getHeapModule()->getNWMotionInstanceHeap(),
            static_cast<u8>(data->m_resGroupNo), -1, &g_soParamAccesserNull, 0, 0, 0, 0, 0, 0, 0)),
    m_param(data->m_param) {
    Weapon::deactivate(false);
}

// Registers the Mario fireball article (kind 0) in the class info table during static initialization.
wnClassInfoImpl<0, wnMarioFireball, wnMarioFireballData> g_wnMarioFireballClassInfo;

template soInsideEventManageModuleBuilder<
    wnMarioFireballModuleAccesserBuildConfig::InsideEventManageModuleBuildConfig,
    wnMarioFireballModuleAccesserBuildConfig::InsideEventManageModuleTypes>::soInsideEventManageModuleBuilder();
// BYTES: the original addresses each singleton's destructor-chain record by its own address; with data pooling the
// three records share one hoisted .bss anchor register. The pragma applies where the template is instantiated.
#pragma push
#pragma pool_data off
template soMotionModuleBuilder<
    wnMarioFireballModuleAccesserBuildConfig::MotionModuleBuildConfig>::soMotionModuleBuilder(soModuleAccesser*, void*);
#pragma pop

template soPostureModuleBuilder<
    wnMarioFireballModuleAccesserBuildConfig::PostureModuleBuildConfig>::soPostureModuleBuilder(soModuleAccesser*, soEventObserverRegistrationDesc*);
template soCollisionAttackModuleBuilder<
    wnMarioFireballModuleAccesserBuildConfig::CollisionAttackModuleBuildConfig>::soCollisionAttackModuleBuilder(soModuleAccesser*, int, u8, soEventObserverRegistrationDesc*);
template soSoundModuleBuilder<
    wnMarioFireballModuleAccesserBuildConfig::SoundModuleBuildConfig>::soSoundModuleBuilder(soModuleAccesser*, soSoundIdExchanger*, soEventObserverRegistrationDesc*);
template soStatusModuleBuilder<
    wnMarioFireballModuleAccesserBuildConfig::StatusModuleBuildConfig>::soStatusModuleBuilder(soModuleAccesser*, const soStatusData*, void*);
template soKineticModuleBuilder<
    wnMarioFireballModuleAccesserBuildConfig::KineticModuleBuildConfig>::soKineticModuleBuilder(soModuleAccesser*);
template soEffectModuleBuilder<
    wnMarioFireballModuleAccesserBuildConfig::EffectModuleBuildConfig>::soEffectModuleBuilder(soModuleAccesser*, void*, void*, void*, void*, soEventObserverRegistrationDesc*);

// BYTES: the module accesser builder constructor in sora_melee calls every module getter and member builder out of line.
#pragma dont_inline on
soModuleAccesserBuilder<wnMarioFireballModuleAccesserBuildConfig>::soModuleAccesserBuilder(
    StageObject* owner, const wnModuleAccesserBuildData& bd) :
    unk194(),
    m_moduleAccsr(
        owner,
        m_resourceModuleBuilder.getModule(),
        m_modelModuleBuilder.getModule(),
        m_motionBuilder.getModule(),
        m_postureModuleBuilder.getModule(),
        m_groundModuleBuilder.getModule(),
        m_situationModuleBuilder.getModule(),
        m_teamBuilder.getModule(),
        m_attackModuleBuilder.getModule(),
        m_hitModuleBuilder.getModule(),
        m_shieldModuleBuilder.getModule(),
        m_reflectorModuleBuilder.getModule(),
        m_absorberBuilder.getModule(),
        m_collisionCatchModuleBuilder.getModule(),
        m_searchBuilder.getModule(),
        m_damageModuleBuilder.getModule(),
        m_catchModuleBuilder.getModule(),
        m_captureModuleBuilder.getModule(),
        m_stopModuleBuilder.getModule(),
        m_turnModuleBuilder.getModule(),
        m_shakeModuleBuilder.getModule(),
        m_soundModuleBuilder.getModule(),
        m_linkModuleBuilder.getModule(),
        m_visibilityModuleBuilder.getModule(),
        m_controllerModuleBuilder.getModule(),
        m_cameraModuleBuilder.getModule(),
        m_workManageModuleBuilder.getModule(),
        m_debugModuleBuilder.getModule(),
        m_animCmdBuilder.getModule(),
        m_statusBuilder.getModule(),
        m_generalTermDecideModuleBuilder.getModule(),
        m_switchDecideModuleBuilder.getModule(),
        m_kineticBuilder.getModule(),
        unk194.getModule(),
        m_generateArticleBuilder.getModule(),
        m_effectModuleBuilder.getModule(),
        m_comboModuleBuilder.getModule(),
        (soAreaModule*)m_areaBuilder.getModule(),
        m_territoryModuleBuilder.getModule(),
        m_targetSearchModuleBuilder.getModule(),
        m_physicsModuleBuilder.getModule(),
        m_slopeModuleBuilder.getModule(),
        m_shadowModuleBuilder.getModule(),
        m_itemManageModuleBuilder.getModule(),
        m_colorBlendModuleBuilder.getModule(),
        m_jostleModuleBuilder.getModule(),
        m_abnormalModuleBuilder.getModule(),
        m_slowModuleBuilder.getModule(),
        m_reflectModuleBuilder.getModule(),
        m_heapModuleBuilder.getModule(),
        m_paramCustomizeModuleBuilder.getModule(),
        m_glowModuleBuilder.getModule()),
    m_heapModuleBuilder(bd.getInstanceHeap(), bd.getNWModelInstanceHeap(), bd.getNWMotionInstanceHeap(), bd.getHeapSlotNo()),
    m_paramCustomizeModuleBuilder(&m_moduleAccsr),
    m_resourceModuleBuilder(bd.getMdlResId(), bd.getAnmResId(), bd.getResGroupNo(), &m_moduleAccsr),
    m_modelModuleBuilder(&m_moduleAccsr, bd.getModelScale(), bd.getModelExtendNodeTable(), soEventObserverRegistrationDescNull::getNullInstance()),
    m_motionBuilder(&m_moduleAccsr, bd.getMotionData()),
    m_postureModuleBuilder(&m_moduleAccsr, soEventObserverRegistrationDescNull::getNullInstance()),
    m_groundModuleBuilder(&m_moduleAccsr, bd.getGroundConditionChecker()),
    m_situationModuleBuilder(&m_moduleAccsr, static_cast<soEventManager&>(m_moduleAccsr.getEventManageModule()).getManageId(), soEventObserverRegistrationDescNull::getNullInstance()),
    m_teamBuilder(bd.getTeam(), &m_moduleAccsr),
    m_attackModuleBuilder(&m_moduleAccsr, owner->getId(), static_cast<u8>(owner->getCategory()), soEventObserverRegistrationDescNull::getNullInstance()),
    m_hitModuleBuilder(&m_moduleAccsr, owner->getId(), static_cast<gfTask::Category>(static_cast<u8>(owner->getCategory())), soEventObserverRegistrationDescNull::getNullInstance()),
    m_shieldModuleBuilder(&m_moduleAccsr, owner->getId(), static_cast<gfTask::Category>(static_cast<u8>(owner->getCategory()))),
    m_reflectorModuleBuilder(&m_moduleAccsr, owner->getId(), static_cast<gfTask::Category>(static_cast<u8>(owner->getCategory()))),
    m_absorberBuilder(&m_moduleAccsr, owner->getId(), static_cast<u8>(owner->getCategory())),
    m_collisionCatchModuleBuilder(&m_moduleAccsr, owner->getId(), static_cast<u8>(owner->getCategory())),
    m_searchBuilder(&m_moduleAccsr, owner->getId(), static_cast<u8>(owner->getCategory())),
    m_damageModuleBuilder(&m_moduleAccsr, soEventObserverRegistrationDescNull::getNullInstance()),
    m_catchModuleBuilder(&m_moduleAccsr),
    m_captureModuleBuilder(&m_moduleAccsr),
    m_stopModuleBuilder(&m_moduleAccsr),
    m_turnModuleBuilder(&m_moduleAccsr),
    m_shakeModuleBuilder(&m_moduleAccsr, bd.getShakeData()),
    m_soundModuleBuilder(&m_moduleAccsr, bd.getSoundIdExchanger(), soEventObserverRegistrationDescNull::getNullInstance()),
    m_linkModuleBuilder(&m_moduleAccsr),
    m_visibilityModuleBuilder(&m_moduleAccsr, bd.getVisibilityData()),
    m_controllerModuleBuilder(&m_moduleAccsr, static_cast<soEventManager&>(m_moduleAccsr.getEventManageModule()).getManageId()),
    m_cameraModuleBuilder(&m_moduleAccsr, (soSet<soCameraRange>*)bd.getCameraRangeSet(), (soSet<soCameraClipSphere>*)bd.getCameraClipSphereSet(), soEventObserverRegistrationDescNull::getNullInstance()),
    m_workManageModuleBuilder(&m_moduleAccsr, bd.getParamAccesser()),
    m_animCmdBuilder(static_cast<soEventManager&>(m_moduleAccsr.getEventManageModule()).getManageId()),
    m_statusBuilder(&m_moduleAccsr, bd.getStatusData(), bd.getPreCheckAnimCmdData()),
    m_generalTermDecideModuleBuilder(),
    m_switchDecideModuleBuilder(),
    m_kineticBuilder(&m_moduleAccsr),
    m_generalWorkBuilder(),
    m_generateArticleBuilder(&m_moduleAccsr),
    m_effectModuleBuilder(&m_moduleAccsr, bd.getEffectNodeData(), bd.getEffectEmitData(), bd.getEffectCommonData(), bd.getEffectScreenData(), soEventObserverRegistrationDescNull::getNullInstance()),
    m_comboModuleBuilder(&m_moduleAccsr),
    m_areaBuilder(&m_moduleAccsr, bd.getAreaCategory(), soEventObserverRegistrationDescNull::getNullInstance()),
    m_territoryModuleBuilder(&m_moduleAccsr, bd.getTerritoryParam(), bd.getTerritoryRect()),
    m_targetSearchModuleBuilder(&m_moduleAccsr, bd.getTargetSearchParam()),
    m_physicsModuleBuilder(&m_moduleAccsr, bd.getIkData()),
    m_slopeModuleBuilder(&m_moduleAccsr, bd.getSlopeAngleLimit()),
    m_shadowModuleBuilder(&m_moduleAccsr),
    m_itemManageModuleBuilder(&m_moduleAccsr, bd.getItemNodeData()),
    m_colorBlendModuleBuilder(&m_moduleAccsr),
    m_jostleModuleBuilder(&m_moduleAccsr, bd.getJostleData()),
    m_abnormalModuleBuilder(&m_moduleAccsr),
    m_slowModuleBuilder(&m_moduleAccsr),
    m_reflectModuleBuilder(&m_moduleAccsr),
    m_glowModuleBuilder(&m_moduleAccsr),
    m_debugModuleBuilder(&m_moduleAccsr, soEventObserverRegistrationDescNull::getNullInstance()) {
    if (m_generalWorkBuilder.getWork()->isNull() == false) {
        getModuleAccesser()->getWorkManageModule().setWork(1, m_generalWorkBuilder.getWork());
    }
}

// BYTES: compact destructor: the simple module builders expand, the big ones stay calls (TIPS, linking sora_melee articles).
inline soModuleAccesserBuilder<wnMarioFireballModuleAccesserBuildConfig>::~soModuleAccesserBuilder() { }
#pragma dont_inline reset

#pragma push
#pragma inline_max_size(10000)
#pragma inline_max_total_size(20000)
#pragma inline_depth(8)
wnMarioFireball::~wnMarioFireball() { }
#pragma pop

// TODO(nonmatching): activate (96.4%): only the register choice of the two descriptor flag bytes differs (the original loads the
//   first into r9 and the second into r0, as every exact sora_melee activate does; ours gives the first byte r0); every
//   bitfield assignment order and the member-wise descriptor constructor were tried (the constructor form scores 93%), and
//   a 9 minute permuter run found nothing.
// TODO(nonmatching): notifyEventAnimCmd is byte exact (99.98%): the only remaining difference is the relocation form of the
//   float pool base (ours is section-relative `.rodata+N`, the original a named float label); check it at link time.
// TODO(nonmatching): the animation command sub builder constructor (98.3%): the presenter's looked-up list gets its own
//   register initialised to null (`li`) where the original reuses the register that holds the reset-mode zero / the null it
//   stores into m_obsrvrList, and re-materialises zero afterwards. Ruled out (F4, under a unit-private macro, all 97.4-98.3%):
//   the result read from the member before or after the capacity check (a reload or a fresh `li`), a null local in the
//   constructor body passed to the helper, a literal null local in the helper, and an uninitialised result (right local
//   shape, but four extra callee-saved registers over the whole constructor). Each reset-mode byte temporary also needs a
//   word-aligned slot (WN_ANIMCMD_RESET_MODE_BOX). The unit cannot link before these two are exact.
// Starts the fireball: it leaves along the launch angle at the launch speed, facing lr, and falls under its own gravity.
void wnMarioFireball::activate(int founderTaskId, int resourceId, int team, Vec3f* __restrict pos, int statusKind, float lr) {
    // MATCH-ONLY: the original reads the life before it builds the descriptor.
    int life = m_param->life;
    wnActivateDesc desc;
    desc.founderTaskId = founderTaskId;
    desc.resourceId = resourceId;
    desc.unk8 = resourceId;
    desc.unkC = resourceId;
    desc.unk10 = -1;
    desc.unk14 = -1;
    desc.unk18 = 0;
    desc.unk1C = 0;
    desc.pos = *pos;
    desc.lr = lr;
    desc.team = team;
    desc.life = life;
    desc.unk40 = 0;
    desc.unk44 = 0x35F;
    desc.unk48 = 0;
    desc.flagsHigh = 1;
    desc.flagsMidHigh = 0;
    desc.flagsMidLow = 1;
    desc.flagsLow = 0;
    desc.unk4DHigh = 0;
    desc.unk38 = 3;
    desc.unk3C = 0xFF;
    Weapon::activate(&desc);
    m_moduleAccesser->getStatusModule().changeStatusForce(statusKind, m_moduleAccesser);
    wnKineticEnergyNormal& energy = dynamic_cast<wnKineticEnergyNormal&>(*m_moduleAccesser->getKineticModule().getEnergy(0));
    float angle = m_param->angle;
    float speed = m_param->speed;
    float vy = speed * (float)sin(angle);
    float vx = lr * (speed * (float)cos(angle));
    energy.m_speed = Vec2f(vx, vy);
    energy.m_speedTarget = Vec2f(3.4028235e38f, m_param->fallSpeedMax);
    energy.m_accel = Vec2f(0.0f, -m_param->gravity);
}

// HYPOTHESIS: the dot product is an inline helper in the original: evaluating it through a call is what puts the bounce
// scale load ahead of the vector arithmetic.
static inline float wnMarioFireballDot(const Vec2f& a, const Vec2f& b) {
    return a.m_x * b.m_x + a.m_y * b.m_y;
}

// Reflects the velocity about the surface normal in place.
// BYTES: the reflection is inlined two levels deep. One level puts its binding temporaries below the caller's own (TIPS,
// locals of an inlined callee) but in reverse order; the second level restores the order. At function scope they sit
// above the caller's scale and XY temporaries. HYPOTHESIS: the original has a reflect helper wrapping a vector helper.
static inline void wnMarioFireballReflectImpl(Vec2f& speed, const Vec2f& normal) {
    speed = speed - normal * 2.0f * wnMarioFireballDot(normal, speed);
}
static inline void wnMarioFireballReflect(Vec2f& speed, const Vec2f& normal) {
    wnMarioFireballReflectImpl(speed, normal);
}

// Anim cmd type 6 is the ground check: the fireball bounces off the surface it touches (the velocity reflected about the
// surface normal and scaled), and breaks up when the bounce would leave it too slow.
bool wnMarioFireball::notifyEventAnimCmd(acAnimCmd* cmd, soModuleAccesser* moduleAccesser, int index) {
    if (Weapon::notifyEventAnimCmd(cmd, moduleAccesser, index)) {
        return true;
    }
    s8 group = cmd->getGroup();
    if (!isObserv(group)) {
        return false;
    }
    if (cmd->getType() > -1) {
        if (cmd->getType() >= 11) {
            return false;
        }
        switch (cmd->getType()) {
        case 6: {
            wnKineticEnergyNormal& energy =
                dynamic_cast<wnKineticEnergyNormal&>(*moduleAccesser->getKineticModule().getEnergy(0));
            Vec2f speed = energy.getSpeed();
            Vec2f normal = moduleAccesser->getGroundModule().getTouchNormal(static_cast<grCollStatus::TouchMask>(0xff), 0);
            wnMarioFireballReflect(speed, normal);
            speed = speed * m_param->bounceScale;
            if (speed.length() < m_param->minBounceSpeed) {
                deactivate(false);
                return true;
            }
            float lr = -1.0f;
            // BYTES: the speed assignment goes through a constructed XY pair (the Robo Beam does the same).
            energy.m_speed = Vec2f(speed.m_x, speed.m_y);
            if (speed.m_x > 0.0f) {
                lr = 1.0f;
            }
            moduleAccesser->getPostureModule().setLr(lr);
            return true;
        }
        }
    }
    return false;
}
