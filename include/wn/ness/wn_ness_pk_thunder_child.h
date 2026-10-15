#pragma once

// The trailing bolt of Ness's PK Thunder (article kind 105): a short-lived head that is steered by the parent bolt.
// HYPOTHESIS: all field names; offsets follow the native code.

#include <wn/wn_weapon_builder.h>
#include <wn/wn_extend_param_accesser.h>

// The parameter block of the bolt: a single float (id 4000).
struct wnNessPKThunderChildParam {
    float unk0;
};

// The static data block of the article (read by its wnModuleAccesserBuildData). HYPOTHESIS: layout of the untouched part.
struct wnNessPKThunderChildData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const wnNessPKThunderChildParam* m_param; // +0x34
};

class wnNessPKThunderChildParamAccesser : public wnExtendParamAccesser<3999, 1, 23999, 0, 0, 0> {
public:
    wnNessPKThunderChildParamAccesser(const wnNessPKThunderChildParam* param) :
        wnExtendParamAccesser<3999, 1, 23999, 0, 0, 0>(105) {
        m_floats[0] = &param->unk0;
    }
    virtual ~wnNessPKThunderChildParamAccesser() { }
};
static_assert(sizeof(wnNessPKThunderChildParamAccesser) == 0x2C, "PK Thunder child parameter accesser size");

class wnNessPKThunderChildModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0, wnInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<1, soMotionModuleImpl, 0, 0, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 1, 0, soCollisionAttackModuleImpl, 1, false, true>
        CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soGroundModuleBuildConfigNull GroundModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<6, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<1, soGeneralWorkBuildConfig<2, 4, 1>, 1, soTransitionModuleBuildConfig<wnStatusTransitionTypeList> >
        StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0> AnimCmdModuleSubBuildConfig;
};

class wnNessPKThunderChild : public wnWeaponBuilder<wnNessPKThunderChildModuleAccesserBuildConfig> {
    const wnNessPKThunderChildParam* m_param;          // +0x1F4C
    wnNessPKThunderChildParamAccesser m_paramAccesser; // +0x1F50
public:
    wnNessPKThunderChild(s32 articleId, const wnArticleBuildInfo& info, const wnNessPKThunderChildData* data);
    virtual ~wnNessPKThunderChild() { }
    void activate(int founderTaskId, int linkTaskId, int team, int kind);
    virtual bool notifyEventCollisionAttackCheck(u32 flags);
    virtual void updateNodeSRT();
};
