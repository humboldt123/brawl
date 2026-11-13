#pragma once

#include <ft/ft_common_data_accesser.h>
#include <ft/ft_fighter_builder.h>
#include <ft/poke/ft_pokemon.h>
#include <sr/sr_common.h>
#include <types.h>

////////////////////////////////////////
// PokeZenigame Build Configuration
////////////////////////////////////////
// The modules that differ from PokeLizardon (the other units of the REL share its builder instances): 287 statuses,
// 484 motions with two partial animations, 12 hit parts, ten area instances and a kinetic mediator with the fighter's
// own transactor (its name is in the RTTI string of the mediator).

typedef soInsideEventManageModuleBuildConfig<
    32, 40, 4, 4, 4, 4, 4, 4, 5, 4, 4, 4, 4, 4, 1, 1, 0, 2, 1, ftInsideEventManageModuleTypes
> ftPokeZenigameInsideEventManageModuleBuildConfig;
typedef soHeapModuleBuildConfig<soHeapModuleImpl> ftPokeZenigameHeapModuleBuildConfig;
typedef soParamCustomizeModuleBuildConfig<ftParamCustomizeModuleImpl> ftPokeZenigameParamCustomizeModuleBuildConfig;
typedef soResourceModuleBuildConfigDynamic<0, ftResourceIdAccesserImpl, soResourceModuleImpl> ftPokeZenigameResourceModuleBuildConfig;
typedef soModelModuleBuildConfig<8, 3, soModelModuleImpl> ftPokeZenigameModelModuleBuildConfig;

typedef ftAnimCmdModuleSubBuildConfig<287, 484> ftPokeZenigameAnimCmdModuleSubBuildConfig;
typedef soMotionModuleBuildConfig<484, soMotionModuleImpl, 2, 1, soTransitionModuleBuildConfig<ftMotionTransitionTypeList>, soMotionAnimObjCacheModuleBuildConfig<5, soMotionAnimObjCacheModuleImpl> > ftPokeZenigameMotionModuleBuildConfig;
typedef soCollisionHitModuleBuildConfig<soCollision::Category_Fighter, 12, 1, soCollisionHitModuleImpl, 0x3ff, true> ftPokeZenigameCollisionHitModuleBuildConfig;
typedef soCollisionShieldModuleBuildConfigGroups<2, 1, 1, soCollisionShieldEventPresenterShield, soCollisionShieldModuleImpl> ftPokeZenigameCollisionShieldModuleBuildConfig;
typedef soLinkModuleBuildConfigCap<6, soLinkModuleImpl> ftPokeZenigameLinkModuleBuildConfig;
typedef soStatusModuleBuildConfig<287, soGeneralWorkBuildConfig<18, 14, 2>, 274, 71, soTransitionModuleBuildConfig<ftStatusTransitionTypeList> > ftPokeZenigameStatusModuleBuildConfig;
typedef soAreaModuleBuildConfigInstances<10, ftAreaModuleImpl> ftPokeZenigameAreaModuleBuildConfig;

// The fighter's own kinetic transactor: changeKinetic handles the modes 0x64 to 0x6D of the special moves, the rest is the
// ftKineticTransactor of sora_melee.
class ftPokeZenigameKineticTransactor : public ftKineticTransactor {
public:
    static void changeKinetic(int mode, void* pools, soModuleAccesser* acc);
    static void changeKineticSub(bool* result, void* pools, Vec2f* speed, soModuleAccesser* acc);
    static void changeKineticSub1(bool* result, void* pools, Vec2f* speed, soModuleAccesser* acc);
    static void changeKineticSub2(bool* result, void* pools, Vec2f* speed, soModuleAccesser* acc);
    static void changeKineticSub3(bool* result, void* pools, Vec2f* speed, soModuleAccesser* acc);
    static void changeKineticSub4(bool* result, void* pools, Vec2f* speed, soModuleAccesser* acc);
    template <typename E>
    static void updateEnergy(E* energy, soModuleAccesser* acc) {
        if (energy->isEnable() != true) {
            return;
        }
        if (energy->isSuspend()) {
            return;
        }
        return energy->updateEnergy(acc);
    }
};
// The gravity energy updates without the enable checks, and the stop energy first applies the Withdraw ground slide.
class ftKineticEnergyGravity;
class ftKineticEnergyStop;
template <>
void ftPokeZenigameKineticTransactor::updateEnergy<ftKineticEnergyGravity>(ftKineticEnergyGravity* energy, soModuleAccesser* acc);
template <>
void ftPokeZenigameKineticTransactor::updateEnergy<ftKineticEnergyStop>(ftKineticEnergyStop* energy, soModuleAccesser* acc);
typedef soKineticModuleBuildConfigMediator<soKineticModuleGenericImpl, ftKineticMediatorImplT<ftPokeZenigameKineticTransactor> > ftPokeZenigameKineticModuleBuildConfig;

