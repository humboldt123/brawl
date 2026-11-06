#pragma once

#include <ft/ft_common_data_accesser.h>
#include <ft/ft_fighter_builder.h>
#include <ft/poke/ft_pokemon.h>
#include <so/collision/so_collision_log_group_share_module.h>
#include <sr/sr_common.h>
#include <types.h>

////////////////////////////////////////
// PokeFushigisou Build Configuration
////////////////////////////////////////
// The modules that differ from PokeLizardon (the other units of the REL share its builder instances): 283 statuses,
// 477 motions with two partial animations, 8 hit parts and five catch parts (the vine grab).

typedef soInsideEventManageModuleBuildConfig<
    32, 40, 4, 4, 4, 4, 4, 4, 5, 4, 4, 4, 4, 4, 1, 1, 0, 2, 1, ftInsideEventManageModuleTypes
> ftPokeFushigisouInsideEventManageModuleBuildConfig;
typedef soHeapModuleBuildConfig<soHeapModuleImpl> ftPokeFushigisouHeapModuleBuildConfig;
typedef soParamCustomizeModuleBuildConfig<ftParamCustomizeModuleImpl> ftPokeFushigisouParamCustomizeModuleBuildConfig;
typedef soResourceModuleBuildConfigDynamic<0, ftResourceIdAccesserImpl, soResourceModuleImpl> ftPokeFushigisouResourceModuleBuildConfig;
typedef soModelModuleBuildConfig<8, 3, soModelModuleImpl> ftPokeFushigisouModelModuleBuildConfig;

typedef ftAnimCmdModuleSubBuildConfig<283, 477> ftPokeFushigisouAnimCmdModuleSubBuildConfig;
typedef soMotionModuleBuildConfig<477, soMotionModuleImpl, 2, 1, soTransitionModuleBuildConfig<ftMotionTransitionTypeList>, soMotionAnimObjCacheModuleBuildConfig<5, soMotionAnimObjCacheModuleImpl> > ftPokeFushigisouMotionModuleBuildConfig;
typedef soCollisionHitModuleBuildConfig<soCollision::Category_Fighter, 8, 1, soCollisionHitModuleImpl, 0x3ff, true> ftPokeFushigisouCollisionHitModuleBuildConfig;
typedef soCollisionShieldModuleBuildConfigGroups<2, 1, 1, soCollisionShieldEventPresenterShield, soCollisionShieldModuleImpl> ftPokeFushigisouCollisionShieldModuleBuildConfig;
typedef soCollisionCatchModuleBuildConfigParts<5, soCollisionCatchModuleImpl> ftPokeFushigisouCollisionCatchModuleBuildConfig;
typedef soLinkModuleBuildConfigCap<6, soLinkModuleImpl> ftPokeFushigisouLinkModuleBuildConfig;
typedef soStatusModuleBuildConfig<283, soGeneralWorkBuildConfig<18, 14, 2>, 274, 71, soTransitionModuleBuildConfig<ftStatusTransitionTypeList> > ftPokeFushigisouStatusModuleBuildConfig;

// HYPOTHESIS: soGenerateArticleManageModuleBuilder (the article pools). Only the fighter unit builds the real pools; the other
// units of the REL see an opaque block of the same size.
#ifdef FT_POKEFUSHIGISOU_ARTICLE_POOLS
#include <ft/poke/ft_poke_fushigisou_article_pools.h>
typedef ftPokeFushigisouArticleManageModuleBuilder ftPokeFushigisouGenerateArticleManageModuleBuilder;
#else
typedef ftOpaqueGenerateArticleManageModuleBuilder<0x9278, Fighter_PokeFushigisou> ftPokeFushigisouGenerateArticleManageModuleBuilder;
#endif

class ftPokeFushigisouBuildConfig : public ftCommonBuildConfig {
public:
    typedef ftPokeFushigisouInsideEventManageModuleBuildConfig InsideEventManageModuleBuildConfig;
    typedef ftPokeFushigisouHeapModuleBuildConfig HeapModuleBuildConfig;
    typedef ftPokeFushigisouParamCustomizeModuleBuildConfig ParamCustomizeModuleBuildConfig;
    typedef ftPokeFushigisouResourceModuleBuildConfig ResourceModuleBuildConfig;
    typedef ftPokeFushigisouAnimCmdModuleSubBuildConfig AnimCmdModuleSubBuildConfig;
    typedef ftPokeFushigisouModelModuleBuildConfig ModelModuleBuildConfig;
    typedef ftPokeFushigisouMotionModuleBuildConfig MotionModuleBuildConfig;
    typedef ftPokeFushigisouCollisionHitModuleBuildConfig CollisionHitModuleBuildConfig;
    typedef ftPokeFushigisouCollisionShieldModuleBuildConfig CollisionShieldModuleBuildConfig;
    typedef ftPokeFushigisouCollisionCatchModuleBuildConfig CollisionCatchModuleBuildConfig;
    typedef ftPokeFushigisouLinkModuleBuildConfig LinkModuleBuildConfig;
    typedef ftPokeFushigisouStatusModuleBuildConfig StatusModuleBuildConfig;
    typedef ftPokeFushigisouGenerateArticleManageModuleBuilder GenerateArticleManageModuleBuilder;
};

// The log group vector's constructor is the sora_melee instance (only declared here).
FT_DOL_ARRAY_VECTOR(s32, 2);

class wnPokeFushigisouLeafCutter;
class wnPokeFushigisouSeed;
class wnPokeFushigisouVine;

class ftPokeFushigisou : public ftFighterBuilder<ftPokeFushigisouBuildConfig> {
    soArrayContractibleTable<const soStatusData> m_statusTable; // +0x11104
    ftData* m_data;                                             // +0x11114
    ftPokemonStaminaModule m_stamina;                           // +0x11118
    ftPokemonEventManageModule m_eventManage;                   // +0x11130
    // HYPOTHESIS: the collision log groups the vine grab shares (two entries; the module is reset on status 0x112).
    soArrayVector<s32, 2> m_logGroups;                          // +0x1114C
    soCollisionLogGroupShareModule m_logGroupShare;             // +0x11160
public:
    ftPokeFushigisou(s32 entryId,
            Heaps::HeapType instHeap,
            Heaps::HeapType nwModelInstHeap,
            Heaps::HeapType nwMotionInstHeap);
    virtual ~ftPokeFushigisou();
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
    virtual void notifyEventSetRumble(int kind, int unk2);
    virtual void notifyEventStopRumble(int kind);
    virtual bool notifyEventPokemonRequestChange(Vec3f* pos, float* lr);
    virtual void notifyEventPokemonTrainerUpdate();
    // The hooks the article mediator calls to start an article of the pool (HYPOTHESIS: names follow PokeLizardon's).
    static bool activeArticle(wnPokeFushigisouLeafCutter* weapon, soModuleAccesser* acc);
    static bool activeArticle(wnPokeFushigisouSeed* weapon, soModuleAccesser* acc);
    static bool activeArticle(wnPokeFushigisouVine* weapon, soModuleAccesser* acc);
};
static_assert(sizeof(ftPokeFushigisou) == 0x1116C, "ftPokeFushigisou layout is wrong!");
