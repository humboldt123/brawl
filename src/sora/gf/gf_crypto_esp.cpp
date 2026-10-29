#include <cstring>
#include <gf/gf_crypto_esp.h>
#include <gf/gf_heap_manager.h>
#include <revolution/OS/OSMutex.h>
#include <revolution/OS/OSThread.h>
#include <types.h>

extern "C" {
s32 ESP_InitLib(void);
s32 ESP_CloseLib(void);
s32 ESP_Encrypt(u32 keyHandle, u8* iv, u8* input, u32 inputSize, u8* output);
s32 ESP_Decrypt(u32 keyHandle, u8* iv, u8* input, u32 inputSize, u8* output);
}

static u8 s_iv[0x10] = {
    0x4E, 0x03, 0x41, 0xDE, 0xE6, 0xBB, 0xAA, 0x41, 0x64, 0x19, 0xB3, 0xEA, 0xE8, 0xF5, 0x3B, 0xD9,
};
static OSMutex* s_mutex;

gfCryptoESP::gfCryptoESP() : gfThread(this, 0xC, 0x1000, Heaps::Thread) {
    memcpy(m_iv, s_iv, sizeof(m_iv));
    m_isDone = false;
    m_result = 0;
    m_dst = NULL;
    m_freeDst = false;
    m_mode = 0;
}

gfCryptoESP::~gfCryptoESP() {
    if (!m_isDone) {
        OSResumeThread(&m_thread);
    }
    if (m_freeDst && m_dst) {
        gfHeapManager::free(m_dst);
        m_dst = NULL;
    }
}

bool gfCryptoESP::encrypt(const void* src, u32 size, Heaps::HeapType heap) {
    if (m_isDone) {
        return false;
    }
    if (!src) {
        return false;
    }
    m_src = src;
    m_srcSize = size;
    m_heap = heap;
    m_mode = 1;
    OSResumeThread(&m_thread);
    return true;
}

bool gfCryptoESP::decrypt(const void* src, u32 size, Heaps::HeapType heap) {
    if (m_isDone) {
        return false;
    }
    if (!src) {
        return false;
    }
    if (size < 0x10 || (size & 0xF)) {
        return false;
    }
    m_src = src;
    m_srcSize = size;
    m_heap = heap;
    m_mode = 2;
    OSResumeThread(&m_thread);
    return true;
}

void gfCryptoESP::run() {
    if (m_mode == 0) {
        return;
    }
    m_result = -1;
    m_dstSize = m_srcSize;
    if (m_mode == 1) {
        u32 rest = m_srcSize & 0xF;
        if (rest) {
            m_dstSize = m_srcSize + (0x10 - rest);
        }
    }
    u8* work = (u8*)gfHeapManager::alloc(m_heap, 0x4000, 0x40);
    m_dst = gfHeapManager::alloc(m_heap, m_dstSize, 0x40);
    if (work && m_dst) {
        if (!s_mutex) {
            OSMutex* mutex = new (Heaps::System) OSMutex;
            if (mutex) {
                OSInitMutex(mutex);
            }
            s_mutex = mutex;
        }
        OSLockMutex(s_mutex);
        if (ESP_InitLib() == 0) {
            bool failed = false;
            u32 count = m_dstSize >> 14;
            if (m_dstSize & 0x3FFF) {
                count++;
            }
            for (u32 i = 0; i < count; i++) {
                u32 offset = i * 0x4000;
                u32 len = 0x4000;
                if (((i + 1) << 14) > m_dstSize) {
                    len = m_dstSize - offset;
                    memset(work, 0, 0x4000);
                    memcpy(work, (const u8*)m_src + offset, m_srcSize - offset);
                } else {
                    memcpy(work, (const u8*)m_src + offset, 0x4000);
                }
                if (m_mode == 1) {
                    if (ESP_Encrypt(6, m_iv, work, len, (u8*)m_dst + offset) != 0) {
                        failed = true;
                        break;
                    }
                } else {
                    if (ESP_Decrypt(6, m_iv, work, len, (u8*)m_dst + offset) != 0) {
                        failed = true;
                        break;
                    }
                }
            }
            if (!failed) {
                m_result = 0;
            }
            ESP_CloseLib();
        }
        OSUnlockMutex(s_mutex);
    } else {
        m_result = -2;
    }
    if (m_result != 0 && m_dst) {
        gfHeapManager::free(m_dst);
        m_dst = NULL;
    }
    if (work) {
        gfHeapManager::free(work);
    }
    m_isDone = true;
}
