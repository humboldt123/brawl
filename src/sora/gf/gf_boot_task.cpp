#include <gf/gf_boot_task.h>
#include <types.h>

// HYPOTHESIS: boot sequence phase values, names not recovered.
s32 lbl_8059C4B0 = 0;
s32 lbl_8059C4B4 = 1;
s32 lbl_8059C4B8 = 1;
s32 lbl_8059C4BC = 1;
s32 lbl_8059C4C0 = 2;

// HYPOTHESIS: untyped placeholders for the zero-initialized .bss of this unit, sizes and order from the original layout.
unsigned char lbl_804948C0[0x18] __attribute__((aligned(8))) = { 0 };
unsigned char lbl_804948D8[0x10] __attribute__((aligned(8))) = { 0 };
gfBootItemArchive* lbl_8059FFB8;
// HYPOTHESIS: untyped placeholder for the zero-initialized .sbss of this unit, size from the original layout.
unsigned char lbl_8059FFBC[0x1] __attribute__((aligned(4))) = { 0 };

gfBootItemArchive* globalGetItemArchive() {
    return lbl_8059FFB8;
}

gfBootTask* gfBootTask::create(bool flag) {
    return new (Heaps::System) gfBootTask(flag);
}

gfBootTask::gfBootTask(bool flag)
    : gfTask("gfBootTask", Category_None, 0xE, 1, true), unk40_b7(true), unk4C(0), unk50(0), unk5C(0),
      unk7C(lbl_8059C4B0), unk80(0) {
    unk40_x = 0;
    unk60 = 0;
    unk40_b6 = flag;
    unk42 = 0;
    unk7C = lbl_8059C4B0;
    unk80 = 0;
    lbl_8059FFB8 = new (Heaps::ItemResource) gfBootItemArchive;
}

gfBootTask::~gfBootTask() { }
