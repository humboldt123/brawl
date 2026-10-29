#ifndef GAMESPY_COMMON_GSPLATFORM_H
#define GAMESPY_COMMON_GSPLATFORM_H

// GameSpy SDK platform layer for the Wii (gsPlatform*.c, gsMemory.c): the
// BSD-style socket wrappers over the SO library, time helpers and the memory
// callbacks. Unnamed functions carry their SDK names in comments.
#include <types.h>
#include <revolution/OS/OSThread.h>

#ifdef __cplusplus
extern "C" {
#endif

// The SO calls as this build links them. OpenRVL's SOBasic.h declares a
// SOSendTo with an extra length argument, so the wrappers use these instead.
int SOSocket(int domain, int type, int protocol);
int SOClose(int s);
int SOListen(int s, int backlog);
int SORecv(int s, void* buf, int len, int flags);
int SOSend(int s, const void* buf, int len, int flags);
int SOSendTo(int s, const void* buf, int len, int flags, const void* to);
int SOShutdown(int s, int how);

u16 SONtoHs(u16 netshort);
u16 SOHtoNs(u16 hostshort);
u32 SOHtoNl(u32 hostlong);

// The Wii socket address (SOSockAddrIn): length, family, port, address.
typedef struct gs_sockaddr_in {
    u8 sin_len;      // 0x0
    u8 sin_family;   // 0x1
    u16 sin_port;    // 0x2
    u32 sin_addr;    // 0x4
} gs_sockaddr_in;

// MSL's strcasecmp; this build's MSL headers do not declare it.
int strcasecmp(const char* s1, const char* s2);

// the last SO error of the wrappers
extern int lbl_805A1044;

typedef void* (*gsMallocCB)(u32 size);
typedef void (*gsFreeCB)(void* ptr);
typedef void* (*gsReallocCB)(void* ptr, u32 size);
typedef void* (*gsMemalignCB)(u32 boundary, u32 size);

// HYPOTHESIS: the memory callbacks lead the gsPlatform .bss; the rest of it
// is other platform state not touched by the matched code.
typedef struct gsPlatformData {
    gsMallocCB mallocCB;      // 0x0
    gsFreeCB freeCB;          // 0x4
    gsReallocCB reallocCB;    // 0x8
    gsMemalignCB memalignCB;  // 0xC
    u8 unk10[0x168];
} gsPlatformData;

__static_assert(sizeof(gsPlatformData) == 0x178, "gsPlatformData");

extern gsPlatformData lbl_80534638;
// the Util_RandInt seed
extern int lbl_8059F598;

// a thread started by gsiStartThread; its stack is freed with it
typedef struct GSIThread {
    OSThread mThread;   // 0x0
    void* mStack;       // 0x318
    u32 unk31C;
} GSIThread;

typedef struct GSIThreadID {
    void* mUserData;    // 0x0
    int mResult;        // 0x4
    int mIsRunning;     // 0x8
    int unkC;
    GSIThread mThread;  // 0x10
} GSIThreadID;

int socket(int domain, int type, int protocol);
int closesocket(int s);
int fn_8035C2D8(int s, int how);                 // shutdown
void fn_8035D720(void);                          // SocketStartUp
void fn_8035D724(void);                          // SocketShutDown
int listen(int s, int backlog);
int recv(int s, char* buf, int len, int flags);
int send(int s, const char* buf, int len, int flags);
int sendto(int s, const char* buf, int len, int flags, const void* to, int tolen);
int fn_8035C658(int s);                          // GOAGetLastError
int fn_8035C660(int s, int* readFlag, int* writeFlag, int* exceptFlag);  // GSISocketSelect
int CanReceiveOnSocket(int sock);
int CanSendOnSocket(int sock);
u32 fn_8035D1B8(u32* value);                     // gsiInterlockedIncrement
void fn_8035D258(s64* t);                        // time
int fn_8035D560(GSIThreadID* thread);            // gsiCleanupThread
u32 fn_8035D728(void);                           // current_time
void fn_8035D774(u32 msec);                      // msleep
void fn_8035DA18(int seed);                      // Util_RandSeed
void fn_8035DFCC(gsMallocCB p_malloc, gsFreeCB p_free, gsReallocCB p_realloc, gsMemalignCB p_memalign);  // gsiMemoryCallbacksSet
void* fn_8035DFE8(u32 size);                     // gsimalloc
void* fn_8035DFF8(void* ptr, u32 size);          // gsirealloc
void fn_8035E00C(void* ptr);                     // gsifree

#ifdef __cplusplus
}
#endif

#endif
