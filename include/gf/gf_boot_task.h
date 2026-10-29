#pragma once

#include <StaticAssert.h>
#include <gf/gf_archive.h>
#include <gf/gf_file_io_handle.h>
#include <gf/gf_task.h>
#include <types.h>

// HYPOTHESIS: the five resident item archives created with the boot task.
struct gfBootItemArchive {
    gfArchive m_archive0;
    gfArchive m_archive1;
    gfArchive m_archive2;
    gfArchive m_archive3;
    gfArchive m_archive4;
    u8 unk280[0x8];
};

gfBootItemArchive* globalGetItemArchive();

class gfBootTask : public gfTask {
public:
    // HYPOTHESIS: a pair of file handles with two cleared 8-bit fields.
    struct HandlePair {
        u32 unk0 : 8;
        gfFileIOHandle m_handle4;
        gfFileIOHandle m_handle8;
        u32 unkC : 8;
        u32 unk10;

        HandlePair() : unk0(0), unkC(0), unk10(0) { }
    };

    u8 unk40_b7 : 1;
    u8 unk40_b6 : 1;
    u32 unk40_x : 8;
    u8 unk42;
    gfFileIOHandle m_handle44;
    gfFileIOHandle m_handle48;
    u32 unk4C;
    u32 unk50;
    gfArchive* unk54;
    u8 unk58[0x4];
    u32 unk5C;
    u32 unk60;
    HandlePair unk64;
    u8 unk78[0x4];
    s32 unk7C;
    s32 unk80;
    void* unk84;
    u8 unk88[0x8];

    // HYPOTHESIS: parameter type from the callee's register use; the map has none.
    static gfBootTask* create(bool flag);
    gfBootTask(bool flag);
    virtual void processDefault();
    virtual ~gfBootTask();
};
static_assert(sizeof(gfBootTask) == 0x90, "Class is the wrong size!");
