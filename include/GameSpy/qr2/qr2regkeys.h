#ifndef GAMESPY_QR2_QR2REGKEYS_H
#define GAMESPY_QR2_QR2REGKEYS_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define NUM_RESERVED_KEYS 50
#define MAX_REGISTERED_KEYS 254

// qr2_registered_key_list: the key name of every registered key id
extern const char* lbl_804901F0[MAX_REGISTERED_KEYS];

void fn_8037B354(int keyid, const char* key);  // qr2_register_key

#ifdef __cplusplus
}
#endif

#endif
