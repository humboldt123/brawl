#include <types.h>
#include <stdio.h>
#include <string.h>
#include <GameSpy/qr2/qr2.h>

// HYPOTHESIS: untyped placeholders for the zero-initialized .bss of this unit, sizes and order from the original layout.
unsigned char lbl_80535A80[0x18] __attribute__((aligned(8))) = { 0 };
unsigned char lbl_80535A98[0x48] __attribute__((aligned(8))) = { 0 };
unsigned char lbl_80535AE0[0x100] __attribute__((aligned(8))) = { 0 };
// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
unsigned char lbl_805A1058[0x8] __attribute__((aligned(4))) = { 0 };

// Function bodies below are derived from https://github.com/jj-link/brawl, where each
// one was matched in a translation unit of its own; they follow the GameSpy SDK's
// qr2.c.

void fn_80378E24(qr2_t qrec, qr2_callback_t nncallback) {
    if (qrec == NULL) {
        qrec = lbl_8059F9A8;
    }
    qrec->nn_callback = nncallback;
}

void fn_80378E38(qr2_t qrec, qr2_callback_t cmcallback) {
    if (qrec == NULL) {
        qrec = lbl_8059F9A8;
    }
    qrec->cm_callback = cmcallback;
}

void fn_80378E4C(qr2_t qrec, qr2_callback_t pacallback) {
    if (qrec == NULL) {
        qrec = lbl_8059F9A8;
    }
    qrec->pa_callback = pacallback;
}

void fn_803791BC(qr2_t qrec) {
    if (qrec == NULL) {
        qrec = lbl_8059F9A8;
    }
    if (qrec->ispublic) {
        fn_8037AC14(qrec, 2);
    }
    if (qrec->hbsock != -1 && qrec->read_socket) {
        closesocket(qrec->hbsock);
    }
    qrec->hbsock = -1;
    qrec->lastheartbeat = 0;
    if (qrec->read_socket) {
        fn_8035D724();
    }
    if (qrec != &lbl_8048FBE0) {
        fn_8035E00C(qrec);
    }
}

int fn_8037925C(qr2_keybuffer_t keybuffer, int keyid) {
    if (keybuffer->numkeys >= MAX_REGISTERED_KEYS) {
        return 0;
    }
    if (keyid < 1 || keyid > MAX_REGISTERED_KEYS) {
        return 0;
    }
    keybuffer->keys[keybuffer->numkeys++] = (u8)keyid;
    return 1;
}

static inline int qr2_buffer_add_inline(qr2_buffer_t outbuf, const char* value) {
    int copylen;

    copylen = strlen(value) + 1;
    if (copylen > MAX_DATA_SIZE - outbuf->len) {
        copylen = MAX_DATA_SIZE - outbuf->len;
    }
    if (copylen <= 0) {
        return 0;
    }
    memcpy(outbuf->buffer + outbuf->len, value, copylen);
    outbuf->len += copylen;
    outbuf->buffer[outbuf->len - 1] = 0;
    return 1;
}

int fn_8037929C(qr2_buffer_t outbuf, int value) {
    char temp[32];

    sprintf(temp, lbl_8059F9AC, value);
    return qr2_buffer_add_inline(outbuf, temp);
}

int fn_8037933C(qr2_buffer_t outbuf, const char* value) {
    return qr2_buffer_add_inline(outbuf, value);
}
