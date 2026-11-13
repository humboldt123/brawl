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
void* NHTTPi_CommThreadProcMain(void* arg);
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
int fn_80390954(void*, int, void*);
void fn_80390A64(void*, int);
int fn_80391144(void*, unsigned int);
int fn_80391160(void*, void*, void*, unsigned int, int);
int fn_80391180(void*, void*, void*, unsigned int, int);
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
int NHTTP_AddPostDataAscii(void*, void*, const char*, const char*);
void fn_80391784(void*, int);
void NHTTPi_TemplateConnectionCallback();
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

void NHTTPi_free(void* ptr) {
    NHTTPBgnEndInfo* info = (NHTTPBgnEndInfo*)NHTTPi_GetSystemInfoP();
    void (*freeFn)(void*) = info->freeFn;
    if (freeFn != 0) {
        freeFn(ptr);
    }
}

void* NHTTPi_alloc(u32 size, s32 align) {
    NHTTPBgnEndInfo* info = (NHTTPBgnEndInfo*)NHTTPi_GetSystemInfoP();
    void* (*alloc)(u32, s32) = info->alloc;
    if (alloc != 0) {
        return alloc(size, align);
    }
    return 0;
}

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
int fn_80391180(void* a, void* req, void* c, unsigned int offset, int len) {
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
    return NHTTPi_SocRecv(a, req, c, at, len);
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
void fn_80391F5C(void* mutex, u8* req, int sock) {
    int ssl;
    NHTTPi_lockReqList((OSMutex*)mutex);
    ssl = *(s32*)(req + 0xac);
    if (ssl > 0) {
        SSLShutdown(ssl);
        *(s32*)(req + 0xac) = -1;
    }
    NHTTPi_unlockReqList((OSMutex*)mutex);
    SOClose(sock);
}

// NHTTPi_SocSend
int fn_803925B8(u8* obj, int sock, char* buf, unsigned int len, int flags) {
    int ssl = *(s32*)(obj + 0xac);
    int r;
    if (ssl > 0) {
        r = SSLWrite(ssl, buf, len);
    } else {
        r = NHTTPi_SocSend_sub(sock, buf, len, flags);
    }
    if (r < 0) {
        if (*(s32*)obj != 0) {
            r = -0x3ea;
        } else if (*(s32*)(obj + 0xac) > 0) {
            if ((unsigned int)(r + 7) > 1) {
                r = -0x3e9;
            } else {
                r = 0;
            }
        } else if (r != -0x38) {
            r = -0x3e9;
        } else {
            r = 0;
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
    s8 c1;
    s8 c2;
    if (n <= 0) {
        return n;
    }
    do {
        c1 = (s8)*a++;
        c2 = (s8)*b++;
        if (c1 == 0 && c2 == 0) {
            return 0;
        }
        if (c2 >= 0x41 && c2 <= 0x5a) {
            c2 = (s8)(c2 + 0x20);
        }
        if (c1 >= 0x41 && c1 <= 0x5a) {
            c1 = (s8)(c1 + 0x20);
        }
        if (c1 != c2) {
            return n;
        }
        n -= 1;
    } while (n != 0);
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
    if (len <= 0) {
        return n;
    }
    do {
        if ((c >= 0x30 && c <= 0x39) || (c >= 0x41 && c <= 0x5a) || (c >= 0x61 && c <= 0x7a) || c == 0x20) {
            n += 1;
        } else {
            n += 3;
        }
        c = (s8)*s++;
        len -= 1;
    } while (len != 0);
    return n;
}

// NHTTPi_compareToken
int fn_80392CD4(const char* s, const char* token) {
    s8 raw;
    for (;;) {
        s8 t = (s8)*token;
        s8 tl = (s8)((t >= 0x41 && t <= 0x5a) ? t + 0x20 : t);
        raw = (s8)*s;
        {
            s8 cl = (s8)((raw >= 0x41 && raw <= 0x5a) ? raw + 0x20 : raw);
            if (cl != tl) {
                return -1;
            }
        }
        if (raw == 0 || raw == 0x20) {
            return 0;
        }
        s += 1;
        token += 1;
    }
}

// NHTTPi_memfind. 0 when needle occurs in the first hayLen bytes, else -1.
int fn_80392E18(const char* hay, int hayLen, const char* needle, int needleLen) {
    int last;
    int i;
    if (hayLen < needleLen) {
        return -1;
    }
    last = hayLen - needleLen;
    for (i = 0; i <= last; i++) {
        int matched;
        int k;
        if ((s8)hay[i] != (s8)needle[0]) {
            continue;
        }
        matched = 1;
        if (needleLen > 1) {
            for (k = 1; k < needleLen; k++) {
                if ((s8)hay[i + k] != (s8)needle[k]) {
                    break;
                }
                matched += 1;
            }
        }
        if (matched == needleLen) {
            return 0;
        }
    }
    return -1;
}

// NHTTPi_createCommThread
int fn_80390954(void* info, int priority, void* stack) {
    void* stackTop = stack;
    int prio = priority;
    void* thread = info;
    if (NHTTPi_IsCreateCommThreadMessageQueue((NHTTPThreadInfo*)thread) == 0) {
        OSInitMessageQueue((OSMessageQueue*)thread, (OSMessage*)((u8*)thread + 0x20), 3);
        NHTTPi_IsCreateCommThreadMessageQueueOn((NHTTPThreadInfo*)thread);
    }
    OSCreateThread((OSThread*)((u8*)thread + 0x30), NHTTPi_CommThreadProc, 0, (u8*)stackTop + 0x2000, 0x2000, prio, 0);
    OSResumeThread((OSThread*)((u8*)thread + 0x30));
    return 1;
}

// NHTTPi_CheckCurrentThread
// Member at offset 0 keeps the addi that a plain pointer would fold into a move.
typedef struct NHTTPChkStr {
    char name[0x1c];
    char fmt[0x14];
    char file[0x10];
} NHTTPChkStr;

static const NHTTPChkStr nhttpChkThread = {
    "NHTTPi_CheckCurrentThread",
    "%s:illegal thread\n",
    "NHTTP_os_RVL.c",
};

void fn_80390A64(void* info, int outside) {
    const NHTTPChkStr* base = &nhttpChkThread;
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
    OSReport(base->fmt, base->name);
    OSPanic(base->file, 0xdf, "halt\n");
}

// Packed like the original pool: format, file at +0x20, function name at +0x30.
static const char nhttpStartupStr[] =
    "NCDGetCurrentIpConfig err = %d\n\0"
    "NHTTP_bgnend.c\0\0"
    "NCDGetCurrentIpConfig";

// NHTTPi_Startup
int fn_803900FC(void* sys, void* alloc, void* freeFn, int priority) {
    NHTTPBgnEndInfo* info = (NHTTPBgnEndInfo*)NHTTPi_GetBgnEndInfoP(sys);
    void* list = NHTTPi_GetListInfoP(sys);
    int* req = (int*)NHTTPi_GetReqInfoP(sys);
    NHTTPReqListLock* mutex = (NHTTPReqListLock*)NHTTPi_GetMutexInfoP(sys);
    void* thread = NHTTPi_GetThreadInfoP(sys);
    void* stack;
    void* (*allocFn)(u32, s32);
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
    stack = 0;
    allocFn = (void* (*)(u32, s32))((NHTTPBgnEndInfo*)NHTTPi_GetSystemInfoP())->alloc;
    if (allocFn != 0) {
        stack = allocFn(0x2000, 8);
    }
    info->unk_7e0 = (s32)stack;
    if (stack == 0) {
        info->error = 1;
        NHTTPi_exitLockReqList();
        return 0;
    }
    if (fn_80390954(thread, priority, stack) == 0) {
        void (*freeCb)(void*);
        void* block;
        info->error = 9;
        block = (void*)info->unk_7e0;
        freeCb = (void (*)(void*))((NHTTPBgnEndInfo*)NHTTPi_GetSystemInfoP())->freeFn;
        if (freeCb != 0) {
            freeCb(block);
        }
        info->unk_7e0 = 0;
        NHTTPi_exitLockReqList();
        return 0;
    }
    {
        int ip = NCDGetCurrentIpConfig((void*)info);
        if (ip < 0) {
            OSReport(nhttpStartupStr, ip);
            OSPanic(nhttpStartupStr + 0x20, 0xe6, nhttpStartupStr + 0x30);
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
    NHTTPHdrNode* node = *headp;
    int found = 0;
    NHTTPHdrNode* neu;
    if (node != 0) {
        if (fn_80392CD4(name, node->name) == 0) {
            found = 1;
        } else {
            node = node->prev;
            while (node != *headp) {
                if (fn_80392CD4(name, node->name) == 0) {
                    found = 1;
                    break;
                }
                node = node->prev;
            }
        }
    }
    if (found != 0) {
        node->value = value;
        return 1;
    }
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
    NHTTPLink* head = *list;
    NHTTPLink* found = 0;
    int ret = 0;
    s32 id = (s32)data;
    if (head != 0) {
        if ((s32)head->data == id) {
            found = head;
        } else {
            NHTTPLink* n = head->prev;
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
        void* req;
        if (head == head->next) {
            *list = 0;
        } else {
            found->next->prev = found->prev;
            found->prev->next = found->next;
            if (*list == found) {
                *list = found->prev;
            }
        }
        req = *(void**)((u8*)found + 0xc);
        conn = NHTTPi_Request2Connection(arg, (int)req);
        fn_80391888((void*)arg, req);
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
    u8* resp;
    if (found == 0) {
        return -1;
    }
    resp = (u8*)NHTTPi_Connection2Response((int)mutex, (int)found);
    if (resp == 0) {
        return -1;
    }
    *(void**)out = *(void**)(resp + 0x28);
    *local = *(s32*)(resp + 0x1c);
    return *(s32*)(resp + 4);
}

// NHTTPGetUserParam
void* fn_80396188(int id) {
    void* mutex = NHTTPi_GetMutexInfoP(NHTTPi_GetSystemInfoP());
    void* conn = NHTTPi_GetConnection((int)mutex, id);
    u8* resp;
    if (conn == 0) {
        return 0;
    }
    resp = (u8*)NHTTPi_Connection2Response((int)mutex, (int)conn);
    if (resp == 0) {
        return 0;
    }
    return *(void**)(resp + 0x438);
}

int NHTTPStartup(NHTTPAlloc alloc, NHTTPFree free, u32 priority) {
    void* sys = NHTTPi_GetSystemInfoP();
    if (*(s32*)lbl_805A1148 == 0) {
        OSRegisterVersion(lbl_8059FCB8);
        *(s32*)lbl_805A1148 = 1;
    }
    return fn_803900FC(sys, (void*)alloc, (void*)free, (int)priority) ? 0 : -1;
}

// NHTTPCreateRequestEx
NHTTPRequestHandle* fn_8039651C(const char* url, NHTTPReqMethod method, char* buffer, size_t bufferSize, NHTTPReqCallback callback, void* userContext, int responseCb, int cleanup) {
    void* conn = (void*)NHTTPCreateConnection(url, (int)method, buffer, (unsigned int)bufferSize, (void*)NHTTPi_TemplateConnectionCallback, userContext);
    void* sys;
    void* mutex;
    u8* req;
    if (conn == 0) {
        return 0;
    }
    sys = NHTTPi_GetSystemInfoP();
    mutex = NHTTPi_GetMutexInfoP(sys);
    conn = NHTTPi_GetConnection((int)mutex, (int)conn);
    req = (u8*)NHTTPi_Connection2Request((int)mutex, (int)conn);
    if (req == 0) {
        return 0;
    }
    if (*(s32*)(req + 0x2c) != 0) {
        *(void**)((u8*)conn + 0x30) = (void*)callback;
        *(s32*)(*(u8**)(req + 0x2c) + 0x2c) = responseCb;
        *(s32*)(*(u8**)(req + 0x2c) + 0x30) = cleanup;
        return (NHTTPRequestHandle*)req;
    }
    fn_80391784(sys, (int)req);
    NHTTPi_OmitConnectionList((int)mutex, (int)conn);
    NHTTPi_free(conn);
    return 0;
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
    return NHTTP_AddPostDataAscii(req, bgn, name, value) ? 0 : -1;
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
    if (*(s32*)base == 0) {
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
    if (*(s32*)base == 0) {
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
    void* reqInfo = NHTTPi_GetReqInfoP(sys);
    void* mutex = NHTTPi_GetMutexInfoP(sys);
    u8* head = *(u8**)reqInfo;
    int ret = 0;
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
            s32 len = *(s32*)(*(u8* volatile*)(*(u8* volatile*)(head + 0xc) + 0x2c) + 0xc);
            *outR = *(s32*)(*(u8* volatile*)(*(u8* volatile*)(head + 0xc) + 0x2c) + 4);
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

int NHTTPi_BufferFullCallback(int, void*);

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
    u8 pad[0x34];
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
    r = fn_803936B0(obj, "Content-Length: ", 0x10);
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

int NHTTPi_getHeaderValue(void*, const char*, int*);
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
    n = NHTTPi_getHeaderValue(resp, name, &start);
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
