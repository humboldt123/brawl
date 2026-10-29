#include <types.h>
#include <GameSpy/qr2/qr2regkeys.h>

// Function body derived from https://github.com/jj-link/brawl; it follows the
// GameSpy SDK's qr2regkeys.c.

void fn_8037B354(int keyid, const char* key) {
    if (keyid < NUM_RESERVED_KEYS || keyid > MAX_REGISTERED_KEYS) {
        return;
    }
    lbl_804901F0[keyid] = key;
}
