#include <gf/gf_display.h>
#include <gf/gf_draw.h>
#include <revolution/GX.h>
#include <revolution/OS.h>
#include <revolution/VI.h>
#include <types.h>

// Not declared by the OpenRVL GX headers.
typedef void (*GXDrawSyncCallback)(u16 token);
extern "C" {
f32 GXGetYScaleFactor(u16 efbHeight, u16 xfbHeight);
GXDrawSyncCallback GXSetDrawSyncCallback(GXDrawSyncCallback callback);
void GXSetDispCopyFrame2Field(u32 mode);
u32 GXSetDispCopyYScale(f32 yScale);
void GXSetDispCopySrc(u16 left, u16 top, u16 width, u16 height);
void GXSetDispCopyDst(u16 width, u16 height);
void GXSetDispCopyGamma(u32 gamma);
void GXSetDrawSync(u16 token);
}
#define GX_COPY_PROGRESSIVE 0

// HYPOTHESIS: the render mode the display was set up with, owned by another unit.
extern GXRenderModeObj* lbl_805A0098;

bool gfFrameBuffer::m_isRenderStarted;
gfFrameBuffer* gfFrameBuffer::m_instance;

void* gfXFBManager::getNextRenderableXFB() {
    if (m_renderIndex == INDEX_NONE) {
        m_renderIndex = 0;
    }
    return m_buffers[m_renderIndex].m_buffer;
}

void* gfXFBManager::getCurrentDispBuffer() {
    for (int i = 0; i < m_numBuffers; i++) {
        if (m_buffers[i].m_state == STATE_DISPLAYED) {
            return m_buffers[i].m_buffer;
        }
    }
    return m_buffers[0].m_buffer;
}

// Carves numBuffers frame buffers of width x height (rows padded to 16 pixels, 2 bytes per pixel) out of base.
void gfXFBManager::setBuffer(u8 numBuffers, u16 width, u16 height, void* base) {
    m_numBuffers = numBuffers;
    m_height = height;
    m_width = width;
    u32 size = (((width + 15) & 0xFFF0) * height) << 1;
    u8* buffer = static_cast<u8*>(base);
    for (u32 i = 0; i < numBuffers; i++) {
        m_buffers[i].m_state = STATE_FREE;
        m_buffers[i].m_buffer = buffer;
        buffer += size;
    }
}

gfFrameBuffer::gfFrameBuffer() {
    m_dispIndex = INDEX_NONE;
    m_drawnIndex = INDEX_UNSET;
    m_renderIndex = INDEX_NONE;
    m_isBlackCleared = 0;
    m_frameState = 0;
    m_efbBuffer1 = NULL;
    m_efbBuffer2 = NULL;
    m_drawDuration = 0;
    m_drawEndTime = 0;
    m_drawStartTime = 0;
    m_waitDuration = 0;
    m_waitEndTime = 0;
    m_waitStartTime = 0;
    m_drawSyncCallback = NULL;
    m_isColorUpdate = 1;
    m_isZUpdate = 1;
    m_isWaited = 0;
    m_unk74_4 = 0;
    m_isCopyEnabled = 1;
    m_instance = this;
}

gfFrameBuffer* gfFrameBuffer::getInstance() {
    return m_instance;
}

void gfFrameBuffer::setEFB(u16 width, u16 height, void* buffer1, void* buffer2) {
    m_efbBuffer1 = buffer1;
    m_efbBuffer2 = buffer2;
    m_efbWidth = width;
    m_efbHeight = height;
}

// Waits for the GP and for a free frame buffer, then copies (or clears) the EFB and starts the next frame.
void gfFrameBuffer::sync() {
    GXDrawDone();

    while (!hasBuffer(STATE_FREE)) {
        m_isWaited = 0;
        m_waitStartTime = OSGetTime();
        VIWaitForRetrace();
        m_waitEndTime = OSGetTime();
        m_waitDuration = m_waitEndTime - m_waitStartTime;
        m_isWaited = 1;
    }

    if (m_isCopyEnabled) {
        BOOL level = OSDisableInterrupts();
        GXSetDrawDoneCallback(drawDoneCallback);
        transferXFB();

        if (m_drawnIndex == INDEX_UNSET) {
            m_drawnIndex = 0;
        }
        if (m_buffers[m_renderIndex].m_state == STATE_FREE) {
            m_buffers[m_renderIndex].m_state = STATE_RENDERING;
            m_drawnIndex = m_renderIndex;
            m_renderIndex++;
            if (m_renderIndex >= m_numBuffers) {
                m_renderIndex = 0;
            }
            if (!m_isRenderStarted) {
                m_isRenderStarted = true;
            }
        }

        GXSetDrawDone();
        OSRestoreInterrupts(level);
    } else {
        clearEFB();
    }

    m_drawStartTime = OSGetTime();
    GXSetDrawSync(TOKEN_FRAME_START);
}

void gfFrameBuffer::nextFrame() {
    m_frameState = 2;
    GXSetDrawSync(TOKEN_FRAME_END);
    GXFlush();
}

