#pragma once

// Lucas's PK Fire bolt (article kind 78): flies with a normal and a gravity energy, its life shrinks with the damage it takes.

#include <wn/wn_weapon_builder.h>
#include <wn/wn_extend_param_accesser.h>

// The parameter block of the bolt (two int and five float parameters, ids 24000.. and 4000..).
// HYPOTHESIS: meanings beyond the uses in activate / notifyEventCollisionHit.
struct wnLucasPKFireParam {
    s32 unk0;
    s32 unk4;
    float unk8;
    float m_gravity;      // +0xC: downward acceleration of the falling bolt
    float m_speedLimit;   // +0x10: falling speed limit
    float m_damageLifeMul; // +0x14: life lost per damage point
    float unk18;
};

// The static data block of the article (read by its wnModuleAccesserBuildData).
// HYPOTHESIS: layout of the untouched part.
struct wnLucasPKFireData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const wnLucasPKFireParam* m_param; // +0x34
    void* m_hitData;                  // +0x38: the hit collisions of the bolt
};

class wnLucasPKFireParamAccesser : public wnExtendParamAccesser<3999, 5, 23999, 2, 0, 0> {
public:
    wnLucasPKFireParamAccesser(const wnLucasPKFireParam* param) : wnExtendParamAccesser<3999, 5, 23999, 2, 0, 0>(78) {
        m_floats[0] = &param->unk8;
        m_floats[1] = &param->m_gravity;
        m_floats[2] = &param->m_speedLimit;
        m_floats[3] = &param->m_damageLifeMul;
        m_floats[4] = &param->unk18;
        m_ints[0] = &param->unk0;
        m_ints[1] = &param->unk4;
    }
    virtual ~wnLucasPKFireParamAccesser() { }
};
static_assert(sizeof(wnLucasPKFireParamAccesser) == 0x44, "PK Fire parameter accesser size");

class wnLucasPKFireModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0, wnInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<2, soMotionModuleImpl, 0, 0, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 2, 0, soCollisionAttackModuleImpl, 1, false, true>
        CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfig<soCollision::Category_Fighter, 2, 1, soCollisionHitModuleImpl, 0x3ff, true>
        CollisionHitModuleBuildConfig;
    typedef wnDamageModuleBuildConfig<soDamageModuleImpl, 1, soSingletonHolder<soDamageTransactorNull>, soSingletonHolder<soDamageEffectorNull> >
        DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<2, soGeneralWorkBuildConfig<2, 2, 1>, 2, soTransitionModuleBuildConfig<wnStatusTransitionTypeList> >
        StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 2, 0, 0, 1, 2, 0, 0, 1, 2, 0, 0, 1, 2, 0, 0> AnimCmdModuleSubBuildConfig;
};

class wnLucasPKFire : public wnWeaponBuilder<wnLucasPKFireModuleAccesserBuildConfig> {
public:
    const wnLucasPKFireParam* m_param;         // +0x23AC
    wnLucasPKFireParamAccesser m_paramAccesser;  // +0x23B0
    wnKineticEnergyGravity m_energyGravity;     // +0x23F4
public:
    wnLucasPKFire(s32 articleId, const wnArticleBuildInfo& info, const wnLucasPKFireData* data);
    virtual ~wnLucasPKFire() { }
    virtual bool notifyEventCollisionAttackCheck(u32 flags);
    virtual void notifyEventCollisionHit(float power, soCollisionAttackData* attackData, u32 index, int unk,
                                         soModuleAccesser* moduleAccesser, soCollisionLog* collisionLog);
    // HYPOTHESIS: parameter names beyond position, speed and facing.
    void activate(int founderTaskId, int team, Vec3f* pos, Vec2f* speed, float lr, float rot);
};
