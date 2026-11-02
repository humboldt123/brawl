#include <if/if_vs_result_task.h>
#include <types.h>

// HYPOTHESIS: untyped placeholders for the zero-initialized .bss of this unit, sizes and order from the original layout.
// The .data tables of the unit point at 36 more buffers of 0x20 bytes after this one (lbl_8049E0F0 .. lbl_8049E530,
// the node names that initMdlData formats with sprintf); the splits give this unit only the first.
unsigned char lbl_8049E0D0[0x20] __attribute__((aligned(8))) = { 0 };
