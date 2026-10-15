#pragma once

// Captain Falcon's Final Smash article, the Blue Falcon (article kind 0x8E).

#include <wn/wn_weapon_builder.h>
#include <wn/wn_extend_param_accesser.h>

WN_STATUS_PROC_VECTOR_DEFAULT_CTOR(4);

// The parameter block of the article (1 float, id 4000).
// HYPOTHESIS: meaning of the parameters.
struct wnCaptainBlueFalconParam {
    float unk0;
};

// The static data block of the article (read by its wnModuleAccesserBuildData).
// HYPOTHESIS: layout of the untouched part.
struct wnCaptainBlueFalconData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const wnCaptainBlueFalconParam* m_param; // +0x34
};

class wnCaptainBlueFalconParamAccesser : public wnExtendParamAccesser<3999, 1, 23999, 0, 0, 0> {
public:
    wnCaptainBlueFalconParamAccesser(const wnCaptainBlueFalconParam* param) : wnExtendParamAccesser<3999, 1, 23999, 0, 0, 0>(0x8E) {
        m_floats[0] = &param->unk0;
    }
    virtual ~wnCaptainBlueFalconParamAccesser() { }
};

class wnCaptainBlueFalconModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0, wnInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<8, soMotionModuleImpl, 0, 1, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 1, 0, soCollisionAttackModuleImpl, 1, false, true>
        CollisionAttackModuleBuildConfig;
    typedef soGroundModuleBuildConfigNull GroundModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef soVisibilityModuleBuildConfig<soVisibilityModuleSimple, 1> VisibilityModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<4, soGeneralWorkBuildConfig<2, 2, 1>, 4, soTransitionModuleBuildConfig<wnStatusTransitionTypeList> >
        StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 4, 0, 0, 1, 8, 0, 0, 1, 8, 0, 0, 1, 8, 0, 0> AnimCmdModuleSubBuildConfig;
    typedef soShadowModuleBuildConfig<soShadowModuleImpl> ShadowModuleBuildConfig;
    typedef soColorBlendModuleBuildConfig<-1, 1, soColorBlendModuleImpl> ColorBlendModuleBuildConfig;
    typedef soGlowModuleBuildConfigNull GlowModuleBuildConfig;
};

class wnCaptainBlueFalcon : public wnWeaponBuilder<wnCaptainBlueFalconModuleAccesserBuildConfig> {
public:
    const wnCaptainBlueFalconParam* m_param; // +0x20E0
private:
    wnCaptainBlueFalconParamAccesser m_paramAccesser; // +0x20E4
public:
    wnCaptainBlueFalcon(s32 articleId, const wnArticleBuildInfo& info, const wnCaptainBlueFalconData* data);
    virtual ~wnCaptainBlueFalcon() { }
    virtual bool notifyEventCollisionAttackCheck(u32) { return false; }
    // HYPOTHESIS: the Blue Falcon is either attached to the founder (constrained to its task) or placed freely with a scale.
    void activate(int founderTaskId, int resourceId, int team, const Vec3f* position, float lr, float scale, bool attached);
};
