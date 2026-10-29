#ifndef GAMESPY_QR2_QR2_H
#define GAMESPY_QR2_QR2_H

// GameSpy SDK query & reporting 2 (qr2.c). Field order follows the SDK's
// qr2_implementation_s up to the fields the matched code touches. The
// functions are unnamed in our symbols; the SDK names are noted next to them.
#include <types.h>
#include <GameSpy/qr2/qr2regkeys.h>
#include <GameSpy/common/gsPlatform.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_DATA_SIZE 1400
#define REQUEST_KEY_LEN 4

typedef void (*qr2_callback_t)(void);

typedef struct qr2_implementation_s {
    int hbsock;                               // 0x0
    char gamename[64];                        // 0x4
    char secret_key[64];                      // 0x44
    char instance_key[REQUEST_KEY_LEN];       // 0x84
    qr2_callback_t server_key_callback;       // 0x88
    qr2_callback_t player_key_callback;       // 0x8C
    qr2_callback_t team_key_callback;         // 0x90
    qr2_callback_t key_list_callback;         // 0x94
    qr2_callback_t playerteam_count_callback; // 0x98
    qr2_callback_t adderror_callback;         // 0x9C
    qr2_callback_t nn_callback;               // 0xA0
    qr2_callback_t cm_callback;               // 0xA4
    qr2_callback_t pa_callback;               // 0xA8
    u32 lastheartbeat;                        // 0xAC
    u32 lastka;                               // 0xB0
    int userstatechangerequested;             // 0xB4
    int listed_state;                         // 0xB8
    int ispublic;                             // 0xBC
    int qport;                                // 0xC0
    int read_socket;                          // 0xC4
} qr2_implementation_s;
typedef qr2_implementation_s* qr2_t;

typedef struct qr2_buffer_s {
    char buffer[MAX_DATA_SIZE];   // 0x0
    int len;                      // 0x578
} qr2_buffer_s;
typedef qr2_buffer_s* qr2_buffer_t;

typedef struct qr2_keybuffer_s {
    u8 keys[MAX_REGISTERED_KEYS]; // 0x0
    int numkeys;                  // 0x100
} qr2_keybuffer_s;
typedef qr2_keybuffer_s* qr2_keybuffer_t;

__static_assert(sizeof(qr2_buffer_s) == 0x57C, "qr2_buffer_s");
__static_assert(sizeof(qr2_keybuffer_s) == 0x104, "qr2_keybuffer_s");

// the current record (used when none is passed) and the static record
extern qr2_t lbl_8059F9A8;
extern qr2_implementation_s lbl_8048FBE0;
// "%d"
extern char lbl_8059F9AC[3];

void fn_80378E24(qr2_t qrec, qr2_callback_t nncallback);  // qr2_register_natneg_callback
void fn_80378E38(qr2_t qrec, qr2_callback_t cmcallback);  // qr2_register_clientmessage_callback
void fn_80378E4C(qr2_t qrec, qr2_callback_t pacallback);  // qr2_register_publicaddress_callback
void fn_803791BC(qr2_t qrec);                             // qr2_shutdown
int fn_8037925C(qr2_keybuffer_t keybuffer, int keyid);    // qr2_keybuffer_add
int fn_8037929C(qr2_buffer_t outbuf, int value);          // qr2_buffer_add_int
int fn_8037933C(qr2_buffer_t outbuf, const char* value);  // qr2_buffer_add
void fn_8037AC14(qr2_t qrec, int statechanged);           // send_heartbeat

#ifdef __cplusplus
}
#endif

#endif
