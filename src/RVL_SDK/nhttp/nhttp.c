#include <types.h>
#include <stdio.h>
#include <string.h>

int printf(const char*, ...);
#include <revolution/OS/OSMessage.h>
#include <revolution/OS/OSMutex.h>
#include <revolution/NHTTP/d_nhttp.h>

// The original SDK built these as separate objects, so calls between them stay
// out of line. Inlining a helper into a caller changes code that already matched.
#pragma auto_inline off

// HYPOTHESIS: untyped placeholders for the zero-initialized .bss of this unit, sizes and order from the original layout.
unsigned char lbl_805365E0[0x40] __attribute__((aligned(8))) = { 0 };
unsigned char lbl_80536620[0xca0] __attribute__((aligned(8))) = { 0 };
// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
unsigned char lbl_805A1148[0x8] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A1150[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A1154[0x4] __attribute__((aligned(4))) = { 0 };

// Function bodies below are taken from https://github.com/jj-link/brawl.

void NHTTPi_kickCommThread(OSMessageQueue* queue) {
    OSSendMessage(queue, 0, 0);
}

void NHTTPi_InitRequestInfo(int *p) {
    *p = 0;
}

void* NHTTPi_memclr(void*, int);
void* NHTTPi_memcpy(void*, const void*, int);
extern int SOSend(int, void *, unsigned int, int);

int NHTTPi_SocSend_sub(int a, char *buf, unsigned int len, int d) {
    char tmp[32] __attribute__((aligned(32)));
    unsigned int head = (unsigned int)buf & 31 ? 32 - ((unsigned int)buf & 31) : 0;
    int total = 0;
    unsigned int n;
    int r;
    NHTTPi_memclr(tmp, 32);
    if (head != 0) {
        if (head > len) head = len;
        NHTTPi_memcpy(tmp, buf, head);
        r = SOSend(a, tmp, head, d);
        if (r > 0) {
            total = r;
            if ((unsigned int)r < head) return r;
            buf += r;
            len -= r;
        } else return r;
    }
    if ((int)len > 0 && (n = len & ~31) != 0) {
        r = SOSend(a, buf, n, d);
        if (r > 0) {
            total += r;
            if ((unsigned int)r < n) return total;
            buf += r;
            len -= r;
        } else {
            if (total > 0) return total;
            return r;
        }
    }
    if ((int)len > 0 && (n = len & 31) != 0) {
        NHTTPi_memclr(tmp, 32);
        NHTTPi_memcpy(tmp, buf, n);
        r = SOSend(a, tmp, n, d);
        if (r > 0) {
            total += r;
        } else {
            if (total > 0) return total;
            return r;
        }
    }
    return total;
}

int NHTTPi_encodeUrlChar(unsigned char *out, s8 in)
{
    s32 hi, lo;
    if (in == 0x20) {
        *out = 0x2B;
        return 1;
    }
    if ((in >= 0x30 && in <= 0x39) || (in >= 0x41 && in <= 0x5A) || (in >= 0x61 && in <= 0x7A)) {
        *out = (unsigned char)in;
        return 1;
    }
    hi = ((u32)in >> 4) & 0xF;
    *out = '%';
    lo = in & 0xF;
    out[1] = (hi < 0x0A) ? (hi + 0x30) : (hi + 0x37);
    out[2] = (lo < 0x0A) ? (lo + 0x30) : (lo + 0x37);
    return 3;
}

s32 NHTTPi_strToInt(u8 *arg0, s32 arg1) {
    s32 val;
    s32 seen;
    s32 prev;
    s32 i;
    u8 *p;
    s8 c;

    if (arg1 > 0xA) {
        return -1;
    }
    val = 0;
    seen = 0;
    p = arg0;
    for (i = arg1; i > 0; i--, p++) {
        c = (s8)*p;
        if (seen != 0) {
            if (c == 0x20 || c == 0) {
                break;
            }
        }
        if (seen == 0 && c == 0x20) {
            continue;
        }
        if (c < 0x30 || c > 0x39) {
            return -1;
        }
        prev = val;
        seen = 1;
        val = (c + val * 0xA) - 0x30;
        if (prev > val) {
            return -1;
        }
    }
    return val;
}

int NHTTPi_strtonum(unsigned char *arg0, int arg1) {
    int value;
    int count = 0;
    value = 0;
    for (; arg1; arg1--) {
        if ((char)*arg0 != ' ' && ((char)*arg0 >= '0') & ((char)*arg0 <= '9')) {
            count++;
            value = (char)*arg0 + value * 10 - '0';
            if (count > 9) return -1;
        }
        arg0++;
    }
    if (count != 0) return value;
    return -1;
}

NHTTPRequestHandle* fn_8039651C(const char* url, NHTTPReqMethod method, char* buffer, size_t bufferSize, NHTTPReqCallback callback, void* userContext, int a, int b);

NHTTPRequestHandle* NHTTPCreateRequest(const char* url, NHTTPReqMethod method, char* buffer, size_t bufferSize, NHTTPReqCallback callback, void* userContext) {
    return fn_8039651C(url, method, buffer, bufferSize, callback, userContext, 0, 0);
}

int NHTTPi_ControlConnectionList(int, int, int);

void* NHTTPi_Request2Connection(int a, int b) {
    return (void*)NHTTPi_ControlConnectionList(a, b, 1);
}

int NHTTPi_ControlConnectionList(int a, int b, int c);

void* NHTTPi_Response2Connection(int a, int b) {
    return (void*)NHTTPi_ControlConnectionList(a, b, 2);
}

int NHTTPi_ControlConnectionList(int a, int b, int c);

void* NHTTPi_GetConnection(int a, int b) {
    return (void*)NHTTPi_ControlConnectionList(a, b, 0);
}

// HYPOTHESIS: begin/end block inside the NHTTP system object. Offsets are the ones the stores use.
typedef struct NHTTPBgnEndInfo {
    u8 pad[0x7c4];
    void* alloc;     // HYPOTHESIS: allocator callback
    void* freeFn;    // HYPOTHESIS: free callback
    void* unk_7cc;
    s32 unk_7d0;
    s32 sslError;
    s32 error;
    s32 shutdown;
    s32 unk_7e0;
    s32 unk_7e4;
} NHTTPBgnEndInfo;

// HYPOTHESIS: OSMutex followed by an initialized flag. OSMutex is 0x18 bytes.
typedef struct NHTTPReqListLock {
    OSMutex mutex;
    s32 initialized;
} NHTTPReqListLock;

// HYPOTHESIS: message queue at the start, OSThread embedded at 0x30.
typedef struct NHTTPCommThread {
    OSMessageQueue queue;
    u8 pad[0x10];
    OSThread thread;
} NHTTPCommThread;

// HYPOTHESIS: flag at 0x348 of the thread-info block.
typedef struct NHTTPThreadInfo {
    u8 pad[0x348];
    s32 createMessageQueue;
} NHTTPThreadInfo;

// HYPOTHESIS: two list pointers, head then tail.
typedef struct NHTTPListInfo {
    void* head;
    void* tail;
} NHTTPListInfo;

// HYPOTHESIS: virtual content length at +0x2c.
typedef struct NHTTPVirtLen {
    u8 pad[0x2c];
    s32 virtualContentLength;
} NHTTPVirtLen;

// HYPOTHESIS: intrusive circular list node. The list pointer addresses the tail.
typedef struct NHTTPLink {
    struct NHTTPLink* next;
    struct NHTTPLink* prev;
    void* data;
} NHTTPLink;

// HYPOTHESIS: request state at +4; header list lives at +0x30.
typedef struct NHTTPReqHeader {
    u8 pad[4];
    s32 state;
} NHTTPReqHeader;

// HYPOTHESIS: connection list node; link to the next node is at +0x20.
typedef struct NHTTPConnNode {
    u8 pad[0x20];
    struct NHTTPConnNode* next;
} NHTTPConnNode;

void* NHTTPi_GetSystemInfoP(void);
void NHTTPi_CleanupAsync(void* sys, void* callback);
void NHTTPi_CommThreadProcMain(void* arg);
int addHdrList(void* list, void* bgn, const char* name, const char* value);
struct NHTTPLink* NHTTPi_getHdrFromList(struct NHTTPLink** list);

void OSRegisterVersion(const char*);
void OSReport(const char*, ...);
void OSPanic(const char*, int, const char*, ...);
int SOiSocket(int, int, int);
int SOShutdown(int, int);
int SOClose(int);
int SOGetAddrInfo(const char*, const char*, const void*, void**);
void SOFreeAddrInfo(void*);
int SOSetSockOpt(int, int, int, const void*, int);
int NCDGetCurrentIpConfig(void*);
extern const char* lbl_8059FCB8;

int fn_803900FC(void*, void*, void*, int);
int fn_80390954(NHTTPCommThread*, int, void*);
void fn_80390A64(void*, int);
int fn_80391144(void*, unsigned int);
int fn_80391160(void*, void*, void*, unsigned int, int);
int fn_80391180(void*, void*, void*, unsigned int, int, int);
int fn_80391888(void*, void*);
int fn_80391960(void*, void*);
int fn_80391AE4(void*);
int fn_80391B8C(void*, int);
int fn_80391D8C(u8*, const char*, char**);
int NHTTPi_SocRecv();
int fn_80392744(const char*, const char*, int);
int fn_80392CD4(const char*, const char*);
int fn_80392E18(const char*, int, const char*, int);
int SSLWrite(int, void*, unsigned int);
int SSLShutdown(int);
struct NHTTPThreadRequest;
int NHTTP_AddPostDataAscii(struct NHTTPThreadRequest*, void*, const char*, const char*);
void fn_80391784(void*, int);
int NHTTPi_TemplateConnectionCallback();
int NHTTPCreateConnection();
int fn_803974A4(void*, void*);

void* NHTTPi_memcpy(void* dst, const void* src, int n) {
    return memcpy(dst, src, n);
}

unsigned long NHTTPi_strlen(const char* s) {
    return strlen(s);
}

int NHTTPi_strcmp(const char* a, const char* b) {
    return strcmp(a, b);
}

void* NHTTPi_memclr(void* dst, int n) {
    return memset(dst, 0, n);
}

NHTTPLink* NHTTPi_getReqFromReqQueue(NHTTPLink** list) {
    return NHTTPi_getHdrFromList(list);
}

void NHTTPi_exitLockReqList(void) {
}

void NHTTPi_lockReqList(OSMutex* mutex) {
    OSLockMutex(mutex);
}

void NHTTPi_unlockReqList(OSMutex* mutex) {
    OSUnlockMutex(mutex);
}

void* NHTTPi_GetBgnEndInfoP(void* sys) {
    return sys;
}

void NHTTPi_SetError(NHTTPBgnEndInfo* info, s32 err) {
    info->error = err;
}

void NHTTPi_SetSSLError(NHTTPBgnEndInfo* info, s32 err) {
    info->sslError = err;
}

s32 NHTTPi_GetSSLError(NHTTPBgnEndInfo* info) {
    return info->sslError;
}

s32 NHTTPi_IsCreateCommThreadMessageQueue(NHTTPThreadInfo* info) {
    return info->createMessageQueue;
}

void* NHTTPi_GetListInfoP(void* sys) {
    return (u8*)sys + 0x800;
}

void* NHTTPi_GetReqInfoP(void* sys) {
    return (u8*)sys + 0x808;
}

void* NHTTPi_GetThreadInfoP(void* sys) {
    return (u8*)sys + 0x840;
}

void* NHTTPi_GetMutexInfoP(void* sys) {
    return (u8*)sys + 0x80c;
}

void NHTTPi_InitMutexInfo(NHTTPReqListLock* info) {
    info->initialized = 0;
}

void NHTTPi_InitThreadInfo(NHTTPThreadInfo* info) {
    info->createMessageQueue = 0;
}

void NHTTPi_IsCreateCommThreadMessageQueueOn(NHTTPThreadInfo* info) {
    info->createMessageQueue = 1;
}

void NHTTPi_InitListInfo(NHTTPListInfo* list) {
    list->head = 0;
    list->tail = 0;
}

void NHTTPi_SetVirtualContentLength(NHTTPVirtLen* obj, s32 len) {
    if (obj == 0) {
        return;
    }
    obj->virtualContentLength = len;
}

s32 NHTTPi_GetVirtualContentLength(NHTTPVirtLen* obj) {
    if (obj != 0) {
        return obj->virtualContentLength;
    }
    return 0;
}

void NHTTPi_InitBgnEndInfo(NHTTPBgnEndInfo* info) {
    info->sslError = 0;
    info->error = 0;
    info->alloc = 0;
    info->freeFn = 0;
    info->unk_7cc = 0;
    info->unk_7d0 = -1;
    info->shutdown = 0;
    info->unk_7e0 = 0;
    info->unk_7e4 = 0;
}

int NHTTPi_GetConnectionListLength(void) {
    NHTTPConnNode* node = *(NHTTPConnNode**)lbl_805A1150;
    int count = 0;
    while (node != 0) {
        node = node->next;
        count++;
    }
    return count;
}

int NHTTPi_CommitConnectionList(int a, int b) {
    // HYPOTHESIS: -1 when the list update returns 0, else 0.
    int v = NHTTPi_ControlConnectionList(a, b, 3);
    return ~(v | -v) >> 31;
}

int NHTTPi_OmitConnectionList(int a, int b) {
    // HYPOTHESIS: -1 when the list update returns 0, else 0.
    int v = NHTTPi_ControlConnectionList(a, b, 4);
    return ~(v | -v) >> 31;
}

void* NHTTPi_Connection2Request(int a, int b) {
    u8* link = (u8*)NHTTPi_ControlConnectionList(a, b, 0);
    if (link != 0) {
        return *(void**)(link + 0x10);
    }
    return 0;
}

void* NHTTPi_Connection2Response(int a, int b) {
    u8* link = (u8*)NHTTPi_ControlConnectionList(a, b, 0);
    if (link != 0) {
        return *(void**)(link + 0x14);
    }
    return 0;
}

void fn_803962D4(void* callback) {
    NHTTPi_CleanupAsync(NHTTPi_GetSystemInfoP(), callback);
}

s32 fn_80396308(void) {
    return NHTTPi_GetSSLError(NHTTPi_GetSystemInfoP());
}

void NHTTPi_initLockReqList(NHTTPReqListLock* info) {
    if (info->initialized == 0) {
        OSInitMutex(&info->mutex);
        info->initialized = 1;
    }
}

void NHTTPi_destroyCommThread(NHTTPCommThread* thread, NHTTPBgnEndInfo* info) {
    info->shutdown = 1;
    OSSendMessage(&thread->queue, 0, 0);
    OSJoinThread(&thread->thread, 0);
}

void NHTTPi_idleCommThread(OSMessageQueue* queue) {
    OSMessage msg;
    OSReceiveMessage(queue, &msg, OS_MSG_BLOCKING);
}

void* NHTTPi_CommThreadProc(void* arg) {
    NHTTPi_CommThreadProcMain(arg);
    return 0;
}

int NHTTP_AddHeaderField(NHTTPReqHeader* req, void* bgn, const char* name, const char* value) {
    if (req->state != 0) {
        return 0;
    }
    return addHdrList((u8*)req + 0x30, bgn, name, value);
}

static inline void nhttpFree(void* ptr) {
    NHTTPBgnEndInfo* info = (NHTTPBgnEndInfo*)NHTTPi_GetSystemInfoP();
    void (*freeFn)(void*) = info->freeFn;
    if (freeFn != 0) {
        freeFn(ptr);
    }
}

static inline void* nhttpAlloc(u32 size, s32 align) {
    NHTTPBgnEndInfo* info = (NHTTPBgnEndInfo*)NHTTPi_GetSystemInfoP();
    void* (*alloc)(u32, s32) = info->alloc;
    if (alloc != 0) {
        return alloc(size, align);
    }
    return 0;
}

void NHTTPi_free(void* ptr) { nhttpFree(ptr); }

void* NHTTPi_alloc(u32 size, s32 align) { return nhttpAlloc(size, align); }

NHTTPLink* NHTTPi_getHdrFromList(NHTTPLink** list) {
    NHTTPLink* node = *list;
    if (node != 0) {
        if (node != node->next) {
            node->next->prev = node->prev;
            node->prev->next = node->next;
            *list = node->prev;
        } else {
            *list = 0;
        }
    }
    return node;
}

void* NHTTPi_GetRequest(int a, int b) {
    u8* link = (u8*)NHTTPi_ControlConnectionList(a, b, 0);
    if (link != 0) {
        return *(void**)(link + 0x10);
    }
    return (void*)b;
}

void* NHTTPi_GetResponse(int a, int b) {
    u8* link = (u8*)NHTTPi_ControlConnectionList(a, b, 0);
    if (link != 0) {
        return *(void**)(link + 0x14);
    }
    return (void*)b;
}

void* NHTTPi_GetSystemInfoP(void) {
    if (*(s32*)lbl_805A1154 == 0) {
        u8* sys = lbl_80536620;
        *(u8**)lbl_805A1154 = sys;
        NHTTPi_InitBgnEndInfo((NHTTPBgnEndInfo*)sys);
        NHTTPi_InitListInfo((NHTTPListInfo*)(sys + 0x800));
        NHTTPi_InitRequestInfo((int*)(sys + 0x808));
        NHTTPi_InitMutexInfo((NHTTPReqListLock*)(sys + 0x80c));
        NHTTPi_InitThreadInfo((NHTTPThreadInfo*)(sys + 0x840));
    }
    return *(void**)lbl_805A1154;
}

void NHTTPi_InitConnectionList(void) {
    NHTTPConnNode* node = *(NHTTPConnNode**)lbl_805A1150;
    if (node != 0) {
        int count = 0;
        while (node != 0) {
            node = node->next;
            count++;
        }
        if (count != 0) {
            printf("*warning: %d connections rests! Please free connections.\n", count);
        }
    }
    *(s32*)lbl_805A1150 = 0;
}

int NHTTP_CancelRequestAsync(void* sys, int conn);

int fn_803967D0(int conn) {
    return NHTTP_CancelRequestAsync(NHTTPi_GetSystemInfoP(), conn) ? 0 : -1;
}

int NHTTPGetConnectionError(int id) {
    u8* conn = (u8*)NHTTPi_GetConnection((int)NHTTPi_GetMutexInfoP(NHTTPi_GetSystemInfoP()), id);
    if (conn != 0) {
        return *(s32*)(conn + 4);
    }
    return -1;
}

int NHTTPGetConnectionStatus(int id) {
    u8* conn = (u8*)NHTTPi_GetConnection((int)NHTTPi_GetMutexInfoP(NHTTPi_GetSystemInfoP()), id);
    if (conn == 0) {
        return -1;
    }
    return *(s32*)conn;
}

int NHTTP_GetProgress(void* sys, int a, int b);

int NHTTPGetProgress(int a, int b) {
    return NHTTP_GetProgress(NHTTPi_GetSystemInfoP(), a, b) ? 0 : -1;
}

int NHTTPGetResultCode(NHTTPResponseHandle id) {
    u8* resp = (u8*)NHTTPi_GetResponse((int)NHTTPi_GetMutexInfoP(NHTTPi_GetSystemInfoP()), (int)id);
    if (resp == 0) {
        return -1;
    }
    if (*(s32*)(resp + 0x14) != 0) {
        return *(s32*)(resp + 0x18);
    }
    return -1;
}

int NHTTPGetBodyBuffer(void* conn, void* out, int* local);

int NHTTPGetBodyAll(NHTTPResponseHandle id, char** out) {
    void* conn = NHTTPi_Response2Connection((int)NHTTPi_GetMutexInfoP(NHTTPi_GetSystemInfoP()), (int)id);
    int local;
    if (conn != 0) {
        local = 0;
        return NHTTPGetBodyBuffer(conn, out, &local);
    }
    return -1;
}

int NHTTPStartConnection(void* conn);

int NHTTPSendRequestAsync(NHTTPRequestHandle req) {
    u8* conn = (u8*)NHTTPi_Request2Connection((int)NHTTPi_GetMutexInfoP(NHTTPi_GetSystemInfoP()), (int)req);
    if (conn != 0 && NHTTPStartConnection(conn) == 0) {
        return *(s32*)(conn + 0x18);
    }
    return -1;
}

int NHTTPi_freeReqQueue(NHTTPLink** list, int arg, void* data);

void NHTTPi_allFreeReqQueue(NHTTPLink** list, int arg) {
    while (*list != 0) {
        NHTTPi_freeReqQueue(list, arg, (*list)->data);
    }
}

int fn_80396A18(int id, s32 option) {
    u8* req = (u8*)NHTTPi_GetRequest((int)NHTTPi_GetMutexInfoP(NHTTPi_GetSystemInfoP()), id);
    if (req == 0) {
        return -1;
    }
    *(s32*)(req + 0xcc) = option;
    return 0;
}

// Header node. next/prev match NHTTPLink; name sits where that struct keeps data.
typedef struct NHTTPHdrNode {
    struct NHTTPHdrNode* next;
    struct NHTTPHdrNode* prev;
    const char* name;
    const char* value;
    s32 extra;
    s32 extra2;
} NHTTPHdrNode;

// Address-info view used by the resolver. The IPv4 address is 4 bytes into ai_addr.
typedef struct NHTTPAddrInfo {
    u8 pad[0x18];
    u8* addr;
} NHTTPAddrInfo;

// NHTTPi_isRecvBufFull
int fn_80391144(void* buf, unsigned int n) {
    return n >= *(unsigned int*)((u8*)buf + 0x1c);
}

// NHTTPi_RecvBuf
int fn_80391160(void* a, void* req, void* c, unsigned int offset, int extra) {
    u8* inner = *(u8**)((u8*)req + 0x2c);
    unsigned int pos = *(unsigned int*)(inner + 0x28);
    unsigned int cap = *(unsigned int*)(inner + 0x1c);
    return NHTTPi_SocRecv(a, req, c, pos + offset, cap - offset, extra);
}

// NHTTPi_RecvBufN
int fn_80391180(void* a, void* req, void* c, unsigned int offset, int len, int flags) {
    u8* inner = *(u8**)((u8*)req + 0x2c);
    unsigned int cap = *(volatile unsigned int*)(inner + 0x1c);
    unsigned int pos;
    unsigned int at;
    int remain;
    if (cap <= offset) {
        return -0x3eb;
    }
    cap = *(volatile unsigned int*)(inner + 0x1c);
    pos = *(unsigned int*)(inner + 0x28);
    remain = (int)(cap - offset);
    at = pos + offset;
    if (len > remain) {
        len = remain;
    }
    return NHTTPi_SocRecv(a, req, c, at, len, flags);
}

// NHTTPi_SocOpen
int fn_80391ED0(void* req) {
    int fd = SOiSocket(2, 1, 0);
    int opt = 0;
    if (req != 0) {
        opt = *(s32*)((u8*)req + 0x244);
    }
    if (fd >= 0) {
        if (opt != 0) {
            SOSetSockOpt(fd, 0xFFFF, 0x1002, &opt, 4);
        }
    }
    return fd;
}

// NHTTPi_SocClose
int fn_80391F5C(void* mutex, u8* req, int sock) {
    int ssl;
    NHTTPi_lockReqList((OSMutex*)mutex);
    ssl = *(s32*)(req + 0xac);
    if (ssl > 0) {
        SSLShutdown(ssl);
        *(s32*)(req + 0xac) = -1;
    }
    NHTTPi_unlockReqList((OSMutex*)mutex);
    return SOClose(sock);
}

// NHTTPi_SocSend
int fn_803925B8(const u8* obj, int sock, char* buf, unsigned int len, int flags) {
    int r;
    if (*(s32*)(obj + 0xac) > 0) {
        r = SSLWrite(*(s32*)(obj + 0xac), buf, len);
    } else {
        r = NHTTPi_SocSend_sub(sock, buf, len, flags);
    }
    if (r < 0) {
        if (*(s32*)obj != 0) {
            r = -0x3ea;
        } else {
            if (*(s32*)(obj + 0xac) > 0) {
                if ((unsigned int)(r + 7) <= 1) {
                    return 0;
                }
            } else if (r == -0x38) {
                return 0;
            }
            r = -0x3e9;
        }
    }
    return r;
}

// NHTTPi_SocCancel
void fn_80392664(void* mutex, void* unused, int sock) {
    (void)unused;
    NHTTPi_lockReqList((OSMutex*)mutex);
    if (sock >= 0) {
        SOShutdown(sock, 2);
    }
    NHTTPi_unlockReqList((OSMutex*)mutex);
}

// NHTTPi_resolveHostname
u32 fn_803926B8(void* unused, const char* host) {
    struct {
        void* info;
        u32 addr;
    } loc;
    (void)unused;
    loc.addr = 0;
    if (SOGetAddrInfo(host, 0, 0, &loc.info) == 0) {
        u8* ai = *(u8**)((u8*)loc.info + 0x18);
        NHTTPi_memcpy(&loc.addr, ai + 4, 4);
        SOFreeAddrInfo(loc.info);
    } else {
        loc.addr = 0;
    }
    return loc.addr;
}

// NHTTPi_strnicmp
int fn_80392744(const char* a, const char* b, int n) {
    int c1;
    int c2;
    for (; n > 0; n--) {
        c1 = (s8)*a++;
        c2 = (s8)*b++;
        if (c1 == 0 || c2 == 0) {
            if (c1 == 0 && c2 == 0) {
                n = 0;
                break;
            }
        }
        if ((c2 >= 0x41) & (c2 <= 0x5a)) {
            c2 += 0x20;
        }
        if ((c1 >= 0x41) & (c1 <= 0x5a)) {
            c1 += 0x20;
        }
        if (c1 != c2) {
            break;
        }
    }
    return n;
}

// NHTTPi_getUrlEncodedSize
int fn_80392810(const char* s) {
    s8 c = (s8)*s++;
    int n = 0;
    while (c != 0) {
        if ((c >= 0x30 && c <= 0x39) || (c >= 0x41 && c <= 0x5a) || (c >= 0x61 && c <= 0x7a) || c == 0x20) {
            n += 1;
        } else {
            n += 3;
        }
        c = (s8)*s++;
    }
    return n;
}

// NHTTPi_getUrlEncodedSize2
int fn_80392880(const char* s, int len) {
    s8 c = (s8)*s++;
    int n = 0;
    for (; len > 0; len--) {
        if ((c >= 0x30 && c <= 0x39) || (c >= 0x41 && c <= 0x5a) || (c >= 0x61 && c <= 0x7a) || c == 0x20) {
            n += 1;
        } else {
            n += 3;
        }
        c = (s8)*s++;
    }
    return n;
}

// NHTTPi_compareToken
static inline int nhttpLowerChar(int c) {
    return ((c >= 'A') & (c <= 'Z')) ? c + 0x20 : c;
}

int fn_80392CD4(const char* s, const char* token) {
    while ((((*s >= 'A') & (*s <= 'Z')) ? *s + 0x20 : *s) ==
           (((*token >= 'A') & (*token <= 'Z')) ? *token + 0x20 : *token)) {
        if (*s == 0 || *s == 0x20) return 0;
        s++;
        token++;
    }
    return -1;
}

// NHTTPi_memfind. 0 when needle occurs in the first hayLen bytes, else -1.
int fn_80392E18(const char* hay, int hayLen, const char* needle, int needleLen) {
    int last;
    int i;
    const char* cursor;
    if (hayLen < needleLen) return -1;
    last = hayLen - needleLen + 1;
    cursor = hay;
    for (i = 0; i < last; i++, cursor++) {
        if (needle[0] == *cursor) {
            const char* b;
            const char* a;
            int matched;
            a = &hay[i + 1];
            b = needle + 1;
            for (matched = 1; matched < needleLen; matched++, b++, a++) {
                if (*a != *b) break;
            }
            if (matched == needleLen) return 0;
        }
    }
    return -1;
}

// NHTTPi_createCommThread
int fn_80390954(NHTTPCommThread* thread, int priority, void* stack) {
    if (NHTTPi_IsCreateCommThreadMessageQueue((NHTTPThreadInfo*)thread) == 0) {
        OSInitMessageQueue((OSMessageQueue*)thread, (OSMessage*)((u8*)thread + 0x20), 3);
        NHTTPi_IsCreateCommThreadMessageQueueOn((NHTTPThreadInfo*)thread);
    }
    OSCreateThread(&thread->thread, NHTTPi_CommThreadProc, 0, (u8*)stack + 0x2000, 0x2000, priority, 0);
    OSResumeThread(&thread->thread);
    return 1;
}

// NHTTPi_CheckCurrentThread
static const char nhttpCheckName[] = "NHTTPi_CheckCurrentThread";
static const char nhttpCheckFormat[] = "%s:illegal thread\n";
static const char nhttpCheckFile[] = "NHTTP_os_RVL.c";

void fn_80390A64(void* info, int outside) {
    OSThread* cur = OSGetCurrentThread();
    OSThread* comm = (OSThread*)((u8*)info + 0x30);
    if (cur == 0) {
        return;
    }
    if (outside == 0) {
        if (cur != comm) {
            goto bad;
        }
    }
    if (outside == 0) {
        return;
    }
    if (cur != comm) {
        return;
    }
bad:
    OSReport(nhttpCheckFormat, nhttpCheckName);
    OSPanic(nhttpCheckFile, 0xdf, "halt\n");
}

#pragma push
#pragma section const_type ".rodata.nhttp_startup"
static const char nhttpStartupFormat[] = "NCDGetCurrentIpConfig err = %d\n";
static const char nhttpStartupFile[] = "NHTTP_bgnend.c";
static const char nhttpStartupName[] = "NCDGetCurrentIpConfig";

#pragma pop

// NHTTPi_Startup
int fn_803900FC(void* sys, void* alloc, void* freeFn, int priority) {
    void* stack;
    NHTTPBgnEndInfo* info = (NHTTPBgnEndInfo*)NHTTPi_GetBgnEndInfoP(sys);
    void* list = NHTTPi_GetListInfoP(sys);
    int* req = (int*)NHTTPi_GetReqInfoP(sys);
    NHTTPReqListLock* mutex = (NHTTPReqListLock*)NHTTPi_GetMutexInfoP(sys);
    void* thread = NHTTPi_GetThreadInfoP(sys);
    info->alloc = alloc;
    info->freeFn = freeFn;
    info->error = 0;
    info->sslError = 0;
    info->shutdown = 0;
    NHTTPi_InitListInfo((NHTTPListInfo*)list);
    NHTTPi_InitRequestInfo(req);
    NHTTPi_initLockReqList(mutex);
    NHTTPi_InitConnectionList();
    info->unk_7d0 = -1;
    stack = nhttpAlloc(0x2000, 8);
    info->unk_7e0 = (s32)stack;
    if (stack == 0) {
        info->error = 1;
        NHTTPi_exitLockReqList();
        return 0;
    }
    if (fn_80390954(thread, priority, stack) == 0) {
        info->error = 9;
        nhttpFree((void*)info->unk_7e0);
        info->unk_7e0 = 0;
        NHTTPi_exitLockReqList();
        return 0;
    }
    {
        int ip = NCDGetCurrentIpConfig((void*)info);
        if (ip < 0) {
            OSReport(nhttpStartupFormat, ip);
            OSPanic(nhttpStartupFile, 0xe6, nhttpStartupName);
        }
    }
    info->unk_7cc = (void*)1;
    return 1;
}

void NHTTPi_CleanupAsync(void* sys, void* callback) {
    NHTTPBgnEndInfo* info = (NHTTPBgnEndInfo*)NHTTPi_GetBgnEndInfoP(sys);
    void* thread = NHTTPi_GetThreadInfoP(sys);
    void* stack;
    void (*freeCb)(void*);
    int count;
    fn_80390A64(thread, 1);
    fn_80391AE4(sys);
    NHTTPi_destroyCommThread((NHTTPCommThread*)thread, info);
    stack = (void*)info->unk_7e0;
    freeCb = (void (*)(void*))((NHTTPBgnEndInfo*)NHTTPi_GetSystemInfoP())->freeFn;
    if (freeCb != 0) {
        freeCb(stack);
    }
    info->unk_7e0 = 0;
    NHTTPi_exitLockReqList();
    info->unk_7cc = 0;
    if (callback != 0) {
        ((void (*)(void))callback)();
    }
    count = NHTTPi_GetConnectionListLength();
    if (count != 0) {
        printf("*warning: %d connections rests! Please free connections.\n", count);
    }
    if (info->unk_7d0 >= 0) {
        SOClose(info->unk_7d0);
        info->unk_7d0 = -1;
    }
}

int addHdrList(void* list, void* bgn, const char* name, const char* value) {
    NHTTPHdrNode** headp = (NHTTPHdrNode**)list;
    int found = 0;
    NHTTPHdrNode* node = *headp;
    NHTTPHdrNode* neu;
    if (node != 0) {
        if (fn_80392CD4(name, node->name) != 0) {
            node = node->prev;
            while (node != *headp) {
                if (fn_80392CD4(name, node->name) == 0) {
                    found = 1;
                    break;
                }
                node = node->prev;
            }
        } else {
            found = 1;
        }
    }
    if (found != 0) {
        node->value = value;
    } else {
        neu = (NHTTPHdrNode*)NHTTPi_alloc(0x18, 4);
        if (neu == 0) {
            printf("failed to allocate memory\n");
            NHTTPi_SetError((NHTTPBgnEndInfo*)bgn, 1);
            return 0;
        }
        neu->name = name;
        neu->value = value;
        neu->extra = 0;
        neu->extra2 = 0;
        if (*headp != 0) {
            neu->next = (*headp)->next;
            neu->prev = *headp;
            (*headp)->next->prev = neu;
            (*headp)->next = neu;
        } else {
            neu->prev = neu;
            neu->next = neu;
            *headp = neu;
        }
    }
    return 1;
}

// NHTTPi_setReqQueue. Returns the id stored on the new node, or -1.
int fn_803906BC(void* list, void* data) {
    u8* node;
    u8* lp = (u8*)list;
    int id = -1;
    node = (u8*)NHTTPi_alloc(0x14, 4);
    if (node != 0) {
        s32 count;
        if (*(u8**)lp != 0) {
            *(u8**)node = **(u8***)lp;
            *(u8**)(node + 4) = *(u8**)lp;
            *(u8**)(*(u8**)(*(u8**)lp) + 4) = node;
            **(u8***)lp = node;
        } else {
            *(u8**)node = node;
            *(u8**)(node + 4) = node;
            *(u8**)lp = node;
        }
        count = *(s32*)(lp + 4);
        *(s32*)(node + 8) = count;
        *(s32*)(lp + 4) = count + 1;
        *(void**)(node + 0xc) = data;
        *(s32*)(node + 0x10) = -1;
        id = *(s32*)(node + 8);
        if (*(s32*)(lp + 4) < 0) {
            *(s32*)(lp + 4) = 0;
        }
    }
    return id;
}

int NHTTPi_freeReqQueue(NHTTPLink** list, int arg, void* data) {
    NHTTPLink* n;
    NHTTPLink* head;
    int ret = 0;
    NHTTPLink* found = 0;
    s32 id = (s32)data;
    head = *list;
    if (head != 0) {
        if ((s32)head->data == id) {
            found = head;
        } else {
            n = head->prev;
            while (n != head) {
                if ((s32)n->data == id) {
                    found = n;
                    break;
                }
                n = n->prev;
            }
        }
    }
    if (found != 0) {
        void* conn;
        if (head != head->next) {
            found->next->prev = found->prev;
            found->prev->next = found->next;
            if (*list == found) {
                *list = found->prev;
            }
        } else {
            *list = 0;
        }
        conn = NHTTPi_Request2Connection(arg, *(int*)((u8*)found + 0xc));
        fn_80391888((void*)arg, *(void**)((u8*)found + 0xc));
        NHTTPi_free(found);
        if (conn != 0) {
            *(s32*)((u8*)conn + 4) = 8;
            fn_803974A4((void*)arg, conn);
        }
        ret = 1;
    }
    return ret;
}

int NHTTPStartConnection(void* conn) {
    void* sys = NHTTPi_GetSystemInfoP();
    u8* found = (u8*)NHTTPi_GetConnection((int)NHTTPi_GetMutexInfoP(sys), (int)conn);
    int r;
    if (found == 0) {
        return -1;
    }
    if (*(void**)(found + 0x10) == 0) {
        return -1;
    }
    r = fn_80391960(sys, *(void**)(found + 0x10));
    *(s32*)(found + 0x18) = r;
    if (r >= 0) {
        *(s32*)found = 1;
    }
    return 0;
}

// NHTTPCancelConnection
int fn_8039602C(int id) {
    void* sys = NHTTPi_GetSystemInfoP();
    u8* conn = (u8*)NHTTPi_GetConnection((int)NHTTPi_GetMutexInfoP(sys), id);
    s32 handle;
    if (conn == 0) {
        return -1;
    }
    handle = *(s32*)(conn + 0x18);
    if (handle >= 0) {
        NHTTP_CancelRequestAsync(sys, handle);
        return 0;
    }
    return -1;
}

int NHTTPGetBodyBuffer(void* conn, void* out, int* local) {
    void* mutex = NHTTPi_GetMutexInfoP(NHTTPi_GetSystemInfoP());
    void* found = NHTTPi_GetConnection((int)mutex, (int)conn);
    if (found != 0) {
        u8* resp = (u8*)NHTTPi_Connection2Response((int)mutex, (int)found);
        if (resp != 0) {
            *(void**)out = *(void**)(resp + 0x28);
            *local = *(s32*)(resp + 0x1c);
            return *(s32*)(resp + 4);
        } else return -1;
    } else return -1;
}

// NHTTPGetUserParam
void* fn_80396188(int id) {
    void* mutex = NHTTPi_GetMutexInfoP(NHTTPi_GetSystemInfoP());
    void* conn = NHTTPi_GetConnection((int)mutex, id);
    if (conn != 0) {
        u8* resp = (u8*)NHTTPi_Connection2Response((int)mutex, (int)conn);
        if (resp != 0) return *(void**)(resp + 0x438);
        else return 0;
    } else return 0;
}

int NHTTPStartup(NHTTPAlloc alloc, NHTTPFree free, u32 priority) {
    void* sys = NHTTPi_GetSystemInfoP();
    if (*(s32*)lbl_805A1148 == 0) {
        OSRegisterVersion(lbl_8059FCB8);
        *(s32*)lbl_805A1148 = 1;
    }
    return -!fn_803900FC(sys, (void*)alloc, (void*)free, (int)priority);
}


int NHTTPAddHeaderField(void* id, const char* name, const char* value) {
    void* sys = NHTTPi_GetSystemInfoP();
    void* req;
    void* bgn;
    req = NHTTPi_GetRequest((int)NHTTPi_GetMutexInfoP(sys), (int)id);
    bgn = NHTTPi_GetBgnEndInfoP(sys);
    if (req == 0) {
        return -1;
    }
    if (*(s32*)((u8*)req + 4) != 0) {
        printf("%s can be called before NHTTPStartConnection()\n", "NHTTPAddHeaderField");
        return -1;
    }
    return NHTTP_AddHeaderField(req, bgn, name, value) ? 0 : -1;
}

int NHTTPAddPostDataAscii(void* id, const char* name, const char* value) {
    void* sys = NHTTPi_GetSystemInfoP();
    void* req;
    void* bgn;
    req = NHTTPi_GetRequest((int)NHTTPi_GetMutexInfoP(sys), (int)id);
    bgn = NHTTPi_GetBgnEndInfoP(sys);
    if (req == 0) {
        return -1;
    }
    if (*(s32*)((u8*)req + 4) != 0) {
        printf("%s can be called before NHTTPStartConnection()\n", "NHTTPAddPostDataAscii");
        return -1;
    }
    return NHTTP_AddPostDataAscii((struct NHTTPThreadRequest*)req, bgn, name, value) ? 0 : -1;
}

void NHTTPDestroyResponse(NHTTPResponseHandle id) {
    void* mutex = NHTTPi_GetMutexInfoP(NHTTPi_GetSystemInfoP());
    void* conn = NHTTPi_Response2Connection((int)mutex, (int)id);
    void* resp;
    if (conn == 0) {
        return;
    }
    resp = NHTTPi_Connection2Response((int)mutex, (int)conn);
    if (resp != 0) {
        fn_80391B8C(mutex, *(s32*)((u8*)conn + 0x14));
    }
    NHTTPi_OmitConnectionList((int)mutex, (int)conn);
    NHTTPi_free(conn);
}

// NHTTPGetHeaderField
int fn_803968E4(int id, const char* name, char** out) {
    void* resp = NHTTPi_GetResponse((int)NHTTPi_GetMutexInfoP(NHTTPi_GetSystemInfoP()), id);
    if (resp == 0) {
        return -1;
    }
    return fn_80391D8C(resp, name, out);
}

// NHTTPSetRootCADefault
int fn_80396D18(int id) {
    u8* req = (u8*)NHTTPi_GetRequest((int)NHTTPi_GetMutexInfoP(NHTTPi_GetSystemInfoP()), id);
    int ret;
    if (req == 0) {
        ret = -1;
    } else {
        *(s32*)(req + 0xd8) = 0;
        ret = 0;
        *(s32*)(req + 0xc0) = 0;
        *(s32*)(req + 0xc4) = 0;
    }
    return ret;
}

// NHTTPSetClientCertDefault
int fn_80396D78(int id) {
    u8* req = (u8*)NHTTPi_GetRequest((int)NHTTPi_GetMutexInfoP(NHTTPi_GetSystemInfoP()), id);
    int ret;
    if (req == 0) {
        ret = -1;
    } else {
        *(s32*)(req + 0xdc) = 0;
        ret = 0;
        *(s32*)(req + 0xc8) = 1;
        *(s32*)(req + 0xb0) = 0;
        *(s32*)(req + 0xb4) = 0;
        *(s32*)(req + 0xb8) = 0;
        *(s32*)(req + 0xbc) = 0;
    }
    return ret;
}

// NHTTPi_WaitForCompletion. The condvar lives in the unit's .bss block.
void fn_80396DE8(u8* obj) {
    u8* base = lbl_805365E0;
    if (*(s32*)lbl_805365E0 == 0) {
        OSInitMutex((OSMutex*)(base + 4));
        OSInitCond((OSCond*)(base + 0x1c));
        *(s32*)base = 1;
    }
    OSLockMutex((OSMutex*)(base + 4));
    while (*(s32*)(obj + 0xc) != 0) {
        OSWaitCond((OSCond*)(base + 0x1c), (OSMutex*)(base + 4));
    }
    OSUnlockMutex((OSMutex*)(base + 4));
}

// NHTTPi_NotifyCompletion
void fn_80396E70(u8* obj) {
    u8* base = lbl_805365E0;
    if (*(s32*)lbl_805365E0 == 0) {
        OSInitMutex((OSMutex*)(base + 4));
        OSInitCond((OSCond*)(base + 0x1c));
        *(s32*)base = 1;
    }
    OSLockMutex((OSMutex*)(base + 4));
    *(s32*)(obj + 0xc) = 0;
    OSSignalCond((OSCond*)(base + 0x1c));
    OSUnlockMutex((OSMutex*)(base + 4));
}

// NHTTPi_CompleteCallback
int fn_803974A4(void* sys, void* conn) {
    typedef void (*NHTTPConnCb)(void*, int, int);
    if (NHTTPi_ControlConnectionList((int)sys, (int)conn, 0) != 0) {
        NHTTPConnCb cb = *(NHTTPConnCb*)((u8*)conn + 0x1c);
        if (cb != 0) {
            cb(conn, 4, 0);
        }
    }
}

int NHTTP_CancelRequestAsync(void* sys, int id) {
    int ret = 0;
    void* reqInfo = NHTTPi_GetReqInfoP(sys);
    void* mutex = NHTTPi_GetMutexInfoP(sys);
    u8* head = *(u8**)reqInfo;
    NHTTPi_lockReqList((OSMutex*)mutex);
    if (head != 0 && *(s32*)(head + 8) == id && *(s32*)(*(u8* volatile*)(head + 0xc)) == 0) {
        *(s32*)(*(u8* volatile*)(head + 0xc)) = 1;
        fn_80392664(mutex, *(void* volatile*)(head + 0xc), *(s32*)(head + 0x10));
        ret = 1;
    }
    if (ret == 0) {
        ret = NHTTPi_freeReqQueue((NHTTPLink**)NHTTPi_GetListInfoP(sys), (int)mutex, (void*)id);
    }
    NHTTPi_unlockReqList((OSMutex*)mutex);
    return ret;
}

int fn_80391AE4(void* sys) {
    void* reqInfo = NHTTPi_GetReqInfoP(sys);
    void* list = NHTTPi_GetListInfoP(sys);
    void* mutex = NHTTPi_GetMutexInfoP(sys);
    u8* head = *(u8**)reqInfo;
    NHTTPi_lockReqList((OSMutex*)mutex);
    if (head != 0 && *(s32*)(*(u8* volatile*)(head + 0xc)) == 0) {
        *(s32*)(*(u8* volatile*)(head + 0xc)) = 1;
        fn_80392664(mutex, *(void* volatile*)(head + 0xc), *(s32*)(head + 0x10));
    }
    NHTTPi_allFreeReqQueue((NHTTPLink**)list, (int)mutex);
    NHTTPi_unlockReqList((OSMutex*)mutex);
}

int NHTTP_GetProgress(void* sys, int received, int total) {
    void* mutex = NHTTPi_GetMutexInfoP(sys);
    u8* head = *(u8**)NHTTPi_GetReqInfoP(sys);
    s32* outR = (s32*)received;
    s32* outT = (s32*)total;
    int ret;
    *outR = 0;
    *outT = 0;
    NHTTPi_lockReqList((OSMutex*)mutex);
    if (head != 0) {
        if (*(s32*)(*(u8* volatile*)(*(u8* volatile*)(head + 0xc) + 0x2c) + 0xc) != 0) {
            s32 len;
            *outR = *(s32*)(*(u8* volatile*)(*(u8* volatile*)(head + 0xc) + 0x2c) + 4);
            len = *(s32*)(*(u8* volatile*)(*(u8* volatile*)(head + 0xc) + 0x2c) + 0xc);
            if ((unsigned)(len + 0x10000) == 0xFFFFu) {
                len = 0;
            } else {
                len = *(s32*)(*(u8* volatile*)(*(u8* volatile*)(head + 0xc) + 0x2c) + 0xc);
            }
            *outT = len;
        }
        ret = 1;
    } else {
        ret = *(void**)NHTTPi_GetListInfoP(sys) != 0;
    }
    NHTTPi_unlockReqList((OSMutex*)mutex);
    return ret;
}

// NHTTP_DestroyResponse
int fn_80391B8C(void* mutex, int respId) {
    typedef void (*NHTTPFreeFn)(void*);
    typedef void (*NHTTPCleanupCb)(void*, NHTTPFreeFn, void*);
    u8* next;
#define resp ((u8*)respId)
    while (*(u8**)(resp + 0x34) != 0) {
        u8* node = *(u8**)(resp + 0x34);
        next = *(u8**)node;
        NHTTPi_free(node);
        *(u8**)(resp + 0x34) = next;
    }
    {
        u8* node = *(u8**)(resp + 0x20);
        if (node != 0) {
            NHTTPi_free(node);
        }
    }
    {
        u8* node = *(u8**)(resp + 0x24);
        if (node != 0) {
            NHTTPi_free(node);
        }
    }
    {
        NHTTPCleanupCb cb = *(NHTTPCleanupCb*)(resp + 0x30);
        if (cb != 0) {
            cb(*(void**)(resp + 0x28), NHTTPi_free, *(void**)(resp + 0x438));
            *(s32*)(resp + 0x28) = 0;
            *(s32*)(resp + 0x1c) = 0;
        }
    }
    {
        void* conn = NHTTPi_Response2Connection((int)mutex, (int)resp);
        if (conn != 0) {
            *(s32*)((u8*)conn + 0x14) = 0;
        }
    }
    NHTTPi_free(resp);
#undef resp
}

// The last four header bytes sit in a 4-byte window. len is the write count.
int fn_803931A8(u8* buf, int len) {
    u8 prev = buf[(len - 2) & 3];
    if ((s8)prev == 0x0d) {
        if ((s8)buf[(len - 1) & 3] == 0x0d) {
            return 1;
        }
    }
    if ((s8)prev == 0x0a) {
        if ((s8)buf[(len - 1) & 3] == 0x0a) {
            return 1;
        }
    }
    if ((s8)buf[(len - 4) & 3] == 0x0d && (s8)buf[(len - 3) & 3] == 0x0a && (s8)prev == 0x0d &&
        (s8)buf[(len - 1) & 3] == 0x0a) {
        return 1;
    }
    return 0;
}

int fn_80393270(s32*, u8*, int, s32*, u8*, int);

// NHTTPi_SendData
int fn_803936B0(u8* obj, void* buf, int len) {
    void* sys = NHTTPi_GetSystemInfoP();
    NHTTPBgnEndInfo* bgn = (NHTTPBgnEndInfo*)NHTTPi_GetBgnEndInfoP(sys);
    u8* thr = (u8*)NHTTPi_GetThreadInfoP(sys);
    s32** req = (s32**)NHTTPi_GetReqInfoP(sys);
    void* q = (void*)(*req)[3];
    void* t = thr + 0x360;
    int r;
    if (len == 0) {
        return 0;
    }
    r = fn_80393270(q, t, bgn->unk_7d0, (s32*)(obj + 0x324), buf, len);
    if (r < 0) {
        return 1;
    }
    if (r == 0) {
        return 2;
    }
    return 0;
}

// NHTTPi_ThreadExecReqQueue
int fn_80394664(s32* out) {
    void* sys = NHTTPi_GetSystemInfoP();
    OSMutex* mutex = (OSMutex*)NHTTPi_GetMutexInfoP(sys);
    NHTTPLink* link;
    NHTTPi_lockReqList(mutex);
    link = NHTTPi_getReqFromReqQueue((NHTTPLink**)NHTTPi_GetListInfoP(sys));
    if (link != 0) {
        NHTTPLink** req = (NHTTPLink**)NHTTPi_GetReqInfoP(sys);
        *out = (s32)link->data;
        *req = link;
    } else {
        *out = -1;
    }
    NHTTPi_unlockReqList(mutex);
    if (*out < 0) {
        NHTTPi_idleCommThread((OSMessageQueue*)NHTTPi_GetThreadInfoP(sys));
        return 0;
    }
    return 1;
}

int fn_803906BC(void*, void*);

// NHTTP_SendRequestAsync
int fn_80391960(void* sys, void* reqv) {
    NHTTPBgnEndInfo* bgn = (NHTTPBgnEndInfo*)NHTTPi_GetBgnEndInfoP(sys);
    void* thr = NHTTPi_GetThreadInfoP(sys);
    OSMutex* mutex = (OSMutex*)NHTTPi_GetMutexInfoP(sys);
    int r;
    if (((s32*)reqv)[1] != 0) {
        NHTTPi_SetError(bgn, 0xb);
        return -1;
    }
    NHTTPi_lockReqList(mutex);
    r = fn_803906BC(NHTTPi_GetListInfoP(sys), reqv);
    if (r >= 0) {
        ((s32*)reqv)[1] = 1;
        NHTTPi_kickCommThread((OSMessageQueue*)thr);
    } else {
        NHTTPi_SetError(bgn, 1);
    }
    NHTTPi_unlockReqList(mutex);
    return r;
}

int SOConnect(int, void*);
unsigned short SOHtoNs(unsigned short);
int NHTTPi_SocSSLConnect(void*, void*, void*, int);

// NHTTPi_SocConnect
int fn_80391FC8(void* a, void* b, s32* c, int sock, int addr, int port) {
    u8 sa[8];
    sa[0] = 8;
    sa[1] = 2;
    *(u16*)(sa + 2) = SOHtoNs(port);
    *(s32*)(sa + 4) = addr;
    if (SOConnect(sock, sa) < 0) {
        int e = -0x3e9;
        if (c[0] != 0) {
            e = -0x3ea;
        }
        return e;
    }
    if (c[2] != 0 && c[3] == 0) {
        return NHTTPi_SocSSLConnect(a, b, c, sock);
    }
    return 0;
}

void NHTTPi_BufferFullCallback(int, void*);

// NHTTPi_BufFull
int fn_803935E4(int a, u32* b) {
    int r = 0;
    int full = fn_80391144(b, b[1]);
    if (b[7] == 0 || b[10] == 0 || full != 0) {
        void* resp = NHTTPi_Response2Connection(a, (int)b);
        if (resp != 0) {
            NHTTPi_BufferFullCallback(a, resp);
            if (b[10] != 0 && b[7] != 0) {
                if (fn_80391144(b, b[1]) == 0) {
                    r = 1;
                }
            }
        }
    } else if (full == 0) {
        r = 1;
    }
    return r;
}

// NHTTPi_destroyRequestObject
int fn_80391888(void* sysv, void* reqv) {
    int sys = (int)sysv;
    u8* conn = (u8*)NHTTPi_Request2Connection(sys, (int)reqv);
    if (conn != 0) {
        *(s32*)(conn + 0x10) = 0;
    }
    {
        NHTTPLink* n = *(NHTTPLink**)((u8*)reqv + 0x30);
        while (n != 0) {
            NHTTPLink* head = n->next;
            if (n != head) {
                NHTTPLink* nxt = head->next;
                NHTTPi_free(head);
                n->next = nxt;
            } else {
                NHTTPi_free(n);
                n = 0;
            }
        }
    }
    {
        NHTTPLink* m = *(NHTTPLink**)((u8*)reqv + 0x34);
        while (m != 0) {
            NHTTPLink* head = m->next;
            if (m != head) {
                NHTTPLink* nx = head->next;
                NHTTPi_free(head);
                m->next = nx;
            } else {
                NHTTPi_free(m);
                m = 0;
            }
        }
    }
    NHTTPi_free(*(void**)((u8*)reqv + 0x24));
    NHTTPi_free(*(void**)((u8*)reqv + 0x28));
    NHTTPi_free(reqv);
    return 1;
}

// NHTTPi_SaveBuf
int fn_80393270(s32* a, u8* buf, int sock, s32* fill, u8* src, int len) {
    int n;
    int rem = len;
    while (rem > 0) {
        int sent;
        if (*a != 0) {
            return -1;
        }
        n = rem;
        if (n > 0x100 - *fill) {
            n = 0x100 - *fill;
        }
        NHTTPi_memcpy(buf + *fill, src, n);
        src += n;
        rem -= n;
        *fill = *fill + n;
        if (*fill == 0x100) {
            sent = fn_803925B8((u8*)a, sock, (char*)buf, 0x100, 0);
            if (sent <= 0) {
                return sent;
            }
            *fill = *fill - sent;
        }
    }
    return len;
}

// NHTTPi_SendHeaderList
int fn_80393BFC(u8* obj) {
    u8* req = (u8*)*(s32**)NHTTPi_GetReqInfoP(NHTTPi_GetSystemInfoP());
    u8* conn = *(u8**)(req + 0xc);
    NHTTPHdrNode* node = (NHTTPHdrNode*)NHTTPi_getHdrFromList((NHTTPLink**)(conn + 0x30));
    int r;
    while (node != 0) {
        r = fn_803936B0(obj, (void*)node->name, NHTTPi_strlen(node->name));
        if (r != 0) {
            return r;
        }
        r = fn_803936B0(obj, ": ", 2);
        if (r != 0) {
            return r;
        }
        r = fn_803936B0(obj, (void*)node->value, NHTTPi_strlen(node->value));
        if (r != 0) {
            return r;
        }
        r = fn_803936B0(obj, "\r\n", 2);
        if (r != 0) {
            return r;
        }
        NHTTPi_free(node);
        node = (NHTTPHdrNode*)NHTTPi_getHdrFromList((NHTTPLink**)(conn + 0x30));
    }
    return 0;
}

// HYPOTHESIS: header receive buffer; 0x400 inline bytes at +0x38, then a chain of 0x200-byte blocks (next pointer at +0, data at +4) starting at +0x34.
typedef struct NHTTPHdrBlock {
    struct NHTTPHdrBlock* next;
    s8 data[0x200];
} NHTTPHdrBlock;

typedef struct NHTTPHdrRecvBuf {
    // HYPOTHESIS: shared header length; the original reloads it during scanning.
    volatile s32 length;
    u8 pad[0x30];
    NHTTPHdrBlock* blocks;
    s8 inl[0x400];
} NHTTPHdrRecvBuf;

// NHTTPi_skipSpaceHdrRecvBuf
int fn_80390D18(NHTTPHdrRecvBuf* buf, int pos, int end) {
    int off;
    NHTTPHdrBlock* blk;
    int i;
    int c;
    if (pos < end) {
        if (pos < 0x400) {
            off = pos;
            blk = 0;
        } else {
            int n = (pos - 0x400) >> 9;
            blk = buf->blocks;
            while (n != 0) {
                blk = blk->next;
                n--;
            }
            off = (pos - 0x400) & 0x1ff;
        }
        for (i = pos; i < end; i++) {
            if (blk == 0) {
                if (off < 0x400) {
                    c = buf->inl[off++];
                    goto check;
                }
                blk = buf->blocks;
                off = 0;
            } else if (off == 0x200) {
                off = 0;
                blk = blk->next;
            }
            c = (u8)blk->data[off++];
check:
            if ((s8)c != 0x20) {
                return i;
            }
        }
    }
    return -1;
}

// NHTTP_DestroyRequest
void fn_80391784(void* sysv, int reqi) {
    u8* req = (u8*)reqi;
    NHTTPLink* n;
    NHTTPLink* nxt;
    void* m = NHTTPi_GetMutexInfoP(sysv);
    u8* conn = (u8*)NHTTPi_Request2Connection((int)m, reqi);
    if (conn != 0) {
        *(s32*)(conn + 0x14) = 0;
    }
    NHTTPi_free(*(void**)(req + 0x2c));
    conn = (u8*)NHTTPi_Request2Connection((int)m, reqi);
    if (conn != 0) {
        *(s32*)(conn + 0x10) = 0;
    }
    n = *(NHTTPLink**)(req + 0x30);
    while (n != 0) {
        NHTTPLink* head = n->next;
        if (n != head) {
            nxt = head->next;
            NHTTPi_free(head);
            n->next = nxt;
        } else {
            NHTTPi_free(n);
            n = 0;
        }
    }
    {
        NHTTPLink* k = *(NHTTPLink**)(req + 0x34);
        while (k != 0) {
            NHTTPLink* head = k->next;
            if (k != head) {
                NHTTPLink* nx = head->next;
                NHTTPi_free(head);
                k->next = nx;
            } else {
                NHTTPi_free(k);
                k = 0;
            }
        }
    }
    NHTTPi_free(*(void**)(req + 0x24));
    NHTTPi_free(*(void**)(req + 0x28));
    NHTTPi_free(req);
}

int NHTTPi_PostSendCallback(int, void*, void*, int);

// NHTTPi_strToHex
s32 fn_80392994(u8* s, s32 len) {
    s8 raw;
    int t;
    s32 val;
    s32 seen;
    s32 i;
    s8 c;
    if (len > 8) {
        return -1;
    }
    if ((len == 8) & ((s8)*s > 0x37)) {
        return -1;
    }
    val = 0;
    seen = 0;
    for (i = len; i > 0; i--, s++) {
        raw = (s8)*s;
        t = raw;
        if ((raw >= 0x41) & (raw <= 0x5a)) {
            t += 0x20;
        }
        c = (s8)t;
        if (c >= 0x30 && c <= 0x39) {
            val = (c + val * 0x10) - 0x30;
            seen = 1;
        } else if (c >= 0x61 && c <= 0x66) {
            val = (c + val * 0x10) - 0x57;
            seen = 1;
        } else {
            if (seen != 0) {
                if (c == 0x20 || c == 0) {
                    break;
                }
            }
            if (seen != 0 || c != 0x20) {
                return -1;
            }
        }
    }
    return val;
}

int fn_80393350(int, s32*, void*, int*, int);
int NHTTPi_intToStr(char*, u32);
int NHTTPi_SendPostData(int, s32*, u8*, void*, int, s32*, int);

// BYTES: keep each original SDK object's literal pool separate.
#pragma push
#pragma section const_type ".rodata.nhttp_thread"
static const char nhttpThreadConnect[] = "CONNECT ";
static const char nhttpThreadHttpVersion[] = " HTTP/1.1\r\n";
static const char nhttpThreadConnectHeaders[] = "Content-Length: 0\r\nPragma: no-cache\r\n";
static const char nhttpThreadProxyAuthorization[] = "Proxy-Authorization: Basic ";
static const char nhttpThreadContentLength[] = "Content-Length: ";
static const char nhttpThreadAuthorization[] = "Authorization: Basic ";
static const char nhttpThreadContentLengthName[] = "Content-Length";
static const char nhttpThreadConnection[] = "Connection";
static const char nhttpThreadKeepAlive[] = "Keep-Alive";
static const char nhttpThreadTransferEncoding[] = "Transfer-Encoding";
#pragma pop

// NHTTPi_SendProcPostDataRaw
int fn_80393CF4(u8* obj) {
    void* sys = NHTTPi_GetSystemInfoP();
    int mutex = (int)NHTTPi_GetMutexInfoP(sys);
    u8* conn = (u8*)((s32**)NHTTPi_GetReqInfoP(sys))[0][3];
    u8* thr = (u8*)NHTTPi_GetThreadInfoP(sys);
    NHTTPBgnEndInfo* bgn = (NHTTPBgnEndInfo*)NHTTPi_GetBgnEndInfoP(sys);
    int len = 0;
    void* t = thr + 0x360;
    char tmp[12];
    int n;
    int r;
    if (*(s32*)(conn + 0x248) == 0) {
        if (fn_80393350(mutex, (s32*)conn, 0, &len, 0) == 0) {
            return 3;
        }
    } else {
        len = *(s32*)(conn + 0x24c);
    }
    n = NHTTPi_intToStr(tmp, len);
    r = fn_803936B0(obj, (void*)nhttpThreadContentLength, 0x10);
    if (r != 0) {
        return r;
    }
    r = fn_803936B0(obj, tmp, n);
    if (r != 0) {
        return r;
    }
    r = fn_803936B0(obj, "\r\n", 2);
    if (r != 0) {
        return r;
    }
    r = fn_803936B0(obj, "\r\n", 2);
    if (r != 0) {
        return r;
    }
    if (*(s32*)(conn + 0x248) == 0) {
        r = NHTTPi_SendPostData(mutex, (s32*)conn, (u8*)t, 0, bgn->unk_7d0, (s32*)(obj + 0x324), 0);
        if (r != 0) {
            return r;
        }
    } else {
        r = fn_803936B0(obj, *(void**)(conn + 0x248), *(s32*)(conn + 0x24c));
        if (r != 0) {
            return r;
        }
    }
    return 0;
}

// NHTTPi_GetPostContentlength
int fn_80393350(int a, s32* obj, void* data, int* out, int mode) {
    int total = 0;
    u8* req = (u8*)NHTTPi_Request2Connection(a, (int)obj);
    if (req == 0) {
        return 0;
    }
    *(s32*)(req + 0x24) = 0;
    for (;;) {
        int len;
        u8* p;
        if (*obj != 0) {
            return 0;
        }
        *(s32*)(req + 0x28) = 0;
        if (NHTTPi_PostSendCallback(a, req, data, total) < 0) {
            return 0;
        }
        len = *(s32*)(req + 0x28);
        p = *(u8**)(req + 0x24);
        if (len == 0) {
            break;
        }
        if (p == 0) {
            return 0;
        }
        total += len;
        switch (mode) {
        case 0:
        case 1:
            *out += len;
            break;
        case 2:
            *out += fn_80392880((const char*)p, len);
            break;
        }
    }
    return 1;
}

int NHTTPi_getHeaderValue(NHTTPHdrRecvBuf*, const char*, int*);
int NHTTPi_loadFromHdrRecvBuf(void*, void*, int, int);

// NHTTP_GetHeaderField
int fn_80391D8C(u8* resp, const char* name, char** out) {
    int start;
    int n;
    if (*(s32*)(resp + 0x14) == 0) {
        return -1;
    }
    if (*(void**)(resp + 0x24) != 0) {
        NHTTPi_free(*(void**)(resp + 0x24));
        *(void**)(resp + 0x24) = 0;
    }
    n = NHTTPi_getHeaderValue((NHTTPHdrRecvBuf*)resp, name, &start);
    if (n >= 0) {
        char* p = (char*)NHTTPi_alloc(n + 1, 4);
        *(char**)(resp + 0x24) = p;
        if (p == 0) {
            return -1;
        }
        p[n] = 0;
        NHTTPi_loadFromHdrRecvBuf(resp, *(char**)(resp + 0x24), start, n);
        *out = *(char**)(resp + 0x24);
        return n;
    }
    if (NHTTPi_strcmp("HTTPSTATUSCODE", name) == 0) {
        char* p = (char*)NHTTPi_alloc(4, 4);
        *(char**)(resp + 0x24) = p;
        if (p == 0) {
            return -1;
        }
        p[3] = 0;
        NHTTPi_loadFromHdrRecvBuf(resp, *(char**)(resp + 0x24), 9, 3);
        *out = *(char**)(resp + 0x24);
        return 3;
    }
    return -1;
}

// HYPOTHESIS: singly linked list head in lbl_805A1150, next at 0x20; op selects the match key.
int NHTTPi_ControlConnectionList(int sys, int key, int op) {
    u8* found = 0;
    u8* n;
    u8** pp;
    NHTTPi_lockReqList((OSMutex*)sys);
    if ((u32)op == 3) {
        *(u8**)(key + 0x20) = *(u8**)lbl_805A1150;
        *(u8**)lbl_805A1150 = (u8*)key;
        found = (u8*)key;
    } else {
        for (pp = (u8**)lbl_805A1150; (n = *pp) != 0; pp = (u8**)(*pp + 0x20)) {
            switch (op) {
            case 0:
                if ((u32)n == (u32)key) found = n;
                break;
            case 1:
                if (*(u32*)(n + 0x10) == (u32)key) found = n;
                break;
            case 2:
                if (*(u32*)(n + 0x14) == (u32)key) found = n;
                break;
            case 4:
                if ((u32)n == (u32)key) { *pp = *(u8**)(n + 0x20); found = n; }
                break;
            }
            if (found != 0) break;
        }
    }
    NHTTPi_unlockReqList((OSMutex*)sys);
    return (int)found;
}

// NHTTPi_findNextLineHdrRecvBuf
int fn_80390B20(NHTTPHdrRecvBuf* buf, int pos, int end, int* colon, int* kind) {
    int c;
    NHTTPHdrBlock* blk;
    int off;
    int i;
    int ret;
    int cr;
    if (colon != 0) {
        *colon = -1;
    }
    if (pos < end) {
        ret = -1;
        cr = 0;
        if (pos < 0x400) {
            off = pos;
            blk = 0;
        } else {
            int n = (pos - 0x400) >> 9;
            blk = buf->blocks;
            while (n != 0) {
                blk = blk->next;
                n--;
            }
            off = (pos - 0x400) & 0x1ff;
        }
        for (i = pos; i < end; i++) {
            if (blk == 0) {
                if (off < 0x400) {
                    c = (s8)(u8)buf->inl[off++];
                    goto check;
                }
                blk = buf->blocks;
                off = 0;
            } else if (off == 0x200) {
                off = 0;
                blk = blk->next;
            }
            c = (u8)blk->data[off++];
check:
            if ((s8)c == ':') {
                if (colon != 0 && *colon < 0) {
                    *colon = i;
                }
            }
            if (cr) {
                if ((s8)c == '\n') {
                    ret = (i == end - 1) ? 0 : i + 1;
                    if (kind != 0) {
                        *kind = 2;
                    }
                }
                return ret;
            }
            if ((s8)c == '\r') {
                ret = (i == end - 1) ? 0 : i + 1;
                cr = 1;
                if (kind != 0) {
                    *kind = 1;
                }
            }
            if ((s8)c == '\n') {
                ret = (i == end - 1) ? 0 : i + 1;
                if (kind != 0) {
                    *kind = 1;
                }
                return ret;
            }
        }
    }
    return -1;
}

// HYPOTHESIS: first word of the header receive buffer is the number of valid bytes.
int NHTTPi_loadFromHdrRecvBuf(void* bufv, void* dstv, int pos, int len) {
    NHTTPHdrBlock* blk = (NHTTPHdrBlock*)bufv;
    int n;
    if (pos + len <= *(int*)blk) {
        if (len != 0) {
            if (pos < 0x400) {
                n = len > 0x400 - pos ? 0x400 - pos : len;
                NHTTPi_memcpy(dstv, ((NHTTPHdrRecvBuf*)blk)->inl + pos, n);
                pos += n;
                len -= n;
                dstv = (u8*)dstv + n;
            }
            if (len != 0) {
                int k;
                pos -= 0x400;
                blk = ((NHTTPHdrRecvBuf*)blk)->blocks;
                k = pos >> 9;
                pos &= 0x1ff;
                while (k != 0) {
                    blk = blk->next;
                    k--;
                }
                while (len != 0) {
                    n = len > 0x200 - pos ? 0x200 - pos : len;
                    NHTTPi_memcpy(dstv, blk->data + pos, n);
                    pos += n;
                    blk = blk->next;
                    pos &= 0x1ff;
                    len -= n;
                    dstv = (u8*)dstv + n;
                }
            }
        }
        return 1;
    }
    return 0;
}

int fn_803911C8(void*, int, int, int, int, int, int, int);

// HYPOTHESIS: connection record is 0x8060 bytes: request at +0x10, response at +0x14, callback at +0x1c.
int NHTTPCreateConnection(int url, int method, int buffer, int size, int cb, int ctx) {
    u8* conn;
    void* sys;
    void* bgn;
    void* mutex;
    u8* req;
    sys = NHTTPi_GetSystemInfoP();
    bgn = NHTTPi_GetBgnEndInfoP(sys);
    mutex = NHTTPi_GetMutexInfoP(sys);
    conn = (u8*)NHTTPi_alloc(0x8060, 0x20);
    if (conn == 0) {
        NHTTPi_SetError((NHTTPBgnEndInfo*)bgn, 1);
        return 0;
    }
    NHTTPi_memclr(conn, 0x8060);
    req = (u8*)fn_803911C8(bgn, url, method, buffer, size, ctx, 0, 0);
    *(u8**)(conn + 0x10) = req;
    if (req == 0) {
        NHTTPi_free(conn);
        return 0;
    }
    *(s32*)(conn + 0x14) = *(s32*)(req + 0x2c);
    *(s32*)(conn + 0) = 0;
    *(s32*)(conn + 0x1c) = cb;
    *(s32*)(conn + 0x24) = 0;
    *(s32*)(conn + 0x28) = 0;
    *(s32*)(conn + 0x18) = -1;
    NHTTPi_CommitConnectionList((int)mutex, (int)conn);
    *(s32*)(conn + 4) = 0xf;
    *(s32*)(conn + 8) = 0;
    *(s32*)(conn + 0xc) = 1;
    NHTTPi_SetVirtualContentLength((NHTTPVirtLen*)conn, 0);
    *(s32*)(conn + 0x30) = 0;
    *(s32*)(conn + 0x8040) = 0;
    *(s32*)(conn + 0x8044) = 0;
    NHTTPi_memclr(conn + 0x40, 0x8000);
    return (int)conn;
}

int fn_803A88D4(int, int);
int fn_803A9558(int, int);
int fn_803A91DC(int, int, int, int, int);
int fn_803A9348(int, int, int);
int fn_803A9490(int, int);
int fn_803A8A2C(int, int);
int fn_803A8AF4(int);

// HYPOTHESIS: SSL connect step; the SSL handle is at +0xac of the connection record and its options at +0xb0..+0xdc.
#define CF(o) (*(int*)((u8*)connv + (o)))
int NHTTPi_SocSSLConnect(void* sysv, void* av, void* connv, int sock) {
    int done = 0;
    int err;
    u8* req;
    CF(0xac) = fn_803A88D4(CF(0xcc), CF(0x28));
    if (*(void**)((u8*)sysv + 0x7e4) != 0 && CF(0xd4) != 0) {
        ((void (*)(int, int))*(void**)((u8*)sysv + 0x7e4))(CF(0xac), CF(0xd4));
    }
    if (CF(0xc8) == 1) {
        if (fn_803A9558(CF(0xac), CF(0xdc)) != 0) {
            return -0x3ed;
        }
    } else {
        if (CF(0xb0) != 0 && CF(0xb8) != 0) {
            if (fn_803A91DC(CF(0xac), CF(0xb0), CF(0xb4), CF(0xb8), CF(0xbc)) != 0) {
                return -0x3ed;
            }
        }
    }
    if (CF(0xc0) != 0) {
        if (fn_803A9348(CF(0xac), CF(0xc0), CF(0xc4)) != 0) {
            return -0x3ec;
        }
    } else {
        if (fn_803A9490(CF(0xac), CF(0xd8)) != 0) {
            return -0x3ec;
        }
    }
    if (fn_803A8A2C(CF(0xac), sock) < -1) {
        return -0x3e9;
    }
    while (done == 0) {
        req = (u8*)NHTTPi_Request2Connection((int)av, (int)connv);
        err = fn_803A8AF4(CF(0xac));
        NHTTPi_SetSSLError((NHTTPBgnEndInfo*)sysv, err);
        if (req != 0) {
            *(int*)(req + 8) = err;
        }
        switch (err) {
        case 0:
            done = 1;
            break;
        case -2:
        case -3:
        case -7:
            break;
        default:
            return -0x3e9;
        }
    }
    return 0;
}
#undef CF

// HYPOTHESIS: request fields used by the connection thread, verified by its
// host lookup, connection and proxy steps; intervening fields remain unknown.
typedef struct NHTTPThreadRequest {
    s32 cancelled;
    s32 state;
    s32 ssl;
    s32 proxy;
    s32 rawPost;
    s32 hostLength;
    s32 pathOffset;
    s32 method;
    s32 port;
    char* url;
    char* host;
    struct NHTTPThreadResponse* response;
    NHTTPHdrNode* headers;
    NHTTPHdrNode* postData;
    char boundary[0x14];
    char authorization[0x5c];
    s32 authorizationLength;
    s32 sslHandle;
    u8 unkB0[0x20];
    s32 postMode;
    u8 unkD4[0xc];
    char proxyHost[0x100];
    s32 proxyPort;
    char proxyAuth[0x5c];
    s32 proxyAuthLength;
    s32 socketOption;
    void* rawData;
    s32 rawLength;
    int (*postCallback)(void*, void*, void*, int, void*);
} NHTTPThreadRequest;

// HYPOTHESIS: active queue entry layout also follows NHTTPi_setReqQueue.
typedef struct NHTTPActiveRequest {
    struct NHTTPActiveRequest* next;
    struct NHTTPActiveRequest* prev;
    s32 requestId;
    NHTTPThreadRequest* request;
    s32 socket;
} NHTTPActiveRequest;

// HYPOTHESIS: connection-thread state and cached endpoint fields.
typedef struct NHTTPThreadState {
    s32 unk0;
    char host[0x100];
    u8 unk104[0x200];
    char statusLine[0xe];
    u8 unk312[2];
    u32 address;
    u32 previousAddress;
    s32 port;
    s32 previousPort;
    s32 sent;
    s32 received;
    s32 contentLength;
    s32 error;
    s32 unk334;
    s32 reuseConnection;
    s32 chunked;
} NHTTPThreadState;

// HYPOTHESIS: response completion and body ownership fields used at request end.
typedef struct NHTTPThreadResponse {
    // HYPOTHESIS: published by the receive thread and observed by header readers.
    volatile s32 headerLength;
    s32 received;
    s32 totalReceived;
    s32 contentLength;
    s32 completed;
    s32 headerParsed;
    s32 statusCode;
    s32 capacity;
    u8 unk20[8];
    void* body;
    void* (*bufferCallback)(void**, s32*, int, void* (*)(u32, s32), void (*)(void*), void*);
    void* cleanupCallback;
    NHTTPHdrBlock* blocks;
    u8 inlineHeader[0x400];
    void* userContext;
} NHTTPThreadResponse;

// HYPOTHESIS: the public connection exposes status followed by error.
typedef struct NHTTPConnectionStatus {
    s32 status;
    s32 error;
    s32 sslError;
    s32 running;
    NHTTPThreadRequest* request;
    NHTTPThreadResponse* response;
    s32 requestId;
    void* callback;
    struct NHTTPConnectionStatus* next;
    void* postData;
    s32 postLength;
    s32 virtualLength;
    void (*completeCallback)(int, void*, void*);
} NHTTPConnectionStatus;

// HYPOTHESIS: network configuration stores enable flags, host, port and login.
typedef struct NHTTPProxyConfig {
    u8 enabled;
    u8 authenticated;
    u8 unk2[2];
    char host[0x100];
    u16 port;
    char user[0x21];
    char password[0x21];
} NHTTPProxyConfig;

extern char lbl_80491778[];
int fn_80396A74(void*, const char*, int, const char*, const char*);

// NHTTPSetProxyDefault
int fn_80396BE4(int id) {
    const char* base = lbl_80491778;
    void* sys = NHTTPi_GetSystemInfoP();
    NHTTPThreadRequest* req = (NHTTPThreadRequest*)NHTTPi_GetRequest((int)NHTTPi_GetMutexInfoP(sys), id);
    u8* bgn = NHTTPi_GetBgnEndInfoP(sys);
    NHTTPProxyConfig* proxy;
    const char* user;
    const char* password;
    int result;
    if (req->ssl != 0) {
        proxy = (NHTTPProxyConfig*)(bgn + 0x170);
    } else {
        proxy = (NHTTPProxyConfig*)(bgn + 0x28);
    }
    if (proxy->enabled == 1) {
        user = 0;
        password = 0;
        if (proxy->authenticated == 1) {
            user = proxy->user;
            password = proxy->password;
        }
        OSReport(base + 0x21c, proxy->host, proxy->port,
                 user ? user : base + 0x210,
                 password ? password : base + 0x210);
        result = fn_80396A74(req, proxy->host, proxy->port, user, password);
        if (result < 0) {
            OSReport(base + 0x240, result);
            OSPanic(base + 0x25c, 0x37d, base + 0x268);
        } else {
            return 0;
        }
    } else {
        printf(base + 0x278);
    }
    return -1;
}

// NHTTPi_ThreadHostAddrProc
int fn_8039471C(NHTTPThreadState* thread) {
    NHTTPThreadRequest* req = (*(NHTTPActiveRequest**)NHTTPi_GetReqInfoP(NHTTPi_GetSystemInfoP()))->request;
    char* host = req->host;
    if (req->proxy != 0) {
        host = req->proxyHost;
    }
    if (NHTTPi_strlen(host) == 0 || NHTTPi_strcmp(host, thread->host) != 0) {
        thread->address = fn_803926B8(req, host);
        if (thread->address == 0) {
            if (req->proxy != 0) {
                thread->error = 12;
                return 0;
            }
            thread->error = 4;
            return 0;
        }
    } else {
        thread->address = thread->previousAddress;
    }
    NHTTPi_memclr(thread->host, 0x100);
    NHTTPi_memcpy(thread->host, host, NHTTPi_strlen(host));
    thread->port = req->port;
    if (req->proxy != 0) {
        thread->port = req->proxyPort;
    }
    if (thread->address != thread->previousAddress || thread->port != thread->previousPort || req->ssl == 1) {
        thread->reuseConnection = 0;
    }
    thread->previousAddress = thread->address;
    thread->previousPort = thread->port;
    return 1;
}

int fn_8039375C(NHTTPThreadState*);
int fn_80393A04(NHTTPThreadState*);

// NHTTPi_ThreadProxyProc
int fn_803949F0(NHTTPThreadState* thread) {
    void* sys = NHTTPi_GetSystemInfoP();
    NHTTPBgnEndInfo* bgn = NHTTPi_GetBgnEndInfoP(sys);
    NHTTPActiveRequest** active = NHTTPi_GetReqInfoP(sys);
    void* mutex = NHTTPi_GetMutexInfoP(sys);
    NHTTPThreadRequest* req = (*active)->request;
    int result;
    thread->error = 10;
    thread->sent = 0;
    if (req->ssl != 0 && req->proxy != 0) {
        result = fn_8039375C(thread);
        if (result != 0) {
            return result;
        }
        if (fn_80393A04(thread) == 0) {
            return 1;
        }
        result = NHTTPi_SocSSLConnect(bgn, mutex, req, bgn->unk_7d0);
        if (result != 0) {
            if (result == -0x3ec) {
                if (NHTTPi_GetSSLError(bgn) != 0) {
                    thread->error = 16;
                }
                return 1;
            }
            if (result == -0x3ed) {
                if (NHTTPi_GetSSLError(bgn) != 0) {
                    thread->error = 17;
                }
                return 1;
            }
            if (NHTTPi_GetSSLError(bgn) != 0) {
                thread->error = 14;
            }
            return 1;
        }
    }
    return 0;
}

int fn_80392EC4(char*, const char*);

// NHTTPSetProxy
int fn_80396A74(void* id, const char* host, int port, const char* user, const char* password) {
    const char* base = lbl_80491778;
    NHTTPThreadRequest* req;
    int hostLength;
    int userLength;
    int passwordLength;
    char credentials[0x41];
    req = NHTTPi_GetRequest((int)NHTTPi_GetMutexInfoP(NHTTPi_GetSystemInfoP()), (int)id);
    if (req == 0 || host == 0) {
        return -1;
    }
    hostLength = NHTTPi_strlen(host);
    if (hostLength > 0x100) {
        printf(base + 0x1a0);
        return -1;
    }
    NHTTPi_memcpy(req->proxyHost, host, hostLength);
    req->proxyPort = port;
    if (user != 0 && password != 0) {
        userLength = NHTTPi_strlen(user);
        passwordLength = NHTTPi_strlen(password);
        if (userLength > 0x20) {
            printf(base + 0x1c8);
            return -1;
        }
        if (passwordLength > 0x20) {
            printf(base + 0x1ec);
            return -1;
        }
        NHTTPi_memclr(credentials, 0x41);
        NHTTPi_memcpy(credentials, user, userLength);
        NHTTPi_memcpy(credentials + userLength, ":", 1);
        NHTTPi_memcpy(credentials + (userLength + 1), password, passwordLength);
        req->proxyAuthLength = fn_80392EC4(req->proxyAuth, credentials);
    }
    req->proxy = 1;
    return 0;
}

// NHTTPi_ThreadReqEnd
void fn_803944E4(NHTTPThreadState* thread) {
    void* sys = NHTTPi_GetSystemInfoP();
    NHTTPBgnEndInfo* bgn = NHTTPi_GetBgnEndInfoP(sys);
    NHTTPActiveRequest** active = NHTTPi_GetReqInfoP(sys);
    OSMutex* mutex = NHTTPi_GetMutexInfoP(sys);
    NHTTPThreadRequest* req = (*active)->request;
    NHTTPThreadResponse* response = req->response;
    NHTTPConnectionStatus* conn = NHTTPi_Request2Connection((int)mutex, (int)req);
    if (req->cancelled != 0) {
        thread->error = 8;
        thread->reuseConnection = 0;
    }
    if (thread->reuseConnection == 0 && bgn->unk_7d0 >= 0) {
        if (fn_80391F5C(mutex, (u8*)req, bgn->unk_7d0) < 0) {
            thread->error = 10;
        }
        bgn->unk_7d0 = -1;
    }
    if (thread->error == 0) {
        response->completed = 1;
    } else {
        response->completed = 0;
        NHTTPi_SetError(bgn, thread->error);
        if (response->body == thread->unk104) {
            response->body = 0;
            response->capacity = 0;
        }
    }
    if (conn != 0) {
        conn->error = thread->error;
    }
    NHTTPi_lockReqList(mutex);
    NHTTPi_free(*active);
    *active = 0;
    NHTTPi_unlockReqList(mutex);
    fn_80391888(mutex, req);
    if (conn != 0 && response->completed != 0) {
        conn->status = 5;
    }
    fn_803974A4(mutex, conn);
    if (conn != 0) {
        fn_80396E70((u8*)conn);
    }
}

// NHTTPi_ThreadConnectProc
int fn_80394870(NHTTPThreadState* thread) {
    void* sys = NHTTPi_GetSystemInfoP();
    NHTTPBgnEndInfo* bgn = NHTTPi_GetBgnEndInfoP(sys);
    NHTTPActiveRequest** active = NHTTPi_GetReqInfoP(sys);
    NHTTPThreadRequest* req = (*active)->request;
    OSMutex* mutex = NHTTPi_GetMutexInfoP(sys);
    if (thread->reuseConnection == 0) {
        if (bgn->unk_7d0 >= 0) {
            if (fn_80391F5C(mutex, (u8*)req, bgn->unk_7d0) < 0) {
                bgn->unk_7d0 = -1;
                thread->error = 10;
                return 0;
            }
        }
        bgn->unk_7d0 = fn_80391ED0(req);
        if (bgn->unk_7d0 < 0) {
            thread->error = 3;
            return 0;
        }
        NHTTPi_lockReqList(mutex);
        (*active)->socket = bgn->unk_7d0;
        NHTTPi_unlockReqList(mutex);
        if (req->cancelled != 0) {
            return 0;
        }
        if (fn_80391FC8(bgn, mutex, (s32*)req, bgn->unk_7d0, thread->address, thread->port) < 0) {
            if (req->proxy != 0) {
                thread->error = 13;
                return 0;
            }
            if (NHTTPi_GetSSLError(bgn) != 0) {
                thread->error = 14;
                return 0;
            }
            thread->error = 5;
            return 0;
        }
    } else {
        NHTTPi_lockReqList(mutex);
        (*active)->socket = bgn->unk_7d0;
        NHTTPi_unlockReqList(mutex);
    }
    return 1;
}

// HYPOTHESIS: callback arguments carry a body pointer and two word-sized
// lengths/flags, with the post-data offset in the fourth word.
typedef struct NHTTPCallbackData {
    void* body;
    s32 length;
    s32 unk8;
    s32 offset;
} NHTTPCallbackData;

static inline int nhttpInvokePostCallback(int mutex, NHTTPConnectionStatus* conn, NHTTPCallbackData* data) {
    if (conn != 0) {
        NHTTPThreadRequest* req = NHTTPi_Connection2Request(mutex, (int)conn);
        if (req != 0) {
            int (*callback)(void*, void*, void*, int, void*) = req->postCallback;
            if (callback != 0) {
                return callback(data->body, &data->length, &data->unk8, data->offset, fn_80396188((int)conn));
            }
        }
    }
    return -1;
}

static inline void nhttpInvokeBufferCallback(int mutex, NHTTPConnectionStatus* conn, NHTTPCallbackData* data) {
    if (conn != 0) {
        NHTTPThreadResponse* response = NHTTPi_Connection2Response(mutex, (int)conn);
        if (response != 0) {
            void* (*callback)(void**, s32*, int, void* (*)(u32, s32), void (*)(void*), void*) = response->bufferCallback;
            if (callback != 0) {
                void* body = data->body;
                int length = NHTTPi_GetVirtualContentLength((NHTTPVirtLen*)conn);
                data->body = callback(&body, &data->length, length, NHTTPi_alloc, NHTTPi_free, fn_80396188((int)conn));
                if (data->body != 0 && body != 0) {
                    data->unk8 = 0;
                }
            }
        }
    }
}

static inline void nhttpInvokeCompleteCallback(int mutex, NHTTPConnectionStatus* conn) {
    if (conn != 0) {
        void (*callback)(int, void*, void*) = conn->completeCallback;
        if (callback != 0) {
            NHTTPThreadResponse* response = NHTTPi_Connection2Response(mutex, (int)conn);
            if (response != 0) {
                int error = NHTTPGetConnectionError((int)conn);
                callback(error, response, fn_80396188((int)conn));
            }
        }
    }
}

int NHTTPi_TemplateConnectionCallback(int id, int event, NHTTPCallbackData* data) {
    int mutex = (int)NHTTPi_GetMutexInfoP(NHTTPi_GetSystemInfoP());
    NHTTPConnectionStatus* conn = NHTTPi_GetConnection(mutex, id);
    int result = 0;
    switch (event) {
    case 1:
        result = nhttpInvokePostCallback(mutex, conn, data);
        break;
    case 2:
        nhttpInvokeBufferCallback(mutex, conn, data);
        result = 0;
        break;
    case 4:
        nhttpInvokeCompleteCallback(mutex, conn);
        result = 0;
        break;
    }
    return result;
}

// NHTTPi_RecvProxyConnectHeader
int fn_80393A04(NHTTPThreadState* thread) {
    void* sys = NHTTPi_GetSystemInfoP();
    NHTTPActiveRequest** active = NHTTPi_GetReqInfoP(sys);
    u8* threadInfo = NHTTPi_GetThreadInfoP(sys);
    NHTTPBgnEndInfo* bgn = NHTTPi_GetBgnEndInfoP(sys);
    void* mutex = NHTTPi_GetMutexInfoP(sys);
    u8* scratch = threadInfo + 0x360;
    NHTTPThreadRequest* req = (*active)->request;
    NHTTPThreadResponse* response = req->response;
    int success = 0;
    int length = 0;
    char header[0x200];
    for (;;) {
        int count = NHTTPi_SocRecv(mutex, req, bgn->unk_7d0, header + length, 0x200 - length, 0);
        char* p;
        int i;
        int ended;
        length += count;
        response->statusCode = NHTTPi_strToInt((u8*)(header + 9), 3);
        if (fn_80392744((char*)header, "HTTP/", 5) == 0 && header[8] == ' ' && response->statusCode == 200) {
            success = 1;
        }
        i = 0;
        ended = 0;
        for (p = header; i < length; p++, i++) {
            if (i > 1 && p[-1] == '\r' && p[0] == '\r') {
                ended = 1;
            } else if (i > 1 && p[-1] == '\n' && p[0] == '\n') {
                ended = 1;
            } else if (i > 3 && p[-3] == '\r' && p[-2] == '\n' && p[-1] == '\r' && p[0] == '\n') {
                ended = 1;
            }
        }
        if (ended != 0) {
            return success != 0;
        }
        if (count < 0) {
            return 0;
        }
        if (length < 0x200) {
            continue;
        }
        count = NHTTPi_SocRecv(mutex, req, bgn->unk_7d0, scratch, 1, 0);
        if (count < 0) {
            return 0;
        }
        if (count != 0) {
            return 0;
        }
    }
}

#pragma push
#pragma section const_type ".rodata.nhttp_post"
static const char nhttpPostDisposition[] = "Content-Disposition: form-data; name=\"";
static const char nhttpPostBinaryType[] = "Content-Type: application/octet-stream\r\nContent-Transfer-Encoding: binary\r\n";
static const char nhttpPostEncodedType[] = "Content-Type: application/x-www-form-urlencoded\r\n";
static const char nhttpPostMultipartType[] = "Content-Type: multipart/form-data; boundary=";
#pragma pop

// NHTTPi_SendProcPostDataAscii
int fn_80394220(NHTTPThreadState* thread) {
    void* sys = NHTTPi_GetSystemInfoP();
    int mutex = (int)NHTTPi_GetMutexInfoP(sys);
    NHTTPThreadRequest* req = (*(NHTTPActiveRequest**)NHTTPi_GetReqInfoP(sys))->request;
    u8* threadInfo = NHTTPi_GetThreadInfoP(sys);
    NHTTPBgnEndInfo* bgn = NHTTPi_GetBgnEndInfoP(sys);
    int length = 0;
    u8* scratch = threadInfo + 0x360;
    NHTTPHdrNode* node = req->postData;
    char buffer[12];
    int digits;
    int result;
    while (node != 0) {
        int encoded = fn_80392810(node->name);
        length += encoded;
        length++;
        if (node->value == 0) {
            if (fn_80393350(mutex, (s32*)req, (void*)node->name, &length, 2) == 0) {
                return 3;
            }
        } else {
            length += fn_80392810(node->value);
        }
        if (node == req->postData->next) {
            break;
        }
        length++;
        node = node->prev;
    }
    digits = NHTTPi_intToStr(buffer, length);
    result = fn_803936B0((u8*)thread, (void*)nhttpPostEncodedType, 0x31);
    if (result != 0) {
        return result;
    }
    result = fn_803936B0((u8*)thread, (void*)nhttpThreadContentLength, 0x10);
    if (result != 0) {
        return result;
    }
    result = fn_803936B0((u8*)thread, buffer, digits);
    if (result != 0) {
        return result;
    }
    result = fn_803936B0((u8*)thread, "\r\n", 2);
    if (result != 0) {
        return result;
    }
    result = fn_803936B0((u8*)thread, "\r\n", 2);
    if (result != 0) {
        return result;
    }
    node = req->postData;
    while (node != 0) {
        int i;
        for (i = 0; (s8)node->name[i] != 0; i++) {
            digits = NHTTPi_encodeUrlChar((u8*)buffer, node->name[i]);
            result = fn_803936B0((u8*)thread, buffer, digits);
            if (result != 0) {
                return result;
            }
        }
        result = fn_803936B0((u8*)thread, "=", 1);
        if (result != 0) {
            return result;
        }
        if (node->value == 0) {
            result = NHTTPi_SendPostData(mutex, (s32*)req, scratch, (void*)node->name, bgn->unk_7d0, &thread->sent, 2);
            if (result != 0) {
                return result;
            }
        } else {
            for (i = 0; (s8)node->value[i] != 0; i++) {
                digits = NHTTPi_encodeUrlChar((u8*)buffer, node->value[i]);
                result = fn_803936B0((u8*)thread, buffer, digits);
                if (result != 0) {
                    return result;
                }
            }
        }
        if (node == req->postData->next) {
            break;
        }
        result = fn_803936B0((u8*)thread, "&", 1);
        if (result != 0) {
            return result;
        }
        node = node->prev;
    }
    return 0;
}

static inline void nhttpWaitConnection(NHTTPConnectionStatus* conn) {
    void* waitSys = NHTTPi_GetSystemInfoP();
    void* waitThread = NHTTPi_GetThreadInfoP(waitSys);
    NHTTPConnectionStatus* waiting = NHTTPi_GetConnection((int)NHTTPi_GetMutexInfoP(waitSys), (int)conn);
    if (waiting != 0) {
        NHTTPConnectionStatus* found = NHTTPi_GetConnection((int)NHTTPi_GetMutexInfoP(NHTTPi_GetSystemInfoP()), (int)conn);
        int status;
        if (found == 0) {
            status = -1;
        } else {
            status = found->status;
        }
        fn_80390A64(waitThread, 1);
        if (status != -1 && status != 0) {
            int error;
            found = NHTTPi_GetConnection((int)NHTTPi_GetMutexInfoP(NHTTPi_GetSystemInfoP()), (int)conn);
            if (found != 0) {
                error = found->error;
            } else {
                error = -1;
            }
            if (error == 15) {
                fn_80396DE8((u8*)waiting);
            }
        }
    }
}

// NHTTPDeleteConnection
int fn_80395E04(int id) {
    void* sys = NHTTPi_GetSystemInfoP();
    int mutex = (int)NHTTPi_GetMutexInfoP(sys);
    void* thread = NHTTPi_GetThreadInfoP(sys);
    NHTTPConnectionStatus* conn = NHTTPi_GetConnection(mutex, id);
    if (conn == 0) {
        return -1;
    }
    fn_80390A64(thread, 1);
    if (conn->requestId >= 0) {
        NHTTP_CancelRequestAsync(sys, conn->requestId);
        conn->requestId = -1;
    }
    if (conn->request != 0 && conn->request->state == 0) {
        fn_80391784(sys, (int)conn->request);
    }
    if (conn->response != 0) {
        if (conn->request == 0) {
            fn_80391B8C((void*)mutex, (int)conn->response);
        } else {
            nhttpWaitConnection(conn);
            fn_80391B8C((void*)mutex, (int)conn->response);
        }
    }
    NHTTPi_OmitConnectionList(mutex, (int)conn);
    NHTTPi_free(conn);
    return 0;
}

int fn_803A8BA0(int, void*, unsigned int);
struct NHTTPBufferedConnection;
int fn_80392238(struct NHTTPBufferedConnection*, int, void*, unsigned int, int);

int SORecv(int, void*, unsigned int, int);

// NHTTPi_SocRecv_sub. The connection owns a buffered socket read.
// HYPOTHESIS: the public connection is followed by its aligned socket buffer.
typedef struct NHTTPBufferedConnection {
    NHTTPConnectionStatus status;
    u8 unk34[12];
    u8 buffer[0x8000];
    s32 offset;
    u32 available;
} NHTTPBufferedConnection;

int fn_80392238(NHTTPBufferedConnection* state, int socket, void* output, unsigned int length, int flags) {
    u8* buffer = state->buffer;
    int result = 0;
    if ((int)length > 0) {
        if (state->available == 0) {
            int count = SORecv(socket, buffer, 0x8000, flags);
            if (count > 0) {
                state->available = count;
                state->offset = 0;
            } else {
                return count;
            }
        }
        if (state->available != 0) {
            if (length > state->available) {
                length = state->available;
            }
            {
                int offset = state->offset;
                NHTTPi_memcpy(output, buffer + offset, length);
            }
            if ((state->available -= length) == 0) {
                NHTTPi_memclr(buffer, 0x8000);
                state->offset = 0;
            } else {
                state->offset += length;
            }
            result = length;
        }
    }
    return result;
}

int NHTTPi_SocRecv(int mutex, NHTTPThreadRequest* req, int socket, void* buffer, unsigned int length, int flags) {
    int result;
    if (req->sslHandle > 0) {
        result = fn_803A8BA0(req->sslHandle, buffer, length);
    } else {
        void* conn = NHTTPi_Request2Connection(mutex, (int)req);
        if (conn != 0) {
            result = fn_80392238((NHTTPBufferedConnection*)conn, socket, buffer, length, flags);
        } else {
            return -1001;
        }
    }
    if (result < 0) {
        if (req->cancelled != 0) return -1002;
        if (req->sslHandle > 0) {
            if (result == -7 || result == -6) return 0;
        } else if (result == -56) return 0;
        return -1001;
    }
    return result;
}

int NHTTPi_PostSendCallback(int mutex, void* connection, void* body, int offset) {
    void* postData;
    int postLength;
    int result = -1;
    NHTTPConnectionStatus* conn = connection;
    if (NHTTPi_ControlConnectionList(mutex, (int)conn, 0) != 0) {
        NHTTPConnectionStatus* found = (void*)NHTTPi_ControlConnectionList(mutex, (int)conn, 0);
        NHTTPThreadResponse* response = found != 0 ? found->response : 0;
        if (response != 0 && conn->callback != 0) {
            NHTTPCallbackData data;
            data.body = body;
            data.length = (int)conn->postData;
            data.unk8 = conn->postLength;
            data.offset = offset;
            result = ((int (*)(void*, int, void*))conn->callback)(conn, 1, &data);
            postLength = data.unk8;
            postData = (void*)data.length;
            found = (void*)NHTTPi_ControlConnectionList(mutex, (int)conn, 0);
            if (found != 0) {
                found->postData = postData;
                found->postLength = postLength;
            }
        }
    }
    return result;
}

void NHTTPi_BufferFullCallback(int mutex, void* connection) {
    void* body;
    int length;
    int received;
    NHTTPConnectionStatus* conn = connection;
    if (NHTTPi_ControlConnectionList(mutex, (int)conn, 0) != 0) {
        NHTTPConnectionStatus* found = (void*)NHTTPi_ControlConnectionList(mutex, (int)conn, 0);
        NHTTPThreadResponse* response = found != 0 ? found->response : 0;
        if (response != 0 && conn->callback != 0) {
            NHTTPCallbackData data;
            data.body = response->body;
            data.length = response->capacity;
            data.unk8 = response->received;
            ((int (*)(void*, int, void*))conn->callback)(conn, 2, &data);
            received = data.unk8;
            length = data.length;
            body = data.body;
            found = (void*)NHTTPi_ControlConnectionList(mutex, (int)conn, 0);
            if (found != 0) {
                found = (void*)NHTTPi_ControlConnectionList(mutex, (int)found, 0);
                response = found != 0 ? found->response : 0;
                if (response != 0) {
                    response->body = body;
                    response->capacity = length;
                    response->received = received;
                }
            }
        }
    }
}

void fn_8039738C(int mutex, void* connection) {
    void* body;
    int length;
    int received;
    NHTTPConnectionStatus* conn = connection;
    if (NHTTPi_ControlConnectionList(mutex, (int)conn, 0) != 0) {
        NHTTPConnectionStatus* found = (void*)NHTTPi_ControlConnectionList(mutex, (int)conn, 0);
        NHTTPThreadResponse* response = found != 0 ? found->response : 0;
        if (response != 0 && conn->callback != 0) {
            NHTTPCallbackData data;
            data.body = response->body;
            data.length = response->capacity;
            data.unk8 = response->received;
            ((int (*)(void*, int, void*))conn->callback)(conn, 3, &data);
            received = data.unk8;
            length = data.length;
            body = data.body;
            found = (void*)NHTTPi_ControlConnectionList(mutex, (int)conn, 0);
            if (found != 0) {
                found = (void*)NHTTPi_ControlConnectionList(mutex, (int)found, 0);
                response = found != 0 ? found->response : 0;
                if (response != 0) {
                    response->body = body;
                    response->capacity = length;
                    response->received = received;
                }
            }
        }
    }
}

int NHTTPi_intToStr(char* out, u32 value) {
    u32 powers[9] = {1000000000, 100000000, 10000000, 1000000, 100000, 10000, 1000, 100, 10};
    u32* power = powers;
    int i;
    int started;
    int count;
    count = 0;
    started = 0;
    for (i = 0; i < 9; power++, i++) {
        if (value >= *power) {
            u32 digit = value / *power;
            value %= *power;
            started = 1;
            out[count++] = digit + '0';
        } else if (started != 0) {
            out[count++] = '0';
        }
    }
    out[count] = value + '0';
    return count + 1;
}

int fn_80392EC4(char* out, const char* input) {
    const char* table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    char* dest = out;
    int length = strlen(input);
    int i;
    for (i = 0; i < length; i += 3) {
        dest[0] = table[input[0] >> 2];
        dest[1] = table[((input[0] & 3) << 4) + (input[1] >> 4)];
        dest[2] = table[((input[1] & 15) << 2) + (input[2] >> 6)];
        dest[3] = table[input[2] & 63];
        input += 3;
        dest += 4;
    }
    if (i == length + 1) dest[-1] = '=';
    else if (i == length + 2) {
        dest[-2] = '=';
        dest[-1] = '=';
    }
    *dest = 0;
    return strlen(out);
}

int fn_80394B44(NHTTPThreadState*);
int fn_80394FE0(NHTTPThreadState*);
int fn_803951CC(NHTTPThreadState*);
int fn_803954C8(NHTTPThreadState*);
void NHTTPi_CommThreadProcMain(void* arg) {
    void* sys = NHTTPi_GetSystemInfoP();
    NHTTPBgnEndInfo* bgn = NHTTPi_GetBgnEndInfoP(sys);
    NHTTPActiveRequest** active = NHTTPi_GetReqInfoP(sys);
    NHTTPThreadState thread;
    thread.unk0 = -1;
    NHTTPi_memclr(thread.host, 0x100);
    NHTTPi_memclr(thread.unk104, 0x200);
    thread.address = -1;
    thread.previousAddress = -1;
    thread.sent = 0;
    thread.reuseConnection = 0;
    thread.chunked = 0;
    thread.unk334 = 0;
    thread.contentLength = 0;
    thread.error = 0;
    while (!bgn->shutdown) {
        if (thread.unk334 == 0) {
            if (!fn_80394664((s32*)&thread)) continue;
            if ((*active)->request->cancelled) {
                fn_803944E4(&thread);
                continue;
            }
            if (!fn_8039471C(&thread)) {
                fn_803944E4(&thread);
                continue;
            }
        }
        if (thread.unk334 == 1) thread.unk334 = 0;
        if (!fn_80394870(&thread)) {
            fn_803944E4(&thread);
            continue;
        }
        switch (fn_803949F0(&thread)) {
        case 0:
            break;
        case 2:
            thread.unk334 = 1;
            continue;
        case 1:
            fn_803944E4(&thread);
            continue;
        }
        switch (fn_80394B44(&thread)) {
        case 0:
            break;
        case 2:
            thread.unk334 = 1;
            continue;
        case 1:
        case 3:
            fn_803944E4(&thread);
            continue;
        }
        if ((*active)->request->cancelled) {
            fn_803944E4(&thread);
            continue;
        }
        if (!fn_80394FE0(&thread)) {
            fn_803944E4(&thread);
            continue;
        }
        if (!fn_803951CC(&thread)) {
            fn_803944E4(&thread);
            continue;
        }
        fn_803954C8(&thread);
        fn_803944E4(&thread);
    }
}

int fn_80390E10(NHTTPHdrRecvBuf*, int, int, const char*, int);

int NHTTPi_getHeaderValue(NHTTPHdrRecvBuf* buf, const char* name, int* start) {
    int colon;
    int kind = 0;
    int pos;
    int next;
    pos = fn_80390B20(buf, 12, buf->length, &colon, &kind);
    while (pos > 0) {
        next = fn_80390B20(buf, pos, buf->length, &colon, &kind);
        if (colon > 0 && fn_80390E10(buf, pos, colon, name, 0) == 0) {
            if (colon + 1 < buf->length) {
                next = fn_80390B20(buf, colon + 1, buf->length, 0, &kind);
                if (next <= 0) {
                    next = buf->length;
                } else {
                    if (next < kind) return -1;
                    next -= kind;
                }
                {
                    pos = fn_80390D18(buf, colon + 1, next);
                    if (pos < 0) pos = next;
                    *start = pos;
                    return next - pos;
                }
            }
            return 0;
        }
        pos = next;
    }
    return -1;
}

int NHTTPi_SendPostData(int mutex, s32* req, u8* buffer, void* body, int socket, s32* fill, int mode) {
    int result;
    char* cursor;
    int offset;
    unsigned int length;
    unsigned int i;
    NHTTPConnectionStatus* conn;
    offset = 0;
    conn = NHTTPi_Request2Connection(mutex, (int)req);
    if (conn == 0) return 3;
    conn->postData = 0;
    for (;;) {
        u8* data;

        if (*req != 0) return 3;
        conn->postLength = 0;
        if (NHTTPi_PostSendCallback(mutex, conn, body, offset) < 0) return 3;
        length = conn->postLength;
        data = conn->postData;
        if (length == 0) break;
        if (data == 0) return 3;
        offset += length;
        switch (mode) {
        case 0:
        case 1:
            result = fn_80393270(req, buffer, socket, fill, data, length);
            if (result < 0) return 1;
            if (result == 0) return 2;
            break;
        case 2:
            {
                cursor = (char*)data;
                for (i = 0; i < length; i++, cursor++) {
                    u8 encoded[3];
                    int count;
                    NHTTPi_memclr(encoded, 3);
                    count = NHTTPi_encodeUrlChar(encoded, *cursor);
                    result = fn_80393270(req, buffer, socket, fill, encoded, count);
                    if (result < 0) return 1;
                    if (result == 0) return 2;
                }
            }
            break;
        }
    }
    return 0;
}

// NHTTP_AddPostDataAscii checks that the multipart delimiter is absent from data.
int NHTTP_AddPostDataAscii(NHTTPThreadRequest* req, void* bgn, const char* name, const char* value) {
    int result = 0;
    int length = 0;
    int initial;
    int c;
    int i;
    int unique;
    if (req->state != 0) return 0;
    if (req->rawPost != 0) {
        printf("already called NHTTPAddPostDataRaw (exclusive fucntion)\n");
        return 0;
    }
    if (value != 0) length = NHTTPi_strlen(value);
    if (fn_80392E18(value, length, req->boundary + 2, 18) < 0) {
        unique = 1;
    } else {
        for (i = 19; i >= 2; i--) {
            initial = "--t9Sf4yfjf1RtvDu3AA"[i];
            c = (u8)req->boundary[i];
            do {
                unsigned char next = (u8)c + 1;
                if (next == 'z' + 1) next = '0';
                else if (next == 'Z' + 1) next = 'a';
                else if (next == '9' + 1) next = 'A';
                c = (s8)next;
                req->boundary[i] = next;
                if (c == initial) break;
                if (fn_80392E18(value, length, req->boundary + 2, 18) < 0) {
                    unique = 1;
                    goto checked;
                }
            } while (1);
        }
        unique = 0;
    }
checked:
    if (unique != 0) {
        result = addHdrList(&req->postData, bgn, name, value);
        if (result != 0) req->postData->next->extra = length;
    }
    return result;
}

int fn_80390E10(NHTTPHdrRecvBuf* buf, int pos, int end, const char* token, int delimiter) {
    NHTTPHdrBlock* blk;
    int off;
    int c;
    if (pos < end) {
        if (pos < 0x400) {
            off = pos;
            blk = 0;
        } else {
            int n = (pos - 0x400) >> 9;
            blk = buf->blocks;
            while (n != 0) {
                blk = blk->next;
                n--;
            }
            off = (pos - 0x400) & 0x1ff;
        }
        if (blk == 0) {
            if (off < 0x400) {
                c = buf->inl[off++];
                goto compare;
            }
            blk = buf->blocks;
            off = 0;
        } else if (off == 0x200) {
            off = 0;
            blk = blk->next;
        }
        c = (u8)blk->data[off++];
compare:
        while (nhttpLowerChar((s8)c) == nhttpLowerChar(*token)) {
            if (*token == 0 || *token == 0x20 || *token == (s8)delimiter || pos == end - 1) return 0;
            if (blk == 0) {
                if (off < 0x400) {
                    c = buf->inl[off++];
                    goto advance;
                }
                blk = buf->blocks;
                off = 0;
            } else if (off == 0x200) {
                off = 0;
                blk = blk->next;
            }
            c = (u8)blk->data[off++];
advance:
            pos++;
            token++;
        }
    }
    return -1;
}

// NHTTPi_ThreadRecvHeaderProc
int fn_80394FE0(NHTTPThreadState* thread) {
    NHTTPThreadRequest* req;
    NHTTPThreadResponse* response;
    int mutex;
    NHTTPConnectionStatus* conn;
    NHTTPBgnEndInfo* bgn;
    int offset;
    s8* dest;
    NHTTPHdrBlock* block;
    int count;
    void* sys;
    u8 recent[4];
    sys = NHTTPi_GetSystemInfoP();
    req = (*(NHTTPActiveRequest**)NHTTPi_GetReqInfoP(sys))->request;
    response = req->response;
    mutex = (int)NHTTPi_GetMutexInfoP(sys);
    conn = NHTTPi_Request2Connection(mutex, (int)req);
    bgn = NHTTPi_GetBgnEndInfoP(sys);
    // BYTES: the original clears this four-byte ring with one word store.
    *(u32*)recent = 0;
    if (conn != 0) conn->status = 3;
    response->headerLength = 0;
    NHTTPi_memclr(thread->statusLine, 14);
    block = response->blocks;
    thread->received = 0;
    for (;;) {

        if (req->cancelled != 0) return 0;
        if (thread->received < 0x400) {
            count = NHTTPi_SocRecv(mutex, req, bgn->unk_7d0,
                response->inlineHeader + thread->received, 1, 0);
            recent[thread->received & 3] = response->inlineHeader[thread->received];
        } else {
            offset = thread->received & 0x1ff;
            if (offset == 0) {
                if (block != 0) {
                    block->next = NHTTPi_alloc(sizeof(NHTTPHdrBlock), 4);
                    block = block->next;
                } else {
                    block = NHTTPi_alloc(sizeof(NHTTPHdrBlock), 4);
                    response->blocks = block;
                }
                if (block == 0) {
                    thread->error = 1;
                    return 0;
                }
                block->next = 0;
            }
            dest = block->data + offset;
            count = NHTTPi_SocRecv(mutex, req, bgn->unk_7d0, dest, 1, 0);
            recent[thread->received & 3] = *dest;
        }
        if (count <= 0) {
            thread->error = 10;
            return 0;
        }
        thread->received += count;
        if (fn_803931A8(recent, thread->received) != 0) break;
    }
    response->headerLength = thread->received;
    if (response->headerLength == 0) {
        thread->error = 7;
        return 0;
    }
    return 1;
}

// NHTTPi_ThreadParseHeaderProc
int fn_803951CC(NHTTPThreadState* thread) {
    NHTTPThreadResponse* response;
    NHTTPThreadRequest* req;
    void* sys;
    u8* scratch;
    int colon;
    int start;
    int length;
    sys = NHTTPi_GetSystemInfoP();
    req = (*(NHTTPActiveRequest**)NHTTPi_GetReqInfoP(sys))->request;
    response = req->response;
    scratch = (u8*)NHTTPi_GetThreadInfoP(sys) + 0x360;
    if (NHTTPi_loadFromHdrRecvBuf(response, thread->statusLine, 0, 14) == 0) {
        thread->error = 7;
        return 0;
    }
    if (fn_80392744(thread->statusLine, "HTTP/", 5) != 0) {
        thread->error = 7;
        return 0;
    }
    if (thread->statusLine[8] != ' ') {
        thread->error = 7;
        return 0;
    }
    response->statusCode = NHTTPi_strToInt((u8*)thread->statusLine + 9, 3);
    if (response->statusCode < 0) {
        thread->error = 7;
        return 0;
    }
    if (fn_80390B20((NHTTPHdrRecvBuf*)response, 12, response->headerLength, &colon, 0) < 0) {
        thread->error = 7;
        return 0;
    }
    length = NHTTPi_getHeaderValue((NHTTPHdrRecvBuf*)response, nhttpThreadContentLengthName, &start);
    thread->contentLength = length;
    if (length == 0) {
        thread->error = 0;
        return 0;
    }
    if (length > 0x100) {
        thread->error = 7;
        return 0;
    }
    if (length > 0) {
        if (NHTTPi_loadFromHdrRecvBuf(response, scratch, start, length) == 0) {
            thread->error = 7;
            return 0;
        }
        thread->contentLength = NHTTPi_strToInt(scratch, thread->contentLength);
        if (thread->contentLength < 0) {
            thread->error = 7;
            return 0;
        }
        response->contentLength = thread->contentLength;
    } else {
        response->contentLength = -1;
    }
    if (req->ssl != 0) {
        thread->reuseConnection = 0;
    } else {
        length = NHTTPi_getHeaderValue((NHTTPHdrRecvBuf*)response, nhttpThreadConnection, &start);
        if (length == 0) {
            thread->error = 7;
            thread->reuseConnection = 0;
            return 0;
        }
        if (length > 0x100) {
            thread->reuseConnection = 0;
        } else if (length > 0) {
            int keep = 0;
            if (fn_80390E10((NHTTPHdrRecvBuf*)response, start, start + length, nhttpThreadKeepAlive, 0) == 0) {
                keep = 1;
            }
            thread->reuseConnection = keep;
        } else {
            thread->reuseConnection = 0;
        }
    }
    length = NHTTPi_getHeaderValue((NHTTPHdrRecvBuf*)response, nhttpThreadTransferEncoding, &start);
    thread->chunked = length;
    if (length == 0) {
        thread->error = 7;
        return 0;
    }
    if (length > 0x100) {
        thread->chunked = 0;
    } else {
        thread->chunked = length > 0 ? fn_80390E10((NHTTPHdrRecvBuf*)response, start, start + length, "chunked", ';') == 0 : 0;
    }
    thread->error = 0;
    response->headerParsed = 1;
    return 1;
}

static inline int nhttpSendProxyAuthorization(NHTTPThreadState* thread) {
    NHTTPThreadRequest* req = (*(NHTTPActiveRequest**)NHTTPi_GetReqInfoP(NHTTPi_GetSystemInfoP()))->request;
    int result;
    if (req->proxyAuthLength == 0) return 0;
    result = fn_803936B0((u8*)thread, (void*)nhttpThreadProxyAuthorization, 27);
    if (result != 0) return result;
    result = fn_803936B0((u8*)thread, req->proxyAuth, req->proxyAuthLength);
    if (result != 0) return result;
    result = fn_803936B0((u8*)thread, "\r\n", 2);
    return result != 0 ? result : 0;
}

// NHTTPi_SendProxyConnectMethod
int fn_8039375C(NHTTPThreadState* thread) {
    void* sys = NHTTPi_GetSystemInfoP();
    NHTTPThreadRequest* req = (*(NHTTPActiveRequest**)NHTTPi_GetReqInfoP(sys))->request;
    u8* threadInfo = NHTTPi_GetThreadInfoP(sys);
    NHTTPBgnEndInfo* bgn = NHTTPi_GetBgnEndInfoP(sys);
    u8* scratch = threadInfo + 0x360;
    char port[12];
    int digits = NHTTPi_intToStr(port, req->port);
    int result;
    result = fn_803936B0((u8*)thread, (void*)nhttpThreadConnect, 8);
    if (result != 0) return result;
    result = fn_803936B0((u8*)thread, req->url + 8, req->hostLength - 8);
    if (result != 0) return result;
    result = fn_803936B0((u8*)thread, ":", 1);
    if (result != 0) return result;
    result = fn_803936B0((u8*)thread, port, digits);
    if (result != 0) return result;
    result = fn_803936B0((u8*)thread, (void*)nhttpThreadHttpVersion, 11);
    if (result != 0) return result;
    result = fn_803936B0((u8*)thread, "Host: ", 6);
    if (result != 0) return result;
    result = fn_803936B0((u8*)thread, req->url + 8, req->hostLength - 8);
    if (result != 0) return result;
    result = fn_803936B0((u8*)thread, ":", 1);
    if (result != 0) return result;
    result = fn_803936B0((u8*)thread, port, digits);
    if (result != 0) return result;
    result = fn_803936B0((u8*)thread, "\r\n", 2);
    if (result != 0) return result;
    result = fn_803936B0((u8*)thread, (void*)nhttpThreadConnectHeaders, 37);
    if (result != 0) return result;
    result = nhttpSendProxyAuthorization(thread);
    if (result != 0) return result;
    fn_803936B0((u8*)thread, "\r\n", 2);
    if (thread->sent > 0) {
        result = fn_803925B8((u8*)req, bgn->unk_7d0, (char*)scratch, thread->sent, 0);
        if (result < 0) return 1;
        if (result == 0) return 2;
    }
    thread->sent = 0;
    NHTTPi_memclr(scratch, 0x100);
    return 0;
}

// NHTTPi_SendProcPostDataBinary
int fn_80393E7C(NHTTPThreadState* thread) {
    void* sys = NHTTPi_GetSystemInfoP();
    int mutex = (int)NHTTPi_GetMutexInfoP(sys);
    NHTTPThreadRequest* req = (*(NHTTPActiveRequest**)NHTTPi_GetReqInfoP(sys))->request;
    u8* threadInfo = NHTTPi_GetThreadInfoP(sys);
    NHTTPBgnEndInfo* bgn = NHTTPi_GetBgnEndInfoP(sys);
    int length = 0;
    u8* scratch = threadInfo + 0x360;
    NHTTPHdrNode* node = req->postData;
    char buffer[12];
    int digits;
    int result;
    while (node != 0) {
        length += 22;
        length += NHTTPi_strlen(node->name) + 41;
        if (node->extra2 != 0) length += 75;
        length += 2;
        if (node->value == 0) {
            if (fn_80393350(mutex, (s32*)req, (void*)node->name, &length, 1) == 0) return 3;
        } else {
            length += node->extra;
        }
        length += 2;
        if (node == req->postData->next) break;
        node = node->prev;
    }
    length += 24;
    digits = NHTTPi_intToStr(buffer, length);
    result = fn_803936B0((u8*)thread, (void*)nhttpPostMultipartType, 44);
    if (result != 0) return result;
    result = fn_803936B0((u8*)thread, req->boundary + 2, 18);
    if (result != 0) return result;
    result = fn_803936B0((u8*)thread, "\r\n", 2);
    if (result != 0) return result;
    result = fn_803936B0((u8*)thread, (void*)nhttpThreadContentLength, 16);
    if (result != 0) return result;
    result = fn_803936B0((u8*)thread, buffer, digits);
    if (result != 0) return result;
    result = fn_803936B0((u8*)thread, "\r\n", 2);
    if (result != 0) return result;
    result = fn_803936B0((u8*)thread, "\r\n", 2);
    if (result != 0) return result;
    node = req->postData;
    while (node != 0) {
        result = fn_803936B0((u8*)thread, req->boundary, 20);
        if (result != 0) return result;
        result = fn_803936B0((u8*)thread, "\r\n", 2);
        if (result != 0) return result;
        result = fn_803936B0((u8*)thread, (void*)nhttpPostDisposition, 38);
        if (result != 0) return result;
        result = fn_803936B0((u8*)thread, (void*)node->name, NHTTPi_strlen(node->name));
        if (result != 0) return result;
        result = fn_803936B0((u8*)thread, "\"\r\n", 3);
        if (result != 0) return result;
        if (node->extra2 != 0) {
            result = fn_803936B0((u8*)thread, (void*)nhttpPostBinaryType, 75);
            if (result != 0) return result;
        }
        result = fn_803936B0((u8*)thread, "\r\n", 2);
        if (result != 0) return result;
        if (node->value == 0) {
            result = NHTTPi_SendPostData(mutex, (s32*)req, scratch, (void*)node->name, bgn->unk_7d0, &thread->sent, 1);
            if (result != 0) return result;
        } else {
            result = fn_803936B0((u8*)thread, (void*)node->value, node->extra);
            if (result != 0) return result;
        }
        result = fn_803936B0((u8*)thread, "\r\n", 2);
        if (result != 0) return result;
        if (node == req->postData->next) break;
        node = node->prev;
    }
    result = fn_803936B0((u8*)thread, req->boundary, 20);
    if (result != 0) return result;
    result = fn_803936B0((u8*)thread, "--\r\n", 4);
    return result != 0 ? result : 0;
}

static inline int nhttpSendAuthorization(NHTTPThreadState* thread) {
    NHTTPThreadRequest* req = (*(NHTTPActiveRequest**)NHTTPi_GetReqInfoP(NHTTPi_GetSystemInfoP()))->request;
    int result;
    if (req->authorizationLength == 0) return 0;
    result = fn_803936B0((u8*)thread, (void*)nhttpThreadAuthorization, 21);
    if (result != 0) return result;
    result = fn_803936B0((u8*)thread, req->authorization, req->authorizationLength);
    if (result != 0) return result;
    result = fn_803936B0((u8*)thread, "\r\n", 2);
    return result != 0 ? result : 0;
}

static inline int nhttpPostUsesMultipart(NHTTPThreadRequest* req) {
    NHTTPHdrNode* node;
    int mode = req->postMode;
    int binary;
    if (mode == 0) {
        NHTTPHdrNode* first = req->postData;
        binary = 0;
        node = first;
        while (node != 0) {
            if (node->extra2 != 0) {
                binary = 1;
                break;
            }
            if (node == first->next) break;
            node = node->prev;
        }
    } else {
        binary = mode == 2;
    }
    return binary;
}

// NHTTPi_ThreadSendProc
int fn_80394B44(NHTTPThreadState* thread) {
    void* sys = NHTTPi_GetSystemInfoP();
    NHTTPThreadRequest* req = (*(NHTTPActiveRequest**)NHTTPi_GetReqInfoP(sys))->request;
    int mutex = (int)NHTTPi_GetMutexInfoP(sys);
    NHTTPBgnEndInfo* bgn = NHTTPi_GetBgnEndInfoP(sys);
    NHTTPConnectionStatus* conn = NHTTPi_Request2Connection(mutex, (int)req);
    int schemeLength;
    u8* scratch = (u8*)NHTTPi_GetThreadInfoP(sys) + 0x360;
    int urlLength = NHTTPi_strlen(req->url);
    int result = 0;
    thread->error = 10;
    if (conn != 0) conn->status = 2;
    thread->sent = 0;
    switch (req->method) {
    case 0: result = fn_803936B0((u8*)thread, "GET ", 4); break;
    case 1: result = fn_803936B0((u8*)thread, "POST ", 5); break;
    case 2: result = fn_803936B0((u8*)thread, "HEAD ", 5); break;
    }
    if (result != 0) return result;
    if (req->proxy != 0 && req->ssl == 0) {
        result = fn_803936B0((u8*)thread, req->url, urlLength);
        if (result != 0) return result;
    } else if (urlLength > req->pathOffset) {
        result = fn_803936B0((u8*)thread, req->url + req->pathOffset, urlLength - req->pathOffset);
        if (result != 0) return result;
    } else {
        result = fn_803936B0((u8*)thread, "/", 1);
        if (result != 0) return result;
    }
    result = fn_803936B0((u8*)thread, (void*)nhttpThreadHttpVersion, 11);
    if (result != 0) return result;
    schemeLength = (req->ssl != 0) + 7;
    result = fn_803936B0((u8*)thread, "Host: ", 6);
    if (result != 0) return result;
    result = fn_803936B0((u8*)thread, req->url + schemeLength, req->hostLength - schemeLength);
    if (result != 0) return result;
    result = fn_803936B0((u8*)thread, "\r\n", 2);
    if (result != 0) return result;
    if (req->proxy != 0 && req->ssl == 0) {
        result = nhttpSendProxyAuthorization(thread);
        if (result != 0) return result;
    }
    result = nhttpSendAuthorization(thread);
    if (result != 0) return result;
    result = fn_80393BFC((u8*)thread);
    if (result != 0) return result;
    if (req->method == 1) {
        if (req->rawPost != 0) result = fn_80393CF4((u8*)thread);
        else if (nhttpPostUsesMultipart(req)) result = fn_80393E7C(thread);
        else result = fn_80394220(thread);
        if (result != 0) {
            if (result == 3) thread->error = 3;
            return result;
        }
    } else {
        result = fn_803936B0((u8*)thread, "\r\n", 2);
        if (result != 0) return result;
    }
    result = 0;
    if (thread->sent > 0) {
        int count = fn_803925B8((u8*)req, bgn->unk_7d0, (char*)scratch, thread->sent, 0);
        thread->sent = 0;
        NHTTPi_memclr(scratch, 0x100);
        if (count < 0) result = 1;
        if (count == 0) result = 2;
    }
    thread->sent = 0;
    NHTTPi_memclr(scratch, 0x100);
    return result;
}

// NHTTP_CreateRequest
int fn_803911C8(void* bgn, int urlArg, int method, int buffer, int capacity, int userContext, int bufferCallback, int cleanupCallback) {
    NHTTPThreadRequest* req = 0;
    char url[0x100];
    int length;
    int schemeLength;
    int remaining;
    u8* authority;
    int i;
    int escapes;
    int phase;
    int output;
    int path;
    int size;
    u8* cursor;
    if (method >= 3 || method < 0) {
        NHTTPi_SetError(bgn, 11);
        goto fail;
    }
    req = NHTTPi_alloc(0x254, 4);
    if (req == 0) {
        NHTTPi_SetError(bgn, 1);
        goto fail;
    }
    NHTTPi_memclr(req, 0x254);
    req->response = NHTTPi_alloc(0x43c, 4);
    if (req->response == 0) {
        NHTTPi_SetError(bgn, 1);
        goto fail;
    }
    NHTTPi_memclr(req->response, 0x43c);
    req->response->body = (void*)buffer;
    req->response->capacity = capacity;
    req->response->bufferCallback = (void*)bufferCallback;
    req->response->cleanupCallback = (void*)cleanupCallback;
    length = NHTTPi_strlen((const char*)urlArg);
    if (length <= 7) {
        NHTTPi_SetError(bgn, 4);
        goto fail;
    }
    NHTTPi_memclr(url, length);
    NHTTPi_memcpy(url, (void*)urlArg, length);
    req->port = 80;
    schemeLength = 7;
    if (fn_80392744(url, "http://", 7) != 0) {
        if (fn_80392744(url, "https://", 8) != 0) {
            NHTTPi_SetError(bgn, 4);
            goto fail;
        }
        req->ssl = 1;
        schemeLength = 8;
        req->port = 443;
    }
    remaining = length - schemeLength;
    authority = (u8*)url + schemeLength;
    if (remaining <= 0) {
        NHTTPi_SetError(bgn, 4);
        goto fail;
    }
    escapes = 0;
    phase = 0;
    cursor = authority;
    for (i = 0; i < remaining && (s8)*cursor != '/'; i++, cursor++) {
        if (phase == 2) {
            phase--;
        } else if (phase == 1) {
            s8 c = fn_80392994(authority + i - 1, 2);
            phase--;
            if (c < 0) {
                NHTTPi_SetError(bgn, 4);
                goto fail;
            }
            if (c == '/') {
                escapes--;
                break;
            }
        } else if ((s8)*cursor == '%') {
            phase = 2;
            escapes++;
        }
    }
    if (phase != 0) {
        NHTTPi_SetError(bgn, 4);
        goto fail;
    }
    size = schemeLength + remaining - escapes * 2 + 1;
    req->url = NHTTPi_alloc(size, 4);
    if (req->url == 0) {
        NHTTPi_SetError(bgn, 1);
        goto fail;
    }
    NHTTPi_memclr(req->url, size);
    NHTTPi_memcpy(req->url, url, schemeLength);
    output = 0;
    phase = 0;
    path = 0;
    cursor = authority;
    for (i = 0; i < remaining; i++, cursor++) {
        if (phase == 2) {
            phase--;
        } else if (phase == 1) {
            s8 c = fn_80392994(authority + i - 1, 2);
            phase--;
            req->url[output + schemeLength - 1] = c;
            if (c == '/') path = 1;
        } else {
            int c = *cursor;
            if (c == '/') path = 1;
            if ((path == 0) & ((s8)c == '%')) {
                phase = 2;
            } else {
                req->url[output + schemeLength] = c;
            }
            output++;
        }
    }
    req->url[schemeLength + output] = 0;
    {
        char* host = req->url + schemeLength;
        char* scan = host;
        for (i = 0; i < output; i++, scan++) {
            if (*scan == '/' || *scan == ':') {
                req->hostLength = i + schemeLength;
                break;
            }
        }
        if (i == output) {
            req->hostLength = i + schemeLength;
            req->pathOffset = i + schemeLength;
        } else if (host[i] == '/') {
            req->pathOffset = req->hostLength;
        } else if (host[i] == ':') {
            for (; i < output; i++) {
                if (host[i] == '/') {
                    req->pathOffset = i + schemeLength;
                    break;
                }
            }
            if (i == output) {
                req->pathOffset = i + schemeLength;
            } else {
                int portStart = req->hostLength + 1;
                int port = NHTTPi_strtonum((u8*)req->url + portStart, req->pathOffset - portStart);
                if (port < 0) port = req->port;
                else if (port > 0xffff) {
                    NHTTPi_SetError(bgn, 4);
                    goto fail;
                }
                req->port = (u16)port;
            }
        }
    }
    size = req->hostLength - ((req->ssl != 0) + 7);
    req->host = NHTTPi_alloc(size + 1, 4);
    if (req->host == 0) {
        NHTTPi_SetError(bgn, 1);
        goto fail;
    }
    NHTTPi_memclr(req->host, size + 1);
    {
        int prefix = (req->ssl != 0) + 7;
        NHTTPi_memcpy(req->host, req->url + prefix, size);
    }
    NHTTPi_memcpy(req->boundary, "--t9Sf4yfjf1RtvDu3AA", 20);
    req->method = method;
    req->sslHandle = 0;
    // HYPOTHESIS: SSL option and callback slots initialized by the SDK.
    ((s32*)req->unkB0)[0] = 0;
    ((s32*)req->unkB0)[1] = 0;
    ((s32*)req->unkB0)[2] = 0;
    ((s32*)req->unkB0)[3] = 0;
    ((s32*)req->unkB0)[4] = 0;
    ((s32*)req->unkB0)[5] = 0;
    ((s32*)req->unkB0)[6] = 0;
    ((s32*)req->unkB0)[7] = 0;
    ((s32*)req->unkD4)[0] = 0;
    req->response->userContext = (void*)userContext;
    req->proxy = 0;
    req->socketOption = 0;
    return (int)req;
fail:
    if (req != 0) {
        if (req->url != 0) NHTTPi_free(req->url);
        if (req->host != 0) NHTTPi_free(req->host);
        if (req->response != 0) NHTTPi_free(req->response);
        NHTTPi_free(req);
    }
    return 0;
}

static inline int nhttpReadLineEnd(int mutex, NHTTPThreadRequest* req, int socket) {
    char recent[2];
    int total = 0;
    int i = 0;
    recent[0] = 0;
    recent[1] = 0;
    while (recent[i & 1] != '\r' || recent[(i - 1) & 1] != '\n') {
        int count = NHTTPi_SocRecv(mutex, req, socket, &recent[i & 1], 1, 0);
        if (count <= 0) return count;
        total += count;
        i++;
    }
    return total;
}

// NHTTPi_ThreadRecvBodyProc
int fn_803954C8(NHTTPThreadState* thread) {
    void* sys = NHTTPi_GetSystemInfoP();
    NHTTPThreadRequest* req = (*(NHTTPActiveRequest**)NHTTPi_GetReqInfoP(sys))->request;
    NHTTPThreadResponse* response = req->response;
    NHTTPBgnEndInfo* bgn = NHTTPi_GetBgnEndInfoP(sys);
    int mutex = (int)NHTTPi_GetMutexInfoP(sys);
    NHTTPConnectionStatus* conn = NHTTPi_Request2Connection(mutex, (int)req);
    u8* scratch = (u8*)NHTTPi_GetThreadInfoP(sys) + 0x360;
    int count;
    if (req->method == 2 || response->statusCode == 204 || response->statusCode == 304 ||
        (response->statusCode >= 100 && response->statusCode < 200)) return 1;
    NHTTPi_SetVirtualContentLength((NHTTPVirtLen*)conn, 0);
    if (conn != 0) conn->status = 4;
    if (thread->contentLength >= 0) {
        NHTTPi_SetVirtualContentLength((NHTTPVirtLen*)conn, thread->contentLength);
        while (thread->contentLength > 0) {
            if (thread->error != 6) {
                if (fn_803935E4(mutex, (u32*)response) == 0) {
                    thread->error = 6;
                    response->body = thread->unk104;
                    response->capacity = 0x200;
                }
            }
            if (thread->error == 6) {
                count = fn_80391180((void*)mutex, req, (void*)bgn->unk_7d0, 0, thread->contentLength, 0);
            } else {
                count = fn_80391180((void*)mutex, req, (void*)bgn->unk_7d0, response->received, thread->contentLength, 0);
            }
            if (count < 0) return 0;
            if (count == 0) break;
            if (thread->error != 6) {
                response->received += count;
                response->totalReceived += count;
            }
            thread->contentLength -= count;
        }
        if (thread->error != 6) {
            if (thread->contentLength != 0) {
                thread->error = fn_80391144(response, response->received) != 0 ? 6 : 10;
            } else {
                thread->error = 0;
            }
        }
    } else {
        thread->error = 10;
        if (thread->chunked != 0) {
            int chunkLength = -1;
            char recent[2];
            for (;;) {
                recent[0] = 0;
                recent[1] = 0;
                thread->received = 0;
                while (thread->received < 0x100) {
                    int digits;
                    int c;
                    count = NHTTPi_SocRecv(mutex, req, bgn->unk_7d0, scratch + thread->received, 1, 0);
                    if (count < 0) return 0;
                    digits = thread->received;
                    c = scratch[digits];
                    recent[digits & 1] = c;
                    if ((s8)c == ';' || ((s8)c == '\n' && recent[(digits - 1) & 1] == '\r')) {
                        if ((s8)c == '\n') digits--;
                        else if (nhttpReadLineEnd(mutex, req, bgn->unk_7d0) <= 0) return 0;
                        if (digits == 0) return 0;
                        chunkLength = fn_80392994(scratch, digits);
                        if (chunkLength < 0) return 0;
                        break;
                    }
                    thread->received++;
                }
                if (thread->received == 0x100) {
                    thread->error = 7;
                    return 0;
                }
                if (chunkLength <= 0) break;
                NHTTPi_SetVirtualContentLength((NHTTPVirtLen*)conn, chunkLength);
                while (chunkLength > 0) {
                    if (thread->error != 6) {
                        if (fn_803935E4(mutex, (u32*)response) == 0) {
                            thread->error = 6;
                            response->body = thread->unk104;
                            response->capacity = 0x200;
                        }
                    }
                    if (thread->error == 6) {
                        count = fn_80391180((void*)mutex, req, (void*)bgn->unk_7d0, 0, chunkLength, 0);
                    } else {
                        count = fn_80391180((void*)mutex, req, (void*)bgn->unk_7d0, response->received, chunkLength, 0);
                    }
                    if (count <= 0) return 0;
                    chunkLength -= count;
                    response->received += count;
                    response->totalReceived += count;
                    if (chunkLength == 0) {
                        if (NHTTPi_SocRecv(mutex, req, bgn->unk_7d0, scratch, 2, 0) <= 0) return 0;
                    }
                }
            }
            nhttpReadLineEnd(mutex, req, bgn->unk_7d0);
            thread->error = 0;
        } else {
            for (;;) {
                if (fn_803935E4(mutex, (u32*)response) == 0) {
                    thread->error = 6;
                    response->body = thread->unk104;
                    response->capacity = 0x200;
                }
                if (thread->error == 6) count = fn_80391160((void*)mutex, req, (void*)bgn->unk_7d0, 0, 0);
                else count = fn_80391160((void*)mutex, req, (void*)bgn->unk_7d0, response->received, 0);
                if (count < 0) return 0;
                if (count == 0) {
                    if (thread->error != 6) thread->error = 0;
                    break;
                }
                response->received += count;
                response->totalReceived += count;
            }
        }
    }
    conn = NHTTPi_Response2Connection(mutex, (int)response);
    if (thread->error == 0 && conn != 0) fn_8039738C(mutex, conn);
    return 1;
}

// NHTTPCreateRequestEx
NHTTPRequestHandle* fn_8039651C(const char* url, NHTTPReqMethod method, char* buffer, size_t bufferSize, NHTTPReqCallback callback, void* userContext, int responseCb, int cleanup) {
    void* conn = (void*)NHTTPCreateConnection((int)url, (int)method, (int)buffer, (unsigned int)bufferSize, (int)NHTTPi_TemplateConnectionCallback, (int)userContext);
    u32 result = 0;
    if (conn != 0) {
        void* sys = NHTTPi_GetSystemInfoP();
        void* mutex = NHTTPi_GetMutexInfoP(sys);
        NHTTPThreadRequest* req;
        conn = NHTTPi_GetConnection((int)mutex, (int)conn);
        req = (NHTTPThreadRequest*)NHTTPi_Connection2Request((int)mutex, (int)conn);
        result = (u32)req;
        if (req != 0) {
            if (req->response != 0) {
                *(void**)((u8*)conn + 0x30) = (void*)callback;
                req->response->bufferCallback = (void*)responseCb;
                req->response->cleanupCallback = (void*)cleanup;
                return (NHTTPRequestHandle*)req;
            } else {
                fn_80391784(sys, (int)req);
                NHTTPi_OmitConnectionList((int)mutex, (int)conn);
                NHTTPi_free(conn);
                result = 0;
            }
        }
    }
    return (NHTTPRequestHandle*)result;
}
