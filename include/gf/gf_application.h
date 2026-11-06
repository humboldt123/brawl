// Local BrawlHeaders shadow: expose the video state at 0x0, the replay timing counter at 0x100, and the pause and low-effect skip state.
#pragma once

#include <StaticAssert.h>
#include <stddef.h>
#include <gf/gf_callback.h>
#include <gf/gf_keep_fb.h>
#include <gf/gf_video.h>
#include <types.h>

struct vcBootParam {
    u8 unk0[0x54];
    void init(int argc, const char* argv[]);
};
// TODO size assertion

class gfApplication;
extern gfApplication* g_gfApplication;
class gfApplication {
public:
    gfVideo m_video;
    u8 unk54[0x7C];
    gfKeepFrameBuffer unkD0;
    u8 unkD1[0x13];
    int m_e4;
    union {
        struct {
            u8 unkE8[0x5];
            u8 unkED; // 0xED HYPOTHESIS: bit 0x2 requests a frame-buffer capture (IfSnapSaveTask)
            u8 unkEE[0xA];
        };
        struct {
            u8 _padE8[0x4];
            u32 m_pauseState : 5; // HYPOTHESIS: the pause bits above this field's neighbor; IfCoinShooter::update tests it for non-zero
            // HYPOTHESIS: one requested-pause bit per index.
            u32 m_pauseFlags : 5;
            u32 : 22;
        };
        struct {
            u8 _padED[0x5];
            u8 : 5;
            // HYPOTHESIS: low-effect skipping is allowed.
            u8 m_lowEffectEnabled : 1;
            u8 : 2;
            u8 : 7;
            // HYPOTHESIS: the low-effect skip state published for this frame.
            u8 m_skipLowEffect : 1;
            // HYPOTHESIS: the low-effect skip state being tracked.
            u8 m_lowEffectActive : 1;
        };
    };
    u16 m_frameRate;
    u8 unkFA[6];
    u32 unk100; // replay recording observes changes in this application counter
    union {
        u8 unk104[0x8];
        // HYPOTHESIS: frames spent above or below the low-effect threshold.
        s32 m_skipCounter;
    };
    void* unk10C; // 0x10C HYPOTHESIS: the capture destination (IfSnapSaveTask)
    union {
        u8 unk110[0x4];
        // HYPOTHESIS: byte tested by sqSingleSimple::start.
        u8 unk110Value;
    };
    gfCallBackList m_114;
    gfCallBackList unk118;
    vcBootParam m_bootParam;

    gfApplication();
    ~gfApplication();
    void init();
    void reset();
    void restart();
    void mainLoop();
    void exit();
    void setPause(int value, int index);
    void clearPause();
    void setSkipLowEffect(u8 level);

    inline static gfApplication* getInstance() { return g_gfApplication; }
};
static_assert(sizeof(gfApplication) == 0x170, "Class is wrong size!");


static_assert(offsetof(gfApplication, unk100) == 0x100, "Replay timing counter offset");

void VC_launch(const char* title, int duration);
