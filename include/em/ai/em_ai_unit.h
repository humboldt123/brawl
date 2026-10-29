#pragma once

// One AI unit of an enemy (mo_enemy/sora_enemy/ai/em_ai_unit.cpp): an action the AI module can pick, described by a
// parameter block of the enemy kind. The Enemy constructor builds emInfo::getAINum of them and hands them to
// emAIModuleImpl. Each enemy class keeps its units in an array the constructor builds with __construct_array and
// emAIUnit's constructor and destructor (the module map's names for 0x2A470 and 0x2A480).

#include <StaticAssert.h>
#include <types.h>

class soModuleAccesser;

// The parameters of one AI unit (an element of the enemy's AI parameter block). HYPOTHESIS: names, from the uses.
struct emAIUnitParam {
    int m_priority;          // 0x00 compared with the update window of emAIModuleImpl::update
    u32 m_targetAreaMask;    // 0x04 the areas (bit n: area n, bit 8: the current target) the unit reacts to
    void* m_decideAnimCmd;   // 0x08 the anim cmds run to decide whether the unit is taken (null: always taken)
    void* m_execAnimCmd;     // 0x0C the anim cmds run when the unit is taken
    int m_statusKind;        // 0x10 the status the unit requests (-1: none)
    u8 m_initFlag;           // 0x14 the initial value of emAIUnit::m_flag
    bool m_setInfo;          // 0x15 the unit records itself as the current AI info
};

class emAIUnit {
public:
    emAIUnitParam* m_param; // 0x0
    u8 m_flag;              // 0x4 (the unit is enabled; anim cmd 'j' 0 / 1 sets it)

    emAIUnit();
    ~emAIUnit();
    void create(emAIUnitParam* param);
    void reset();
    void resetFlag();
    void setFlag(int flag);
    int getFlag(); // (callers test the whole register: the byte is returned as an int)
    // Runs the decide anim cmds of the unit for area areaIndex; true: the unit is taken.
    bool execAnimCmdDecide(soModuleAccesser* acc, int animCmdGroup, int areaIndex);
    void execAnimCmdExec(soModuleAccesser* acc, int animCmdGroup);
    int getPriorty();
    int getStatusKind();
    // MATCH-ONLY: never_inline, isTargetKind1 tail-calls it in the original.
    static bool isTargetKind(u32 mask, int area) __attribute__((never_inline));
    bool isTargetKind1(int area);
    bool isSetInfo();
};
static_assert(sizeof(emAIUnit) == 8, "Class is wrong size!");
