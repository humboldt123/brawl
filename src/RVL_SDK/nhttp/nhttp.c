#include <types.h>
#include <revolution/OS/OSMessage.h>
#include <revolution/NHTTP/d_nhttp.h>

// HYPOTHESIS: untyped placeholders for the zero-initialized .bss of this unit, sizes and order from the original layout.
unsigned char lbl_805365E0[0x40] __attribute__((aligned(8))) = { 0 };
unsigned char lbl_80536620[0xca0] __attribute__((aligned(8))) = { 0 };
// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
unsigned char lbl_805A1148[0x8] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A1150[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A1154[0x4] __attribute__((aligned(4))) = { 0 };

// Function bodies below are taken from https://github.com/jj-link/brawl.

void fn_80390A58(OSMessageQueue* queue) {
    OSSendMessage(queue, 0, 0);
}

void fn_803911BC(int *p) {
    *p = 0;
}

extern void fn_80392738(void *, int);
extern void fn_8039272C(void *, char *, int);
extern int SOSend(int, void *, unsigned int, int);

int fn_80392420(int a, char *buf, unsigned int len, int d) {
    char tmp[32] __attribute__((aligned(32)));
    unsigned int head = (unsigned int)buf & 31 ? 32 - ((unsigned int)buf & 31) : 0;
    int total = 0;
    unsigned int n;
    int r;
    fn_80392738(tmp, 32);
    if (head != 0) {
        if (head > len) head = len;
        fn_8039272C(tmp, buf, head);
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
        fn_80392738(tmp, 32);
        fn_8039272C(tmp, buf, n);
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

int fn_803928F4(unsigned char *out, s8 in)
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

s32 fn_80392AB8(u8 *arg0, s32 arg1) {
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

int fn_80392D88(unsigned char *arg0, int arg1) {
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

void fn_80396EEC(int, int, int);

void fn_803970CC(int a, int b) {
    fn_80396EEC(a, b, 1);
}

void fn_80396EEC(int a, int b, int c);

void fn_803970D4(int a, int b, int c) {
    fn_80396EEC(a, b, 2);
}

void fn_80396EEC(int a, int b, int c);

void fn_803970DC(int a, int b) {
    fn_80396EEC(a, b, 0);
}
