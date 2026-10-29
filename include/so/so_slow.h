#pragma once

#include <StaticAssert.h>
#include <gf/gf_task.h>
#include <sr/sr_common.h>
#include <types.h>

// The global slow-motion / hit-stop task ("Slow"): each frame it samples the game's slow rate.
class soSlow : public gfTask {
    int m_kind;
    float m_rate;
    u8 m_isOn;
public:
    soSlow();
    virtual void processBegin();
    virtual ~soSlow();
    // HYPOTHESIS: true while this frame's state is the one that should be estimated (the update pass), and while it
    // should be adjusted (the position fix pass); both are false during a hit pause.
    bool isEstimate();
    bool isAdjust();
    // HYPOTHESIS: names of the fields StageObject::processUpdate reads: the kind of global slow motion (0..2, how the
    // motion frames are advanced), the rate to advance them by, and whether the slow motion is currently on.
    int getKind() const { return m_kind; }
    float getRate() const { return m_rate; }
    u8 isOn() const { return m_isOn; }

#ifdef SO_SLOW_GET_INSTANCE_OUT_OF_LINE
    // The fighter RELs keep one out-of-line copy of this accessor instead of expanding it at each call.
    static soSlow* getInstance() __attribute__((never_inline)) {
#else
    static soSlow* getInstance() {
#endif
        if (ms_instance == NULL) {
            ms_instance = new (Heaps::System) soSlow();
        }
        return ms_instance;
    }
    static soSlow* ms_instance;
};
static_assert(sizeof(soSlow) == 0x4C, "Class is wrong size!");
