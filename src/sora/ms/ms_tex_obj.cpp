#include <ms/ms_tex_obj.h>
#include <revolution/OS/OSCache.h>
#include <types.h>

msTexObj::msTexObj() {
    m_buffer = NULL;
    m_bufferSize = 0;
    m_srcY = 0;
    m_srcX = 0;
    m_width = 0;
    m_height = 0;
    m_format = (GXTexFmt)0;
    m_mipmap = 1;
    m_clear = 0;
}

msTexObj::~msTexObj() {
    gfHeapManager::free(m_buffer);
}

bool msTexObj::copyTexObjFromEFB(f32 x0, f32 y0, f32 x1, f32 y1, GXTexFmt format, Heaps::HeapType heapType, bool clear) {
    if (m_buffer != NULL) {
        gfHeapManager::free(m_buffer);
        m_buffer = NULL;
        m_bufferSize = 0;
        m_height = 0;
        m_width = 0;
        m_srcX = 0;
        m_srcY = 0;
    }

    m_mipmap = 0;
    m_format = format;
    m_srcY = y0;
    m_srcX = x0;
    m_width = x1 - x0;
    m_height = y1 - y0;
    m_clear = clear;

    u32 size = GXGetTexBufferSize(m_width, m_height, format, 0, 0);
    m_buffer = gfHeapManager::alloc(heapType, size, 32);
    m_bufferSize = size;
    DCInvalidateRange(m_buffer, size);

    u16 width = m_width;
    u16 height = m_height;
    if (m_mipmap) {
        width = width >> 1;
        height = height >> 1;
    }

    GXSetTexCopySrc(m_srcX, m_srcY, m_width, m_height);
    GXSetTexCopyDst(width, height, m_format, m_mipmap);
    GXCopyTex(m_buffer, m_clear);

    GXInitTexObj(&m_texObj, m_buffer, width, height, m_format, GX_CLAMP, GX_CLAMP, 0);
    GXInitTexObjLOD(&m_texObj, GX_LINEAR, GX_LINEAR, 0.0f, 0.0f, 0.0f, 0, 0, GX_ANISO_1);
    return true;
}

void msTexObj::freeTexObj() {
    gfHeapManager::free(m_buffer);
    m_buffer = NULL;
    m_bufferSize = 0;
    m_height = 0;
    m_width = 0;
    m_srcX = 0;
    m_srcY = 0;
}
