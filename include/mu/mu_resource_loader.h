#pragma once

#include <StaticAssert.h>
#include <gf/gf_file_io_handle.h>
#include <types.h>

// A resource loader of the scene module that reads a pack of files (it starts with a file handle).
class scResourceLoader : public gfFileIOHandle {
public:
    u32 m_unk04;
    u32 m_unk08;
    u32 m_unk0C;
    u32 m_unk10;
    u32 m_unk14;
    u32 m_unk18;
    u8 m_flagA : 3;                     // 0x1C
    u8 m_flagB : 5;
    u8 m_unk1D[3];
    scResourceLoader() {
        m_unk04 = 0;
        m_unk08 = 0;
        m_unk0C = 0;
        m_unk10 = 0;
        m_unk18 = 0;
        m_unk14 = 0xFF;
        m_flagA = 0;
    }
    ~scResourceLoader() { }
};
