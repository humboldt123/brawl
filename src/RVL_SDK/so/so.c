// socket library (SO). talks to the IOS network stack through /dev/net/ip/top ioctls.
// structure and ioctl numbers follow the ref socket layer, the "enter / alloc / ioctl / free / leave"
// shape of every call is the same, only the helper names are my own guesses (HYPOTHESIS).
#include "revolution.h"
#include "revolution/ipc.h"
#include <cstdio>
#include <cstring>
#include <cstdarg>

// BYTES: the helpers below (state check, alloc, error save) were not inlined in the original,
// so keep the auto inliner off for the whole file.
#pragma auto_inline off

#define SO_EALREADY -7
#define SO_EAGAIN -6
#define SO_EAFNOSUPPORT -5
#define SO_EBUSY -10
#define SO_EINVAL -28
#define SO_EINPROGRESS -26
#define SO_ENETRESET -39
#define SO_ENOMEM -49
#define SO_ENOBUFS -42
#define SO_ERR_NETRM_NOTAVAIL 0x80000000

#define NET_SO_ACCEPT 0x01
#define NET_SO_BIND 0x02
#define NET_SO_CLOSE 0x03
#define NET_SO_CONNECT 0x04
#define NET_SO_FCNTL 0x05
#define NET_SO_GETPEERNAME 0x06
#define NET_SO_GETSOCKNAME 0x07
#define NET_SO_GETSOCKOPT 0x08
#define NET_SO_SETSOCKOPT 0x09
#define NET_SO_LISTEN 0x0A
#define NET_SO_POLL 0x0B
#define NET_SO_RECVFROM 0x0C
#define NET_SO_SENDTO 0x0D
#define NET_SO_SHUTDOWN 0x0E
#define NET_SO_SOCKET 0x0F
#define NET_SO_GETHOSTID 0x10
#define NET_SO_GETHOSTBYNAME 0x11
#define NET_SO_INETATON 0x15
#define NET_SO_GETADDRINFO 0x18
#define NET_SO_INIT 0x1F

// names for the alloc/free callbacks (second arg of SOLibraryConfig.alloc)
#define SO_MEM_ADDRINFO 10
#define SO_MEM_IPCBUF 11 // HYPOTHESIS: the 0x460 byte scratch block SOInit hands out
#define SO_MEM_IPC 12    // HYPOTHESIS: ioctl argument blocks
#define SO_MEM_IPCDATA 13 // HYPOTHESIS: bounce buffer for unaligned send/recv data

typedef struct SOLibraryConfig {
    void *(*alloc)(u32 name, s32 size);
    void (*free)(u32 name, void *ptr, s32 size);
} SOLibraryConfig;

typedef struct SOSockAddr {
    u8 len;
    u8 family;
    u8 data[6];
} SOSockAddr;

typedef struct SOInAddr {
    u32 addr;
} SOInAddr;

typedef struct SOHostEnt {
    char *name;
    char **aliases;
    s16 addrType;
    s16 length;
    u8 **addrList;
} SOHostEnt;

typedef struct SOPollFD {
    int fd;
    int events;
    int revents;
} SOPollFD;

typedef struct SOAddrInfo SOAddrInfo;
struct SOAddrInfo {
    int flags;
    int family;
    int sockType;
    int protocol;
    u32 addrLen;
    char *canonName;
    SOSockAddr *addr;
    SOAddrInfo *next;
};

// 0x18 bytes of library state
typedef struct SOLib {
    void *(*alloc)(u32 name, s32 size); // 0x00
    void (*free)(u32 name, void *ptr, s32 size); // 0x04
    s32 status;                          // 0x08, -2 = just initialised, -1 = starting up, >= 0 = running
    s32 fd;                              // 0x0C, /dev/net/ip/top
    void *scratch;                       // 0x10, 0x460 bytes from the first allocation (used by getHostByName)
    s32 allocCount;                      // 0x14
} SOLib;

static SOLib lib;
static char ntoaBuf[0x28];

// 0 = not initialised, 1 = SOInit done, 2 = started
static u8 state;
static s32 errorNoThread;
static s32 versionRegistered;
static s32 socketVersionRegistered;

const char *__SOVersion = "<< RVL_SDK - SO \trelease build: Jun 28 2007 18:29:42 (0x4199_60831) >>";
s32 __SOCheckMem2 = 1;
const char *__SOSocketVersion = "<< RVL_SDK - SOCKET \trelease build: Jun 28 2007 18:29:43 (0x4199_60831) >>";

#define SO_SAVE_ERROR(rv)                \
    do {                                 \
        OSThread *t = OSGetCurrentThread(); \
        if (t != NULL) {                 \
            t->error = (rv);             \
        } else {                         \
            errorNoThread = (rv);        \
        }                                \
    } while (0)

SOLib *SOiGetLib(void);
int SOiGetCheckMem2(void);
BOOL SOiIsInitialized(void);
void *SOiAlloc(u32 name, s32 size);
void SOiFree(u32 name, void *ptr, s32 size);
int SOiEnter(int unused, int *fd);
int SOiLeave(int unused, int rv);

int SOInit(const SOLibraryConfig *config) {
    int rv = 0;
    BOOL enabled = OSDisableInterrupts();
    void *buf;

    if (versionRegistered == 0) {
        OSRegisterVersion(__SOVersion);
        versionRegistered = 1;
    }

    switch (state) {
    case 1:
    case 2:
        rv = SO_EALREADY;
        break;
    case 0:
    default:
        if (config == NULL || config->alloc == NULL || config->free == NULL) {
            rv = SO_EINVAL;
            break;
        }

        memset(&lib, 0, sizeof(lib));
        lib.alloc = config->alloc;
        lib.free = config->free;
        lib.allocCount = 0;
        lib.status = -2;
        lib.fd = -1;

        if (lib.alloc != NULL) {
            buf = lib.alloc(SO_MEM_IPCBUF, 0x460);
            if (buf != NULL) {
                lib.allocCount++;

                if (__SOCheckMem2) {
                    u32 addr = (u32)buf & 0x1FFFFFFF;

                    if (addr < 0x10000000 || addr >= 0x18000000) {
                        // not in mem2, ipc can't dma from there
                        if (buf != NULL && lib.free != NULL) {
                            lib.allocCount--;
                            lib.free(SO_MEM_IPCBUF, buf, 0x460);
                        }

                        buf = NULL;
                    }
                }
            }
        } else {
            buf = NULL;
        }

        lib.scratch = buf;
        if (buf == NULL) {
            rv = SO_ENOMEM;
        }

        if (buf != NULL) {
            state = 1;
        }
        break;
    }

    SO_SAVE_ERROR(rv);
    OSRestoreInterrupts(enabled);
    return rv;
}

