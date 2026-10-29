#pragma once

#include <StaticAssert.h>
#include <st/st_trigger.h>
#include <st/st_trigger_observe.h>
#include <types.h>

class grAdventureNotifyGimmickTriggerStatus;

// The trigger callback that tells a gimmick about the edges of its triggers (stActTriggerGimmickStatus is defined in
// another unit of the module; only the layout its constructor fills is declared here).
class stActTriggerGimmickStatus : public stTriggerCB {
    u32 unk8;                                       // 0x8, always 0
    u8 unkC;                                        // 0xC, always 1
    int m_index;                                    // 0x10, which trigger of the owner (0: off edge, 1: on edge)
    u8 m_isValid;                                   // 0x14, stTriggerData::m_isValid of the trigger
    int m_flag;                                     // 0x18, passed through to the owner's notify event
    grAdventureNotifyGimmickTriggerStatus* m_owner; // 0x1C

public:
    stActTriggerGimmickStatus(grAdventureNotifyGimmickTriggerStatus* owner, int index, bool isValid, int flag);
    virtual void userProc();
};
static_assert(sizeof(stActTriggerGimmickStatus) == 0x20, "Class is wrong size!");

// Mix-in base of the adventure gimmicks that react to a trigger: a status word (set by the trigger callbacks) and the
// two edge callbacks created for the trigger.
class grAdventureNotifyGimmickTriggerStatus {
public:
    grAdventureNotifyGimmickTriggerStatus() : m_status(-1) {
        m_triggers[0] = NULL;
        m_triggers[1] = NULL;
    }
    virtual ~grAdventureNotifyGimmickTriggerStatus() { }
    virtual void notifyEventOnTrigger(int flag) = 0;
    virtual void notifyEventOffTrigger(int flag) = 0;
    virtual void updateStatus();

protected:
    int m_status;                              // 0x4
    stActTriggerGimmickStatus* m_triggers[2];  // 0x8

public:
    void createGimmickStatusTrigger(grAdventureNotifyGimmickTriggerStatus* owner, stTrigger* trigger, stTriggerData* data, int index, int flag);
    void setGimmickStatus(const int* status);
};
static_assert(sizeof(grAdventureNotifyGimmickTriggerStatus) == 0x10, "Class is wrong size!");
