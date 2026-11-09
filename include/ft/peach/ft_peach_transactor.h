#pragma once

#include <ft/kirby/ft_kirby_copy_ability_process.h>
#include <mt/mt_vector.h>
#include <types.h>

class soModuleAccesser;
class soCollisionAttackModule;
class soCollisionLog;
class wnPeachKinopio;

// Peach's copy-ability process: the per-fighter helper singleton (the code is in sora_melee; RTTI name ftPeachTransactor,
// base ftKirbyCopyAbilityProcess) that starts the Toad article and takes part in some of the events of the fighter.
// The overridden slots are the ones of ftKirbyCopyAbilityProcess; the shield events keep the argument lists of the fighter
// events of the same name.
class ftPeachTransactor : public ftKirbyCopyAbilityProcess {
public:
    ftPeachTransactor();
    virtual ~ftPeachTransactor();
    virtual void processUpdate(soModuleAccesser* moduleAccesser);
    virtual void notifyEventChangeSituation(int kind, int prevKind, soModuleAccesser* moduleAccesser); // slot 6
    virtual void notifyEventCollisionShield(soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float posY, soModuleAccesser* moduleAccesser); // slot 8
    virtual bool notifyEventCollisionShieldCheck(soModuleAccesser* moduleAccesser); // slot 9
    virtual bool activeArticle(soArticle* article, soModuleAccesser* moduleAccesser);
    // Local declaration of the REL singleton-holder entry point.
    static ftPeachTransactor* getInstance();
    // HYPOTHESIS: runs while the fighter is in status 0x112 (the map names it processUpdateTransact).
    void updateStatus112(soModuleAccesser* moduleAccesser);
    // HYPOTHESIS: the id is the effect resource of the fighter.
    bool activeArticle1(wnPeachKinopio* weapon, soModuleAccesser* acc, u32 etcResId);
};
static_assert(sizeof(ftPeachTransactor) == 4, "Transactor singleton layout is wrong!");
