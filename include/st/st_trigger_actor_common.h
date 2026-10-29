#pragma once

#include <StaticAssert.h>
#include <gf/gf_area.h>
#include <gr/gr_gimmick.h>
#include <gr/gr_gimmick_motion_path.h>
#include <gr/gr_adventure_motion_ground.h>
#include <gr/ground.h>
#include <st/st_trigger.h>
#include <types.h>

class stActTriggerMotionPathMoveStart : public stTriggerCB {
public:
    u32 unk8;
    grGimmickMotionPath* unkC;
    float unk10;

    virtual void userProc();
};

class stActTriggerMotionPathMoveStop : public stTriggerCB {
public:
    u32 unk8;
    grGimmickMotionPath* unkC;

    virtual void userProc();
};

class stActTriggerGroundValid : public stTriggerCB {
public:
    u32 unk8;
    bool unkC;
    char _spacerD[3];
    Ground* unk10;

    virtual void userProc();
};

class stActTriggerGfAreaValid : public stTriggerCB {
public:
    u32 unk8;
    bool unkC;
    char _spacerD[3];
    gfArea* unk10;

    stActTriggerGfAreaValid(bool isValid, gfArea* area) : unk8(0), unkC(isValid), unk10(area) { }
    virtual void userProc();
};
static_assert(sizeof(stActTriggerGfAreaValid) == 0x14, "Class is wrong size!");