// VI pre-retrace callback: shows the next ready frame buffer and frees the one that was displayed.
void gfFrameBuffer::vsyncCallback(u32 retraceCount) {
    gfFrameBuffer* self = m_instance;

    if (self->hasBuffer(STATE_READY)) {
        u8 next = 0;
        if (self->m_dispIndex != INDEX_NONE) {
            self->m_buffers[self->m_dispIndex].m_state = STATE_FREE;
            next = self->m_dispIndex + 1;
            if (next >= self->m_numBuffers) {
                next = 0;
            }
        }
        if (self->m_buffers[next].m_state == STATE_READY) {
            self->m_buffers[next].m_state = STATE_DISPLAYED;
            self->m_dispIndex = next;
            VISetNextFrameBuffer(self->m_buffers[next].m_buffer);
            if (!self->m_isBlackCleared) {
                VISetBlack(FALSE);
                self->m_isBlackCleared = 1;
            }
        }
    }

    VIFlush();
}

// GX draw-done callback: the EFB copy into the drawn buffer finished.
void gfFrameBuffer::drawDoneCallback() {
    gfFrameBuffer* self = m_instance;
    self->m_buffers[self->m_drawnIndex].m_state = STATE_READY;
    if (self->m_drawnIndex >= self->m_numBuffers) {
        self->m_drawnIndex = 0;
    }
    m_instance->m_frameState = 1;
    GXSetDrawDoneCallback(NULL);
}

// GX draw-sync callback: the frame buffer's own tokens time the frame, the others go to the user callback.
void gfFrameBuffer::drawSyncCallback(u16 token) {
    gfFrameBuffer* self = m_instance;
    if ((token & 0xF000) == 0xF000) {
        switch (token) {
        case TOKEN_FRAME_START:
            break;
        case TOKEN_FRAME_END:
            self->m_drawEndTime = OSGetTime();
            self->m_drawDuration = self->m_drawEndTime - self->m_drawStartTime;
            break;
        case TOKEN_COPY_DONE:
            break;
        }
    } else if (self->m_drawSyncCallback != NULL) {
        self->m_drawSyncCallback(token);
    }
}

void gfFrameBuffer::setup() {
    m_yScale = GXGetYScaleFactor(m_efbHeight, m_xfbHeight);
    VISetPreRetraceCallback(vsyncCallback);
    GXSetDrawSyncCallback(drawSyncCallback);
}

// Copies the EFB into the buffer to render, with the display's copy filter, and clears the EFB behind it.
void gfFrameBuffer::transferXFB() {
    GXRenderModeObj* rmode = lbl_805A0098;

    GXSetCopyFilter(FALSE, rmode->sample_pattern, TRUE, rmode->vfilter);
    GXSetDispCopyFrame2Field(GX_COPY_PROGRESSIVE);
    GXSetDispCopyYScale(m_yScale);
    GXSetDispCopySrc(0, 0, m_efbWidth, m_efbHeight);
    GXSetDispCopyDst(m_width, m_height);
    GXSetDispCopyGamma(0);
    GXSetCopyClear(m_clearColor, m_clearZ);

    if (m_isColorUpdate) {
        GXSetColorUpdate(TRUE);
    } else {
        GXSetColorUpdate(FALSE);
    }
    if (m_isZUpdate) {
        GXSetZMode(FALSE, GX_ALWAYS, TRUE);
    } else {
        GXSetZMode(FALSE, GX_ALWAYS, FALSE);
    }
    GXSetAlphaUpdate(TRUE);

    GXCopyDisp(getNextRenderableXFB(), TRUE);

    GXSetCopyFilter(FALSE, rmode->sample_pattern, FALSE, rmode->vfilter);
    GXSetDispCopyYScale(1.0f);
    GXSetDrawSync(TOKEN_COPY_DONE);
    GXFlush();
}

// Clears the EFB with a full-screen strip of the clear color (used when the copy is disabled).
void gfFrameBuffer::clearEFB() {
    if (!m_isColorUpdate && !m_isZUpdate) {
        return;
    }

    GXSetDrawSync(TOKEN_COPY_DONE);
    gfDrawSetupCoord2D();
    gfDrawSetVtxPosColorPrimEnvironment();

    if (m_isColorUpdate) {
        GXSetColorUpdate(TRUE);
    } else {
        GXSetColorUpdate(FALSE);
    }
    if (m_isZUpdate) {
        GXSetZMode(FALSE, GX_ALWAYS, TRUE);
    } else {
        GXSetZMode(FALSE, GX_ALWAYS, FALSE);
    }
    GXSetBlendMode(GX_BM_NONE, GX_BL_ZERO, GX_BL_ZERO, GX_LO_CLEAR);

    f32 width = m_efbWidth;
    f32 height = m_efbHeight;
    f32 depth = -1.0f;
    u32 color = *reinterpret_cast<u32*>(&m_clearColor);

    GXBegin(GX_TRIANGLESTRIP, GX_VTXFMT1, 4);
    GXPosition3f32(0.0f, 0.0f, depth);
    GXColor1u32(color);
    GXPosition3f32(width, 0.0f, depth);
    GXColor1u32(color);
    GXPosition3f32(0.0f, height, depth);
    GXColor1u32(color);
    GXPosition3f32(width, height, depth);
    GXColor1u32(color);
    GXEnd();

    GXFlush();
}
