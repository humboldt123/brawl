#include <cstring>
#include <gf/gf_crypto_aes.h>
#include <gf/gf_heap_manager.h>
#include <revolution/OS/OSHardware.h>
#include <revolution/OS/OSThread.h>
#include <revolution/OS/OSTime.h>
#include <types.h>

// HYPOTHESIS: the NETAESContext layout is not recovered; the size covers the original stack frame.
struct NETAESContext {
    u8 unk0[0x120];
};

extern "C" {
// HYPOTHESIS: NETLockCrypto / NETUnlockCrypto (crypto.o), not named yet.
BOOL fn_8038F430();
void fn_8038F4C0();
// HYPOTHESIS: NETAESCreate / NETAESDelete / NETAESEncrypt / NETAESDecrypt (aes.o), not named yet.
BOOL fn_8038FD64(NETAESContext* ctx, const void* key, u32 keySize, const void* iv);
void fn_8038FD6C(NETAESContext* ctx);
BOOL fn_8038FD70(NETAESContext* ctx, void* dst, const void* src, u32 size);
BOOL fn_8038FE28(NETAESContext* ctx, void* dst, const void* src, u32 size);
}

static u8 s_iv[0x10] = {
    0x4E, 0x03, 0x41, 0xDE, 0xE6, 0xBB, 0xAA, 0x41, 0x64, 0x19, 0xB3, 0xEA, 0xE8, 0xF5, 0x3B, 0xD9,
};
static u8 s_key[0x10] = {
    0x92, 0x65, 0x47, 0x1A, 0x9C, 0xBF, 0x3D, 0x56, 0x8A, 0x13, 0xD3, 0xC4, 0x81, 0x53, 0x2C, 0x18,
};

gfCryptoAES::gfCryptoAES() : gfThread(this, 0xC, 0x1000, Heaps::Thread) {
    memcpy(m_iv, s_iv, sizeof(m_iv));
    memcpy(m_key, s_key, sizeof(m_key));
    m_isDone = false;
    m_result = 0;
    m_dst = NULL;
    m_freeDst = false;
}

gfCryptoAES::~gfCryptoAES() {
    if (!m_isDone) {
        OSResumeThread(&m_thread);
    }
    if (m_freeDst && m_dst) {
        gfHeapManager::free(m_dst);
        m_dst = NULL;
    }
}

bool gfCryptoAES::encrypt(const void* src, u32 size, Heaps::HeapType heap) {
    if (m_isDone) {
        return false;
    }
    if (!src) {
        return false;
    }
    if ((u32)src & 0x1F) {
        return false;
    }
    m_src = src;
    m_srcSize = size;
    m_heap = heap;
    m_mode = 1;
    OSResumeThread(&m_thread);
    return true;
}

bool gfCryptoAES::decrypt(const void* src, u32 size, Heaps::HeapType heap) {
    if (m_isDone) {
        return false;
    }
    if (!src) {
        return false;
    }
    if (size < 0x10 || (size & 0xF)) {
        return false;
    }
    if ((u32)src & 0x1F) {
        return false;
    }
    m_src = src;
    m_srcSize = size;
    m_heap = heap;
    m_mode = 2;
    OSResumeThread(&m_thread);
    return true;
}

void gfCryptoAES::run() {
    if (m_mode == 0) {
        return;
    }
    m_result = -1;
    m_dstSize = m_srcSize;
    if (m_mode == 1) {
        m_dstSize = (m_srcSize + 0xF) & ~0xF;
    }
    m_dst = gfHeapManager::alloc(m_heap, m_dstSize, 0x20);
    if (m_dst) {
        while (!fn_8038F430()) {
            OSSleepTicks(OS_MSEC_TO_TICKS(1));
        }
        NETAESContext ctx __attribute__((aligned(32)));
        if (fn_8038FD64(&ctx, m_key, sizeof(m_key), m_iv)) {
            if (m_mode == 1) {
                u32 rest = m_srcSize & 0x1F;
                u32 body = m_srcSize - rest;
                if (body != 0) {
                    if (!fn_8038FD70(&ctx, m_dst, m_src, body)) {
                        goto finish;
                    }
                }
                if (rest != 0) {
                    u8 tail[0x20] __attribute__((aligned(32)));
                    memset(tail, 0, sizeof(tail));
                    memcpy(tail, (const u8*)m_src + body, rest);
                    if (!fn_8038FD70(&ctx, (u8*)m_dst + body, tail, rest <= 0x10 ? 0x10 : 0x20)) {
                        goto finish;
                    }
                }
                m_result = 0;
            } else if (fn_8038FE28(&ctx, m_dst, m_src, m_dstSize)) {
                m_result = 0;
            }
            fn_8038FD6C(&ctx);
        }
        fn_8038F4C0();
    } else {
        m_result = -2;
    }
finish:
    if (m_result != 0 && m_dst) {
        gfHeapManager::free(m_dst);
        m_dst = NULL;
    }
    m_isDone = true;
}
