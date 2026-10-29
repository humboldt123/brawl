#pragma once

// SHADOW of BrawlHeaders em/em_ai_module_impl.h (-I include comes first): the AI module of the enemies
// (mo_enemy/sora_enemy/ai/em_ai_module_impl.cpp), 0x64 bytes (embedded in Enemy at 0x1108). Each frame the status
// module's transition check runs update, which picks one of the enemy's AI units (em/ai/em_ai_unit.h) for one of the
// areas (target kinds) whose target exists, runs its anim cmds and requests its status.

#include <StaticAssert.h>
#include <em/ai/em_ai_unit.h>
#include <so/anim/so_anim_cmd_event_presenter.h>
#include <so/collision/so_collision_attack_event_presenter.h>
#include <so/so_array.h>
#ifdef EM_ENEMY_BUILDER
#include <ft/builder/ft_builder_status.h> // soArrayQueueImpl (the same class, as in em/em_enemy_modules.h)
#else
#include <so/so_array_queue_impl.h>
#endif
#include <types.h>

class soModuleAccesser;

// The AI unit the module last took and the area it took it for (-2 / -1: none).
// NOTE: unk4 keeps the name the param accesser units (duon, porky) read it by.
class emAIInfo {
public:
    int m_unitIndex; // 0x0
    int unk4;        // 0x4 the area index
    emAIInfo() : m_unitIndex(-2), unk4(-1) { }
    ~emAIInfo() { }
    // Module map name (emGeneralTermDisideModuleImpl's range): true when there is no unit or area, or the area has no
    // target any more.
    bool isInvalid(soModuleAccesser* acc);
};
static_assert(sizeof(emAIInfo) == 8, "Class is wrong size!");

#ifdef EM_AI_MODULE_IMPL_BUILDER
// The unit calls the sora_melee constructor and destructor of the history array.
#include <ft/builder/ft_dol_instances.h>
FT_DOL_ARRAY_VECTOR(emAIInfo, 2);
#endif

// One choice of switchRnd (an element of the table an anim cmd passes). HYPOTHESIS: names, from switchRnd.
struct emAIRndEntry {
    u32 m_weight;     // 0x0
    float m_scale;    // 0x4 times the enemy's value float 0x1100000C, added to the weight
    void* m_animCmd;  // 0x8 the anim cmds that decide whether the choice is allowed (null: always)
};

class emAIModuleImpl : public soAnimCmdEventObserver, public soCollisionAttackEventObserver {
public:
    soModuleAccesser* m_moduleAccesser;                       // 0x18
    emAIUnit* m_units;                                        // 0x1C
    emAIUnitParam** m_unitParams;                             // 0x20 (the enemy's AI parameter block)
    int m_unitNum;                                            // 0x24
    // HYPOTHESIS: the anim cmd interpreter groups the units' anim cmds (6) and switchRnd's (7) run in.
    int m_animCmdGroup;                                       // 0x28
    int m_rndAnimCmdGroup;                                    // 0x2C
    // What the enemy's attacks hit since initAttackAreaTarget (notifyEventCollisionAttack): bit 0 / 1 the player 1
    // fighter (the entry's main / other fighter), 2 / 3 the same for player 2, 4 / 5 an enemy of the same / another
    // create id. HYPOTHESIS: the meaning of each bit.
    u32 m_attackAreaTarget;                                   // 0x30
    // HYPOTHESIS: the AI is stopped while the enemy is inactive (Enemy::deactivate sets it, Enemy::activate clears it).
    bool m_isInactive;                                        // 0x34
    int m_keepArea;                                           // 0x38 the area the last update kept for the next one
    soArrayQueueImpl<soArrayVector<emAIInfo, 2> > m_history;  // 0x3C the previous AI infos
    emAIInfo m_currentInfo;                                   // 0x5C

    // HYPOTHESIS: aiData is the enemy's AI parameter block (a soValueAccesser constant); 6 and 7 are passed by every enemy.
    emAIModuleImpl(soModuleAccesser* acc, emAIUnit* entries, void* aiData, int entryNum, int p5, int p6);
    ~emAIModuleImpl();
    virtual bool isObserv(char group);
    virtual bool notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, soAnimCmdEventS32 unk3);
    // The module observes the attack events of its enemy (the base declares the handler pure).
    virtual void notifyEventCollisionAttack(float power, soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser);
    virtual bool notifyEventCollisionAttackCheck(u32 flags);

    bool isInvalid(int unitIndex);
    void createUnitAll();
    void reset();
    void resetFlag(int unitIndex);
    // (int flags: the original passes the value on to emAIUnit::setFlag without narrowing it.)
    void setFlag(int flag, int unitIndex);
    // Runs the AI units whose priority lies in [priMin, priMax]; true when one of them requested a status.
    // HYPOTHESIS: the meaning of the bounds (emStatusModuleImpl::checkTransition passes its priority window).
    bool update(int priMin, int priMax);
    emAIInfo* getCurrentAIInfo();
    emAIInfo* getPrevAIInfo(u32 index);
    int switchRnd(emAIRndEntry* entries, u32 num);
    int getNearestAreaTargetInTargetSearch(int targetSearchId, u32 areaMask);
    bool isAttackAreaTarget(int area);
    void initAttackAreaTarget();
};
static_assert(sizeof(emAIModuleImpl) == 0x64, "Class is wrong size!");