int SOFinish(void) {
    int rv = 0;
    BOOL enabled = OSDisableInterrupts();

    switch (state) {
    case 0:
        rv = SO_EALREADY;
        break;
    case 1:
        if (lib.status > -2) {
            rv = SO_EBUSY;
        } else if (lib.allocCount > 1) {
            rv = SO_EAGAIN;
        } else {
            state = 0;

            if (lib.scratch != NULL && lib.free != NULL) {
                lib.allocCount--;
                lib.free(SO_MEM_IPCBUF, lib.scratch, 0x460);
            }
        }
        break;
    case 2:
        rv = SO_EINPROGRESS;
        break;
    }

    SO_SAVE_ERROR(rv);
    OSRestoreInterrupts(enabled);
    return rv;
}

SOLib *SOiGetLib(void) {
    return &lib;
}

int SOiGetCheckMem2(void) {
    return __SOCheckMem2;
}

BOOL SOiIsInitialized(void) {
    BOOL result = FALSE;
    BOOL enabled = OSDisableInterrupts();

    switch (state) {
    case 1:
    case 2:
        result = TRUE;
        break;
    }

    OSRestoreInterrupts(enabled);
    return result;
}

void *SOiAlloc(u32 name, s32 size) {
    void *p;

    if (size > 0 && lib.alloc != NULL) {
        p = lib.alloc(name, size);

        if (p != NULL) {
            lib.allocCount++;

            if (__SOCheckMem2) {
                u32 addr = (u32)p & 0x1FFFFFFF;

                if (addr < 0x10000000 || addr >= 0x18000000) {
                    if (p != NULL && lib.free != NULL) {
                        lib.allocCount--;
                        lib.free(name, p, size);
                    }

                    p = NULL;
                }
            }

            return p;
        }
    }

    return NULL;
}

void SOiFree(u32 name, void *ptr, s32 size) {
    if (ptr == NULL) {
        return;
    }

    if (lib.free == NULL) {
        return;
    }

    lib.allocCount--;
    lib.free(name, ptr, size);
}

// checks the library is up and hands out the ip resource manager fd. returns 0 or an SO error
int SOiEnter(int unused, int *fd) {
    int rv = 0;
    BOOL enabled = OSDisableInterrupts();

    switch (state) {
    case 0:
        rv = SO_ENETRESET;
        break;
    default:
        rv = SO_EINVAL;
        break;
    case 2:
        if (lib.status < 0) {
            rv = SO_EBUSY;
        } else if (OSGetCurrentThread() == NULL) {
            rv = SO_ERR_NETRM_NOTAVAIL;
        } else {
            *fd = lib.fd;
        }
        break;
    }

    if (rv != 0) {
        SO_SAVE_ERROR(rv);
    }

    OSRestoreInterrupts(enabled);
    return rv;
}

int SOiLeave(int unused, int rv) {
    BOOL enabled = OSDisableInterrupts();

    SO_SAVE_ERROR(rv);
    OSRestoreInterrupts(enabled);
    return rv;
}

int SOSocket(int af, int type, int protocol) {
    int fd;
    int *args;
    int rv;

    if (socketVersionRegistered == 0) {
        OSRegisterVersion(__SOSocketVersion);
        socketVersionRegistered = 1;
    }

    rv = SOiEnter(0, &fd);
    if (rv == 0) {
        if (af == 0x17) {
            rv = SO_EAFNOSUPPORT;
        } else {
            args = (int *)SOiAlloc(SO_MEM_IPC, 0x20);
            if (args == NULL) {
                rv = SO_ENOMEM;
            } else {
                args[0] = af;
                args[1] = type;
                args[2] = protocol;

                rv = IOS_Ioctl(fd, NET_SO_SOCKET, args, 0xC, NULL, 0);
                SOiFree(SO_MEM_IPC, args, 0x20);
            }
        }

        rv = SOiLeave(0, rv);
    }

    return rv;
}

// same as SOSocket without the version registration; nothing in the demo code explains why it is duplicated
int SOiSocket(int af, int type, int protocol) {
    int fd;
    int rv;
    int *args;

    rv = SOiEnter(0, &fd);
    if (rv == 0) {
        if (af == 0x17) {
            rv = SO_EAFNOSUPPORT;
        } else {
            args = (int *)SOiAlloc(SO_MEM_IPC, 0x20);
            if (args == NULL) {
                rv = SO_ENOMEM;
            } else {
                args[0] = af;
                args[1] = type;
                args[2] = protocol;

                rv = IOS_Ioctl(fd, NET_SO_SOCKET, args, 0xC, NULL, 0);
                SOiFree(SO_MEM_IPC, args, 0x20);
            }
        }

        rv = SOiLeave(0, rv);
    }

    return rv;
}

int SOClose(int s) {
    int fd;
    int rv;
    int *args;

    rv = SOiEnter(0, &fd);
    if (rv == 0) {
        args = (int *)SOiAlloc(SO_MEM_IPC, 0x20);
        if (args == NULL) {
            rv = SO_ENOMEM;
        } else {
            args[0] = s;

            rv = IOS_Ioctl(fd, NET_SO_CLOSE, args, 4, NULL, 0);
            SOiFree(SO_MEM_IPC, args, 0x20);
        }

        rv = SOiLeave(0, rv);
    }

    return rv;
}

