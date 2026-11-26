#include <types.h>

// TODO: clean up, give variables real names
extern int lbl_805644C0[];

int fn_803EA188(void) {
    return lbl_805644C0[0x10];
}


extern char *fn_803EA100(s8);
extern int lbl_805644C0[];

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
s32 fn_803EAE34(s8 arg0, s32 *arg1) {
    char *p;
    u16 v;

    p = fn_803EA100(arg0);
    v = *(u16 *)(p + 0x1F8C);
    if (!(v & 8)) {
        lbl_805644C0[0x10] = 9;
        *(s32 *)(p + 0x1F80) = 9;
        return 9;
    }
    if (v & 2) {
        *arg1 = 1;
    } else {
        *arg1 = 0;
    }
    return 0;
}

// TODO: clean up, give variables real names
/* jj_803E96F4 */
extern void PFFILE_FinalizeAllFiles(void *);
extern void PFDIR_FinalizeAllDirs(void *);
extern int PFCACHE_FlushAllCaches(void *);
extern void fn_803DA9AC(void *);
extern int PFDRV_unmount(void *, int);
extern void PFDRV_ClearMountRequested(void *);
extern void PFDRV_ClearUnmountRequested(void *);
#define lbl_805644C0 ((char*)&lbl_805644C0)

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
static inline int finish(char *self, int flags)
{
    int status = PFDRV_unmount(self, flags);
    if (status != 0)
        return status;
    if (*(u16 *)(self + 0x1F8C) & 8) {
        *(int *)(self + 0x1654) = 0;
        *(int *)(self + 0x189C) = 0;
        *(int *)(self + 0x1AE4) = 0;
        *(int *)(self + 0x1D2C) = 0;
    }
    *(u16 *)(self + 0x1F8C) &= ~8;
    return 0;
}
// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
int fn_803E96F4(char *self, int flags)
{
    int result = 0;
    int status;
    if (!(*(u16 *)(self + 0x1F8C) & 8))
        return 9;
    PFFILE_FinalizeAllFiles(self);
    PFDIR_FinalizeAllDirs(self);
    status = PFCACHE_FlushAllCaches(self);
    if (status != 0)
        result = status;
    if (status == 0 || (flags & 1)) {
        fn_803DA9AC(self);
        status = finish(self, flags);
        if (status != 0 && result == 0)
            result = status;
    }
    if (result == 0 || (flags & 1)) {
        PFDRV_ClearMountRequested(self);
        PFDRV_ClearUnmountRequested(self);
        --*(int *)(lbl_805644C0 + 0x34);
    }
    return result;
}
#undef lbl_805644C0

/* jj_803E94E8 */
extern int fn_803D6E5C(void);
extern int fn_803D7168(char *, s32);
extern int PFCACHE_InitCaches(char *);
extern int PFENT_GetRootDir(char *, char *);
extern int PFENT_MakeRootDir(char *);
extern int PFFAT_InitFATRegion(char *);
extern void fn_803EBBDC(s32 *);
#define lbl_805644C0 ((char*)&lbl_805644C0)

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
static inline int find_id(int id)
{
    unsigned int i;
    char *entry = lbl_805644C0 + 8;
    for (i = 1; i < 4; i++) {
        if ((*(unsigned int *)(entry + 0x80) &= 1) &&
            id == *(int *)(entry + 0x84)) return 1;
        entry += 8;
    }
    return 0;
}

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
int fn_803E94E8(char *arg0)
{
    s32 sp8;
    u32 cnt;
    char *q;
    s32 r, v;
    u16 h;
    s32 var_r3;

    *(s32 *)(arg0 + 0x1F80) = 0;
    *(s32 *)(arg0 + 0x1F84) = 0;
    r = fn_803D6E5C();
    if (r != 0)
        return r;

    h = *(u16 *)arg0;
    if (((s32)h == 0) || (h & 0x1FF))
        return 0xF;

    v = (h >> 9) & 0x7F;
    if (v > (s32)*(u16 *)(arg0 + 0x1628) ||
        (v * 2) > (s32)*(u16 *)(arg0 + 0x162A))
        return 0x1E;

    r = PFCACHE_InitCaches(arg0);
    if (r != 0)
        return r;

    h = *(u16 *)(arg0 + 0x1F8C) | 8;
    *(u16 *)(arg0 + 0x1F8C) = h;
    if (!(h & 8)) {
        var_r3 = 9;
    } else {
        fn_803EBBDC(&sp8);
        *(s32 *)(arg0 + 0x1654) |= 1;
        if (find_id(sp8)) {
            *(s32 *)(arg0 + 0x189C) |= 1;
            *(s32 *)(arg0 + 0x18A0) = sp8;
        }

        q = arg0 + 0x165C;
        cnt = 0;
        do {
            r = PFENT_GetRootDir(arg0, q);
            if (r != 0) {
                var_r3 = r;
                goto done;
            }
            cnt++;
            q += 0x248;
        } while (cnt < 4);
        var_r3 = 0;
    }

done:
    if (var_r3 == 0 && (*(u16 *)(arg0 + 0x1F8C) & 0x40)) {
        var_r3 = fn_803D7168(arg0, *(s32 *)(arg0 + 0x1FC4));
        if (var_r3 == 0 && !(*(u16 *)(arg0 + 0x1F8C) & 0x80)) {
            var_r3 = PFFAT_InitFATRegion(arg0);
            if (var_r3 == 0)
                var_r3 = PFENT_MakeRootDir(arg0);
        }
    }

    if (var_r3 != 0)
        *(u16 *)(arg0 + 0x1F8C) &= ~8;
    return var_r3;
}
#undef lbl_805644C0

