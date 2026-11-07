#pragma once

// The trainer's long animation-command override puts an extra Fighter virtual before getOwner.
#ifndef FT_FIGHTER_ANIMCMD_LONG
#define FT_FIGHTER_ANIMCMD_LONG
#endif
#include <ft/ft_fighter_builder.h>
#include <types.h>
#include <ft/ft_extend_param_accesser.h>

// HYPOTHESIS: inside-event article cap is 2 (the map's ninth number). Status unk starts with 13
// and is unused by the builder layout. Motion partial-anim count is 0.
typedef soInsideEventManageModuleBuildConfig<
    32, 40, 4, 4, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 1, 1, 0, 2, 1, ftInsideEventManageModuleTypes
> ftPokeTrainerInsideEventManageModuleBuildConfig;
typedef soHeapModuleBuildConfig<soHeapModuleImpl> ftPokeTrainerHeapModuleBuildConfig;
typedef soParamCustomizeModuleBuildConfig<ftParamCustomizeModuleImpl> ftPokeTrainerParamCustomizeModuleBuildConfig;
typedef soResourceModuleBuildConfigDynamic<0, ftResourceIdAccesserImpl, soResourceModuleImpl> ftPokeTrainerResourceModuleBuildConfig;
typedef soModelModuleBuildConfig<3, 0, soModelModuleImpl> ftPokeTrainerModelModuleBuildConfig;
typedef ftAnimCmdModuleSubBuildConfig<299, 515> ftPokeTrainerAnimCmdModuleSubBuildConfig;
typedef soMotionModuleBuildConfig<515, soMotionModuleImpl, 0, 1, soTransitionModuleBuildConfig<ftMotionTransitionTypeList>, soMotionAnimObjCacheModuleBuildConfig<5, soMotionAnimObjCacheModuleImpl> > ftPokeTrainerMotionModuleBuildConfig;
typedef soStatusModuleBuildConfig<299, soGeneralWorkBuildConfig<18, 14, 2>, 274, 13, soTransitionModuleBuildConfig<ftStatusTransitionTypeList> > ftPokeTrainerStatusModuleBuildConfig;
typedef soGeneralWorkBuildConfig<76, 35, 3> ftPokeTrainerGeneralWorkBuildConfig;

class ftPokeTrainerBuildConfig : public ftCommonBuildConfig {
public:
    typedef ftPokeTrainerInsideEventManageModuleBuildConfig InsideEventManageModuleBuildConfig;
    typedef ftPokeTrainerHeapModuleBuildConfig HeapModuleBuildConfig;
    typedef ftPokeTrainerParamCustomizeModuleBuildConfig ParamCustomizeModuleBuildConfig;
    typedef ftPokeTrainerResourceModuleBuildConfig ResourceModuleBuildConfig;
    typedef ftPokeTrainerModelModuleBuildConfig ModelModuleBuildConfig;
    typedef ftPokeTrainerAnimCmdModuleSubBuildConfig AnimCmdModuleSubBuildConfig;
    typedef ftPokeTrainerMotionModuleBuildConfig MotionModuleBuildConfig;
    typedef ftPokeTrainerStatusModuleBuildConfig StatusModuleBuildConfig;
    typedef ftPokeTrainerGeneralWorkBuildConfig GeneralWorkBuildConfig;
    typedef ftNullGenerateArticleManageModuleBuilder GenerateArticleManageModuleBuilder;
};

class ftPokeTrainer;
class wnPokeTrainerMBall;

// The Final Smash article of the trainer (wn_poke_trainer_final.cpp). HYPOTHESIS: only the position query is known here.
class wnPokeTrainerFinal : public StageObject {
public:
    Vec3f getFinalPokemonPos(int index);
};

// Sends the entry events (a Pokemon dies, a Pokemon starts, ...) to the observers of the entry. The trainer owns one for the
// Pokemon it tells about its own update.
class ftEntryEventPresenter : public soEventPresenter<ftEntryEventObserver> {
public:
    virtual ~ftEntryEventPresenter() { }
    bool notifyEventPokemonRequestChange(Vec3f* pos, float* lr) {
        soInstanceManagerFullProperty<ftEntryEventObserver*>* list = getObserverList();
        for (int i = 0; i < (s32)list->size(); i++) {
            if (list->atIndex(i)->notifyEventPokemonRequestChange(pos, lr) == 1) {
                return true;
            }
        }
        return false;
    }
    void notifyEventPokemonTrainerUpdate() {
        soInstanceManagerFullProperty<ftEntryEventObserver*>* list = getObserverList();
        for (int i = 0; i < (s32)list->size(); i++) {
            list->atIndex(i)->notifyEventPokemonTrainerUpdate();
        }
    }
};

