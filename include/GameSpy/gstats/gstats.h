#ifndef GAMESPY_GSTATS_GSTATS_H
#define GAMESPY_GSTATS_GSTATS_H

// GameSpy SDK stats/persistence (gstats.c, gbucket.c). Only what the matched
// functions touch is declared; the functions are unnamed in our symbols, so
// the SDK names are noted next to the placeholders.
#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct bucketset_s* bucketset_t;

typedef enum bucketType {
    bt_int,
    bt_float,
    bt_string
} bucketType;

// HYPOTHESIS: the game record; only the bucket set at 0xC is evidenced.
typedef struct statsgame_s {
    u32 unk0;
    u32 unk4;
    u32 unk8;
    bucketset_t buckets;  // 0xC
} statsgame_s;
typedef statsgame_s* statsgame_t;

typedef void* (*bucketfunc_t)(bucketset_t set, char* name, void* value);

// the stats server socket (INVALID_SOCKET while disconnected)
extern int lbl_8059FAF0;
// the current game, used when no game is passed
extern statsgame_t lbl_805A10B0;

void* fn_803822D8(bucketset_t set, char* name, bucketType type, void* initialValue);  // BucketNew
int fn_80383698(void);                                                               // IsStatsConnected
int fn_80384E70(statsgame_t game, char* name, bucketfunc_t func, int value);        // BucketIntOp
double fn_80384EFC(statsgame_t game, char* name, bucketfunc_t func, double value);  // BucketFloatOp
char* fn_80384F88(statsgame_t game, char* name, bucketfunc_t func, char* value);    // BucketStringOp

#ifdef __cplusplus
}
#endif

#endif
