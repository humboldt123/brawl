// Local BrawlHeaders shadow: expose the video state at 0x0 and the replay timing counter at 0x100.
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
    u8 unkE8[0x5];
    u8 unkED; // 0xED HYPOTHESIS: bit 0x2 requests a frame-buffer capture (IfSnapSaveTask)
    u8 unkEE[0xA];
    u16 m_frameRate;
    u8 unkFA[6];
    u32 unk100; // replay recording observes changes in this application counter
    u8 unk104[0x8];
    void* unk10C; // 0x10C HYPOTHESIS: the capture destination (IfSnapSaveTask)
    u8 unk110[0x4];
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

    inline static gfApplication* getInstance() { return g_gfApplication; }
};
static_assert(sizeof(gfApplication) == 0x170, "Class is wrong size!");


static_assert(offsetof(gfApplication, unk100) == 0x100, "Replay timing counter offset");
