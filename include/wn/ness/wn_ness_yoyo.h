#pragma once

// Ness's Yo-Yo (article kind 107): the string of the yo-yo, a lasso-style article. Its head is a separate article
// (wnNessYoyoHead). HYPOTHESIS: all field names; offsets follow the native code.

#include <wn/wn_weapon_builder.h>
#include <wn/wn_extend_param_accesser.h>
#include <wn/ness/wn_ness_yoyo_events.h>

// The parameter block of the string: two ints (ids 24000..) and one float (id 4000).
struct wnNessYoyoParam {
    s32 unk0; // HYPOTHESIS: the number of string segments while the yo-yo is out
    s32 unk4; // HYPOTHESIS: the number of string segments otherwise
    float unk8;
};

// The static data block of the article (read by its wnModuleAccesserBuildData). HYPOTHESIS: layout of the untouched part.
struct wnNessYoyoData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const wnNessYoyoParam* m_param; // +0x34
};

class wnNessYoyoParamAccesser : public wnExtendParamAccesser<3999, 1, 23999, 2, 0, 0> {
public:
    wnNessYoyoParamAccesser(const wnNessYoyoParam* param) : wnExtendParamAccesser<3999, 1, 23999, 2, 0, 0>(107) {
        m_floats[0] = &param->unk8;
        m_ints[0] = &param->unk0;
        m_ints[1] = &param->unk4;
    }
    virtual ~wnNessYoyoParamAccesser() { }
};

// The status process pointer vector uses the no-argument constructor, not the size overload.
#ifndef WN_NESS_YOYO_PROC_VECTOR
#define WN_NESS_YOYO_PROC_VECTOR // shared by the Yo-Yo and its head
WN_STATUS_PROC_VECTOR_ARG_CTOR(3);
#endif

struct wnNessYoyoModuleAccesserBuildConfig : wnCommonBuildConfig {
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 5, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0, wnInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<1, soMotionModuleImpl, 0, 0, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> >
        MotionModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 1, 0, soCollisionAttackModuleImpl, 1, false, true>
        CollisionAttackModuleBuildConfig;
    typedef soGroundModuleBuildConfigNull GroundModuleBuildConfig;
    typedef soGlowModuleBuildConfigNull GlowModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<6, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef soPhysicsModuleBuildConfigCapNoFlag<0, soPhysicsModuleImpl> PhysicsModuleBuildConfig;
    typedef soColorBlendModuleBuildConfig<-1, 0, soColorBlendModuleImpl> ColorBlendModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<3, soGeneralWorkBuildConfig<2, 10, 1>, 3, soTransitionModuleBuildConfig<wnStatusTransitionTypeList> >
        StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 3, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0> AnimCmdModuleSubBuildConfig;
};

class wnNessYoyo : public wnWeaponBuilder<wnNessYoyoModuleAccesserBuildConfig> {
public:
    const wnNessYoyoParam* m_param;          // +0x2100
    wnNessYoyoParamAccesser m_paramAccesser; // +0x2104

    wnNessYoyo(s32 articleId, const wnArticleBuildInfo& info, const wnNessYoyoData* data);
    virtual ~wnNessYoyo() { }
    virtual void notifyEventLink(soLinkEventArgs* event, soModuleAccesser* acc, StageObject* sender, int unk4);
    void activate(int founderTaskId, int kind, int team, bool flag);
};
