// Local BrawlHeaders shadow: declare the verified replay synchronization API.
#pragma once

#include <StaticAssert.h>
#include <gf/gf_thread.h>
#include <gf/gf_file_io_request.h>
#include <types.h>

class utBitArray;

// The fixed set of requests the manager hands out: a bit set of the ones in use and the request storage.
// HYPOTHESIS: names.
struct gfFileIORequestPool {
    s32 unk0;
    utBitArray* m_usedBits;
    gfFileIORequest* m_requests;
};

class gfFileIOManager {
public:
    char _0x0[4];
    gfThread* m_thread;
    gfFileIORequestQueue* m_queue1;
    gfFileIORequestQueue* m_queue2;
    gfFileIORequestPool* m_requestPool; // 0x10
    char _0x14[2];
    u16 unk16;
    char _0x18[0x80];
    bool unk98 : 1;
    u8 unk99[0x3];
public:
    u32 getNumActiveRequests() { return m_queue1->getSize(); };

    void setSyncMode(bool enable);

    gfFileIORequest* allocRequest();
    bool freeRequest(gfFileIORequest* req);
    void push(gfFileIORequest* req);
};
static_assert(sizeof(gfFileIOManager) == 0x9c, "Wrong size for class!");

extern gfFileIOManager* g_gfFileIOManager;
