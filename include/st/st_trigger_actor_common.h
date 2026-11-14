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

// HYPOTHESIS: the second trigger is read back as two halfwords.
struct stActTriggerFrameHalves {
    u16 id;
    u8 mid;
    u8 keep;
};

// HYPOTHESIS: the record stActTriggerFrameCB is constructed with: a frame, then two triggers.
struct stActTriggerFrameCBData {
    float unk0;
    u16 unk4;
    u8 unk6;
    u8 unk7;
    union {
        stTriggerData unk8;
        stActTriggerFrameHalves parts;
    };
};

class stActTriggerFrameCB : public stTriggerCB, public stActTriggerFrameCBZero {
public:
    float unkC;
    stActTriggerFrameCBData* unk10;

    stActTriggerFrameCB(stActTriggerFrameCBData* data);
    virtual ~stActTriggerFrameCB();
    virtual void userProc();
};
static_assert(sizeof(stActTriggerFrameCB) == 0x14, "Class is wrong size!");
