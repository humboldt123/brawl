#include <types.h>
#include <string.h>
#include <GameSpy/serverbrowsing/sb_internal.h>

// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
HashTable lbl_805A1120 = NULL;
unsigned char lbl_805A1124[0x4] __attribute__((aligned(4))) = { 0 };
SBServer lbl_805A1128 = NULL;  // HYPOTHESIS: SBNullServer
unsigned char lbl_805A1130[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A1134[0x4] __attribute__((aligned(4))) = { 0 };

// Function bodies below are derived from https://github.com/jj-link/brawl, where each
// one was matched in a translation unit of its own. HYPOTHESIS: this unit spans
// several original source files, so its functions are not auto-inlined into one
// another; helpers the original inlined are marked inline.
// Checked: without this pragma 1 function in this unit stop matching.
#pragma auto_inline off

u8 fn_80387608(u8 *arg0, u32 arg1, u8 *arg2, u32 arg3, u8 *arg4, u32 *arg5) {
    u32 count;
    u32 mask;
    u32 v;

    if (arg1 == 0) {
        return 0;
    }

    count = 0;
    mask = 1;
    while (mask < arg1) {
        mask = (mask << 1) + 1;
    }

    do {
        *arg4 = arg0[*arg4] + arg2[(*arg5)++];
        if (*arg5 >= arg3) {
            *arg5 = 0;
            *arg4 = *arg4 + arg3;
        }

        count++;
        v = mask & *arg4;
        if (count > 11) {
            v %= arg1;
        }
    } while (v > arg1);

    return (u8)v;
}

extern void fn_80389610(int, int, int, int, int, int, int, int);

void fn_80389754(int a1, int a2, int a3, int a4, int a5, int a6, int a7) {
    fn_80389610(a1, a2, a3, a4, a5, a6, 0x80, a7);
}

s32 fn_8038A298(const u8 *arg0, s32 arg1) {
    s32 i = 0;
    while (arg1 > 0) {
        if ((s8)*arg0 == 0)
            return i + 1;
        i++;
        arg0++;
        arg1--;
    }
    return -1;
}

// ---------------------------------------------------------------- sb_queryengine

void fn_80387D30(SBQueryEngine* engine, int maxupdates, int queryversion, int lanBrowse, SBEngineCallbackFn callback, void* instance) {
    if (lanBrowse || lbl_805A1048 == 1) {
        fn_8035D720();
        engine->queryversion = queryversion;
        engine->maxupdates = maxupdates;
        engine->numserverkeys = 0;
        engine->ListCallback = callback;
        engine->instance = instance;
        engine->mypublicip = 0;
        engine->querysock = socket(2, 2, 0x11);
        engine->pendinglist.last = NULL;
        engine->pendinglist.first = NULL;
        engine->pendinglist.count = 0;
        engine->querylist.last = NULL;
        engine->querylist.first = NULL;
        engine->querylist.count = 0;
    }
}

void fn_80387DD8(SBQueryEngine* engine) {
    engine->pendinglist.last = NULL;
    engine->pendinglist.first = NULL;
    engine->pendinglist.count = 0;
    engine->querylist.last = NULL;
    engine->querylist.first = NULL;
    engine->querylist.count = 0;
}

void fn_80387DF8(SBQueryEngine* engine) {
    closesocket(engine->querysock);
    engine->querysock = -1;
    engine->pendinglist.last = NULL;
    engine->pendinglist.first = NULL;
    engine->pendinglist.count = 0;
    engine->querylist.last = NULL;
    engine->querylist.first = NULL;
    engine->querylist.count = 0;
}

void fn_80387E4C(SBQueryEngine* engine, SBServer server, int addfront, int querytype, int usequerychallenge) {
    server->state &= 0x43;
    server->splitResponseBitmap = 0;
    server->querychallenge = 0;
    if (querytype == 2) {
        return;
    }
    if (querytype == 0) {
        server->state |= 4;
    } else if (querytype == 1) {
        server->state |= 8;
    } else if (querytype == 2) {
        server->state |= 0x20;
    } else {
        return;
    }
    if (usequerychallenge && (u32)querytype <= 1) {
        server->state |= 0x80;
    }
    if (engine->querylist.count < engine->maxupdates) {
        fn_80387AB0(engine, server);
    } else if (addfront) {
        server->next = engine->pendinglist.first;
        engine->pendinglist.first = server;
        if (!engine->pendinglist.last) {
            engine->pendinglist.last = server;
        }
        engine->pendinglist.count++;
    } else {
        if (engine->pendinglist.last) {
            engine->pendinglist.last->next = server;
        }
        engine->pendinglist.last = server;
        server->next = NULL;
        if (!engine->pendinglist.first) {
            engine->pendinglist.first = server;
        }
        engine->pendinglist.count++;
    }
}

static inline SBServer FIFOGetFirst(SBServerFIFO* fifo) {
    SBServer hold = fifo->first;

    if (hold != NULL) {
        fifo->first = hold->next;
        if (fifo->first == NULL) {
            fifo->last = NULL;
        }
        fifo->count--;
    }
    return hold;
}

void fn_8038831C(SBQueryEngine* engine) {
    u32 ctime;
    SBServer server;

    if (engine->querylist.count == 0) {
        return;
    }
    fn_80388120(engine, 0);
    ctime = fn_8035D728();
    while (engine->querylist.first != NULL) {
        if (ctime <= engine->querylist.first->updatetime + 2500) {
            break;
        }
        engine->querylist.first->flags |= 0x10;
        engine->querylist.first->updatetime = 2500;
        engine->querylist.first->flags &= 0xD3;
        engine->ListCallback(engine, 1, engine->querylist.first, engine->instance);
        FIFOGetFirst(&engine->querylist);
    }
    if (engine->pendinglist.count > 0) {
        while (engine->querylist.count < engine->maxupdates && engine->pendinglist.count > 0) {
            server = FIFOGetFirst(&engine->pendinglist);
            fn_80387AB0(engine, server);
        }
    }
    if (engine->querylist.count == 0) {
        engine->ListCallback(engine, 2, NULL, engine->instance);
    }
}

void fn_80388498(SBQueryEngine* engine, u8 keyid) {
    if (engine->numserverkeys >= MAX_QUERY_KEYS) {
        return;
    }
    engine->serverkeys[engine->numserverkeys++] = keyid;
}

void fn_803884B8(SBQueryEngine* engine, SBServer server) {
    if (fn_80387A3C(&engine->querylist, server) == 0) {
        fn_80387A3C(&engine->pendinglist, server);
    }
}

// ---------------------------------------------------------------- sb_server

int fn_8038857C(const void* elem1, const void* elem2) {
    return strcasecmp(((const SBRefString*)elem1)->str, ((const SBRefString*)elem2)->str);
}

HashTable fn_80388590(SBServerList* slist) {
    if (lbl_805A1120 == NULL) {
        lbl_805A1120 = fn_8035AED0(sizeof(SBRefString), 500, 4, fn_80388508, fn_8038857C, fn_80388588);
    }
    return lbl_805A1120;
}

void fn_803885E8(SBServerList* slist) {
    if (lbl_805A1120 != NULL && fn_8035AFF0(lbl_805A1120) == 0) {
        fn_8035AF74(lbl_805A1120);
        lbl_805A1120 = NULL;
    }
}

void fn_8038862C(void* elem) {
    SBServer server = *(SBServer*)elem;

    fn_8035AF74(server->keyvals);
    server->keyvals = NULL;
    fn_8035E00C(server);
}

char* fn_8038892C(SBServer server) {
    gs_in_addr addr;

    addr.s_addr = server->publicip;
    return SOInetNtoA(addr);
}

u16 fn_80388960(SBServer server) {
    return SONtoHs(server->publicport);
}

int fn_80388970(SBServer server) {
    return (server->flags & 2) == 2;
}

char* fn_8038897C(SBServer server) {
    gs_in_addr addr;

    addr.s_addr = server->privateip;
    return SOInetNtoA(addr);
}

u16 fn_803889B0(SBServer server) {
    return SONtoHs(server->privateport);
}

const char* fn_8038A1B0(SBServerList* slist, const char* str) {
    SBRefString ref;
    SBRefString* val;

    ref.str = str;
    val = fn_8035B1C4(fn_80388590(slist), &ref);
    if (val != NULL) {
        val->refcount++;
        return val->str;
    }
    ref.str = fn_8035D654(str);
    ref.refcount = 1;
    fn_8035B078(fn_80388590(slist), &ref);
    return ref.str;
}

void fn_80388FD4(void* elem) {
    SBKeyValuePair* kv = elem;

    fn_8038A238(NULL, kv->key);
    fn_8038A238(NULL, kv->value);
}

int fn_80389088(const void* elem1, const void* elem2) {
    const SBKeyValuePair* kv1 = elem1;
    const SBKeyValuePair* kv2 = elem2;

    if (kv1->key == NULL || kv2->key == NULL) {
        return 1;
    }
    return strcasecmp(kv1->key, kv2->key);
}

void fn_80389180(SBServer server, u32 ip, u16 port) {
    server->privateip = ip;
    server->privateport = port;
}

int fn_803891A4(SBServer server) {
    return server == lbl_805A1128;
}

void fn_8038D0A8(void* sb, u32 ip, u16 port, int cookie) {
    u8 buffer[10];

    buffer[0] = 0xFD;
    buffer[1] = 0xFC;
    buffer[2] = 0x1E;
    buffer[3] = 0x66;
    buffer[4] = 0x6A;
    buffer[5] = 0xB2;
    cookie = SOHtoNl(cookie);
    memcpy(buffer + 6, &cookie, 4);
    fn_8038CFA4(sb, ip, port, buffer, 10);
}
