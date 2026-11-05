#pragma once

// Snake's Cypher article (article kind 0x17).

#include <so/damage/so_damage_event_presenter.h>
#include <wn/wn_weapon_builder.h>

WN_STATUS_PROC_VECTOR_ARG_CTOR(4);
// The hit part vector is built by the sora_melee constructor (wn_snake_shared_arrays.h).
#include <wn/snake/wn_snake_shared_arrays.h>

#include <wn/snake/wn_snake_cypher_data.h>

class wnSnakeCypherModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0, wnInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<4, soMotionModuleImpl, 0, 0, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 1, 0, soCollisionAttackModuleImpl, 1, false, true>
        CollisionAttackModuleBuildConfig;
    typedef soGroundModuleBuildConfigNull GroundModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfig<soCollision::Category_Fighter, 1, 1, soCollisionHitModuleImpl, 0x3FF, true>
        CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef soVisibilityModuleBuildConfig<soVisibilityModuleSimple, 1> VisibilityModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<4, soGeneralWorkBuildConfig<2, 2, 1>, 4, soTransitionModuleBuildConfig<wnStatusTransitionTypeList> >
        StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 4, 0, 0, 1, 4, 0, 0, 1, 4, 0, 0, 1, 4, 0, 0> AnimCmdModuleSubBuildConfig;
    typedef soColorBlendModuleBuildConfig<-1, 0, soColorBlendModuleImpl> ColorBlendModuleBuildConfig;
};

class wnSnakeCypher : public wnWeaponBuilder<wnSnakeCypherModuleAccesserBuildConfig>, public soDamageEventObserver {
public:
    const wnSnakeCypherParam* m_param; // +0x221C
    wnSnakeCypher(s32 articleId, const wnArticleBuildInfo& info, const wnSnakeCypherData* data);
    virtual ~wnSnakeCypher() { }
    // HYPOTHESIS: argument meanings.
    void activate(int resourceId, int team, int founderTaskId);
    void shoot(const float* speedX);
    virtual void notifyEventOnDamage(soDamage* damage, bool unused, soModuleAccesser* moduleAccesser);
    void removeModelConstraint();
    virtual void updateNodeSRT();
    virtual void adjustParentGroundCollision(int unused, float* x);
};
