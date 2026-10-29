#pragma once

// SHADOW of BrawlHeaders gf/gf_gameframe_counter.h (-I include comes first): the Enemy unit (EM_ENEMY_BUILDER) builds
// its frame counters with an inline constructor in the reset state.

#include <StaticAssert.h>
#include <types.h>

class gfGameFrameCounter {
    u32 m_startFrame;
    u32 m_latestStopFrame;
    u32 m_stoppedFrames;
    static const u32 ResetState = 0xFFFFFFFF;

public:
#if defined(EM_ENEMY_BUILDER) || defined(GF_GAMEFRAME_COUNTER_INLINE_CTOR)
    gfGameFrameCounter() : m_startFrame(ResetState), m_latestStopFrame(0), m_stoppedFrames(0) { }
#endif
    void reset();
    void start();
    void stop();
    u32 getCounter() const;
};
static_assert(sizeof(gfGameFrameCounter) == 0xC, "Class is the wrong size!");
