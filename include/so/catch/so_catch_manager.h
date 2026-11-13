#pragma once

#include <StaticAssert.h>
#include <gf/gf_task.h>
#include <nw4r/ut/ut_list.h>
#include <types.h>

class soCatchModuleImpl;

// HYPOTHESIS: the record a catch module puts in the manager's list (it sits at 0x50 of soCatchModuleImpl, whose
// constructor points m_module at the module and whose initInfo resets m_order to the largest float).
struct soCatchCheckNode {
    soCatchModuleImpl* m_module; // 0x00
    float m_order;               // 0x04 smaller values are checked first
    nw4r::ut::Link m_link;       // 0x08
};

// The task that runs the catch checks of all catch modules once a frame, in the order of their m_order values (ties
// are broken at random). The list is rebuilt every frame: processBegin empties it, the modules add themselves.
class soCatchManager : public gfTask {
public:
    nw4r::ut::List m_list; // 0x40

    static soCatchManager* m_instance;

    soCatchManager();
    virtual void processBegin();
    virtual void processCatch();
    virtual ~soCatchManager();
    void addCheckList(soCatchCheckNode* node);
};
static_assert(sizeof(soCatchManager) == 0x4C, "Class is wrong size!");
