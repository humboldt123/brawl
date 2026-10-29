#pragma once

#include <StaticAssert.h>
#include <gr/gr_yakumono_punch_slider.h>
#include <types.h>

// HYPOTHESIS: the adventure punch slider whose slider follows an attached motion path (sora_adv_stage
// gr_adventure_motion_punch_slider.o); it adds no members to grYakumonoPunchSlider.
class grAdventureMotionPunchSlider : public grYakumonoPunchSlider {
public:
    grAdventureMotionPunchSlider(const char* taskName) : grYakumonoPunchSlider(taskName) { }
    virtual ~grAdventureMotionPunchSlider();
    virtual void updateMove(float deltaFrame);
    virtual void setCulcCallback(gfArchive* archive);

    static grAdventureMotionPunchSlider* create(int mdlIndex, const char* taskName);
};
static_assert(sizeof(grAdventureMotionPunchSlider) == 0x1C4, "Class is wrong size!");
