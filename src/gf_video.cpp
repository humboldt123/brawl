#include <gf/gf_video.h>
#include <revolution/GX.h>
#include <revolution/OS/OSError.h>
#include <revolution/OS/OSHardware.h>
#include <revolution/OS/OSTime.h>
#include <revolution/SC/scapi.h>
#include <revolution/SC/scsystem.h>
#include <revolution/VI/vi.h>
#include <types.h>

extern "C" {
u16 GXGetNumXfbLines(u16 efbHeight, f32 yScale);
f32 GXGetYScaleFactor(u16 efbHeight, u16 xfbHeight);
void GXAdjustForOverscan(const GXRenderModeObj* rmin, GXRenderModeObj* rmout, u16 hor, u16 ver);
u32 VIGetDTVStatus(void);
// HYPOTHESIS: render modes of the GX unit that are not named yet.
extern GXRenderModeObj GXNtsc480Int;
extern GXRenderModeObj lbl_80475FA0;
extern GXRenderModeObj lbl_80475FDC;
extern GXRenderModeObj lbl_80476144;
extern GXRenderModeObj lbl_80476054;
extern GXRenderModeObj lbl_80476090;
extern GXRenderModeObj lbl_804760CC;
extern GXRenderModeObj lbl_804761BC;
extern GXRenderModeObj lbl_804761F8;
extern GXRenderModeObj lbl_80476234;
}

namespace nw4r {
namespace g3d {
namespace G3DState {
void SetRenderModeObj(const GXRenderModeObj& rObj);
}
}
}

struct gfVideoWidthMode {
    u16 viWidth;
    f32 aspectCorrection;
};

static GXRenderModeObj s_ntsc480Prog = {
    2,
    640,
    480,
    480,
    40,
    0,
    640,
    480,
    VI_XFB_MODE_SF,
    FALSE,
    FALSE,
    {
        {6, 6}, {6, 6}, {6, 6},
        {6, 6}, {6, 6}, {6, 6},
        {6, 6}, {6, 6}, {6, 6},
        {6, 6}, {6, 6}, {6, 6},
    },
    {8, 8, 10, 12, 10, 8, 8},
};

static GXRenderModeObj* s_renderModes[6][4] = {
    {&GXNtsc480Int, &GXNtsc480IntDf, &lbl_80475FA0, &lbl_80475FDC},
    {&lbl_80476144, &GXPal528IntDf, &lbl_80476144, &GXPal528IntDf},
    {&lbl_80476054, &GXMpal480IntDf, &lbl_80476090, &lbl_804760CC},
    {&GXNtsc480Int, &GXNtsc480IntDf, &lbl_80475FA0, &lbl_80475FDC},
    {&lbl_80476144, &GXPal528IntDf, &lbl_80476144, &GXPal528IntDf},
    {&lbl_804761BC, &GXEurgb60Hz480IntDf, &lbl_804761F8, &lbl_80476234},
};

static gfVideoWidthMode s_widthModes[6][2] = {
    {{670, 0.95170456f}, {686, 0.9744318f}},
    {{660, 0.94602275f}, {682, 0.96875f}},
    {{660, 0.95170456f}, {686, 0.9744318f}},
    {{670, 0.95170456f}, {686, 0.9744318f}},
    {{660, 0.94602275f}, {686, 0.96875f}},
    {{660, 0.95170456f}, {686, 0.9744318f}},
};

gfVideo::gfVideo() {
    m_retraceCount = 0;
    m_aspectCorrection = 1.0f;
    m_wide = false;
    lbl_805A0098 = this;
}

gfVideo::~gfVideo() {
    lbl_805A0098 = NULL;
}

