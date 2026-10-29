#include <types.h>
#include <revolution/OS/OSError.h>
#include <revolution/OS/OSThread.h>
#include <revolution/OS/OSSemaphore.h>
#include <FAT/pf2.h>

// pfk diagnostics
extern char lbl_804930A0[0x2D];
extern char lbl_804930D0[0x23];
extern char lbl_804930F4[0x32];
extern char lbl_80493128[0x32];
extern char lbl_8049315C[0x2F];
extern char lbl_8049318C[0x33];

// Function bodies below are derived from https://github.com/jj-link/brawl, where each
// one was matched in a translation unit of its own.

int fn_803EC27C(u32* context) {
    OSThread* thread;

    if (context == NULL) {
        OSReport(lbl_804930A0);
        return -2;
    }
    thread = OSGetCurrentThread();
    if (thread == NULL) {
        OSReport(lbl_804930D0);
        *context = 0;
        return -3;
    }
    *context = (u32)thread;
    return 0;
}

s32 fn_803EC2F8(PFK_SEMAPHORE_INFO* info) {
    s32 count;
    OSSemaphore* sem;

    if (info == NULL || (sem = info->sem) == NULL || info->initial > info->max) {
        OSReport(lbl_804930F4);
        return 0;
    }
    OSInitSemaphore(sem, info->max);
    count = info->max - info->initial;
    while (count != 0) {
        OSWaitSemaphore(info->sem);
        count--;
    }
    return (s32)info->sem;
}

int fn_803EC394(s32 resource) {
    if (resource == 0) {
        OSReport(lbl_80493128);
        return -2;
    }
    return 0;
}

int fn_803EC3D4(s32 resource) {
    if (resource == 0) {
        OSReport(lbl_8049315C);
        return -2;
    }
    OSWaitSemaphore((OSSemaphore*)resource);
    return 0;
}

int fn_803EC418(s32 resource) {
    if (resource == 0) {
        OSReport(lbl_8049318C);
        return -2;
    }
    OSSignalSemaphore((OSSemaphore*)resource);
    return 0;
}