int SOiListen(int s, int backlog);

int SOListen(int s, int backlog) {
    int fd;
    int rv;
    int *args;

    rv = SOiEnter(0, &fd);
    if (rv == 0) {
        args = (int *)SOiAlloc(SO_MEM_IPC, 0x20);
        if (args == NULL) {
            rv = SO_ENOMEM;
        } else {
            args[0] = s;
            args[1] = backlog;

            rv = IOS_Ioctl(fd, NET_SO_LISTEN, args, 8, NULL, 0);
            SOiFree(SO_MEM_IPC, args, 0x20);
        }

        rv = SOiLeave(0, rv);
    }

    return rv;
}

int SOAccept(int s, SOSockAddr *addr) {
    int size;
    int rv;
    u8 *args;
    u8 *out;
    int fd;

    rv = SOiEnter(0, &fd);
    if (rv == 0) {
        if (addr != NULL && (addr->len > 8 || addr->len < 8)) {
            rv = SO_EINVAL;
        } else {
            size = ((addr == NULL ? 4 : addr->len + 0x20) + 0x1F) & ~0x1F;

            args = (u8 *)SOiAlloc(SO_MEM_IPC, size);
            if (args == NULL) {
                rv = SO_ENOMEM;
            } else {
                *(int *)args = s;
                out = args + 0x20;

                if (addr == NULL) {
                    rv = IOS_Ioctl(fd, NET_SO_ACCEPT, args, 4, NULL, 0);
                } else {
                    memcpy(out, addr, addr->len);
                    rv = IOS_Ioctl(fd, NET_SO_ACCEPT, args, 4, out, addr->len);

                    if (rv >= 0) {
                        memcpy(addr, out, out[0]);
                    }
                }

                SOiFree(SO_MEM_IPC, args, size);
            }
        }

        rv = SOiLeave(0, rv);
    }

    return rv;
}

int SOBind(int s, const SOSockAddr *addr) {
    int fd;
    int rv;
    int *args;

    rv = SOiEnter(0, &fd);
    if (rv == 0) {
        if (addr == NULL || addr->len > 8 || addr->len < 8) {
            rv = SO_EINVAL;
        } else {
            args = (int *)SOiAlloc(SO_MEM_IPC, 0x40);
            if (args == NULL) {
                rv = SO_ENOMEM;
            } else {
                args[0] = s;
                args[1] = 1;
                memcpy(&args[2], addr, addr->len);

                rv = IOS_Ioctl(fd, NET_SO_BIND, args, 0x24, NULL, 0);
                SOiFree(SO_MEM_IPC, args, 0x40);
            }
        }

        rv = SOiLeave(0, rv);
    }

    return rv;
}

int SOConnect(int s, const SOSockAddr *addr) {
    int fd;
    int rv;
    int *args;

    rv = SOiEnter(0, &fd);
    if (rv == 0) {
        if (addr == NULL || addr->len > 8 || addr->len < 8) {
            rv = SO_EINVAL;
        } else {
            args = (int *)SOiAlloc(SO_MEM_IPC, 0x40);
            if (args == NULL) {
                rv = SO_ENOMEM;
            } else {
                args[0] = s;
                args[1] = 1;
                memcpy(&args[2], addr, addr->len);

                rv = IOS_Ioctl(fd, NET_SO_CONNECT, args, 0x24, NULL, 0);
                SOiFree(SO_MEM_IPC, args, 0x40);
            }
        }

        rv = SOiLeave(0, rv);
    }

    return rv;
}

int SOGetSockName(int s, SOSockAddr *addr) {
    int size;
    int rv;
    u8 *args;
    SOSockAddr *out;
    int fd;

    rv = SOiEnter(0, &fd);
    if (rv == 0) {
        if (addr == NULL || addr->len > 8 || addr->len < 8) {
            rv = SO_EINVAL;
        } else {
            size = (addr->len + 0x3F) & ~0x1F;

            args = (u8 *)SOiAlloc(SO_MEM_IPC, size);
            if (args == NULL) {
                rv = SO_ENOMEM;
            } else {
                *(int *)args = s;
                out = (SOSockAddr *)(args + 0x20);
                memcpy(out, addr, addr->len);

                rv = IOS_Ioctl(fd, NET_SO_GETSOCKNAME, args, 4, out, addr->len);
                if (rv >= 0) {
                    memcpy(addr, out, out->len);
                }

                SOiFree(SO_MEM_IPC, args, size);
            }
        }

        rv = SOiLeave(0, rv);
    }

    return rv;
}

int SOShutdown(int s, int how) {
    int fd;
    int rv;
    int *args;

    rv = SOiEnter(0, &fd);
    if (rv == 0) {
        args = (int *)SOiAlloc(SO_MEM_IPC, 0x20);
        if (args == NULL) {
            rv = SO_ENOMEM;
        } else {
            args[0] = s;
            args[1] = how;

            rv = IOS_Ioctl(fd, NET_SO_SHUTDOWN, args, 8, NULL, 0);
            SOiFree(SO_MEM_IPC, args, 0x20);
        }

        rv = SOiLeave(0, rv);
    }

    return rv;
}

int SOFcntl(int s, int cmd, ...) {
    int fd;
    int rv;
    int *args;
    int flag;
    va_list va;

    va_start(va, cmd);
    flag = va_arg(va, int);
    va_end(va);

    rv = SOiEnter(0, &fd);
    if (rv == 0) {
        args = (int *)SOiAlloc(SO_MEM_IPC, 0x20);
        if (args == NULL) {
            rv = SO_ENOMEM;
        } else {
            args[0] = s;
            args[1] = cmd;
            args[2] = flag;

            rv = IOS_Ioctl(fd, NET_SO_FCNTL, args, 0xC, NULL, 0);
            SOiFree(SO_MEM_IPC, args, 0x20);
        }

        rv = SOiLeave(0, rv);
    }

    return rv;
}

