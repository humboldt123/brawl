#pragma once

#include <types.h>

// HYPOTHESIS: an eight-byte tag object of two words. The original builds two of them as file-local statics, (0xff, 0) and
// (0xff, 1), in the Special N units of Kirby that include the shared item headers (the same tags as the R.O.B. ones in
// ft_robot_unk8.h); nothing reads them. The constructor is the out-of-line function at ft_kirby 0x16144 (Special N unit).
struct ftKirbyUnk8 {
    u32 unk0;
    u32 unk4;
    ftKirbyUnk8(u32 a, u32 b);
};