void gfVideo::init() {
    m_aspectCorrection = 1.0f;
    m_deflicker = true;
    m_progressive = false;
    bool progressive = false;
    while (SCCheckStatus() == SC_STATUS_BUSY) {
        OSSleepTicks(OS_MSEC_TO_TICKS(1));
    }
    if (SCGetAspectRatio() == SC_ASPECT_WIDE) {
        m_wide = true;
    }
    if (SCGetProgressiveMode() == 1 && VIGetDTVStatus()) {
        progressive = true;
        VIGetScanMode();
    }
    m_tvFormat = VIGetTvFormat();
    m_tvFormatId = m_tvFormat;
    u16 wide = 0;
    if (m_wide) {
        wide = 1;
    }
    u16 viWidth = s_widthModes[m_tvFormat][wide].viWidth;
    m_aspectCorrection = s_widthModes[m_tvFormat][wide].aspectCorrection;
    GXRenderModeObj* renderMode;
    switch (m_tvFormat) {
    case VI_TV_FMT_NTSC:
        if (progressive) {
            m_progressive = true;
            renderMode = &s_ntsc480Prog;
        } else {
            renderMode = &GXNtsc480IntDf;
        }
        break;
    case VI_TV_FMT_PAL:
        renderMode = &GXPal528IntDf;
        break;
    case VI_TV_FMT_EURGB60:
        if (progressive) {
            renderMode = &GXEurgb60Hz480IntDf;
        } else {
            renderMode = &lbl_804761F8;
        }
        break;
    case VI_TV_FMT_MPAL:
        if (progressive) {
            renderMode = &lbl_80476090;
        } else {
            renderMode = &GXMpal480IntDf;
        }
        break;
    default:
        OSPanic("gf_video.cpp", 176, "VideoInit: invalid TV format\n");
        break;
    }
    GXAdjustForOverscan(renderMode, &m_renderMode, 0, 0);
    m_renderMode.viWidth = viWidth;
    m_renderMode.viXOrigin = (720 - viWidth) / 2;
    if (m_tvFormat == VI_TV_FMT_PAL) {
        m_renderMode.xfbHeight = GXGetNumXfbLines(480, GXGetYScaleFactor(480, 542));
        m_renderMode.viYOrigin = (576 - m_renderMode.xfbHeight) / 2;
    }
    m_fbWidth = m_renderMode.fbWidth;
    m_xfbHeight = m_renderMode.xfbHeight;
    m_fbAspectCorrection = m_aspectCorrection * (m_fbWidth / m_xfbHeight);
    VISetPostRetraceCallback(VideoPostRetraceCallback);
    nw4r::g3d::G3DState::SetRenderModeObj(*renderMode);
}

extern "C" void fn_800384CC(gfVideo* video, u32 wide) {
    if (video->m_wide != wide) {
        video->m_wide = wide;
        u16 index = 0;
        if (video->m_wide) {
            index = 1;
        }
        u16 viWidth = s_widthModes[video->m_tvFormat][index].viWidth;
        video->m_aspectCorrection = s_widthModes[video->m_tvFormat][index].aspectCorrection;
        video->m_renderMode.viWidth = viWidth;
        video->m_renderMode.viXOrigin = (720 - viWidth) / 2;
        VIConfigure(&video->m_renderMode);
        VIFlush();
    }
}

void gfVideo::VideoPostRetraceCallback(u32 retraceCount) {
    lbl_805A0098->m_retraceCount++;
}

void gfVideo::setDeflicker(u32 deflicker) {
    if (m_deflicker != deflicker) {
        m_deflicker = deflicker;
        int index;
        if (!m_progressive) {
            if (deflicker) {
                index = 1;
            } else {
                index = 0;
            }
        } else {
            index = (deflicker != 0) + 2;
        }
        // MATCH-ONLY: the mode index is narrowed to u16 at the lookup.
        GXRenderModeObj* renderMode = s_renderModes[m_tvFormatId][(u16)index];
        GXAdjustForOverscan(renderMode, &m_renderMode, 0, 0);
        u16 wide = 0;
        if (m_wide) {
            wide = 1;
        }
        u16 viWidth = s_widthModes[m_tvFormat][wide].viWidth;
        m_aspectCorrection = s_widthModes[m_tvFormat][wide].aspectCorrection;
        m_renderMode.viWidth = viWidth;
        m_renderMode.viXOrigin = (720 - viWidth) / 2;
        nw4r::g3d::G3DState::SetRenderModeObj(*renderMode);
        VIConfigure(&m_renderMode);
        VIFlush();
    }
}
