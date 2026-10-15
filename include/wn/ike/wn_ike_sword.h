#pragma once

// Ike's sword article (article kind 0x3A): the sword of Ike's Final Smash / forward special that is activated at the hand.

#include <wn/wn_weapon_builder.h>
#include <wn/wn_extend_param_accesser.h>

FT_DOL_ARRAY_VECTOR(soCollisionAttackPart, 4);
WN_STATUS_PROC_VECTOR_DEFAULT_CTOR(4);

// The parameter block of the sword (one float parameter, id 4000).
// HYPOTHESIS: meaning of the parameter.
struct wnIkeSwordParam {
    float unk0;
};

// The static data block of the article (read by its wnModuleAccesserBuildData).
// HYPOTHESIS: layout of the untouched part.
struct wnIkeSwordData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const wnIkeSwordParam* m_param; // +0x34
};

class wnIkeSwordParamAccesser : public wnExtendParamAccesser<3999, 1, 23999, 0, 0, 0> {
public:
    wnIkeSwordParamAccesser(const wnIkeSwordParam* param) : wnExtendParamAccesser<3999, 1, 23999, 0, 0, 0>(0x3A) {
        m_floats[0] = &param->unk0;
    }
    virtual ~wnIkeSwordParamAccesser() { }
};

// Ike's sword has no items: the accesser gets the shared null item module.
extern char g_soItemManageModuleNull[];
template <typename T>
class wnIkeSwordNullItemManageModuleBuildConfig {
public:
    typedef T ModuleType;
};

template <typename T>
class soItemManageModuleBuilder<wnIkeSwordNullItemManageModuleBuildConfig<T> > {
public:
    soItemManageModuleBuilder(soModuleAccesser*, void*) { }
    soItemManageModule* getModule() { return (soItemManageModule*)g_soItemManageModuleNull; }
};

class wnIkeSwordModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef wnIkeSwordNullItemManageModuleBuildConfig<soItemManageModuleImpl> ItemManageModuleBuildConfig;
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 5, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0, wnInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef soGroundModuleBuildConfigNull GroundModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soColorBlendModuleBuildConfig<-1, 0, soColorBlendModuleImpl> ColorBlendModuleBuildConfig;
    typedef soShadowModuleBuildConfig<soShadowModuleImpl> ShadowModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<4, soMotionModuleImpl, 0, 1, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 4, 0, soCollisionAttackModuleImpl, 1, false, true>
        CollisionAttackModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<4, soGeneralWorkBuildConfig<2, 8, 2>, 4, soTransitionModuleBuildConfig<wnStatusTransitionTypeList> >
        StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 4, 0, 0, 1, 4, 0, 0, 1, 4, 0, 0, 1, 4, 0, 0> AnimCmdModuleSubBuildConfig;
};

class wnIkeSword : public wnWeaponBuilder<wnIkeSwordModuleAccesserBuildConfig> {
public:
    const wnIkeSwordParam* m_param; // +0x22D0
private:
    wnIkeSwordParamAccesser m_paramAccesser; // +0x22D4
public:
    wnIkeSword(s32 articleId, const wnArticleBuildInfo& info, const wnIkeSwordData* data);
    virtual ~wnIkeSword() { }
};
