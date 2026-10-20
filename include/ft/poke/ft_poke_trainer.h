#pragma once

// The trainer's long animation-command override puts an extra Fighter virtual before getOwner.
#ifndef FT_FIGHTER_ANIMCMD_LONG
#define FT_FIGHTER_ANIMCMD_LONG
#endif
#include <ft/fighter.h>
#include <types.h>

class ftPokeTrainer;

// Sends the entry events (a Pokemon dies, a Pokemon starts, ...) to the observers of the entry. The trainer owns one for the
// Pokemon it tells about its own update.
class ftEntryEventPresenter : public soEventPresenter<ftEntryEventObserver> {
public:
    virtual ~ftEntryEventPresenter() { }
    void notifyEventPokemonTrainerUpdate() {
        soInstanceManagerFullProperty<ftEntryEventObserver*>* list = getObserverList();
        for (int i = 0; i < list->size(); i++) {
            list->at(i)->notifyEventPokemonTrainerUpdate();
        }
    }
};

// The module that walks the trainer around the stage while a Pokemon is out (ft_poke_trainer_wandering_module.cpp).
// HYPOTHESIS: parameter types.
class ftPokeTrainerWanderingModule {
public:
    u8 m_data[0x60];
    void replace(u8 index, ftPokeTrainer* trainer, soModuleAccesser* acc);
    void setDirectionTarget(soModuleAccesser* acc);
    void preprocessFixPosition(ftPokeTrainer* trainer, soModuleAccesser* acc);
    void processFixPosition(ftPokeTrainer* trainer, Fighter* partner, soModuleAccesser* acc);
};

// Pokemon Trainer (fighter kind 0x1C). It is the one fighter that is built into the main melee module: the three
// Pokemon it hands out are separate fighters (ft_poke_zenigame / fushigisou / lizardon) with their own RELs.
// HYPOTHESIS: the block between Fighter and the trainer members is the module storage of
// ftFighterBuilder<ftPokeTrainerBuildConfig>; it stays opaque until the build config is recovered.
class ftPokeTrainerBuilderBlock : public Fighter {
public:
    u8 m_builderBlock[0x7F64 - 0x194];
};

class ftPokeTrainer : public ftPokeTrainerBuilderBlock {
public:
    u8 unk7F64[0x7F9C - 0x7F64];
    ftData* m_commonData; // 0x7F9C
    ftEntryEventPresenter m_entryEventPresenter; // 0x7FA0
    ftPokeTrainerWanderingModule m_wandering; // 0x7FAC
    Vec2f m_advFollowPos; // 0x800C: where the "follow" effect of the adventure mode was last placed

    ftKind getKindDirect() const { return *reinterpret_cast<const ftKind*>(&_272[0]); }

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
    virtual void notifyEventPokemonStart(int unk1);
    virtual void notifyEventPokemonSpecial(int unk1, int unk2);
    virtual void notifyEventPokeTrainerReplace(u8 index);

    Fighter* getPartner();
    void setChange(int changeKind);
    void reqAdvFollowEffect(Vec2f* pos);
    bool isExistAdvFollowEffect();
};