// The module that walks the trainer around the stage while a Pokemon is out (ft_poke_trainer_wandering_module.cpp).
// HYPOTHESIS: parameter types.
class ftPokeTrainerWanderingModule {
public:
    u8 m_data[0x60];
    ~ftPokeTrainerWanderingModule();
    void replace(u8 index, ftPokeTrainer* trainer, soModuleAccesser* acc);
    void setDirectionTarget(soModuleAccesser* acc);
    void start(ftPokeTrainer* trainer, Vec3f* pos, Vec3f* unk, soModuleAccesser* acc);
    Vec3f getTargetUpperPos(soModuleAccesser* acc);
    void preprocessFixPosition(ftPokeTrainer* trainer, soModuleAccesser* acc);
    void processFixPosition(ftPokeTrainer* trainer, Fighter* partner, soModuleAccesser* acc);
};

// Pokemon Trainer (fighter kind 0x1C). It is the one fighter that is built into the main melee module: the three
// Pokemon it hands out are separate fighters (ft_poke_zenigame / fushigisou / lizardon) with their own RELs.
class ftPokeTrainer : public ftFighterBuilder<ftPokeTrainerBuildConfig> {
    u8 unkTail[0x7F9C - sizeof(ftFighterBuilder<ftPokeTrainerBuildConfig>)];
public:
    ftData* m_commonData; // 0x7F9C
    ftEntryEventPresenter m_entryEventPresenter; // 0x7FA0
    ftPokeTrainerWanderingModule m_wandering; // 0x7FAC
    Vec2f m_advFollowPos; // 0x800C: where the "follow" effect of the adventure mode was last placed

    ftPokeTrainer(s32 entryId, Heaps::HeapType instHeap, Heaps::HeapType nwModelInstHeap, Heaps::HeapType nwMotionInstHeap);

    ftKind getKindDirect() const { return *reinterpret_cast<const ftKind*>(&_272[0]); }

    virtual void trainerStart(float lr, Vec3f* pos, Vec3f* unk, int startKind, bool unk5);
    virtual void trainerRestart(float lr, float* pos, bool unk3, bool unk4);
    virtual void processUpdate();
    virtual void processFixPosition();
    virtual void updateNodeSRT();
    virtual void disappear(bool effect);
    virtual void processHit();
    virtual void processFixCamera();
    virtual void renderDebug();
    virtual bool isObserv(char kind);
    virtual void notifyEventChangeSituation(SituationKind kind, SituationKind prevKind, soModuleAccesser* moduleAccesser);
    virtual void onActivate();
    virtual void onDeactivate();
    virtual void attachInstance();
    virtual void detachInstance();
    virtual void standbyAdvFollow();
    virtual void setCursor(bool);
    virtual void setNameCursor(bool);
    virtual void setLoupe(bool);
    virtual void setLoupeDamage(bool);
    virtual void setVisibility(int visibility);
    virtual void notifyEventBeat();
    virtual bool isInclude(float* rect, Vec3f* out);
    virtual void notifyEventDeadPartner(int index);
    virtual void notifyEventPartnerResourcePrepared();
    virtual void notifyEventWarp();
    virtual void notifyEventPokemonCollect();
    virtual void notifyEventPokemonChangeCancel();
    virtual void notifyEventPokemonAppeal();
    virtual void notifyEventPokemonRebirthEnd();
    virtual void notifyEventPokemonAttack();
    virtual void notifyEventPokemonInflict();
    virtual void notifyEventPokemonDamage();
    virtual bool notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, s32 unk3);
    virtual void notifyEventPokemonStart(int unk1);
    virtual void notifyEventPokemonSpecial(int unk1, int unk2);
    virtual void notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser);
    virtual void notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObj, int unk4);
    virtual void notifyEventPokeTrainerReplace(u8 index);

    Fighter* getPartner();
    static bool activeArticle(wnPokeTrainerMBall* weapon, soModuleAccesser* acc);
    void setChange(int changeKind);
    void sendPartnerPosEvents();
    void reqAdvFollowEffect(Vec2f* pos);
    bool isExistAdvFollowEffect();
};
static_assert(offsetof(ftPokeTrainer, m_commonData) == 0x7F9C, "m_commonData");
static_assert(offsetof(ftPokeTrainer, m_entryEventPresenter) == 0x7FA0, "presenter");
static_assert(offsetof(ftPokeTrainer, m_wandering) == 0x7FAC, "wandering");
static_assert(offsetof(ftPokeTrainer, m_advFollowPos) == 0x800C, "follow");

class ftPokeTrainerExtendParamAccesser : public ftExtendParamAccesserEx<3999, 9, 23999, 4> {
public:
    virtual void setup(const u8* extData);
};
