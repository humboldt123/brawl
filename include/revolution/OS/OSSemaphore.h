#ifndef RVL_SDK_OS_SEMAPHORE_H
#define RVL_SDK_OS_SEMAPHORE_H

// RVL SDK counting semaphores. OpenRVL does not provide this header yet; the
// layout and prototypes follow the Revolution SDK's os/OSSemaphore.h.
#include <revolution/types.h>
#include <revolution/OS/OSThread.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct OSSemaphore {
    s32 count;            // 0x0
    OSThreadQueue queue;  // 0x4
} OSSemaphore;

void OSInitSemaphore(OSSemaphore* sem, s32 count);
s32 OSWaitSemaphore(OSSemaphore* sem);
s32 OSTryWaitSemaphore(OSSemaphore* sem);
s32 OSSignalSemaphore(OSSemaphore* sem);
s32 OSGetSemaphoreCount(OSSemaphore* sem);

#ifdef __cplusplus
}
#endif

#endif
