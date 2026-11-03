#include <gf/gf_scene.h>
#include <sq/sq_adventure.h>
#include <sr/sr_common.h>
#include <types.h>

sqAdventure* sqAdventure::create() {
    return new (Heaps::GlobalMode) sqAdventure("sqAdventure");
}

void srReportLessRelease(const char* format, ...) {
}

void srReportLessMastar(const char* format, ...) {
}

// HYPOTHESIS: command records contain an opcode and either an integer or a pointer.
extern const struct {
    s32 unk0;
    union {
        s32 unk4;
        const s32* unk4p;
    };
} lbl_1_rodata_1378[];

s32 sqAdventure::getStartStepId(s32 index) {
    if (index < 0 || index > 33) {
        return 0;
    }
    bool found = false;
    for (s32 i = 0; ; ++i) {
        if (found) {
            if (lbl_1_rodata_1378[i].unk0 == 0 ||
                lbl_1_rodata_1378[i].unk0 == 1 ||
                lbl_1_rodata_1378[i].unk0 == 3 ||
                lbl_1_rodata_1378[i].unk0 == 4 ||
                lbl_1_rodata_1378[i].unk0 == 27) {
                return *lbl_1_rodata_1378[i].unk4p;
            }
        } else if (lbl_1_rodata_1378[i].unk0 == 22) {
            if (index == lbl_1_rodata_1378[i].unk4 - 1) {
                found = true;
            }
        }
    }
}
