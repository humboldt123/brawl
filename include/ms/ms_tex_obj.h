#pragma once

#include <StaticAssert.h>
#include <gf/gf_heap_manager.h>
#include <revolution/GX.h>
#include <types.h>

// A texture object whose image is copied out of the embedded frame buffer into a heap allocation
// (sora/ms/ms_tex_obj.cpp). Used by the message/menu code for screenshots of the frame (player tags, ...).
class msTexObj {
public:
    msTexObj();
    virtual ~msTexObj();

    // Copies the frame buffer rectangle (x0, y0)-(x1, y1) into a new buffer of the given format and heap and
    // initializes m_texObj with it (linear filtering, no mipmaps). Always returns true.
    bool copyTexObjFromEFB(f32 x0, f32 y0, f32 x1, f32 y1, GXTexFmt format, Heaps::HeapType heapType, bool clear);
    void freeTexObj();

    GXTexObj m_texObj;   // 0x4
    void* m_buffer;      // 0x24
    u32 m_bufferSize;    // 0x28
    int m_srcY;          // 0x2C, frame buffer copy origin (y before x in memory)
    int m_srcX;          // 0x30
    int m_width;         // 0x34
    int m_height;        // 0x38
    GXTexFmt m_format;   // 0x3C
    u8 m_mipmap;         // 0x40, copy destination mipmap flag (halves the destination size)
    u8 m_clear;          // 0x41, clear the frame buffer after the copy
};
static_assert(sizeof(msTexObj) == 0x44, "Class is the wrong size!");
