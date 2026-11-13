#pragma once

// The unit that builds the follower (WN_WEAPON_BUILDER) sees the full class; everything else keeps the opaque one.
#ifdef WN_WEAPON_BUILDER

#include <wn/wn_weapon_builder.h>
#include <wn/wn_article_manage_module.h>
#include <wn/wn_damage_transactor_actor.h>
#include <wn/wn_kinetic_energy_gravity.h>
#include <wn/wn_kinetic_mediator_impl.h>
#include <wn/wn_kinetic_transactor.h>
#include <wn/dedede/wn_dedede_follower_damage_transactor.h>
#include <wn/dedede/wn_dedede_follower_kinetic_transactor.h>
#include <wn/dedede/wn_dedede_follower_param_accesser.h>

// The follower's damage module takes the shared transactor object of this unit instead of a function-local copy.
template <> class soSingletonHolder<wnDededeFollowerDamageTransactorImpl> {
public:
    static wnDededeFollowerDamageTransactorImpl* getInstance() { return &g_wnDededeFollowerDamageTransactorImpl; }
};

// HYPOTHESIS: the shared null address list of the animation command packs (a sora_melee object) behind an out-of-line accessor.
template <typename T> class soSingletonConcrete {
public:
    static T* getInstance();
};

// HYPOTHESIS: the static data block of the follower; only the observed offsets are named.
struct wnDededeFollowerData {
    s32 m_resGroupNo;
    u8 unk4[0x54];
    const wnParamDededeFollowerCommon* m_common; // +0x58
    const wnParamDededeFollower* m_param;        // +0x5C
    u32 m_hitDataStorage;                        // +0x60, start of an opaque set
};

WN_ARTICLE_EVENT_OBSERVER_VECTOR(1);
FT_DOL_ARRAY_VECTOR(soModelNodeSetUp, 3);
FT_DOL_ARRAY_VECTOR(soCollisionHitPart, 1);
FT_DOL_ARRAY_VECTOR(soArticle*, 1);
WN_STATUS_PROC_VECTOR_ARG_CTOR(26);

// The energies are those of the Diddy Kong barrel.
typedef soTypeList<soInstancePoolInfo<soKineticEnergyNormal, 1, soKineticEnergyHolder, soKineticEnergyInitInfo<0, 1> >,
        soTypeList<soInstancePoolInfo<wnKineticEnergyGravity, 1, soKineticEnergyHolder, soKineticEnergyInitInfo<1, 1> >,
        soTypeList<soInstancePoolInfo<soKineticEnergyNormal, 1, soKineticEnergyHolder, soKineticEnergyInitInfo<2, 2> >,
        soTypeList<soInstancePoolInfo<soKineticEnergyGroundMovement, 1, soKineticEnergyHolder, soKineticEnergyInitInfo<3, 8> >,
        soTypeList<soInstancePoolInfo<soKineticEnergyWindNormal, 1, soKineticEnergyHolder, soKineticEnergyInitInfo<4, 4> >,
        soTypeListNullType> > > > > wnDededeFollowerBuildEnergyTypeList;
typedef soKineticTransactorInfo<wnDededeFollowerKineticTransactor, wnKineticTransactor, wnDededeFollowerKineticTransactor,
                                soKineticTransactHelper> wnDededeFollowerBuildKineticTransactorInfo;

// HYPOTHESIS: every argument of the module configurations follows the truncated names of the original's destructors.
struct wnDededeFollowerModuleAccesserBuildConfig : wnCommonBuildConfig {
    typedef soKineticModuleBuildConfig<soKineticModuleGenericImpl,
                                       soKineticMediatorImpl<wnDededeFollowerBuildEnergyTypeList, wnDededeFollowerBuildKineticTransactorInfo> >
        KineticModuleBuildConfig;
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0,
                                                 wnInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef soModelModuleBuildConfig<3, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<28, soMotionModuleImpl, 0, 0, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> >
        MotionModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 1, 0, soCollisionAttackModuleImpl, 1, false, true>
        CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfig<soCollision::Category_Fighter, 1, 1, soCollisionHitModuleImpl, 0x3ff, true>
        CollisionHitModuleBuildConfig;
    typedef wnDamageModuleBuildConfig<soDamageModuleActor, 1, soSingletonHolder<wnDededeFollowerDamageTransactorImpl>,
                                      soSingletonHolder<soDamageEffectorNull> >
        DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef soVisibilityModuleBuildConfig<soVisibilityModuleImpl, 1> VisibilityModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<26, soGeneralWorkBuildConfig<12, 11, 16>, 26,
                                      soTransitionModuleBuildConfig<wnStatusTransitionTypeList> >
        StatusModuleBuildConfig;
    typedef wnNullMediatorArticleManageModuleBuilder<1> GenerateArticleManageModuleBuilder;
    typedef soAreaModuleBuildConfigWeapon<2, 4> AreaModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 26, 0, 0, 1, 28, 0, 0, 1, 28, 0, 0, 1, 28, 0, 0> AnimCmdModuleSubBuildConfig;
};

