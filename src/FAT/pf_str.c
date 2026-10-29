#include <types.h>
#include <FAT/pf2.h>


// only the small leaf functions are drafted so far; the rest of the unit
// (move/init/len/cmp/toupper) is still asm-only.

void VFiPFSTR_SetCodeMode(PF_STR* p_str, u32 code_mode) {
    p_str->code_mode = code_mode;
}

u32 VFiPFSTR_GetCodeMode(PF_STR* p_str) {
    return p_str->code_mode;
}

void VFiPFSTR_SetLocalStr(PF_STR* p_str, s8* p_local) {
    // HYPOTHESIS: mode 1 (probably the plain/ascii-ish one) ignores the caller's
    // buffer and falls back to the head pointer
    if (p_str->code_mode == 1 || p_local == 0) {
        p_str->p_local = p_str->p_head;
    } else {
        p_str->p_local = p_local;
    }
}

const s8* VFiPFSTR_GetStrPos(PF_STR* p_str, u32 target) {
    if (target == 1) {
        return p_str->p_head;
    }
    if (target == 2) {
        return p_str->p_tail;
    }
    return p_str->p_local;
}

// Function bodies below are derived from https://github.com/jj-link/brawl.

u16 fn_803D68B8(PF_STR* p_str) {
    return p_str->p_tail - p_str->p_head;
}

s32 PFSTR_InitStr(PF_STR* p_str, const char* s, u32 code_mode) {
    if (code_mode == 1) {
        p_str->p_head = (const s8*)s;
        p_str->p_tail = (const s8*)s + VFipf_strlen(s);
    } else if (code_mode == 2) {
        p_str->p_head = (const s8*)s;
        p_str->p_tail = (const s8*)s + VFipf_w_strlen((const u16*)s) * 2;
    }
    p_str->code_mode = code_mode;
    return 0;
}
