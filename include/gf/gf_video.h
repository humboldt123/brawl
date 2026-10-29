#pragma once

#include <StaticAssert.h>
#include <revolution/GX/GXTypes.h>
#include <revolution/GX/GXFrameBuf.h>
#include <types.h>

class gfVideo;
// HYPOTHESIS: the current video instance, set by the constructor and cleared by the destructor.
extern gfVideo* lbl_805A0098;

class gfVideo {
public:
    GXRenderModeObj m_renderMode; // 0x0
    u32 m_retraceCount;           // 0x3C
    u16 m_fbWidth;                // 0x40
    u16 m_xfbHeight;              // 0x42
    f32 m_aspectCorrection;       // 0x44
    // HYPOTHESIS: aspect correction scaled by the frame-buffer ratio.
    f32 m_fbAspectCorrection;     // 0x48
    u8 m_deflicker : 1;           // 0x4C
    u8 m_progressive : 1;
    u8 m_wide : 1;
    u8 m_tvFormatId;              // 0x4D
    u32 m_tvFormat;               // 0x50

    gfVideo();
    ~gfVideo();
    void init();
    static void VideoPostRetraceCallback(u32 retraceCount);
    // HYPOTHESIS: parameter type from the callee's register use; the map has none.
    void setDeflicker(u32 deflicker);
};
static_assert(sizeof(gfVideo) == 0x54, "Class is the wrong size!");
