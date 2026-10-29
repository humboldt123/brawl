#include <types.h>
#include <FAT/pf2.h>

// Function bodies below are derived from https://github.com/jj-link/brawl.

extern void fn_803EBBE0(s32 *, s32 *);

u8 PFENT_getcurrentDateTimeForEnt(s16 *arg0, s16 *arg1) {
    s32 date[3];
    s32 time[4];

    fn_803EBBE0(date, time);
    *arg0 = (s16)((date[2] & 0x1F) | ((date[1] << 5) & 0x1E0) | (((date[0] - 0x7BC) << 9) & 0xFE00));
    *arg1 = (s16)((((u32)time[2] >> 1) & 0x1F) | ((time[1] << 5) & 0x7E0) | ((time[0] << 11) & 0xF800));
    return (u8)time[3];
}


int PFENT_ITER_GetEntryOfPath(PF2_ENT_ITER* iter, PF2_DIR_ENT* entry, PF2_VOLUME* vol, PF2_STR* str, u32 no_look_last_token) {
    return fn_803DE800(iter, entry, vol, str, 0, no_look_last_token);
}

int fn_803DD64C(PF_FFD* p_ffd, PF2_DIR_ENT* p_ent, u32 index, PF2_STR* pattern, u8 attr_required, u8 attr_unwanted) {
    u32 lpos;
    u32 ppos;

    return fn_803DD390(p_ffd, p_ent, index, pattern, attr_required, attr_unwanted, &lpos, &ppos);
}

int fn_803DD944(PF2_DIR_ENT* p_ent, u8 num_entries, PF_FFD* p_ffd, u32* p_next_chain, PF2_STR* p_filename) {
    u32 pos;

    return fn_803DD674(p_ent, num_entries, p_ffd, p_next_chain, p_filename, &pos);
}

int fn_803DF754(PF2_ENT_ITER* iter, PF2_DIR_ENT* ent, PF2_VOLUME* vol, PF2_STR* path) {
    return fn_803DE800(iter, ent, vol, path, 1, 1);
}
