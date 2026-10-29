#pragma once

// Ivysaur's Vine Whip (article kind 75): the lasso-style vine of the Pokemon Trainer's Ivysaur. It shares the lasso statuses
// of the sora_melee module with Lucas's Rope Snake and Link's clawshot. HYPOTHESIS: all field names; offsets follow the
// native code.

#include <wn/wn_weapon_builder.h>
#include <wn/wn_extend_param_accesser.h>

// The parameter block of the vine: two floats (ids 4000..). HYPOTHESIS: meaning of the parameters.
struct wnPokeFushigisouVineParam {
    float unk0, unk4;
};

// The static data block of the article (read by its wnModuleAccesserBuildData). HYPOTHESIS: layout of the untouched part.
struct wnPokeFushigisouVineData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const wnPokeFushigisouVineParam* m_param; // +0x34
};

class wnPokeFushigisouVineParamAccesser : public wnExtendParamAccesser<3999, 2, 23999, 0, 0, 0> {
public:
    wnPokeFushigisouVineParamAccesser(const wnPokeFushigisouVineParam* param) : wnExtendParamAccesser<3999, 2, 23999, 0, 0, 0>(75) {
        m_floats[0] = &param->unk0;
        m_floats[1] = &param->unk4;
    }
    virtual ~wnPokeFushigisouVineParamAccesser() { }
};

// The status process pointer vector uses the no-argument constructor, not the size overload.
WN_STATUS_PROC_VECTOR_ARG_CTOR(9);

struct wnPokeFushigisouVineModuleAccesserBuildConfig : wnCommonBuildConfig {
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0, wnInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<2, soMotionModuleImpl, 0, 1, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> >
        MotionModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 1, 0, soCollisionAttackModuleImpl, 1, false, true>
        CollisionAttackModuleBuildConfig;
    typedef soGroundModuleBuildConfigNull GroundModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soSoundModuleBuildConfigNull SoundModuleBuildConfig;
    typedef soKineticModuleBuildConfigNull KineticModuleBuildConfig;
    typedef soReflectModuleBuildConfigNull ReflectModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef soPhysicsModuleBuildConfigCap<0, soPhysicsModuleImpl> PhysicsModuleBuildConfig;
    typedef soColorBlendModuleBuildConfig<-1, 0, soColorBlendModuleImpl> ColorBlendModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<9, soGeneralWorkBuildConfig<3, 3, 1>, 9, soTransitionModuleBuildConfig<wnStatusTransitionTypeList> >
        StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 9, 0, 0, 1, 2, 0, 0, 1, 2, 0, 0, 1, 2, 0, 0> AnimCmdModuleSubBuildConfig;
};

class wnPokeFushigisouVine : public wnWeaponBuilder<wnPokeFushigisouVineModuleAccesserBuildConfig> {
public:
    const wnPokeFushigisouVineParam* m_param;          // +0x1F48
    wnPokeFushigisouVineParamAccesser m_paramAccesser; // +0x1F4C

    wnPokeFushigisouVine(s32 articleId, const wnArticleBuildInfo& info, const wnPokeFushigisouVineData* data);
    virtual ~wnPokeFushigisouVine() { }
    // HYPOTHESIS: parameter meanings and the position of the float among them (register use only); the three ints after the
    // resource id fill descriptor words 0xC..0x14, the flag attaches the vine to the founder.
    void activate(int founderTaskId, int resourceId, int unkC, int unk10, int unk14, int team, Vec3f* position, float lr,
                  bool attach);
    virtual bool notifyEventCollisionAttackCheck(u32) { return false; }
};
