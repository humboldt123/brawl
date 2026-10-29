#include <types.h>
#include <GameSpy/natneg/natneg.h>

// HYPOTHESIS: untyped placeholders for the zero-initialized .bss of this unit, sizes and order from the original layout.
unsigned char lbl_80535FE0[0x40] __attribute__((aligned(8))) = { 0 };
unsigned char lbl_80536020[0x200] __attribute__((aligned(8))) = { 0 };
NAT lbl_80536220 = { 0 };  // natify's detection result
unsigned char lbl_80536300[0x200] __attribute__((aligned(8))) = { 0 };
// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
unsigned char lbl_805A10D0[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A10D4[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A10D8[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A10DC[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A10E0[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A10E4[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A10E8[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A10EC[0x4] __attribute__((aligned(4))) = { 0 };
DArray lbl_805A10F0 = NULL;
unsigned char lbl_805A10F4[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A10F8[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A10FC[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A1100[0x4] __attribute__((aligned(4))) = { 0 };
int lbl_805A1104 = 0;  // detected mapping scheme
u32 lbl_805A1108 = 0;  // detection start time
NatDetectionResultsFunc lbl_805A110C = NULL;
unsigned char lbl_805A1110[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A1114[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A1118[0x8] __attribute__((aligned(4))) = { 0 };

// mapping and ERT sockets and the detected NAT type (natify.c)
extern int lbl_8059FB70;
extern int lbl_8059FB74;
extern int lbl_8059FB78;

// Function bodies below are derived from https://github.com/jj-link/brawl, where each
// one was matched in a translation unit of its own; they follow the GameSpy SDK's
// natneg.c and natify.c.

u32 fn_8038563C(void) {
    int i;
    gs_hostent* hp;
    u32 addr;
    u32* paddr;
    u32 lastAddr = 0;

    hp = fn_8035CF8C();
    if (hp == NULL) {
        return 0;
    }
    i = 0;
    while (1) {
        paddr = (u32*)hp->h_addr_list[i];
        if (paddr == NULL) {
            break;
        }
        addr = *paddr;
        if (addr != SOHtoNl(0x7F000001)) {
            lastAddr = addr;
            if (fn_8035D140(paddr)) {
                return addr;
            }
        }
        i++;
    }
    return lastAddr;
}

void fn_80385CC4(void) {
    if (lbl_805A10F0 != NULL) {
        fn_8035A4FC(lbl_805A10F0);
        lbl_805A10F0 = NULL;
    }
}

u32 fn_80385CF8(void) {
    int i;
    gs_hostent* hp;
    u32 addr;
    u32* paddr;
    u32 lastAddr = 0;

    hp = fn_8035CF8C();
    if (hp == NULL) {
        return 0;
    }
    i = 0;
    while (1) {
        paddr = (u32*)hp->h_addr_list[i];
        if (paddr == NULL) {
            break;
        }
        addr = *paddr;
        if (addr != SOHtoNl(0x7F000001)) {
            lastAddr = addr;
            if (fn_8035D140(paddr)) {
                return addr;
            }
        }
        i++;
    }
    return lastAddr;
}

int fn_80386238(int sock) {
    int result = 1;

    if (sock != -1) {
        if (fn_8035D728() - lbl_805A1108 < 10000) {
            result = fn_80385A00(sock, &lbl_80536220);
        } else {
            result = 0;
        }
        if (!result) {
            int success = fn_80385A04(&lbl_80536220);
            lbl_805A110C(success, lbl_80536220);
            lbl_8059FB78 = lbl_80536220.natType;
            lbl_805A1104 = lbl_80536220.mappingScheme;
            if (lbl_8059FB70 != -1) {
                closesocket(lbl_8059FB70);
            }
            lbl_8059FB70 = -1;
            if (lbl_8059FB74 != -1) {
                closesocket(lbl_8059FB74);
            }
            lbl_8059FB74 = -1;
        }
    }
    return result;
}

static inline NATNegotiator FindNegotiatorForCookie(int cookie) {
    int i;

    if (lbl_805A10F0 == NULL) {
        return NULL;
    }
    for (i = 0; i < fn_8035A590(lbl_805A10F0); i++) {
        NATNegotiator neg = fn_8035A598(lbl_805A10F0, i);
        if (neg->cookie == cookie) {
            return neg;
        }
    }
    return NULL;
}

static inline void NNCancelInline(int cookie) {
    NATNegotiator neg = FindNegotiatorForCookie(cookie);

    if (neg == NULL) {
        return;
    }
    if (neg->negotiateSock != -1) {
        closesocket(neg->negotiateSock);
    }
    neg->negotiateSock = -1;
    neg->state = ns_canceled;
}

void fn_80386658(int cookie) {
    NNCancelInline(cookie);
}
