#pragma once

// Wario's fart: the gas article the Waft (down special) leaves behind (article kind 0x61).

#include <wn/wn_weapon_builder.h>
#include <types.h>

#ifndef WN_WEAPON_BUILDER
// Fighter units only call activate: the opaque article with the extent of the generic builder.
struct wnWarioGarlicModuleAccesserBuildConfig;
template <>
class wnWeaponBuilder<wnWarioGarlicModuleAccesserBuildConfig> : public Weapon {
    u8 unkD0[0x2160 - sizeof(Weapon)];
public:
    virtual ~wnWeaponBuilder();
    virtual void deactivateDescendantForce();
};
class wnWarioGarlic : public wnWeaponBuilder<wnWarioGarlicModuleAccesserBuildConfig> {
public:
    // HYPOTHESIS: the names of the arguments follow what activate stores them in.
    void activate(int founderTaskId, int resourceId, int team);
};
#else
#include <wn/wn_extend_param_accesser.h>

// The parameter block of the article. HYPOTHESIS: layout; only the first float is exposed through the parameter accesser.
struct wnWarioGarlicParam {
    float unk0;
};

// The static data block of the article (read by its wnModuleAccesserBuildData).
// HYPOTHESIS: layout of the untouched part.
struct wnWarioGarlicData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const wnWarioGarlicParam* m_param; // +0x34
};

class wnWarioGarlicParamAccesser : public wnExtendParamAccesser<3999, 1, 23999, 0, 0, 0> {
public:
    explicit wnWarioGarlicParamAccesser(const wnWarioGarlicParam* param) : wnExtendParamAccesser<3999, 1, 23999, 0, 0, 0>(0x61) {
        m_floats[0] = &param->unk0;
    }
    virtual ~wnWarioGarlicParamAccesser() { }
};

class wnWarioGarlicModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 5, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0, wnInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef soGroundModuleBuildConfigNull GroundModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<1, soMotionModuleImpl, 0, 0, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 1, 0, soCollisionAttackModuleImpl, 1, false, true>
        CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soColorBlendModuleBuildConfig<-1, 0, soColorBlendModuleImpl> ColorBlendModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<1, soGeneralWorkBuildConfig<2, 2, 1>, 1, soTransitionModuleBuildConfig<wnStatusTransitionTypeList> >
        StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0> AnimCmdModuleSubBuildConfig;
};

static_assert(sizeof(wnWeaponBuilder<wnWarioGarlicModuleAccesserBuildConfig>) == 0x2070, "Wario garlic builder extent");

class wnWarioGarlic : public wnWeaponBuilder<wnWarioGarlicModuleAccesserBuildConfig> {
    const wnWarioGarlicParam* m_param; // +0x2070
    wnWarioGarlicParamAccesser m_paramAccesser; // +0x2074
    u8 unk20A0[0xC0];
public:
    wnWarioGarlic(s32 articleId, const wnArticleBuildInfo& info, const wnWarioGarlicData* data);
    virtual ~wnWarioGarlic() { }
    // HYPOTHESIS: the names of the arguments follow what activate stores them in.
    void activate(int founderTaskId, int resourceId, int team);
};
static_assert(sizeof(wnWarioGarlic) == 0x2160, "Wario garlic article extent");
#endif
