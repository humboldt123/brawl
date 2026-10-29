#pragma once

#include <ft/ft_fighter_builder.h>
#include <sr/sr_common.h>
#include <so/event/so_event_presenter.h>
#include <types.h>

////////////////////////////////////////
// PokeLizardon Build Configuration
////////////////////////////////////////

typedef soInsideEventManageModuleBuildConfig<
    32, 40, 4, 4, 4, 4, 4, 4, 5, 4, 4, 4, 4, 4, 1, 1, 0, 2, 1, ftInsideEventManageModuleTypes
> ftPokeLizardonInsideEventManageModuleBuildConfig;
typedef soHeapModuleBuildConfig<soHeapModuleImpl> ftPokeLizardonHeapModuleBuildConfig;
typedef soParamCustomizeModuleBuildConfig<ftParamCustomizeModuleImpl> ftPokeLizardonParamCustomizeModuleBuildConfig;
typedef soResourceModuleBuildConfigDynamic<0, ftResourceIdAccesserImpl, soResourceModuleImpl> ftPokeLizardonResourceModuleBuildConfig;
typedef soModelModuleBuildConfig<8, 3, soModelModuleImpl> ftPokeLizardonModelModuleBuildConfig;

typedef ftAnimCmdModuleSubBuildConfig<281, 477> ftPokeLizardonAnimCmdModuleSubBuildConfig;
typedef soMotionModuleBuildConfig<477, soMotionModuleImpl, 3, 1, soTransitionModuleBuildConfig<ftMotionTransitionTypeList>, soMotionAnimObjCacheModuleBuildConfig<5, soMotionAnimObjCacheModuleImpl> > ftPokeLizardonMotionModuleBuildConfig;
typedef soCollisionHitModuleBuildConfig<soCollision::Category_Fighter, 16, 1, soCollisionHitModuleImpl, 0x3ff, true> ftPokeLizardonCollisionHitModuleBuildConfig;
typedef soCollisionShieldModuleBuildConfigGroups<2, 1, 1, soCollisionShieldEventPresenterShield, soCollisionShieldModuleImpl> ftPokeLizardonCollisionShieldModuleBuildConfig;
typedef soLinkModuleBuildConfigCap<6, soLinkModuleImpl> ftPokeLizardonLinkModuleBuildConfig;
typedef soStatusModuleBuildConfig<281, soGeneralWorkBuildConfig<18, 14, 2>, 274, 71, soTransitionModuleBuildConfig<ftStatusTransitionTypeList> > ftPokeLizardonStatusModuleBuildConfig;

// HYPOTHESIS: soGenerateArticleManageModuleBuilder (the article pools). Only the fighter unit builds the real pools; the other
// units of the REL see an opaque block of the same size.
#ifdef FT_POKELIZARDON_ARTICLE_POOLS
#include <ft/poke/ft_poke_lizardon_article_pools.h>
typedef ftPokeLizardonArticleManageModuleBuilder ftPokeLizardonGenerateArticleManageModuleBuilder;
#else
typedef ftOpaqueGenerateArticleManageModuleBuilder<0xFDF0, Fighter_PokeLizardon> ftPokeLizardonGenerateArticleManageModuleBuilder;
#endif

class ftPokeLizardonBuildConfig : public ftCommonBuildConfig {
public:
    typedef ftPokeLizardonInsideEventManageModuleBuildConfig InsideEventManageModuleBuildConfig;
    typedef ftPokeLizardonHeapModuleBuildConfig HeapModuleBuildConfig;
    typedef ftPokeLizardonParamCustomizeModuleBuildConfig ParamCustomizeModuleBuildConfig;
    typedef ftPokeLizardonResourceModuleBuildConfig ResourceModuleBuildConfig;
    typedef ftPokeLizardonAnimCmdModuleSubBuildConfig AnimCmdModuleSubBuildConfig;
    typedef ftPokeLizardonModelModuleBuildConfig ModelModuleBuildConfig;
    typedef ftPokeLizardonMotionModuleBuildConfig MotionModuleBuildConfig;
    typedef ftPokeLizardonCollisionHitModuleBuildConfig CollisionHitModuleBuildConfig;
    typedef ftPokeLizardonCollisionShieldModuleBuildConfig CollisionShieldModuleBuildConfig;
    typedef ftPokeLizardonLinkModuleBuildConfig LinkModuleBuildConfig;
    typedef ftPokeLizardonStatusModuleBuildConfig StatusModuleBuildConfig;
    typedef ftPokeLizardonGenerateArticleManageModuleBuilder GenerateArticleManageModuleBuilder;
};

