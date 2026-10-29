#include <ut/ut_bit_array.h>
#include <gf/gf_file_io_manager.h>
#include <gf/gf_file_io_request.h>
#include <revolution/OS/OSMutex.h>

#include <gf/gf_file_io_manager.h>
#include <gf/gf_file_io_request.h>
#include <types.h>

struct gfFileIOSlots {
    u16 m_size;
    u16 unk2;
    gfFileIORequest* m_elements[128];
};

// MATCH-ONLY: gfFileIORequestQueue::m_queue and utQueue::unk2 are private in the headers, so this view exposes them.
struct gfFileIORequestQueueView {
    OSMutex mutex;
    gfFileIOSlots* list;
};

void gfFileIOManager::push(gfFileIORequest* req) {
    gfFileIORequestQueueView* queue = (gfFileIORequestQueueView*) m_queue1;
    OSLockMutex(&queue->mutex);
    u16 idx;
    gfFileIOSlots* list = queue->list;
    idx = list->unk2;
    list->unk2 = idx + 1;
    list->m_elements[idx] = req;
    OSUnlockMutex(&queue->mutex);
}


struct gfFileIORequestPoolView {
    s32 unk0;
    u32* usedBits;
    gfFileIORequest* requests;
};

// MATCH-ONLY: gfFileIOManager leaves offset 0x10 as an unnamed char array in the header, so this view exposes the pool pointer.
struct gfFileIOManagerView {
    char pad0[0x10];
    gfFileIORequestPoolView* pool;
};


gfFileIORequest* gfFileIOManager::allocRequest() {
    gfFileIOManagerView* self = (gfFileIOManagerView*) this;
    gfFileIORequestPoolView* pool = self->pool;
    gfFileIORequest* req;
    if (reinterpret_cast<utBitArray*>(pool->usedBits)->hasUnsetBit()) {
        s32 idx = reinterpret_cast<utBitArray*>(pool->usedBits)->getUnsetBit();
        // HYPOTHESIS: usedBits points at a header word, so the bit words start 4 bytes in.
        pool->usedBits[(idx >> 5) + 1] |= 1 << (idx & 0x1F);
        req = (gfFileIORequest*) ((u8*) pool->requests + idx * 0xA4);
    } else {
        req = NULL;
    }
    gfFileIORequest* result = NULL;
    if (req != NULL) {
        // MATCH-ONLY: the second null check is kept so the compiler emits the same branch pair.
        if (req != NULL) {
            req->m_isCached = false;
            req->unk16_b7 = 0;
            req->unk17_b6 = 0;
            req->unk17_b5 = 0;
            req->m_isCancelRequested = false;
            req->m_isCanceled = false;
            req->unk17_b2 = 0;
            req->unk17_b1 = 0;
            req->unk17_b0 = 0;
            req->unk18_b7 = 0;
            req->unk18_b6 = 0;
            req->m_isReady = 1;
            req->m_notifyCallback = NULL;
        }
        result = req;
    }
    return result;
}

// MATCH-ONLY: the header leaves offset 0x16 inside an unnamed char array, so this view exposes the halfword that is cleared.
struct gfFileIOManagerSyncView {
    char pad0[0x16];
    u16 unk16;
};

void gfFileIOManager::setSyncMode(bool enable) {
    gfFileIOManagerSyncView* self = (gfFileIOManagerSyncView*) this;
    unk98 = enable;
    self->unk16 = 0;
    if (!unk98) {
        OSLockMutex(&((gfFileIORequestQueueView*) m_queue1)->mutex);
        u16 count1 = ((gfFileIORequestQueueView*) m_queue1)->list->unk2;
        for (u32 i = 0; i < count1; i++) {
            gfFileIORequest* req = ((gfFileIORequestQueueView*) m_queue1)->list->m_elements[i];
            if (req->m_kind == gfFileIORequest::Read) {
                req->unk17_b1 = 0;
            }
        }
        OSUnlockMutex(&((gfFileIORequestQueueView*) m_queue1)->mutex);
        OSLockMutex(&((gfFileIORequestQueueView*) m_queue2)->mutex);
        u16 count2 = ((gfFileIORequestQueueView*) m_queue2)->list->unk2;
        for (u32 j = 0; j < count2; j++) {
            gfFileIORequest* req = ((gfFileIORequestQueueView*) m_queue2)->list->m_elements[j];
            if (req->m_kind == gfFileIORequest::Read) {
                req->unk17_b1 = 0;
            }
        }
        OSUnlockMutex(&((gfFileIORequestQueueView*) m_queue2)->mutex);
    }
}