// HYPOTHESIS: soGenerateArticleManageModuleBuilder (the article pools). Only the fighter unit builds the real pools; the other
// units of the REL see an opaque block of the same size.
#ifdef FT_POKEZENIGAME_ARTICLE_POOLS
#include <ft/poke/ft_poke_zenigame_article_pools.h>
typedef ftPokeZenigameArticleManageModuleBuilder ftPokeZenigameGenerateArticleManageModuleBuilder;
#else
typedef ftOpaqueGenerateArticleManageModuleBuilder<0xE2BC, Fighter_PokeZenigame> ftPokeZenigameGenerateArticleManageModuleBuilder;
#endif

class ftPokeZenigameBuildConfig : public ftCommonBuildConfig {
public:
    typedef ftPokeZenigameInsideEventManageModuleBuildConfig InsideEventManageModuleBuildConfig;
    typedef ftPokeZenigameHeapModuleBuildConfig HeapModuleBuildConfig;
    typedef ftPokeZenigameParamCustomizeModuleBuildConfig ParamCustomizeModuleBuildConfig;
    typedef ftPokeZenigameResourceModuleBuildConfig ResourceModuleBuildConfig;
    typedef ftPokeZenigameAnimCmdModuleSubBuildConfig AnimCmdModuleSubBuildConfig;
    typedef ftPokeZenigameModelModuleBuildConfig ModelModuleBuildConfig;
    typedef ftPokeZenigameMotionModuleBuildConfig MotionModuleBuildConfig;
    typedef ftPokeZenigameCollisionHitModuleBuildConfig CollisionHitModuleBuildConfig;
    typedef ftPokeZenigameCollisionShieldModuleBuildConfig CollisionShieldModuleBuildConfig;
    typedef ftPokeZenigameLinkModuleBuildConfig LinkModuleBuildConfig;
    typedef ftPokeZenigameStatusModuleBuildConfig StatusModuleBuildConfig;
    typedef ftPokeZenigameKineticModuleBuildConfig KineticModuleBuildConfig;
    typedef ftPokeZenigameAreaModuleBuildConfig AreaModuleBuildConfig;
    typedef ftPokeZenigameGenerateArticleManageModuleBuilder GenerateArticleManageModuleBuilder;
};

class wnPokeZenigameWater;

class ftPokeZenigame : public ftFighterBuilder<ftPokeZenigameBuildConfig> {
    soArrayContractibleTable<const soStatusData> m_statusTable; // +0x162E0
    ftData* m_data;                                             // +0x162F0
    ftPokemonStaminaModule m_stamina;                           // +0x162F4
    ftPokemonEventManageModule m_eventManage;                   // +0x1630C
public:
    ftPokeZenigame(s32 entryId,
            Heaps::HeapType instHeap,
            Heaps::HeapType nwModelInstHeap,
            Heaps::HeapType nwMotionInstHeap);
    virtual ~ftPokeZenigame();
    virtual void processUpdate();
    virtual bool notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, s32 unk3);
    virtual void notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObject, int unk4);
    virtual void onDeadEnd();
    virtual void onActivate();
    virtual void onDeactivate();
    virtual void onStart(int startKind);
    virtual u32 getChangeSucceedOption() { return 0x850; }
    virtual void change(void* work, int kind);
    virtual void notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser);
    virtual bool notifyEventCollisionAttackCheck(u32 flags);
    virtual void notifyEventCollisionAttack(float power, soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser);
    virtual void notifyEventOnDamage(soDamage* damage, bool unk2, soModuleAccesser* moduleAccesser);
    virtual void notifyEventSetRumble(int kind, bool unk2);
    virtual void notifyEventStopRumble(int kind);
    virtual bool notifyEventPokemonRequestChange(Vec3f* pos, float* lr);
    virtual void notifyEventPokemonTrainerUpdate();
};
static_assert(sizeof(ftPokeZenigame) == 0x16328, "ftPokeZenigame layout is wrong!");