int SOPoll(SOPollFD fds[], unsigned nfds, s64 timeout) {
    int size;
    int bytes;
    int rv;
    u8 *args;
    u8 *copy;
    int fd;

    rv = SOiEnter(0, &fd);
    if (rv == 0) {
        if (fds == NULL) {
            rv = SO_EINVAL;
        } else {
            bytes = nfds * sizeof(SOPollFD);
            size = (bytes + 0x3F) & ~0x1F;

            args = (u8 *)SOiAlloc(SO_MEM_IPC, size);
            if (args == NULL) {
                rv = SO_ENOMEM;
            } else {
                copy = args + 0x20;

                if (timeout <= -1) {
                    memcpy(args, &timeout, 8);
                } else {
                    s64 ms = timeout / (OS_TIMER_CLOCK / 1000);
                    *(s64 *)args = ms;
                }

                memcpy(copy, fds, bytes);
                rv = IOS_Ioctl(fd, NET_SO_POLL, args, 8, copy, bytes);

                if (rv >= 0) {
                    memcpy(fds, copy, bytes);
                }

                SOiFree(SO_MEM_IPC, args, size);
            }
        }

        rv = SOiLeave(0, rv);
    }

    return rv;
}

int SOGetHostID(void) {
    int fd;
    int enter;
    int rv = 0;

    enter = SOiEnter(0, &fd);
    if (enter == 0) {
        rv = IOS_Ioctl(fd, NET_SO_GETHOSTID, NULL, 0, NULL, 0);
        SOiLeave(0, enter);
    }

    return rv;
}

// true when [buf, buf + len) can go straight to the ipc layer without a bounce copy
static inline BOOL SOiCanDmaDirect(void *buf, int len) {
    BOOL direct = TRUE;

    if (len != 0) {
        BOOL ok = FALSE;
        BOOL aligned = FALSE;

        if (((u32)buf & 0x1F) == 0 && (len % 32) == 0) {
            aligned = TRUE;
        }

        if (aligned) {
            BOOL mem2ok = TRUE;

            if (SOiGetCheckMem2()) {
                BOOL inRange = FALSE;
                u32 addr = (u32)buf & 0x1FFFFFFF;

                if (addr >= 0x10000000 && addr < 0x18000000) {
                    inRange = TRUE;
                }

                if (!inRange) {
                    mem2ok = FALSE;
                }
            }

            if (mem2ok) {
                ok = TRUE;
            }
        }

        if (!ok) {
            direct = FALSE;
        }
    }

    return direct;
}

int SOiRecvFrom(int unused, int s, void *buf, int len, int flags, SOSockAddr *from) {
    IOSIoVector *v;
    u8 *data;
    int argSize;
    int fd;
    u8 *args;
    BOOL direct;
    int rv;
    u8 *addr;

    if (len > 0x8000) {
        len = 0x8000;
    }

    rv = SOiEnter(unused, &fd);
    if (rv == 0) {
        if (from != NULL && (from->len > 8 || from->len < 8)) {
            rv = SO_EINVAL;
        } else if (len < 0 || (len > 0 && buf == NULL)) {
            rv = SO_EINVAL;
        } else {
            direct = SOiCanDmaDirect(buf, len);

            argSize = ((from == NULL ? 0 : from->len) + 0x5F) & ~0x1F;

            args = (u8 *)SOiAlloc(SO_MEM_IPC, argSize);
            if (direct) {
                data = (u8 *)buf;
            } else {
                data = (u8 *)SOiAlloc(13, (len + 0x1F) & ~0x1F);
            }

            if (args == NULL || data == NULL) {
                rv = SO_ENOMEM;
            } else {
                v = (IOSIoVector *)args;
                *(int *)(args + 0x20) = s;
                *(int *)(args + 0x24) = flags;
                addr = args + 0x40;

                v[0].base = args + 0x20;
                v[0].length = 8;
                v[1].base = data;
                v[1].length = len;

                if (from == NULL) {
                    v[2].base = NULL;
                    v[2].length = 0;
                    rv = IOS_Ioctlv(fd, NET_SO_RECVFROM, 1, 2, v);
                } else {
                    memcpy(addr, from, from->len);
                    v[2].base = addr;
                    v[2].length = from->len;
                    rv = IOS_Ioctlv(fd, NET_SO_RECVFROM, 1, 2, v);

                    if (rv >= 0) {
                        u32 n = from->len;

                        if (n > addr[0]) {
                            n = addr[0];
                        }

                        memcpy(from, addr, n);
                    }
                }

                if (rv >= 0 && !direct) {
                    memcpy(buf, data, len);
                }
            }

            if (!direct) {
                SOiFree(13, data, (len + 0x1F) & ~0x1F);
            }

            SOiFree(SO_MEM_IPC, args, argSize);
        }

        rv = SOiLeave(unused, rv);
    }

    return rv;
}

int SOiSendTo(int unused, int s, const void *buf, int len, int flags, const SOSockAddr *to) {
    int fd;
    int rv;
    BOOL direct;
    u8 *data;
    u8 *args;

    rv = SOiEnter(unused, &fd);
    if (rv == 0) {
        if (to != NULL && (to->len > 8 || to->len < 8)) {
            rv = SO_EINVAL;
        } else if (len < 0 || (len > 0 && buf == NULL)) {
            rv = SO_EINVAL;
        } else {
            direct = SOiCanDmaDirect((void *)buf, len);

            args = (u8 *)SOiAlloc(SO_MEM_IPC, 0x60);
            if (direct) {
                data = (u8 *)buf;
            } else {
                data = (u8 *)SOiAlloc(14, (len + 0x1F) & ~0x1F);
            }

            if (args == NULL || data == NULL) {
                rv = SO_ENOMEM;
            } else {
                // args + 0x00: two iovecs, args + 0x20: the NETSoSend block (sock, flags, validAddr, addr)
                *(int *)(args + 0x20) = s;
                *(int *)(args + 0x24) = flags;

                if (to == NULL) {
                    *(int *)(args + 0x28) = 0;
                } else {
                    *(int *)(args + 0x28) = 1;
                    memcpy(args + 0x2C, to, to->len);
                }

                if (!direct) {
                    memcpy(data, buf, len);
                }

                ((IOSIoVector *)args)[0].base = data;
                ((IOSIoVector *)args)[0].length = len;
                ((IOSIoVector *)args)[1].base = args + 0x20;
                ((IOSIoVector *)args)[1].length = 0x28;
                rv = IOS_Ioctlv(fd, NET_SO_SENDTO, 2, 0, (IOSIoVector *)args);
            }

            if (!direct) {
                SOiFree(14, data, (len + 0x1F) & ~0x1F);
            }

            SOiFree(SO_MEM_IPC, args, 0x60);
        }

        rv = SOiLeave(unused, rv);
    }

    return rv;
}

