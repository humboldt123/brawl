#pragma once

// Popo's ice shot article (article kind 109, builder kind 0x6D): the ice block the Ice Climbers' Ice Shot (side special)
// throws. It listens for damage and for attacks, and reuses the ice berg's single-normal-energy kinetic setup.

#include <so/collision/so_collision_attack_event_presenter.h>
#include <so/damage/so_damage_event_presenter.h>
#include <wn/wn_weapon_builder.h>
#include <wn/wn_kinetic_mediator_impl.h>
#include <wn/wn_kinetic_transactor.h>
#include <wn/wn_extend_param_accesser.h>
#include <ft/builder/ft_module_accesser_builder.h>

// The parameter block of the article (11 float and 7 int parameters, ids 4000.. and 24000..).
// HYPOTHESIS: meaning of the parameters.
struct wnPopoIceShotParam {
    float unk0;
    float unk4;
    float unk8;
    float unkC;
    float unk10;
    s32 unk14;
    s32 unk18;
    s32 unk1C;
    float unk20;
    float unk24;
    s32 unk28;
    s32 unk2C;
    float unk30;
    float unk34;
    float unk38;
    s32 unk3C;
    s32 unk40;
    float unk44;
};
static_assert(sizeof(wnPopoIceShotParam) == 0x48, "Ice shot parameter block extent");

// The static data block of the article (read by its wnModuleAccesserBuildData).
// HYPOTHESIS: layout of the untouched part.
struct wnPopoIceShotData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const wnPopoIceShotParam* m_param; // +0x34
    u8 unk38[8];                       // start of the collision hit data set handed to the hit module
};

struct wnArticleBuildInfo;

class wnPopoIceShotParamAccesser : public wnExtendParamAccesser<3999, 11, 23999, 7, 0, 0> {
public:
    wnPopoIceShotParamAccesser(const wnPopoIceShotParam* param) : wnExtendParamAccesser<3999, 11, 23999, 7, 0, 0>(0x6d) {
        m_floats[0] = &param->unk0;
        m_floats[1] = &param->unk4;
        m_floats[2] = &param->unk8;
        m_floats[3] = &param->unkC;
        m_floats[4] = &param->unk10;
        m_floats[5] = &param->unk20;
        m_floats[6] = &param->unk24;
        m_floats[7] = &param->unk30;
        m_floats[8] = &param->unk34;
        m_floats[9] = &param->unk38;
        m_floats[10] = &param->unk44;
        m_ints[0] = &param->unk14;
        m_ints[1] = &param->unk18;
        m_ints[2] = &param->unk1C;
        m_ints[3] = &param->unk28;
        m_ints[4] = &param->unk2C;
        m_ints[5] = &param->unk3C;
        m_ints[6] = &param->unk40;
    }
    virtual ~wnPopoIceShotParamAccesser() { }
};
static_assert(sizeof(wnPopoIceShotParamAccesser) == 0x70, "Ice shot parameter accesser extent");

// One normal energy, as the ice berg.
typedef soTypeList<soInstancePoolInfo<soKineticEnergyNormal, 1, soKineticEnergyHolder, soKineticEnergyInitInfo<0, 1> >,
                   soTypeListNullType> wnPopoIceShotKineticEnergyTypeList;

// HYPOTHESIS: the update helper's transactor carries no code of its own (the original updates the energy through the
// shared energy update).
class wnPopoIceShotKineticTransactor : public wnKineticTransactor { };

typedef soKineticTransactorInfo<wnKineticTransactor, wnPopoIceShotKineticTransactor, wnKineticTransactor, soKineticTransactHelper>
    wnPopoIceShotKineticTransactorInfo;

class wnPopoIceShotModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soKineticModuleBuildConfig<soKineticModuleGenericImpl,
                                       soKineticMediatorImpl<wnPopoIceShotKineticEnergyTypeList, wnPopoIceShotKineticTransactorInfo> >
        KineticModuleBuildConfig;
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 5, 4, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0, wnInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<3, soMotionModuleImpl, 0, 0, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 1, 0, soCollisionAttackModuleImpl, 1, false, true>
        CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfig<soCollision::Category_Fighter, 1, 1, soCollisionHitModuleImpl, 0x3ff, true>
        CollisionHitModuleBuildConfig;
    typedef wnDamageModuleBuildConfig<soDamageModuleImpl, 1, soSingletonHolder<soDamageTransactorNull>, soSingletonHolder<soDamageEffectorNull> >
        DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef soSlopeModuleBuildConfig<-1, -1, soSlopeModuleImpl> SlopeModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<3, soGeneralWorkBuildConfig<2, 2, 1>, 3, soTransitionModuleBuildConfig<wnStatusTransitionTypeList> >
        StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 3, 0, 0, 1, 2, 0, 0, 1, 2, 0, 0, 1, 2, 0, 0> AnimCmdModuleSubBuildConfig;
};

// Same no-common-energies layout as the ice berg: this unit defines WN_WEAPON_NO_COMMON_ENERGIES, the fighter pool does not.
#ifdef FT_POPO_ARTICLE_POOLS
template <>
class wnWeaponBuilder<wnPopoIceShotModuleAccesserBuildConfig> : public Weapon {
protected:
    wnModuleAccesserBuilder<wnPopoIceShotModuleAccesserBuildConfig> m_modules; // +0xCC
    u32 m_modulesPad;
public:
    // Defined in the ice shot unit. Defining it here instantiates the shared fighter posture and ground builders.
    wnWeaponBuilder(const char* name, s32 articleId, const wnArticleBuildInfo& info, const wnModuleAccesserBuildData& bd,
                    bool weaponFlag = true);
    virtual ~wnWeaponBuilder() { }
    virtual void deactivateDescendantForce() {
        wnDeactivateArticleDescendants(this->m_modules.m_generateArticleBuilder);
    }
};
#endif

class wnPopoIceShot : public wnWeaponBuilder<wnPopoIceShotModuleAccesserBuildConfig>, public soDamageEventObserver,
                      public soCollisionAttackEventObserver {
public:
    const wnPopoIceShotParam* m_param;           // +0x235C
    wnPopoIceShotParamAccesser m_paramAccesser;  // +0x2360
    u8 unk23CC[4]; // HYPOTHESIS: tail word whose owner is unknown; keeps the original 0x23D0 extent
    wnPopoIceShot(s32 articleId, const wnArticleBuildInfo& info, const wnPopoIceShotData* data);
    virtual ~wnPopoIceShot() { }
    // HYPOTHESIS: the parameter order and types follow the Popo transactor's call (founder task, resource, team,
    // position, facing, extra id) and the Blizzard activation; the map gives none.
    void activate(int founderTaskId, int resourceId, int team, const Vec3f* pos, float lr, int unk40);
    void shoot();
    virtual void processUpdate();
    virtual void notifyEventOnDamage(soDamage* damage, bool, soModuleAccesser* moduleAccesser);
    virtual void notifyEventCollisionAttack(float power, soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser);
    virtual bool notifyEventCollisionAttackCheck(u32 flags);
    virtual void notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser);
    virtual void updatePosture(bool);
};
static_assert(sizeof(wnPopoIceShot) == 0x23D0, "Popo ice shot article extent");
