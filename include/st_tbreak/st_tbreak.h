#pragma once

#include <gm/gm_lib.h>
#include <gr/gr_gimmick_catapult.h>
#include <gr/gr_gimmick_spring.h>
#include <gr/gr_madein.h>
#include <it/it_manager.h>
#include <memory.h>
#include <st/st_class_info.h>
#include <st/st_melee.h>
#include <st/st_trigger.h>
#include <types.h>

template<typename T>
class stClassInfoImpl<Stages::TargetBreak, T> : public stClassInfo {
public:
    stClassInfoImpl() : stClassInfo() {
        setClassInfo(Stages::TargetBreak, this);
    }

    virtual ~stClassInfoImpl() {
        setClassInfo(Stages::TargetBreak, 0);
    }

    virtual T* create() {
        return T::create();
    }

    virtual void preload() { }
};

// One ground object of the Break the Targets stage. The first create() argument is the model index: 0 is the
// level, 1 a target, 2 the node model the targets are attached to, 3 the item-spawn node model, 8 the moving block
// and 9 a step (see stTargetBreak::createObj).
class grTargetBreak : public grMadein {
public:
    grTargetBreak(const char* taskName) : grMadein(taskName) { setupMelee(); }
    virtual ~grTargetBreak();

    // A hit on the target is remembered for the stage (the damage taken, the attacker and where it came from).
    virtual void onDamage(int index, soDamage* damage, soDamageAttackerInfo* attackerInfo);

    void thisIsTarget() __attribute__((never_inline));
    void targetBroken() __attribute__((never_inline));

    // The stage reads and clears the hit flag of the target.
    bool wasHit() { return m_isHit; }
    float getLastDamage() { return m_hitPointInfo->m_lastDamageTaken; }
    int getLastAttacker() { return m_hitPointInfo->m_lastPlayerHit; }
    // MATCH-ONLY: sets the "enemy" bit of the hit category with a plain byte OR like the original.
    void markEnemy() { reinterpret_cast<u8*>(this)[0x16C] |= 0x8; }

    static grTargetBreak* create(int mdlIndex, const char* tgtNodeName, const char* taskName) __attribute__((never_inline));
};

// The trampoline of the stage: it throws fighters that land on it upwards when a target is hit (HYPOTHESIS).
class grGimmickTargetBreakSpring : public grGimmickSpring {
public:
    grGimmickTargetBreakSpring(const char* taskName) : grGimmickSpring(taskName) { }
    virtual ~grGimmickTargetBreakSpring();

    // Plays the sound and switches the model back to the resting pose after the spring was used.
    virtual void setMotionOff();
};

// The state of the level's coroutine (see stNewporkSeq in st_newpork.h): m_line is the number of the source line to
// resume at, m_tag is a marker that is stored with it.
struct stTargetBreakSeq {
    int m_tag;
    int m_line;
};

// A level of the stage (five of them: the level number comes from the melee rules). HYPOTHESIS: names of the fields
// follow their use in createObj / update.
struct stTargetBreakLevel {
    float m_unk0;               // 0x00
    float m_targetSize;         // 0x04: size of the hit sphere of a target
    float m_chainRadius;        // 0x08: how close the next target has to be to break in a chain reaction
    float m_chainDelay;         // 0x0C: frames until a target in range breaks
    u32 m_movingTargets;        // 0x10: bit mask: the targets that follow a moving node
    bool m_hasItemNode;         // 0x14: the level has the item spawn node model
    char _15[3];
    s32 m_catapult;             // 0x18: model index of the catapult (-1: none)
    s32 m_spring;               // 0x1C: model index of the spring (-1: none)
    s32 m_unk20[7];             // 0x20: HYPOTHESIS: (collision index, -1/0 pairs) of the spring
    struct Belt {
        s32 m_direction;        // degrees, 361: none (HYPOTHESIS)
        s32 m_unk4;
        s32 m_unk8;
        s32 m_unkC;
        s32 m_unk10;
    } m_belt[4];                // 0x3C: HYPOTHESIS: one entry per conveyor belt
};
static_assert(sizeof(stTargetBreakLevel) == 0x8C, "Class is wrong size!");

// An item the stage hands out when the level starts and the node of the item model it appears at.
struct stTargetBreakItem {
    const char* m_nodeName;
    itKind m_kind;
    u32 m_variant;
};

// Break the Targets: ten targets (more precisely: m_targets) have to be broken; hitting one starts a chain reaction
// that breaks the targets close to it, too.
class stTargetBreak : public stMelee {
    grTargetBreak* m_field;                  // 0x1D8: the level
    grTargetBreak* m_targets[10];            // 0x1DC
    grTargetBreak* m_nodeModel;              // 0x204: the node model the targets are attached to
    grTargetBreak* m_movingTargetNodes[10];  // 0x208: the targets that move have a node model of their own
    grTargetBreak* m_itemNodeModel;          // 0x230: the model with the item spawn nodes
    grTargetBreak* m_steps[12];              // 0x234
    grTargetBreak* m_stepLocators[12];       // 0x264
    grTargetBreak* m_iceField;               // 0x294: the ice of the fourth level
    u32 m_stepLocatorNode[12];               // 0x298: node index of the step locator
    char _2C8[0x30];
    grGimmickBeltConveyorData* m_beltData[4];   // 0x2F8: the level's conveyor belts
    stTrigger* m_beltTrigger[4];             // 0x308
    u32 m_beltNodeStart[4];                  // 0x318
    u32 m_beltNodeEnd[4];                    // 0x328
    grTargetBreak* m_block;                  // 0x338: the moving block of the last level
    grGimmickCatapultData* m_catapultData;   // 0x33C
    grGimmickCatapult* m_catapult;           // 0x340
    u32 m_springNode;                        // 0x344
    grGimmickSpringData* m_springData;       // 0x348
    grGimmickSpring* m_spring;               // 0x34C
    s32 m_level;                             // 0x350
    s32 m_hitCount;                          // 0x354: targets broken so far
    s32 m_targetsLeft;                       // 0x358
    stTargetBreakSeq m_seq;                  // 0x35C
    float m_targetTimer[10];                 // 0x364: frames until the target breaks in a chain reaction (-1: not hit)
    float m_chainRadiusSq;                   // 0x38C
    float m_damageTotal;                     // 0x390
    u32 m_hitsBy[2];                         // 0x394: targets broken by fighters of team 0 / 1
    u32 m_targetHitBy[10];                   // 0x39C: who started the break of a target

public:
    stTargetBreak();
    static stTargetBreak* create();

    virtual ~stTargetBreak();
    virtual void createObj();
    virtual bool loading();
    virtual void update(float deltaFrame);
    virtual bool isReStartSamePoint() { return false; }
    virtual int getBgmID() const { return 0x2712; }

    bool putItem();

    static stClassInfoImpl<Stages::TargetBreak, stTargetBreak> bss_loc_14;
};
static_assert(sizeof(stTargetBreak) == 0x3C4, "Class is wrong size!");