int SORecvFrom(int s, void *buf, int len, int flags, SOSockAddr *from) {
    return SOiRecvFrom(0, s, buf, len, flags, from);
}

int SORecv(int s, void *buf, int len, int flags) {
    return SOiRecvFrom(0, s, buf, len, flags, NULL);
}

int SOSendTo(int s, const void *buf, int len, int flags, const SOSockAddr *to) {
    return SOiSendTo(0, s, buf, len, flags, to);
}

int SOSend(int s, const void *buf, int len, int flags) {
    return SOiSendTo(0, s, buf, len, flags, NULL);
}

int SOGetSockOpt(int s, int level, int optname, void *optval, int *optlen) {
    int fd;
    int rv;
    int *args;

    rv = SOiEnter(0, &fd);
    if (rv == 0) {
        args = (int *)SOiAlloc(SO_MEM_IPC, 0x20);
        if (args == NULL) {
            rv = SO_ENOMEM;
        } else {
            args[0] = s;
            args[1] = level;
            args[2] = optname;

            rv = IOS_Ioctl(fd, NET_SO_GETSOCKOPT, NULL, 0, args, 0x18);
            if (rv >= 0 && optlen != NULL) {
                if (*optlen >= args[3]) {
                    if (optval != NULL) {
                        memcpy(optval, &args[4], args[3]);
                    }

                    *optlen = args[3];
                } else {
                    *optlen = args[3];
                    rv = SO_EINVAL;
                }
            }

            SOiFree(SO_MEM_IPC, args, 0x20);
        }

        rv = SOiLeave(0, rv);
    }

    return rv;
}

int SOSetSockOpt(int s, int level, int optname, const void *optval, int optlen) {
    int fd;
    int rv;
    int *args;

    rv = SOiEnter(0, &fd);
    if (rv == 0) {
        if (optlen < 0 || optlen > 0x14) {
            rv = SO_EINVAL;
        } else {
            args = (int *)SOiAlloc(SO_MEM_IPC, 0x40);
            if (args == NULL) {
                rv = SO_ENOMEM;
            } else {
                args[0] = s;
                args[1] = level;
                args[2] = optname;
                args[3] = optlen;

                if (optval != NULL) {
                    memcpy(&args[4], optval, optlen);
                } else {
                    memset(&args[4], 0, optlen);
                }

                rv = IOS_Ioctl(fd, NET_SO_SETSOCKOPT, args, 0x24, NULL, 0);
                SOiFree(SO_MEM_IPC, args, 0x40);
            }
        }

        rv = SOiLeave(0, rv);
    }

    return rv;
}

char *SOInetNtoA(SOInAddr in) {
    u8 *p = (u8 *)&in;

    sprintf(ntoaBuf, "%d.%d.%d.%d", p[0], p[1], p[2], p[3]);
    return ntoaBuf;
}

u32 SONtoHl(u32 v) {
    return v;
}

u16 SONtoHs(u16 v) {
    return v;
}

u32 SOHtoNl(u32 v) {
    return v;
}

u16 SOHtoNs(u16 v) {
    return v;
}

// the nwc24 scheduler (kd) has to be told when the network goes up or down; these four live in
// nwc24.c and are plain "no parameter" scheduler commands 6..9
extern int NWC24iExecCmd6(int *out);
extern int NWC24iExecCmd7(int *out);
extern int NWC24iExecCmd8(void);
extern int NWC24iExecCmd9(void);

// HYPOTHESIS: reads the link state from the net config lib (ncd). 3..5 means the link is usable
extern int NCDGetLinkStatus(void);

#define SO_ERR_LINK_DOWN -112

SOHostEnt *SOGetHostByName(const char *name) {
    SOHostEnt *ent;
    char *buf;
    int rv;
    int size;
    int fd;
    int len;
    SOHostEnt *result = NULL;

    rv = SOiEnter(0, &fd);
    if (rv == 0) {
        if (name == NULL) {
            rv = SO_EINVAL;
        } else {
            len = strlen(name);
            size = (len + 0x20) & ~0x1F;

            buf = (char *)SOiAlloc(SO_MEM_IPC, size);
            ent = (SOHostEnt *)SOiGetLib()->scratch;

            if (buf == NULL) {
                rv = SO_ENOMEM;
            } else {
                strcpy(buf, name);

                rv = IOS_Ioctl(fd, NET_SO_GETHOSTBYNAME, buf, len + 1, ent, 0x460);
                if (rv >= 0) {
                    // the stack filled it in with its own addresses, shift every pointer over to ours
                    int diff = (int)((u8 *)ent + 0x10) - (int)ent->name;
                    int *p = (int *)((u8 *)ent + 0x340);

                    while (*p != 0) {
                        *p += diff;
                        p++;
                    }

                    ent->aliases = (char **)((int)ent->aliases + diff);
                    ent->name = (char *)((int)ent->name + diff);
                    ent->addrList = (u8 **)((int)ent->addrList + diff);
                    result = ent;
                }

                SOiFree(SO_MEM_IPC, buf, size);
            }
        }

        SOiLeave(0, rv);
    }

    return result;
}

