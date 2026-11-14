#pragma once

#include <so/status/so_status_module_impl.h>

class soModuleAccesser;

// The beast's two status processes live in their own file (wn_ganon_beast_status_uniq_process.cpp).
class wnGanonBeastStatusUniqProcessStart : public soStatusUniqProcess {
public:
    wnGanonBeastStatusUniqProcessStart() { }
    virtual ~wnGanonBeastStatusUniqProcessStart() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

class wnGanonBeastStatusUniqProcessAttack : public soStatusUniqProcess {
public:
    wnGanonBeastStatusUniqProcessAttack() { }
    virtual ~wnGanonBeastStatusUniqProcessAttack() { }
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

extern wnGanonBeastStatusUniqProcessStart g_wnGanonBeastStatusUniqProcessStart;
extern wnGanonBeastStatusUniqProcessAttack g_wnGanonBeastStatusUniqProcessAttack;

// ---- the beast article (article kind 0x1F): Ganondorf's Beast Ganon final smash weapon ----------------------------------
// HYPOTHESIS: all names and layouts below are read off the original object's builder names and constructor.

#include <so/damage/so_damage_event_presenter.h>
#include <wn/wn_extend_param_accesser.h>
#include <wn/wn_weapon_builder.h>

struct wnArticleBuildInfo;
struct soVisibilityData;
class soCollisionAttackData;
class soCollisionHitData;
#include <so/so_array.h>
class soDamage;

struct wnGanonBeastParam {
    float unk0;
    float unk4;
    float unk8;
};

struct wnGanonBeastData {
    s32 m_resGroupNo;
    u8 unk4[0x20];
    soVisibilityData* m_visibilityData; // +0x24
    u8 unk28[0xC];
    const wnGanonBeastParam* m_param;   // +0x34
    soSet<soCollisionAttackData> m_attackDataSet; // +0x38: the builder receives this set's address (HYPOTHESIS: element type)
    soSet<soCollisionHitData> m_hitDataSet;       // +0x40
};

class wnGanonBeastParamAccesser : public wnExtendParamAccesser<3999, 3, 23999, 0, 0, 0> {
public:
    explicit wnGanonBeastParamAccesser(const wnGanonBeastParam* param) : wnExtendParamAccesser<3999, 3, 23999, 0, 0, 0>(0x80) {
        m_floats[0] = &param->unk0;
        m_floats[1] = &param->unk4;
        m_floats[2] = &param->unk8;
    }
    virtual ~wnGanonBeastParamAccesser() { }
};

class wnGanonBeastModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0, wnInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<2, soMotionModuleImpl, 0, 1, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soGroundModuleBuildConfigNull GroundModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 4, 0, soCollisionAttackModuleImpl, 2, false, true>
        CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfig<soCollision::Category_Fighter, 20, 1, soCollisionHitModuleImpl, 0x3FF, true>
        CollisionHitModuleBuildConfig;
    typedef wnDamageModuleBuildConfig<soDamageModuleImpl, 1, soSingletonHolder<soDamageTransactorNull>, soSingletonHolder<soDamageEffectorNull> >
        DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef soVisibilityModuleBuildConfig<soVisibilityModuleImpl, 1> VisibilityModuleBuildConfig;
    typedef soCameraModuleBuildConfig<soCameraModuleImpl> CameraModuleBuildConfig;
    typedef soShadowModuleBuildConfig<soShadowModuleImpl> ShadowModuleBuildConfig;
    typedef soGlowModuleBuildConfigNull GlowModuleBuildConfig;
    typedef soColorBlendModuleBuildConfig<5, 0, soColorBlendModuleImpl> ColorBlendModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<2, soGeneralWorkBuildConfig<2, 2, 1>, 2, soTransitionModuleBuildConfig<wnStatusTransitionTypeList> >
        StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 2, 0, 0, 1, 2, 0, 0, 1, 2, 0, 0, 1, 2, 0, 0> AnimCmdModuleSubBuildConfig;
};

class wnGanonBeast : public wnWeaponBuilder<wnGanonBeastModuleAccesserBuildConfig>, public soDamageEventObserver {
    const wnGanonBeastParam* m_param;
    wnGanonBeastParamAccesser m_paramAccesser;
public:
    wnGanonBeast(s32 articleId, const wnArticleBuildInfo& info, const wnGanonBeastData* data);
    virtual ~wnGanonBeast() { }
    void activate(s32 founderTaskId, s32 unk1, s32 team, const Vec3f* pos, float lr);
    virtual bool notifyEventCollisionAttackCheck(u32 flags);
    virtual void notifyEventOnDamage(soDamage* damage, bool, soModuleAccesser* moduleAccesser);
};
