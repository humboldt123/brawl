#pragma once

// Kirby's Nabe (cooking pot) article (article kind 0x91).

#include <wn/wn_weapon_builder.h>

WN_STATUS_PROC_VECTOR_ARG_CTOR(6);

// The static data block of the article (read by its wnModuleAccesserBuildData).
// HYPOTHESIS: layout of the untouched part.
struct wnKirbyNabeData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const void* m_param; // +0x34
};

class wnKirbyNabeModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0, wnInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<6, soMotionModuleImpl, 0, 0, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 2, 0, soCollisionAttackModuleImpl, 1, false, true>
        CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef soVisibilityModuleBuildConfig<soVisibilityModuleSimple, 1> VisibilityModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<6, soGeneralWorkBuildConfig<1, 2, 4>, 6, soTransitionModuleBuildConfig<wnStatusTransitionTypeList> >
        StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 6, 0, 0, 1, 6, 0, 0, 1, 6, 0, 0, 1, 6, 0, 0> AnimCmdModuleSubBuildConfig;
    typedef wnKineticModuleBuildConfigSized<wnKineticModuleImpl, soKineticMediatorNull, 12> KineticModuleBuildConfig;
    typedef soGlowModuleBuildConfigNull GlowModuleBuildConfig;
};

class wnKirbyNabe : public wnWeaponBuilder<wnKirbyNabeModuleAccesserBuildConfig> {
public:
    wnKirbyNabe(s32 articleId, const wnArticleBuildInfo& info, const wnKirbyNabeData* data);
    virtual ~wnKirbyNabe() { }
    // HYPOTHESIS: argument meanings.
    void activate(int founderTaskId, int resourceId, const Vec3f* position, float lr, float unk0, float unk1);
    virtual void notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* linkedObject, int unk4);
    virtual bool notifyEventCollisionAttackCheck(u32) { return false; }
};