void SOFreeAddrInfo(SOAddrInfo *ai) {
    BOOL enabled = OSDisableInterrupts();

    if (SOiIsInitialized() == TRUE && ai != NULL) {
        SOiFree(SO_MEM_ADDRINFO, ai, 0x840);
    }

    OSRestoreInterrupts(enabled);
}

// maps a kd/net-config failure into the matching SO error, same table in startup, cleanup and enter-config
static inline int SOiMapKdError(int r, int out) {
    int rv = SO_EINVAL;

    switch (r + 0x21) {
    case 0x21:
        rv = 0;
        break;
    case 0x0B:
    case 0x14:
        rv = -0x30;
        break;
    case 0x00:
    case 0x1F:
        rv = out;
        break;
    case 0x20:
        rv = SO_ERR_NETRM_NOTAVAIL;
        break;
    case 0x04:
        rv = SO_EINPROGRESS;
        break;
    }

    return rv;
}

// undoes a temporary start made by SOiEnterConfig when flag == 1, then records rv
int SOiLeaveConfig(int unused, int rv, int flag) {
    BOOL enabled;
    int r;

    if (flag == 1) {
        r = NWC24iExecCmd9();

        switch (r) {
        case -0x1D:
            rv = SO_EINPROGRESS;
            break;
        case 0:
            break;
        case -1:
        default:
            rv = SO_ERR_NETRM_NOTAVAIL;
            break;
        }

        if (IOS_Close(lib.fd) < 0) {
            rv = SO_ERR_NETRM_NOTAVAIL;
        }

        enabled = OSDisableInterrupts();

        if (rv != SO_ERR_NETRM_NOTAVAIL) {
            lib.fd = -1;
            lib.status = -2;
        }
    } else {
        enabled = OSDisableInterrupts();
    }

    SO_SAVE_ERROR(rv);
    OSRestoreInterrupts(enabled);
    return rv;
}

// like SOiEnter, but if the stack hasn't been started this does a temporary start of the ip
// resource manager so config calls still work (flag is set to 1 in that case)
int SOiEnterConfig(int unused, int *fd, int *flag) {
    int rv = 0;
    BOOL enabled = OSDisableInterrupts();
    int r;
    int link;

    switch (state) {
    case 0:
        rv = SO_ENETRESET;
        break;
    default:
        if (lib.status > -2) {
            rv = SO_EBUSY;
            break;
        }

        if (OSGetCurrentThread() == NULL) {
            rv = SO_ERR_NETRM_NOTAVAIL;
            break;
        }

        lib.status = -1;
        OSRestoreInterrupts(enabled);

        lib.fd = IOS_Open("/dev/net/ip/top", 0);
        if (lib.fd < 0) {
            enabled = OSDisableInterrupts();

            if (lib.fd == -6) {
                lib.status = -2;
                rv = SO_EINPROGRESS;
            } else {
                rv = SO_ERR_NETRM_NOTAVAIL;
            }
            break;
        }

        *fd = lib.fd;
        r = NWC24iExecCmd8();
        if (r == 0) {
            link = NCDGetLinkStatus();
            switch (link) {
            case 3:
            case 4:
            case 5:
                rv = IOS_Ioctl(lib.fd, NET_SO_INIT, NULL, 0, NULL, 0);
                if (rv == 0) {
                    *flag = 1;
                } else {
                    rv = SOiLeaveConfig(unused, rv, 1);
                }
                break;
            case -8:
                rv = SOiLeaveConfig(unused, SO_EINPROGRESS, 1);
                break;
            case -2:
            case -1:
                rv = SOiLeaveConfig(unused, SO_ERR_NETRM_NOTAVAIL, 1);
                break;
            default:
                rv = SOiLeaveConfig(unused, -0x30, 1);
                break;
            }

            enabled = OSDisableInterrupts();
        } else {
            // dense on purpose: the original is a jump table over every kd error code
            switch (r) {
            case -0x1D:
                rv = SO_EINPROGRESS;
                break;
            case -28:
            case -27:
            case -26:
            case -25:
            case -24:
            case -23:
            case -22:
            case -21:
            case -20:
            case -19:
            case -18:
            case -17:
            case -16:
            default:
                rv = SO_ERR_NETRM_NOTAVAIL;
                break;
            case -0xF:
                rv = -0x30;
                break;
            case -2:
                rv = SO_ENETRESET;
                break;
            }

            if (IOS_Close(lib.fd) < 0) {
                rv = SO_ERR_NETRM_NOTAVAIL;
            }

            enabled = OSDisableInterrupts();
            if (rv != SO_ERR_NETRM_NOTAVAIL) {
                lib.fd = -1;
                lib.status = -2;
            }
        }
        break;
    case 2:
        if (lib.status < 0) {
            rv = SO_EBUSY;
        } else if (OSGetCurrentThread() == NULL) {
            rv = SO_ERR_NETRM_NOTAVAIL;
        } else {
            *flag = 0;
            *fd = lib.fd;
        }
        break;
    }

    if (rv != 0) {
        SO_SAVE_ERROR(rv);
    }

    OSRestoreInterrupts(enabled);
    return rv;
}