template <> class wnWeaponBuilder<wnDededeFollowerModuleAccesserBuildConfig> : public Weapon {
protected:
    wnModuleAccesserBuilder<wnDededeFollowerModuleAccesserBuildConfig> m_modules;
public:
    wnWeaponBuilder(const char* name, s32 articleId, const wnArticleBuildInfo& info,
                    const wnModuleAccesserBuildData& bd) __attribute__((never_inline));
    virtual ~wnWeaponBuilder() { }
    virtual void deactivateDescendantForce() {
        wnDeactivateArticleDescendants(this->m_modules.m_generateArticleBuilder);
    }
};

// The native constructor and RTTI establish this direct builder base.
// Only pointer-based status operations are declared here; the builder storage
// and the follower's appended fields remain unreconstructed.
class wnDededeFollower : public wnWeaponBuilder<wnDededeFollowerModuleAccesserBuildConfig> {
    const wnDededeFollowerData* m_data;                                // +0x2644
    const wnParamDededeFollowerCommon* m_common;                       // +0x2648
    const wnParamDededeFollower* m_param;                              // +0x264C
    wnDededeFollowerParamAccesser m_paramAccesser;                     // +0x2650
    soArrayContractibleTable<const acAnimCmdConv*> m_animCmdTable0;    // +0x2734
    soAnimCmdAddressPackArraySeparate m_animCmdPack0;                  // +0x2744
    soArrayContractibleTable<const acAnimCmdConv*> m_animCmdTable1;    // +0x2760
    soAnimCmdAddressPackArraySeparate m_animCmdPack1;                  // +0x2770
    soArrayContractibleTable<const acAnimCmdConv*> m_animCmdTable2;    // +0x278C
    soAnimCmdAddressPackArraySeparate m_animCmdPack2;                  // +0x279C
public:
    wnDededeFollower(s32 articleId, const wnArticleBuildInfo& info, const wnDededeFollowerData* data);
    virtual void processUpdate();
    virtual void processHit();
    void toShoot(Vec3f* pos, float lr, float unk1, float unk2, bool unk3);
    virtual void notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser);
    bool isPickup();
    bool isFinal();
    void updateAttackInfoStatus();
    virtual bool isCanInhaled();
    virtual bool isCanEat();
    virtual void onDeactivate();
    virtual void notifyEventCollisionHit(float power, soCollisionAttackData* attackData, u32 index, int, soModuleAccesser* moduleAccesser, soCollisionLog* log);
    void had(int ownerTaskId, int nodeId, int teamNo);
    bool isPossibleDeadChange();
    void toDead();
};

#else

#include <wn/wn_weapon_builder.h>

struct wnDededeFollowerModuleAccesserBuildConfig;

// The native constructor and RTTI establish this direct builder base.
// Only pointer-based status operations are declared here; the builder storage
// and the follower's appended fields remain unreconstructed.
class wnDededeFollower : public wnWeaponBuilder<wnDededeFollowerModuleAccesserBuildConfig> {
public:
    virtual void processUpdate();
    virtual void processHit();
    void toShoot(Vec3f* pos, float lr, float unk1, float unk2, bool unk3);
    virtual void notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser);
    bool isPickup();
    bool isFinal();
    void updateAttackInfoStatus();
    virtual bool isCanInhaled();
    virtual bool isCanEat();
    void had(int ownerTaskId, int nodeId, int teamNo);
    bool isPossibleDeadChange();
    void toDead();
};

#endif
