#pragma once

// The head of Ness's Yo-Yo (article kind 108): the weight at the end of the string (wnNessYoyo, kind 107) that damages opponents
// and takes the string up again. HYPOTHESIS: all field names; offsets follow the native code.

#include <wn/wn_weapon_builder.h>
#include <wn/wn_extend_param_accesser.h>

// The parameter block of the head: nine floats (ids 4000..4008).
struct wnNessYoyoHeadParam {
    float unk0, unk4, unk8, unkC, unk10, unk14, unk18, unk1C, unk20;
};

// The static data block of the article (read by its wnModuleAccesserBuildData). HYPOTHESIS: layout of the untouched part.
struct wnNessYoyoHeadData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const wnNessYoyoHeadParam* m_param; // +0x34
};

class wnNessYoyoHeadParamAccesser : public wnExtendParamAccesser<3999, 9, 23999, 0, 0, 0> {
public:
    wnNessYoyoHeadParamAccesser(const wnNessYoyoHeadParam* param) : wnExtendParamAccesser<3999, 9, 23999, 0, 0, 0>(108) {
        m_floats[0] = &param->unk0;
        m_floats[1] = &param->unk4;
        m_floats[2] = &param->unk8;
        m_floats[3] = &param->unkC;
        m_floats[4] = &param->unk10;
        m_floats[5] = &param->unk14;
        m_floats[6] = &param->unk18;
        m_floats[7] = &param->unk1C;
        m_floats[8] = &param->unk20;
    }
    virtual ~wnNessYoyoHeadParamAccesser() { }
};

// The status process pointer vector uses the no-argument constructor, not the size overload.
#ifndef WN_NESS_YOYO_PROC_VECTOR
#define WN_NESS_YOYO_PROC_VECTOR // shared by the Yo-Yo and its head
WN_STATUS_PROC_VECTOR_DEFAULT_CTOR(3);
#endif

struct wnNessYoyoHeadModuleAccesserBuildConfig : wnCommonBuildConfig {
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 5, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0, wnInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<6, soMotionModuleImpl, 0, 0, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> >
        MotionModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 1, 0, soCollisionAttackModuleImpl, 1, false, true>
        CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<6, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef soColorBlendModuleBuildConfig<-1, 0, soColorBlendModuleImpl> ColorBlendModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<3, soGeneralWorkBuildConfig<4, 6, 1>, 3, soTransitionModuleBuildConfig<wnStatusTransitionTypeList> >
        StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 3, 0, 0, 1, 6, 0, 0, 1, 6, 0, 0, 1, 6, 0, 0> AnimCmdModuleSubBuildConfig;
};

class wnNessYoyoHead : public wnWeaponBuilder<wnNessYoyoHeadModuleAccesserBuildConfig> {
public:
    const wnNessYoyoHeadParam* m_param;          // +0x2168
    wnNessYoyoHeadParamAccesser m_paramAccesser; // +0x216C

    wnNessYoyoHead(s32 articleId, const wnArticleBuildInfo& info, const wnNessYoyoHeadData* data);
    virtual ~wnNessYoyoHead() { }
    virtual void notifyEventLink(soLinkEventArgs* event, soModuleAccesser* acc, StageObject* sender, int unk4);
    virtual void notifyEventCollisionAttack(float power, soCollisionLog* log, soModuleAccesser* acc);
    virtual bool notifyEventCollisionAttackCheck(u32 flags);
    void activate(int founderTaskId, int kind, int team, Vec3f* pos, float lr, bool flag);
};