// HYPOTHESIS: the Pokemon Trainer's party state; both modules are constructed in sora_melee from the first extend parameter block.
class ftPokemonStaminaModule {
    u8 unk0[0xC];
public:
    // HYPOTHESIS: unkC / unk14 are copied into the work floats 0x11000013 / 0x11000014 at the start.
    float unkC;
    float unk10;
    float unk14;
    ftPokemonStaminaModule(const void* extendParam, soModuleAccesser* moduleAccesser);
    ~ftPokemonStaminaModule() { }
};

// HYPOTHESIS: the entry event presenter is the first base (the fighter walks the observer list at the address of the module).
class ftPokemonEventManageModule : public soEventPresenter<ftEntryEventObserver> {
    u8 unkC[0x1C - 0xC];
public:
    ftPokemonEventManageModule(s32 entryId, soModuleAccesser* moduleAccesser, ftPokemonStaminaModule* stamina);
    virtual ~ftPokemonEventManageModule() { }
    // HYPOTHESIS: inline presenter helpers in the style of the trainer's ftEntryEventPresenter: each tells every observer.
    void notifyEventPokemonStart(int taskId) {
        soInstanceManagerFullProperty<ftEntryEventObserver*>* list = getObserverList();
        for (int i = 0; i < (s32)list->size(); i++) {
            list->atIndex(i)->notifyEventPokemonStart(taskId);
        }
    }
    void notifyEventPokemonCollect() {
        soInstanceManagerFullProperty<ftEntryEventObserver*>* list = getObserverList();
        for (int i = 0; i < (s32)list->size(); i++) {
            list->atIndex(i)->notifyEventPokemonCollect();
        }
    }
    void notifyEventPokemonChangeCancel() {
        soInstanceManagerFullProperty<ftEntryEventObserver*>* list = getObserverList();
        for (int i = 0; i < (s32)list->size(); i++) {
            list->atIndex(i)->notifyEventPokemonChangeCancel();
        }
    }
};

class wnPokeLizardonBreath;
class wnPokeLizardonRock;
class wnPokeLizardonRockStone;

class ftPokeLizardon : public ftFighterBuilder<ftPokeLizardonBuildConfig> {
    soArrayContractibleTable<const soStatusData> m_statusTable; // +0x17F90
    ftData* m_data;                                             // +0x17FA0
    ftPokemonStaminaModule m_stamina;                           // +0x17FA4
    ftPokemonEventManageModule m_eventManage;                   // +0x17FBC
    bool m_unk17FD8;
    u8 unk17FD9[3];
public:
    ftPokeLizardon(s32 entryId,
            Heaps::HeapType instHeap,
            Heaps::HeapType nwModelInstHeap,
            Heaps::HeapType nwMotionInstHeap);
    virtual ~ftPokeLizardon();
    virtual void processUpdate();
    virtual bool notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, s32 unk3);
    virtual bool isObserv(char kind);
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
    virtual void notifyEventChangeMotion(int motionKind, int unk2, void* motionData, soModuleAccesser* moduleAccesser);
    virtual void notifyEventOnDamage(soDamage* damage, bool unk2, soModuleAccesser* moduleAccesser);
    virtual void notifyEventSetRumble(int kind, int unk2);
    virtual void notifyEventStopRumble(int kind);
    virtual bool notifyEventPokemonRequestChange(Vec3f* pos, float* lr);
    virtual void notifyEventPokemonTrainerUpdate();
    // The hooks the article mediator calls to start an article of the pool (HYPOTHESIS: names follow the module map).
    static bool activeArticle(wnPokeLizardonRock* weapon, soModuleAccesser* acc);
};