int SOiGetInterfaceOpt(int unused, int level, int optname, void *optval, int *optlen) {
    int *hdr;
    u8 *data;
    int fd;
    int len;
    int size;
    int flag;
    u8 *args;
    int *lenBuf;
    int rv;

    rv = SOiEnterConfig(0, &fd, &flag);
    if (rv == 0) {
        if ((u32)(optname - 0x1001) <= 1) {
            rv = SO_EINVAL;
        } else {
            len = (optlen == NULL || *optlen < 0) ? 0 : *optlen;
            size = (len + 0x7F) & ~0x1F;

            args = (u8 *)SOiAlloc(SO_MEM_IPC, size);
            if (args == NULL) {
                rv = SO_ENOMEM;
            } else {
                hdr = (int *)(args + 0x20);
                lenBuf = (int *)(args + 0x40);
                data = args + 0x60;

                hdr[0] = level;
                hdr[1] = optname;
                *lenBuf = (optlen == NULL || *optlen < 0) ? 0 : *optlen;

                ((IOSIoVector *)args)[0].base = (u8 *)hdr;
                ((IOSIoVector *)args)[0].length = 8;
                ((IOSIoVector *)args)[1].base = data;
                ((IOSIoVector *)args)[1].length = (optlen == NULL || *optlen < 0) ? 0 : *optlen;
                ((IOSIoVector *)args)[2].base = (u8 *)lenBuf;
                ((IOSIoVector *)args)[2].length = 4;

                rv = IOS_Ioctlv(fd, 0x1C, 1, 2, (IOSIoVector *)args);
                if (rv >= 0 && optlen != NULL) {
                    if (*optlen >= *lenBuf) {
                        if (optval != NULL) {
                            memcpy(optval, data, *lenBuf);
                        }

                        *optlen = *lenBuf;
                    } else {
                        *optlen = *lenBuf;
                        rv = SO_EINVAL;
                    }
                }

                SOiFree(SO_MEM_IPC, args, size);
            }
        }

        rv = SOiLeaveConfig(0, rv, flag);
    }

    return rv;
}

int SOInetAtoN(const char *cp, SOInAddr *inp) {
    int size;
    int rv;
    u8 *args;
    u8 *str;
    int fd;
    int flag;

    rv = SOiEnterConfig(0, &fd, &flag);
    if (rv == 0) {
        if (cp == NULL) {
            rv = SO_EINVAL;
        } else {
            size = (strlen(cp) + 0x40) & ~0x1F;

            args = (u8 *)SOiAlloc(SO_MEM_IPC, size);
            if (args == NULL) {
                rv = SO_ENOMEM;
            } else {
                str = args + 0x20;
                if (cp != NULL) {
                    strcpy((char *)str, cp);
                }

                rv = IOS_Ioctl(fd, NET_SO_INETATON, str, strlen(cp), args, 4);
                if (rv >= 0 && inp != NULL) {
                    memcpy(inp, args, 4);
                }

                SOiFree(SO_MEM_IPC, args, size);
            }
        }

        rv = SOiLeaveConfig(0, rv, flag);
    }

    return rv;
}

#define TICKS_PER_MS (OS_TIMER_CLOCK / 1000)

// polls the stack's config error / host id every 10ms until it has an address (or the deadline passes)
int SOiWaitConfig(u32 timeoutMs) {
    int rv = 0;
    s64 deadline = 0;
    int optval;
    int optlen;

    if (timeoutMs != 0) {
        deadline = __OSGetSystemTime() + timeoutMs * TICKS_PER_MS;
    }

    for (;;) {
        int r;

        OSSleepTicks(OSMillisecondsToTicks((s64)10));

        optlen = 4;
        r = SOiGetInterfaceOpt(0, 0xFFFE, 0x1003, &optval, &optlen);
        if (r != 0) {
            rv = r;
            break;
        }

        if (r == 0 && optval != 0) {
            rv = optval;
            break;
        }

        if (SOGetHostID() != 0) {
            break;
        }

        if (deadline != 0 && __OSGetSystemTime() > deadline) {
            rv = -0x4C;
            break;
        }
    }

    return rv;
}

int SOStartupEx(u32 timeoutMs) {
    s64 deadline = 0;
    int retries = 4;
    int rv;
    int out;

    if (timeoutMs != 0) {
        deadline = __OSGetSystemTime() + timeoutMs * TICKS_PER_MS;
    }

    do {
        BOOL enabled = OSDisableInterrupts();
        int link;
        int r;

        rv = 0;

        switch (state) {
        case 1:
            if (lib.status > -2) {
                rv = SO_EBUSY;
            } else if (OSGetCurrentThread() == NULL) {
                rv = SO_ERR_NETRM_NOTAVAIL;
            } else {
                lib.status = -1;
                OSRestoreInterrupts(enabled);

                // wait for the link
                for (;;) {
                    link = NCDGetLinkStatus();
                    if (link != -8 && link != 1) {
                        break;
                    }

                    OSSleepTicks(OSMillisecondsToTicks((s64)10));
                    if (deadline != 0 && __OSGetSystemTime() > deadline) {
                        rv = (link == 1) ? -0x79 : SO_ERR_NETRM_NOTAVAIL;
                        break;
                    }
                }

                if (rv == 0) {
                    if (link < 0) {
                        rv = SO_ERR_NETRM_NOTAVAIL;
                    } else if (link == 2) {
                        rv = -0x2D;
                    } else {
                        // open the ip resource manager, it may still be busy coming up
                        for (;;) {
                            lib.fd = IOS_Open("/dev/net/ip/top", 0);
                            if (lib.fd != -6) {
                                break;
                            }

                            OSSleepTicks(OSMillisecondsToTicks((s64)10));
                            if (deadline != 0 && __OSGetSystemTime() > deadline) {
                                rv = SO_ERR_NETRM_NOTAVAIL;
                                break;
                            }
                        }

                        if (rv == 0 && lib.fd < 0) {
                            rv = SO_ERR_NETRM_NOTAVAIL;
                        }

                        if (rv == 0) {
                            out = 0;

                            for (;;) {
                                r = NWC24iExecCmd6(&out);
                                if (r != -0x1D) {
                                    break;
                                }

                                OSSleepTicks(OSMillisecondsToTicks((s64)10));
                                if (deadline != 0 && __OSGetSystemTime() > deadline) {
                                    rv = SO_ERR_NETRM_NOTAVAIL;
                                    break;
                                }
                            }

                            if (rv == 0) {
                                rv = SOiMapKdError(r, out);
                            }

                            if (rv != 0) {
                                if (IOS_Close(lib.fd) < 0) {
                                    rv = SO_ERR_NETRM_NOTAVAIL;
                                } else {
                                    lib.fd = -1;
                                }
                            }
                        }
                    }
                }

                enabled = OSDisableInterrupts();
                if (rv == 0) {
                    state = 2;
                    lib.status = 0;
                } else {
                    state = 1;

                    if (rv != SO_ERR_NETRM_NOTAVAIL) {
                        lib.status = -2;
                    }
                }
            }
            break;
        case 2:
            rv = SO_EALREADY;
            break;
        default:
            rv = SO_ENETRESET;
            break;
        }

        SO_SAVE_ERROR(rv);
        OSRestoreInterrupts(enabled);

        if (rv == 0) {
            s64 remaining;

            if (deadline != 0) {
                remaining = deadline - __OSGetSystemTime();
            } else {
                remaining = 0;
            }

            if (deadline != 0 && remaining <= 0) {
                rv = -0x4C;
            } else {
                rv = SOiWaitConfig((u32)(remaining / TICKS_PER_MS));
            }

            if (rv != 0) {
                SOCleanup();
            }
        }

        SO_SAVE_ERROR(rv);
    } while (rv == -0x70 && retries-- > 0);

    return rv;
}

