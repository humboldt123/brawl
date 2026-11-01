#include <ut/ut_bit_array.h>
#include <gf/gf_file_io_manager.h>
#include <gf/gf_file_io_request.h>
#include <revolution/OS/OSMutex.h>
#include <types.h>
#include <new>

void gfFileIOManager::push(gfFileIORequest* req) {
    m_queue1->push(req);
}

gfFileIORequest* gfFileIOManager::allocRequest() {
    gfFileIORequestPool* pool = m_requestPool;
    gfFileIORequest* req;
    if (pool->m_usedBits->hasUnsetBit()) {
        s32 idx = pool->m_usedBits->getUnsetBit();
        pool->m_usedBits->m_bits[idx >> 5] |= 1 << (idx & 0x1F);
        req = &pool->m_requests[idx];
    } else {
        req = NULL;
    }
    gfFileIORequest* result = NULL;
    if (req != NULL) {
        result = new (req) gfFileIORequest;
    }
    return result;
}

void gfFileIOManager::setSyncMode(bool enable) {
    unk98 = enable;
    unk16 = 0;
    if (!unk98) {
        OSLockMutex(&m_queue1->m_mtx);
        u16 count1 = m_queue1->m_queue->getCount();
        for (u32 i = 0; i < count1; i++) {
            gfFileIORequest* req = m_queue1->m_queue->get(i);
            if (req->m_kind == gfFileIORequest::Read) {
                req->unk17_b1 = 0;
            }
        }
        OSUnlockMutex(&m_queue1->m_mtx);
        OSLockMutex(&m_queue2->m_mtx);
        u16 count2 = m_queue2->m_queue->getCount();
        for (u32 j = 0; j < count2; j++) {
            gfFileIORequest* req = m_queue2->m_queue->get(j);
            if (req->m_kind == gfFileIORequest::Read) {
                req->unk17_b1 = 0;
            }
        }
        OSUnlockMutex(&m_queue2->m_mtx);
    }
}
