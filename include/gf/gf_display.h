#pragma once

#include <StaticAssert.h>
#include <revolution/GX.h>
#include <revolution/OS.h>
#include <types.h>

// Frame buffer management (sora/gf/gf_display.cpp): the ring of external frame buffers the display shows and
// the EFB copy / draw-sync bookkeeping around each frame.
// HYPOTHESIS: member names (the map gives the function names only); the layout is from the accessors.

// The external frame buffers and which one is displayed, drawn and rendered next.
class gfXFBManager {
public:
    enum {
        STATE_FREE = 0,
        STATE_RENDERING = 1,
        STATE_READY = 2,
        STATE_DISPLAYED = 3,
        INDEX_NONE = 0xF0,
        INDEX_UNSET = 0xF1,
    };

    struct Buffer {
        int m_state;    // 0x0, STATE_*
        void* m_buffer; // 0x4
    };

    void* getNextRenderableXFB();
    void* getCurrentDispBuffer();
    void setBuffer(u8 numBuffers, u16 width, u16 height, void* base);

    u16 m_height;        // 0x0
    u16 m_width;         // 0x2
    u8 m_numBuffers;     // 0x4
    u8 m_dispIndex;      // 0x5, buffer the VI shows (INDEX_NONE before the first frame)
    u8 m_drawnIndex;     // 0x6, buffer the last EFB copy went to (INDEX_UNSET before the first frame)
    u8 m_renderIndex;    // 0x7, buffer the next EFB copy goes to (INDEX_NONE before the first frame)
    u8 m_isBlackCleared; // 0x8, VISetBlack(FALSE) done once the first frame is shown
    Buffer m_buffers[2]; // 0xC, m_numBuffers entries (setBuffer fills as many as it is told)
    u32 unk1C;           // 0x1C, not touched by this unit

    // True when one of the buffers is in the given state.
    bool hasBuffer(int state) {
        for (int i = 0; i < m_numBuffers; i++) {
            if (m_buffers[i].m_state == state) {
                return true;
            }
        }
        return false;
    }
};
static_assert(sizeof(gfXFBManager) == 0x20, "Class is the wrong size!");

class gfFrameBuffer : public gfXFBManager {
public:
    typedef void (*DrawSyncCallback)(u16 token);

    enum {
        TOKEN_FRAME_START = 0xF000,
        TOKEN_FRAME_END = 0xFFE0,
        TOKEN_COPY_DONE = 0xFFF0,
    };

    gfFrameBuffer();

    static gfFrameBuffer* getInstance();
    void setEFB(u16 width, u16 height, void* buffer1, void* buffer2);
    void sync();
    void nextFrame();
    static void vsyncCallback(u32 retraceCount);
    static void drawDoneCallback();
    static void drawSyncCallback(u16 token);
    void setup();
    void transferXFB();
    void clearEFB();

    int m_frameState;                  // 0x20, 1 = draw done, 2 = next frame requested
    void* m_efbBuffer1;                // 0x24
    void* m_efbBuffer2;                // 0x28
    GXColor m_clearColor;              // 0x2C
    u32 m_clearZ;                      // 0x30
    u16 m_efbWidth;                    // 0x34
    u16 m_efbHeight;                   // 0x36
    u16 m_xfbHeight;                   // 0x38
    f32 m_yScale;                      // 0x3C
    s64 m_drawStartTime;            // 0x40
    s64 m_drawEndTime;              // 0x48
    s64 m_drawDuration;             // 0x50
    s64 m_waitStartTime;            // 0x58
    s64 m_waitEndTime;              // 0x60
    s64 m_waitDuration;             // 0x68
    DrawSyncCallback m_drawSyncCallback; // 0x70, for tokens that are not the frame buffer's own
    u8 m_isColorUpdate : 1;            // 0x74
    u8 m_isZUpdate : 1;
    u8 m_isWaited : 1;                 // a retrace was waited for in the last sync
    u8 m_unk74_4 : 1;
    u8 m_isCopyEnabled : 1;            // sync copies the EFB (otherwise it only clears it)
    u8 m_unk74_0 : 3;

    static gfFrameBuffer* m_instance;
    static bool m_isRenderStarted; // set once a buffer has been handed out for rendering
};
static_assert(sizeof(gfFrameBuffer) == 0x78, "Class is the wrong size!");