int SOCleanup(void) {
    int rv = 0;
    int out;
    int r;
    BOOL enabled = OSDisableInterrupts();

    out = 0;
    switch (state) {
    case 1:
        rv = SO_ENETRESET;
        break;
    case 0:
    default:
        rv = SO_EALREADY;
        break;
    case 2:
        if (lib.status < 0) {
            rv = SO_EBUSY;
        } else if (OSGetCurrentThread() == NULL) {
            rv = SO_ERR_NETRM_NOTAVAIL;
        } else {
            lib.status = -1;
            OSRestoreInterrupts(enabled);

            r = NWC24iExecCmd7(&out);
            rv = SOiMapKdError(r, out);

            if (rv == 0) {
                if (IOS_Close(lib.fd) < 0) {
                    rv = SO_ERR_NETRM_NOTAVAIL;
                } else {
                    lib.fd = -1;
                }
            }

            enabled = OSDisableInterrupts();
            if (rv == 0) {
                state = 1;
                lib.status = -2;
            } else if (rv != SO_ERR_NETRM_NOTAVAIL) {
                state = 2;
                lib.status = 0;
            }
        }
        break;
    }

    SO_SAVE_ERROR(rv);
    OSRestoreInterrupts(enabled);
    return rv;
}

int SOGetAddrInfo(const char *node, const char *serv, const SOAddrInfo *hints, SOAddrInfo **res) {
    int rv;
    u8 *nodeBuf;
    u8 *servBuf;
    int size;
    int nodeLen;
    u8 *ai;
    u8 *pool;
    int servLen;
    SOAddrInfo *it;
    u8 *args;
    u8 *hintBuf;
    int fd;

    rv = SOiEnter(0, &fd);
    if (rv == 0) {
        // the sdk sizes the service string off the *node* name, which only works because they are short
        servLen = (serv == NULL) ? 0 : strlen(node) + 1;
        nodeLen = (node == NULL) ? 0 : strlen(node) + 1;

        size = ((((nodeLen + 0x1F) & ~0x1F) + ((servLen + 0x1F) & ~0x1F)) + 0x5F) & ~0x1F;

        args = (u8 *)SOiAlloc(SO_MEM_IPC, size);
        if (args == NULL) {
            rv = SO_ENOMEM;
        } else {
            ai = (u8 *)SOiAlloc(SO_MEM_ADDRINFO, 0x840);
            if (ai == NULL) {
                SOiFree(SO_MEM_IPC, args, size);
                rv = SO_ENOMEM;
            } else {
                nodeBuf = args + 0x20;
                nodeLen = (node == NULL) ? 0 : strlen(node) + 1;
                servBuf = nodeBuf + ((nodeLen + 0x1F) & ~0x1F);
                servLen = (serv == NULL) ? 0 : strlen(node) + 1;
                hintBuf = servBuf + ((servLen + 0x1F) & ~0x1F);

                if (node != NULL) {
                    strcpy((char *)nodeBuf, node);
                }

                ((u8 **)args)[0] = (node != NULL) ? nodeBuf : NULL;
                ((int *)args)[1] = (node == NULL) ? 0 : strlen(node);

                if (serv != NULL) {
                    strcpy((char *)servBuf, serv);
                }

                ((u8 **)args)[2] = (serv != NULL) ? servBuf : NULL;
                ((int *)args)[3] = (serv == NULL) ? 0 : strlen(serv);

                if (hints != NULL) {
                    memcpy(hintBuf, hints, 0x20);
                } else {
                    memset(hintBuf, 0, 0x20);
                }

                if (((SOAddrInfo *)hintBuf)->family == 0) {
                    ((SOAddrInfo *)hintBuf)->family = 2;
                }

                if (((SOAddrInfo *)hintBuf)->family == 0x17) {
                    *res = NULL;
                    rv = -0x44;
                    SOiFree(SO_MEM_ADDRINFO, ai, 0x840);
                } else {
                    ((u8 **)args)[4] = hintBuf;
                    ((int *)args)[5] = 0x20;
                    ((u8 **)args)[6] = ai;
                    ((int *)args)[7] = 0x834;

                    rv = IOS_Ioctlv(fd, NET_SO_GETADDRINFO, 3, 1, (IOSIoVector *)args);
                    if (rv >= 0) {
                        *res = (SOAddrInfo *)ai;

                        // the stack wrote this with its own pointers, point everything at our copy
                        it = (SOAddrInfo *)ai;
                        pool = ai + 0x460;
                        while (it != NULL) {
                            it->addr = (SOSockAddr *)pool;
                            if (it->next != NULL) {
                                it->next = (SOAddrInfo *)((u8 *)it + 0x20);
                            }

                            it = it->next;
                            pool += 0x1C;
                        }
                    } else {
                        *res = NULL;
                        SOiFree(SO_MEM_ADDRINFO, ai, 0x840);
                    }
                }

                SOiFree(SO_MEM_IPC, args, size);
            }
        }

        rv = SOiLeave(0, rv);
    }

    return rv;
}
